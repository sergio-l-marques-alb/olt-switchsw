/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_baseDhcpServerGlobal.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to snmp-object.xml
*
* @create  9 January 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_baseDhcpServerGlobal_obj.h"
#include "usmdb_dhcps_api.h"

/*******************************************************************************
* @function fpObjGet_baseDhcpServerGlobal_DhcpServerBootpAutomatic
*
* @purpose Get 'DhcpServerBootpAutomatic'
*
* @description Used to enable or disable autmatic address allocation to bootp 
*              clients from dynamic address pools. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDhcpServerGlobal_DhcpServerBootpAutomatic (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDhcpServerBootpAutomaticValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbDhcpsBootpAutomaticGet (L7_UNIT_CURRENT,
                                 &objDhcpServerBootpAutomaticValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDhcpServerBootpAutomaticValue,
                     sizeof (objDhcpServerBootpAutomaticValue));

  /* return the object value: DhcpServerBootpAutomatic */
  owa.rc =
    xLibBufDataSet (bufp, (xLibU8_t *) & objDhcpServerBootpAutomaticValue,
                    sizeof (objDhcpServerBootpAutomaticValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseDhcpServerGlobal_DhcpServerBootpAutomatic
*
* @purpose Set 'DhcpServerBootpAutomatic'
*
* @description Used to enable or disable autmatic address allocation to bootp 
*              clients from dynamic address pools. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseDhcpServerGlobal_DhcpServerBootpAutomatic (void *wap,
                                                                 void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDhcpServerBootpAutomaticValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: DhcpServerBootpAutomatic */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objDhcpServerBootpAutomaticValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDhcpServerBootpAutomaticValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDhcpsBootpAutomaticSet (L7_UNIT_CURRENT,
                                 objDhcpServerBootpAutomaticValue);
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
* @function fpObjGet_baseDhcpServerGlobal_DhcpServerAdminMode
*
* @purpose Get 'DhcpServerAdminMode'
*
* @description  Admin-mode of the DHCP Server. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDhcpServerGlobal_DhcpServerAdminMode (void *wap,
                                                            void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDhcpServerAdminModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbDhcpsAdminModeGet (L7_UNIT_CURRENT, &objDhcpServerAdminModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDhcpServerAdminModeValue,
                     sizeof (objDhcpServerAdminModeValue));

  /* return the object value: DhcpServerAdminMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objDhcpServerAdminModeValue,
                           sizeof (objDhcpServerAdminModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseDhcpServerGlobal_DhcpServerAdminMode
*
* @purpose Set 'DhcpServerAdminMode'
*
* @description  Admin-mode of the DHCP Server. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseDhcpServerGlobal_DhcpServerAdminMode (void *wap,
                                                            void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDhcpServerAdminModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: DhcpServerAdminMode */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) & objDhcpServerAdminModeValue,
                           &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDhcpServerAdminModeValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDhcpsAdminModeSet (L7_UNIT_CURRENT, objDhcpServerAdminModeValue);
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
* @function fpObjSet_baseDhcpServerGlobal_DhcpServerLeaseClearAllBindings
*
* @purpose Set 'DhcpServerLeaseClearAllBindings'
*
* @description Clears All the DHCP server bindings. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseDhcpServerGlobal_DhcpServerLeaseClearAllBindings (void
                                                                        *wap,
                                                                        void
                                                                        *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objDhcpServerLeaseClearAllBindingsValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: DhcpServerLeaseClearAllBindings */
  owa.rc = xLibBufDataGet (bufp,
                           (xLibU8_t *) &
                           objDhcpServerLeaseClearAllBindingsValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objDhcpServerLeaseClearAllBindingsValue, owa.len);

  /* set the value in application */
  owa.l7rc =
    usmDbDhcpsBindingClearAll (L7_UNIT_CURRENT,
                               objDhcpServerLeaseClearAllBindingsValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_DHCPS_CLEAR_BINDINGS_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
