
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_SwitchingFdbAgingTimeoutEntry.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to Switching-object.xml
*
* @create  26 May 2008, Monday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_SwitchingFdbAgingTimeoutEntry_obj.h"
#include "usmdb_mib_bridge_api.h"
#include "usmdb_util_api.h"

/*******************************************************************************
* @function fpObjGet_SwitchingFdbAgingTimeoutEntry_ID
*
* @purpose Get 'ID'
 *@description  [ID] FDB ID   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingFdbAgingTimeoutEntry_ID (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIDValue;
  xLibU32_t nextObjIDValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ID */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingFdbAgingTimeoutEntry_ID,
      (xLibU8_t *) & objIDValue, &owa.len);
  if(usmDbFeaturePresentCheck(L7_UNIT_CURRENT, L7_FDB_COMPONENT_ID, 
        L7_FDB_AGETIME_PER_VLAN_FEATURE_ID) == L7_TRUE)
  {
    if (owa.rc != XLIBRC_SUCCESS)
    {
      FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
      owa.l7rc = usmDbFdbIdNextGet(L7_UNIT_CURRENT, 0 , &nextObjIDValue);
    }
    else
    {
      FPOBJ_TRACE_CURRENT_KEY (bufp, &objIDValue, owa.len);
      owa.l7rc = usmDbFdbIdNextGet(L7_UNIT_CURRENT, objIDValue+1, &nextObjIDValue);
    }

    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_ENDOF_TABLE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }
  else
  {
    /* Ageout per vlan is not supported */
    if (owa.rc != XLIBRC_SUCCESS)
    {
      FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
      nextObjIDValue = FD_SIM_DEFAULT_MGMT_VLAN_ID;
      owa.rc = XLIBRC_SUCCESS;
    }
    else
    {
      FPOBJ_TRACE_CURRENT_KEY (bufp, &objIDValue, owa.len);
      owa.l7rc = L7_FAILURE;
    }

    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_ENDOF_TABLE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjIDValue, owa.len);

  /* return the object value: ID */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjIDValue, sizeof (objIDValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_SwitchingFdbAgingTimeoutEntry_AddressAgingTimeout
*
* @purpose Get 'AddressAgingTimeout'
 *@description  [AddressAgingTimeout] Aging timeout value for an FDB   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingFdbAgingTimeoutEntry_AddressAgingTimeout (void *wap, void *bufp)
{

  fpObjWa_t kwaID = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIDValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAddressAgingTimeoutValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ID */
  kwaID.rc = xLibFilterGet (wap, XOBJ_SwitchingFdbAgingTimeoutEntry_ID,
                            (xLibU8_t *) & keyIDValue, &kwaID.len);
  if (kwaID.rc != XLIBRC_SUCCESS)
  {
    kwaID.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaID);
    return kwaID.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIDValue, kwaID.len);

  /* get the value from application */
  owa.l7rc = usmDbFDBAddressAgingTimeoutGet(L7_UNIT_CURRENT, keyIDValue, &objAddressAgingTimeoutValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AddressAgingTimeout */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objAddressAgingTimeoutValue,
                           sizeof (objAddressAgingTimeoutValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjSet_SwitchingFdbAgingTimeoutEntry_AddressAgingTimeout
*
* @purpose Set 'AddressAgingTimeout'
 *@description  [AddressAgingTimeout] Aging timeout value for an FDB   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_SwitchingFdbAgingTimeoutEntry_AddressAgingTimeout (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objAddressAgingTimeoutValue;

  fpObjWa_t kwaID = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIDValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AddressAgingTimeout */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objAddressAgingTimeoutValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objAddressAgingTimeoutValue, owa.len);

  /* retrieve key: ID */
  kwaID.rc = xLibFilterGet (wap, XOBJ_SwitchingFdbAgingTimeoutEntry_ID,
                            (xLibU8_t *) & keyIDValue, &kwaID.len);
  if (kwaID.rc != XLIBRC_SUCCESS)
  {
    kwaID.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaID);
    return kwaID.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIDValue, kwaID.len);

  /* set the value in application */
  owa.l7rc = usmDbFDBAddressAgingTimeoutSet(L7_UNIT_CURRENT, keyIDValue, objAddressAgingTimeoutValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
