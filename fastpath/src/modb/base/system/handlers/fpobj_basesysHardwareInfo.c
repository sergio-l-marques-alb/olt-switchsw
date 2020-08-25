/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_basesysHardwareInfo.c
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
#include "_xe_basesysHardwareInfo_obj.h"
#include "usmdb_registry_api.h"
#include "usmdb_sim_api.h"
#include "usmdb_util_api.h"

/*******************************************************************************
* @function fpObjGet_basesysHardwareInfo_sysInventoryMachineType
*
* @purpose Get 'sysInventoryMachineType'
*
* @description The machine type of this switch. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basesysHardwareInfo_sysInventoryMachineType (void *wap,
                                                               void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objsysInventoryMachineTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbMachineTypeGet (L7_UNIT_CURRENT, objsysInventoryMachineTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objsysInventoryMachineTypeValue,
                     strlen (objsysInventoryMachineTypeValue));

  /* return the object value: sysInventoryMachineType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objsysInventoryMachineTypeValue,
                           strlen (objsysInventoryMachineTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basesysHardwareInfo_sysInventoryMachineModel
*
* @purpose Get 'sysInventoryMachineModel'
*
* @description The model within the machine type 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basesysHardwareInfo_sysInventoryMachineModel (void *wap,
                                                                void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objsysInventoryMachineModelValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbMachineModelGet (L7_UNIT_CURRENT, objsysInventoryMachineModelValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objsysInventoryMachineModelValue,
                     strlen (objsysInventoryMachineModelValue));

  /* return the object value: sysInventoryMachineModel */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objsysInventoryMachineModelValue,
                           strlen (objsysInventoryMachineModelValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basesysHardwareInfo_sysInventorySerialNum
*
* @purpose Get 'sysInventorySerialNum'
*
* @description The unique box serial number for this switch 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basesysHardwareInfo_sysInventorySerialNum (void *wap,
                                                             void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objsysInventorySerialNumValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbSwDevInfoSerialNumGet (L7_UNIT_CURRENT, objsysInventorySerialNumValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objsysInventorySerialNumValue,
                     strlen (objsysInventorySerialNumValue));

  /* return the object value: sysInventorySerialNum */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objsysInventorySerialNumValue,
                           strlen (objsysInventorySerialNumValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basesysHardwareInfo_sysInventoryFRUNum
*
* @purpose Get 'sysInventoryFRUNum'
*
* @description The field replaceable unit number 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basesysHardwareInfo_sysInventoryFRUNum (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objsysInventoryFRUNumValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  memset(objsysInventoryFRUNumValue, 0x00, sizeof(objsysInventoryFRUNumValue));
  owa.l7rc = usmDbFRUNumGet (L7_UNIT_CURRENT, objsysInventoryFRUNumValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objsysInventoryFRUNumValue,
                     strlen (objsysInventoryFRUNumValue));

  /* return the object value: sysInventoryFRUNum */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objsysInventoryFRUNumValue,
                           strlen (objsysInventoryFRUNumValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basesysHardwareInfo_sysInventoryPartNum
*
* @purpose Get 'sysInventoryPartNum'
*
* @description The manufacturing part number 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basesysHardwareInfo_sysInventoryPartNum (void *wap,
                                                           void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t  objsysInventoryPartNumValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbPartNumGet (L7_UNIT_CURRENT, objsysInventoryPartNumValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objsysInventoryPartNumValue,
                     strlen (objsysInventoryPartNumValue));

  /* return the object value: sysInventoryPartNum */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objsysInventoryPartNumValue,
                           strlen (objsysInventoryPartNumValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basesysHardwareInfo_sysInventoryMaintLevel
*
* @purpose Get 'sysInventoryMaintLevel'
*
* @description The identification of the hardware change level 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basesysHardwareInfo_sysInventoryMaintLevel (void *wap,
                                                              void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t  objsysInventoryMaintLevelValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbMaintLevelGet (L7_UNIT_CURRENT, objsysInventoryMaintLevelValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objsysInventoryMaintLevelValue,
                     strlen (objsysInventoryMaintLevelValue));

  /* return the object value: sysInventoryMaintLevel */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objsysInventoryMaintLevelValue,
                           strlen (objsysInventoryMaintLevelValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basesysHardwareInfo_sysInventoryManufacturer
*
* @purpose Get 'sysInventoryManufacturer'
*
* @description The two-octet code that identifies the manufacturer 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basesysHardwareInfo_sysInventoryManufacturer (void *wap,
                                                                void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objsysInventoryManufacturerValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbManufacturerGet (L7_UNIT_CURRENT, objsysInventoryManufacturerValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objsysInventoryManufacturerValue,
                     strlen (objsysInventoryManufacturerValue));

  /* return the object value: sysInventoryManufacturer */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objsysInventoryManufacturerValue,
                           strlen (objsysInventoryManufacturerValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basesysHardwareInfo_sysInventoryBurnedInMacAddr
*
* @purpose Get 'sysInventoryBurnedInMacAddr'
*
* @description The burned-in universally administered MAC address of this switch 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basesysHardwareInfo_sysInventoryBurnedInMacAddr (void *wap,
                                                                   void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr6_t objsysInventoryBurnedInMacAddrValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbSwDevCtrlBurnedInMacAddrGet (L7_UNIT_CURRENT,
                                      objsysInventoryBurnedInMacAddrValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objsysInventoryBurnedInMacAddrValue,
                     L7_MAC_ADDR_LEN);

  /* return the object value: sysInventoryBurnedInMacAddr */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) objsysInventoryBurnedInMacAddrValue,
                     L7_MAC_ADDR_LEN);
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basesysHardwareInfo_sysInventorySwVersion
*
* @purpose Get 'sysInventorySwVersion'
*
* @description The release.version.maintenance number of the code currently running 
*              on the switch. For example, if the release was 1, the 
*              version was 2 and the maintenance number was 4, the format would 
*              be '1.2.4' 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basesysHardwareInfo_sysInventorySwVersion (void *wap,
                                                             void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t  objsysInventorySwVersionValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbSwVersionGet (L7_UNIT_CURRENT, objsysInventorySwVersionValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objsysInventorySwVersionValue,
                     strlen (objsysInventorySwVersionValue));

  /* return the object value: sysInventorySwVersion */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objsysInventorySwVersionValue,
                           strlen (objsysInventorySwVersionValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
/*******************************************************************************
* @function fpObjGet_basesysHardwareInfo_sysInventoryHwVersion
*
* @purpose Get 'sysInventoryHwVersion'
*
* @description The hardware version identifier of the switch as specified 
*              by the manufacturer
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basesysHardwareInfo_sysInventoryHwVersion (void *wap,
                                                             void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t  objsysInventoryHwVersionValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbHwVersionGet (L7_UNIT_CURRENT, objsysInventoryHwVersionValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objsysInventoryHwVersionValue,
                     strlen (objsysInventoryHwVersionValue));

  /* return the object value: sysInventoryHwVersion */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objsysInventoryHwVersionValue,
                           strlen (objsysInventoryHwVersionValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_basesysHardwareInfo_sysInventoryOperSys
*
* @purpose Get 'sysInventoryOperSys'
*
* @description The operating system currently running on the switch 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basesysHardwareInfo_sysInventoryOperSys (void *wap,
                                                           void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t  objsysInventoryOperSysValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbOperSysGet (L7_UNIT_CURRENT, objsysInventoryOperSysValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objsysInventoryOperSysValue,
                     strlen (objsysInventoryOperSysValue));

  /* return the object value: sysInventoryOperSys */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objsysInventoryOperSysValue,
                           strlen (objsysInventoryOperSysValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basesysHardwareInfo_sysInventoryNPD
*
* @purpose Get 'sysInventoryNPD'
*
* @description Identifies the network processor hardware 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basesysHardwareInfo_sysInventoryNPD (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t  objsysInventoryNPDValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbNPDGet (L7_UNIT_CURRENT, objsysInventoryNPDValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objsysInventoryNPDValue,
                     strlen (objsysInventoryNPDValue));

  /* return the object value: sysInventoryNPD */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objsysInventoryNPDValue,
                           strlen (objsysInventoryNPDValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basesysHardwareInfo_sysInventoryAdditiionalPackages
*
* @purpose Get 'sysInventoryAdditiionalPackages'
*
* @description A list of the optional software packages installed on the switch, 
*              if any. For example, FASTPATH BGP-4, or FASTPATH Multicast 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basesysHardwareInfo_sysInventoryAdditiionalPackages (void
                                                                       *wap,
                                                                       void
                                                                       *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objsysInventoryAdditiionalPackagesValue;
  L7_BOOL addpkgs = L7_FALSE;
  memset(objsysInventoryAdditiionalPackagesValue, 0x00,
		                            sizeof(objsysInventoryAdditiionalPackagesValue));
  FPOBJ_TRACE_ENTER (bufp);	

  if (usmDbComponentPresentCheck(L7_UNIT_CURRENT, L7_FLEX_BGP_MAP_COMPONENT_ID) == L7_TRUE)
  {
    addpkgs = L7_TRUE;
    osapiSnprintf(objsysInventoryAdditiionalPackagesValue, sizeof(objsysInventoryAdditiionalPackagesValue), "OLTSWITCH BGP-4");
  }

  if ((usmDbComponentPresentCheck(L7_UNIT_CURRENT, L7_FLEX_QOS_ACL_COMPONENT_ID) == L7_TRUE) ||
      (usmDbComponentPresentCheck(L7_UNIT_CURRENT, L7_FLEX_QOS_COS_COMPONENT_ID) == L7_TRUE) ||
      (usmDbComponentPresentCheck(L7_UNIT_CURRENT, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID) == L7_TRUE))
  {
    if (addpkgs == L7_FALSE)
    {
      osapiSnprintf(objsysInventoryAdditiionalPackagesValue, sizeof(objsysInventoryAdditiionalPackagesValue), "OLTSWITCH QoS");
    }
    else
    {
      osapiStrncat(objsysInventoryAdditiionalPackagesValue, "\n", strlen("\n"));
      osapiStrncat(objsysInventoryAdditiionalPackagesValue, "OLTSWITCH QoS", strlen("OLTSWITCH QoS"));
    }
    addpkgs = L7_TRUE;
  }

  if (usmDbComponentPresentCheck(L7_UNIT_CURRENT, L7_FLEX_MCAST_MAP_COMPONENT_ID) == L7_TRUE)
  {
    if (addpkgs == L7_FALSE)
    {
      osapiSnprintf(objsysInventoryAdditiionalPackagesValue, sizeof(objsysInventoryAdditiionalPackagesValue), "OLTSWITCH Multicast");
    }
    else
    {
      osapiStrncat(objsysInventoryAdditiionalPackagesValue, "\n", strlen("\n"));
      osapiStrncat(objsysInventoryAdditiionalPackagesValue, "OLTSWITCH Multicast", strlen("OLTSWITCH Multicast"));
    }
    addpkgs = L7_TRUE;
  }

  if (usmDbComponentPresentCheck(L7_UNIT_CURRENT, L7_FLEX_IPV6_MAP_COMPONENT_ID) == L7_TRUE)
  {
    if (addpkgs == L7_FALSE)
    {
      osapiSnprintf(objsysInventoryAdditiionalPackagesValue, sizeof(objsysInventoryAdditiionalPackagesValue), "OLTSWITCH IPv6");
    }
    else
    {
      osapiStrncat(objsysInventoryAdditiionalPackagesValue, "\n", strlen("\n"));
      osapiStrncat(objsysInventoryAdditiionalPackagesValue, "OLTSWITCH IPv6", strlen("OLTSWITCH IPv6"));
    }
    addpkgs = L7_TRUE;
  }

  if (usmDbComponentPresentCheck(L7_UNIT_CURRENT, L7_FLEX_IPV6_MGMT_COMPONENT_ID) == L7_TRUE)
  {
    if (addpkgs == L7_FALSE)
    {
      osapiSnprintf(objsysInventoryAdditiionalPackagesValue, sizeof(objsysInventoryAdditiionalPackagesValue), "OLTSWITCH IPv6 Management");
    }
    else
    {
      osapiStrncat(objsysInventoryAdditiionalPackagesValue, "\n", strlen("\n"));
      osapiStrncat(objsysInventoryAdditiionalPackagesValue, "OLTSWITCH IPv6 Management", strlen("OLTSWITCH IPv6 Management"));
    }
    addpkgs = L7_TRUE;
  }

  if (usmDbComponentPresentCheck(L7_UNIT_CURRENT, L7_FLEX_WIRELESS_COMPONENT_ID) == L7_TRUE)
  {
    if (addpkgs == L7_FALSE)
    {
      osapiSnprintf (objsysInventoryAdditiionalPackagesValue, sizeof (objsysInventoryAdditiionalPackagesValue),
                     "OLTSWITCH Wireless");
    }
    else
    {
      osapiStrncat(objsysInventoryAdditiionalPackagesValue, "\n", strlen("\n"));
      osapiStrncat(objsysInventoryAdditiionalPackagesValue, "OLTSWITCH Wireless", strlen("OLTSWITCH Wireless"));
    }
    addpkgs = L7_TRUE;
  }

  if (usmDbComponentPresentCheck(L7_UNIT_CURRENT, L7_FLEX_STACKING_COMPONENT_ID) == L7_TRUE)
  {
    if (addpkgs == L7_FALSE)
    {
      osapiSnprintf(objsysInventoryAdditiionalPackagesValue, sizeof(objsysInventoryAdditiionalPackagesValue),  "OLTSWITCH Stacking");
    }
    else
    {
      osapiStrncat(objsysInventoryAdditiionalPackagesValue, "\n", strlen("\n"));
      osapiStrncat(objsysInventoryAdditiionalPackagesValue, "OLTSWITCH Stacking", strlen("OLTSWITCH Stacking"));
    }
    addpkgs = L7_TRUE;
  }

  if((usmDbComponentPresentCheck(L7_UNIT_CURRENT, L7_FLEX_METRO_DOT1AD_COMPONENT_ID) == L7_TRUE) ||
     (usmDbComponentPresentCheck(L7_UNIT_CURRENT, L7_DOT3AH_COMPONENT_ID)            == L7_TRUE) ||
     (usmDbComponentPresentCheck(L7_UNIT_CURRENT, L7_DOT1AG_COMPONENT_ID)            == L7_TRUE) ||
     (usmDbComponentPresentCheck(L7_UNIT_CURRENT, L7_TR069_COMPONENT_ID)             == L7_TRUE))
  {
    if (addpkgs == L7_FALSE)
    {
      osapiSnprintf(objsysInventoryAdditiionalPackagesValue, sizeof(objsysInventoryAdditiionalPackagesValue),  "OLTSWITCH Metro");
    }
    else
    {
      osapiStrncat(objsysInventoryAdditiionalPackagesValue, "\n", strlen("\n"));
      osapiStrncat(objsysInventoryAdditiionalPackagesValue, "OLTSWITCH Metro", strlen("OLTSWITCH Metro"));
    }
    addpkgs = L7_TRUE;
  }

  if (addpkgs == L7_FALSE)
  {
    osapiSnprintf(objsysInventoryAdditiionalPackagesValue, sizeof(objsysInventoryAdditiionalPackagesValue), "None");
  }

  FPOBJ_TRACE_VALUE (bufp, objsysInventoryAdditiionalPackagesValue,
                     strlen (objsysInventoryAdditiionalPackagesValue));

  /* return the object value: sysInventoryAdditiionalPackages */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) objsysInventoryAdditiionalPackagesValue,
                    strlen (objsysInventoryAdditiionalPackagesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_basesysHardwareInfo_sysLocalAdminMacAddr
*
* @purpose Get 'sysLocalAdminMacAddr'
*
* @description Unit's System Locally Administered Mac Address 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basesysHardwareInfo_sysLocalAdminMacAddr (void *wap,
                                                            void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  xLibStr6_t objsysLocalAdminMacAddrValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbSwDevCtrlLocalAdminAddrGet (L7_UNIT_CURRENT,
                                     objsysLocalAdminMacAddrValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objsysLocalAdminMacAddrValue,
                      L7_MAC_ADDR_LEN);

  /* return the object value: sysLocalAdminMacAddr */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objsysLocalAdminMacAddrValue,
                      L7_MAC_ADDR_LEN);
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_basesysHardwareInfo_sysLocalAdminMacAddr
*
* @purpose Set 'sysLocalAdminMacAddr'
*
* @description Unit's System Locally Administered Mac Address 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basesysHardwareInfo_sysLocalAdminMacAddr (void *wap,
                                                            void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  xLibStr6_t objsysLocalAdminMacAddrValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: sysLocalAdminMacAddr */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) objsysLocalAdminMacAddrValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objsysLocalAdminMacAddrValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbSwDevCtrlLocalAdminAddrSet (L7_UNIT_CURRENT,
                                     objsysLocalAdminMacAddrValue);
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
* @function fpObjGet_basesysHardwareInfo_sysMacAddrType
*
* @purpose Get 'sysMacAddrType'
*
* @description Unit's System Mac Address Type 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_basesysHardwareInfo_sysMacAddrType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objsysMacAddrTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbSwDevCtrlMacAddrTypeGet (L7_UNIT_CURRENT, &objsysMacAddrTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysMacAddrTypeValue,
                     sizeof (objsysMacAddrTypeValue));

  /* return the object value: sysMacAddrType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objsysMacAddrTypeValue,
                           sizeof (objsysMacAddrTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_basesysHardwareInfo_sysMacAddrType
*
* @purpose Set 'sysMacAddrType'
*
* @description Unit's System Mac Address Type 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_basesysHardwareInfo_sysMacAddrType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objsysMacAddrTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: sysMacAddrType */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objsysMacAddrTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objsysMacAddrTypeValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbSwDevCtrlMacAddrTypeSet (L7_UNIT_CURRENT, objsysMacAddrTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

