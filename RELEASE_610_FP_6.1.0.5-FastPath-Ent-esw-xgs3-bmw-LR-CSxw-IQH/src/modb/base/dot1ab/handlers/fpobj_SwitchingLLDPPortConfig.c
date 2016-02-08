/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_SwitchingLLDPPortConfig.c
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
#include "_xe_SwitchingLLDPPortConfig_obj.h"
#include "usmdb_lldp_api.h"
#include "usmdb_nim_api.h"

/*******************************************************************************
* @function fpObjGet_SwitchingLLDPPortConfig_PortNum
*
* @purpose Get 'PortNum'
*
* @description [PortNum]: Interfaces for LLDP Port Configuration 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPPortConfig_PortNum (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPortNumValue;
  xLibU32_t nextObjPortNumValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PortNum */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPPortConfig_PortNum,
                          (xLibU8_t *) & objPortNumValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, (char *)NULL, 0);
    owa.l7rc = usmDbLldpValidIntfFirstGet( &nextObjPortNumValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objPortNumValue, owa.len);
    owa.l7rc = usmDbLldpValidIntfNextGet( objPortNumValue,
                                          &nextObjPortNumValue);
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
* @function fpObjGet_SwitchingLLDPPortConfig_AdminStatus
*
* @purpose Get 'AdminStatus'
*
* @description [AdminStatus]: The administratively desired status of the local
*              LLDP agent. If the associated lldpPortConfigAdminStatus
*              object has a value of 'txOnly(1)', then LLDP agent will transmit
*              LLDP frames on this port and it will not store any
*              information about the remote syst 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPPortConfig_AdminStatus (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPortNumValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAdminStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PortNum */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPPortConfig_PortNum,
                          (xLibU8_t *) & keyPortNumValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPortNumValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbLldpIntfTxModeGet (keyPortNumValue,
                            &objAdminStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AdminStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAdminStatusValue,
                           sizeof (objAdminStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingLLDPPortConfig_AdminStatus
*
* @purpose Set 'AdminStatus'
*
* @description [AdminStatus]: The administratively desired status of the local
*              LLDP agent. If the associated lldpPortConfigAdminStatus
*              object has a value of 'txOnly(1)', then LLDP agent will transmit
*              LLDP frames on this port and it will not store any
*              information about the remote syst 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingLLDPPortConfig_AdminStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAdminStatusValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPortNumValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AdminStatus */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objAdminStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAdminStatusValue, owa.len);

  /* retrieve key: PortNum */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPPortConfig_PortNum,
                          (xLibU8_t *) & keyPortNumValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPortNumValue, kwa.len);

  /* set the value in application */
  owa.l7rc =
     usmDbLldpIntfTxModeSet (keyPortNumValue,
                             objAdminStatusValue);
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
* @function fpObjGet_SwitchingLLDPPortConfig_NotificationEnable
*
* @purpose Get 'NotificationEnable'
*
* @description [NotificationEnable]: The NotificationEnable controls, on a
*              per port basis, whether or not notifications from the agent
*              are enabled. The value true(1) means that notifications are
*              enabled; the value false(2) means that they are not. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPPortConfig_NotificationEnable (void *wap,
                                                              void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPortNumValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objNotificationEnableValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PortNum */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPPortConfig_PortNum,
                          (xLibU8_t *) & keyPortNumValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPortNumValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbLldpIntfNotificationModeGet (keyPortNumValue,
                                               &objNotificationEnableValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: NotificationEnable */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objNotificationEnableValue,
                           sizeof (objNotificationEnableValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingLLDPPortConfig_NotificationEnable
*
* @purpose Set 'NotificationEnable'
*
* @description [NotificationEnable]: The NotificationEnable controls, on a
*              per port basis, whether or not notifications from the agent
*              are enabled. The value true(1) means that notifications are
*              enabled; the value false(2) means that they are not. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingLLDPPortConfig_NotificationEnable (void *wap,
                                                              void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objNotificationEnableValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPortNumValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: NotificationEnable */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objNotificationEnableValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objNotificationEnableValue, owa.len);

  /* retrieve key: PortNum */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPPortConfig_PortNum,
                          (xLibU8_t *) & keyPortNumValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPortNumValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbLldpIntfNotificationModeSet (keyPortNumValue,
                                               objNotificationEnableValue);
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
* @function fpObjGet_SwitchingLLDPPortConfig_TLVsTxEnable
*
* @purpose Get 'TLVsTxEnable'
*
* @description [TLVsTxEnable]: The lldpPortConfigTLVsTxEnable, defined as
*              a bitmap,includes the basic set of LLDP TLVs whose transmission
*              is allowed on the local LLDP agent by the network management.
*              Each bit in the bitmap corresponds to a TLV type associated
*              with a specific optional T 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPPortConfig_TLVsTxEnable (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPortNumValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objTLVsTxEnableValue={0};
  FPOBJ_TRACE_ENTER (bufp);
  L7_BOOL portDesc,sysName,sysDesc,sysCap,flag=L7_FALSE;

  /* retrieve key: PortNum */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPPortConfig_PortNum,
                          (xLibU8_t *) & keyPortNumValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPortNumValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbLldpIntfTxTLVsGet (keyPortNumValue, &portDesc, &sysName, &sysDesc, &sysCap);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if (sysName == L7_TRUE)
  {
    strcpy(objTLVsTxEnableValue, "System Name");
    flag = L7_TRUE;
  }
  if (sysDesc == L7_TRUE)
  {
    if(flag == L7_TRUE)
    {
      strcat(objTLVsTxEnableValue, "<BR>");
    }
    strcat(objTLVsTxEnableValue, "System Description");
    flag = L7_TRUE;
  }
  if (sysCap == L7_TRUE)
  {
    if(flag == L7_TRUE)
    {
      strcat(objTLVsTxEnableValue, "<BR>");
    }
    strcat(objTLVsTxEnableValue, "System Capabilities");
    flag = L7_TRUE;
  }
  if (portDesc == L7_TRUE)
  {
    if(flag == L7_TRUE)
    {
      strcat(objTLVsTxEnableValue, "<BR>");
    }
    strcat(objTLVsTxEnableValue, "Port Description");
  }


  /* return the object value: TLVsTxEnable */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objTLVsTxEnableValue,
                           strlen (objTLVsTxEnableValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_SwitchingLLDPPortConfig_TLVsTxSystemName
*
* @purpose Get 'TLVsTxEnable'
*
* @description [TLVsTxEnable]: The lldpPortConfigTLVsTxEnable, defined as
*              a bitmap,includes the basic set of LLDP TLVs whose transmission
*              is allowed on the local LLDP agent by the network management.
*              Each bit in the bitmap corresponds to a TLV type associated
*              with a specific optional T 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPPortConfig_TLVsTxSystemName (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPortNumValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);
  L7_BOOL portDesc,sysName,sysDesc,sysCap;

  /* retrieve key: PortNum */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPPortConfig_PortNum,
                          (xLibU8_t *) & keyPortNumValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPortNumValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbLldpIntfTxTLVsGet (keyPortNumValue, &portDesc, &sysName, &sysDesc, &sysCap);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: TLVsTxEnable */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &sysName,
                           sizeof (sysName));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_SwitchingLLDPPortConfig_TLVsTxSystemName
*
* @purpose Get 'TLVsTxSystemName'
*
* @description [TLVsTxSystemName]: The lldpPortConfigTLVsTxEnable, defined as
*              a bitmap,includes the basic set of LLDP TLVs whose transmission
*              is allowed on the local LLDP agent by the network management.
*              Each bit in the bitmap corresponds to a TLV type associated
*              with a specific optional T 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingLLDPPortConfig_TLVsTxSystemName (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPortNumValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);
  L7_BOOL portDesc,sysName,sysDesc,sysCap,val;

 /* retrieve object: TLVsTxEnable */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) &val, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &val, owa.len);

  /* retrieve key: PortNum */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPPortConfig_PortNum,
                          (xLibU8_t *) & keyPortNumValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPortNumValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbLldpIntfTxTLVsGet (keyPortNumValue, &portDesc, &sysName, &sysDesc, &sysCap);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  owa.l7rc = usmDbLldpIntfTxTLVsSet (keyPortNumValue, portDesc, val,
                                     sysDesc, sysCap);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingLLDPPortConfig_TLVsTxSystemDescr
*
* @purpose Get 'TLVsTxSystemDescr'
*
* @description [TLVsTxEnable]: The lldpPortConfigTLVsTxEnable, defined as
*              a bitmap,includes the basic set of LLDP TLVs whose transmission
*              is allowed on the local LLDP agent by the network management.
*              Each bit in the bitmap corresponds to a TLV type associated
*              with a specific optional T 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPPortConfig_TLVsTxSystemDescr (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPortNumValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);
  L7_BOOL portDesc,sysName,sysDesc,sysCap;

  /* retrieve key: PortNum */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPPortConfig_PortNum,
                          (xLibU8_t *) & keyPortNumValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPortNumValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbLldpIntfTxTLVsGet (keyPortNumValue, &portDesc, &sysName, &sysDesc, &sysCap);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: TLVsTxEnable */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &sysDesc,
                           sizeof (sysDesc));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_SwitchingLLDPPortConfig_TLVsTxSystemDescr
*
* @purpose Get 'TLVsTxSystemName'
*
* @description [TLVsTxSystemName]: The lldpPortConfigTLVsTxEnable, defined as
*              a bitmap,includes the basic set of LLDP TLVs whose transmission
*              is allowed on the local LLDP agent by the network management.
*              Each bit in the bitmap corresponds to a TLV type associated
*              with a specific optional T 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingLLDPPortConfig_TLVsTxSystemDescr (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPortNumValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);
  L7_BOOL portDesc,sysName,sysDesc,sysCap,val;

 /* retrieve object: TLVsTxEnable */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) &val, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &val, owa.len);

  /* retrieve key: PortNum */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPPortConfig_PortNum,
                          (xLibU8_t *) & keyPortNumValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPortNumValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbLldpIntfTxTLVsGet (keyPortNumValue, &portDesc, &sysName, &sysDesc, &sysCap);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  owa.l7rc = usmDbLldpIntfTxTLVsSet (keyPortNumValue, portDesc, sysName,
                                     val, sysCap);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingLLDPPortConfig_TLVsTxSystemCap
*
* @purpose Get 'TLVsTxSystemDescr'
*
* @description [TLVsTxEnable]: The lldpPortConfigTLVsTxEnable, defined as
*              a bitmap,includes the basic set of LLDP TLVs whose transmission
*              is allowed on the local LLDP agent by the network management.
*              Each bit in the bitmap corresponds to a TLV type associated
*              with a specific optional T 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPPortConfig_TLVsTxSystemCap (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPortNumValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);
  L7_BOOL portDesc,sysName,sysDesc,sysCap;

  /* retrieve key: PortNum */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPPortConfig_PortNum,
                          (xLibU8_t *) & keyPortNumValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPortNumValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbLldpIntfTxTLVsGet (keyPortNumValue, &portDesc, &sysName, &sysDesc, &sysCap);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: TLVsTxEnable */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &sysCap,
                           sizeof (sysCap));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_SwitchingLLDPPortConfig_TLVsTxSystemCap
*
* @purpose Get 'TLVsTxSystemName'
*
* @description [TLVsTxSystemName]: The lldpPortConfigTLVsTxEnable, defined as
*              a bitmap,includes the basic set of LLDP TLVs whose transmission
*              is allowed on the local LLDP agent by the network management.
*              Each bit in the bitmap corresponds to a TLV type associated
*              with a specific optional T 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingLLDPPortConfig_TLVsTxSystemCap (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPortNumValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);
  L7_BOOL portDesc,sysName,sysDesc,sysCap,val;

 /* retrieve object: TLVsTxEnable */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) &val, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &val, owa.len);

  /* retrieve key: PortNum */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPPortConfig_PortNum,
                          (xLibU8_t *) & keyPortNumValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPortNumValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbLldpIntfTxTLVsGet (keyPortNumValue, &portDesc, &sysName, &sysDesc, &sysCap);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  owa.l7rc = usmDbLldpIntfTxTLVsSet (keyPortNumValue, portDesc, sysName,
                                     sysDesc, val);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingLLDPPortConfig_TLVsTxPortDescr
*
* @purpose Get 'TLVsTxSystemDescr'
*
* @description [TLVsTxEnable]: The lldpPortConfigTLVsTxEnable, defined as
*              a bitmap,includes the basic set of LLDP TLVs whose transmission
*              is allowed on the local LLDP agent by the network management.
*              Each bit in the bitmap corresponds to a TLV type associated
*              with a specific optional T 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPPortConfig_TLVsTxPortDescr (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPortNumValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);
  L7_BOOL portDesc,sysName,sysDesc,sysCap;

  /* retrieve key: PortNum */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPPortConfig_PortNum,
                          (xLibU8_t *) & keyPortNumValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPortNumValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbLldpIntfTxTLVsGet (keyPortNumValue, &portDesc, &sysName, &sysDesc, &sysCap);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: TLVsTxEnable */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &portDesc,
                           sizeof (portDesc));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_SwitchingLLDPPortConfig_TLVsTxPortDescr
*
* @purpose Get 'TLVsTxSystemName'
*
* @description [TLVsTxSystemName]: The lldpPortConfigTLVsTxEnable, defined as
*              a bitmap,includes the basic set of LLDP TLVs whose transmission
*              is allowed on the local LLDP agent by the network management.
*              Each bit in the bitmap corresponds to a TLV type associated
*              with a specific optional T 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingLLDPPortConfig_TLVsTxPortDescr (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPortNumValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);
  L7_BOOL portDesc,sysName,sysDesc,sysCap,val;

 /* retrieve object: TLVsTxEnable */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) &val, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &val, owa.len);

  /* retrieve key: PortNum */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPPortConfig_PortNum,
                          (xLibU8_t *) & keyPortNumValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPortNumValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbLldpIntfTxTLVsGet (keyPortNumValue, &portDesc, &sysName, &sysDesc, &sysCap);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  owa.l7rc = usmDbLldpIntfTxTLVsSet (keyPortNumValue, val, sysName,
                                     sysDesc, sysCap);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_SwitchingLLDPPortConfig_TxMgmtInfo
*
* @purpose Get 'TxMgmtInfo'
*
* @description [TxMgmtInfo]: mgmt addr transmit mode for the specified interface
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPPortConfig_TxMgmtInfo (void *wap, void *bufp)
{
  fpObjWa_t kwaPortNum = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPortNumValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTxMgmtInfoValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PortNum */
  kwaPortNum.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPPortConfig_PortNum,
                                 (xLibU8_t *) & keyPortNumValue,
                                 &kwaPortNum.len);
  if (kwaPortNum.rc != XLIBRC_SUCCESS)
  {
    kwaPortNum.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaPortNum);
    return kwaPortNum.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPortNumValue, kwaPortNum.len);

  /* get the value from application */
  owa.l7rc = usmDbLldpIntfTxMgmtAddrGet(keyPortNumValue,
                                        &objTxMgmtInfoValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: TxMgmtInfo */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTxMgmtInfoValue,
                           sizeof (objTxMgmtInfoValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingLLDPPortConfig_TxMgmtInfo
*
* @purpose Set 'TxMgmtInfo'
*
* @description [TxMgmtInfo]: mgmt addr transmit mode for the specified interface
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingLLDPPortConfig_TxMgmtInfo (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTxMgmtInfoValue;
  fpObjWa_t kwaPortNum = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPortNumValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: TxMgmtInfo */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objTxMgmtInfoValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTxMgmtInfoValue, owa.len);

  /* retrieve key: PortNum */
  kwaPortNum.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPPortConfig_PortNum,
                                 (xLibU8_t *) & keyPortNumValue,
                                 &kwaPortNum.len);
  if (kwaPortNum.rc != XLIBRC_SUCCESS)
  {
    kwaPortNum.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaPortNum);
    return kwaPortNum.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPortNumValue, kwaPortNum.len);

  /* set the value in application */
  owa.l7rc = usmDbLldpIntfTxMgmtAddrSet(keyPortNumValue,
                                        objTxMgmtInfoValue);
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
* @function fpObjGet_SwitchingLLDPPortConfig_LldpPortReceiveMode
*
* @purpose Get 'LldpPortReceiveMode'
*
* @description [LldpPortReceiveMode] LLDP - 802.1AB receive mode for the selected interface
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPPortConfig_LldpPortReceiveMode (void *wap, void *bufp)
{

  fpObjWa_t kwaPortNum = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPortNumValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objLldpPortReceiveModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PortNum */
  kwaPortNum.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPPortConfig_PortNum,
                                 (xLibU8_t *) & keyPortNumValue, &kwaPortNum.len);
  if (kwaPortNum.rc != XLIBRC_SUCCESS)
  {
    kwaPortNum.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaPortNum);
    return kwaPortNum.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPortNumValue, kwaPortNum.len);

  /* get the value from application */
  owa.l7rc = usmDbLldpIntfRxModeGet ( keyPortNumValue, &objLldpPortReceiveModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: LldpPortReceiveMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objLldpPortReceiveModeValue,
                           sizeof (objLldpPortReceiveModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingLLDPPortConfig_LldpPortReceiveMode
*
* @purpose Set 'LldpPortReceiveMode'
*
* @description [LldpPortReceiveMode] LLDP - 802.1AB receive mode for the selected interface
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingLLDPPortConfig_LldpPortReceiveMode (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objLldpPortReceiveModeValue;

  fpObjWa_t kwaPortNum = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPortNumValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: LldpPortReceiveMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objLldpPortReceiveModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objLldpPortReceiveModeValue, owa.len);

  /* retrieve key: PortNum */
  kwaPortNum.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPPortConfig_PortNum,
                                 (xLibU8_t *) & keyPortNumValue, &kwaPortNum.len);
  if (kwaPortNum.rc != XLIBRC_SUCCESS)
  {
    kwaPortNum.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaPortNum);
    return kwaPortNum.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPortNumValue, kwaPortNum.len);

  /* set the value in application */
  owa.l7rc = usmDbLldpIntfRxModeSet ( keyPortNumValue, objLldpPortReceiveModeValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingLLDPPortConfig_LldpXPortTransmitMode
*
* @purpose Get 'LldpXPortTransmitMode'
*
* @description [LldpXPortTransmitMode] LLDP - 802.1AB transmit mode for the selected interface
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPPortConfig_LldpXPortTransmitMode (void *wap, void *bufp)
{

  fpObjWa_t kwaPortNum = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPortNumValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objLldpXPortTransmitModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: PortNum */
  kwaPortNum.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPPortConfig_PortNum,
                                 (xLibU8_t *) & keyPortNumValue, &kwaPortNum.len);
  if (kwaPortNum.rc != XLIBRC_SUCCESS)
  {
    kwaPortNum.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaPortNum);
    return kwaPortNum.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPortNumValue, kwaPortNum.len);

  /* get the value from application */
  owa.l7rc = usmDbLldpIntfTxModeGet ( keyPortNumValue, &objLldpXPortTransmitModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: LldpXPortTransmitMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objLldpXPortTransmitModeValue,
                           sizeof (objLldpXPortTransmitModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingLLDPPortConfig_LldpXPortTransmitMode
*
* @purpose Set 'LldpXPortTransmitMode'
*
* @description [LldpXPortTransmitMode] LLDP - 802.1AB transmit mode for the selected interface
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingLLDPPortConfig_LldpXPortTransmitMode (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objLldpXPortTransmitModeValue;

  fpObjWa_t kwaPortNum = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyPortNumValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: LldpXPortTransmitMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objLldpXPortTransmitModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objLldpXPortTransmitModeValue, owa.len);

  /* retrieve key: PortNum */
  kwaPortNum.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPPortConfig_PortNum,
                                 (xLibU8_t *) & keyPortNumValue, &kwaPortNum.len);
  if (kwaPortNum.rc != XLIBRC_SUCCESS)
  {
    kwaPortNum.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaPortNum);
    return kwaPortNum.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyPortNumValue, kwaPortNum.len);

  /* set the value in application */
  owa.l7rc = usmDbLldpIntfTxModeSet ( keyPortNumValue, objLldpXPortTransmitModeValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingLLDPPortConfig_LinkStatus
*
* @purpose Get 'LinkStatus'
*
* @description [LinkStatus] Indicates whether the Link is up or down.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingLLDPPortConfig_LinkStatus (void *wap, void *bufp)
{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  L7_uint32 link_state;
  xLibStr256_t objLinkStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingLLDPPortConfig_PortNum,
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



