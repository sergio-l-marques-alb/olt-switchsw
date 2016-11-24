/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_SwitchingVoiceVlanInterfaceConfig.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to VoiceVlan-object.xml
*
* @create  5 February 2008
*
* @author Radha K 
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_SwitchingVoiceVlanInterfaceConfig_obj.h"
#include "commdefs.h"
#include "usmdb_voice_vlan_api.h"
#include "usmdb_util_api.h"
#include "usmdb_nim_api.h"
#include "usmdb_trapmgr_api.h"
#include "usmdb_common.h"
#include "usmdb_policy_api.h"
#include "usmdb_dvlantag_api.h"
#include "usmdb_dot3ad_api.h"
#include "usmdb_dot1q_api.h"

/*******************************************************************************
* @function fpObjGet_SwitchingVoiceVlanInterfaceConfig_Interface
*
* @purpose Get 'Interface'
*
* @description [Interface]: Interfaces on which port based parameters will
*              be configured 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingVoiceVlanInterfaceConfig_Interface (void *wap,
                                                               void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objInterfaceValue;
  xLibU32_t nextObjInterfaceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingVoiceVlanInterfaceConfig_Interface,
                          (xLibU8_t *) & objInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbGetNextPhysicalIntIfNumber ( 0,
                                       &nextObjInterfaceValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objInterfaceValue, owa.len);
    owa.l7rc =
      usmDbGetNextPhysicalIntIfNumber ( objInterfaceValue,
                                       &nextObjInterfaceValue);
  }
  while ( (owa.l7rc == L7_SUCCESS ) && (usmDbVoiceVlanInterfaceValidate(L7_UNIT_CURRENT, nextObjInterfaceValue ) != L7_SUCCESS ))
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &nextObjInterfaceValue, owa.len);
    owa.l7rc =
      usmDbGetNextPhysicalIntIfNumber ( nextObjInterfaceValue,
                                       &nextObjInterfaceValue);
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjInterfaceValue, owa.len);

  /* return the object value: Interface */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjInterfaceValue,
                           sizeof (objInterfaceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

#if 0 /* Invalid Objects */

/*******************************************************************************
* @function fpObjGet_SwitchingVoiceVlanInterfaceConfig_IfIndex
*
* @purpose Get 'IfIndex'
*
* @description [IfIndex]: IfIndex for this port 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingVoiceVlanInterfaceConfig_IfIndex (void *wap,
                                                             void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIfIndexValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVoiceVlanInterfaceConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbExtIfNumFromIntIfNum ( keyInterfaceValue,
                                        &objIfIndexValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: IfIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIfIndexValue,
                           sizeof (objIfIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingVoiceVlanInterfaceConfig_IanaType
*
* @purpose Get 'IanaType'
*
* @description [IanaType]: IANA Type for this port 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingVoiceVlanInterfaceConfig_IanaType (void *wap,
                                                              void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIanaTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVoiceVlanInterfaceConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIfTypeGet (L7_UNIT_CURRENT, keyInterfaceValue,
                             &objIanaTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: IanaType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIanaTypeValue,
                           sizeof (objIanaTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingVoiceVlanInterfaceConfig_AdminMode
*
* @purpose Get 'AdminMode'
*
* @description [AdminMode]: The switch's Port Admin Mode 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingVoiceVlanInterfaceConfig_AdminMode (void *wap,
                                                               void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAdminModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVoiceVlanInterfaceConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIfAdminStateGet (L7_UNIT_CURRENT, keyInterfaceValue,
                                   &objAdminModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AdminMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAdminModeValue,
                           sizeof (objAdminModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingVoiceVlanInterfaceConfig_AdminMode
*
* @purpose Set 'AdminMode'
*
* @description [AdminMode]: The switch's Port Admin Mode 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingVoiceVlanInterfaceConfig_AdminMode (void *wap,
                                                               void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAdminModeValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AdminMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objAdminModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAdminModeValue, owa.len);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVoiceVlanInterfaceConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbIfAdminStateSet (L7_UNIT_CURRENT, keyInterfaceValue,
                                   objAdminModeValue);
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
* @function fpObjGet_SwitchingVoiceVlanInterfaceConfig_LinkTrapMode
*
* @purpose Get 'LinkTrapMode'
*
* @description [LinkTrapMode]: If enabled, link up and link down traps will
*              be sent for this port 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingVoiceVlanInterfaceConfig_LinkTrapMode (void *wap,
                                                                  void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objLinkTrapModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVoiceVlanInterfaceConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIfLinkUpDownTrapEnableGet (L7_UNIT_CURRENT, keyInterfaceValue,
                                             &objLinkTrapModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: LinkTrapMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objLinkTrapModeValue,
                           sizeof (objLinkTrapModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingVoiceVlanInterfaceConfig_LinkTrapMode
*
* @purpose Set 'LinkTrapMode'
*
* @description [LinkTrapMode]: If enabled, link up and link down traps will
*              be sent for this port 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingVoiceVlanInterfaceConfig_LinkTrapMode (void *wap,
                                                                  void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objLinkTrapModeValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: LinkTrapMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objLinkTrapModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objLinkTrapModeValue, owa.len);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVoiceVlanInterfaceConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbIfLinkUpDownTrapEnableSet (L7_UNIT_CURRENT, keyInterfaceValue,
                                             objLinkTrapModeValue);
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
* @function fpObjGet_SwitchingVoiceVlanInterfaceConfig_ClearStats
*
* @purpose Get 'ClearStats'
*
* @description [ClearStats]: clear stats for this port only 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingVoiceVlanInterfaceConfig_ClearStats (void *wap,
                                                                void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objClearStatsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVoiceVlanInterfaceConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  objClearStatsValue=0;
  /* return the object value: ClearStats */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objClearStatsValue,
                           sizeof (objClearStatsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingVoiceVlanInterfaceConfig_ClearStats
*
* @purpose Set 'ClearStats'
*
* @description [ClearStats]: clear stats for this port only 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingVoiceVlanInterfaceConfig_ClearStats (void *wap,
                                                                void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objClearStatsValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: ClearStats */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objClearStatsValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objClearStatsValue, owa.len);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVoiceVlanInterfaceConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbIntfStatReset (L7_UNIT_CURRENT, keyInterfaceValue);
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
* @function fpObjGet_SwitchingVoiceVlanInterfaceConfig_DefaultType
*
* @purpose Get 'DefaultType'
*
* @description [DefaultType]: This object identifies the default administrative
*              port type, to be used in conjunction with the operational
*              port type denoted by agentPortType. The set of possible
*              values for this object is the same as the set defined for
*              the agentPortType object. This ob 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingVoiceVlanInterfaceConfig_DefaultType (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objDefaultTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVoiceVlanInterfaceConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIfDefaultSpeedGet (L7_UNIT_CURRENT, keyInterfaceValue,
                                     (xLibU32_t *)objDefaultTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: DefaultType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objDefaultTypeValue,
                           strlen (objDefaultTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingVoiceVlanInterfaceConfig_DefaultType
*
* @purpose Set 'DefaultType'
*
* @description [DefaultType]: This object identifies the default administrative
*              port type, to be used in conjunction with the operational
*              port type denoted by agentPortType. The set of possible
*              values for this object is the same as the set defined for
*              the agentPortType object. This ob 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingVoiceVlanInterfaceConfig_DefaultType (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objDefaultTypeValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: DefaultType */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objDefaultTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objDefaultTypeValue, owa.len);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVoiceVlanInterfaceConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbIfDefaultSpeedSet (L7_UNIT_CURRENT, keyInterfaceValue,
                                     (xLibU32_t)objDefaultTypeValue);
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
* @function fpObjGet_SwitchingVoiceVlanInterfaceConfig_PortType
*
* @purpose Get 'PortType'
*
* @description [PortType]: This object identifies the port type. An initial
*              set of MAU types are defined in RFC 2668. The assignment
*              of OBJECT IDENTIFIERs to new types of MAUs is managed by the
*              IANA. If the MAU type is unknown, the object identifier unknownMauType
*              OBJECT IDENTIF 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingVoiceVlanInterfaceConfig_PortType (void *wap,
                                                              void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objPortTypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVoiceVlanInterfaceConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIntfConnectorTypeGet (keyInterfaceValue, (xLibU32_t *)objPortTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: PortType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objPortTypeValue,
                           strlen (objPortTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingVoiceVlanInterfaceConfig_AutoNegAdminStatus
*
* @purpose Get 'AutoNegAdminStatus'
*
* @description [AutoNegAdminStatus]: This object identifies the administration
*              status of auto negotiation for this port. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingVoiceVlanInterfaceConfig_AutoNegAdminStatus (void
                                                                        *wap,
                                                                        void
                                                                        *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAutoNegAdminStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVoiceVlanInterfaceConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIfAutoNegAdminStatusGet (L7_UNIT_CURRENT, keyInterfaceValue,
                                           &objAutoNegAdminStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AutoNegAdminStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAutoNegAdminStatusValue,
                           sizeof (objAutoNegAdminStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingVoiceVlanInterfaceConfig_AutoNegAdminStatus
*
* @purpose Set 'AutoNegAdminStatus'
*
* @description [AutoNegAdminStatus]: This object identifies the administration
*              status of auto negotiation for this port. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingVoiceVlanInterfaceConfig_AutoNegAdminStatus (void
                                                                        *wap,
                                                                        void
                                                                        *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAutoNegAdminStatusValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AutoNegAdminStatus */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objAutoNegAdminStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAutoNegAdminStatusValue, owa.len);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVoiceVlanInterfaceConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbIfAutoNegAdminStatusSet (L7_UNIT_CURRENT, keyInterfaceValue,
                                           objAutoNegAdminStatusValue);
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
* @function fpObjGet_SwitchingVoiceVlanInterfaceConfig_Dot3FlowControlMode
*
* @purpose Get 'Dot3FlowControlMode'
*
* @description [Dot3FlowControlMode]: Config flowcontrol allows you to enable
*              or disable 802.3x flow control for this port. This value
*              applies to only full-duplex mode ports. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingVoiceVlanInterfaceConfig_Dot3FlowControlMode (void
                                                                         *wap,
                                                                         void
                                                                         *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot3FlowControlModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVoiceVlanInterfaceConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIfFlowCtrlModeGet (L7_UNIT_CURRENT, keyInterfaceValue,
                                     &objDot3FlowControlModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Dot3FlowControlMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDot3FlowControlModeValue,
                           sizeof (objDot3FlowControlModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingVoiceVlanInterfaceConfig_Dot3FlowControlMode
*
* @purpose Set 'Dot3FlowControlMode'
*
* @description [Dot3FlowControlMode]: Config flowcontrol allows you to enable
*              or disable 802.3x flow control for this port. This value
*              applies to only full-duplex mode ports. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingVoiceVlanInterfaceConfig_Dot3FlowControlMode (void
                                                                         *wap,
                                                                         void
                                                                         *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDot3FlowControlModeValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Dot3FlowControlMode */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objDot3FlowControlModeValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDot3FlowControlModeValue, owa.len);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVoiceVlanInterfaceConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbIfFlowCtrlModeSet (L7_UNIT_CURRENT, keyInterfaceValue,
                                     objDot3FlowControlModeValue);
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
* @function fpObjGet_SwitchingVoiceVlanInterfaceConfig_DvlanTagMode
*
* @purpose Get 'DvlanTagMode'
*
* @description [DvlanTagMode]: Controls the Double Vlan Tag mode on this port.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingVoiceVlanInterfaceConfig_DvlanTagMode (void *wap,
                                                                  void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDvlanTagModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVoiceVlanInterfaceConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbDvlantagIntfModeGet (L7_UNIT_CURRENT, keyInterfaceValue,
                                       &objDvlanTagModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: DvlanTagMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDvlanTagModeValue,
                           sizeof (objDvlanTagModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingVoiceVlanInterfaceConfig_DvlanTagMode
*
* @purpose Set 'DvlanTagMode'
*
* @description [DvlanTagMode]: Controls the Double Vlan Tag mode on this port.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingVoiceVlanInterfaceConfig_DvlanTagMode (void *wap,
                                                                  void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDvlanTagModeValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: DvlanTagMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objDvlanTagModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDvlanTagModeValue, owa.len);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVoiceVlanInterfaceConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbDvlantagIntfModeSet (L7_UNIT_CURRENT, keyInterfaceValue,
                                       objDvlanTagModeValue);
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
* @function fpObjGet_SwitchingVoiceVlanInterfaceConfig_DVlanTagEthertype
*
* @purpose Get 'DVlanTagEthertype'
*
* @description [DVlanTagEthertype]: Configures the Double Vlan Tag Ethertype
*              for this port. If this object is supported, ethertype cannot
*              be configured globally for the switch. Commonly used are
*              the Ethertypes for vman tags (34984, or 0x88A8) and dot1q
*              tags (33024, or 0x8100) 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingVoiceVlanInterfaceConfig_DVlanTagEthertype (void
                                                                       *wap,
                                                                       void
                                                                       *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDVlanTagEthertypeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVoiceVlanInterfaceConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbDvlantagIntfEthertypeGet (L7_UNIT_CURRENT, keyInterfaceValue,
                                            &objDVlanTagEthertypeValue, L7_NULL);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: DVlanTagEthertype */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDVlanTagEthertypeValue,
                           sizeof (objDVlanTagEthertypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingVoiceVlanInterfaceConfig_DVlanTagEthertype
*
* @purpose Set 'DVlanTagEthertype'
*
* @description [DVlanTagEthertype]: Configures the Double Vlan Tag Ethertype
*              for this port. If this object is supported, ethertype cannot
*              be configured globally for the switch. Commonly used are
*              the Ethertypes for vman tags (34984, or 0x88A8) and dot1q
*              tags (33024, or 0x8100) 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingVoiceVlanInterfaceConfig_DVlanTagEthertype (void
                                                                       *wap,
                                                                       void
                                                                       *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDVlanTagEthertypeValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: DVlanTagEthertype */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objDVlanTagEthertypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDVlanTagEthertypeValue, owa.len);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVoiceVlanInterfaceConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbDvlantagIntfEthertypeSet (L7_UNIT_CURRENT, keyInterfaceValue,
                                            objDVlanTagEthertypeValue);
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
* @function fpObjGet_SwitchingVoiceVlanInterfaceConfig_DVlanTagCustomerId
*
* @purpose Get 'DVlanTagCustomerId'
*
* @description [DVlanTagCustomerId]: Configures the Customer ID for the Double
*              Vlan Tag for this port. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingVoiceVlanInterfaceConfig_DVlanTagCustomerId (void
                                                                        *wap,
                                                                        void
                                                                        *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDVlanTagCustomerIdValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVoiceVlanInterfaceConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbDvlantagIntfCustIdGet (L7_UNIT_CURRENT, keyInterfaceValue,
                                         &objDVlanTagCustomerIdValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: DVlanTagCustomerId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDVlanTagCustomerIdValue,
                           sizeof (objDVlanTagCustomerIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingVoiceVlanInterfaceConfig_DVlanTagCustomerId
*
* @purpose Set 'DVlanTagCustomerId'
*
* @description [DVlanTagCustomerId]: Configures the Customer ID for the Double
*              Vlan Tag for this port. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingVoiceVlanInterfaceConfig_DVlanTagCustomerId (void
                                                                        *wap,
                                                                        void
                                                                        *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDVlanTagCustomerIdValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: DVlanTagCustomerId */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objDVlanTagCustomerIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDVlanTagCustomerIdValue, owa.len);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVoiceVlanInterfaceConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbDvlantagIntfCustIdSet (L7_UNIT_CURRENT, keyInterfaceValue,
                                         objDVlanTagCustomerIdValue);
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
* @function fpObjGet_SwitchingVoiceVlanInterfaceConfig_MaxFrameSizeLimit
*
* @purpose Get 'MaxFrameSizeLimit'
*
* @description [MaxFrameSizeLimit]: This object identifies the largest value
*              that can be configured for MaxFrameSize 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingVoiceVlanInterfaceConfig_MaxFrameSizeLimit (void
                                                                       *wap,
                                                                       void
                                                                       *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMaxFrameSizeLimitValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVoiceVlanInterfaceConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIfConfigMaxFrameSizeGet ( keyInterfaceValue,
                                     &objMaxFrameSizeLimitValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: MaxFrameSizeLimit */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMaxFrameSizeLimitValue,
                           sizeof (objMaxFrameSizeLimitValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingVoiceVlanInterfaceConfig_MaxFrameSizeLimit
*
* @purpose Set 'MaxFrameSizeLimit'
*
* @description [MaxFrameSizeLimit]: This object identifies the largest value
*              that can be configured for MaxFrameSize 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingVoiceVlanInterfaceConfig_MaxFrameSizeLimit (void
                                                                       *wap,
                                                                       void
                                                                       *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMaxFrameSizeLimitValue;
  xLibU32_t maxSupported;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: MaxFrameSizeLimit */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objMaxFrameSizeLimitValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMaxFrameSizeLimitValue, owa.len);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVoiceVlanInterfaceConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  owa.l7rc = usmDbIfMaxFrameSizeGet( keyInterfaceValue, &maxSupported);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  if ( objMaxFrameSizeLimitValue > maxSupported )
  {
    owa.rc = XLIBRC_CROSS_BOUNDS;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  /* set the value in application */
  owa.l7rc = usmDbIfConfigMaxFrameSizeSet ( keyInterfaceValue,
                              objMaxFrameSizeLimitValue);
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
* @function fpObjGet_SwitchingVoiceVlanInterfaceConfig_MaxFrameSize
*
* @purpose Get 'MaxFrameSize'
*
* @description [MaxFrameSize]: This object identifies the currently configured
*              maximum frame size value for this port. The maximmum value
*              that this object can be set to is the value of agentPortMaxFrameSizeLimit.
*              For Ethernet ports which support 802.1Q
*              vlan tagging, the minimum value 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingVoiceVlanInterfaceConfig_MaxFrameSize (void *wap,
                                                                  void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMaxFrameSizeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVoiceVlanInterfaceConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbIfConfigMaxFrameSizeGet ( keyInterfaceValue,
                                           &objMaxFrameSizeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: MaxFrameSize */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMaxFrameSizeValue,
                           sizeof (objMaxFrameSizeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingVoiceVlanInterfaceConfig_MaxFrameSize
*
* @purpose Set 'MaxFrameSize'
*
* @description [MaxFrameSize]: This object identifies the currently configured
*              maximum frame size value for this port. The maximmum value
*              that this object can be set to is the value of agentPortMaxFrameSizeLimit.
*              For Ethernet ports which support 802.1Q
*              vlan tagging, the minimum value 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingVoiceVlanInterfaceConfig_MaxFrameSize (void *wap,
                                                                  void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMaxFrameSizeValue;
  xLibU32_t maxSupported;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: MaxFrameSize */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objMaxFrameSizeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMaxFrameSizeValue, owa.len);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVoiceVlanInterfaceConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  owa.l7rc = usmDbIfMaxFrameSizeGet( keyInterfaceValue, &maxSupported);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  if ( objMaxFrameSizeValue > maxSupported )
  {
    owa.rc = XLIBRC_CROSS_BOUNDS;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  /* set the value in application */
  owa.l7rc = usmDbIfConfigMaxFrameSizeSet ( keyInterfaceValue,
                                           objMaxFrameSizeValue);
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
* @function fpObjGet_SwitchingVoiceVlanInterfaceConfig_BroadcastControlMode
*
* @purpose Get 'BroadcastControlMode'
*
* @description [BroadcastControlMode]: If enabled, broadcast storm recovery
*              will function on this port. When you specify Enable for Broadcast
*              Storm Recovery and the broadcast traffic on this Ethernet
*              port exceeds the configured threshold, the switch blocks
*              (discards) the broadcast traffic. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingVoiceVlanInterfaceConfig_BroadcastControlMode (void
                                                                          *wap,
                                                                          void
                                                                          *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objBroadcastControlModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVoiceVlanInterfaceConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbSwDevCtrlBcastStormModeIntfGet ( keyInterfaceValue,
                                         &objBroadcastControlModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: BroadcastControlMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objBroadcastControlModeValue,
                           sizeof (objBroadcastControlModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingVoiceVlanInterfaceConfig_BroadcastControlMode
*
* @purpose Set 'BroadcastControlMode'
*
* @description [BroadcastControlMode]: If enabled, broadcast storm recovery
*              will function on this port. When you specify Enable for Broadcast
*              Storm Recovery and the broadcast traffic on this Ethernet
*              port exceeds the configured threshold, the switch blocks
*              (discards) the broadcast traffic. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingVoiceVlanInterfaceConfig_BroadcastControlMode (void
                                                                          *wap,
                                                                          void
                                                                          *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objBroadcastControlModeValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: BroadcastControlMode */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objBroadcastControlModeValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objBroadcastControlModeValue, owa.len);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVoiceVlanInterfaceConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbSwDevCtrlBcastStormModeIntfSet ( keyInterfaceValue,
                                         objBroadcastControlModeValue);
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
* @function fpObjGet_SwitchingVoiceVlanInterfaceConfig_BroadcastControlThreshold
*
* @purpose Get 'BroadcastControlThreshold'
*
* @description [BroadcastControlThreshold]: Configures the broadcast storm
*              recovery threshold for this port as a percentage of port speed.
*              
*
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_SwitchingVoiceVlanInterfaceConfig_BroadcastControlThreshold (void *wap,
                                                                      void
                                                                      *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objBroadcastControlThresholdValue;
  L7_RATE_UNIT_t rate_unit;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVoiceVlanInterfaceConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbSwDevCtrlBcastStormThresholdIntfGet ( keyInterfaceValue,
                                              &objBroadcastControlThresholdValue, &rate_unit);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: BroadcastControlThreshold */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objBroadcastControlThresholdValue,
                    sizeof (objBroadcastControlThresholdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingVoiceVlanInterfaceConfig_BroadcastControlThreshold
*
* @purpose Set 'BroadcastControlThreshold'
*
* @description [BroadcastControlThreshold]: Configures the broadcast storm
*              recovery threshold for this port as a percentage of port speed.
*              
*
* @return
*******************************************************************************/
xLibRC_t
fpObjSet_SwitchingVoiceVlanInterfaceConfig_BroadcastControlThreshold (void *wap,
                                                                      void
                                                                      *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objBroadcastControlThresholdValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  xLibU32_t tempBroadcastControlThresholdValue;
  xLibU32_t objBroadcastControlThresholdUnitValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: BroadcastControlThreshold */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objBroadcastControlThresholdValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objBroadcastControlThresholdValue, owa.len);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVoiceVlanInterfaceConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  owa.l7rc =
    usmDbSwDevCtrlBcastStormThresholdIntfGet (keyInterfaceValue,
                                                                        &tempBroadcastControlThresholdValue, 
                                                                        &objBroadcastControlThresholdUnitValue);

  if (owa.l7rc == L7_SUCCESS)
  {
    /* set the value in application */
    owa.l7rc =
      usmDbSwDevCtrlBcastStormThresholdIntfSet (keyInterfaceValue,
                                                                          objBroadcastControlThresholdValue,
                                                                          objBroadcastControlThresholdUnitValue);
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
* @function fpObjGet_SwitchingVoiceVlanInterfaceConfig_MulticastControlMode
*
* @purpose Get 'MulticastControlMode'
*
* @description [MulticastControlMode]: If enabled, multicast storm recovery
*              will function on this port.When you specify Enable for Multicast
*              Storm Recovery and the multicast traffic on this Ethernet
*              port exceeds the configured threshold, the switch blocks
*              (discards) the multicast traffic. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingVoiceVlanInterfaceConfig_MulticastControlMode (void
                                                                          *wap,
                                                                          void
                                                                          *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMulticastControlModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVoiceVlanInterfaceConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbSwDevCtrlMcastStormModeIntfGet ( keyInterfaceValue,
                                         &objMulticastControlModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: MulticastControlMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMulticastControlModeValue,
                           sizeof (objMulticastControlModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingVoiceVlanInterfaceConfig_MulticastControlMode
*
* @purpose Set 'MulticastControlMode'
*
* @description [MulticastControlMode]: If enabled, multicast storm recovery
*              will function on this port.When you specify Enable for Multicast
*              Storm Recovery and the multicast traffic on this Ethernet
*              port exceeds the configured threshold, the switch blocks
*              (discards) the multicast traffic. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingVoiceVlanInterfaceConfig_MulticastControlMode (void
                                                                          *wap,
                                                                          void
                                                                          *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMulticastControlModeValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: MulticastControlMode */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objMulticastControlModeValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMulticastControlModeValue, owa.len);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVoiceVlanInterfaceConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbSwDevCtrlMcastStormModeIntfSet ( keyInterfaceValue,
                                         objMulticastControlModeValue);
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
* @function fpObjGet_SwitchingVoiceVlanInterfaceConfig_MulticastControlThreshold
*
* @purpose Get 'MulticastControlThreshold'
*
* @description [MulticastControlThreshold]: Configures the multicast storm
*              recovery threshold for this port as a percentage of port speed.
*              
*
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_SwitchingVoiceVlanInterfaceConfig_MulticastControlThreshold (void *wap,
                                                                      void
                                                                      *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMulticastControlThresholdValue;
  L7_RATE_UNIT_t rate_unit;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVoiceVlanInterfaceConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbSwDevCtrlMcastStormThresholdIntfGet (keyInterfaceValue,
                                                                        &objMulticastControlThresholdValue,
                                                                        &rate_unit);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: MulticastControlThreshold */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objMulticastControlThresholdValue,
                    sizeof (objMulticastControlThresholdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingVoiceVlanInterfaceConfig_MulticastControlThreshold
*
* @purpose Set 'MulticastControlThreshold'
*
* @description [MulticastControlThreshold]: Configures the multicast storm
*              recovery threshold for this port as a percentage of port speed.
*              
*
* @return
*******************************************************************************/
xLibRC_t
fpObjSet_SwitchingVoiceVlanInterfaceConfig_MulticastControlThreshold (void *wap,
                                                                      void
                                                                      *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMulticastControlThresholdValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  xLibU32_t tempMulticastControlThresholdValue;
  xLibU32_t objMulticastControlThresholdUnitValue;


  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: MulticastControlThreshold */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objMulticastControlThresholdValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMulticastControlThresholdValue, owa.len);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVoiceVlanInterfaceConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  owa.l7rc =
    usmDbSwDevCtrlMcastStormThresholdIntfGet (keyInterfaceValue,
                                                                        &tempMulticastControlThresholdValue,
                                                                        &objMulticastControlThresholdUnitValue);
  if (owa.l7rc == L7_SUCCESS)
  {
    /* set the value in application */
    owa.l7rc =
      usmDbSwDevCtrlMcastStormThresholdIntfSet (keyInterfaceValue,
                                                                          objMulticastControlThresholdValue,
                                                                          objMulticastControlThresholdUnitValue);
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
* @function fpObjGet_SwitchingVoiceVlanInterfaceConfig_UnicastControlMode
*
* @purpose Get 'UnicastControlMode'
*
* @description [UnicastControlMode]: If enabled, unicast storm recovery will
*              function on this port.When you specify Enable for Unicast
*              Storm Recovery and the unknown unicast traffic on this Ethernet
*              port exceeds the configured threshold, the switch blocks
*              (discards) the unknown unicast traff 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingVoiceVlanInterfaceConfig_UnicastControlMode (void
                                                                        *wap,
                                                                        void
                                                                        *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUnicastControlModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVoiceVlanInterfaceConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbSwDevCtrlUcastStormModeIntfGet ( keyInterfaceValue,
                                         &objUnicastControlModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: UnicastControlMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objUnicastControlModeValue,
                           sizeof (objUnicastControlModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingVoiceVlanInterfaceConfig_UnicastControlMode
*
* @purpose Set 'UnicastControlMode'
*
* @description [UnicastControlMode]: If enabled, unicast storm recovery will
*              function on this port.When you specify Enable for Unicast
*              Storm Recovery and the unknown unicast traffic on this Ethernet
*              port exceeds the configured threshold, the switch blocks
*              (discards) the unknown unicast traff 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingVoiceVlanInterfaceConfig_UnicastControlMode (void
                                                                        *wap,
                                                                        void
                                                                        *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUnicastControlModeValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: UnicastControlMode */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objUnicastControlModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objUnicastControlModeValue, owa.len);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVoiceVlanInterfaceConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbSwDevCtrlUcastStormModeIntfSet ( keyInterfaceValue,
                                         objUnicastControlModeValue);
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
* @function fpObjGet_SwitchingVoiceVlanInterfaceConfig_UnicastControlThreshold
*
* @purpose Get 'UnicastControlThreshold'
*
* @description [UnicastControlThreshold]: Configures the unicast storm recovery
*              threshold for this port as a percentage of port speed.
*              
*
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_SwitchingVoiceVlanInterfaceConfig_UnicastControlThreshold (void *wap,
                                                                    void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUnicastControlThresholdValue;
  L7_RATE_UNIT_t rate_unit;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVoiceVlanInterfaceConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbSwDevCtrlUcastStormThresholdIntfGet (keyInterfaceValue,
                                                                        &objUnicastControlThresholdValue,
                                                                        &rate_unit);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: UnicastControlThreshold */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objUnicastControlThresholdValue,
                           sizeof (objUnicastControlThresholdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingVoiceVlanInterfaceConfig_UnicastControlThreshold
*
* @purpose Set 'UnicastControlThreshold'
*
* @description [UnicastControlThreshold]: Configures the unicast storm recovery
*              threshold for this port as a percentage of port speed.
*              
*
* @return
*******************************************************************************/
xLibRC_t
fpObjSet_SwitchingVoiceVlanInterfaceConfig_UnicastControlThreshold (void *wap,
                                                                    void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objUnicastControlThresholdValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;

  xLibU32_t tempUnicastControlThresholdValue;
  xLibU32_t objUnicastControlThresholdUnitValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: UnicastControlThreshold */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objUnicastControlThresholdValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objUnicastControlThresholdValue, owa.len);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVoiceVlanInterfaceConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  owa.l7rc =
    usmDbSwDevCtrlUcastStormThresholdIntfGet (keyInterfaceValue,
                                                                        &tempUnicastControlThresholdValue,
                                                                        &objUnicastControlThresholdUnitValue);
                                                                        
  if (owa.l7rc != L7_SUCCESS)
  {
    /* set the value in application */
    owa.l7rc =
      usmDbSwDevCtrlUcastStormThresholdIntfSet ( keyInterfaceValue,
                                                objUnicastControlThresholdValue,
                                                objUnicastControlThresholdUnitValue);
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
#endif /* Invalid Objects */

#if 0
/*******************************************************************************
* @function fpObjGet_SwitchingVoiceVlanInterfaceConfig_VoiceVlanMode
*
* @purpose Get 'VoiceVlanMode'
*
* @description [VoiceVlanMode]: Describes and Configures the Port Voice VLAN
*              Mode 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingVoiceVlanInterfaceConfig_VoiceVlanMode (void *wap,
                                                                   void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objVoiceVlanModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVoiceVlanInterfaceConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmdbVoiceVlanPortAdminModeGet (L7_UNIT_CURRENT, keyInterfaceValue,
                                             &objVoiceVlanModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: VoiceVlanMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objVoiceVlanModeValue,
                           sizeof (objVoiceVlanModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingVoiceVlanInterfaceConfig_VoiceVlanMode
*
* @purpose Set 'VoiceVlanMode'
*
* @description [VoiceVlanMode]: Describes and Configures the Port Voice VLAN
*              Mode 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingVoiceVlanInterfaceConfig_VoiceVlanMode (void *wap,
                                                                   void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objVoiceVlanModeValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: VoiceVlanMode */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objVoiceVlanModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objVoiceVlanModeValue, owa.len);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVoiceVlanInterfaceConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmdbvoiceVlanPortAdminModeSet (L7_UNIT_CURRENT, keyInterfaceValue,
                                             VOICE_VLAN_CONFIG_DISABLE,
                                             objVoiceVlanModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
#endif

/*******************************************************************************
* @function fpObjGet_SwitchingVoiceVlanInterfaceConfig_VoiceVlanMode
*
* @purpose Get 'VoiceVlanMode'
 *@description  [VoiceVlanMode] Describes and Configures the Port Voice VLAN Mode
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingVoiceVlanInterfaceConfig_VoiceVlanMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objVoiceVlanModeValue;

  xLibU32_t keyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingVoiceVlanInterfaceConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmdbVoiceVlanPortAdminModeGet(L7_UNIT_CURRENT, keyInterfaceValue, &objVoiceVlanModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objVoiceVlanModeValue, sizeof (objVoiceVlanModeValue));

  /* return the object value: VoiceVlanMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objVoiceVlanModeValue,
                           sizeof (objVoiceVlanModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingVoiceVlanInterfaceConfig_VoiceVlanMode
*
* @purpose Set 'VoiceVlanMode'
 *@description  [VoiceVlanMode] Describes and Configures the Port Voice VLAN Mode
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingVoiceVlanInterfaceConfig_VoiceVlanMode (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objVoiceVlanModeValue,objVoiceVlanModeValueValue;

  xLibU32_t keyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: VoiceVlanMode */
  owa.len = sizeof (objVoiceVlanModeValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objVoiceVlanModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objVoiceVlanModeValue, owa.len);

   /* retrieve object: VoiceVlanModeValue */
  
  if (objVoiceVlanModeValue==  XUI_VOICE_VLAN_CONFIG_VLAN_ID || (objVoiceVlanModeValue== XUI_VOICE_VLAN_CONFIG_DOT1P) )
  {

    owa.len = sizeof (objVoiceVlanModeValueValue);
    owa.rc = xLibFilterGet(wap, XOBJ_SwitchingVoiceVlanInterfaceConfig_VoiceVlanModeValue,
                        (xLibU8_t *) & objVoiceVlanModeValueValue, &owa.len);
    if (owa.rc != XLIBRC_SUCCESS)
    {
      owa.rc = XLIBRC_FILTER_MISSING;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    FPOBJ_TRACE_VALUE (bufp, &objVoiceVlanModeValueValue, owa.len);

    if (objVoiceVlanModeValue == XUI_VOICE_VLAN_CONFIG_VLAN_ID)
    {
      if (objVoiceVlanModeValueValue < L7_DOT1Q_MIN_VLAN_ID ||
         (objVoiceVlanModeValueValue > L7_DOT1Q_MAX_VLAN_ID ))
      {
        owa.rc = XLIBRC_CONFIGPORTVOICEVLAN_VLAN;
        FPOBJ_TRACE_EXIT (bufp, owa);
       return owa.rc;
      }
      if(objVoiceVlanModeValueValue != L7_NULL)
      {
        if( usmDbVlanIDGet(L7_UNIT_CURRENT,objVoiceVlanModeValueValue) != L7_SUCCESS)
        {
          owa.rc = XLIBRC_VLAN_ID_DOES_NOT_EXISTS;
          FPOBJ_TRACE_EXIT (bufp, owa);
         return owa.rc;
        } 
      }
    }
    else if (objVoiceVlanModeValue == XUI_VOICE_VLAN_CONFIG_DOT1P)
    {
       if ((objVoiceVlanModeValueValue < L7_DOT1P_MIN_PRIORITY) ||
           ( objVoiceVlanModeValueValue > L7_DOT1P_MAX_PRIORITY))
       {
         owa.rc = XLIBRC_CONFIGPORTVOICEVLAN_DOT1P;
        FPOBJ_TRACE_EXIT (bufp, owa);
       return owa.rc;
      }
    }


  }
  else 
  {
    objVoiceVlanModeValueValue = 0;
  }

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingVoiceVlanInterfaceConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmdbvoiceVlanPortAdminModeSet(L7_UNIT_CURRENT, keyInterfaceValue, objVoiceVlanModeValue, objVoiceVlanModeValueValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingVoiceVlanInterfaceConfig_VoiceVlanID
*
* @purpose Get 'VoiceVlanID'
*
* @description [VoiceVlanID]: Describes and Configures the Port Voice VLAN
*              ID if the Voice Vlan Mode is Vlan ID 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingVoiceVlanInterfaceConfig_VoiceVlanID (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objVoiceVlanIDValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVoiceVlanInterfaceConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmdbVoiceVlanPortVlanIdGet (L7_UNIT_CURRENT, keyInterfaceValue,
                                          &objVoiceVlanIDValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: VoiceVlanID */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objVoiceVlanIDValue,
                           sizeof (objVoiceVlanIDValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingVoiceVlanInterfaceConfig_VoiceVlanID
*
* @purpose Set 'VoiceVlanID'
*
* @description [VoiceVlanID]: Describes and Configures the Port Voice VLAN
*              ID if the Voice Vlan Mode is Vlan ID 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingVoiceVlanInterfaceConfig_VoiceVlanID (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objVoiceVlanIDValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: VoiceVlanID */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objVoiceVlanIDValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objVoiceVlanIDValue, owa.len);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVoiceVlanInterfaceConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmdbVoiceVlanPortVlanIdSet (L7_UNIT_CURRENT, keyInterfaceValue,
                                          objVoiceVlanIDValue);
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
* @function fpObjGet_SwitchingVoiceVlanInterfaceConfig_VoiceVlanPriority
*
* @purpose Get 'VoiceVlanPriority'
*
* @description [VoiceVlanPriority]: Describes and Configures the Port Voice
*              VLAN Priority if the Voice Vlan Mode is dot1p. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingVoiceVlanInterfaceConfig_VoiceVlanPriority (void
                                                                       *wap,
                                                                       void
                                                                       *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objVoiceVlanPriorityValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVoiceVlanInterfaceConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmdbVoiceVlanPortVlanPriorityGet (L7_UNIT_CURRENT, keyInterfaceValue,
                                       &objVoiceVlanPriorityValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: VoiceVlanPriority */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objVoiceVlanPriorityValue,
                           sizeof (objVoiceVlanPriorityValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingVoiceVlanInterfaceConfig_VoiceVlanPriority
*
* @purpose Set 'VoiceVlanPriority'
*
* @description [VoiceVlanPriority]: Describes and Configures the Port Voice
*              VLAN Priority if the Voice Vlan Mode is dot1p. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingVoiceVlanInterfaceConfig_VoiceVlanPriority (void
                                                                       *wap,
                                                                       void
                                                                       *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objVoiceVlanPriorityValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: VoiceVlanPriority */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objVoiceVlanPriorityValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objVoiceVlanPriorityValue, owa.len);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVoiceVlanInterfaceConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* set the value in application */
  owa.l7rc =
    usmdbVoiceVlanPortVlanPrioritySet (L7_UNIT_CURRENT, keyInterfaceValue,
                                       objVoiceVlanPriorityValue);
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
* @function fpObjGet_SwitchingVoiceVlanInterfaceConfig_VoiceVlanDataPriorityMode
*
* @purpose Get 'VoiceVlanDataPriorityMode'
*
* @description [VoiceVlanDataPriorityMode]: Describes and Configures the Port
*              Voice VLAN Data Priority Mode 
*
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_SwitchingVoiceVlanInterfaceConfig_VoiceVlanDataPriorityMode (void *wap,
                                                                      void
                                                                      *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objVoiceVlanDataPriorityModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVoiceVlanInterfaceConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmdbVoiceVlanPortCosOverrideGet (L7_UNIT_CURRENT, keyInterfaceValue,
                                      &objVoiceVlanDataPriorityModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: VoiceVlanDataPriorityMode */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objVoiceVlanDataPriorityModeValue,
                    sizeof (objVoiceVlanDataPriorityModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingVoiceVlanInterfaceConfig_VoiceVlanDataPriorityMode
*
* @purpose Set 'VoiceVlanDataPriorityMode'
*
* @description [VoiceVlanDataPriorityMode]: Describes and Configures the Port
*              Voice VLAN Data Priority Mode 
*
* @return
*******************************************************************************/
xLibRC_t
fpObjSet_SwitchingVoiceVlanInterfaceConfig_VoiceVlanDataPriorityMode (void *wap,
                                                                      void
                                                                      *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objVoiceVlanDataPriorityModeValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: VoiceVlanDataPriorityMode */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objVoiceVlanDataPriorityModeValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objVoiceVlanDataPriorityModeValue, owa.len);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVoiceVlanInterfaceConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* set the value in application */
  owa.l7rc =
    usmdbVoiceVlanPortCosOverrideSet (L7_UNIT_CURRENT, keyInterfaceValue,
                                      objVoiceVlanDataPriorityModeValue);
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
* @function fpObjGet_SwitchingVoiceVlanInterfaceConfig_VoiceVlanDscp
*
* @purpose Get 'VoiceVlanDscp'
 *@description  [VoiceVlanDscp] Describes and Configures the Port Voice VLAN
* dscp if the Voice Vlan Mode is dscp.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingVoiceVlanInterfaceConfig_VoiceVlanDscp (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objVoiceVlanDscpValue;

  xLibU32_t keyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingVoiceVlanInterfaceConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmdbVoiceVlanPortDscpGet(L7_UNIT_CURRENT, keyInterfaceValue, &objVoiceVlanDscpValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objVoiceVlanDscpValue, sizeof (objVoiceVlanDscpValue));

  /* return the object value: VoiceVlanDscp */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objVoiceVlanDscpValue,
                           sizeof (objVoiceVlanDscpValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingVoiceVlanInterfaceConfig_VoiceVlanDscp
*
* @purpose Set 'VoiceVlanDscp'
 *@description  [VoiceVlanDscp] Describes and Configures the Port Voice VLAN
* dscp if the Voice Vlan Mode is dscp.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingVoiceVlanInterfaceConfig_VoiceVlanDscp (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objVoiceVlanDscpValue;

  xLibU32_t keyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: VoiceVlanDscp */
  owa.len = sizeof (objVoiceVlanDscpValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objVoiceVlanDscpValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objVoiceVlanDscpValue, owa.len);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingVoiceVlanInterfaceConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmdbVoiceVlanPortDscpSet(L7_UNIT_CURRENT, keyInterfaceValue, objVoiceVlanDscpValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingVoiceVlanInterfaceConfig_VoiceVlanOperationalStatus
*
* @purpose Get 'VoiceVlanOperationalStatus'
*
* @description [VoiceVlanOperationalStatus]: Describes the Port Voice VLAN
*              Operational Status 
*
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_SwitchingVoiceVlanInterfaceConfig_VoiceVlanOperationalStatus (void
                                                                       *wap,
                                                                       void
                                                                       *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objVoiceVlanOperationalStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVoiceVlanInterfaceConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmdbVoiceVlanPorStatusGet (L7_UNIT_CURRENT, keyInterfaceValue,
                                         &objVoiceVlanOperationalStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: VoiceVlanOperationalStatus */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objVoiceVlanOperationalStatusValue,
                    sizeof (objVoiceVlanOperationalStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingVoiceVlanInterfaceConfig_PortLACPMode
*
* @purpose Get 'PortLACPMode'
*
* @description [PortLACPMode]: nable/disable 802.3ad LACP on this port 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingVoiceVlanInterfaceConfig_PortLACPMode (void *wap,
                                                                  void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPortLACPModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVoiceVlanInterfaceConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbDot3adAggPortLacpModeGet (L7_UNIT_CURRENT, keyInterfaceValue,
                                            &objPortLACPModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: PortLACPMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPortLACPModeValue,
                           sizeof (objPortLACPModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_SwitchingVoiceVlanInterfaceConfig_PortLACPMode
*
* @purpose Set 'PortLACPMode'
*
* @description [PortLACPMode]: nable/disable 802.3ad LACP on this port 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingVoiceVlanInterfaceConfig_PortLACPMode (void *wap,
                                                                  void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objPortLACPModeValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyInterfaceValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: PortLACPMode */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objPortLACPModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPortLACPModeValue, owa.len);

  /* retrieve key: Interface */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingVoiceVlanInterfaceConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, kwa.len);

  /* set the value in application */
  owa.l7rc = usmDbDot3adAggPortLacpModeSet (L7_UNIT_CURRENT, keyInterfaceValue,
                                            objPortLACPModeValue);
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
* @function fpObjGet_SwitchingVoiceVlanInterfaceConfig_PortAuthMode
*
* @purpose Get 'PortAuthMode'
 *@description  [PortAuthMode] enable/disable authentication on this port   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingVoiceVlanInterfaceConfig_PortAuthMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objPortAuthModeValue;

  xLibU32_t keyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingVoiceVlanInterfaceConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmdbVoiceVlanPortAuthGet(L7_UNIT_CURRENT, keyInterfaceValue, &objPortAuthModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objPortAuthModeValue, sizeof (objPortAuthModeValue));

  /* return the object value: PortAuthMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objPortAuthModeValue,
                           sizeof (objPortAuthModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingVoiceVlanInterfaceConfig_PortAuthMode
*
* @purpose Set 'PortAuthMode'
 *@description  [PortAuthMode] enable/disable authentication on this port   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingVoiceVlanInterfaceConfig_PortAuthMode (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objPortAuthModeValue;

  xLibU32_t keyInterfaceValue;
  /*xLibU32_t objswitchModeValue;*/

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: PortAuthMode */
  owa.len = sizeof (objPortAuthModeValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objPortAuthModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objPortAuthModeValue, owa.len);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingVoiceVlanInterfaceConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);
  owa.rc = XLIBRC_SUCCESS;

  #if 0
  /* Get the mode first */
  if (FD_VOICE_VLAN_AUTH_STATE != objPortAuthModeValue)
  {
    owa.l7rc = usmDbDot1qSwPortModeGet(L7_UNIT_CURRENT, keyInterfaceValue, &objswitchModeValue);
    if(owa.l7rc == L7_SUCCESS)
    {
      if(objswitchModeValue != DOT1Q_SWPORT_MODE_GENERAL)
      {
        owa.rc = XLIBRC_SWPORT_NOTGENERALMODE_DISPLAY;
      }
    }
    else
    {
        owa.rc = XLIBRC_SWPORT_MODEGET_FAILURE_DISPLAY;
    }

    if (owa.rc != XLIBRC_SUCCESS)
    {
      return owa.rc;
    }
  }
  #endif
  /* set the value in application */
  owa.l7rc = usmdbVoiceVlanPortAuthSet(L7_UNIT_CURRENT, keyInterfaceValue, objPortAuthModeValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    switch(owa.l7rc)
    {
      case L7_NOT_SUPPORTED:
        owa.rc = XLIBRC_VOICEVLANAUTH_ERR_DOT1X_DISABLED;
        break;
      
      case L7_REQUEST_DENIED:
        owa.rc = XLIBRC_VOICEVLANAUTH_ERR_NOT_MAC_BASED;
        break;
      
      default:
        owa.rc = XLIBRC_VOICEVLANAUTH_ERR;
        break;
    }
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingVoiceVlanInterfaceConfig_VoiceVlanModeValue
*
* @purpose Get 'VoiceVlanModeValue'
 *@description  [VoiceVlanModeValue] Describes and Configures the Port Voice
* VLAN value or priority value based on the voice vlan mode
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingVoiceVlanInterfaceConfig_VoiceVlanModeValue (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objVoiceVlanModeValueValue, val;

  xLibU32_t keyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingVoiceVlanInterfaceConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmdbVoiceVlanPortAdminModeGet(L7_UNIT_CURRENT, keyInterfaceValue, &val);
  
  if (owa.l7rc == L7_SUCCESS)
  {
    if (val == XUI_VOICE_VLAN_CONFIG_VLAN_ID) 
    {
      owa.l7rc = usmdbVoiceVlanPortVlanIdGet(L7_UNIT_CURRENT, keyInterfaceValue, &objVoiceVlanModeValueValue);
    }
    else if (XUI_VOICE_VLAN_CONFIG_DOT1P == val) 
    {
      owa.l7rc = usmdbVoiceVlanPortVlanPriorityGet(L7_UNIT_CURRENT, keyInterfaceValue, &objVoiceVlanModeValueValue);
    }
    else
    {
      objVoiceVlanModeValueValue = 0;
    }    
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objVoiceVlanModeValueValue, sizeof (objVoiceVlanModeValueValue));

  /* return the object value: VoiceVlanModeValue */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objVoiceVlanModeValueValue,
                           sizeof (objVoiceVlanModeValueValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingVoiceVlanInterfaceConfig_VoiceVlanModeValue
*
* @purpose Set 'VoiceVlanModeValue'
 *@description  [VoiceVlanModeValue] Describes and Configures the Port Voice
* VLAN value or priority value based on the voice vlan mode
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingVoiceVlanInterfaceConfig_VoiceVlanModeValue (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objVoiceVlanModeValueValue;

  xLibU32_t keyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: VoiceVlanModeValue */
  owa.len = sizeof (objVoiceVlanModeValueValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objVoiceVlanModeValueValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objVoiceVlanModeValueValue, owa.len);

  /* retrieve key: Interface 
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingVoiceVlanInterfaceConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
*/
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* set the value in application 
  owa.l7rc = usmDbSetUnknown (L7_UNIT_CURRENT, keyInterfaceValue, objVoiceVlanModeValueValue);
*/
  owa.l7rc = L7_SUCCESS;
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
/*******************************************************************************
* @function fpObjGet_SwitchingVoiceVlanInterfaceConfig_voiceVlanPortDSCP
*
* @purpose Get 'voiceVlanPortDSCP'
 *@description  [voiceVlanPortDSCP] <HTML>The value of the dscp value   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingVoiceVlanInterfaceConfig_voiceVlanPortDSCP (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objvoiceVlanPortDSCPValue;

  xLibU32_t keyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingVoiceVlanInterfaceConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmdbVoiceVlanPortDscpGet(L7_UNIT_CURRENT, keyInterfaceValue, &objvoiceVlanPortDSCPValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objvoiceVlanPortDSCPValue, sizeof (objvoiceVlanPortDSCPValue));

  /* return the object value: voiceVlanPortDSCP */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objvoiceVlanPortDSCPValue,
                           sizeof (objvoiceVlanPortDSCPValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingVoiceVlanInterfaceConfig_voiceVlanPortDSCP
*
* @purpose Set 'voiceVlanPortDSCP'
 *@description  [voiceVlanPortDSCP] <HTML>The value of the dscp value   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingVoiceVlanInterfaceConfig_voiceVlanPortDSCP (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objvoiceVlanPortDSCPValue;

  xLibU32_t keyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: voiceVlanPortDSCP */
  owa.len = sizeof (objvoiceVlanPortDSCPValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objvoiceVlanPortDSCPValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objvoiceVlanPortDSCPValue, owa.len);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingVoiceVlanInterfaceConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmdbVoiceVlanPortDscpSet(L7_UNIT_CURRENT, keyInterfaceValue, objvoiceVlanPortDSCPValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingVoiceVlanInterfaceConfig_voiceVlanAuthMode
*
* @purpose Get 'voiceVlanAuthMode'
 *@description  [voiceVlanAuthMode] <HTML>Enable/Disable Auth State for Voice
* Vlan   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingVoiceVlanInterfaceConfig_voiceVlanAuthMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objvoiceVlanAuthModeValue;

  xLibU32_t keyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingVoiceVlanInterfaceConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmdbVoiceVlanPortAuthGet (L7_UNIT_CURRENT, keyInterfaceValue, &objvoiceVlanAuthModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objvoiceVlanAuthModeValue, sizeof (objvoiceVlanAuthModeValue));

  /* return the object value: voiceVlanAuthMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objvoiceVlanAuthModeValue,
                           sizeof (objvoiceVlanAuthModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingVoiceVlanInterfaceConfig_voiceVlanAuthMode
*
* @purpose Set 'voiceVlanAuthMode'
* @description  [voiceVlanAuthMode] <HTML>Enable/Disable Auth State for Voice
* Vlan   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingVoiceVlanInterfaceConfig_voiceVlanAuthMode (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objvoiceVlanAuthModeValue;

  xLibU32_t keyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: voiceVlanAuthMode */
  owa.len = sizeof (objvoiceVlanAuthModeValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objvoiceVlanAuthModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objvoiceVlanAuthModeValue, owa.len);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingVoiceVlanInterfaceConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmdbVoiceVlanPortAuthSet (L7_UNIT_CURRENT, keyInterfaceValue, objvoiceVlanAuthModeValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* fpObjGet_SwitchingVoiceVlanInterfaceConfig_voiceVlanDeviceInfo
*
* @purpose Get 'voiceVlanDeviceInfo'
* @description  [voiceVlanDeviceInfo] <HTML>Get the voip device identified on
* the port   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingVoiceVlanInterfaceConfig_voiceVlanDeviceInfo (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibStr6_t objvoiceVlanDeviceInfoValue;
  xLibStr6_t nextObjvoiceVlanDeviceInfoValue;

  xLibU32_t keyInterfaceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: Interface */
  owa.len = sizeof (keyInterfaceValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingVoiceVlanInterfaceConfig_Interface,
                          (xLibU8_t *) & keyInterfaceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyInterfaceValue, owa.len);

  memset(objvoiceVlanDeviceInfoValue,0x00,sizeof(objvoiceVlanDeviceInfoValue));
  memset(nextObjvoiceVlanDeviceInfoValue,0x00,sizeof(nextObjvoiceVlanDeviceInfoValue));

  /* retrieve key: voiceVlanDeviceInfo */
  owa.len = sizeof (objvoiceVlanDeviceInfoValue);
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingVoiceVlanInterfaceConfig_voiceVlanDeviceInfo,
                          (xLibU8_t *) objvoiceVlanDeviceInfoValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmdbVoiceVlanPortDeviceInfoFirstGet (L7_UNIT_CURRENT, keyInterfaceValue,
                                     nextObjvoiceVlanDeviceInfoValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, objvoiceVlanDeviceInfoValue, owa.len);
    memcpy(nextObjvoiceVlanDeviceInfoValue,objvoiceVlanDeviceInfoValue,sizeof(objvoiceVlanDeviceInfoValue)); 
    owa.l7rc =  usmdbVoiceVlanPortDeviceInfoNextGet (L7_UNIT_CURRENT, keyInterfaceValue,
                                    nextObjvoiceVlanDeviceInfoValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, nextObjvoiceVlanDeviceInfoValue, owa.len);

  /* return the object value: voiceVlanDeviceInfo */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjvoiceVlanDeviceInfoValue,
                           sizeof (nextObjvoiceVlanDeviceInfoValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

