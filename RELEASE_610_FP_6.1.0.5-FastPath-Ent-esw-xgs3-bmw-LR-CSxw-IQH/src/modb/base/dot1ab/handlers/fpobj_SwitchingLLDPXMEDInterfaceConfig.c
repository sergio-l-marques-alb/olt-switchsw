/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_SwitchingLLDPXMEDInterfaceConfig.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to LLDP-object.xml
*
* @create  7 March 2008
*
* @author  Radha K 
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_SwitchingLLDPXMEDInterfaceConfig_obj.h"
#include "usmdb_util_api.h"
#include "usmdb_nim_api.h"
#include "usmdb_lldp_api.h"

extern L7_RC_t usmDbIfOperStatusGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val);

/*******************************************************************************
* @function fpObjGet_SwitchingLLDPXMEDInterfaceConfig_LldpXMedInterface
*
* @purpose Get 'LldpXMedInterface'
*
* @description [LldpXMedInterface]: An interface on the Local System 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPXMEDInterfaceConfig_LldpXMedInterface(void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objLldpXMedInterfaceValue;
  xLibU32_t nextObjLldpXMedInterfaceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: LocalPortNum */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDInterfaceConfig_LldpXMedInterface,
                          (xLibU8_t *) & objLldpXMedInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbValidIntIfNumFirstGet (&nextObjLldpXMedInterfaceValue);
  }
  else
  {
      FPOBJ_TRACE_CURRENT_KEY (bufp, &objLldpXMedInterfaceValue, owa.len);
      owa.l7rc = usmDbLldpValidIntfNextGet ( objLldpXMedInterfaceValue,
                                            &nextObjLldpXMedInterfaceValue);
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjLldpXMedInterfaceValue, owa.len);

  /* return the object value: LocalPortNum */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjLldpXMedInterfaceValue,
                           sizeof (objLldpXMedInterfaceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_SwitchingLLDPXMEDInterfaceConfig_LldpXMedPortAdminMode
*
* @purpose Get 'LldpXMedPortAdminMode'
*
* @description [LldpXMedPortAdminMode]: The MED admin mode for this interface
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPXMEDInterfaceConfig_LldpXMedPortAdminMode(void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyLldpXMedInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objLldpXMedPortAdminModeValue = L7_FALSE;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: LocalPortNum */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDInterfaceConfig_LldpXMedInterface,
                          (xLibU8_t *) & keyLldpXMedInterfaceValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLldpXMedInterfaceValue, kwa.len);


  if( keyLldpXMedInterfaceValue != L7_ALL_INTERFACES)
  {
  /* get the value from application */
    owa.l7rc =
      usmDbLldpXMedPortAdminModeGet (keyLldpXMedInterfaceValue,
                                               &objLldpXMedPortAdminModeValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }
  /* return the object value: PortAutoNegSupported */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objLldpXMedPortAdminModeValue,
                           sizeof (objLldpXMedPortAdminModeValue));

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_SwitchingLLDPXMEDInterfaceConfig_LldpXMedPortAdminMode
*
* @purpose Set 'LldpXMedPortAdminMode'
*
* @description [LldpXMedPortAdminMode]: The MED admin mode for this interface
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingLLDPXMEDInterfaceConfig_LldpXMedPortAdminMode(void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objLldpXMedPortAdminModeValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyLldpXMedInterfaceValue;
  xLibU32_t intIfNum;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: NotifEnabled */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objLldpXMedPortAdminModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objLldpXMedPortAdminModeValue, owa.len);

  /* retrieve key: PortNum */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDInterfaceConfig_LldpXMedInterface,
                          (xLibU8_t *) & keyLldpXMedInterfaceValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLldpXMedInterfaceValue, kwa.len);

  /* set the value in application */

  if( keyLldpXMedInterfaceValue == L7_ALL_INTERFACES )
  {
    intIfNum = 0;
    usmDbLldpValidIntfFirstGet(&intIfNum);
    do
    {
      owa.l7rc =
          usmDbLldpXMedPortAdminModeSet (intIfNum,
                                           objLldpXMedPortAdminModeValue);
      if (owa.l7rc != L7_SUCCESS)
      {
        owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
      }
    } while( usmDbLldpValidIntfNextGet(intIfNum, &intIfNum) == L7_SUCCESS);

  }
  else
  {
    owa.l7rc =
      usmDbLldpXMedPortAdminModeSet (keyLldpXMedInterfaceValue,
                                           objLldpXMedPortAdminModeValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingLLDPXMedConfig_NotifEnabled
*
* @purpose Get 'NotifEnabled'
*
* @description [NotifEnabled]: A value of 'true(1)' enables sending the topology
*              change traps on this port. A value of 'false(2)' disables
*              sending the topology change traps on this port. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPXMEDInterfaceConfig_NotifEnabled (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPortNumValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objNotifEnabledValue = L7_FALSE;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PortNum */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDInterfaceConfig_LldpXMedInterface,
                          (xLibU8_t *) & keyPortNumValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPortNumValue, kwa.len);

  /* get the value from application */

  if( keyPortNumValue != L7_ALL_INTERFACES)
  {
    owa.l7rc =
       usmDbLldpXMedPortConfigNotifEnableGet (keyPortNumValue,
                                           &objNotifEnabledValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }

  }
  /* return the object value: NotifEnabled */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objNotifEnabledValue,
                           sizeof (objNotifEnabledValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingLLDPXMedConfig_NotifEnabled
*
* @purpose Set 'NotifEnabled'
*
* @description [NotifEnabled]: A value of 'true(1)' enables sending the topology
*              change traps on this port. A value of 'false(2)' disables
*              sending the topology change traps on this port. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingLLDPXMEDInterfaceConfig_NotifEnabled (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objNotifEnabledValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPortNumValue;
  xLibU32_t intIfNum;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: NotifEnabled */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objNotifEnabledValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objNotifEnabledValue, owa.len);

  /* retrieve key: PortNum */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDInterfaceConfig_LldpXMedInterface,
                          (xLibU8_t *) & keyPortNumValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPortNumValue, kwa.len);

  /* set the value in application */

  if( keyPortNumValue == L7_ALL_INTERFACES )
  { 
    intIfNum = 0;
    usmDbLldpValidIntfFirstGet(&intIfNum);
    do 
    {
      owa.l7rc =
           usmDbLldpXMedPortConfigNotifEnableSet (intIfNum,
                                           objNotifEnabledValue);
      if (owa.l7rc != L7_SUCCESS)
      {  
        owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
      }

    }while( usmDbLldpValidIntfNextGet(intIfNum, &intIfNum) == L7_SUCCESS) ;
  }
  else
  {
    owa.l7rc =
      usmDbLldpXMedPortConfigNotifEnableSet (keyPortNumValue,
                                           objNotifEnabledValue);
  }
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
* @function fpObjGet_SwitchingLLDPXMedConfig_TLVsTxEnable
*
* @purpose Get 'TLVsTxEnable'
*
* @description [TLVsTxEnable]: The lldpXMedPortConfigTLVsTxEnable, defined
*              as a bitmap,includes the MED organizationally defined set
*              of LLDP TLVs whose transmission is allowed on the local LLDP
*              agent by the network management. Each bit in the bitmap corresponds
*              to an LLDP- 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPXMEDInterfaceConfig_TLVsTxEnable (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPortNumValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objTLVsTxEnableValue;
  xLibU32_t strlength=0,flag=L7_FALSE;
  lldpXMedCapabilities_t capGet;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PortNum */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDInterfaceConfig_LldpXMedInterface,
                          (xLibU8_t *) & keyPortNumValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPortNumValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbLldpXMedPortConfigTLVsEnabledGet ( keyPortNumValue,
                                            &capGet);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  memset(objTLVsTxEnableValue,0x00,sizeof(objTLVsTxEnableValue));
  /* return the object value: TLVsTxEnable */
  if (capGet.bitmap[1] & LLDP_MED_CAP_CAPABILITIES_BITMASK)
  {
    osapiStrncpySafe(objTLVsTxEnableValue, usmDbLldpXMedMediaCapabilitiesString(0), sizeof(objTLVsTxEnableValue));
    flag=L7_TRUE;
  }
  if (capGet.bitmap[1] & LLDP_MED_CAP_NETWORKPOLICY_BITMASK)
  {
    if(flag == L7_TRUE)
    {
      strcat(objTLVsTxEnableValue,"<BR>");
    }
    if((strlen(objTLVsTxEnableValue) + strlen(usmDbLldpXMedMediaCapabilitiesString(1))) < sizeof(xLibStr256_t)) 
    {
      strcat(objTLVsTxEnableValue, usmDbLldpXMedMediaCapabilitiesString(1)); 
      flag=L7_TRUE;
    }
  }
  if (capGet.bitmap[1] & LLDP_MED_CAP_LOCATION_BITMASK)
  {
    if(flag == L7_TRUE)
    {
      strcat(objTLVsTxEnableValue,"<BR>");
    }
    if((strlen(objTLVsTxEnableValue) + strlen(usmDbLldpXMedMediaCapabilitiesString(2))) < sizeof(xLibStr256_t)) 
    {
      strcat(objTLVsTxEnableValue, usmDbLldpXMedMediaCapabilitiesString(2));
      flag=L7_TRUE;
    }
  }
  if (capGet.bitmap[1] & LLDP_MED_CAP_EXT_PSE_BITMASK)
  {
    if(flag == L7_TRUE)
    {
      strcat(objTLVsTxEnableValue,"<BR>");
    }
    if((strlen(objTLVsTxEnableValue) + strlen(usmDbLldpXMedMediaCapabilitiesString(3))) < sizeof(xLibStr256_t)) 
    {
      strcat(objTLVsTxEnableValue, usmDbLldpXMedMediaCapabilitiesString(3));
      flag=L7_TRUE;
    }
  }
  if (capGet.bitmap[1] & LLDP_MED_CAP_EXT_PD_BITMASK)
  {
    if(flag == L7_TRUE)
    {
      strcat(objTLVsTxEnableValue,"<BR>");
    }
    if((strlen(objTLVsTxEnableValue) + strlen(usmDbLldpXMedMediaCapabilitiesString(4))) < sizeof(xLibStr256_t)) 
    {
      strcat(objTLVsTxEnableValue, usmDbLldpXMedMediaCapabilitiesString(4));
      flag=L7_TRUE;
    }
  }
  if (capGet.bitmap[1] & LLDP_MED_CAP_INVENTORY_BITMASK)
  {
    if(flag == L7_TRUE)
    {
      strcat(objTLVsTxEnableValue,"<BR>");
    }
    if((strlen(objTLVsTxEnableValue) + strlen(usmDbLldpXMedMediaCapabilitiesString(5))) < sizeof(xLibStr256_t)) 
    {
      strcat(objTLVsTxEnableValue, usmDbLldpXMedMediaCapabilitiesString(5));
    }
  } 

  if(strlen(objTLVsTxEnableValue) >= sizeof(xLibStr256_t))
  {
    strlength = sizeof(xLibStr256_t);   
  }
  else
  {
    strlength = strlen(objTLVsTxEnableValue);   
  }

  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objTLVsTxEnableValue,
                           strlength);
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingLLDPXMEDInterfaceConfig_OperationalStatus
*
* @purpose Get 'OperationalStatus'
*
* @description [OperationalStatus]: whether we have seen a MED TLV and we
*              are transmitting MED TLVs
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPXMEDInterfaceConfig_OperationalStatus(void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyLldpXMedInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objOperationalStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: LocalPortNum */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDInterfaceConfig_LldpXMedInterface,
                          (xLibU8_t *) & keyLldpXMedInterfaceValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyLldpXMedInterfaceValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbLldpXMedPortTransmitEnabledGet ( keyLldpXMedInterfaceValue,
                                               &objOperationalStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: PortAutoNegSupported */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objOperationalStatusValue,
                           sizeof (objOperationalStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}



xLibRC_t fpObjGet_SwitchingLLDPXMEDInterfaceConfig_LLDPMedCapCapabilities (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPortNumValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibU32_t val=L7_DISABLE;
  lldpXMedCapabilities_t capGet;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PortNum */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDInterfaceConfig_LldpXMedInterface,
                          (xLibU8_t *) & keyPortNumValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPortNumValue, kwa.len);

  /* get the value from application */
  if( keyPortNumValue != L7_ALL_INTERFACES)
  {

    owa.l7rc =
    usmDbLldpXMedPortConfigTLVsEnabledGet ( keyPortNumValue,
                                            &capGet);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
   }

  /* return the object value */
    if (capGet.bitmap[1] & LLDP_MED_CAP_CAPABILITIES_BITMASK)
    {
      val = L7_ENABLE;
    }
    else
    {
      val = L7_DISABLE;
    }
  }
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &val, sizeof (val));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

xLibRC_t fpObjSet_SwitchingLLDPXMEDInterfaceConfig_LLDPMedCapCapabilities (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPortNumValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t val;
  xLibU32_t intIfNum;
  lldpXMedCapabilities_t capGet;
  FPOBJ_TRACE_ENTER (bufp);
  
  memset(&capGet, 0, sizeof(lldpXMedCapabilities_t));

  /* retrieve object:  */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & val, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &val, owa.len);

  /* retrieve key: PortNum */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDInterfaceConfig_LldpXMedInterface,
                          (xLibU8_t *) & keyPortNumValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPortNumValue, kwa.len);


  if( keyPortNumValue == L7_ALL_INTERFACES )
  { 
    intIfNum = 0;
    usmDbLldpValidIntfFirstGet(&intIfNum);
    do 
    {
      owa.l7rc =
        usmDbLldpXMedPortConfigTLVsEnabledGet ( intIfNum,
                                            &capGet);
      if (owa.l7rc != L7_SUCCESS)
      {  
        owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
      }
      if(val == L7_ENABLE)
      {
        capGet.bitmap[1] |= LLDP_MED_CAP_CAPABILITIES_BITMASK;
      }
      else
      {
        capGet.bitmap[1] &= ~LLDP_MED_CAP_CAPABILITIES_BITMASK;
      }

  /* set the value from application */
      owa.l7rc =
         usmDbLldpXMedPortConfigTLVsEnabledSet ( intIfNum,
                                            &capGet);
      if (owa.l7rc != L7_SUCCESS)
       {
         owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
         FPOBJ_TRACE_EXIT (bufp, owa);
         return owa.rc;
       }
    }while( usmDbLldpValidIntfNextGet(intIfNum, &intIfNum) == L7_SUCCESS);

  }
  else
  {
  /* get the value from application */
    owa.l7rc =
      usmDbLldpXMedPortConfigTLVsEnabledGet ( keyPortNumValue,
                                            &capGet);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    if(val == L7_ENABLE)
    {
      capGet.bitmap[1] |= LLDP_MED_CAP_CAPABILITIES_BITMASK;
    }
    else
    {
      capGet.bitmap[1] &= ~LLDP_MED_CAP_CAPABILITIES_BITMASK;
    }

  /* set the value from application */
    owa.l7rc =
       usmDbLldpXMedPortConfigTLVsEnabledSet ( keyPortNumValue,
                                            &capGet);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


xLibRC_t fpObjGet_SwitchingLLDPXMEDInterfaceConfig_LLDPMedCapNetworkPolicy (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPortNumValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibU32_t val= L7_DISABLE;
  lldpXMedCapabilities_t capGet;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PortNum */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDInterfaceConfig_LldpXMedInterface,
                          (xLibU8_t *) & keyPortNumValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPortNumValue, kwa.len);

  /* get the value from application */
  if( keyPortNumValue != L7_ALL_INTERFACES)
  {

    owa.l7rc =
    usmDbLldpXMedPortConfigTLVsEnabledGet ( keyPortNumValue,
                                            &capGet);
    if (owa.l7rc != L7_SUCCESS)
    {  
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }

  /* return the object value */
    if (capGet.bitmap[1] & LLDP_MED_CAP_NETWORKPOLICY_BITMASK)
    {
      val = L7_ENABLE;
    }
    else
    {
      val = L7_DISABLE;
    }
  }
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &val,
                           sizeof (val));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

xLibRC_t fpObjSet_SwitchingLLDPXMEDInterfaceConfig_LLDPMedCapNetworkPolicy (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPortNumValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t val;
  lldpXMedCapabilities_t capGet;
  xLibU32_t intIfNum;
  FPOBJ_TRACE_ENTER (bufp);
  memset(&capGet, 0, sizeof(lldpXMedCapabilities_t));

  /* retrieve object:  */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & val, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &val, owa.len);

  /* retrieve key: PortNum */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDInterfaceConfig_LldpXMedInterface,
                          (xLibU8_t *) & keyPortNumValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPortNumValue, kwa.len);


  if( keyPortNumValue == L7_ALL_INTERFACES )
  {
    intIfNum = 0;
    usmDbLldpValidIntfFirstGet(&intIfNum);
    do
    {
      owa.l7rc =
      usmDbLldpXMedPortConfigTLVsEnabledGet ( intIfNum,
                                            &capGet);
      if (owa.l7rc != L7_SUCCESS)
      {
        owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
      }

      if(val == L7_ENABLE)
      {
        capGet.bitmap[1] |= LLDP_MED_CAP_NETWORKPOLICY_BITMASK;
      }
      else
      {
        capGet.bitmap[1] &= ~LLDP_MED_CAP_NETWORKPOLICY_BITMASK;
      }

  /* set the value from application */
      owa.l7rc =
        usmDbLldpXMedPortConfigTLVsEnabledSet ( intIfNum,
                                            &capGet);

      if (owa.l7rc != L7_SUCCESS)
      {
        owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
      }
    }while(usmDbLldpValidIntfNextGet(intIfNum, &intIfNum) == L7_SUCCESS );

  }

  else
  {

  /* get the value from application */
    owa.l7rc =
        usmDbLldpXMedPortConfigTLVsEnabledGet ( keyPortNumValue,
                                            &capGet);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }  

    if(val == L7_ENABLE)
    {
      capGet.bitmap[1] |= LLDP_MED_CAP_NETWORKPOLICY_BITMASK;
    }
    else
    {
      capGet.bitmap[1] &= ~LLDP_MED_CAP_NETWORKPOLICY_BITMASK;
    }

  /* set the value from application */
    owa.l7rc =
      usmDbLldpXMedPortConfigTLVsEnabledSet ( keyPortNumValue,
                                            &capGet);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }

  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


xLibRC_t fpObjGet_SwitchingLLDPXMEDInterfaceConfig_LLDPMedCapLocationIdentification (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPortNumValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibU32_t val= L7_DISABLE;
  lldpXMedCapabilities_t capGet;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PortNum */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDInterfaceConfig_LldpXMedInterface,
                          (xLibU8_t *) & keyPortNumValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPortNumValue, kwa.len);

  /* get the value from application */

  if( keyPortNumValue != L7_ALL_INTERFACES)
  {

     owa.l7rc =
    usmDbLldpXMedPortConfigTLVsEnabledGet ( keyPortNumValue,
                                            &capGet);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }

  /* return the object value */
    if (capGet.bitmap[1] & LLDP_MED_CAP_LOCATION_BITMASK)
    {
      val = L7_ENABLE;
    }
    else
    {
      val = L7_DISABLE;
    }   
  }
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &val,
                           sizeof (val));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

xLibRC_t fpObjSet_SwitchingLLDPXMEDInterfaceConfig_LLDPMedCapLocationIdentification (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPortNumValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t val;
  xLibU32_t intIfNum;
  lldpXMedCapabilities_t capGet;
  FPOBJ_TRACE_ENTER (bufp);
  memset(&capGet, 0, sizeof(lldpXMedCapabilities_t));

  /* retrieve object:  */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & val, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &val, owa.len);

  /* retrieve key: PortNum */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDInterfaceConfig_LldpXMedInterface,
                          (xLibU8_t *) & keyPortNumValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPortNumValue, kwa.len);

  if( keyPortNumValue == L7_ALL_INTERFACES )
  {
    intIfNum = 0;
    usmDbLldpValidIntfFirstGet(&intIfNum);
    do
    {
         /* get the value from application */
      owa.l7rc =
            usmDbLldpXMedPortConfigTLVsEnabledGet ( intIfNum,
                                            &capGet);
      if (owa.l7rc != L7_SUCCESS)
      {
        owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
      }

      if(val == L7_ENABLE)
      {
        capGet.bitmap[1] |= LLDP_MED_CAP_LOCATION_BITMASK;
      }
      else
      {
        capGet.bitmap[1] &= ~LLDP_MED_CAP_LOCATION_BITMASK;
      }  

  /* set the value from application */
      owa.l7rc =
        usmDbLldpXMedPortConfigTLVsEnabledSet ( intIfNum,
                                            &capGet);
      if (owa.l7rc != L7_SUCCESS)
      {
        owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
      }

    } while( usmDbLldpValidIntfNextGet(intIfNum, &intIfNum) == L7_SUCCESS );
 }
 else
 {

  /* get the value from application */
   owa.l7rc =
     usmDbLldpXMedPortConfigTLVsEnabledGet ( keyPortNumValue,
                                            &capGet);
   if (owa.l7rc != L7_SUCCESS)
   {
     owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
     FPOBJ_TRACE_EXIT (bufp, owa);
     return owa.rc;
   }

   if(val == L7_ENABLE)
   {
     capGet.bitmap[1] |= LLDP_MED_CAP_LOCATION_BITMASK;
   }
   else
   {
     capGet.bitmap[1] &= ~LLDP_MED_CAP_LOCATION_BITMASK;
   }

  /* set the value from application */
   owa.l7rc =
     usmDbLldpXMedPortConfigTLVsEnabledSet ( keyPortNumValue,
                                            &capGet);
   if (owa.l7rc != L7_SUCCESS)
   {
     owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
     FPOBJ_TRACE_EXIT (bufp, owa);
     return owa.rc;
   }
 }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


xLibRC_t fpObjGet_SwitchingLLDPXMEDInterfaceConfig_LLDPMedCapExtPSE (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPortNumValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibU32_t val=L7_DISABLE;
  lldpXMedCapabilities_t capGet;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PortNum */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDInterfaceConfig_LldpXMedInterface,
                          (xLibU8_t *) & keyPortNumValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPortNumValue, kwa.len);

  if( keyPortNumValue != L7_ALL_INTERFACES)
  {
  /* get the value from application */
    owa.l7rc =
      usmDbLldpXMedPortConfigTLVsEnabledGet ( keyPortNumValue,
                                            &capGet);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }

  /* return the object value */
    if (capGet.bitmap[1] & LLDP_MED_CAP_EXT_PSE_BITMASK)
    {
      val = L7_ENABLE;
    }
    else
    {
      val = L7_DISABLE;
    }

  }
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &val,
                           sizeof (val));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

xLibRC_t fpObjSet_SwitchingLLDPXMEDInterfaceConfig_LLDPMedCapExtPSE (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPortNumValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t val;
  xLibU32_t intIfNum;
  lldpXMedCapabilities_t capGet;
  FPOBJ_TRACE_ENTER (bufp);
  memset(&capGet, 0, sizeof(lldpXMedCapabilities_t));

  /* retrieve object:  */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & val, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &val, owa.len);

  /* retrieve key: PortNum */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDInterfaceConfig_LldpXMedInterface,
                          (xLibU8_t *) & keyPortNumValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPortNumValue, kwa.len);

  /* get the value from application */
 
  if( keyPortNumValue == L7_ALL_INTERFACES )
  {
    intIfNum = 0;
    usmDbLldpValidIntfFirstGet(&intIfNum);
    do
    {
      owa.l7rc =
      usmDbLldpXMedPortConfigTLVsEnabledGet ( intIfNum,
                                            &capGet);
      if (owa.l7rc != L7_SUCCESS)
      {
        owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
      }

      if(val == L7_ENABLE)
      {
        capGet.bitmap[1] |= LLDP_MED_CAP_EXT_PSE_BITMASK;
      }
      else
      {
        capGet.bitmap[1] &= ~LLDP_MED_CAP_EXT_PSE_BITMASK;
      }

  /* set the value from application */
      owa.l7rc =
        usmDbLldpXMedPortConfigTLVsEnabledSet ( intIfNum,
                                            &capGet);
      if (owa.l7rc != L7_SUCCESS)
      {
        owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
      }

   }while( usmDbLldpValidIntfNextGet(intIfNum, &intIfNum) == L7_SUCCESS );

  }
  else
  {
 
    owa.l7rc =
      usmDbLldpXMedPortConfigTLVsEnabledGet ( keyPortNumValue,
                                            &capGet);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }

    if(val == L7_ENABLE)
    {
      capGet.bitmap[1] |= LLDP_MED_CAP_EXT_PSE_BITMASK;
    }
    else
    {
      capGet.bitmap[1] &= ~LLDP_MED_CAP_EXT_PSE_BITMASK;
    }

  /* set the value from application */
  owa.l7rc =
    usmDbLldpXMedPortConfigTLVsEnabledSet ( keyPortNumValue,
                                            &capGet);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

 }

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


xLibRC_t fpObjGet_SwitchingLLDPXMEDInterfaceConfig_LLDPMedCapExtPD (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPortNumValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibU32_t val=L7_DISABLE ;
  lldpXMedCapabilities_t capGet;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PortNum */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDInterfaceConfig_LldpXMedInterface,
                          (xLibU8_t *) & keyPortNumValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPortNumValue, kwa.len);

  /* get the value from application */

  if( keyPortNumValue != L7_ALL_INTERFACES)
  {
    owa.l7rc =
      usmDbLldpXMedPortConfigTLVsEnabledGet ( keyPortNumValue,
                                            &capGet);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }

  /* return the object value */
    if (capGet.bitmap[1] & LLDP_MED_CAP_EXT_PD_BITMASK)
    {
      val = L7_ENABLE;
    }
    else
    {
      val = L7_DISABLE;
    }
  }
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &val,
                           sizeof (val));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

xLibRC_t fpObjSet_SwitchingLLDPXMEDInterfaceConfig_LLDPMedCapExtPD (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPortNumValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t val;
  xLibU32_t intIfNum;
  lldpXMedCapabilities_t capGet;
  FPOBJ_TRACE_ENTER (bufp);
  memset(&capGet, 0, sizeof(lldpXMedCapabilities_t));

  /* retrieve object:  */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & val, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &val, owa.len);

  /* retrieve key: PortNum */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDInterfaceConfig_LldpXMedInterface,
                          (xLibU8_t *) & keyPortNumValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPortNumValue, kwa.len);

  /* get the value from application */

  if( keyPortNumValue == L7_ALL_INTERFACES )
  {
    intIfNum = 0;
    usmDbLldpValidIntfFirstGet(&intIfNum);
    do
    {
      owa.l7rc =
      usmDbLldpXMedPortConfigTLVsEnabledGet ( intIfNum,
                                            &capGet);
      if (owa.l7rc != L7_SUCCESS)
      {
        owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
      }

      if(val == L7_ENABLE)
      {
        capGet.bitmap[1] |= LLDP_MED_CAP_EXT_PD_BITMASK;
      }
      else
      {
        capGet.bitmap[1] &= ~LLDP_MED_CAP_EXT_PD_BITMASK;
      }

  /* set the value from application */
      owa.l7rc =
         usmDbLldpXMedPortConfigTLVsEnabledSet ( intIfNum,
                                            &capGet);
      if (owa.l7rc != L7_SUCCESS)
      {
        owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
      }
    } while( usmDbLldpValidIntfNextGet(intIfNum, &intIfNum) == L7_SUCCESS) ;

  }
  else
  {
    owa.l7rc =
      usmDbLldpXMedPortConfigTLVsEnabledGet ( keyPortNumValue,
                                             &capGet);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }

    if(val == L7_ENABLE)
    {
      capGet.bitmap[1] |= LLDP_MED_CAP_EXT_PD_BITMASK;
    }
    else
    {
      capGet.bitmap[1] &= ~LLDP_MED_CAP_EXT_PD_BITMASK;
    }

  /* set the value from application */
    owa.l7rc =
      usmDbLldpXMedPortConfigTLVsEnabledSet ( keyPortNumValue,
                                            &capGet);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


xLibRC_t fpObjGet_SwitchingLLDPXMEDInterfaceConfig_LLDPMedCapInventory (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPortNumValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibU32_t val = L7_DISABLE;
  lldpXMedCapabilities_t capGet;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PortNum */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDInterfaceConfig_LldpXMedInterface,
                          (xLibU8_t *) & keyPortNumValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPortNumValue, kwa.len);

  /* get the value from application */
  if( keyPortNumValue != L7_ALL_INTERFACES)
  {
    owa.l7rc =
      usmDbLldpXMedPortConfigTLVsEnabledGet ( keyPortNumValue,
                                            &capGet);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }

  /* return the object value */
    if (capGet.bitmap[1] & LLDP_MED_CAP_INVENTORY_BITMASK)
    {
      val = L7_ENABLE;
    }
    else
    {
      val = L7_DISABLE;
    }
  }
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &val,
                           sizeof (val));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

xLibRC_t fpObjSet_SwitchingLLDPXMEDInterfaceConfig_LLDPMedCapInventory (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPortNumValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t val;
  xLibU32_t intIfNum;
  lldpXMedCapabilities_t capGet;
  FPOBJ_TRACE_ENTER (bufp);
  memset(&capGet, 0, sizeof(lldpXMedCapabilities_t));

  /* retrieve object:  */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & val, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &val, owa.len);

  /* retrieve key: PortNum */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDInterfaceConfig_LldpXMedInterface,
                          (xLibU8_t *) & keyPortNumValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPortNumValue, kwa.len);

  /* get the value from application */


  if( keyPortNumValue == L7_ALL_INTERFACES )
  {
    intIfNum = 0;
    usmDbLldpValidIntfFirstGet(&intIfNum);
    do
    {
      owa.l7rc =
      usmDbLldpXMedPortConfigTLVsEnabledGet ( intIfNum,
                                            &capGet);
     if (owa.l7rc != L7_SUCCESS)
     {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
     }

     if(val == L7_ENABLE)
     {
       capGet.bitmap[1] |= LLDP_MED_CAP_INVENTORY_BITMASK;
     }
     else
     {
       capGet.bitmap[1] &= ~LLDP_MED_CAP_INVENTORY_BITMASK;
     }

  /* set the value from application */
     owa.l7rc =
      usmDbLldpXMedPortConfigTLVsEnabledSet ( intIfNum,
                                            &capGet);
      if (owa.l7rc != L7_SUCCESS)
      {
        owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
      }
    } while( usmDbLldpValidIntfNextGet(intIfNum, &intIfNum) == L7_SUCCESS);

  }
  else
  {
    owa.l7rc =
       usmDbLldpXMedPortConfigTLVsEnabledGet ( keyPortNumValue,
                                            &capGet);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }

    if(val == L7_ENABLE)
    {
      capGet.bitmap[1] |= LLDP_MED_CAP_INVENTORY_BITMASK;
    }
    else
    {
      capGet.bitmap[1] &= ~LLDP_MED_CAP_INVENTORY_BITMASK;
    } 

  /* set the value from application */
    owa.l7rc =
      usmDbLldpXMedPortConfigTLVsEnabledSet ( keyPortNumValue,
                                            &capGet);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }

   }
   FPOBJ_TRACE_EXIT (bufp, owa);
   return owa.rc;
}



/*******************************************************************************
* @function fpObjGet_SwitchingLLDPXMEDInterfaceConfig_LinkStatus
*
* @purpose Get 'LinkStatus'
*
* @description [LinkStatus] Indicates whether the Link is up or down.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPXMEDInterfaceConfig_LinkStatus (void *wap, void *bufp)
{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  L7_uint32 link_state;
  xLibStr256_t objLinkStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDInterfaceConfig_LldpXMedInterface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIfOperStatusGet(L7_UNIT_CURRENT, keyInterfaceValue, &link_state);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if ( link_state == L7_UP )
  {
    strcpy(objLinkStatusValue,"Up");
  }
  else
  {
    strcpy(objLinkStatusValue,"Down");
  }
  /* return the object value: LinkStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objLinkStatusValue, strlen (objLinkStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}



#if 0

/*******************************************************************************
* @function fpObjGet_SwitchingLLDPXMedConfig_PortCapSupported
*
* @purpose Get 'PortCapSupported'
*
* @description [PortCapSupported]: The bitmap includes the MED organizationally
*              defined set of LLDP TLVs whose transmission is possible
*              for the respective port on the LLDP agent of the device.
*              Each bit in the bitmap corresponds to an LLDP-MED subtype
*              associated with a specific TIA TR41.4 M 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPXMedConfig_PortCapSupported (void *wap,
                                                            void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPortNumValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objPortCapSupportedValue;
  lldpXMedCapabilities_t capSup;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PortNum */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPXMEDInterfaceConfig_LldpXMedInterface,
                          (xLibU8_t *) & keyPortNumValue, &kwa.len);
  if (kwa.rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPortNumValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbLldpXMedPortCapSupportedGet ( keyPortNumValue,
                                                &capSup);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: PortCapSupported */
  if (capSup.bitmap[1] & LLDP_MED_CAP_CAPABILITIES_BITMASK)
  {
    strcpy(objPortCapSupportedValue, usmDbLldpXMedMediaCapabilitiesString(0));
  }
  if (capSup.bitmap[1] & LLDP_MED_CAP_NETWORKPOLICY_BITMASK)
  {
    strcpy(objPortCapSupportedValue, usmDbLldpXMedMediaCapabilitiesString(1));
  }
  if (capSup.bitmap[1] & LLDP_MED_CAP_LOCATION_BITMASK)
  {
    strcpy(objPortCapSupportedValue, usmDbLldpXMedMediaCapabilitiesString(2));
  } 
  if (capSup.bitmap[1] & LLDP_MED_CAP_EXT_PSE_BITMASK)
  {
    strcpy(objPortCapSupportedValue, usmDbLldpXMedMediaCapabilitiesString(3));
  }
  if (capSup.bitmap[1] & LLDP_MED_CAP_EXT_PD_BITMASK)
  {
    strcpy(objPortCapSupportedValue, usmDbLldpXMedMediaCapabilitiesString(4));
  }
  if (capSup.bitmap[1] & LLDP_MED_CAP_INVENTORY_BITMASK)
  {
    strcpy(objPortCapSupportedValue, usmDbLldpXMedMediaCapabilitiesString(5));
  }
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objPortCapSupportedValue,
                           strlen (objPortCapSupportedValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
#endif

/*STARTTTTTTTTTTTTTTTTTTTTT HERE */

/*******************************************************************************
* @function fpObjSet_SwitchingLLDPXMEDInterfaceConfig_LldpXMedPortAdminMode
*
* @purpose Set 'LldpXMedPortAdminMode'
*
* @description [LldpXMedPortAdminMode]: The MED admin mode for this interface
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingLLDPXMEDGlobalConfig_LldpXMedPortAdminMode(void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objLldpXMedPortAdminModeValue;
  xLibU32_t intIfNum;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: NotifEnabled */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objLldpXMedPortAdminModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objLldpXMedPortAdminModeValue, owa.len);


  /* set the value in application */

    intIfNum = 0;
    usmDbLldpValidIntfFirstGet(&intIfNum);
    do
    {
      owa.l7rc =
          usmDbLldpXMedPortAdminModeSet (intIfNum,
                                           objLldpXMedPortAdminModeValue);
      if (owa.l7rc != L7_SUCCESS)
      {
        owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
      }
    } while( usmDbLldpValidIntfNextGet(intIfNum, &intIfNum) == L7_SUCCESS);

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingLLDPXMEDInterfaceConfig_LldpXMedPortAdminMode
*
* @purpose Set 'LldpXMedPortAdminMode'
*
* @description [LldpXMedPortAdminMode]: The MED admin mode for this interface
*
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPXMEDGlobalConfig_LldpXMedPortAdminMode(void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objLldpXMedPortAdminModeValue,currLldpXMedPortAdminModeValue;
  xLibU32_t intIfNum;
  FPOBJ_TRACE_ENTER (bufp);


  /* set the value in application */

    intIfNum = 0;
    usmDbLldpValidIntfFirstGet(&intIfNum);
    do
    {
      owa.l7rc =
          usmDbLldpXMedPortAdminModeGet (intIfNum,
                                           &objLldpXMedPortAdminModeValue);
      if (owa.l7rc != L7_SUCCESS)
      {
        owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
      }
    } while(0);

  while( usmDbLldpValidIntfNextGet(intIfNum, &intIfNum) == L7_SUCCESS)
  {
    owa.l7rc =usmDbLldpXMedPortAdminModeGet (intIfNum,
                                           &currLldpXMedPortAdminModeValue);
    if (owa.l7rc != L7_SUCCESS)
    {
        owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
    }

    if(currLldpXMedPortAdminModeValue!=objLldpXMedPortAdminModeValue)
    {
      objLldpXMedPortAdminModeValue = L7_DISABLE;
      break;
    }
      objLldpXMedPortAdminModeValue=currLldpXMedPortAdminModeValue;
  }

  FPOBJ_TRACE_EXIT (bufp, owa);
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objLldpXMedPortAdminModeValue,
                           sizeof (objLldpXMedPortAdminModeValue));

  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingLLDPXMedConfig_NotifEnabled
*
* @purpose Set 'NotifEnabled'
*
* @description [NotifEnabled]: A value of 'true(1)' enables sending the topology
*              change traps on this port. A value of 'false(2)' disables
*              sending the topology change traps on this port. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingLLDPXMEDGlobalConfig_NotifEnabled (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objNotifEnabledValue;
  xLibU32_t intIfNum;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: NotifEnabled */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objNotifEnabledValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objNotifEnabledValue, owa.len);


  /* set the value in application */

  intIfNum = 0;
  usmDbLldpValidIntfFirstGet(&intIfNum);
  do 
  {
    owa.l7rc =
         usmDbLldpXMedPortConfigNotifEnableSet (intIfNum,
                                           objNotifEnabledValue);
    if (owa.l7rc != L7_SUCCESS)
    {  
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }

  }while( usmDbLldpValidIntfNextGet(intIfNum, &intIfNum) == L7_SUCCESS) ;

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
* @function fpObjGet_SwitchingLLDPXMedConfig_NotifEnabled
*
* @purpose Set 'NotifEnabled'
*
* @description [NotifEnabled]: A value of 'true(1)' enables sending the topology
*              change traps on this port. A value of 'false(2)' disables
*              sending the topology change traps on this port.
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPXMEDGlobalConfig_NotifEnabled (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objNotifEnabledValue,currNotifEnabledValue;
  xLibU32_t intIfNum;
  FPOBJ_TRACE_ENTER (bufp);


  intIfNum = 0;
  usmDbLldpValidIntfFirstGet(&intIfNum);
  do
  {
    owa.l7rc =
         usmDbLldpXMedPortConfigNotifEnableGet (intIfNum,
                                           &objNotifEnabledValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }

  }while(0) ;

  while( usmDbLldpValidIntfNextGet(intIfNum, &intIfNum) == L7_SUCCESS)
  {
    owa.l7rc =
         usmDbLldpXMedPortConfigNotifEnableGet (intIfNum,
                                           &currNotifEnabledValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    if(currNotifEnabledValue!=objNotifEnabledValue)
    {
      objNotifEnabledValue=L7_DISABLE;
      break;
    }
      objNotifEnabledValue=currNotifEnabledValue;
   }
  FPOBJ_TRACE_EXIT (bufp, owa);
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objNotifEnabledValue,
                           sizeof (objNotifEnabledValue));

  return owa.rc;
}


xLibRC_t fpObjSet_SwitchingLLDPXMEDGlobalConfig_LLDPMedCapCapabilities (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t val;
  xLibU32_t intIfNum;
  lldpXMedCapabilities_t capGet;
  FPOBJ_TRACE_ENTER (bufp);
  
  memset(&capGet, 0, sizeof(lldpXMedCapabilities_t));

  /* retrieve object:  */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & val, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &val, owa.len);

  intIfNum = 0;
  usmDbLldpValidIntfFirstGet(&intIfNum);
  do 
  {
    owa.l7rc =
      usmDbLldpXMedPortConfigTLVsEnabledGet ( intIfNum,
                                            &capGet);
    if (owa.l7rc != L7_SUCCESS)
    {  
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    if(val == L7_ENABLE)
    {
      capGet.bitmap[1] |= LLDP_MED_CAP_CAPABILITIES_BITMASK;
    }
    else
    {
      capGet.bitmap[1] &= ~LLDP_MED_CAP_CAPABILITIES_BITMASK;
    }

  /* set the value from application */
    owa.l7rc =
       usmDbLldpXMedPortConfigTLVsEnabledSet ( intIfNum,
                                          &capGet);
    if (owa.l7rc != L7_SUCCESS)
     {
       owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
       FPOBJ_TRACE_EXIT (bufp, owa);
       return owa.rc;
     }
  }while( usmDbLldpValidIntfNextGet(intIfNum, &intIfNum) == L7_SUCCESS);

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


xLibRC_t fpObjGet_SwitchingLLDPXMEDGlobalConfig_LLDPMedCapCapabilities (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t val,currval;
  xLibU32_t intIfNum;
  lldpXMedCapabilities_t capGet;
  FPOBJ_TRACE_ENTER (bufp);

  memset(&capGet, 0, sizeof(lldpXMedCapabilities_t));
  /* retrieve object:  */
  intIfNum = 0;
  usmDbLldpValidIntfFirstGet(&intIfNum);
  do
  {
    owa.l7rc =
      usmDbLldpXMedPortConfigTLVsEnabledGet ( intIfNum,
                                            &capGet);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    if((capGet.bitmap[1]&LLDP_MED_CAP_CAPABILITIES_BITMASK)==LLDP_MED_CAP_CAPABILITIES_BITMASK)
    {
      val=L7_ENABLE;
    }
    else
    {
      val=L7_DISABLE;
    }

  }while(0);
  while(usmDbLldpValidIntfNextGet(intIfNum, &intIfNum) == L7_SUCCESS)
  {
    owa.l7rc =
      usmDbLldpXMedPortConfigTLVsEnabledGet ( intIfNum,
                                            &capGet);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    if((capGet.bitmap[1]&LLDP_MED_CAP_CAPABILITIES_BITMASK)==LLDP_MED_CAP_CAPABILITIES_BITMASK)
    {
      currval=L7_ENABLE;
    }
    else
    {
      currval=L7_DISABLE;
    }
    if(currval!=val)
    {
       val=L7_DISABLE;
       break;
    }
      val=currval;
  }

  FPOBJ_TRACE_EXIT (bufp, owa);
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &val,
                           sizeof (val));

  return owa.rc;
}

xLibRC_t fpObjSet_SwitchingLLDPXMEDGlobalConfig_LLDPMedCapNetworkPolicy (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t val;
  lldpXMedCapabilities_t capGet;
  xLibU32_t intIfNum;
  FPOBJ_TRACE_ENTER (bufp);
  memset(&capGet, 0, sizeof(lldpXMedCapabilities_t));

  /* retrieve object:  */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & val, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &val, owa.len);

  intIfNum = 0;
  usmDbLldpValidIntfFirstGet(&intIfNum);
  do
  {
    owa.l7rc =
      usmDbLldpXMedPortConfigTLVsEnabledGet ( intIfNum,
                                            &capGet);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }

    if(val == L7_ENABLE)
    {
      capGet.bitmap[1] |= LLDP_MED_CAP_NETWORKPOLICY_BITMASK;
    }
    else
    {
      capGet.bitmap[1] &= ~LLDP_MED_CAP_NETWORKPOLICY_BITMASK;
    }

  /* set the value from application */
    owa.l7rc =
        usmDbLldpXMedPortConfigTLVsEnabledSet ( intIfNum,
                                            &capGet);

    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }while(usmDbLldpValidIntfNextGet(intIfNum, &intIfNum) == L7_SUCCESS );

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

xLibRC_t fpObjGet_SwitchingLLDPXMEDGlobalConfig_LLDPMedCapNetworkPolicy (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t val,currval;
  lldpXMedCapabilities_t capGet;
  xLibU32_t intIfNum;
  FPOBJ_TRACE_ENTER (bufp);
  memset(&capGet, 0, sizeof(lldpXMedCapabilities_t));


  intIfNum = 0;
  usmDbLldpValidIntfFirstGet(&intIfNum);
  do
  {
    owa.l7rc =
      usmDbLldpXMedPortConfigTLVsEnabledGet ( intIfNum,
                                            &capGet);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    if((capGet.bitmap[1]&LLDP_MED_CAP_NETWORKPOLICY_BITMASK)==LLDP_MED_CAP_NETWORKPOLICY_BITMASK)
    {
      val=L7_ENABLE;
    }
    else
    {
      val=L7_DISABLE;
    }

  }while(0);

  while(usmDbLldpValidIntfNextGet(intIfNum, &intIfNum) == L7_SUCCESS )
  {
    owa.l7rc =
      usmDbLldpXMedPortConfigTLVsEnabledGet ( intIfNum,
                                            &capGet);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    if((capGet.bitmap[1]&LLDP_MED_CAP_NETWORKPOLICY_BITMASK)==LLDP_MED_CAP_NETWORKPOLICY_BITMASK)
    {
      currval=L7_ENABLE;
    }
    else
    {
      currval=L7_DISABLE;
    }
    if(currval!=val)
    {
      val=L7_DISABLE;
      break;
    }
      val=currval;

   }
  FPOBJ_TRACE_EXIT (bufp, owa);
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &val,
                           sizeof (val));

  return owa.rc;
}


xLibRC_t fpObjSet_SwitchingLLDPXMEDGlobalConfig_LLDPMedCapLocationIdentification (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t val;
  xLibU32_t intIfNum;
  lldpXMedCapabilities_t capGet;
  FPOBJ_TRACE_ENTER (bufp);
  memset(&capGet, 0, sizeof(lldpXMedCapabilities_t));

  /* retrieve object:  */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & val, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &val, owa.len);


  intIfNum = 0;
  usmDbLldpValidIntfFirstGet(&intIfNum);
  do
  {
         /* get the value from application */
    owa.l7rc =
            usmDbLldpXMedPortConfigTLVsEnabledGet ( intIfNum,
                                            &capGet);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }

    if(val == L7_ENABLE)
    {
      capGet.bitmap[1] |= LLDP_MED_CAP_LOCATION_BITMASK;
    }
    else
    {
      capGet.bitmap[1] &= ~LLDP_MED_CAP_LOCATION_BITMASK;
    }  

  /* set the value from application */
    owa.l7rc =
        usmDbLldpXMedPortConfigTLVsEnabledSet ( intIfNum,
                                            &capGet);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  } while( usmDbLldpValidIntfNextGet(intIfNum, &intIfNum) == L7_SUCCESS );

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

xLibRC_t fpObjGet_SwitchingLLDPXMEDGlobalConfig_LLDPMedCapLocationIdentification (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t val,currval;
  xLibU32_t intIfNum;
  lldpXMedCapabilities_t capGet;
  FPOBJ_TRACE_ENTER (bufp);
  memset(&capGet, 0, sizeof(lldpXMedCapabilities_t));

  intIfNum = 0;
  usmDbLldpValidIntfFirstGet(&intIfNum);
  do
  {
         /* get the value from application */
    owa.l7rc =
            usmDbLldpXMedPortConfigTLVsEnabledGet ( intIfNum,
                                            &capGet);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }

    if((capGet.bitmap[1]&LLDP_MED_CAP_LOCATION_BITMASK)==LLDP_MED_CAP_LOCATION_BITMASK)
    {
      val=L7_ENABLE;
    }
    else
    {
      val=L7_DISABLE;
    }

  } while(0);

  while(usmDbLldpValidIntfNextGet(intIfNum, &intIfNum) == L7_SUCCESS )
  {
    owa.l7rc =
            usmDbLldpXMedPortConfigTLVsEnabledGet ( intIfNum,
                                            &capGet);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }

    if((capGet.bitmap[1]&LLDP_MED_CAP_LOCATION_BITMASK)==LLDP_MED_CAP_LOCATION_BITMASK)
    {
      currval=L7_ENABLE;
    }
    else
    {
      currval=L7_DISABLE;
    }
    if(currval!=val)
    {
      val=L7_DISABLE;
      break;
    }
      val=currval;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &val,
                           sizeof (val));
  return owa.rc;
}

xLibRC_t fpObjSet_SwitchingLLDPXMEDGlobalConfig_LLDPMedCapExtPSE (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t val;
  xLibU32_t intIfNum;
  lldpXMedCapabilities_t capGet;
  FPOBJ_TRACE_ENTER (bufp);
  memset(&capGet, 0, sizeof(lldpXMedCapabilities_t));

  /* retrieve object:  */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & val, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &val, owa.len);

  /* get the value from application */
 
  intIfNum = 0;
  usmDbLldpValidIntfFirstGet(&intIfNum);
  do
  {
    owa.l7rc =
    usmDbLldpXMedPortConfigTLVsEnabledGet ( intIfNum,
                                            &capGet);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }

    if(val == L7_ENABLE)
    {
      capGet.bitmap[1] |= LLDP_MED_CAP_EXT_PSE_BITMASK;
    }
    else
    {
      capGet.bitmap[1] &= ~LLDP_MED_CAP_EXT_PSE_BITMASK;
    }

  /* set the value from application */
    owa.l7rc =
        usmDbLldpXMedPortConfigTLVsEnabledSet ( intIfNum,
                                            &capGet);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }

   }while( usmDbLldpValidIntfNextGet(intIfNum, &intIfNum) == L7_SUCCESS );

   FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

xLibRC_t fpObjGet_SwitchingLLDPXMEDGlobalConfig_LLDPMedCapExtPSE (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t val,currval;
  xLibU32_t intIfNum;
  lldpXMedCapabilities_t capGet;
  FPOBJ_TRACE_ENTER (bufp);
  memset(&capGet, 0, sizeof(lldpXMedCapabilities_t));


  /* get the value from application */

  intIfNum = 0;
  usmDbLldpValidIntfFirstGet(&intIfNum);
  do
  {
    owa.l7rc =
    usmDbLldpXMedPortConfigTLVsEnabledGet ( intIfNum,
                                            &capGet);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    if((capGet.bitmap[1]&LLDP_MED_CAP_EXT_PSE_BITMASK)==LLDP_MED_CAP_EXT_PSE_BITMASK)
    {
      val=L7_ENABLE;
    }
    else
    {
      val=L7_DISABLE;
    }
   }while(0);

  while( usmDbLldpValidIntfNextGet(intIfNum, &intIfNum) == L7_SUCCESS)
  {
       owa.l7rc =
    usmDbLldpXMedPortConfigTLVsEnabledGet ( intIfNum,
                                            &capGet);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    if((capGet.bitmap[1]&LLDP_MED_CAP_EXT_PSE_BITMASK)==LLDP_MED_CAP_EXT_PSE_BITMASK)
    {
      currval=L7_ENABLE;
    }
    else
    {
      currval=L7_DISABLE;
    }
    if(currval!=val)
    {
      val=L7_DISABLE;
      break;
    }
      val=currval;
  }

  FPOBJ_TRACE_EXIT (bufp, owa);
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &val,
                           sizeof (val));
  return owa.rc;
}

xLibRC_t fpObjSet_SwitchingLLDPXMEDGlobalConfig_LLDPMedCapExtPD (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t val;
  xLibU32_t intIfNum;
  lldpXMedCapabilities_t capGet;
  FPOBJ_TRACE_ENTER (bufp);
  memset(&capGet, 0, sizeof(lldpXMedCapabilities_t));

  /* retrieve object:  */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & val, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &val, owa.len);


  /* get the value from application */

  intIfNum = 0;
  usmDbLldpValidIntfFirstGet(&intIfNum);
  do
  {
    owa.l7rc =
    usmDbLldpXMedPortConfigTLVsEnabledGet ( intIfNum,
                                            &capGet);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }

    if(val == L7_ENABLE)
    {
      capGet.bitmap[1] |= LLDP_MED_CAP_EXT_PD_BITMASK;
    }
    else
    {
      capGet.bitmap[1] &= ~LLDP_MED_CAP_EXT_PD_BITMASK;
    }

  /* set the value from application */
    owa.l7rc =
         usmDbLldpXMedPortConfigTLVsEnabledSet ( intIfNum,
                                            &capGet);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  } while( usmDbLldpValidIntfNextGet(intIfNum, &intIfNum) == L7_SUCCESS) ;

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

xLibRC_t fpObjGet_SwitchingLLDPXMEDGlobalConfig_LLDPMedCapExtPD (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t val,currval;
  xLibU32_t intIfNum;
  lldpXMedCapabilities_t capGet;
  FPOBJ_TRACE_ENTER (bufp);
  memset(&capGet, 0, sizeof(lldpXMedCapabilities_t));


  /* get the value from application */

  intIfNum = 0;
  usmDbLldpValidIntfFirstGet(&intIfNum);
  do
  {
    owa.l7rc =
    usmDbLldpXMedPortConfigTLVsEnabledGet ( intIfNum,
                                            &capGet);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    if((capGet.bitmap[1]&LLDP_MED_CAP_EXT_PD_BITMASK)==LLDP_MED_CAP_EXT_PD_BITMASK)
    {
      val=L7_ENABLE;
    }
    else
    {
      val=L7_DISABLE;
    }


  } while(0);

  while( usmDbLldpValidIntfNextGet(intIfNum, &intIfNum) == L7_SUCCESS)
  {
      owa.l7rc =
    usmDbLldpXMedPortConfigTLVsEnabledGet ( intIfNum,
                                            &capGet);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    if((capGet.bitmap[1]&LLDP_MED_CAP_EXT_PD_BITMASK)==LLDP_MED_CAP_EXT_PD_BITMASK)
    {
      currval=L7_ENABLE;
    }
    else
    {
      currval=L7_DISABLE;
    }

    if(currval!=val)
    {
      val=L7_DISABLE;
      break;
    }
      val=currval;
   }
  FPOBJ_TRACE_EXIT (bufp, owa);
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &val,
                           sizeof (val));
  return owa.rc;
}

xLibRC_t fpObjSet_SwitchingLLDPXMEDGlobalConfig_LLDPMedCapInventory (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t val;
  xLibU32_t intIfNum;
  lldpXMedCapabilities_t capGet;
  FPOBJ_TRACE_ENTER (bufp);
  memset(&capGet, 0, sizeof(lldpXMedCapabilities_t));

  /* retrieve object:  */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & val, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &val, owa.len);


  /* get the value from application */

  intIfNum = 0;
  usmDbLldpValidIntfFirstGet(&intIfNum);
  do
  {
    owa.l7rc =
     usmDbLldpXMedPortConfigTLVsEnabledGet ( intIfNum,
                                            &capGet);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }

    if(val == L7_ENABLE)
    {
       capGet.bitmap[1] |= LLDP_MED_CAP_INVENTORY_BITMASK;
    }
    else
    {
      capGet.bitmap[1] &= ~LLDP_MED_CAP_INVENTORY_BITMASK;
    }

  /* set the value from application */
    owa.l7rc =
      usmDbLldpXMedPortConfigTLVsEnabledSet ( intIfNum,
                                            &capGet);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  } while( usmDbLldpValidIntfNextGet(intIfNum, &intIfNum) == L7_SUCCESS);

   FPOBJ_TRACE_EXIT (bufp, owa);
   return owa.rc;
}


xLibRC_t fpObjGet_SwitchingLLDPXMEDGlobalConfig_LLDPMedCapInventory (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t val,currval;
  xLibU32_t intIfNum;
  lldpXMedCapabilities_t capGet;
  FPOBJ_TRACE_ENTER (bufp);
  memset(&capGet, 0, sizeof(lldpXMedCapabilities_t));


  /* get the value from application */

  intIfNum = 0;
  usmDbLldpValidIntfFirstGet(&intIfNum);
  do
  {
    owa.l7rc =
     usmDbLldpXMedPortConfigTLVsEnabledGet ( intIfNum,
                                            &capGet);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    if((capGet.bitmap[1]&LLDP_MED_CAP_INVENTORY_BITMASK)==LLDP_MED_CAP_INVENTORY_BITMASK)
    {
       val=L7_ENABLE;
    }
    else
    {
      val=L7_DISABLE;
    }

  } while(0);
   while( usmDbLldpValidIntfNextGet(intIfNum, &intIfNum) == L7_SUCCESS)
   {
        owa.l7rc =
     usmDbLldpXMedPortConfigTLVsEnabledGet ( intIfNum,
                                            &capGet);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    if((capGet.bitmap[1]&LLDP_MED_CAP_INVENTORY_BITMASK)==LLDP_MED_CAP_INVENTORY_BITMASK)
    {
       currval=L7_ENABLE;
    }
    else
    {
      currval=L7_DISABLE;
    }
    if(currval!=val)
    {
      val=L7_DISABLE;
      break;
    }
      val=currval;
   }

   FPOBJ_TRACE_EXIT (bufp, owa);
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &val,
                           sizeof (val));
  return owa.rc;
}

