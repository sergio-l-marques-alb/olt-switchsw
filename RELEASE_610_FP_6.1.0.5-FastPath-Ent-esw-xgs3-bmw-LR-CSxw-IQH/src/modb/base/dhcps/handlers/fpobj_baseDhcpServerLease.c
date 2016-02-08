/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_baseDhcpServerLease.c
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
#include "_xe_baseDhcpServerLease_obj.h"
#include "usmdb_dhcps_api.h"
#include "dhcps_exports.h"

/*******************************************************************************
* @function fpObjGet_baseDhcpServerLease_IPAddress
*
* @purpose Get 'IPAddress'
*
* @description This specifies the IP address leased to the client. 
*              
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDhcpServerLease_IPAddress (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIPAddressValue;
  xLibU32_t nextObjIPAddressValue;
  xLibU32_t poolIndex,tempLeaseIndex,tempState;
  xLibStr256_t poolNameTemp;
  tempLeaseIndex = 0;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IPAddress */
  owa.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerLease_IPAddress,
                          (xLibU8_t *) & objIPAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);

    if(usmDbDhcpsPoolEntryFirst(L7_UNIT_CURRENT,poolNameTemp,&poolIndex) == L7_SUCCESS)
    {
      owa.l7rc = usmDbDhcpsLeaseEntryFirst(L7_UNIT_CURRENT,poolNameTemp, 
				&tempLeaseIndex, &nextObjIPAddressValue, &tempState);
	   if(owa.l7rc != L7_SUCCESS)
	   {
		   while(usmDbDhcpsPoolEntryNext(L7_UNIT_CURRENT,poolNameTemp,&poolIndex) == L7_SUCCESS)
		   	{
		   	   owa.l7rc = usmDbDhcpsLeaseEntryFirst(L7_UNIT_CURRENT,poolNameTemp, 
				                   &tempLeaseIndex, &nextObjIPAddressValue, &tempState);
			   if(owa.l7rc == L7_SUCCESS)
			   	{
			   	   break;
			   	}
		   	}
		}
    }
    else
   {
    owa.l7rc = L7_FAILURE;
   }
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objIPAddressValue, owa.len);
    owa.l7rc = usmDbDhcpsLeaseGetNext (L7_UNIT_CURRENT, objIPAddressValue,
                                       &nextObjIPAddressValue);
  }
  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjIPAddressValue, owa.len);

  /* return the object value: IPAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjIPAddressValue,
                           sizeof (objIPAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseDhcpServerLease_IPMask
*
* @purpose Get 'IPMask'
*
* @description This specifies the IP mask of the address leased to the client. 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDhcpServerLease_IPMask (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIPAddressValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIPMaskValue;
  xLibStr256_t tempHWAddr;
  xLibStr256_t tempClientIdentifier;
  xLibStr256_t tempPoolName;
  xLibU32_t tempRemainingTime;
  memset(tempHWAddr, 0x00, sizeof(tempHWAddr));
  	
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IPAddress */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerLease_IPAddress,
                          (xLibU8_t *) & keyIPAddressValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIPAddressValue, kwa.len);

  /* get the value from application */
    
  owa.l7rc = usmDbDhcpsLeaseDataGet (L7_UNIT_CURRENT, keyIPAddressValue,
                                     &objIPMaskValue,tempHWAddr,tempClientIdentifier,
                                     tempPoolName,&tempRemainingTime);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: IPMask */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIPMaskValue,
                           sizeof (objIPMaskValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseDhcpServerLease_HWAddress
*
* @purpose Get 'HWAddress'
*
* @description This specifies the hardware address of the client. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDhcpServerLease_HWAddress (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIPAddressValue;
	
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
  xLibStr256_t objHWAddressValue;

   xLibU32_t tempIPMask;
  xLibStr256_t tempClientIdentifier;
  xLibStr256_t tempPoolName;
  xLibU32_t tempRemainingTime;
  memset(objHWAddressValue, 0x00, sizeof(objHWAddressValue));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IPAddress */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerLease_IPAddress,
                          (xLibU8_t *) & keyIPAddressValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIPAddressValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbDhcpsLeaseDataGet (L7_UNIT_CURRENT, keyIPAddressValue,
                                     &tempIPMask,objHWAddressValue, tempClientIdentifier,
                                     tempPoolName,&tempRemainingTime);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: HWAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objHWAddressValue,
                           strlen(objHWAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseDhcpServerLease_RemainingTime
*
* @purpose Get 'RemainingTime'
*
* @description This specifies the period for which the lease is valid. It is 
*              displayed in days, hours, minutes, and seconds. 
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDhcpServerLease_RemainingTime (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIPAddressValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objRemainingTimeValue;

    xLibStr256_t tempHWAddr;
  xLibStr256_t tempClientIdentifier;
  xLibStr256_t tempPoolName;
  xLibU32_t tempIPMask;
	memset(tempHWAddr, 0x00, sizeof(tempHWAddr));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IPAddress */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerLease_IPAddress,
                          (xLibU8_t *) & keyIPAddressValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIPAddressValue, kwa.len);

  /* get the value from application */
  owa.l7rc = usmDbDhcpsLeaseDataGet (L7_UNIT_CURRENT, keyIPAddressValue,
                                     &tempIPMask,tempHWAddr,tempClientIdentifier,
                                     tempPoolName,&objRemainingTimeValue);
  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: RemainingTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objRemainingTimeValue,
                           sizeof (objRemainingTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseDhcpServerLease_Type
*
* @purpose Get 'Type'
*
* @description This specifies the type of address lease as Automatic or Manual. 
*              
*              
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDhcpServerLease_Type (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIPAddressValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objTypeValue;

  xLibStr256_t tempHWAddr;
  xLibStr256_t tempClientIdentifier;
  xLibStr256_t tempPoolName;
  xLibU32_t tempRemainingTime,tempIPMask;

  memset(tempHWAddr, 0x00, sizeof(tempHWAddr));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IPAddress */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerLease_IPAddress,
                          (xLibU8_t *) & keyIPAddressValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIPAddressValue, kwa.len);

  /* get the value from application */
  memset(tempPoolName,0x00,sizeof(tempPoolName));
  if(usmDbDhcpsLeaseDataGet (L7_UNIT_CURRENT, keyIPAddressValue,
                                     &tempIPMask,tempHWAddr,tempClientIdentifier,
                                     tempPoolName,&tempRemainingTime) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  owa.l7rc = usmDbDhcpsPoolTypeGet (L7_UNIT_CURRENT, tempPoolName,
                                     &objTypeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: Type */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objTypeValue,
                           sizeof (objTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseDhcpServerLease_Status
*
* @purpose Get 'Status'
*
* @description [Status]: This specifies the row-status of the address-lease.
*              Implemented values  are - active(1) and destroy(6). For
*              a valid range, the row-satus will return active(1). When it
*              is set to destroy(6), the corresponding address is deleted.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDhcpServerLease_Status (void *wap, void *bufp)
{
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIPAddressValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IPAddress */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerLease_IPAddress,
                          (xLibU8_t *) & keyIPAddressValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIPAddressValue, kwa.len);

  /* get the value from application */

  objStatusValue = L7_ROW_STATUS_ACTIVE;
  owa.l7rc = L7_SUCCESS;
  
  /* return the object value: Status */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStatusValue,
                           sizeof (objStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseDhcpServerLease_Status
*
* @purpose Set 'Status'
*
* @description [Status]: This specifies the row-status of the address-lease.
*              Implemented values  are - active(1) and destroy(6). For
*              a valid range, the row-satus will return active(1). When it
*              is set to destroy(6), the corresponding address is deleted.
*              
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseDhcpServerLease_Status (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStatusValue;
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIPAddressValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Status */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objStatusValue, owa.len);

  /* retrieve key: IPAddress */
  kwa.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerLease_IPAddress,
                          (xLibU8_t *) & keyIPAddressValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIPAddressValue, kwa.len);

  /* call the usmdb only for add and delete */
  if (objStatusValue == L7_ROW_STATUS_ACTIVE)
  {
    owa.l7rc = L7_SUCCESS;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else if (objStatusValue == L7_ROW_STATUS_DESTROY)
  {
    /* Delete the existing row */
    owa.l7rc = usmDbDhcpsBindingClear (L7_UNIT_CURRENT, keyIPAddressValue,
                                       L7_DHCPS_ACTIVE_LEASE);
    if (owa.l7rc != L7_SUCCESS)
    {
      owa.rc = XLIBRC_DHCPS_SPECIFIC_BINDING_DELETE_FAILED;  /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  return XLIBRC_SUCCESS;
}

