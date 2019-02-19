
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_fastPathQOSIscsi.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to fastPath-object.xml
*
* @create  02 December 2008, Tuesday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_fastPathQOSIscsi_obj.h"
#include "usmdb_qos_iscsi.h"
#include "iscsi_exports.h"

extern void iscsiDebugBuffToStr(L7_uchar8 *text, L7_uchar8 *buffer, L7_uint32 bufferLength);
/*******************************************************************************
* @function fpObjGet_fastPathQOSIscsi_adminMode
*
* @purpose Get 'adminMode'
 *@description  [adminMode] Configures the global admini mode for the iSCSI
* component   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSIscsi_adminMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objadminModeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbIscsiAdminModeGet (&objadminModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objadminModeValue, sizeof (objadminModeValue));

  /* return the object value: adminMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objadminModeValue, sizeof (objadminModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_fastPathQOSIscsi_adminMode
*
* @purpose Set 'adminMode'
 *@description  [adminMode] Configures the global admini mode for the iSCSI
* component   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSIscsi_adminMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objadminModeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: adminMode */
  owa.len = sizeof (objadminModeValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objadminModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objadminModeValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbIscsiAdminModeSet (objadminModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_fastPathQOSIscsi_vlanPriority
*
* @purpose Get 'vlanPriority'
 *@description  [vlanPriority] Configures the VLAN priority for treatment of
* iSCSI packets   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSIscsi_vlanPriority (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objvlanPriorityValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbIscsiVlanPriorityGet (&objvlanPriorityValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objvlanPriorityValue, sizeof (objvlanPriorityValue));

  /* return the object value: vlanPriority */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objvlanPriorityValue,
                           sizeof (objvlanPriorityValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_fastPathQOSIscsi_vlanPriority
*
* @purpose Set 'vlanPriority'
 *@description  [vlanPriority] Configures the VLAN priority for treatment of
* iSCSI packets   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSIscsi_vlanPriority (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objvlanPriorityValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: vlanPriority */
  owa.len = sizeof (objvlanPriorityValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objvlanPriorityValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objvlanPriorityValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbIscsiVlanPrioritySet (objvlanPriorityValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjList_fastPathQOSIscsi_vlanPriority
*
* @purpose Set 'vlanPriority'
 *@description  [vlanPriority] Configures the VLAN priority for treatment of
* iSCSI packets   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_fastPathQOSIscsi_vlanPriority (void *wap, void *bufp)
{
  return fpObjAppGetKey_U32Number(wap, bufp,XOBJ_fastPathQOSIscsi_vlanPriority,
      L7_DOT1P_MIN_PRIORITY, L7_DOT1P_MAX_PRIORITY, 1);
}

/*******************************************************************************
* @function fpObjGet_fastPathQOSIscsi_Dscp
*
* @purpose Get 'Dscp'
* @description  [Dscp] Configures the DSCP value for treatment of iSCSI packets   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSIscsi_Dscp (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objDscpValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbIscsiDscpGet (&objDscpValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objDscpValue, sizeof (objDscpValue));

  /* return the object value: Dscp */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDscpValue, sizeof (objDscpValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_fastPathQOSIscsi_Dscp
*
* @purpose Set 'Dscp'
 *@description  [Dscp] Configures the DSCP value for treatment of iSCSI packets   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSIscsi_Dscp (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objDscpValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Dscp */
  owa.len = sizeof (objDscpValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objDscpValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDscpValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbIscsiDscpSet (objDscpValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjList_fastPathQOSIscsi_Dscp
*
* @purpose Set 'Dscp'
* @description  [Dscp] Configures the DSCP value for treatment of iSCSI packets   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_fastPathQOSIscsi_Dscp (void *wap, void *bufp)
{
  return fpObjAppGetKey_U32Number(wap, bufp,XOBJ_fastPathQOSIscsi_Dscp,
      L7_ACL_MIN_DSCP, L7_ACL_MAX_DSCP, 1);
}

/*******************************************************************************
* @function fpObjGet_fastPathQOSIscsi_IpPresedence
*
* @purpose Get 'IpPresedence'
 *@description  [IpPresedence] Configures the IP precedence value for treatment
* of iSCSI packets   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSIscsi_IpPresedence (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objIpPresedenceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbIscsiPrecGet (&objIpPresedenceValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objIpPresedenceValue, sizeof (objIpPresedenceValue));

  /* return the object value: IpPresedence */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIpPresedenceValue,
                           sizeof (objIpPresedenceValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_fastPathQOSIscsi_IpPresedence
*
* @purpose Set 'IpPresedence'
 *@description  [IpPresedence] Configures the IP precedence value for treatment
* of iSCSI packets   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSIscsi_IpPresedence (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objIpPresedenceValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: IpPresedence */
  owa.len = sizeof (objIpPresedenceValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objIpPresedenceValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objIpPresedenceValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbIscsiPrecSet (objIpPresedenceValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjList_fastPathQOSIscsi_IpPresedence
*
* @purpose Set 'IpPresedence'
* @description  [IpPresedence] Configures the IP precedence value for treatment
* of iSCSI packets   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjList_fastPathQOSIscsi_IpPresedence (void *wap, void *bufp)
{
  return fpObjAppGetKey_U32Number(wap, bufp,XOBJ_fastPathQOSIscsi_IpPresedence,
      L7_ACL_MIN_PRECEDENCE,L7_ACL_MAX_PRECEDENCE, 1);
}

/*******************************************************************************
* @function fpObjGet_fastPathQOSIscsi_markingMode
*
* @purpose Get 'markingMode'
 *@description  [markingMode] Configures whether to mark iSCSI packets   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSIscsi_markingMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objmarkingModeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbIscsiMarkingModeGet (&objmarkingModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objmarkingModeValue, sizeof (objmarkingModeValue));

  /* return the object value: markingMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmarkingModeValue, sizeof (objmarkingModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_fastPathQOSIscsi_markingMode
*
* @purpose Set 'markingMode'
 *@description  [markingMode] Configures whether to mark iSCSI packets   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSIscsi_markingMode (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objmarkingModeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: markingMode */
  owa.len = sizeof (objmarkingModeValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objmarkingModeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objmarkingModeValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbIscsiMarkingModeSet (objmarkingModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_fastPathQOSIscsi_TagField
*
* @purpose Get 'TagField'
 *@description  [TagField] Configures which type of packet priority marking to
* use   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSIscsi_TagField (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTagFieldValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbIscsiTagFieldGet (&objTagFieldValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objTagFieldValue, sizeof (objTagFieldValue));

  /* return the object value: TagField */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTagFieldValue, sizeof (objTagFieldValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_fastPathQOSIscsi_TagField
*
* @purpose Set 'TagField'
 *@description  [TagField] Configures which type of packet priority marking to
* use   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSIscsi_TagField (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTagFieldValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: TagField */
  owa.len = sizeof (objTagFieldValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objTagFieldValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTagFieldValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = usmDbIscsiTagFieldSet (objTagFieldValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_fastPathQOSIscsi_TimeoutInterval
*
* @purpose Get 'TimeoutInterval'
 *@description  [TimeoutInterval] Configures the session time out value used for
* iSCSI sessions   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSIscsi_TimeoutInterval (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTimeoutIntervalValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbIscsiTimeOutIntervalGet (&objTimeoutIntervalValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objTimeoutIntervalValue, sizeof (objTimeoutIntervalValue));

  /* Convert the value from seconds to minutes as application stores in units of seconds. */
  objTimeoutIntervalValue = (objTimeoutIntervalValue/60);
  /* return the object value: TimeoutInterval */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTimeoutIntervalValue,
                           sizeof (objTimeoutIntervalValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_fastPathQOSIscsi_TimeoutInterval
*
* @purpose Set 'TimeoutInterval'
 *@description  [TimeoutInterval] Configures the session time out value used for
* iSCSI sessions   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSIscsi_TimeoutInterval (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTimeoutIntervalValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: TimeoutInterval */
  owa.len = sizeof (objTimeoutIntervalValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objTimeoutIntervalValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTimeoutIntervalValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* Convert the value from minutes to seconds as the application stores in units of seconds. */
  objTimeoutIntervalValue = (objTimeoutIntervalValue * 60);
  /* set the value in application */
  owa.l7rc = usmDbIscsiTimeOutIntervalSet (objTimeoutIntervalValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_fastPathQOSIscsi_TargetEntryId
*
* @purpose Get 'TargetEntryId'
 *@description  [TargetEntryId] Target Entry Index   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSIscsi_TargetEntryId (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTargetEntryIdValue;
  xLibU32_t nextObjTargetEntryIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: TargetEntryId */
  owa.len = sizeof (objTargetEntryIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSIscsi_TargetEntryId,
                          (xLibU8_t *) & objTargetEntryIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbIscsiTargetTableFirstGet (&nextObjTargetEntryIdValue,L7_FALSE);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objTargetEntryIdValue, owa.len);
    owa.l7rc = usmDbIscsiTargetTableNextGet (objTargetEntryIdValue,&nextObjTargetEntryIdValue,L7_FALSE);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjTargetEntryIdValue, owa.len);

  /* return the object value: TargetEntryId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjTargetEntryIdValue,
                           sizeof (nextObjTargetEntryIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_fastPathQOSIscsi_TargetTcpPort
*
* @purpose Get 'TargetTcpPort'
 *@description  [TargetTcpPort] TCP Port of the given entry   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSIscsi_TargetTcpPort (void *wap, void *bufp)
{
  /* This routine is made dummy for now */
  return XLIBRC_SUCCESS;
}


/*******************************************************************************
* @function fpObjSet_fastPathQOSIscsi_TargetTcpPort
*
* @purpose Set 'TargetTcpPort'
 *@description  [TargetTcpPort] TCP Port of the given entry   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSIscsi_TargetTcpPort (void *wap, void *bufp)
{
  /* This routine is made dummy for now */
  return XLIBRC_SUCCESS;
}

/*******************************************************************************
* @function fpObjGet_fastPathQOSIscsi_TargetIpAddress
*
* @purpose Get 'TargetIpAddress'
 *@description  [TargetIpAddress] IP Address of the given entry   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSIscsi_TargetIpAddress (void *wap, void *bufp)
{
  /* This routine is made dummy for now */
  return XLIBRC_SUCCESS;
}

/*******************************************************************************
* @function fpObjSet_fastPathQOSIscsi_TargetIpAddress
*
* @purpose Set 'TargetIpAddress'
 *@description  [TargetIpAddress] IP Address of the given entry   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSIscsi_TargetIpAddress (void *wap, void *bufp)
{
  /* This routine is made dummy for now */
  return XLIBRC_SUCCESS;
}

/*******************************************************************************
* @function fpObjGet_fastPathQOSIscsi_TargetName
*
* @purpose Get 'TargetName'
 *@description  [TargetName] Target Name of the given entry.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSIscsi_TargetName (void *wap, void *bufp)
{
  /* This routine is made dummy for now */
  return XLIBRC_SUCCESS;
}

/*******************************************************************************
* @function fpObjSet_fastPathQOSIscsi_TargetName
*
* @purpose Set 'TargetName'
 *@description  [TargetName] Target Name of the given entry.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSIscsi_TargetName (void *wap, void *bufp)
{
  /* This routine is made dummy for now */
  return XLIBRC_SUCCESS;
}

/*******************************************************************************
* @function fpObjGet_fastPathQOSIscsi_TargetRowStatus
*
* @purpose Get 'TargetRowStatus'
 *@description  [TargetRowStatus] Row Status Object to Create a TCP Entry for
* Iscsi.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSIscsi_TargetRowStatus (void *wap, void *bufp)
{
  /* This routine is made dummy for now */
  return XLIBRC_SUCCESS;
}

/*******************************************************************************
* @function fpObjSet_fastPathQOSIscsi_TargetRowStatus
*
* @purpose Set 'TargetRowStatus'
 *@description  [TargetRowStatus] Row Status Object to Create a TCP Entry for
* Iscsi.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_fastPathQOSIscsi_TargetRowStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTargetRowStatusValue;

  xLibU32_t keyTargetEntryIdValue;
  xLibU32_t keyTargetTcpPortValue;
  L7_inet_addr_t keyTargetIpAddrValue;
  xLibU8_t keyTargetName[ISCSI_NAME_LENGTH_MAX];
  

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: TargetRowStatus */
  owa.len = sizeof (objTargetRowStatusValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objTargetRowStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTargetRowStatusValue, owa.len);

  /* retrieve key: TargetEntryId */
  owa.len = sizeof (keyTargetEntryIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSIscsi_TargetEntryId,
                          (xLibU8_t *) & keyTargetEntryIdValue, &owa.len);
  
  if ((owa.rc != XLIBRC_SUCCESS) && 
      (objTargetRowStatusValue != L7_ROW_STATUS_CREATE_AND_GO))
  {
     owa.rc = XLIBRC_FILTER_MISSING;
     FPOBJ_TRACE_EXIT (bufp, owa);
     return owa.rc;
  }

  if(objTargetRowStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
    memset(&keyTargetIpAddrValue,0x00,sizeof(keyTargetIpAddrValue));  
    memset(keyTargetName,0x00,sizeof(keyTargetName));  
    keyTargetIpAddrValue.family = L7_AF_INET;
   /* Retrieve the other dependent objects like Entry ID,TCP Port,IP Address,Target Name */
   /* In this case TCP Port,IP Address and Target Name are required attributes but not
    * dependents
    */

    /* retrieve key: Tcp Port */
    owa.len = sizeof (keyTargetTcpPortValue);
    owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSIscsi_TargetTcpPort,
                            (xLibU8_t *) & keyTargetTcpPortValue, &owa.len);
    if (owa.rc != XLIBRC_SUCCESS)
    {
      owa.rc = XLIBRC_FILTER_MISSING;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }

    /* retrieve key: IP Address */
    owa.len = sizeof (keyTargetIpAddrValue);
    xLibFilterGet (wap, XOBJ_fastPathQOSIscsi_TargetIpAddress,
                   (xLibU8_t *) &keyTargetIpAddrValue, &owa.len);

    /* Return code cather is not necessary for this as becuase these fields 
     * are optional
     */
    owa.rc = XLIBRC_SUCCESS;

    /* retrieve key: Target Name */
    owa.len = sizeof (keyTargetName);
    xLibFilterGet (wap, XOBJ_fastPathQOSIscsi_TargetName,
                          (xLibU8_t *) keyTargetName, &owa.len);
   /* Return code cather is not necessary for this as becuase these fields 
    * are optional
    */
    owa.rc = XLIBRC_SUCCESS;


    /*Retrived all the required objects now call the appropriate USMDB */
    /* if row status object is specified and eual to delete return success */

    /* set the value in application */
     owa.l7rc = usmDbIscsiTargetTcpPortAdd (keyTargetTcpPortValue,
                                            &keyTargetIpAddrValue,keyTargetName);
  }
  else if(objTargetRowStatusValue == L7_ROW_STATUS_DESTROY)
  {
    if ((usmDbIscsiTargetTableEntryTcpPortGet(keyTargetEntryIdValue, &keyTargetTcpPortValue) == L7_SUCCESS) &&
        (usmDbIscsiTargetTableEntryIpAddressGet(keyTargetEntryIdValue, &keyTargetIpAddrValue) == L7_SUCCESS))
    {
      owa.l7rc = usmDbIscsiTargetTcpPortDelete (keyTargetTcpPortValue, &keyTargetIpAddrValue);
    }
    else
    {
      owa.l7rc = L7_FAILURE;
    }
  }
  else
  {
    owa.l7rc = L7_FAILURE;
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ISCSI_TARGET_NAME_ADD_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_fastPathQOSIscsi_TableEntryTCPPort
*
* @purpose Get 'TableEntryTCPPort'
 *@description  [TableEntryTCPPort] Retrieves the TCP port number for the
* specified entry in Target TCP port table.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSIscsi_TableEntryTCPPort (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTableEntryTCPPortValue;

  xLibU32_t keyTargetEntryIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: TargetEntryId */
  owa.len = sizeof (keyTargetEntryIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSIscsi_TargetEntryId,
                          (xLibU8_t *) & keyTargetEntryIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTargetEntryIdValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbIscsiTargetTableEntryTcpPortGet (keyTargetEntryIdValue, &objTableEntryTCPPortValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objTableEntryTCPPortValue, sizeof (objTableEntryTCPPortValue));

  /* return the object value: TableEntryTCPPort */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTableEntryTCPPortValue,
                           sizeof (objTableEntryTCPPortValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_fastPathQOSIscsi_TableEntryIpAddress
*
* @purpose Get 'TableEntryIpAddress'
 *@description  [TableEntryIpAddress] Retrieves the target IP address for the
* specified entry in the Target TCP port table.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSIscsi_TableEntryIpAddress (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  L7_inet_addr_t objTableEntryIpAddressValue;

  xLibU32_t keyTargetEntryIdValue;

  FPOBJ_TRACE_ENTER (bufp);

  memset(&objTableEntryIpAddressValue,0x00,sizeof(objTableEntryIpAddressValue));

  /* retrieve key: TargetEntryId */
  owa.len = sizeof (keyTargetEntryIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSIscsi_TargetEntryId,
                          (xLibU8_t *) & keyTargetEntryIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTargetEntryIdValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbIscsiTargetTableEntryIpAddressGet(keyTargetEntryIdValue, &objTableEntryIpAddressValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objTableEntryIpAddressValue, strlen (objTableEntryIpAddressValue));

  /* return the object value: TableEntryIpAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objTableEntryIpAddressValue,
                           sizeof (objTableEntryIpAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_fastPathQOSIscsi_TableEntryName
*
* @purpose Get 'TableEntryName'
 *@description  [TableEntryName] Retrieves the target name for the specified
* entry in the Target TCP port table   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSIscsi_TableEntryName (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU8_t objTableEntryNameValue[ISCSI_NAME_LENGTH_MAX];

  xLibU32_t keyTargetEntryIdValue;

  FPOBJ_TRACE_ENTER (bufp);
  
 
  /* retrieve key: TargetEntryId */
  owa.len = sizeof (keyTargetEntryIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSIscsi_TargetEntryId,
                          (xLibU8_t *) & keyTargetEntryIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTargetEntryIdValue, owa.len);
  
  memset(objTableEntryNameValue,0x00,sizeof(objTableEntryNameValue));

  /* get the value from application */
  owa.l7rc = usmDbIscsiTargetTableEntryTargetNameGet (keyTargetEntryIdValue, objTableEntryNameValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objTableEntryNameValue, strlen (objTableEntryNameValue));

  /* return the object value: TableEntryName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objTableEntryNameValue,
                           strlen (objTableEntryNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_fastPathQOSIscsi_TableEntryInUse
*
* @purpose Get 'TableEntryInUse'
 *@description  [TableEntryInUse] Reports whether the  Target TCP port table
* entry corresponding with  entryId is in use/   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSIscsi_TableEntryInUse (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTableEntryInUseValue;

  xLibU32_t keyTargetEntryIdValue;

  L7_BOOL returnValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: TargetEntryId */
  owa.len = sizeof (keyTargetEntryIdValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSIscsi_TargetEntryId,
                          (xLibU8_t *) & keyTargetEntryIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyTargetEntryIdValue, owa.len);
  /* just return SUCCESS as because boolean return value is used further */
  owa.l7rc = L7_SUCCESS;
  /* get the value from application */
  returnValue = usmDbIscsiTargetTableEntryInUseStatusGet (keyTargetEntryIdValue);

  objTableEntryInUseValue = returnValue;
  FPOBJ_TRACE_VALUE (bufp, &objTableEntryInUseValue, sizeof (objTableEntryInUseValue));

  /* return the object value: TableEntryInUse */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTableEntryInUseValue,
                           sizeof (objTableEntryInUseValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_fastPathQOSIscsi_SessionIndex
*
* @purpose Get 'SessionIndex'
 *@description  [SessionIndex] Retrieves the entry in the iSCSI session table.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSIscsi_SessionIndex (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objSessionIndexValue;
  xLibU32_t nextObjSessionIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: SessionIndex */
  owa.len = sizeof (objSessionIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSIscsi_SessionIndex,
                          (xLibU8_t *) & objSessionIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbIscsiSessionFirstGet (&nextObjSessionIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objSessionIndexValue, owa.len);
    owa.l7rc = usmDbIscsiSessionNextGet (objSessionIndexValue,&nextObjSessionIndexValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjSessionIndexValue, owa.len);

  /* return the object value: SessionIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjSessionIndexValue,
                           sizeof (nextObjSessionIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_fastPathQOSIscsi_SessionTargetName
*
* @purpose Get 'SessionTargetName'
 *@description  [SessionTargetName] The Target Name of the given Session.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSIscsi_SessionTargetName (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU8_t objSessionTargetNameValue[ISCSI_NAME_LENGTH_MAX];

  xLibU32_t keySessionIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: SessionIndex */
  owa.len = sizeof (keySessionIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSIscsi_SessionIndex,
                          (xLibU8_t *) & keySessionIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySessionIndexValue, owa.len);

  memset(objSessionTargetNameValue,0x00,sizeof(objSessionTargetNameValue));

  /* get the value from application */
  owa.l7rc = usmDbIscsiSessionTargetNameGet (keySessionIndexValue, objSessionTargetNameValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objSessionTargetNameValue, strlen (objSessionTargetNameValue));

  /* return the object value: SessionTargetName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objSessionTargetNameValue,
                           strlen (objSessionTargetNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_fastPathQOSIscsi_SessionInitiatorName
*
* @purpose Get 'SessionInitiatorName'
 *@description  [SessionInitiatorName] The Session Initiator Name.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSIscsi_SessionInitiatorName (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU8_t objSessionInitiatorNameValue[ISCSI_NAME_LENGTH_MAX];

  xLibU32_t keySessionIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: SessionIndex */
  owa.len = sizeof (keySessionIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSIscsi_SessionIndex,
                          (xLibU8_t *) & keySessionIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySessionIndexValue, owa.len);
  memset(objSessionInitiatorNameValue,0x00,sizeof(objSessionInitiatorNameValue));

  /* get the value from application */
  owa.l7rc = usmDbIscsiSessionInitiatorNameGet (keySessionIndexValue, objSessionInitiatorNameValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objSessionInitiatorNameValue, strlen (objSessionInitiatorNameValue));

  /* return the object value: SessionInitiatorName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objSessionInitiatorNameValue,
                           strlen (objSessionInitiatorNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_fastPathQOSIscsi_SessionStartTime
*
* @purpose Get 'SessionStartTime'
 *@description  [SessionStartTime] Session Start Time of a given Session Index   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSIscsi_SessionStartTime (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU8_t  objSessionStartTimeValue[ISCSI_NAME_LENGTH_MAX];
  xLibU32_t keySessionIndexValue;
  L7_clocktime timeVal, startTime;
  L7_timespec ts;

  FPOBJ_TRACE_ENTER (bufp);
 
  /* retrieve key: SessionIndex */
  owa.len = sizeof (keySessionIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSIscsi_SessionIndex,
                          (xLibU8_t *) & keySessionIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySessionIndexValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbIscsiSessionStartTimeGet (keySessionIndexValue, &startTime);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* Now convert the timestamp in seconds to Date and Time format. */
  osapiClockTimeRaw(&timeVal);
  timeVal.seconds = timeVal.seconds - startTime.seconds;
  osapiConvertRawUpTime(timeVal.seconds, &ts);
  memset(objSessionStartTimeValue, 0x00, sizeof(objSessionStartTimeValue));
  sprintf(objSessionStartTimeValue, "%02d:%02d:%02d:%02d (DD:HH:MM:SS)",
                           ts.days, ts.hours, ts.minutes, ts.seconds);
  
  FPOBJ_TRACE_VALUE (bufp, &objSessionStartTimeValue, sizeof (objSessionStartTimeValue));

  /* return the object value: SessionStartTime */
  owa.rc = xLibBufDataSet (bufp, objSessionStartTimeValue, sizeof(objSessionStartTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_fastPathQOSIscsi_SessionTimeLeft
*
* @purpose Get 'SessionTimeLeft'
* @description  [SessionTimeLeft] Time Left for the Session of a given Session Index
* @notes
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSIscsi_SessionTimeLeft (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objSessionTimeLeftValue;
  xLibU32_t keySessionIndexValue;
  L7_uint32 agingTime = 0;
  L7_uint32 silentTime = 0;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: SessionIndex */
  owa.len = sizeof (keySessionIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSIscsi_SessionIndex,
                          (xLibU8_t *) & keySessionIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySessionIndexValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbIscsiSessionSilentTimeGet(keySessionIndexValue, &silentTime);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  owa.l7rc = usmDbIscsiTimeOutIntervalGet(&agingTime);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* Calculate the Time Left for the Session to expire in Seconds. */ 
  objSessionTimeLeftValue = agingTime - silentTime;
  
  FPOBJ_TRACE_VALUE (bufp, &objSessionTimeLeftValue, sizeof (objSessionTimeLeftValue));

  /* return the object value: SessionTimeLeft */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objSessionTimeLeftValue,
                           sizeof (objSessionTimeLeftValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_fastPathQOSIscsi_SessionSilentTime
*
* @purpose Get 'SessionSilentTime'
 *@description  [SessionSilentTime] Session Silent time of the Given Session
* Index.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSIscsi_SessionSilentTime (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objSessionSilentTimeValue;

  xLibU32_t keySessionIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: SessionIndex */
  owa.len = sizeof (keySessionIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSIscsi_SessionIndex,
                          (xLibU8_t *) & keySessionIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySessionIndexValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbIscsiSessionSilentTimeGet (keySessionIndexValue, &objSessionSilentTimeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objSessionSilentTimeValue, sizeof (objSessionSilentTimeValue));

  /* return the object value: SessionSilentTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objSessionSilentTimeValue,
                           sizeof (objSessionSilentTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_fastPathQOSIscsi_SessionISID
*
* @purpose Get 'SessionISID'
 *@description  [SessionISID] The ISID of the given Session ID.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSIscsi_SessionISID (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU8_t iSID[6];
  xLibU8_t objSessionISIDValue[(sizeof(iSID)+1)];
  xLibU32_t keySessionIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: SessionIndex */
  owa.len = sizeof (keySessionIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSIscsi_SessionIndex,
                          (xLibU8_t *) & keySessionIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySessionIndexValue, owa.len);
  
  memset(iSID,0,sizeof(iSID));
  memset(objSessionISIDValue,0,sizeof(objSessionISIDValue));
  /* get the value from application */
  owa.l7rc = usmDbIscsiSessionIsidGet (keySessionIndexValue, iSID);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  iscsiDebugBuffToStr(objSessionISIDValue, iSID, sizeof(iSID));
  FPOBJ_TRACE_VALUE (bufp, objSessionISIDValue, sizeof (objSessionISIDValue));

  /* return the object value: SessionISID */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *)  objSessionISIDValue, strlen (objSessionISIDValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_fastPathQOSIscsi_ConnectionIndex
*
* @purpose Get 'ConnectionIndex'
 *@description  [ConnectionIndex] The Connection Index of the given Session
* Index.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSIscsi_ConnectionIndex (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objConnectionIndexValue;
  xLibU32_t nextObjConnectionIndexValue;

  xLibU32_t keySessionIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: SessionIndex */
  owa.len = sizeof (keySessionIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSIscsi_SessionIndex,
                          (xLibU8_t *) & keySessionIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySessionIndexValue, owa.len);

  /* retrieve key: ConnectionIndex */
  owa.len = sizeof (objConnectionIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSIscsi_ConnectionIndex,
                          (xLibU8_t *) & objConnectionIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbIscsiConnectionFirstGet(keySessionIndexValue,&nextObjConnectionIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objConnectionIndexValue, owa.len);
    owa.l7rc = usmDbIscsiConnectionNextGet (keySessionIndexValue,
                                    objConnectionIndexValue, &nextObjConnectionIndexValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjConnectionIndexValue, owa.len);

  /* return the object value: ConnectionIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjConnectionIndexValue,
                           sizeof (nextObjConnectionIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_fastPathQOSIscsi_ConnectionTargetIPAddress
*
* @purpose Get 'ConnectionTargetIPAddress'
 *@description  [ConnectionTargetIPAddress] The Connection Target IP address of
* given Connection ID and Seession ID.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSIscsi_ConnectionTargetIPAddress (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  L7_inet_addr_t objConnectionTargetIPAddressValue;

  xLibU32_t keySessionIndexValue;
  xLibU32_t keyConnectionIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: SessionIndex */
  owa.len = sizeof (keySessionIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSIscsi_SessionIndex,
                          (xLibU8_t *) & keySessionIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySessionIndexValue, owa.len);

  /* retrieve key: ConnectionIndex */
  owa.len = sizeof (keyConnectionIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSIscsi_ConnectionIndex,
                          (xLibU8_t *) & keyConnectionIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyConnectionIndexValue, owa.len);

  memset(&objConnectionTargetIPAddressValue,0x00,sizeof(objConnectionTargetIPAddressValue));
  /* get the value from application */
  owa.l7rc = usmDbIscsiConnectionTargetIpAddressGet (keyConnectionIndexValue, &objConnectionTargetIPAddressValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objConnectionTargetIPAddressValue,
                     strlen (objConnectionTargetIPAddressValue));

  /* return the object value: ConnectionTargetIPAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objConnectionTargetIPAddressValue,
                           sizeof (objConnectionTargetIPAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_fastPathQOSIscsi_ConnectionInitiatorIPAddress
*
* @purpose Get 'ConnectionInitiatorIPAddress'
 *@description  [ConnectionInitiatorIPAddress] The Connection Initiator IP
* Address.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSIscsi_ConnectionInitiatorIPAddress (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  L7_inet_addr_t objConnectionInitiatorIPAddressValue;

  xLibU32_t keySessionIndexValue;
  xLibU32_t keyConnectionIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: SessionIndex */
  owa.len = sizeof (keySessionIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSIscsi_SessionIndex,
                          (xLibU8_t *) & keySessionIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySessionIndexValue, owa.len);

  /* retrieve key: ConnectionIndex */
  owa.len = sizeof (keyConnectionIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSIscsi_ConnectionIndex,
                          (xLibU8_t *) & keyConnectionIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyConnectionIndexValue, owa.len);
  
  memset(&objConnectionInitiatorIPAddressValue,0x00,sizeof(objConnectionInitiatorIPAddressValue));
  /* get the value from application */
  owa.l7rc = usmDbIscsiConnectionInitiatorIpAddressGet (keyConnectionIndexValue, 
                                &objConnectionInitiatorIPAddressValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, objConnectionInitiatorIPAddressValue,
                     strlen (objConnectionInitiatorIPAddressValue));

  /* return the object value: ConnectionInitiatorIPAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objConnectionInitiatorIPAddressValue,
                           sizeof (objConnectionInitiatorIPAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_fastPathQOSIscsi_ConnectionTargetTCPPort
*
* @purpose Get 'ConnectionTargetTCPPort'
 *@description  [ConnectionTargetTCPPort] The Connector Target TCP Port.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSIscsi_ConnectionTargetTCPPort (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objConnectionTargetTCPPortValue;

  xLibU32_t keySessionIndexValue;
  xLibU32_t keyConnectionIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: SessionIndex */
  owa.len = sizeof (keySessionIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSIscsi_SessionIndex,
                          (xLibU8_t *) & keySessionIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySessionIndexValue, owa.len);

  /* retrieve key: ConnectionIndex */
  owa.len = sizeof (keyConnectionIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSIscsi_ConnectionIndex,
                          (xLibU8_t *) & keyConnectionIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyConnectionIndexValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbIscsiConnectionTargetTcpPortGet (keyConnectionIndexValue,
                                  &objConnectionTargetTCPPortValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objConnectionTargetTCPPortValue,
                     sizeof (objConnectionTargetTCPPortValue));

  /* return the object value: ConnectionTargetTCPPort */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objConnectionTargetTCPPortValue,
                           sizeof (objConnectionTargetTCPPortValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_fastPathQOSIscsi_ConnectionInitiatorTCPPort
*
* @purpose Get 'ConnectionInitiatorTCPPort'
 *@description  [ConnectionInitiatorTCPPort] The Connection Initiator TCP Port.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSIscsi_ConnectionInitiatorTCPPort (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objConnectionInitiatorTCPPortValue;

  xLibU32_t keySessionIndexValue;
  xLibU32_t keyConnectionIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: SessionIndex */
  owa.len = sizeof (keySessionIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSIscsi_SessionIndex,
                          (xLibU8_t *) & keySessionIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySessionIndexValue, owa.len);

  /* retrieve key: ConnectionIndex */
  owa.len = sizeof (keyConnectionIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSIscsi_ConnectionIndex,
                          (xLibU8_t *) & keyConnectionIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyConnectionIndexValue, owa.len);

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbIscsiConnectionInitiatorTcpPortGet (keyConnectionIndexValue, &objConnectionInitiatorTCPPortValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objConnectionInitiatorTCPPortValue,
                     sizeof (objConnectionInitiatorTCPPortValue));

  /* return the object value: ConnectionInitiatorTCPPort */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objConnectionInitiatorTCPPortValue,
                           sizeof (objConnectionInitiatorTCPPortValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_fastPathQOSIscsi_ConnectionCID
*
* @purpose Get 'ConnectionCID'
 *@description  [ConnectionCID] The CID of the given Connection ID   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSIscsi_ConnectionCID (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objConnectionCIDValue;

  xLibU32_t keySessionIndexValue;
  xLibU32_t keyConnectionIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: SessionIndex */
  owa.len = sizeof (keySessionIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSIscsi_SessionIndex,
                          (xLibU8_t *) & keySessionIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keySessionIndexValue, owa.len);

  /* retrieve key: ConnectionIndex */
  owa.len = sizeof (keyConnectionIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSIscsi_ConnectionIndex,
                          (xLibU8_t *) & keyConnectionIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyConnectionIndexValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbIscsiConnectionCidGet(keyConnectionIndexValue, &objConnectionCIDValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objConnectionCIDValue, sizeof (objConnectionCIDValue));

  /* return the object value: ConnectionCID */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objConnectionCIDValue,
                           sizeof (objConnectionCIDValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_fastPathQOSIscsi_ConnectionSessionID
*
* @purpose Get 'ConnectionSessionID'
 *@description  [ConnectionSessionID] The Session Index of a given Connection
* Index   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_fastPathQOSIscsi_ConnectionSessionID (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objConnectionSessionIDValue;

  xLibU32_t keyConnectionIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ConnectionIndex */
  owa.len = sizeof (keyConnectionIndexValue);
  owa.rc = xLibFilterGet (wap, XOBJ_fastPathQOSIscsi_ConnectionIndex,
                          (xLibU8_t *) & keyConnectionIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyConnectionIndexValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbIscsiConnectionSessionIdGet(keyConnectionIndexValue,
                              &objConnectionSessionIDValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objConnectionSessionIDValue, sizeof (objConnectionSessionIDValue));

  /* return the object value: ConnectionSessionID */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objConnectionSessionIDValue,
                           sizeof (objConnectionSessionIDValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
