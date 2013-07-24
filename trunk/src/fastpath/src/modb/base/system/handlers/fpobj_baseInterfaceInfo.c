/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_baseInterfaceInfo.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to sntp-object.xml
*
* @create  14 January 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_baseInterfaceInfo_obj.h"
#include "usmdb_nim_api.h"
#include "usmdb_2233_stats_api.h"
#include "usmdb_util_api.h"
#include "usmdb_1213_api.h"
#include "usmdb_trapmgr_api.h"
#include "xlib_private.h"
#include "usmdb_mib_rmon_api.h"
/*******************************************************************************
* @function fpObjGet_baseInterfaceInfo_ifIndex
*
* @purpose Get 'ifIndex'
*
* @description  A unique value, greater than zero, for each interface. It is 
*              recommended that values are assigned contiguously starting from 
*              1. The value for each interface sub-layer must remain constant 
*              at least from one re-initial 
*              
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseInterfaceInfo_ifIndex (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objifIndexValue;
  xLibU32_t nextObjifIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_baseInterfaceInfo_ifIndex,
                          (xLibU8_t *) & objifIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbIntIfNumTypeFirstGet(L7_UNIT_CURRENT, USM_PHYSICAL_INTF | USM_LAG_INTF,
                                 0, &nextObjifIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objifIndexValue, owa.len);
	 owa.l7rc = usmDbIntIfNumTypeNextGet(L7_UNIT_CURRENT, USM_PHYSICAL_INTF | USM_LAG_INTF,
                                               0, objifIndexValue, &nextObjifIndexValue);
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjifIndexValue, owa.len);

  /* return the object value: ifIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjifIndexValue,
                           sizeof (objifIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseInterfaceInfo_ifDescr
*
* @purpose Get 'ifDescr'
*
* @description  A textual string containing information about the interface. 
*              This string should include the name of the manufacturer, the 
*              product name and the version of the interface hardware/software. 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseInterfaceInfo_ifDescr (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objifDescrValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseInterfaceInfo_ifIndex,
                          (xLibU8_t *) & keyifIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIfDescrGet (L7_UNIT_CURRENT, keyifIndexValue,
                              objifDescrValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ifDescr */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objifDescrValue,
                           strlen (objifDescrValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseInterfaceInfo_ifType
*
* @purpose Get 'ifType'
*
* @description  The type of interface. Additional values for ifType are assigned 
*              by the Internet Assigned Numbers Authority (IANA), through 
*              updating the syntax of the IANAifType textual convention. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseInterfaceInfo_ifType (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objifTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseInterfaceInfo_ifIndex,
                          (xLibU8_t *) & keyifIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIfTypeGet (L7_UNIT_CURRENT, keyifIndexValue, &objifTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ifType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objifTypeValue,
                           sizeof (objifTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseInterfaceInfo_ifIndexNumber
*
* @purpose Get 'ifIndexNumber'
*
* @description [ifIndexNumber]  The internal interface number.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseInterfaceInfo_ifIndexNumber (void *wap, void *bufp)
{

  fpObjWa_t kwaifIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  kwaifIndex.rc = xLibFilterGet (wap, XOBJ_baseInterfaceInfo_ifIndex,
                                 (xLibU8_t *) & keyifIndexValue, &kwaifIndex.len);
  if (kwaifIndex.rc != XLIBRC_SUCCESS)
  {
    kwaifIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaifIndex);
    return kwaifIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwaifIndex.len);

  /* get the value from application */

  /* return the object value: ifIndexNumber */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & keyifIndexValue,
                           sizeof (keyifIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseInterfaceInfo_configMaskOffset
*
* @purpose Get 'configMaskOffset'
*
* @description [configMaskOffset] the Configuration Mask Offset
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseInterfaceInfo_configMaskOffset (void *wap, void *bufp)
{

  fpObjWa_t kwaifIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objconfigMaskOffsetValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  kwaifIndex.rc = xLibFilterGet (wap, XOBJ_baseInterfaceInfo_ifIndex,
                                 (xLibU8_t *) & keyifIndexValue, &kwaifIndex.len);
  if (kwaifIndex.rc != XLIBRC_SUCCESS)
  {
    kwaifIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaifIndex);
    return kwaifIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwaifIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbConfigIdMaskOffsetGet ( keyifIndexValue, &objconfigMaskOffsetValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: configMaskOffset */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objconfigMaskOffsetValue,
                           sizeof (objconfigMaskOffsetValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseInterfaceInfo_ifMtu
*
* @purpose Get 'ifMtu'
*
* @description  The size of the largest packet which can be sent/received on 
*              the interface, specified in octets. For interfaces that are used 
*              for transmitting network datagrams, this is the size of the 
*              largest network datagram that c 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseInterfaceInfo_ifMtu (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objifMtuValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseInterfaceInfo_ifIndex,
                          (xLibU8_t *) & keyifIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIfMtuGet (L7_UNIT_CURRENT, keyifIndexValue, &objifMtuValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ifMtu */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objifMtuValue,
                           sizeof (objifMtuValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseInterfaceInfo_ifSpeed
*
* @purpose Get 'ifSpeed'
*
* @description  An estimate of the interface's current bandwidth in bits per 
*              second. For interfaces which do not vary in bandwidth or for 
*              those where no accurate estimation can be made, this object should 
*              contain the nominal bandwidt 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseInterfaceInfo_ifSpeed (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objifSpeedValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
 kwa.rc = xLibFilterGet (wap, XOBJ_baseInterfaceInfo_ifIndex,
                          (xLibU8_t *) & keyifIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIfSpeedStatusGet (L7_UNIT_CURRENT, keyifIndexValue,
                                    &objifSpeedValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ifSpeed */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objifSpeedValue,
                           sizeof (objifSpeedValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseInterfaceInfo_ifPhysAddress
*
* @purpose Get 'ifPhysAddress'
*
* @description  The interface's address at its protocol sub-layer. For example, 
*              for an 802.x interface, this object normally contains a MAC 
*              address. The interface's media-specific MIB must define the 
*              bit and byte ordering and the fo 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseInterfaceInfo_ifPhysAddress (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  xLibStr6_t objifPhysAddressValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseInterfaceInfo_ifIndex,
                          (xLibU8_t *) & keyifIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIfPhysAddressGet (L7_UNIT_CURRENT, keyifIndexValue,
                                    objifPhysAddressValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ifPhysAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objifPhysAddressValue,
                             sizeof (xLibStr6_t));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseInterfaceInfo_ifAdminStatus
*
* @purpose Get 'ifAdminStatus'
*
* @description  The desired state of the interface. The testing(3) state indicates 
*              that no operational packets can be passed. When a managed 
*              system initializes, all interfaces start with ifAdminStatus 
*              in the down(2) state. As a res 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseInterfaceInfo_ifAdminStatus (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objifAdminStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseInterfaceInfo_ifIndex,
                          (xLibU8_t *) & keyifIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIfAdminStateGet (L7_UNIT_CURRENT, keyifIndexValue,
                                   &objifAdminStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ifAdminStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objifAdminStatusValue,
                           sizeof (objifAdminStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseInterfaceInfo_ifAdminStatus
*
* @purpose Set 'ifAdminStatus'
*
* @description  The desired state of the interface. The testing(3) state indicates 
*              that no operational packets can be passed. When a managed 
*              system initializes, all interfaces start with ifAdminStatus 
*              in the down(2) state. As a res 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseInterfaceInfo_ifAdminStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objifAdminStatusValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ifAdminStatus */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objifAdminStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objifAdminStatusValue, owa.len);

  /* retrieve key: ifIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseInterfaceInfo_ifIndex,
                          (xLibU8_t *) & keyifIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbIfAdminStateSet (L7_UNIT_CURRENT, keyifIndexValue,
                                   objifAdminStatusValue);
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
* @function fpObjGet_baseInterfaceInfo_ifOperStatus
*
* @purpose Get 'ifOperStatus'
*
* @description  The current operational state of the interface. The testing(3) 
*              state indicates that no operational packets can be passed. 
*              If ifAdminStatus is down(2) then ifOperStatus should be down(2). 
*              If ifAdminStatus is changed 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseInterfaceInfo_ifOperStatus (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objifOperStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseInterfaceInfo_ifIndex,
                          (xLibU8_t *) & keyifIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIfOperStatusGet(L7_UNIT_CURRENT, keyifIndexValue,
                                 &objifOperStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  if (objifOperStatusValue == L7_UP)
  {
    objifOperStatusValue = L7_XUI_LINK_UP;
  }
  else
  {
    objifOperStatusValue = L7_XUI_LINK_DOWN;
  }
  /* return the object value: ifOperStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objifOperStatusValue,
                           sizeof (objifOperStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseInterfaceInfo_ifLastChange
*
* @purpose Get 'ifLastChange'
*
* @description  The value of sysUpTime at the time the interface entered its 
*              current operational state. If the current state was entered prior 
*              to the last re-initialization of the local network management 
*              subsystem, then this object 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseInterfaceInfo_ifLastChange (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objifLastChangeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseInterfaceInfo_ifIndex,
                          (xLibU8_t *) & keyifIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIfLastChangeGet (L7_UNIT_CURRENT, keyifIndexValue,
                                   &objifLastChangeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  /*Convert ticks to seeconds */
  objifLastChangeValue = objifLastChangeValue * 100;

  /* return the object value: ifLastChange */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objifLastChangeValue,
                           sizeof (objifLastChangeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseInterfaceInfo_ifInOctets
*
* @purpose Get 'ifInOctets'
*
* @description  The total number of octets received on the interface, including 
*              framing characters. Discontinuities in the value of this counter 
*              can occur at re-initialization of the management system, 
*              and at other times 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseInterfaceInfo_ifInOctets (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue,high,low;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibStr256_t objifInOctetsValue;
  xLibU64_t num;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseInterfaceInfo_ifIndex,
                          (xLibU8_t *) & keyifIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIfHCInOctetsGet (L7_UNIT_CURRENT, keyifIndexValue,
                                 &high,&low);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

   num.high = high;
   num.low = low;

  memset(objifInOctetsValue, 0x00, sizeof(objifInOctetsValue));
  memcpy(objifInOctetsValue,(void *)&num, sizeof(xLibU64_t));


  

  /* return the object value: ifInOctets */
  owa.rc = xLibBufDataSet (bufp,(xLibU8_t *)objifInOctetsValue,
                           sizeof (xLibU64_t));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseInterfaceInfo_ifInUcastPkts
*
* @purpose Get 'ifInUcastPkts'
*
* @description  The number of packets, delivered by this sub-layer to a higher 
*              (sub-)layer, which were not addressed to a multicast or broadcast 
*              address at this sub-layer. Discontinuities in the value 
*              of this counter can occur at 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseInterfaceInfo_ifInUcastPkts (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibStr256_t objifInUcastPktsValue;
  L7_uint32 high;
  L7_uint32 low;
  L7_ulong64 data64;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseInterfaceInfo_ifIndex,
                          (xLibU8_t *) & keyifIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIfHCInUcastPktsGet (L7_UNIT_CURRENT, keyifIndexValue,
                                     &high,&low);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  data64.high = high;
  data64.low = low;

  memset(objifInUcastPktsValue, 0x00, sizeof(objifInUcastPktsValue));
  memcpy(objifInUcastPktsValue,(void *)&data64, sizeof(L7_ulong64));


  /* return the object value: ifInUcastPkts */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objifInUcastPktsValue,
                           sizeof (objifInUcastPktsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseInterfaceInfo_ifInNUcastPkts
*
* @purpose Get 'ifInNUcastPkts'
*
* @description  The number of packets, delivered by this sub-layer to a higher 
*              (sub-)layer, which were addressed to a multicast or broadcast 
*              address at this sub-layer. Discontinuities in the value of 
*              this counter can occur at 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseInterfaceInfo_ifInNUcastPkts (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibStr256_t objifInNUcastPktsValue;
  L7_ulong64 num;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseInterfaceInfo_ifIndex,
                          (xLibU8_t *) & keyifIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIfHCInNUcastPktsGet(L7_UNIT_CURRENT, keyifIndexValue,
                                     &num);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  memset(objifInNUcastPktsValue, 0x00, sizeof(objifInNUcastPktsValue));
  memcpy(objifInNUcastPktsValue,(void *)&num, sizeof(xLibU64_t));

  /* return the object value: ifInNUcastPkts */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *)objifInNUcastPktsValue,
                           sizeof (xLibU64_t));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseInterfaceInfo_ifInDiscards
*
* @purpose Get 'ifInDiscards'
*
* @description  The number of inbound packets which were chosen to be discarded 
*              even though no errors had been detected to prevent their being 
*              deliverable to a higher-layer protocol. One possible reason 
*              for discarding such a packet 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseInterfaceInfo_ifInDiscards (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objifInDiscardsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseInterfaceInfo_ifIndex,
                          (xLibU8_t *) & keyifIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIfInDiscardsGet (L7_UNIT_CURRENT, keyifIndexValue,
                                   &objifInDiscardsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ifInDiscards */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objifInDiscardsValue,
                           sizeof (objifInDiscardsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseInterfaceInfo_ifInErrors
*
* @purpose Get 'ifInErrors'
*
* @description  For packet-oriented interfaces, the number of inbound packets 
*              that contained errors preventing them from being deliverable 
*              to a higher-layer protocol. For character- oriented or fixed-length 
*              interfaces, the number of 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseInterfaceInfo_ifInErrors (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objifInErrorsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
 kwa.rc = xLibFilterGet (wap, XOBJ_baseInterfaceInfo_ifIndex,
                          (xLibU8_t *) & keyifIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIfInErrorsGet (L7_UNIT_CURRENT, keyifIndexValue,
                                 &objifInErrorsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ifInErrors */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objifInErrorsValue,
                           sizeof (objifInErrorsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseInterfaceInfo_ifInUnknownProtos
*
* @purpose Get 'ifInUnknownProtos'
*
* @description The number of packets received via the interface which were discarded 
*              because of an unknown or unsupported protocol. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseInterfaceInfo_ifInUnknownProtos (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objifInUnknownProtosValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseInterfaceInfo_ifIndex,
                          (xLibU8_t *) & keyifIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIfInUnknownProtosGet (L7_UNIT_CURRENT, keyifIndexValue,
                                        &objifInUnknownProtosValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ifInUnknownProtos */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objifInUnknownProtosValue,
                           sizeof (objifInUnknownProtosValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseInterfaceInfo_ifOutOctets
*
* @purpose Get 'ifOutOctets'
*
* @description  The total number of octets transmitted out of the interface, 
*              including framing characters. Discontinuities in the value of 
*              this counter can occur at re-initialization of the management 
*              system, and at other 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseInterfaceInfo_ifOutOctets (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue,high,low;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibStr256_t objifOutOctetsValue;
  xLibU64_t num;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseInterfaceInfo_ifIndex,
                          (xLibU8_t *) & keyifIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIfHCOutOctetsGet(L7_UNIT_CURRENT, keyifIndexValue,
                                  &high,&low);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  num.high=high;
  num.low=low;
 
  memset(objifOutOctetsValue, 0x00, sizeof(objifOutOctetsValue));
  memcpy(objifOutOctetsValue,(void *)&num, sizeof(xLibU64_t));
 
  /* return the object value: ifOutOctets */
  owa.rc = xLibBufDataSet (bufp,(xLibU8_t *)objifOutOctetsValue,
                           sizeof (xLibU64_t));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseInterfaceInfo_ifOutUcastPkts
*
* @purpose Get 'ifOutUcastPkts'
*
* @description  The total number of packets that higher-level protocols requested 
*              be transmitted, and which were not addressed to a multicast 
*              or broadcast address at this sub-layer, including those that 
*              were discarded or not sent. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseInterfaceInfo_ifOutUcastPkts (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibStr256_t objifOutUcastPktsValue;
  L7_uint32 high;
  L7_uint32 low;
  L7_ulong64 data64;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseInterfaceInfo_ifIndex,
                          (xLibU8_t *) & keyifIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIfHCOutUcastPktsGet (L7_UNIT_CURRENT, keyifIndexValue,
                                     &high,&low);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  data64.high = high;
  data64.low = low;

  memset(objifOutUcastPktsValue, 0x00, sizeof(objifOutUcastPktsValue));
  memcpy(objifOutUcastPktsValue,(void *)&data64, sizeof(L7_ulong64));


  /* return the object value: ifOutUcastPkts */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objifOutUcastPktsValue,
                           sizeof (objifOutUcastPktsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseInterfaceInfo_ifOutNUcastPkts
*
* @purpose Get 'ifOutNUcastPkts'
*
* @description  The total number of packets that higher-level protocols requested 
*              be transmitted, and which were addressed to a multicast 
*              or broadcast address at this sub-layer, including those that 
*              were discarded or not sent. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseInterfaceInfo_ifOutNUcastPkts (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibStr256_t objifOutNUcastPktsValue;
  L7_ulong64 num;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseInterfaceInfo_ifIndex,
                          (xLibU8_t *) & keyifIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIfHCOutNUcastPktsGet (L7_UNIT_CURRENT, keyifIndexValue,
                                      &num);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  memset(objifOutNUcastPktsValue, 0x00, sizeof(objifOutNUcastPktsValue));
  memcpy(objifOutNUcastPktsValue,(void *)&num, sizeof(xLibU64_t));


  /* return the object value: ifOutNUcastPkts */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *)objifOutNUcastPktsValue,
                           sizeof (xLibU64_t));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseInterfaceInfo_ifOutDiscards
*
* @purpose Get 'ifOutDiscards'
*
* @description The number of outbound packets which were chosen to be discarded 
*              even though no errors had been detected to prevent their being 
*              transmitted. One possible reason for discarding such a packet 
*              could be to free up buffer space. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseInterfaceInfo_ifOutDiscards (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objifOutDiscardsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseInterfaceInfo_ifIndex,
                          (xLibU8_t *) & keyifIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIfOutDiscardsGet (L7_UNIT_CURRENT, keyifIndexValue,
                                    &objifOutDiscardsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ifOutDiscards */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objifOutDiscardsValue,
                           sizeof (objifOutDiscardsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseInterfaceInfo_ifOutErrors
*
* @purpose Get 'ifOutErrors'
*
* @description  For packet-oriented interfaces, the number of outbound packets 
*              that could not be transmitted because of errors. For character-oriented 
*              or fixed-length interfaces, the number of outbound 
*              transmission units that could no 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseInterfaceInfo_ifOutErrors (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objifOutErrorsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseInterfaceInfo_ifIndex,
                          (xLibU8_t *) & keyifIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIfOutErrorsGet (L7_UNIT_CURRENT, keyifIndexValue,
                                  &objifOutErrorsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ifOutErrors */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objifOutErrorsValue,
                           sizeof (objifOutErrorsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseInterfaceInfo_ifOutQLen
*
* @purpose Get 'ifOutQLen'
*
* @description The length of the output packet queue (in packets). 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseInterfaceInfo_ifOutQLen (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objifOutQLenValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseInterfaceInfo_ifIndex,
                          (xLibU8_t *) & keyifIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIfOutQLenGet (L7_UNIT_CURRENT, keyifIndexValue,
                                &objifOutQLenValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ifOutQLen */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objifOutQLenValue,
                           sizeof (objifOutQLenValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseInterfaceInfo_ifSpecific
*
* @purpose Get 'ifSpecific'
*
* @description A reference to MIB definitions specific to the particular media 
*              being used to realize the interface. For example, if the interface 
*              is realized by an ethernet, then the value of this object 
*              refers to a document defining objects specific to ethernet. 
*              If this information is not present, its value should be set 
*              to the OBJECT IDENTIFIER { 0 0 }, which is a syntatically 
*              valid object identifier, and any conformant implementation of 
*              ASN.1 and BER must be able to generate and recognize this value. 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseInterfaceInfo_ifSpecific (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objifSpecificValue;
  xLibU32_t tempLen;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseInterfaceInfo_ifIndex,
                          (xLibU8_t *) & keyifIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIfSpecificGet (L7_UNIT_CURRENT, keyifIndexValue,
                                 &objifSpecificValue,&tempLen);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ifSpecific */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objifSpecificValue,
                           sizeof (objifSpecificValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseInterfaceInfo_ifNumber
*
* @purpose Get 'ifNumber'
*
* @description  The number of network interfaces (regardless of their current 
*              state) present on this system. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseInterfaceInfo_ifNumber (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objifNumberValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDb1213IfNumberGet (L7_UNIT_CURRENT, &objifNumberValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objifNumberValue, sizeof (objifNumberValue));

  /* return the object value: ifNumber */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objifNumberValue,
                           sizeof (objifNumberValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseInterfaceInfo_IfEncapsulationType
*
* @purpose Get 'IfEncapsulationType'
*
* @description [IfEncapsulationType]:  Encaptulation type. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseInterfaceInfo_IfEncapsulationType (void *wap, void *bufp)
{
  fpObjWa_t kwaifIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIfEncapsulationTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  kwaifIndex.rc = xLibFilterGet (wap, XOBJ_baseInterfaceInfo_ifIndex,
                                 (xLibU8_t *) & keyifIndexValue,
                                 &kwaifIndex.len);
  if (kwaifIndex.rc != XLIBRC_SUCCESS)
  {
    kwaifIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaifIndex);
    return kwaifIndex.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwaifIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbIfEncapsulationTypeGet ( keyifIndexValue,
                              &objIfEncapsulationTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: IfEncapsulationType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIfEncapsulationTypeValue,
                           sizeof (objIfEncapsulationTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_baseInterfaceInfo_IfEncapsulationType
*
* @purpose Set 'IfEncapsulationType'
 *@description  [IfEncapsulationType]  Encaptulation type.
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseInterfaceInfo_IfEncapsulationType (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objIfEncapsulationTypeValue;

  xLibU32_t keyifIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: IfEncapsulationType */
  owa.len = sizeof (objIfEncapsulationTypeValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objIfEncapsulationTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objIfEncapsulationTypeValue, owa.len);

  /* retrieve key: ifIndex */
  owa.len = sizeof (keyifIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_baseInterfaceInfo_ifIndex,
                          (xLibU8_t *) & keyifIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbIfEncapsulationTypeSet(keyifIndexValue, objIfEncapsulationTypeValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseInterfaceInfo_ifName
*
* @purpose Get 'ifName'
*
* @description The Name of the interface 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseInterfaceInfo_ifName (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objifNameValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseInterfaceInfo_ifIndex,
                          (xLibU8_t *) & keyifIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIfNameGet (L7_UNIT_CURRENT, keyifIndexValue, objifNameValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ifName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objifNameValue,
                           strlen (objifNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseInterfaceInfo_ifInMulticastPkts
*
* @purpose Get 'ifInMulticastPkts'
*
* @description Multicast packets received 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseInterfaceInfo_ifInMulticastPkts (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue,high,low;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibStr256_t objifInMulticastPktsValue;
  xLibU64_t num;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseInterfaceInfo_ifIndex,
                          (xLibU8_t *) & keyifIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIfHCInMulticastPktsGet (L7_UNIT_CURRENT, keyifIndexValue,
                                        &high,&low);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
   num.high = high;
   num.low = low;

   memset(objifInMulticastPktsValue, 0x00, sizeof(objifInMulticastPktsValue));
   memcpy(objifInMulticastPktsValue,(void *)&num, sizeof(xLibU64_t));


  /* return the object value: ifInMulticastPkts */
  owa.rc = xLibBufDataSet (bufp,(xLibU8_t *)objifInMulticastPktsValue,
                           sizeof (xLibU64_t));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseInterfaceInfo_ifInBroadcastPkts
*
* @purpose Get 'ifInBroadcastPkts'
*
* @description Broadcast packets received 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseInterfaceInfo_ifInBroadcastPkts (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue,high,low;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibStr256_t objifInBroadcastPktsValue;
  xLibU64_t num;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseInterfaceInfo_ifIndex,
                          (xLibU8_t *) & keyifIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIfHCInBroadcastPktsGet (L7_UNIT_CURRENT, keyifIndexValue,
                                        &high,&low);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  num.high = high;
  num.low = low;

  memset(objifInBroadcastPktsValue, 0x00, sizeof(objifInBroadcastPktsValue));
  memcpy(objifInBroadcastPktsValue,(void *)&num, sizeof(xLibU64_t));


  /* return the object value: ifInBroadcastPkts */
  owa.rc = xLibBufDataSet (bufp,(xLibU8_t *)objifInBroadcastPktsValue,
                           sizeof (xLibU64_t));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseInterfaceInfo_ifOutMulticastPkts
*
* @purpose Get 'ifOutMulticastPkts'
*
* @description Multicast packets sent 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseInterfaceInfo_ifOutMulticastPkts (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue,high,low;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibStr256_t objifOutMulticastPktsValue;
  xLibU64_t num;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseInterfaceInfo_ifIndex,
                          (xLibU8_t *) & keyifIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIfHCOutMulticastPktsGet (L7_UNIT_CURRENT, keyifIndexValue,
                                         &high,&low);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  num.high = high;
  num.low = low;

  memset(objifOutMulticastPktsValue, 0x00, sizeof(objifOutMulticastPktsValue));
  memcpy(objifOutMulticastPktsValue,(void *)&num, sizeof(xLibU64_t));
 
  /* return the object value: ifOutMulticastPkts */
  owa.rc = xLibBufDataSet (bufp,objifOutMulticastPktsValue,
                           sizeof (xLibU64_t));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseInterfaceInfo_ifOutBroadcastPkts
*
* @purpose Get 'ifOutBroadcastPkts'
*
* @description Broadcast packets sent 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseInterfaceInfo_ifOutBroadcastPkts (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue,high,low;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibStr256_t objifOutBroadcastPktsValue;
  xLibU64_t num;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseInterfaceInfo_ifIndex,
                          (xLibU8_t *) & keyifIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIfHCOutBroadcastPktsGet (L7_UNIT_CURRENT, keyifIndexValue,
                                         &high,&low);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  num.high = high;
  num.low = low;

  memset(objifOutBroadcastPktsValue, 0x00, sizeof(objifOutBroadcastPktsValue));
  memcpy(objifOutBroadcastPktsValue,(void *)&num, sizeof(xLibU64_t));

  /* return the object value: ifOutBroadcastPkts */
  owa.rc = xLibBufDataSet (bufp,(xLibU8_t *)objifOutBroadcastPktsValue,
                           sizeof (xLibU64_t));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseInterfaceInfo_ifLinkUpDownTrapEnable
*
* @purpose Get 'ifLinkUpDownTrapEnable'
*
* @description Link UP Down trap 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseInterfaceInfo_ifLinkUpDownTrapEnable (void *wap,
                                                            void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objifLinkUpDownTrapEnableValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseInterfaceInfo_ifIndex,
                          (xLibU8_t *) & keyifIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIfLinkUpDownTrapEnableGet (L7_UNIT_CURRENT, keyifIndexValue,
                                             &objifLinkUpDownTrapEnableValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ifLinkUpDownTrapEnable */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objifLinkUpDownTrapEnableValue,
                           sizeof (objifLinkUpDownTrapEnableValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseInterfaceInfo_ifLinkUpDownTrapEnable
*
* @purpose Set 'ifLinkUpDownTrapEnable'
*
* @description Link UP Down trap 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseInterfaceInfo_ifLinkUpDownTrapEnable (void *wap,
                                                            void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objifLinkUpDownTrapEnableValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ifLinkUpDownTrapEnable */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objifLinkUpDownTrapEnableValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objifLinkUpDownTrapEnableValue, owa.len);

  /* retrieve key: ifIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseInterfaceInfo_ifIndex,
                          (xLibU8_t *) & keyifIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbIfLinkUpDownTrapEnableSet (L7_UNIT_CURRENT, keyifIndexValue,
                                             objifLinkUpDownTrapEnableValue);
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
* @function fpObjGet_baseInterfaceInfo_ifHighSpeed
*
* @purpose Get 'ifHighSpeed'
*
* @description Speed Status 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseInterfaceInfo_ifHighSpeed (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objifHighSpeedValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseInterfaceInfo_ifIndex,
                          (xLibU8_t *) & keyifIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIfSpeedStatusGet (L7_UNIT_CURRENT, keyifIndexValue,
                                    &objifHighSpeedValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ifHighSpeed */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objifHighSpeedValue,
                           sizeof (objifHighSpeedValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseInterfaceInfo_ifPromiscuousMode
*
* @purpose Get 'ifPromiscuousMode'
*
* @description Promiscuous mode 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseInterfaceInfo_ifPromiscuousMode (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objifPromiscuousModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseInterfaceInfo_ifIndex,
                          (xLibU8_t *) & keyifIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIfPromiscuousModeGet (L7_UNIT_CURRENT, keyifIndexValue,
                                        &objifPromiscuousModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ifPromiscuousMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objifPromiscuousModeValue,
                           sizeof (objifPromiscuousModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseInterfaceInfo_ifPromiscuousMode
*
* @purpose Set 'ifPromiscuousMode'
*
* @description Promiscuous mode 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseInterfaceInfo_ifPromiscuousMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objifPromiscuousModeValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ifPromiscuousMode */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objifPromiscuousModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objifPromiscuousModeValue, owa.len);

  /* retrieve key: ifIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseInterfaceInfo_ifIndex,
                          (xLibU8_t *) & keyifIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbIfPromiscuousModeSet (L7_UNIT_CURRENT, keyifIndexValue,
                                        objifPromiscuousModeValue);
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
* @function fpObjGet_baseInterfaceInfo_ifConnectorPresent
*
* @purpose Get 'ifConnectorPresent'
*
* @description check if connector present on this interface 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseInterfaceInfo_ifConnectorPresent (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objifConnectorPresentValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseInterfaceInfo_ifIndex,
                          (xLibU8_t *) & keyifIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIfConnectorPresentGet (L7_UNIT_CURRENT, keyifIndexValue,
                                         &objifConnectorPresentValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ifConnectorPresent */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objifConnectorPresentValue,
                           sizeof (objifConnectorPresentValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseInterfaceInfo_ifAlias
*
* @purpose Get 'ifAlias'
*
* @description Alias for this interface 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseInterfaceInfo_ifAlias (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objifAliasValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseInterfaceInfo_ifIndex,
                          (xLibU8_t *) & keyifIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIfAliasGet (L7_UNIT_CURRENT, keyifIndexValue,
                              objifAliasValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ifAlias */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objifAliasValue,
                           strlen (objifAliasValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseInterfaceInfo_ifAlias
*
* @purpose Set 'ifAlias'
*
* @description Alias for this interface 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseInterfaceInfo_ifAlias (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objifAliasValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ifAlias */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objifAliasValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objifAliasValue, owa.len);

  /* retrieve key: ifIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseInterfaceInfo_ifIndex,
                          (xLibU8_t *) & keyifIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa.len);

  if(strlen(objifAliasValue) > L7_NIM_IF_ALIAS_SIZE)
  {
     owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
     FPOBJ_TRACE_EXIT (bufp, owa);
     return owa.rc;
  }
  /* set the value in application */
  owa.l7rc = usmDbIfAliasSet (L7_UNIT_CURRENT, keyifIndexValue,
                              objifAliasValue);
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
* @function fpObjGet_baseInterfaceInfo_ifCounterDiscontinuityTime
*
* @purpose Get 'ifCounterDiscontinuityTime'
*
* @description Counter discontinuity time 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseInterfaceInfo_ifCounterDiscontinuityTime (void *wap,
                                                                void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objifCounterDiscontinuityTimeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseInterfaceInfo_ifIndex,
                          (xLibU8_t *) & keyifIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbIfCounterDiscontinuityTimeGet (L7_UNIT_CURRENT, keyifIndexValue,
                                        &objifCounterDiscontinuityTimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ifCounterDiscontinuityTime */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objifCounterDiscontinuityTimeValue,
                    sizeof (objifCounterDiscontinuityTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseInterfaceInfo_ifTestId
*
* @purpose Get 'ifTestId'
*
* @description TestID 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseInterfaceInfo_ifTestId (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objifTestIdValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseInterfaceInfo_ifIndex,
                          (xLibU8_t *) & keyifIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIfTestIdGet (L7_UNIT_CURRENT, keyifIndexValue,
                               &objifTestIdValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ifTestId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objifTestIdValue,
                           sizeof (objifTestIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseInterfaceInfo_ifTestId
*
* @purpose Set 'ifTestId'
*
* @description TestID 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseInterfaceInfo_ifTestId (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objifTestIdValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ifTestId */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objifTestIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objifTestIdValue, owa.len);

  /* retrieve key: ifIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseInterfaceInfo_ifIndex,
                          (xLibU8_t *) & keyifIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbIfTestIdSet (L7_UNIT_CURRENT, keyifIndexValue,
                               objifTestIdValue);
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
* @function fpObjGet_baseInterfaceInfo_ifTestStatus
*
* @purpose Get 'ifTestStatus'
*
* @description Test status 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseInterfaceInfo_ifTestStatus (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objifTestStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseInterfaceInfo_ifIndex,
                          (xLibU8_t *) & keyifIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIfTestStatusGet (L7_UNIT_CURRENT, keyifIndexValue,
                                   &objifTestStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ifTestStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objifTestStatusValue,
                           sizeof (objifTestStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseInterfaceInfo_ifTestStatus
*
* @purpose Set 'ifTestStatus'
*
* @description Test status 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseInterfaceInfo_ifTestStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objifTestStatusValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ifTestStatus */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objifTestStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objifTestStatusValue, owa.len);

  /* retrieve key: ifIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseInterfaceInfo_ifIndex,
                          (xLibU8_t *) & keyifIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbIfTestStatusSet (L7_UNIT_CURRENT, keyifIndexValue,
                                   objifTestStatusValue);
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
* @function fpObjGet_baseInterfaceInfo_ifTestType
*
* @purpose Get 'ifTestType'
*
* @description Test type 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseInterfaceInfo_ifTestType (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objifTestTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseInterfaceInfo_ifIndex,
                          (xLibU8_t *) & keyifIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa.len);

  /* get the value from application */
  /*owa.l7rc = usmDbIfTestTypeGet (L7_UNIT_CURRENT, keyifIndexValue,
                                 &objifTestTypeValue);*/
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ifTestType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objifTestTypeValue,
                           sizeof (objifTestTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseInterfaceInfo_ifTestType
*
* @purpose Set 'ifTestType'
*
* @description Test type 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseInterfaceInfo_ifTestType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objifTestTypeValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ifTestType */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objifTestTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objifTestTypeValue, owa.len);

  /* retrieve key: ifIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseInterfaceInfo_ifIndex,
                          (xLibU8_t *) & keyifIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa.len);

  /* set the value in application */
  /*owa.l7rc = usmDbIfTestTypeSet (L7_UNIT_CURRENT, keyifIndexValue,
                                 objifTestTypeValue);*/
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
* @function fpObjGet_baseInterfaceInfo_ifTestResult
*
* @purpose Get 'ifTestResult'
*
* @description Test Result 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseInterfaceInfo_ifTestResult (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objifTestResultValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseInterfaceInfo_ifIndex,
                          (xLibU8_t *) & keyifIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIfTestResultGet (L7_UNIT_CURRENT, keyifIndexValue,
                                   &objifTestResultValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ifTestResult */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objifTestResultValue,
                           sizeof (objifTestResultValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseInterfaceInfo_ifTestCode
*
* @purpose Get 'ifTestCode'
*
* @description Test Code 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseInterfaceInfo_ifTestCode (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objifTestCodeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseInterfaceInfo_ifIndex,
                          (xLibU8_t *) & keyifIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa.len);

  /* get the value from application */
  /*owa.l7rc = usmDbIfTestCodeGet (L7_UNIT_CURRENT, keyifIndexValue,
                                 &objifTestCodeValue);*/
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ifTestCode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objifTestCodeValue,
                           sizeof (objifTestCodeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseInterfaceInfo_ifTestOwner
*
* @purpose Get 'ifTestOwner'
*
* @description Test Owner 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseInterfaceInfo_ifTestOwner (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objifTestOwnerValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseInterfaceInfo_ifIndex,
                          (xLibU8_t *) & keyifIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIfTestOwnerGet (L7_UNIT_CURRENT, keyifIndexValue,
                                  objifTestOwnerValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: ifTestOwner */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objifTestOwnerValue,
                           strlen (objifTestOwnerValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseInterfaceInfo_ifTestOwner
*
* @purpose Set 'ifTestOwner'
*
* @description Test Owner 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseInterfaceInfo_ifTestOwner (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objifTestOwnerValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ifTestOwner */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objifTestOwnerValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objifTestOwnerValue, owa.len);

  /* retrieve key: ifIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseInterfaceInfo_ifIndex,
                          (xLibU8_t *) & keyifIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbIfTestOwnerSet (L7_UNIT_CURRENT, keyifIndexValue,
                                  objifTestOwnerValue);
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
* @function fpObjGet_baseInterfaceInfo_ifInUcastPktsPercentage 
*
* @purpose Get 'ifInUcastPktsPercentage'
*
* @description To get the unicast packets received in percentage.
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseInterfaceInfo_ifInUcastPktsPercentage (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibUL32_t objifInUcastPktsValue;
  L7_uint32 high;
  L7_uint32 low;
  L7_ulong64 data64;
  xLibDouble64_t objifInUcastPktsValueInPercent; 
  xLibU32_t objetherStatsPktsValue;
  xLibStr256_t objifInUcastPercent;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseInterfaceInfo_ifIndex,
                          (xLibU8_t *) & keyifIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa.len);

  /* get number of unicast packets received from application */
  owa.l7rc = usmDbIfHCInUcastPktsGet (L7_UNIT_CURRENT, keyifIndexValue,
                                     &high,&low);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  data64.high = high;
  data64.low = low;
  sprintf(objifInUcastPercent, "%d%d",data64.high,data64.low);
  
  objifInUcastPktsValue = atol(objifInUcastPercent);
  
  if(objifInUcastPktsValue != 0)
  {

    /* get total no: of packets received from application */
    owa.l7rc = usmDbEtherStatsPktsGet (L7_UNIT_CURRENT, keyifIndexValue,
                                     &objetherStatsPktsValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
 
    if (objetherStatsPktsValue != 0) {
      objifInUcastPktsValueInPercent = ((double)objifInUcastPktsValue / (double)objetherStatsPktsValue) * 100;
      sprintf(objifInUcastPercent,"%f",objifInUcastPktsValueInPercent); 
    }

    else {
      sprintf(objifInUcastPercent,"%d",objetherStatsPktsValue);
    }
  }
  else 
  {
    sprintf(objifInUcastPercent,"%ld",objifInUcastPktsValue);
  }
  /* return the object value: ifInUcastPkts */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objifInUcastPercent,
                           sizeof (objifInUcastPercent));
  
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseInterfaceInfo_ifInNUcastPktsInPercent 
*
* @purpose Get 'ifInNUcastPktsInPercent'
*
* @description To get the non-unicast packets received in percentage.
*
* @return
*******************************************************************************/

xLibRC_t fpObjGet_baseInterfaceInfo_ifInNUcastPktsInPercent (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibUL32_t objifInNUcastPktsValue;
  L7_ulong64 num;
  xLibDouble64_t objifInNUcastPktsValueInPercent;
  xLibU32_t objetherStatsPktsValue;
  xLibStr256_t objifInNUcastPercent;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseInterfaceInfo_ifIndex,
                          (xLibU8_t *) & keyifIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIfHCInNUcastPktsGet(L7_UNIT_CURRENT, keyifIndexValue,
                                     &num);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  sprintf(objifInNUcastPercent,"%d%d",num.high,num.low);
  objifInNUcastPktsValue = atol(objifInNUcastPercent);
  
  if(objifInNUcastPktsValue != 0)
  {
    /* get total no: of packets received from application */
    owa.l7rc = usmDbEtherStatsPktsGet (L7_UNIT_CURRENT, keyifIndexValue,
                                     &objetherStatsPktsValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }

    if(objetherStatsPktsValue != 0){
      objifInNUcastPktsValueInPercent = ((double)objifInNUcastPktsValue / (double)objetherStatsPktsValue) * 100;
      sprintf(objifInNUcastPercent,"%f",objifInNUcastPktsValueInPercent);
    }
    else{
      sprintf(objifInNUcastPercent,"%d",objetherStatsPktsValue);
    }
  }
  else 
  {
      sprintf(objifInNUcastPercent,"%ld",objifInNUcastPktsValue);
  }
  /* return the object value: ifInNUcastPkts */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *)objifInNUcastPercent,
                           sizeof (objifInNUcastPercent));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseInterfaceInfo_ifInErrorsInPercent 
*
* @purpose Get 'ifInErrorsInPercent'
*
* @description To get the error packets received in percentage.
*
* @return
*******************************************************************************/

xLibRC_t fpObjGet_baseInterfaceInfo_ifInErrorsInPercent (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyifIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objifInErrorsValue;
  xLibDouble64_t objifInErrorPktsValueInPercent;
  xLibU32_t objetherStatsPktsValue;
  xLibStr256_t objifInErrorPktsPercent;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ifIndex */
 kwa.rc = xLibFilterGet (wap, XOBJ_baseInterfaceInfo_ifIndex,
                          (xLibU8_t *) & keyifIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyifIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIfInErrorsGet (L7_UNIT_CURRENT, keyifIndexValue,
                                 &objifInErrorsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  if (objifInErrorsValue != 0)
  { 
     /* get total no: of packets received from application */
     owa.l7rc = usmDbEtherStatsPktsGet (L7_UNIT_CURRENT, keyifIndexValue,
                                     &objetherStatsPktsValue);
     if (owa.l7rc != L7_SUCCESS)
     { 
       owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
       FPOBJ_TRACE_EXIT (bufp, owa);
       return owa.rc;
     }
  
     if(objetherStatsPktsValue != 0){
         objifInErrorPktsValueInPercent = ((double)objifInErrorsValue / (double)objetherStatsPktsValue) * 100;
         sprintf(objifInErrorPktsPercent,"%f",objifInErrorPktsValueInPercent);
     }
     else{
       sprintf(objifInErrorPktsPercent,"%d",objetherStatsPktsValue);
     }
  }
  else 
  {
     sprintf(objifInErrorPktsPercent,"%d",objifInErrorsValue);
  }
  
  /* return the object value: ifInErrors */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objifInErrorPktsPercent,
                           sizeof (objifInErrorPktsPercent));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}



