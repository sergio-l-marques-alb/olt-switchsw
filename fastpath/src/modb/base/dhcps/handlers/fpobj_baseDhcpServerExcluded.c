/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_baseDhcpServerExcluded.c
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
#include "_xe_baseDhcpServerExcluded_obj.h"
#include "usmdb_dhcps_api.h"

/*******************************************************************************
* @function fpObjGet_baseDhcpServerExcluded_RangeIndex
*
* @purpose Get 'RangeIndex'
*
* @description This specifies the index of the excluded IP address-range table. 
*              
*              
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDhcpServerExcluded_RangeIndex (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRangeIndexValue;
  xLibU32_t nextObjRangeIndexValue;
  xLibU32_t tempStartAddrValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: RangeIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerExcluded_RangeIndex,
                          (xLibU8_t *) & objRangeIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjRangeIndexValue = 1;
    while( nextObjRangeIndexValue <= L7_DHCPS_MAX_EXCLUDE_NUMBER )
	{
        owa.l7rc = usmDbDhcpsSNMPExcludeAddressRangeGet (L7_UNIT_CURRENT, nextObjRangeIndexValue,
		                                                          &tempStartAddrValue,&tempStartAddrValue);
		if (owa.l7rc == L7_SUCCESS)
		{
		    break ;
		}
		nextObjRangeIndexValue++ ;
	}
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objRangeIndexValue, owa.len);
    owa.l7rc =
      usmDbDhcpsSNMPExcludeAddressRangeGetNext (L7_UNIT_CURRENT,
                                                objRangeIndexValue,
                                                &nextObjRangeIndexValue);
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjRangeIndexValue, owa.len);

  /* return the object value: RangeIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjRangeIndexValue,
                           sizeof (objRangeIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseDhcpServerExcluded_StartIpAddress
*
* @purpose Get 'StartIpAddress'
*
* @description This specifies the starting IP address of the excluded address-range. 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDhcpServerExcluded_StartIpAddress (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRangeIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStartIpAddressValue;
  xLibU32_t tempEndAddrValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: RangeIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerExcluded_RangeIndex,
                          (xLibU8_t *) & keyRangeIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRangeIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDhcpsSNMPExcludeAddressRangeGet (L7_UNIT_CURRENT, keyRangeIndexValue,
                                          &objStartIpAddressValue,&tempEndAddrValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: StartIpAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStartIpAddressValue,
                           sizeof (objStartIpAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseDhcpServerExcluded_EndIpAddress
*
* @purpose Get 'EndIpAddress'
*
* @description This specifies the ending IP address of the excluded address-range. 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDhcpServerExcluded_EndIpAddress (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRangeIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objEndIpAddressValue;
  xLibU32_t tempStartAddrValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: RangeIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerExcluded_RangeIndex,
                          (xLibU8_t *) & keyRangeIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRangeIndexValue, kwa.len);

  /* get the value from application */
  owa.l7rc =
    usmDbDhcpsSNMPExcludeAddressRangeGet (L7_UNIT_CURRENT, keyRangeIndexValue,
                                          &tempStartAddrValue,&objEndIpAddressValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: EndIpAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objEndIpAddressValue,
                           sizeof (objEndIpAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseDhcpServerExcluded_Status
*
* @purpose Get 'Status'
*
* @description [Status]: This specifies the row-status of the excluded address-range.
*              Implemented values are - active(1) and destroy(6).
*              For a valid range, the row-satus will return active(1).
*              When it is set to destroy(6), the correspondi 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDhcpServerExcluded_Status (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRangeIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: RangeIndex */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerExcluded_RangeIndex,
                          (xLibU8_t *) & keyRangeIndexValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRangeIndexValue, kwa.len);

  /* get the value from application */
  objStatusValue = L7_ROW_STATUS_ACTIVE;

  /* return the object value: Status */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStatusValue,
                           sizeof (objStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseDhcpServerExcluded_Status
*
* @purpose Set 'Status'
*
* @description [Status]: This specifies the row-status of the excluded address-range.
*              Implemented values are - active(1) and destroy(6).
*              For a valid range, the row-satus will return active(1).
*              When it is set to destroy(6), the correspondi 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseDhcpServerExcluded_Status (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStatusValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyRangeIndexValue;

  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRangeStartIPAddrValue;

  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRangeEndIPAddrValue;

  L7_uint32 startIPAddress = L7_NULL;
  L7_uint32 endIPAddress = L7_NULL;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Status */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objStatusValue, owa.len);

  /* call the usmdb only for add and delete */
  if (objStatusValue == L7_ROW_STATUS_ACTIVE)
  {
      owa.rc = XLIBRC_SUCCESS;  /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
  }
  /* call the usmdb only for add and delete */
  else if (objStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {  
    FPOBJ_TRACE_ENTER (bufp);

    /* retrieve key: RangeIndex */
    kwa1.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerExcluded_RangeStartIpAddress,
                            (xLibU8_t *) & objRangeStartIPAddrValue, &kwa1.len);
    if (kwa1.rc != XLIBRC_SUCCESS)
    {
      FPOBJ_TRACE_EXIT (bufp, kwa1);
      return kwa1.rc;
    }

    kwa2.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerExcluded_RangeEndIpAddress,
                            (xLibU8_t *) & objRangeEndIPAddrValue, &kwa2.len);
    if (kwa2.rc != XLIBRC_SUCCESS)
    {
      FPOBJ_TRACE_EXIT (bufp, kwa2);
      return kwa2.rc;
    }

    if(objRangeEndIPAddrValue == 0)
    {
       objRangeEndIPAddrValue = objRangeStartIPAddrValue;
    }

    if(objRangeStartIPAddrValue == 0)
    {
      owa.rc = XLIBRC_SUCCESS;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    /* set the value in application */

    owa.l7rc = usmDbDhcpsExcludedAddressRangeAdd (L7_UNIT_CURRENT, objRangeStartIPAddrValue, objRangeEndIPAddrValue);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
  }
  else if (objStatusValue == L7_ROW_STATUS_DESTROY)
  {  
           /* retrieve key: RangeIndex */
    kwa.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerExcluded_RangeIndex,
                            (xLibU8_t *) & keyRangeIndexValue, &kwa.len);
    if (kwa.rc != XLIBRC_SUCCESS)
    {
      kwa.rc = XLIBRC_FILTER_MISSING;
      FPOBJ_TRACE_EXIT (bufp, kwa);
      return kwa.rc;
    }
    FPOBJ_TRACE_CURRENT_KEY (bufp, &keyRangeIndexValue, kwa.len);


    /* Delete the existing row */
    owa.l7rc = usmDbDhcpsSNMPExcludeAddressRangeGet(L7_UNIT_CURRENT,keyRangeIndexValue, &startIPAddress,
	                                                                   &endIPAddress);
    if(owa.l7rc == L7_SUCCESS)
   {
	    owa.l7rc = usmDbDhcpsExcludedAddressRangeDelete(L7_UNIT_CURRENT,startIPAddress,endIPAddress);
   }

    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  return XLIBRC_SUCCESS;
}


/*******************************************************************************
* @function fpObjSet_baseDhcpServerExcluded_RangeStartIpAddress
*
* @purpose Set 'RangeStartIpAddress'
 *@description  [RangeStartIpAddress] This specifies the starting IP address of
* the excluded address-range.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseDhcpServerExcluded_RangeStartIpAddress (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibIpV4_t));
  xLibIpV4_t objRangeStartIpAddressValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: RangeStartIpAddress */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objRangeStartIpAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objRangeStartIpAddressValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = L7_SUCCESS;

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_baseDhcpServerExcluded_RangeEndIpAddress
*
* @purpose Set 'RangeEndIpAddress'
 *@description  [RangeEndIpAddress] This specifies the ending IP address of the
* excluded address-range.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseDhcpServerExcluded_RangeEndIpAddress (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibIpV4_t));
  xLibIpV4_t objRangeEndIpAddressValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: RangeEndIpAddress */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objRangeEndIpAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objRangeEndIpAddressValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */
  owa.l7rc = L7_SUCCESS;
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjSet_baseDhcpServerExcluded_createExcludeAddrRange
*
* @purpose Set 'createExcludeAddrRange'
 *@description  [createExcludeAddrRange] This creates  excluded address-range.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseDhcpServerExcluded_createExcludeAddrRange (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));

  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRangeStartIPAddrValue;

  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRangeEndIPAddrValue;
	
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: RangeIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerExcluded_RangeStartIpAddress,
                          (xLibU8_t *) & objRangeStartIPAddrValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }

  kwa2.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerExcluded_RangeEndIpAddress,
                          (xLibU8_t *) & objRangeEndIPAddrValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }

  
  
if(objRangeEndIPAddrValue == 0)
{
     objRangeEndIPAddrValue = objRangeStartIPAddrValue;
}

  if(objRangeStartIPAddrValue == 0)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objcreateExcludeAddrRangeValue, owa.len);

  /* if row status object is specified and eual to delete return success */

  /* set the value in application */

  owa.l7rc = usmDbDhcpsExcludedAddressRangeAdd (L7_UNIT_CURRENT,
                                                                                objRangeStartIPAddrValue,
                                                                                objRangeEndIPAddrValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


