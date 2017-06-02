
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_routingswitchHelperTableGlobal.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to routing-object.xml
*
* @create  03 December 2008, Wednesday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_routingswitchHelperTableGlobal_obj.h"

/*******************************************************************************
* @function fpObjGet_routingswitchHelperTableGlobal_helperIpAddress
*
* @purpose Get 'helperIpAddress'
 *@description  [helperIpAddress] The Helper IP Address assigned to this
* interface.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingswitchHelperTableGlobal_helperIpAddress (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibIpV4_t objhelperIpAddressValue;
  xLibU32_t objhelperIpDestUDPPortValue;
  xLibU16_t nextObjhelperIpDestUDPPortValue;
  xLibU32_t nextIfIndexValue = IH_INTF_ANY; 
  L7_BOOL discard;
  xLibU32_t hitCount;
  FPOBJ_TRACE_ENTER (bufp);
  
  objhelperIpAddressValue = 0;
  /* retrieve key: helperIpDestUDPPort */
  owa.len = sizeof (objhelperIpDestUDPPortValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingswitchHelperTableGlobal_helperIpDestUDPPort,
                          (xLibU8_t *) & objhelperIpDestUDPPortValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* retrieve key: helperIpAddress */
  owa.len = sizeof (objhelperIpAddressValue);

  owa.rc = xLibFilterGet (wap, XOBJ_routingswitchHelperTableGlobal_helperIpAddress,
                          (xLibU8_t *) & objhelperIpAddressValue, &owa.len);
  nextObjhelperIpDestUDPPortValue = (xLibU16_t)objhelperIpDestUDPPortValue;
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objhelperIpAddressValue, owa.len);
  }
  owa.l7rc = usmDbIpHelperAddressNext(&nextIfIndexValue, &nextObjhelperIpDestUDPPortValue, &objhelperIpAddressValue, &discard, &hitCount);
  if ((owa.l7rc == L7_SUCCESS) && (nextObjhelperIpDestUDPPortValue != (xLibU16_t)objhelperIpDestUDPPortValue) )
  {
     owa.l7rc = L7_FAILURE;
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &objhelperIpAddressValue, owa.len);

  /* return the object value: helperIpAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objhelperIpAddressValue,
                           sizeof (objhelperIpAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingswitchHelperTableGlobal_helperIpDestUDPPort
*
* @purpose Get 'helperIpDestUDPPort'
 *@description  [helperIpDestUDPPort] Destination port of the UDP packets to be
* relayed.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingswitchHelperTableGlobal_helperIpDestUDPPort (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objhelperIpDestUDPPortValue;
  xLibIpV4_t nextObjhelperIpAddressValue;
  xLibU16_t nextObjhelperIpDestUDPPortValue;
  xLibU32_t nextIfIndexValue = IH_INTF_ANY;
  L7_BOOL discard;
  xLibU32_t hitCount;
  FPOBJ_TRACE_ENTER (bufp);

  nextObjhelperIpAddressValue = 0; 
  nextObjhelperIpDestUDPPortValue = 0;
  discard = L7_FALSE;
  /* retrieve key: helperIpDestUDPPort */
  owa.len = sizeof (objhelperIpDestUDPPortValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingswitchHelperTableGlobal_helperIpDestUDPPort,
                          (xLibU8_t *) & objhelperIpDestUDPPortValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    do
    {
      owa.l7rc = usmDbIpHelperAddressNext(&nextIfIndexValue, &nextObjhelperIpDestUDPPortValue, &nextObjhelperIpAddressValue, &discard, &hitCount);
      if (owa.l7rc == L7_SUCCESS)
      {
        if (nextIfIndexValue == IH_INTF_ANY)
        {
          break;
        }
      }
    }while(owa.l7rc == L7_SUCCESS);

  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objhelperIpDestUDPPortValue, owa.len);
    nextObjhelperIpDestUDPPortValue = (xLibU16_t)objhelperIpDestUDPPortValue;
    do
    {
    owa.l7rc = usmDbIpHelperAddressNext(&nextIfIndexValue, &nextObjhelperIpDestUDPPortValue, &nextObjhelperIpAddressValue, &discard, &hitCount);
    }while(owa.l7rc == L7_SUCCESS && (nextObjhelperIpDestUDPPortValue == (xLibU16_t)objhelperIpDestUDPPortValue));
    
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objhelperIpDestUDPPortValue = nextObjhelperIpDestUDPPortValue;
  FPOBJ_TRACE_NEW_KEY (bufp, &objhelperIpDestUDPPortValue, owa.len);

  /* return the object value: helperIpDestUDPPort */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objhelperIpDestUDPPortValue,
                           sizeof (objhelperIpDestUDPPortValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingswitchHelperTableGlobal_helperIpHitCount
*
* @purpose Get 'helperIpHitCount'
 *@description  [helperIpHitCount] The number of times a packet has been
* forwarded or discarded according to this entry.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingswitchHelperTableGlobal_helperIpHitCount (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objhelperIpHitCountValue;
  xLibIpV4_t keyhelperIpAddressValue;
  xLibU32_t keyhelperIpDestUDPPortValue;

  xLibU32_t intf = IH_INTF_ANY;
  L7_BOOL discard;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: helperIpDestUDPPort */
  owa.len = sizeof (keyhelperIpDestUDPPortValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingswitchHelperTableGlobal_helperIpDestUDPPort,
                          (xLibU8_t *) & keyhelperIpDestUDPPortValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyhelperIpDestUDPPortValue, owa.len);


  /* retrieve key: helperIpAddress */
  owa.len = sizeof (keyhelperIpAddressValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingswitchHelperTableGlobal_helperIpAddress,
                          (xLibU8_t *) & keyhelperIpAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyhelperIpAddressValue, owa.len);

  /* get the value from application */
   discard = L7_FALSE;
   owa.l7rc = usmDbIpHelperAddressGet(intf,(L7_ushort16)keyhelperIpDestUDPPortValue, keyhelperIpAddressValue,
                                     discard, &objhelperIpHitCountValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_VALUE (bufp, &objhelperIpHitCountValue, sizeof (objhelperIpHitCountValue));

  /* return the object value: helperIpHitCount */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objhelperIpHitCountValue,
                           sizeof (objhelperIpHitCountValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_routingswitchHelperTableGlobal_helperStatus
*
* @purpose Get 'helperStatus'
 *@description  [helperStatus] Creates a new entry in the Helper Address table.
* Allowed values are: createAndGo(4), destroy(6).   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_routingswitchHelperTableGlobal_helperStatus (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objhelperStatusValue;

  xLibIpV4_t keyhelperIpAddressValue;
  xLibU32_t keyhelperIpDestUDPPortValue;
  xLibU32_t intf = IH_INTF_ANY;
  L7_BOOL discard;
  xLibU32_t hitcount;
  FPOBJ_TRACE_ENTER (bufp);

   /* retrieve key: helperIpDestUDPPort */
  owa.len = sizeof (keyhelperIpDestUDPPortValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingswitchHelperTableGlobal_helperIpDestUDPPort,
                          (xLibU8_t *) & keyhelperIpDestUDPPortValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyhelperIpDestUDPPortValue, owa.len);

  /* retrieve key: helperIpAddress */
  owa.len = sizeof (keyhelperIpAddressValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingswitchHelperTableGlobal_helperIpAddress,
                          (xLibU8_t *) & keyhelperIpAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyhelperIpAddressValue, owa.len);

  /* get the value from application */
   discard = L7_FALSE;
   owa.l7rc = usmDbIpHelperAddressGet(intf,(L7_ushort16)keyhelperIpDestUDPPortValue, keyhelperIpAddressValue,
                                     discard, &hitcount);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  objhelperStatusValue = L7_ROW_STATUS_ACTIVE;
  FPOBJ_TRACE_VALUE (bufp, &objhelperStatusValue, sizeof (objhelperStatusValue));

  /* return the object value: helperStatus */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objhelperStatusValue,
                           sizeof (objhelperStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_routingswitchHelperTableGlobal_helperStatus
*
* @purpose Set 'helperStatus'
 *@description  [helperStatus] Creates a new entry in the Helper Address table.
* Allowed values are: createAndGo(4), destroy(6).   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_routingswitchHelperTableGlobal_helperStatus (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA2 ();
  xLibU32_t objhelperStatusValue;

  xLibIpV4_t keyhelperIpAddressValue;
  xLibU32_t keyhelperIpDestUDPPortValue;
  xLibU32_t intf = IH_INTF_ANY;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: helperStatus */
  owa.len = sizeof (objhelperStatusValue);
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objhelperStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objhelperStatusValue, owa.len);

  /* retrieve key: helperIpAddress */
  owa.len = sizeof (keyhelperIpAddressValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingswitchHelperTableGlobal_helperIpAddress,
                          (xLibU8_t *) & keyhelperIpAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyhelperIpAddressValue, owa.len);

  /* retrieve key: helperIpDestUDPPort */
  owa.len = sizeof (keyhelperIpDestUDPPortValue);
  owa.rc = xLibFilterGet (wap, XOBJ_routingswitchHelperTableGlobal_helperIpDestUDPPort,
                          (xLibU8_t *) & keyhelperIpDestUDPPortValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyhelperIpDestUDPPortValue, owa.len);

  owa.l7rc = L7_SUCCESS;
  if (objhelperStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {
     owa.l7rc =
     usmDbIpHelperAddressAdd(intf, (L7_ushort16)keyhelperIpDestUDPPortValue,
                             keyhelperIpAddressValue);

  }
  else if (objhelperStatusValue == L7_ROW_STATUS_DESTROY)
  {
    owa.l7rc =   usmDbIpHelperAddressDelete(intf, (L7_ushort16)keyhelperIpDestUDPPortValue,
                                 keyhelperIpAddressValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
