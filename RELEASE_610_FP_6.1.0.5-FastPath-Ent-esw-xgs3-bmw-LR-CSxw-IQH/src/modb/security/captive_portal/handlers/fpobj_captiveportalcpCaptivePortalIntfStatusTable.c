
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_captiveportalcpCaptivePortalIntfStatusTable.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to captiveportal-object.xml
*
* @create  06 July 2008, Sunday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_captiveportalcpCaptivePortalIntfStatusTable_obj.h"

#include "usmdb_cpdm_api.h"
#include "usmdb_cpdm_connstatus_api.h"
#include "usmdb_cpim_api.h"

/*******************************************************************************
* @function fpObjGet_captiveportalcpCaptivePortalIntfStatusTable_cpCaptivePortalInstanceId
*
* @purpose Get 'cpCaptivePortalInstanceId'
 *@description  [cpCaptivePortalInstanceId] The identifier associated with this
* instance of captive portal.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpCaptivePortalIntfStatusTable_cpCaptivePortalInstanceId (void *wap,
                                                                                         void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpCaptivePortalInstanceIdValue;
  xLibU16_t nextObjcpCaptivePortalInstanceIdValue;
  xLibU32_t objcpCaptivePortalIntfIfIndexValue;
  xLibU32_t nextObjcpCaptivePortalIntfIfIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (objcpCaptivePortalInstanceIdValue);
  owa.rc =
    xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalIntfStatusTable_cpCaptivePortalInstanceId,
                   (xLibU8_t *) & objcpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objcpCaptivePortalInstanceIdValue = 0;
    nextObjcpCaptivePortalInstanceIdValue = 0;
    objcpCaptivePortalIntfIfIndexValue = 0;
    owa.l7rc = usmDbCpdmCPConfigIntIfNumNextGet (objcpCaptivePortalInstanceIdValue,
                    objcpCaptivePortalIntfIfIndexValue, &nextObjcpCaptivePortalInstanceIdValue, 
                    &nextObjcpCaptivePortalIntfIfIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objcpCaptivePortalInstanceIdValue, owa.len);
    objcpCaptivePortalIntfIfIndexValue = 0;
    do
    {
      owa.l7rc = usmDbCpdmCPConfigIntIfNumNextGet (objcpCaptivePortalInstanceIdValue,
                    objcpCaptivePortalIntfIfIndexValue, &nextObjcpCaptivePortalInstanceIdValue, 
                    &nextObjcpCaptivePortalIntfIfIndexValue);
      objcpCaptivePortalIntfIfIndexValue = nextObjcpCaptivePortalIntfIfIndexValue;
    }
    while ((objcpCaptivePortalInstanceIdValue == nextObjcpCaptivePortalInstanceIdValue)
                    && (owa.l7rc == L7_SUCCESS));
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjcpCaptivePortalInstanceIdValue, owa.len);

  /* return the object value: cpCaptivePortalInstanceId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjcpCaptivePortalInstanceIdValue,
                           sizeof (nextObjcpCaptivePortalInstanceIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_captiveportalcpCaptivePortalIntfStatusTable_cpCaptivePortalIntfIfIndex
*
* @purpose Get 'cpCaptivePortalIntfIfIndex'
 *@description  [cpCaptivePortalIntfIfIndex] The ifIndex of the interface
* associated with this entry.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpCaptivePortalIntfStatusTable_cpCaptivePortalIntfIfIndex (void *wap,
                                                                                          void
                                                                                          *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpCaptivePortalInstanceIdValue;
  xLibU16_t nextObjcpCaptivePortalInstanceIdValue;
  xLibU32_t objcpCaptivePortalIntfIfIndexValue;
  xLibU32_t nextObjcpCaptivePortalIntfIfIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (objcpCaptivePortalInstanceIdValue);
  owa.rc =
    xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalIntfStatusTable_cpCaptivePortalInstanceId,
                   (xLibU8_t *) & objcpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* retrieve key: cpCaptivePortalIntfIfIndex */
  owa.len = sizeof (objcpCaptivePortalIntfIfIndexValue);
  owa.rc =
    xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalIntfStatusTable_cpCaptivePortalIntfIfIndex,
                   (xLibU8_t *) & objcpCaptivePortalIntfIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objcpCaptivePortalIntfIfIndexValue = 0;
    owa.l7rc = usmDbCpdmCPConfigIntIfNumNextGet (objcpCaptivePortalInstanceIdValue,
                    objcpCaptivePortalIntfIfIndexValue, &nextObjcpCaptivePortalInstanceIdValue, 
                    &nextObjcpCaptivePortalIntfIfIndexValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objcpCaptivePortalIntfIfIndexValue, owa.len);
    do
    {
        owa.l7rc = usmDbCpdmCPConfigIntIfNumNextGet (objcpCaptivePortalInstanceIdValue,
                    objcpCaptivePortalIntfIfIndexValue, &nextObjcpCaptivePortalInstanceIdValue, 
                    &nextObjcpCaptivePortalIntfIfIndexValue);
    }
    while ((objcpCaptivePortalInstanceIdValue == nextObjcpCaptivePortalInstanceIdValue) && (objcpCaptivePortalIntfIfIndexValue == nextObjcpCaptivePortalIntfIfIndexValue) && (owa.l7rc == L7_SUCCESS));
  }

  if (!(objcpCaptivePortalInstanceIdValue == nextObjcpCaptivePortalInstanceIdValue) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjcpCaptivePortalIntfIfIndexValue, owa.len);

  /* return the object value: cpCaptivePortalIntfIfIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjcpCaptivePortalIntfIfIndexValue,
                           sizeof (nextObjcpCaptivePortalIntfIfIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_captiveportalcpCaptivePortalIntfStatusTable_cpCaptivePortalIntfOperStatus
*
* @purpose Get 'cpCaptivePortalIntfOperStatus'
 *@description  [cpCaptivePortalIntfOperStatus] Indication of whether captive
* portal instance is active on the interface.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpCaptivePortalIntfStatusTable_cpCaptivePortalIntfOperStatus (void
                                                                                             *wap,
                                                                                             void
                                                                                             *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpCaptivePortalIntfOperStatusValue;

  xLibU32_t keycpCaptivePortalInstanceIdValue;
  xLibU32_t keycpCaptivePortalIntfIfIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc =
    xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalIntfStatusTable_cpCaptivePortalInstanceId,
                   (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* retrieve key: cpCaptivePortalIntfIfIndex */
  owa.len = sizeof (keycpCaptivePortalIntfIfIndexValue);
  owa.rc =
    xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalIntfStatusTable_cpCaptivePortalIntfIfIndex,
                   (xLibU8_t *) & keycpCaptivePortalIntfIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalIntfIfIndexValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbCpdmIntfStatusGetIsEnabled (keycpCaptivePortalIntfIfIndexValue,
                              &objcpCaptivePortalIntfOperStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objcpCaptivePortalIntfOperStatusValue,
                     sizeof (objcpCaptivePortalIntfOperStatusValue));

  /* return the object value: cpCaptivePortalIntfOperStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objcpCaptivePortalIntfOperStatusValue,
                           sizeof (objcpCaptivePortalIntfOperStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_captiveportalcpCaptivePortalIntfStatusTable_cpCaptivePortalIntfOperDisabledReason
*
* @purpose Get 'cpCaptivePortalIntfOperDisabledReason'
 *@description  [cpCaptivePortalIntfOperDisabledReason] The reason for the
* captive portal instance interface not being currently operational.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t
fpObjGet_captiveportalcpCaptivePortalIntfStatusTable_cpCaptivePortalIntfOperDisabledReason (void
                                                                                            *wap,
                                                                                            void
                                                                                            *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpCaptivePortalIntfOperDisabledReasonValue;
  xLibU32_t objcpCaptivePortalIntfOperStatusValue;

  xLibU32_t keycpCaptivePortalInstanceIdValue;
  xLibU32_t keycpCaptivePortalIntfIfIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc =
    xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalIntfStatusTable_cpCaptivePortalInstanceId,
                   (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* retrieve key: cpCaptivePortalIntfIfIndex */
  owa.len = sizeof (keycpCaptivePortalIntfIfIndexValue);
  owa.rc =
    xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalIntfStatusTable_cpCaptivePortalIntfIfIndex,
                   (xLibU8_t *) & keycpCaptivePortalIntfIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalIntfIfIndexValue, owa.len);

  if ((usmDbCpdmIntfStatusGetIsEnabled (keycpCaptivePortalIntfIfIndexValue, &objcpCaptivePortalIntfOperStatusValue) == L7_SUCCESS) && (objcpCaptivePortalIntfOperStatusValue == L7_DISABLE))
  {
    /* get the value from application */
    owa.l7rc = usmDbCpdmIntfStatusGetWhy (keycpCaptivePortalIntfIfIndexValue,
                              &objcpCaptivePortalIntfOperDisabledReasonValue);
    if (owa.l7rc != L7_SUCCESS)
    {
        owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
    }

    FPOBJ_TRACE_VALUE (bufp, &objcpCaptivePortalIntfOperDisabledReasonValue,
                     sizeof (objcpCaptivePortalIntfOperDisabledReasonValue));
  }
  else
  {
    objcpCaptivePortalIntfOperDisabledReasonValue = L7_INTF_WHY_DISABLED_NONE;
  }

  /* return the object value: cpCaptivePortalIntfOperDisabledReason */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objcpCaptivePortalIntfOperDisabledReasonValue,
                           sizeof (objcpCaptivePortalIntfOperDisabledReasonValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_captiveportalcpCaptivePortalIntfStatusTable_cpCaptivePortalIntfBlockStatus
*
* @purpose Get 'cpCaptivePortalIntfBlockStatus'
 *@description  [cpCaptivePortalIntfBlockStatus] Indication of whether the
* captive portal instance interface is currently blocked for
* authentications.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpCaptivePortalIntfStatusTable_cpCaptivePortalIntfBlockStatus (void
                                                                                              *wap,
                                                                                              void
                                                                                              *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpCaptivePortalIntfBlockStatusValue;

  xLibU32_t keycpCaptivePortalInstanceIdValue;
  xLibU32_t keycpCaptivePortalIntfIfIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc =
    xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalIntfStatusTable_cpCaptivePortalInstanceId,
                   (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* retrieve key: cpCaptivePortalIntfIfIndex */
  owa.len = sizeof (keycpCaptivePortalIntfIfIndexValue);
  owa.rc =
    xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalIntfStatusTable_cpCaptivePortalIntfIfIndex,
                   (xLibU8_t *) & keycpCaptivePortalIntfIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalIntfIfIndexValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbCpimIntfCapabilityBlockStatusGet (keycpCaptivePortalIntfIfIndexValue,
                              &objcpCaptivePortalIntfBlockStatusValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objcpCaptivePortalIntfBlockStatusValue,
                     sizeof (objcpCaptivePortalIntfBlockStatusValue));

  /* return the object value: cpCaptivePortalIntfBlockStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objcpCaptivePortalIntfBlockStatusValue,
                           sizeof (objcpCaptivePortalIntfBlockStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_captiveportalcpCaptivePortalIntfStatusTable_cpCaptivePortalIntfAuthUserCount
*
* @purpose Get 'cpCaptivePortalIntfAuthUserCount'
 *@description  [cpCaptivePortalIntfAuthUserCount] The number of currently
* authenticated users for this captive portal instance interface.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_captiveportalcpCaptivePortalIntfStatusTable_cpCaptivePortalIntfAuthUserCount (void
                                                                                                *wap,
                                                                                                void
                                                                                                *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objcpCaptivePortalIntfAuthUserCountValue;

  xLibU32_t keycpCaptivePortalInstanceIdValue;
  xLibU32_t keycpCaptivePortalIntfIfIndexValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: cpCaptivePortalInstanceId */
  owa.len = sizeof (keycpCaptivePortalInstanceIdValue);
  owa.rc =
    xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalIntfStatusTable_cpCaptivePortalInstanceId,
                   (xLibU8_t *) & keycpCaptivePortalInstanceIdValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalInstanceIdValue, owa.len);

  /* retrieve key: cpCaptivePortalIntfIfIndex */
  owa.len = sizeof (keycpCaptivePortalIntfIfIndexValue);
  owa.rc =
    xLibFilterGet (wap, XOBJ_captiveportalcpCaptivePortalIntfStatusTable_cpCaptivePortalIntfIfIndex,
                   (xLibU8_t *) & keycpCaptivePortalIntfIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keycpCaptivePortalIntfIfIndexValue, owa.len);

  /* get the value from application */
  owa.l7rc = usmDbCpdmIntfStatusGetUsers (keycpCaptivePortalInstanceIdValue,
                              &objcpCaptivePortalIntfAuthUserCountValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objcpCaptivePortalIntfAuthUserCountValue,
                     sizeof (objcpCaptivePortalIntfAuthUserCountValue));

  /* return the object value: cpCaptivePortalIntfAuthUserCount */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objcpCaptivePortalIntfAuthUserCountValue,
                           sizeof (objcpCaptivePortalIntfAuthUserCountValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
