
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_routingospfv2GranularTraps.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to routing-object.xml
*
* @create  20 December 2008, Saturday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_routingospfv2GranularTraps_obj.h"
#include "usmdb_mib_ospf_api.h"

/*******************************************************************************
* @function fpObjGet_routingospfv2GranularTraps_TrapAll
*
* @purpose Get 'TrapAll'
 *@description  [TrapAll] Allows a user to enable/disable all the ospfv2 Traps   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfv2GranularTraps_TrapAll (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTrapAllValue = L7_DISABLE;
  xLibU32_t val = 0;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  if ( (usmDbOspfTrapFlagsGet(L7_UNIT_CURRENT, &val)) == L7_SUCCESS ) /* val is ospf trapflag value */
  {
    if(val == L7_OSPF_TRAP_ALL)
    {
      objTrapAllValue = L7_ENABLE;
    }
  }

  FPOBJ_TRACE_VALUE (bufp, &objTrapAllValue, sizeof (objTrapAllValue));

  /* return the object value: TrapAll */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTrapAllValue, sizeof (objTrapAllValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_routingospfv2GranularTraps_TrapAll
*
* @purpose Set 'TrapAll'
 *@description  [TrapAll] Allows a user to enable/disable all the ospfv2 Traps   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfv2GranularTraps_TrapAll (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTrapAllValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: TrapAll */
  owa.len = sizeof (objTrapAllValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objTrapAllValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTrapAllValue, owa.len);


  /* set the value in application */
  owa.l7rc = usmDbOspfTrapModeSet (L7_UNIT_CURRENT, objTrapAllValue, L7_OSPF_TRAP_ALL);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_routingospfv2GranularTraps_TrapErrosAll
*
* @purpose Get 'TrapErrosAll'
 *@description  [TrapErrosAll] Allows a user to enable/disable all the ospfv2
* Error Traps   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfv2GranularTraps_TrapErrosAll (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTrapErrosAllValue = L7_DISABLE;
  xLibU32_t val = 0;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  if ( (usmDbOspfTrapFlagsGet(L7_UNIT_CURRENT, &val)) == L7_SUCCESS ) /* val is ospf trapflag value */
  {
    if((L7_OSPF_TRAP_ERRORS_ALL & val) == L7_OSPF_TRAP_ERRORS_ALL)
    {
      objTrapErrosAllValue = L7_ENABLE;
    }
  }

  FPOBJ_TRACE_VALUE (bufp, &objTrapErrosAllValue, sizeof (objTrapErrosAllValue));

  /* return the object value: TrapErrosAll */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTrapErrosAllValue,
                           sizeof (objTrapErrosAllValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_routingospfv2GranularTraps_TrapErrosAll
*
* @purpose Set 'TrapErrosAll'
 *@description  [TrapErrosAll] Allows a user to enable/disable all the ospfv2
* Error Traps   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfv2GranularTraps_TrapErrosAll (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTrapErrosAllValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: TrapErrosAll */
  owa.len = sizeof (objTrapErrosAllValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objTrapErrosAllValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTrapErrosAllValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbOspfTrapModeSet (L7_UNIT_CURRENT, objTrapErrosAllValue, L7_OSPF_TRAP_ERRORS_ALL);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_routingospfv2GranularTraps_TrapErrosAuthFail
*
* @purpose Get 'TrapErrosAuthFail'
 *@description  [TrapErrosAuthFail] Allows a user to enable/disable
* Authentication Failure Trap   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfv2GranularTraps_TrapErrosAuthFail (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTrapErrosAuthFailValue = L7_DISABLE;
  xLibU32_t val = 0;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  if ( (usmDbOspfTrapFlagsGet(L7_UNIT_CURRENT, &val)) == L7_SUCCESS ) /* val is ospf trapflag value */
  {
    if(val & L7_OSPF_TRAP_IF_AUTH_FAILURE)
    {
      objTrapErrosAuthFailValue = L7_ENABLE;
    }
  }

  FPOBJ_TRACE_VALUE (bufp, &objTrapErrosAuthFailValue, sizeof (objTrapErrosAuthFailValue));

  /* return the object value: TrapErrosAuthFail */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTrapErrosAuthFailValue,
                           sizeof (objTrapErrosAuthFailValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_routingospfv2GranularTraps_TrapErrosAuthFail
*
* @purpose Set 'TrapErrosAuthFail'
 *@description  [TrapErrosAuthFail] Allows a user to enable/disable
* Authentication Failure Trap   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfv2GranularTraps_TrapErrosAuthFail (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTrapErrosAuthFailValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: TrapErrosAuthFail */
  owa.len = sizeof (objTrapErrosAuthFailValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objTrapErrosAuthFailValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTrapErrosAuthFailValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbOspfTrapModeSet (L7_UNIT_CURRENT, objTrapErrosAuthFailValue, L7_OSPF_TRAP_IF_AUTH_FAILURE);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_routingospfv2GranularTraps_TrapBadPkt
*
* @purpose Get 'TrapBadPkt'
 *@description  [TrapBadPkt] Allows a user to enable/disable the bad-packet Trap   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfv2GranularTraps_TrapBadPkt (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTrapBadPktValue = L7_DISABLE;
  xLibU32_t val = 0;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  if ( (usmDbOspfTrapFlagsGet(L7_UNIT_CURRENT, &val)) == L7_SUCCESS ) /* val is ospf trapflag value */
  {
    if(val & L7_OSPF_TRAP_RX_BAD_PACKET)
    {
      objTrapBadPktValue = L7_ENABLE;
    }
  }
  FPOBJ_TRACE_VALUE (bufp, &objTrapBadPktValue, sizeof (objTrapBadPktValue));

  /* return the object value: TrapBadPkt */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTrapBadPktValue, sizeof (objTrapBadPktValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_routingospfv2GranularTraps_TrapBadPkt
*
* @purpose Set 'TrapBadPkt'
 *@description  [TrapBadPkt] Allows a user to enable/disable the bad-packet Trap   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfv2GranularTraps_TrapBadPkt (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTrapBadPktValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: TrapBadPkt */
  owa.len = sizeof (objTrapBadPktValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objTrapBadPktValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTrapBadPktValue, owa.len);


  /* set the value in application */
  owa.l7rc = usmDbOspfTrapModeSet (L7_UNIT_CURRENT, objTrapBadPktValue, L7_OSPF_TRAP_RX_BAD_PACKET);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_routingospfv2GranularTraps_TrapConfgError
*
* @purpose Get 'TrapConfgError'
 *@description  [TrapConfgError] Allows a user to enable/disable the
* Configuration Error Trap   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfv2GranularTraps_TrapConfgError (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTrapConfgErrorValue = L7_DISABLE;
  xLibU32_t val = 0;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  if ( (usmDbOspfTrapFlagsGet(L7_UNIT_CURRENT, &val)) == L7_SUCCESS ) /* val is ospf trapflag value */
  {
    if(val & L7_OSPF_TRAP_IF_CONFIG_ERROR)
    {
      objTrapConfgErrorValue = L7_ENABLE;
    }
  }
  FPOBJ_TRACE_VALUE (bufp, &objTrapConfgErrorValue, sizeof (objTrapConfgErrorValue));

  /* return the object value: TrapConfgError */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTrapConfgErrorValue,
                           sizeof (objTrapConfgErrorValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_routingospfv2GranularTraps_TrapConfgError
*
* @purpose Set 'TrapConfgError'
 *@description  [TrapConfgError] Allows a user to enable/disable the
* Configuration Error Trap   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfv2GranularTraps_TrapConfgError (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTrapConfgErrorValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: TrapConfgError */
  owa.len = sizeof (objTrapConfgErrorValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objTrapConfgErrorValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTrapConfgErrorValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbOspfTrapModeSet (L7_UNIT_CURRENT, objTrapConfgErrorValue, L7_OSPF_TRAP_IF_CONFIG_ERROR);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_routingospfv2GranularTraps_TrapVirtAuthFail
*
* @purpose Get 'TrapVirtAuthFail'
 *@description  [TrapVirtAuthFail] Allows a user to enable/disable the Virtual
* Authentication Failure Trap   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfv2GranularTraps_TrapVirtAuthFail (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTrapVirtAuthFailValue = L7_DISABLE;
  xLibU32_t val = 0;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  if ( (usmDbOspfTrapFlagsGet(L7_UNIT_CURRENT, &val)) == L7_SUCCESS ) /* val is ospf trapflag value */
  {
    if(val & L7_OSPF_TRAP_VIRT_IF_AUTH_FAILURE)
    {
       objTrapVirtAuthFailValue = L7_ENABLE;
    }
  }
  FPOBJ_TRACE_VALUE (bufp, &objTrapVirtAuthFailValue, sizeof (objTrapVirtAuthFailValue));

  /* return the object value: TrapVirtAuthFail */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTrapVirtAuthFailValue,
                           sizeof (objTrapVirtAuthFailValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_routingospfv2GranularTraps_TrapVirtAuthFail
*
* @purpose Set 'TrapVirtAuthFail'
 *@description  [TrapVirtAuthFail] Allows a user to enable/disable the Virtual
* Authentication Failure Trap   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfv2GranularTraps_TrapVirtAuthFail (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTrapVirtAuthFailValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: TrapVirtAuthFail */
  owa.len = sizeof (objTrapVirtAuthFailValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objTrapVirtAuthFailValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTrapVirtAuthFailValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbOspfTrapModeSet (L7_UNIT_CURRENT, objTrapVirtAuthFailValue, L7_OSPF_TRAP_VIRT_IF_AUTH_FAILURE);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_routingospfv2GranularTraps_TrapVirtBadPkt
*
* @purpose Get 'TrapVirtBadPkt'
 *@description  [TrapVirtBadPkt] Allows a user to enable/disable the Virtual Bad
* Packet Trap   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfv2GranularTraps_TrapVirtBadPkt (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTrapVirtBadPktValue = L7_DISABLE;
  xLibU32_t val = 0;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  if ( (usmDbOspfTrapFlagsGet(L7_UNIT_CURRENT, &val)) == L7_SUCCESS ) /* val is ospf trapflag value */
  {
    if(val & L7_OSPF_TRAP_VIRT_IF_RX_BAD_PACKET)
    {
      objTrapVirtBadPktValue = L7_ENABLE;
    }
  }
  FPOBJ_TRACE_VALUE (bufp, &objTrapVirtBadPktValue, sizeof (objTrapVirtBadPktValue));

  /* return the object value: TrapVirtBadPkt */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTrapVirtBadPktValue,
                           sizeof (objTrapVirtBadPktValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_routingospfv2GranularTraps_TrapVirtBadPkt
*
* @purpose Set 'TrapVirtBadPkt'
 *@description  [TrapVirtBadPkt] Allows a user to enable/disable the Virtual Bad
* Packet Trap   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfv2GranularTraps_TrapVirtBadPkt (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTrapVirtBadPktValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: TrapVirtBadPkt */
  owa.len = sizeof (objTrapVirtBadPktValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objTrapVirtBadPktValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTrapVirtBadPktValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbOspfTrapModeSet (L7_UNIT_CURRENT, objTrapVirtBadPktValue, L7_OSPF_TRAP_VIRT_IF_RX_BAD_PACKET);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_routingospfv2GranularTraps_TrapVirtCfgError
*
* @purpose Get 'TrapVirtCfgError'
 *@description  [TrapVirtCfgError] Allows a user to enable/disable the Virtual
* Config Error Trap   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfv2GranularTraps_TrapVirtCfgError (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTrapVirtCfgErrorValue = L7_DISABLE;
  xLibU32_t val = 0;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  if ( (usmDbOspfTrapFlagsGet(L7_UNIT_CURRENT, &val)) == L7_SUCCESS ) /* val is ospf trapflag value */
  {
    if(val & L7_OSPF_TRAP_VIRT_IF_CONFIG_ERROR)
    {
      objTrapVirtCfgErrorValue = L7_ENABLE;
    }
  }
  FPOBJ_TRACE_VALUE (bufp, &objTrapVirtCfgErrorValue, sizeof (objTrapVirtCfgErrorValue));

  /* return the object value: TrapVirtCfgError */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTrapVirtCfgErrorValue,
                           sizeof (objTrapVirtCfgErrorValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_routingospfv2GranularTraps_TrapVirtCfgError
*
* @purpose Set 'TrapVirtCfgError'
 *@description  [TrapVirtCfgError] Allows a user to enable/disable the Virtual
* Config Error Trap   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfv2GranularTraps_TrapVirtCfgError (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTrapVirtCfgErrorValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: TrapVirtCfgError */
  owa.len = sizeof (objTrapVirtCfgErrorValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objTrapVirtCfgErrorValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTrapVirtCfgErrorValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbOspfTrapModeSet (L7_UNIT_CURRENT, objTrapVirtCfgErrorValue, L7_OSPF_TRAP_VIRT_IF_CONFIG_ERROR);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_routingospfv2GranularTraps_TrapIfRxPkt
*
* @purpose Get 'TrapIfRxPkt'
 *@description  [TrapIfRxPkt] Allows a user to enable/disable If-Rx pkt Trap   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfv2GranularTraps_TrapIfRxPkt (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTrapIfRxPktValue = L7_DISABLE;
  xLibU32_t val = 0;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  if ( (usmDbOspfTrapFlagsGet(L7_UNIT_CURRENT, &val)) == L7_SUCCESS ) /* val is ospf trapflag value */
  {
    if(val & L7_OSPF_TRAP_IF_RX_PACKET)
    {
      objTrapIfRxPktValue = L7_ENABLE;
    }
  }

  FPOBJ_TRACE_VALUE (bufp, &objTrapIfRxPktValue, sizeof (objTrapIfRxPktValue));

  /* return the object value: TrapIfRxPkt */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTrapIfRxPktValue, sizeof (objTrapIfRxPktValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_routingospfv2GranularTraps_TrapIfRxPkt
*
* @purpose Set 'TrapIfRxPkt'
 *@description  [TrapIfRxPkt] Allows a user to enable/disable If-Rx pkt Trap   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfv2GranularTraps_TrapIfRxPkt (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTrapIfRxPktValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: TrapIfRxPkt */
  owa.len = sizeof (objTrapIfRxPktValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objTrapIfRxPktValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTrapIfRxPktValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbOspfTrapModeSet (L7_UNIT_CURRENT, objTrapIfRxPktValue, L7_OSPF_TRAP_IF_RX_PACKET);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_routingospfv2GranularTraps_TrapLSAAll
*
* @purpose Get 'TrapLSAAll'
 *@description  [TrapLSAAll] Allows a user to enable/disable all the ospfv2 LSA
* Traps   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfv2GranularTraps_TrapLSAAll (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTrapLSAAllValue = L7_DISABLE;
  xLibU32_t val = 0;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  if ( (usmDbOspfTrapFlagsGet(L7_UNIT_CURRENT, &val)) == L7_SUCCESS ) /* val is ospf trapflag value */
  {
    if((val & L7_OSPF_TRAP_LSA_ALL) == L7_OSPF_TRAP_LSA_ALL)
    {
      objTrapLSAAllValue = L7_ENABLE;
    }
  }

  FPOBJ_TRACE_VALUE (bufp, &objTrapLSAAllValue, sizeof (objTrapLSAAllValue));

  /* return the object value: TrapLSAAll */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTrapLSAAllValue, sizeof (objTrapLSAAllValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_routingospfv2GranularTraps_TrapLSAAll
*
* @purpose Set 'TrapLSAAll'
 *@description  [TrapLSAAll] Allows a user to enable/disable all the ospfv2 LSA
* Traps   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfv2GranularTraps_TrapLSAAll (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTrapLSAAllValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: TrapLSAAll */
  owa.len = sizeof (objTrapLSAAllValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objTrapLSAAllValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTrapLSAAllValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbOspfTrapModeSet (L7_UNIT_CURRENT, objTrapLSAAllValue, L7_OSPF_TRAP_LSA_ALL);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_routingospfv2GranularTraps_TrapLSAMaxAge
*
* @purpose Get 'TrapLSAMaxAge'
 *@description  [TrapLSAMaxAge] Allows a user to enable/disable LSA Max Age Trap   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfv2GranularTraps_TrapLSAMaxAge (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTrapLSAMaxAgeValue = L7_DISABLE;
  xLibU32_t val = 0;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  if ( (usmDbOspfTrapFlagsGet(L7_UNIT_CURRENT, &val)) == L7_SUCCESS ) /* val is ospf trapflag value */
  {
    if(val & L7_OSPF_TRAP_MAX_AGE_LSA)
    {
      objTrapLSAMaxAgeValue  = L7_ENABLE;
    }
  }
  FPOBJ_TRACE_VALUE (bufp, &objTrapLSAMaxAgeValue, sizeof (objTrapLSAMaxAgeValue));

  /* return the object value: TrapLSAMaxAge */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTrapLSAMaxAgeValue,
                           sizeof (objTrapLSAMaxAgeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_routingospfv2GranularTraps_TrapLSAMaxAge
*
* @purpose Set 'TrapLSAMaxAge'
 *@description  [TrapLSAMaxAge] Allows a user to enable/disable LSA Max Age Trap   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfv2GranularTraps_TrapLSAMaxAge (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTrapLSAMaxAgeValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: TrapLSAMaxAge */
  owa.len = sizeof (objTrapLSAMaxAgeValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objTrapLSAMaxAgeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTrapLSAMaxAgeValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbOspfTrapModeSet (L7_UNIT_CURRENT, objTrapLSAMaxAgeValue, L7_OSPF_TRAP_MAX_AGE_LSA);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_routingospfv2GranularTraps_TrapLSAOriginate
*
* @purpose Get 'TrapLSAOriginate'
 *@description  [TrapLSAOriginate] Allows a user to enable/disable LSA Originate
* Trap   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfv2GranularTraps_TrapLSAOriginate (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTrapLSAOriginateValue = L7_DISABLE;
  xLibU32_t val = 0;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  if ( (usmDbOspfTrapFlagsGet(L7_UNIT_CURRENT, &val)) == L7_SUCCESS ) /* val is ospf trapflag value */
  {
    if(val & L7_OSPF_TRAP_ORIGINATE_LSA)
    {
      objTrapLSAOriginateValue = L7_ENABLE;
    }
  }

  FPOBJ_TRACE_VALUE (bufp, &objTrapLSAOriginateValue, sizeof (objTrapLSAOriginateValue));

  /* return the object value: TrapLSAOriginate */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTrapLSAOriginateValue,
                           sizeof (objTrapLSAOriginateValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_routingospfv2GranularTraps_TrapLSAOriginate
*
* @purpose Set 'TrapLSAOriginate'
 *@description  [TrapLSAOriginate] Allows a user to enable/disable LSA Originate
* Trap   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfv2GranularTraps_TrapLSAOriginate (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTrapLSAOriginateValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: TrapLSAOriginate */
  owa.len = sizeof (objTrapLSAOriginateValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objTrapLSAOriginateValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTrapLSAOriginateValue, owa.len);


  /* set the value in application */
  owa.l7rc = usmDbOspfTrapModeSet (L7_UNIT_CURRENT, objTrapLSAOriginateValue, L7_OSPF_TRAP_ORIGINATE_LSA);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_routingospfv2GranularTraps_TrapOverflowAll
*
* @purpose Get 'TrapOverflowAll'
 *@description  [TrapOverflowAll] Allows a user to enable/disable all the ospfv2
* Overflow Traps   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfv2GranularTraps_TrapOverflowAll (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTrapOverflowAllValue = L7_DISABLE;
  xLibU32_t val = 0;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  if ( (usmDbOspfTrapFlagsGet(L7_UNIT_CURRENT, &val)) == L7_SUCCESS ) /* val is ospf trapflag value */
  {
    if((val & L7_OSPF_TRAP_OVERFLOW_ALL) == L7_OSPF_TRAP_OVERFLOW_ALL)
    {
      objTrapOverflowAllValue = L7_ENABLE;
    }
  }
  FPOBJ_TRACE_VALUE (bufp, &objTrapOverflowAllValue, sizeof (objTrapOverflowAllValue));

  /* return the object value: TrapOverflowAll */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTrapOverflowAllValue,
                           sizeof (objTrapOverflowAllValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_routingospfv2GranularTraps_TrapOverflowAll
*
* @purpose Set 'TrapOverflowAll'
 *@description  [TrapOverflowAll] Allows a user to enable/disable all the ospfv2
* Overflow Traps   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfv2GranularTraps_TrapOverflowAll (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTrapOverflowAllValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: TrapOverflowAll */
  owa.len = sizeof (objTrapOverflowAllValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objTrapOverflowAllValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTrapOverflowAllValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbOspfTrapModeSet (L7_UNIT_CURRENT, objTrapOverflowAllValue, L7_OSPF_TRAP_OVERFLOW_ALL);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_routingospfv2GranularTraps_TrapLsdbOverFlow
*
* @purpose Get 'TrapLsdbOverFlow'
 *@description  [TrapLsdbOverFlow] Allows a user to enable/disable LSDB Overflow
* Trap   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfv2GranularTraps_TrapLsdbOverFlow (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTrapLsdbOverFlowValue = L7_DISABLE;
  xLibU32_t val = 0;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  if ( (usmDbOspfTrapFlagsGet(L7_UNIT_CURRENT, &val)) == L7_SUCCESS ) /* val is ospf trapflag value */
  {
    if(val & L7_OSPF_TRAP_LS_DB_OVERFLOW)
    {
      objTrapLsdbOverFlowValue = L7_ENABLE;
    }
  }

  FPOBJ_TRACE_VALUE (bufp, &objTrapLsdbOverFlowValue, sizeof (objTrapLsdbOverFlowValue));

  /* return the object value: TrapLsdbOverFlow */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTrapLsdbOverFlowValue,
                           sizeof (objTrapLsdbOverFlowValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_routingospfv2GranularTraps_TrapLsdbOverFlow
*
* @purpose Set 'TrapLsdbOverFlow'
 *@description  [TrapLsdbOverFlow] Allows a user to enable/disable LSDB Overflow
* Trap   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfv2GranularTraps_TrapLsdbOverFlow (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTrapLsdbOverFlowValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: TrapLsdbOverFlow */
  owa.len = sizeof (objTrapLsdbOverFlowValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objTrapLsdbOverFlowValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTrapLsdbOverFlowValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbOspfTrapModeSet (L7_UNIT_CURRENT, objTrapLsdbOverFlowValue, L7_OSPF_TRAP_LS_DB_OVERFLOW);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_routingospfv2GranularTraps_TrapLsdbApproachOverflow
*
* @purpose Get 'TrapLsdbApproachOverflow'
 *@description  [TrapLsdbApproachOverflow] Allows a user to enable/disable LSDB
* Approaching Overflow Trap   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfv2GranularTraps_TrapLsdbApproachOverflow (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTrapLsdbApproachOverflowValue = L7_DISABLE;
  xLibU32_t val = 0;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  if ( (usmDbOspfTrapFlagsGet(L7_UNIT_CURRENT, &val)) == L7_SUCCESS ) /* val is ospf trapflag value */
  {
    if(val & L7_OSPF_TRAP_LS_DB_APPROACHING_OVERFLOW)
    {
      objTrapLsdbApproachOverflowValue = L7_ENABLE;
    }
  }

  FPOBJ_TRACE_VALUE (bufp, &objTrapLsdbApproachOverflowValue,
                     sizeof (objTrapLsdbApproachOverflowValue));

  /* return the object value: TrapLsdbApproachOverflow */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTrapLsdbApproachOverflowValue,
                           sizeof (objTrapLsdbApproachOverflowValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_routingospfv2GranularTraps_TrapLsdbApproachOverflow
*
* @purpose Set 'TrapLsdbApproachOverflow'
 *@description  [TrapLsdbApproachOverflow] Allows a user to enable/disable LSDB
* Approaching Overflow Trap   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfv2GranularTraps_TrapLsdbApproachOverflow (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTrapLsdbApproachOverflowValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: TrapLsdbApproachOverflow */
  owa.len = sizeof (objTrapLsdbApproachOverflowValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objTrapLsdbApproachOverflowValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTrapLsdbApproachOverflowValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbOspfTrapModeSet (L7_UNIT_CURRENT, objTrapLsdbApproachOverflowValue, L7_OSPF_TRAP_LS_DB_APPROACHING_OVERFLOW);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_routingospfv2GranularTraps_TrapRetxAll
*
* @purpose Get 'TrapRetxAll'
 *@description  [TrapRetxAll] Allows a user to enable/disable all the ospfv2
* Retransmit Traps   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfv2GranularTraps_TrapRetxAll (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTrapRetxAllValue = L7_DISABLE;
  xLibU32_t val = 0;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  if ( (usmDbOspfTrapFlagsGet(L7_UNIT_CURRENT, &val)) == L7_SUCCESS ) /* val is ospf trapflag value */
  {
    if((val & L7_OSPF_TRAP_RETRANSMIT_ALL) == L7_OSPF_TRAP_RETRANSMIT_ALL)
    {
      objTrapRetxAllValue = L7_ENABLE;
    }
  }

  FPOBJ_TRACE_VALUE (bufp, &objTrapRetxAllValue, sizeof (objTrapRetxAllValue));

  /* return the object value: TrapRetxAll */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTrapRetxAllValue, sizeof (objTrapRetxAllValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_routingospfv2GranularTraps_TrapRetxAll
*
* @purpose Set 'TrapRetxAll'
 *@description  [TrapRetxAll] Allows a user to enable/disable all the ospfv2
* Retransmit Traps   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfv2GranularTraps_TrapRetxAll (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTrapRetxAllValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: TrapRetxAll */
  owa.len = sizeof (objTrapRetxAllValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objTrapRetxAllValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTrapRetxAllValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbOspfTrapModeSet (L7_UNIT_CURRENT, objTrapRetxAllValue, L7_OSPF_TRAP_RETRANSMIT_ALL);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_routingospfv2GranularTraps_TrapRetxPkts
*
* @purpose Get 'TrapRetxPkts'
 *@description  [TrapRetxPkts] Allows a user to enable/disable Retransmit
* Packets Trap   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfv2GranularTraps_TrapRetxPkts (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTrapRetxPktsValue = L7_DISABLE;
  xLibU32_t val = 0;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  if ( (usmDbOspfTrapFlagsGet(L7_UNIT_CURRENT, &val)) == L7_SUCCESS ) /* val is ospf trapflag value */
  {
    if(val & L7_OSPF_TRAP_TX_RETRANSMIT)
    {
      objTrapRetxPktsValue = L7_ENABLE;
    }
  }
  FPOBJ_TRACE_VALUE (bufp, &objTrapRetxPktsValue, sizeof (objTrapRetxPktsValue));

  /* return the object value: TrapRetxPkts */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTrapRetxPktsValue,
                           sizeof (objTrapRetxPktsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_routingospfv2GranularTraps_TrapRetxPkts
*
* @purpose Set 'TrapRetxPkts'
 *@description  [TrapRetxPkts] Allows a user to enable/disable Retransmit
* Packets Trap   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfv2GranularTraps_TrapRetxPkts (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTrapRetxPktsValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: TrapRetxPkts */
  owa.len = sizeof (objTrapRetxPktsValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objTrapRetxPktsValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTrapRetxPktsValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbOspfTrapModeSet (L7_UNIT_CURRENT, objTrapRetxPktsValue, L7_OSPF_TRAP_TX_RETRANSMIT);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_routingospfv2GranularTraps_TrapRetxVirtualPkts
*
* @purpose Get 'TrapRetxVirtualPkts'
 *@description  [TrapRetxVirtualPkts] Allows a user to enable/disable Retransmit
* Packets on virtual link Trap   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfv2GranularTraps_TrapRetxVirtualPkts (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTrapRetxVirtualPktsValue = L7_DISABLE;
  xLibU32_t val = 0;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  if ( (usmDbOspfTrapFlagsGet(L7_UNIT_CURRENT, &val)) == L7_SUCCESS ) /* val is ospf trapflag value */
  {
    if(val & L7_OSPF_TRAP_VIRT_IF_TX_RETRANSMIT)
    {
      objTrapRetxVirtualPktsValue = L7_ENABLE;
    }
  }

  FPOBJ_TRACE_VALUE (bufp, &objTrapRetxVirtualPktsValue, sizeof (objTrapRetxVirtualPktsValue));

  /* return the object value: TrapRetxVirtualPkts */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTrapRetxVirtualPktsValue,
                           sizeof (objTrapRetxVirtualPktsValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_routingospfv2GranularTraps_TrapRetxVirtualPkts
*
* @purpose Set 'TrapRetxVirtualPkts'
 *@description  [TrapRetxVirtualPkts] Allows a user to enable/disable Retransmit
* Packets on virtual link Trap   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfv2GranularTraps_TrapRetxVirtualPkts (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTrapRetxVirtualPktsValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: TrapRetxVirtualPkts */
  owa.len = sizeof (objTrapRetxVirtualPktsValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objTrapRetxVirtualPktsValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTrapRetxVirtualPktsValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbOspfTrapModeSet (L7_UNIT_CURRENT, objTrapRetxVirtualPktsValue, L7_OSPF_TRAP_VIRT_IF_TX_RETRANSMIT);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_routingospfv2GranularTraps_TrapRTBEntryInfo
*
* @purpose Get 'TrapRTBEntryInfo'
 *@description  [TrapRTBEntryInfo] Allows a user to enable/disable RTB Entry
* Info Trap   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfv2GranularTraps_TrapRTBEntryInfo (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTrapRTBEntryInfoValue = L7_DISABLE;
  xLibU32_t val = 0;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  if ( (usmDbOspfTrapFlagsGet(L7_UNIT_CURRENT, &val)) == L7_SUCCESS ) /* val is ospf trapflag value */
  {
    if(val & L7_OSPF_TRAP_RTB_ENTRY_INFO)
    {
      objTrapRTBEntryInfoValue = L7_ENABLE;
    }
  }
  FPOBJ_TRACE_VALUE (bufp, &objTrapRTBEntryInfoValue, sizeof (objTrapRTBEntryInfoValue));

  /* return the object value: TrapRTBEntryInfo */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTrapRTBEntryInfoValue,
                           sizeof (objTrapRTBEntryInfoValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_routingospfv2GranularTraps_TrapRTBEntryInfo
*
* @purpose Set 'TrapRTBEntryInfo'
 *@description  [TrapRTBEntryInfo] Allows a user to enable/disable RTB Entry
* Info Trap   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfv2GranularTraps_TrapRTBEntryInfo (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTrapRTBEntryInfoValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: TrapRTBEntryInfo */
  owa.len = sizeof (objTrapRTBEntryInfoValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objTrapRTBEntryInfoValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTrapRTBEntryInfoValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbOspfTrapModeSet (L7_UNIT_CURRENT, objTrapRTBEntryInfoValue, L7_OSPF_TRAP_RTB_ENTRY_INFO);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_routingospfv2GranularTraps_TrapStateChngAll
*
* @purpose Get 'TrapStateChngAll'
 *@description  [TrapStateChngAll] Allows a user to enable/disable all the
* ospfv2 State Change Traps   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfv2GranularTraps_TrapStateChngAll (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTrapStateChngAllValue = L7_DISABLE;
  xLibU32_t val = 0;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  if ( (usmDbOspfTrapFlagsGet(L7_UNIT_CURRENT, &val)) == L7_SUCCESS ) /* val is ospf trapflag value */
  {
    if((val & L7_OSPF_TRAP_STATE_CHANGE_ALL) == L7_OSPF_TRAP_STATE_CHANGE_ALL)
    {
       objTrapStateChngAllValue = L7_ENABLE;
    }
  }

  FPOBJ_TRACE_VALUE (bufp, &objTrapStateChngAllValue, sizeof (objTrapStateChngAllValue));

  /* return the object value: TrapStateChngAll */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTrapStateChngAllValue,
                           sizeof (objTrapStateChngAllValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_routingospfv2GranularTraps_TrapStateChngAll
*
* @purpose Set 'TrapStateChngAll'
 *@description  [TrapStateChngAll] Allows a user to enable/disable all the
* ospfv2 State Change Traps   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfv2GranularTraps_TrapStateChngAll (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTrapStateChngAllValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: TrapStateChngAll */
  owa.len = sizeof (objTrapStateChngAllValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objTrapStateChngAllValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTrapStateChngAllValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbOspfTrapModeSet (L7_UNIT_CURRENT, objTrapStateChngAllValue, L7_OSPF_TRAP_STATE_CHANGE_ALL);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_routingospfv2GranularTraps_TrapIfStateChng
*
* @purpose Get 'TrapIfStateChng'
 *@description  [TrapIfStateChng] Allows a user to enable/disable Interface
* State Change Trap   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfv2GranularTraps_TrapIfStateChng (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTrapIfStateChngValue = L7_DISABLE;
  xLibU32_t val = 0;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  if ( (usmDbOspfTrapFlagsGet(L7_UNIT_CURRENT, &val)) == L7_SUCCESS ) /* val is ospf trapflag value */
  {
    if(val & L7_OSPF_TRAP_IF_STATE_CHANGE)
    {
      objTrapIfStateChngValue = L7_ENABLE;
    }
  }
  FPOBJ_TRACE_VALUE (bufp, &objTrapIfStateChngValue, sizeof (objTrapIfStateChngValue));

  /* return the object value: TrapIfStateChng */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTrapIfStateChngValue,
                           sizeof (objTrapIfStateChngValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_routingospfv2GranularTraps_TrapIfStateChng
*
* @purpose Set 'TrapIfStateChng'
 *@description  [TrapIfStateChng] Allows a user to enable/disable Interface
* State Change Trap   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfv2GranularTraps_TrapIfStateChng (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTrapIfStateChngValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: TrapIfStateChng */
  owa.len = sizeof (objTrapIfStateChngValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objTrapIfStateChngValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTrapIfStateChngValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbOspfTrapModeSet (L7_UNIT_CURRENT, objTrapIfStateChngValue, L7_OSPF_TRAP_IF_STATE_CHANGE);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_routingospfv2GranularTraps_TrapNbrStateChng
*
* @purpose Get 'TrapNbrStateChng'
 *@description  [TrapNbrStateChng] Allows a user to enable/disable Neighbor
* State Change Trap   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfv2GranularTraps_TrapNbrStateChng (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTrapNbrStateChngValue = L7_DISABLE;
  xLibU32_t val = 0;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  if ( (usmDbOspfTrapFlagsGet(L7_UNIT_CURRENT, &val)) == L7_SUCCESS ) /* val is ospf trapflag value */
  {
    if(val & L7_OSPF_TRAP_NBR_STATE_CHANGE)
    {
      objTrapNbrStateChngValue = L7_ENABLE;
    }
  }

  FPOBJ_TRACE_VALUE (bufp, &objTrapNbrStateChngValue, sizeof (objTrapNbrStateChngValue));

  /* return the object value: TrapNbrStateChng */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTrapNbrStateChngValue,
                           sizeof (objTrapNbrStateChngValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_routingospfv2GranularTraps_TrapNbrStateChng
*
* @purpose Set 'TrapNbrStateChng'
 *@description  [TrapNbrStateChng] Allows a user to enable/disable Neighbor
* State Change Trap   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfv2GranularTraps_TrapNbrStateChng (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTrapNbrStateChngValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: TrapNbrStateChng */
  owa.len = sizeof (objTrapNbrStateChngValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objTrapNbrStateChngValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTrapNbrStateChngValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbOspfTrapModeSet (L7_UNIT_CURRENT, objTrapNbrStateChngValue, L7_OSPF_TRAP_NBR_STATE_CHANGE);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_routingospfv2GranularTraps_TrapVirtIfStateChng
*
* @purpose Get 'TrapVirtIfStateChng'
 *@description  [TrapVirtIfStateChng] Allows a user to enable/disable Virtual
* Interface State Change Trap   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfv2GranularTraps_TrapVirtIfStateChng (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTrapVirtIfStateChngValue = L7_DISABLE;
 xLibU32_t val = 0;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  if ( (usmDbOspfTrapFlagsGet(L7_UNIT_CURRENT, &val)) == L7_SUCCESS ) /* val is ospf trapflag value */
  {
    if(val & L7_OSPF_TRAP_VIRT_IF_STATE_CHANGE)
    {
      objTrapVirtIfStateChngValue = L7_ENABLE;
    }
  }
  FPOBJ_TRACE_VALUE (bufp, &objTrapVirtIfStateChngValue, sizeof (objTrapVirtIfStateChngValue));

  /* return the object value: TrapVirtIfStateChng */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTrapVirtIfStateChngValue,
                           sizeof (objTrapVirtIfStateChngValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_routingospfv2GranularTraps_TrapVirtIfStateChng
*
* @purpose Set 'TrapVirtIfStateChng'
 *@description  [TrapVirtIfStateChng] Allows a user to enable/disable Virtual
* Interface State Change Trap   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfv2GranularTraps_TrapVirtIfStateChng (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTrapVirtIfStateChngValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: TrapVirtIfStateChng */
  owa.len = sizeof (objTrapVirtIfStateChngValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objTrapVirtIfStateChngValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTrapVirtIfStateChngValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbOspfTrapModeSet (L7_UNIT_CURRENT, objTrapVirtIfStateChngValue, L7_OSPF_TRAP_VIRT_IF_STATE_CHANGE);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_routingospfv2GranularTraps_TrapVirtNbrStateChng
*
* @purpose Get 'TrapVirtNbrStateChng'
 *@description  [TrapVirtNbrStateChng] Allows a user to enable/disable Virtual
* Neighbor State Change Trap   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingospfv2GranularTraps_TrapVirtNbrStateChng (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTrapVirtNbrStateChngValue = L7_DISABLE;
  xLibU32_t val = 0;

  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  if ( (usmDbOspfTrapFlagsGet(L7_UNIT_CURRENT, &val)) == L7_SUCCESS ) /* val is ospf trapflag value */
  {
    if(val & L7_OSPF_TRAP_VIRT_NBR_STATE_CHANGE)
    {
      objTrapVirtNbrStateChngValue = L7_ENABLE;
    }
  }

  FPOBJ_TRACE_VALUE (bufp, &objTrapVirtNbrStateChngValue, sizeof (objTrapVirtNbrStateChngValue));

  /* return the object value: TrapVirtNbrStateChng */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTrapVirtNbrStateChngValue,
                           sizeof (objTrapVirtNbrStateChngValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_routingospfv2GranularTraps_TrapVirtNbrStateChng
*
* @purpose Set 'TrapVirtNbrStateChng'
 *@description  [TrapVirtNbrStateChng] Allows a user to enable/disable Virtual
* Neighbor State Change Trap   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingospfv2GranularTraps_TrapVirtNbrStateChng (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objTrapVirtNbrStateChngValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: TrapVirtNbrStateChng */
  owa.len = sizeof (objTrapVirtNbrStateChngValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objTrapVirtNbrStateChngValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objTrapVirtNbrStateChngValue, owa.len);

  /* set the value in application */
  owa.l7rc = usmDbOspfTrapModeSet(L7_UNIT_CURRENT, objTrapVirtNbrStateChngValue, L7_OSPF_TRAP_VIRT_NBR_STATE_CHANGE);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
