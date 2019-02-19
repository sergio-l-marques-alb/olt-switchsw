
#include "fpobj_util.h"
#include "_xe_ipmcastmgmdGroupTable_obj.h"
#include "l3_mcast_commdefs.h"
#include "l7_mgmd_api.h"
#include "usmdb_igmp_api.h"
#include "usmdb_mib_igmp_api.h"

L7_char8 *pStrInfo_common_Line_Temp = "-----";
L7_char8 *pStrErr_common_Err_Temp = "Err";


xLibRC_t fpObjGet_ipmcastmgmdGroupTable_mgmdGroupAddressType (void *wap, void *bufp)
{
  xLibU32_t objmgmdGroupAddressTypeValue;
  xLibU32_t nextObjmgmdGroupAddressTypeValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdGroupAddressType */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastmgmdGroupTable_mgmdGroupAddressType,
                          (xLibU8_t *) & objmgmdGroupAddressTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextObjmgmdGroupAddressTypeValue = L7_INET_ADDR_TYPE_IPV4;
    owa.l7rc = L7_SUCCESS;
  }
  
  else if (objmgmdGroupAddressTypeValue == L7_INET_ADDR_TYPE_IPV4)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &nextObjmgmdGroupAddressTypeValue, owa.len);
    nextObjmgmdGroupAddressTypeValue = L7_INET_ADDR_TYPE_IPV6;
    owa.l7rc = L7_SUCCESS;
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objmgmdGroupAddressTypeValue, owa.len);
    owa.l7rc = L7_FAILURE;
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjmgmdGroupAddressTypeValue, owa.len);

  /* return the object value: mgmdGroupAddressType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjmgmdGroupAddressTypeValue,
                           sizeof (nextObjmgmdGroupAddressTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}




xLibRC_t fpObjGet_ipmcastmgmdGroupTable_mgmdGroupAddress (void *wap, void *bufp)
{

  xLibU32_t objmgmdGroupAddressTypeValue;
  xLibU32_t intIfNum = 0;  
  L7_inet_addr_t objmgmdGroupAddressValue;
  L7_inet_addr_t nextObjmgmdGroupAddressValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  fpObjWa_t kwaType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  FPOBJ_TRACE_ENTER (bufp);

  memset(&objmgmdGroupAddressValue,0, sizeof(objmgmdGroupAddressValue));

  /* retrieve key: mgmdGroupAddressType */
  kwaType.rc = xLibFilterGet (wap, XOBJ_ipmcastmgmdGroupTable_mgmdGroupAddressType,
                          (xLibU8_t *) & objmgmdGroupAddressTypeValue, &kwaType.len);
  if (kwaType.rc != XLIBRC_SUCCESS)
  {
    kwaType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaType);
    return kwaType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objmgmdGroupAddressTypeValue, kwaType.len);

    
  memset(&nextObjmgmdGroupAddressValue, 0, sizeof(nextObjmgmdGroupAddressValue));

  /* retrieve key: mgmdGroupAddress */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastmgmdGroupTable_mgmdGroupAddress,
                          (xLibU8_t *) &objmgmdGroupAddressValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbMgmdGrpIntfEntryNextGet (objmgmdGroupAddressTypeValue, &nextObjmgmdGroupAddressValue, &intIfNum);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objmgmdGroupAddressValue, owa.len);
    intIfNum = 0;
    do
    {
      owa.l7rc = usmDbMgmdGrpIntfEntryNextGet(objmgmdGroupAddressTypeValue,
                                       &nextObjmgmdGroupAddressValue,
                                       &intIfNum);
    }while((owa.l7rc==L7_SUCCESS)&&(memcmp(&nextObjmgmdGroupAddressValue,&objmgmdGroupAddressValue,sizeof(objmgmdGroupAddressValue))));
   
    if(owa.l7rc == L7_SUCCESS)
    {
      while((!memcmp(&nextObjmgmdGroupAddressValue,&objmgmdGroupAddressValue,sizeof(objmgmdGroupAddressValue)))
              &&(owa.l7rc == L7_SUCCESS))
      {

         owa.l7rc = usmDbMgmdGrpIntfEntryNextGet(objmgmdGroupAddressTypeValue,
                                     &nextObjmgmdGroupAddressValue,
                                     &intIfNum);
      }

    }

  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjmgmdGroupAddressValue, owa.len);

  /* return the object value: mgmdGroupAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &nextObjmgmdGroupAddressValue,
                           sizeof (nextObjmgmdGroupAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}



xLibRC_t fpObjGet_ipmcastmgmdGroupTable_mgmdGroupIfIndex (void *wap, void *bufp)
{

  xLibU32_t objmgmdGroupAddressTypeValue;
  xLibU32_t objmgmdGroupIfIndexValue=0;
  xLibU32_t nextObjmgmdGroupIfIndexValue=0;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  L7_inet_addr_t objmgmdGroupAddressValue;
  L7_inet_addr_t nextObjmgmdGroupAddressValue;
  fpObjWa_t kwaAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  
  FPOBJ_TRACE_ENTER (bufp);

  memset(&objmgmdGroupAddressValue, 0x00, sizeof(objmgmdGroupAddressValue));
  memset(&nextObjmgmdGroupAddressValue, 0x00, sizeof(nextObjmgmdGroupAddressValue));
  /* retrieve key: mgmdGroupAddressType */
  kwa.rc = xLibFilterGet (wap, XOBJ_ipmcastmgmdGroupTable_mgmdGroupAddressType,
                          (xLibU8_t *) & objmgmdGroupAddressTypeValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objmgmdGroupAddressTypeValue, kwa.len);

  kwaAddress.rc = xLibFilterGet (wap, XOBJ_ipmcastmgmdGroupTable_mgmdGroupAddress,
                          (xLibU8_t *) &objmgmdGroupAddressValue, &kwaAddress.len);
  if (kwaAddress.rc != XLIBRC_SUCCESS)
  {
    kwaAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaAddress);
    return kwaAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objmgmdGroupAddressValue, kwaAddress.len);

  /* retrieve key: mgmdGroupIfIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_ipmcastmgmdGroupTable_mgmdGroupIfIndex,
                          (xLibU8_t *) & objmgmdGroupIfIndexValue, &owa.len);

  
  nextObjmgmdGroupIfIndexValue = 0;
  memset(&nextObjmgmdGroupAddressValue, 0x00, sizeof(nextObjmgmdGroupAddressValue));
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    
    do
    {
      owa.l7rc = usmDbMgmdGrpIntfEntryNextGet(objmgmdGroupAddressTypeValue,
                                       &nextObjmgmdGroupAddressValue,
                                       &nextObjmgmdGroupIfIndexValue);
    }while((owa.l7rc==L7_SUCCESS)&&(memcmp(&(nextObjmgmdGroupAddressValue.addr),&(objmgmdGroupAddressValue.addr),sizeof(objmgmdGroupAddressValue.addr))));
        
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objmgmdGroupIfIndexValue, owa.len);
    do
    {
      owa.l7rc = usmDbMgmdGrpIntfEntryNextGet (objmgmdGroupAddressTypeValue,
                                       &nextObjmgmdGroupAddressValue,
                                       &nextObjmgmdGroupIfIndexValue);
    }while((owa.l7rc == L7_SUCCESS)&&(memcmp(&(nextObjmgmdGroupAddressValue.addr),&(objmgmdGroupAddressValue.addr),sizeof(objmgmdGroupAddressValue.addr))));
   
    if(owa.l7rc == L7_SUCCESS)
    {
      while((!memcmp(&(nextObjmgmdGroupAddressValue.addr),&(objmgmdGroupAddressValue.addr),sizeof(objmgmdGroupAddressValue.addr)))
              &&(nextObjmgmdGroupIfIndexValue != objmgmdGroupIfIndexValue) 
              &&(owa.l7rc == L7_SUCCESS))
      {

         owa.l7rc = usmDbMgmdGrpIntfEntryNextGet (objmgmdGroupAddressTypeValue,
                                     &nextObjmgmdGroupAddressValue,
                                     &nextObjmgmdGroupIfIndexValue);
      }
      if( owa.l7rc == L7_SUCCESS)
      {
        if(!memcmp(&(nextObjmgmdGroupAddressValue.addr),&(objmgmdGroupAddressValue.addr),sizeof(objmgmdGroupAddressValue.addr)))
           owa.l7rc = usmDbMgmdGrpIntfEntryNextGet (objmgmdGroupAddressTypeValue,
                                       &nextObjmgmdGroupAddressValue,
                                       &nextObjmgmdGroupIfIndexValue);
  
        else
          owa.l7rc = L7_FAILURE;
      }

    }

  }
  if ((memcmp(&(nextObjmgmdGroupAddressValue.addr),&(objmgmdGroupAddressValue.addr),sizeof(objmgmdGroupAddressValue.addr)))
      || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjmgmdGroupIfIndexValue, owa.len);

  /* return the object value: mgmdGroupIfIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjmgmdGroupIfIndexValue,
                           sizeof (objmgmdGroupIfIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

xLibRC_t fpObjGet_ipmcastmgmdGroupTable_mgmdGroupUpTime (void *wap, void *bufp)
{

  fpObjWa_t kwamgmdGroupAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdGroupAddressTypeValue;
  fpObjWa_t kwamgmdGroupAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keymgmdGroupAddressValue;
  fpObjWa_t kwamgmdGroupIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdGroupIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objmgmdGroupUpTimeValue;
  L7_uint32 val;
  L7_uint32 sfMode = 0, compatMode = 0;
  L7_BOOL ISv3 = L7_FALSE;
  usmDbTimeSpec_t timeSpec;
  FPOBJ_TRACE_ENTER (bufp);

  memset(objmgmdGroupUpTimeValue, 0x00, sizeof(objmgmdGroupUpTimeValue));
  /* retrieve key: mgmdGroupAddressType */
  kwamgmdGroupAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdGroupTable_mgmdGroupAddressType,
                   (xLibU8_t *) & keymgmdGroupAddressTypeValue,
                   &kwamgmdGroupAddressType.len);
  if (kwamgmdGroupAddressType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdGroupAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdGroupAddressType);
    return kwamgmdGroupAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdGroupAddressTypeValue,
                           kwamgmdGroupAddressType.len);

  /* retrieve key: mgmdGroupAddress */
  kwamgmdGroupAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdGroupTable_mgmdGroupAddress,
                   (xLibU8_t *)&keymgmdGroupAddressValue, &kwamgmdGroupAddress.len);
  if (kwamgmdGroupAddress.rc != XLIBRC_SUCCESS)
  {
    kwamgmdGroupAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdGroupAddress);
    return kwamgmdGroupAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdGroupAddressValue, kwamgmdGroupAddress.len);

  /* retrieve key: mgmdGroupIfIndex */
  kwamgmdGroupIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdGroupTable_mgmdGroupIfIndex,
                   (xLibU8_t *) & keymgmdGroupIfIndexValue, &kwamgmdGroupIfIndex.len);
  if (kwamgmdGroupIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdGroupIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdGroupIfIndex);
    return kwamgmdGroupIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdGroupIfIndexValue, kwamgmdGroupIfIndex.len);

  /* get the value from application */
  if((owa.l7rc=(usmDbMgmdCacheGroupCompatModeGet(L7_UNIT_CURRENT, keymgmdGroupAddressTypeValue, &keymgmdGroupAddressValue,
                         keymgmdGroupIfIndexValue, &compatMode)) == L7_SUCCESS) && (compatMode == 3))
  {
    ISv3 = L7_TRUE;
    if(usmDbMgmdCacheGroupSourceFilterModeGet (L7_UNIT_CURRENT, keymgmdGroupAddressTypeValue, &keymgmdGroupAddressValue,
                               keymgmdGroupIfIndexValue, &sfMode) != L7_SUCCESS)
    {
      sfMode = MGMD_FILTER_MODE_EXCLUDE;
    }
  }
  if((ISv3 == L7_TRUE) && (sfMode == 1))
  {
    strcpy( objmgmdGroupUpTimeValue, "-----");
  }
  else
  {
     owa.l7rc = usmDbMgmdCacheUpTimeGet (L7_UNIT_CURRENT, keymgmdGroupAddressTypeValue,
                             (L7_inet_addr_t *)&keymgmdGroupAddressValue,
                           keymgmdGroupIfIndexValue, &val);
     memset(&timeSpec, 0, sizeof(usmDbTimeSpec_t));
     osapiConvertRawUpTime(val,(L7_timespec *)&timeSpec);
     osapiSnprintf (objmgmdGroupUpTimeValue, sizeof (objmgmdGroupUpTimeValue), "%2.2d:%2.2d:%2.2d",timeSpec.hours,timeSpec.minutes,timeSpec.seconds );
  }
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: mgmdGroupUpTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objmgmdGroupUpTimeValue,
                           sizeof (objmgmdGroupUpTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

xLibRC_t fpObjGet_ipmcastmgmdGroupTable_mgmdGroupExpiryTime (void *wap, void *bufp)
{

  fpObjWa_t kwamgmdGroupAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdGroupAddressTypeValue;
  fpObjWa_t kwamgmdGroupAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keymgmdGroupAddressValue;
  fpObjWa_t kwamgmdGroupIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdGroupIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objmgmdGroupExpiryTimeValue;
  L7_uint32 val;
  L7_uint32 sfMode = 0, compatMode = 0;
  L7_BOOL ISv3 = L7_FALSE;
  usmDbTimeSpec_t timeSpec;

  FPOBJ_TRACE_ENTER (bufp);

  memset(objmgmdGroupExpiryTimeValue, 0x00, sizeof(objmgmdGroupExpiryTimeValue));
  /* retrieve key: mgmdGroupAddressType */
  kwamgmdGroupAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdGroupTable_mgmdGroupAddressType,
                   (xLibU8_t *) & keymgmdGroupAddressTypeValue,
                   &kwamgmdGroupAddressType.len);
  if (kwamgmdGroupAddressType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdGroupAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdGroupAddressType);
    return kwamgmdGroupAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdGroupAddressTypeValue,
                           kwamgmdGroupAddressType.len);

  /* retrieve key: mgmdGroupAddress */
  kwamgmdGroupAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdGroupTable_mgmdGroupAddress,
                   (xLibU8_t *) &keymgmdGroupAddressValue, &kwamgmdGroupAddress.len);
  if (kwamgmdGroupAddress.rc != XLIBRC_SUCCESS)
  {
    kwamgmdGroupAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdGroupAddress);
    return kwamgmdGroupAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdGroupAddressValue, kwamgmdGroupAddress.len);

  /* retrieve key: mgmdGroupIfIndex */
  kwamgmdGroupIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdGroupTable_mgmdGroupIfIndex,
                   (xLibU8_t *) & keymgmdGroupIfIndexValue, &kwamgmdGroupIfIndex.len);
  if (kwamgmdGroupIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdGroupIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdGroupIfIndex);
    return kwamgmdGroupIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdGroupIfIndexValue, kwamgmdGroupIfIndex.len);
  /* get the value from application */
  if((owa.l7rc=(usmDbMgmdCacheGroupCompatModeGet(L7_UNIT_CURRENT, keymgmdGroupAddressTypeValue, &keymgmdGroupAddressValue,
                         keymgmdGroupIfIndexValue, &compatMode)) == L7_SUCCESS) && (compatMode == 3))
  {
    ISv3 = L7_TRUE;
    if(usmDbMgmdCacheGroupSourceFilterModeGet (L7_UNIT_CURRENT, keymgmdGroupAddressTypeValue, &keymgmdGroupAddressValue,
                               keymgmdGroupIfIndexValue, &sfMode) != L7_SUCCESS)
    {
      sfMode = MGMD_FILTER_MODE_EXCLUDE;
    }
  }
  if((ISv3 == L7_TRUE) && (sfMode == 1))
  {
    strcpy( objmgmdGroupExpiryTimeValue, "-----");
  }
  else
  {
    owa.l7rc = usmDbMgmdCacheExpiryTimeGet (L7_UNIT_CURRENT, keymgmdGroupAddressTypeValue,
                              (L7_inet_addr_t *)&keymgmdGroupAddressValue,
                              keymgmdGroupIfIndexValue, &val);
    memset(&timeSpec, 0, sizeof(usmDbTimeSpec_t));
    osapiConvertRawUpTime(val,(L7_timespec *)&timeSpec);
    osapiSnprintf (objmgmdGroupExpiryTimeValue, sizeof (objmgmdGroupExpiryTimeValue), 
                           "%2.2d:%2.2d:%2.2d",timeSpec.hours,timeSpec.minutes,timeSpec.seconds );
  }
 
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: mgmdGroupExpiryTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objmgmdGroupExpiryTimeValue,
                           sizeof (objmgmdGroupExpiryTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}



xLibRC_t fpObjGet_ipmcastmgmdGroupTable_mgmdGroupSourceFilterMode (void *wap,
                                                                               void *bufp)
{

  fpObjWa_t kwamgmdGroupAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdGroupAddressTypeValue;
  fpObjWa_t kwamgmdGroupAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keymgmdGroupAddressValue;
  fpObjWa_t kwamgmdGroupIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdGroupIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdGroupSourceFilterModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdGroupAddressType */
  kwamgmdGroupAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdGroupTable_mgmdGroupAddressType,
                   (xLibU8_t *) & keymgmdGroupAddressTypeValue,
                   &kwamgmdGroupAddressType.len);
  if (kwamgmdGroupAddressType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdGroupAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdGroupAddressType);
    return kwamgmdGroupAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdGroupAddressTypeValue,
                           kwamgmdGroupAddressType.len);

  /* retrieve key: mgmdGroupAddress */
  kwamgmdGroupAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdGroupTable_mgmdGroupAddress,
                   (xLibU8_t *) &keymgmdGroupAddressValue, &kwamgmdGroupAddress.len);
  if (kwamgmdGroupAddress.rc != XLIBRC_SUCCESS)
  {
    kwamgmdGroupAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdGroupAddress);
    return kwamgmdGroupAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdGroupAddressValue, kwamgmdGroupAddress.len);

  /* retrieve key: mgmdGroupIfIndex */
  kwamgmdGroupIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdGroupTable_mgmdGroupIfIndex,
                   (xLibU8_t *) & keymgmdGroupIfIndexValue, &kwamgmdGroupIfIndex.len);
  if (kwamgmdGroupIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdGroupIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdGroupIfIndex);
    return kwamgmdGroupIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdGroupIfIndexValue, kwamgmdGroupIfIndex.len);

  /* get the value from application */
  owa.l7rc = usmDbMgmdCacheGroupSourceFilterModeGet (L7_UNIT_CURRENT, keymgmdGroupAddressTypeValue,
                              (L7_inet_addr_t *)&keymgmdGroupAddressValue,
                              keymgmdGroupIfIndexValue,
                              &objmgmdGroupSourceFilterModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: mgmdGroupSourceFilterMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmgmdGroupSourceFilterModeValue,
                           sizeof (objmgmdGroupSourceFilterModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


xLibRC_t fpObjGet_ipmcastmgmdGroupTable_mgmdGroupVersion1HostTimer (void *wap,
                                                                                void *bufp)
{

  fpObjWa_t kwamgmdGroupAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdGroupAddressTypeValue;
  fpObjWa_t kwamgmdGroupAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keymgmdGroupAddressValue;
  fpObjWa_t kwamgmdGroupIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdGroupIfIndexValue;

  L7_uint32 val;
  L7_uint32 compatMode = 0;
  usmDbTimeSpec_t timeSpec;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objmgmdGroupVersion1HostTimerValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdGroupAddressType */
  kwamgmdGroupAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdGroupTable_mgmdGroupAddressType,
                   (xLibU8_t *) & keymgmdGroupAddressTypeValue,
                   &kwamgmdGroupAddressType.len);
  if (kwamgmdGroupAddressType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdGroupAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdGroupAddressType);
    return kwamgmdGroupAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdGroupAddressTypeValue,
                           kwamgmdGroupAddressType.len);

  /* retrieve key: mgmdGroupAddress */
  kwamgmdGroupAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdGroupTable_mgmdGroupAddress,
                   (xLibU8_t *) &keymgmdGroupAddressValue, &kwamgmdGroupAddress.len);
  if (kwamgmdGroupAddress.rc != XLIBRC_SUCCESS)
  {
    kwamgmdGroupAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdGroupAddress);
    return kwamgmdGroupAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp,&keymgmdGroupAddressValue, kwamgmdGroupAddress.len);

  /* retrieve key: mgmdGroupIfIndex */
  kwamgmdGroupIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdGroupTable_mgmdGroupIfIndex,
                   (xLibU8_t *) & keymgmdGroupIfIndexValue, &kwamgmdGroupIfIndex.len);
  if (kwamgmdGroupIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdGroupIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdGroupIfIndex);
    return kwamgmdGroupIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdGroupIfIndexValue, kwamgmdGroupIfIndex.len);

  owa.l7rc =usmDbMgmdCacheGroupCompatModeGet(L7_UNIT_CURRENT, keymgmdGroupAddressTypeValue, 
                                                                              &keymgmdGroupAddressValue,keymgmdGroupIfIndexValue, 
                                                                              &compatMode);

  if (owa.l7rc == L7_SUCCESS)
  {
    if(compatMode == 1)
    {
      if(keymgmdGroupAddressTypeValue == L7_AF_INET )
      {
        if(usmDbMgmdCacheVersion1HostTimerGet (L7_UNIT_CURRENT, keymgmdGroupAddressTypeValue, 
                        &keymgmdGroupAddressValue, keymgmdGroupIfIndexValue, &val) == L7_SUCCESS)
        {
          memset(&timeSpec, 0, sizeof(usmDbTimeSpec_t));
          osapiConvertRawUpTime(val,(L7_timespec *)&timeSpec);
          osapiSnprintf (objmgmdGroupVersion1HostTimerValue, sizeof(objmgmdGroupVersion1HostTimerValue), 
                         "%2.2d:%2.2d:%2.2d",timeSpec.hours,timeSpec.minutes,timeSpec.seconds);
        }
        else
        {
          strcpy(objmgmdGroupVersion1HostTimerValue, "-----");   
        }
      }
      else
      {
        if(usmDbMgmdCacheVersion2HostTimerGet (L7_UNIT_CURRENT, keymgmdGroupAddressTypeValue,
                        &keymgmdGroupAddressValue, keymgmdGroupIfIndexValue, &val) == L7_SUCCESS)
        {
          memset(&timeSpec, 0, sizeof(usmDbTimeSpec_t));
          osapiConvertRawUpTime(val,(L7_timespec *)&timeSpec);
          osapiSnprintf (objmgmdGroupVersion1HostTimerValue, sizeof(objmgmdGroupVersion1HostTimerValue),
                         "%2.2d:%2.2d:%2.2d",timeSpec.hours,timeSpec.minutes,timeSpec.seconds);
        }
        else
        {
          strcpy(objmgmdGroupVersion1HostTimerValue, "-----");
        }
      } 
    }
    else
    {
      strcpy(objmgmdGroupVersion1HostTimerValue, "-----");   
    }
  }
  else
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: mgmdGroupVersion1HostTimer */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objmgmdGroupVersion1HostTimerValue,
                           sizeof (objmgmdGroupVersion1HostTimerValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}



xLibRC_t fpObjGet_ipmcastmgmdGroupTable_mgmdGroupGroupCompatMode (void *wap, void *bufp)
{

  fpObjWa_t kwamgmdGroupAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdGroupAddressTypeValue;
  fpObjWa_t kwamgmdGroupAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keymgmdGroupAddressValue;
  fpObjWa_t kwamgmdGroupIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdGroupIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objmgmdGroupGroupCompatModeValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdGroupAddressType */
  kwamgmdGroupAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdGroupTable_mgmdGroupAddressType,
                   (xLibU8_t *) & keymgmdGroupAddressTypeValue,
                   &kwamgmdGroupAddressType.len);
  if (kwamgmdGroupAddressType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdGroupAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdGroupAddressType);
    return kwamgmdGroupAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdGroupAddressTypeValue,
                           kwamgmdGroupAddressType.len);

  /* retrieve key: mgmdGroupAddress */
  kwamgmdGroupAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdGroupTable_mgmdGroupAddress,
                   (xLibU8_t *) &keymgmdGroupAddressValue, &kwamgmdGroupAddress.len);
  if (kwamgmdGroupAddress.rc != XLIBRC_SUCCESS)
  {
    kwamgmdGroupAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdGroupAddress);
    return kwamgmdGroupAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdGroupAddressValue, kwamgmdGroupAddress.len);

  /* retrieve key: mgmdGroupIfIndex */
  kwamgmdGroupIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdGroupTable_mgmdGroupIfIndex,
                   (xLibU8_t *) & keymgmdGroupIfIndexValue, &kwamgmdGroupIfIndex.len);
  if (kwamgmdGroupIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdGroupIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdGroupIfIndex);
    return kwamgmdGroupIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdGroupIfIndexValue, kwamgmdGroupIfIndex.len);
  /* get the value from application */
  owa.l7rc = usmDbMgmdCacheGroupCompatModeGet (L7_UNIT_CURRENT, keymgmdGroupAddressTypeValue,
                              (L7_inet_addr_t *)&keymgmdGroupAddressValue,
                              keymgmdGroupIfIndexValue,
                              &objmgmdGroupGroupCompatModeValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: mgmdGroupGroupCompatMode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objmgmdGroupGroupCompatModeValue,
                           sizeof (objmgmdGroupGroupCompatModeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


xLibRC_t fpObjGet_ipmcastmgmdGroupTable_mgmdGroupSrcListExpiryTime (void *wap,
                                                                                void *bufp)
{

  fpObjWa_t kwamgmdGroupAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdGroupAddressTypeValue;
  fpObjWa_t kwamgmdGroupAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keymgmdGroupAddressValue;
  fpObjWa_t kwamgmdGroupIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdGroupIfIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objmgmdGroupSrcListExpiryTimeValue;
  xLibStr256_t tempObjmgmdGroupSrcListExpiryTimeValue;
  L7_uint32 compatMode = 0;
  L7_BOOL ISv3 = L7_FALSE;
  L7_uint32 sfMode = 0;
  L7_inet_addr_t groupHostAddressValue;
  L7_uint32 val;
  L7_uint32  indexIntIface;
  usmDbTimeSpec_t timeSpec;
  xLibStr256_t buf, bigBuf;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdGroupAddressType */
  kwamgmdGroupAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdGroupTable_mgmdGroupAddressType,
                   (xLibU8_t *) & keymgmdGroupAddressTypeValue,
                   &kwamgmdGroupAddressType.len);
  if (kwamgmdGroupAddressType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdGroupAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdGroupAddressType);
    return kwamgmdGroupAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdGroupAddressTypeValue,
                           kwamgmdGroupAddressType.len);

  /* retrieve key: mgmdGroupAddress */
  kwamgmdGroupAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdGroupTable_mgmdGroupAddress,
                   (xLibU8_t *) &keymgmdGroupAddressValue, &kwamgmdGroupAddress.len);
  if (kwamgmdGroupAddress.rc != XLIBRC_SUCCESS)
  {
    kwamgmdGroupAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdGroupAddress);
    return kwamgmdGroupAddress.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdGroupAddressValue, kwamgmdGroupAddress.len);

  /* retrieve key: mgmdGroupIfIndex */
  kwamgmdGroupIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdGroupTable_mgmdGroupIfIndex,
                   (xLibU8_t *) & keymgmdGroupIfIndexValue,
                   &kwamgmdGroupIfIndex.len);
  if (kwamgmdGroupIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdGroupIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdGroupIfIndex);
    return kwamgmdGroupIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdGroupIfIndexValue,
                           kwamgmdGroupIfIndex.len);


  if((usmDbMgmdCacheGroupCompatModeGet(L7_UNIT_CURRENT, keymgmdGroupAddressTypeValue, &keymgmdGroupAddressValue,
                                     keymgmdGroupIfIndexValue, &compatMode) == L7_SUCCESS) && (compatMode == 3))
  {
    ISv3 = L7_TRUE;
    if(usmDbMgmdCacheGroupSourceFilterModeGet (L7_UNIT_CURRENT, keymgmdGroupAddressTypeValue, &keymgmdGroupAddressValue,
                                               keymgmdGroupIfIndexValue, &sfMode) != L7_SUCCESS)
    {
      sfMode = MGMD_FILTER_MODE_EXCLUDE;
    }
  }


  if(ISv3 == L7_TRUE)
  {
    L7_inet_addr_t tmpGroupIp;

    memset(objmgmdGroupSrcListExpiryTimeValue, 0, sizeof(objmgmdGroupSrcListExpiryTimeValue));
    memset(&tmpGroupIp, 0, sizeof(L7_inet_addr_t));
    memset(&groupHostAddressValue, 0, sizeof(L7_inet_addr_t));
    inetCopy (&tmpGroupIp, &keymgmdGroupAddressValue);
    indexIntIface = keymgmdGroupIfIndexValue;
    owa.l7rc = usmDbMgmdSrcListEntryNextGet(L7_UNIT_CURRENT, keymgmdGroupAddressTypeValue, &keymgmdGroupAddressValue, &indexIntIface, &groupHostAddressValue);
    if((owa.l7rc == L7_FAILURE) || (!L7_INET_IS_ADDR_EQUAL (&tmpGroupIp, &keymgmdGroupAddressValue)) || (indexIntIface != keymgmdGroupIfIndexValue))
    {
      owa.l7rc = L7_FAILURE;
      strcpy(objmgmdGroupSrcListExpiryTimeValue, "-----"); 
    }
    while(owa.l7rc == L7_SUCCESS)
    {

      if(usmDbMgmdCacheGroupSourceFilterModeGet(L7_UNIT_CURRENT, keymgmdGroupAddressTypeValue, &keymgmdGroupAddressValue,
                              keymgmdGroupIfIndexValue, &val) == L7_SUCCESS)
      {
        if(val == MGMD_FILTER_MODE_INCLUDE)
        {
           if(usmDbMgmdSrcExpiryTimeGet(L7_UNIT_CURRENT, keymgmdGroupAddressTypeValue, &keymgmdGroupAddressValue,
                               keymgmdGroupIfIndexValue, &groupHostAddressValue, &val) == L7_SUCCESS)
           {
             memset(&timeSpec, 0, sizeof(usmDbTimeSpec_t));
             osapiConvertRawUpTime(val,(L7_timespec *)&timeSpec);
             osapiSnprintf(tempObjmgmdGroupSrcListExpiryTimeValue, sizeof(tempObjmgmdGroupSrcListExpiryTimeValue),"%2.2d:%2.2d:%2.2d", timeSpec.hours,
                          timeSpec.minutes,timeSpec.seconds);
           }
           else
           {
             strcpy(tempObjmgmdGroupSrcListExpiryTimeValue, pStrErr_common_Err_Temp); 
           }
        }
        else if(val == MGMD_FILTER_MODE_EXCLUDE)
             {
               if(usmDbMgmdSrcExpiryTimeGet(L7_UNIT_CURRENT, keymgmdGroupAddressTypeValue, &keymgmdGroupAddressValue,
                                         keymgmdGroupIfIndexValue, &groupHostAddressValue, &val) == L7_SUCCESS)
               {
                 if(val == 0)
                 {
                   strcpy(tempObjmgmdGroupSrcListExpiryTimeValue, pStrInfo_common_Line_Temp); 
                 }
                 else
                 {
                   memset(&timeSpec, 0, sizeof(usmDbTimeSpec_t));
                   osapiConvertRawUpTime(val,(L7_timespec *)&timeSpec);
                   osapiSnprintf(tempObjmgmdGroupSrcListExpiryTimeValue, sizeof(tempObjmgmdGroupSrcListExpiryTimeValue),"%2.2d:%2.2d:%2.2d", timeSpec.hours,
                                 timeSpec.minutes,timeSpec.seconds);
                 }
               }
               else
               {
                 strcpy(tempObjmgmdGroupSrcListExpiryTimeValue, pStrErr_common_Err_Temp ); 
               }
             }
             inetAddrHtop(&groupHostAddressValue, buf);
             osapiSnprintf(bigBuf, sizeof(bigBuf), "%s(%s)", buf, tempObjmgmdGroupSrcListExpiryTimeValue); 
             osapiStrncat(objmgmdGroupSrcListExpiryTimeValue, bigBuf, (sizeof(objmgmdGroupSrcListExpiryTimeValue)-strlen(objmgmdGroupSrcListExpiryTimeValue))-1);
             
             owa.l7rc = usmDbMgmdSrcListEntryNextGet(L7_UNIT_CURRENT, keymgmdGroupAddressTypeValue, &keymgmdGroupAddressValue, &indexIntIface, &groupHostAddressValue);
             if((!L7_INET_IS_ADDR_EQUAL (&tmpGroupIp, &keymgmdGroupAddressValue)) || (indexIntIface != keymgmdGroupIfIndexValue))
             {
               owa.l7rc = L7_FAILURE;
             }
             if( owa.l7rc == L7_SUCCESS)
             {
               osapiStrncat(objmgmdGroupSrcListExpiryTimeValue, "\n", (sizeof(objmgmdGroupSrcListExpiryTimeValue)-strlen(objmgmdGroupSrcListExpiryTimeValue))-1); 
             }
      }
    } 
  }
  else
  {
    strcpy(objmgmdGroupSrcListExpiryTimeValue, "-----");
  }

  /* return the object value: mgmdGroupExpiryTime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objmgmdGroupSrcListExpiryTimeValue,
                           sizeof (objmgmdGroupSrcListExpiryTimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

xLibRC_t fpObjGet_ipmcastmgmdGroupTable_mgmdGroupLastReporter (void *wap, void *bufp)
{

  fpObjWa_t kwamgmdGroupAddressType = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdGroupAddressTypeValue;
  fpObjWa_t kwamgmdGroupAddress = FPOBJ_INIT_WA (sizeof (L7_inet_addr_t));
  L7_inet_addr_t keymgmdGroupAddressValue;
  fpObjWa_t kwamgmdGroupIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keymgmdGroupIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objmgmdGroupLastReporterValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: mgmdGroupAddressType */
  kwamgmdGroupAddressType.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdGroupTable_mgmdGroupAddressType,
                   (xLibU8_t *) & keymgmdGroupAddressTypeValue,
                   &kwamgmdGroupAddressType.len);
  if (kwamgmdGroupAddressType.rc != XLIBRC_SUCCESS)
  {
    kwamgmdGroupAddressType.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdGroupAddressType);
    return kwamgmdGroupAddressType.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdGroupAddressTypeValue,
                           kwamgmdGroupAddressType.len);

  /* retrieve key: mgmdGroupAddress */
  kwamgmdGroupAddress.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdGroupTable_mgmdGroupAddress,
                   (xLibU8_t *) &keymgmdGroupAddressValue, &kwamgmdGroupAddress.len);
  if (kwamgmdGroupAddress.rc != XLIBRC_SUCCESS)
  {
    kwamgmdGroupAddress.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdGroupAddress);
    return kwamgmdGroupAddress.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdGroupAddressValue, kwamgmdGroupAddress.len);

  /* retrieve key: mgmdGroupIfIndex */
  kwamgmdGroupIfIndex.rc =
    xLibFilterGet (wap, XOBJ_ipmcastmgmdGroupTable_mgmdGroupIfIndex,
                   (xLibU8_t *) & keymgmdGroupIfIndexValue, &kwamgmdGroupIfIndex.len);
  if (kwamgmdGroupIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwamgmdGroupIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwamgmdGroupIfIndex);
    return kwamgmdGroupIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keymgmdGroupIfIndexValue, kwamgmdGroupIfIndex.len);
  /* get the value from application */
  owa.l7rc = usmDbMgmdCacheLastReporterGet (L7_UNIT_CURRENT, keymgmdGroupAddressTypeValue,
                              (L7_inet_addr_t *)&keymgmdGroupAddressValue,
                              keymgmdGroupIfIndexValue, (L7_inet_addr_t *)objmgmdGroupLastReporterValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: mgmdGroupLastReporter */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objmgmdGroupLastReporterValue,
                           sizeof (objmgmdGroupLastReporterValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

