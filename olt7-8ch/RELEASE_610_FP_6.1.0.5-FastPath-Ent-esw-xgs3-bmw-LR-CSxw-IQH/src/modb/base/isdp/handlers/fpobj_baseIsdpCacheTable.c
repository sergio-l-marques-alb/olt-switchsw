
/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
********************************************************************************
*
* @filename fpobj_baseIsdpCacheTable.c
*
* @purpose  
*
* @component object handlers
*
* @comments  Refer to base-object.xml
*
* @create  05 June 2008, Thursday
*
* @notes   This file is auto generated and should be used as starting point to
*          develop the object handlers
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/

#include "fpobj_util.h"
#include "_xe_baseIsdpCacheTable_obj.h"
#include "usmdb_isdp_api.h"
#include "osapi_support.h"

/*******************************************************************************
* @function fpObjGet_baseIsdpCacheTable_IsdpCacheIfIndex
*
* @purpose Get 'IsdpCacheIfIndex'
 *@description  [IsdpCacheIfIndex] The ifIndex value of the local interface.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseIsdpCacheTable_IsdpCacheIfIndex (void *wap, void *bufp)
{

  xLibU32_t objIsdpCacheIfIndexValue;
  xLibU32_t nextObjIsdpCacheIfIndexValue;
  xLibStr256_t objIsdpCacheDeviceId;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IsdpCacheIfIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_baseIsdpCacheTable_IsdpCacheIfIndex,
                          (xLibU8_t *) & objIsdpCacheIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {

    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    objIsdpCacheIfIndexValue = 0;
    memset(objIsdpCacheDeviceId, 0x00, sizeof(objIsdpCacheDeviceId)); 

    owa.l7rc = usmdbIsdpNeighborGetNext (objIsdpCacheIfIndexValue, objIsdpCacheDeviceId, 
                                               &nextObjIsdpCacheIfIndexValue, objIsdpCacheDeviceId);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objIsdpCacheIfIndexValue, owa.len);
    memset (&objIsdpCacheDeviceId, 0, sizeof (objIsdpCacheDeviceId));
    do
    {

      owa.l7rc = usmdbIsdpNeighborGetNext (objIsdpCacheIfIndexValue, objIsdpCacheDeviceId, 
                                                   &nextObjIsdpCacheIfIndexValue, objIsdpCacheDeviceId);                                                   
                       
    }
    while ((objIsdpCacheIfIndexValue == nextObjIsdpCacheIfIndexValue) && (owa.l7rc == L7_SUCCESS));
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjIsdpCacheIfIndexValue, owa.len);
  /* return the object value: IsdpCacheIfIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjIsdpCacheIfIndexValue,
                           sizeof (nextObjIsdpCacheIfIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseIsdpCacheTable_IsdpCacheIndex
*
* @purpose Get 'IsdpCacheIndex'
 *@description  [IsdpCacheIndex] Index value is unique integer id of each
* neighbor entry.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseIsdpCacheTable_IsdpCacheIndex (void *wap, void *bufp)
{

  xLibU32_t objIsdpCacheIfIndexValue;
  xLibU32_t nextObjIsdpCacheIfIndexValue;
  xLibU32_t objIsdpCacheIndexValue;
  xLibU32_t nextObjIsdpCacheIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibS8_t objIsdpGlobalDeviceIdValue[L7_ISDP_DEVICE_ID_LEN];
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IsdpCacheIfIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_baseIsdpCacheTable_IsdpCacheIfIndex,
                          (xLibU8_t *) & objIsdpCacheIfIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &objIsdpCacheIfIndexValue, owa.len);

  /* retrieve key: IsdpCacheIndex */
  owa.rc = xLibFilterGet (wap, XOBJ_baseIsdpCacheTable_IsdpCacheIndex,
                          (xLibU8_t *) & objIsdpCacheIndexValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmdbIsdpNeighborIndexedGetNext (objIsdpCacheIfIndexValue, objIsdpCacheIndexValue,
                                                                          &nextObjIsdpCacheIfIndexValue,&nextObjIsdpCacheIndexValue,
                                                                          objIsdpGlobalDeviceIdValue);
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objIsdpCacheIndexValue, owa.len);

    owa.l7rc = usmdbIsdpNeighborIndexedGetNext (objIsdpCacheIfIndexValue, objIsdpCacheIndexValue, 
                                                                          &nextObjIsdpCacheIfIndexValue,&nextObjIsdpCacheIndexValue,
                                                                          objIsdpGlobalDeviceIdValue);
  }
  
  if ((objIsdpCacheIfIndexValue != nextObjIsdpCacheIfIndexValue) || (owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjIsdpCacheIndexValue, owa.len);
  /* return the object value: IsdpCacheIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjIsdpCacheIndexValue,
                           sizeof (nextObjIsdpCacheIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseIsdpCacheTable_IsdpCacheAddress
*
* @purpose Get 'IsdpCacheAddress'
 *@description  [IsdpCacheAddress] The (first) network-layer address of the
* device's SNMP-agent as reported in the Address TLV of the most
* recently received ISDP message.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseIsdpCacheTable_IsdpCacheAddress (void *wap, void *bufp)
{

  fpObjWa_t kwaIsdpCacheIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIsdpCacheIfIndexValue;
  fpObjWa_t kwaIsdpCacheDeviceId = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyIsdpCacheDeviceIdValue;

  xLibU8_t success = L7_FALSE;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objIsdpCacheAddressValue;
  xLibStr256_t isdpCacheAddressStr;

  xLibU32_t ipAddress;
  xLibU32_t ipAddressIndex = 0;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IsdpCacheIfIndex */
  kwaIsdpCacheIfIndex.rc = xLibFilterGet (wap, XOBJ_baseIsdpCacheTable_IsdpCacheIfIndex,
                                          (xLibU8_t *) & keyIsdpCacheIfIndexValue,
                                          &kwaIsdpCacheIfIndex.len);
  if (kwaIsdpCacheIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwaIsdpCacheIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaIsdpCacheIfIndex);
    return kwaIsdpCacheIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIsdpCacheIfIndexValue, kwaIsdpCacheIfIndex.len);

  memset(keyIsdpCacheDeviceIdValue, 0x0, sizeof(keyIsdpCacheDeviceIdValue));

  /* retrieve key: IsdpCacheDeviceId */
  kwaIsdpCacheDeviceId.rc = xLibFilterGet (wap, XOBJ_baseIsdpCacheTable_IsdpCacheDeviceId,
                                        (xLibU8_t *) & keyIsdpCacheDeviceIdValue,
                                        &kwaIsdpCacheDeviceId.len);
  if (kwaIsdpCacheDeviceId.rc != XLIBRC_SUCCESS)
  {
    kwaIsdpCacheDeviceId.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaIsdpCacheDeviceId);
    return kwaIsdpCacheDeviceId.rc;
  }
 
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIsdpCacheDeviceIdValue, kwaIsdpCacheDeviceId.len);
 
  memset(isdpCacheAddressStr, 0x0, sizeof(isdpCacheAddressStr));

  owa.l7rc = usmdbIsdpIntfNeighborAddressGetNext(keyIsdpCacheIfIndexValue, 
                                                 keyIsdpCacheDeviceIdValue, 0,
                                                 &ipAddressIndex);
/* while(owa.l7rc == L7_SUCCESS)  */
  if(owa.l7rc == L7_SUCCESS) 
  {
    if((owa.l7rc = usmdbIsdpIntfNeighborIpAddressGet(keyIsdpCacheIfIndexValue, 
                                                     keyIsdpCacheDeviceIdValue, 
                                                     ipAddressIndex, &ipAddress)) == L7_SUCCESS)
    { 
      osapiInetNtop(L7_AF_INET, (L7_uchar8*)&ipAddress, objIsdpCacheAddressValue, sizeof (xLibStr256_t));
#if 0 
      if (success != L7_FALSE)
      {
        /* If success is true means not first time so put comma */
        strncpy(&isdpCacheAddressStr[strlen(isdpCacheAddressStr)], ", ", strlen(isdpCacheAddressStr) - strlen(", ")); 
      }
      strncpy(&isdpCacheAddressStr[strlen(isdpCacheAddressStr)], 
               objIsdpCacheAddressValue, 
               (strlen(isdpCacheAddressStr) - strlen(objIsdpCacheAddressValue))); 
#endif
      success = L7_TRUE; /* At least once successful */
    }

/*    owa.l7rc = usmdbIsdpIntfNeighborAddressGetNext(keyIsdpCacheIfIndexValue, 
                                                   keyIsdpCacheDeviceIdValue, 
                                                   ipAddressIndex, &ipAddressIndex);
*/
  }
  
  if (success == L7_FALSE)
  { /* Not even a single entrty is there */
    owa.rc = XLIBRC_FAILURE;   
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: IsdpCacheAddress */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objIsdpCacheAddressValue,
                           strlen (objIsdpCacheAddressValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseIsdpCacheTable_IsdpCacheLocalIntf
*
* @purpose Get 'IsdpCacheLocalIntf'
 *@description  [IsdpCacheLocalIntf] The device Interface which had this
* neighbor.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseIsdpCacheTable_IsdpCacheLocalIntf (void *wap, void *bufp)
{

  fpObjWa_t kwaIsdpCacheIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIsdpCacheIfIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objIsdpCacheLocalIntfValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IsdpCacheIfIndex */
  kwaIsdpCacheIfIndex.rc = xLibFilterGet (wap, XOBJ_baseIsdpCacheTable_IsdpCacheIfIndex,
                                          (xLibU8_t *) & keyIsdpCacheIfIndexValue,
                                          &kwaIsdpCacheIfIndex.len);
  if (kwaIsdpCacheIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwaIsdpCacheIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaIsdpCacheIfIndex);
    return kwaIsdpCacheIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIsdpCacheIfIndexValue, kwaIsdpCacheIfIndex.len);

  fpObjWa_t kwaIsdpCacheDeviceId = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyIsdpCacheDeviceIdValue;

  /* retrieve key: IsdpCacheDeviceId */
  kwaIsdpCacheDeviceId.rc = xLibFilterGet (wap, XOBJ_baseIsdpCacheTable_IsdpCacheDeviceId,
                                        (xLibU8_t *) & keyIsdpCacheDeviceIdValue,
                                        &kwaIsdpCacheDeviceId.len);
  if (kwaIsdpCacheDeviceId.rc != XLIBRC_SUCCESS)
  {
    kwaIsdpCacheDeviceId.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaIsdpCacheDeviceId);
    return kwaIsdpCacheDeviceId.rc;
  }
 
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIsdpCacheDeviceIdValue, kwaIsdpCacheDeviceId.len);

  /* get the value from application */
  memset(objIsdpCacheLocalIntfValue, 0, sizeof(xLibStr256_t));
  owa.l7rc = L7_SUCCESS;
  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: IsdpCacheLocalIntf */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objIsdpCacheLocalIntfValue,
                           strlen (objIsdpCacheLocalIntfValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseIsdpCacheTable_IsdpCacheVersion
*
* @purpose Get 'IsdpCacheVersion'
 *@description  [IsdpCacheVersion] The Version string as reported in the most
* recent ISDP message.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseIsdpCacheTable_IsdpCacheVersion (void *wap, void *bufp)
{

  fpObjWa_t kwaIsdpCacheIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIsdpCacheIfIndexValue;
  fpObjWa_t kwaIsdpCacheDeviceId = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyIsdpCacheDeviceIdValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibS8_t objIsdpCacheVersionValue[L7_ISDP_VERSION_LEN];

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IsdpCacheIfIndex */
  kwaIsdpCacheIfIndex.rc = xLibFilterGet (wap, XOBJ_baseIsdpCacheTable_IsdpCacheIfIndex,
                                          (xLibU8_t *) & keyIsdpCacheIfIndexValue,
                                          &kwaIsdpCacheIfIndex.len);
  if (kwaIsdpCacheIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwaIsdpCacheIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaIsdpCacheIfIndex);
    return kwaIsdpCacheIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIsdpCacheIfIndexValue, kwaIsdpCacheIfIndex.len);

  memset(keyIsdpCacheDeviceIdValue, 0x0, sizeof(keyIsdpCacheDeviceIdValue));

  /* retrieve key: IsdpCacheDeviceId */
  kwaIsdpCacheDeviceId.rc = xLibFilterGet (wap, XOBJ_baseIsdpCacheTable_IsdpCacheDeviceId,
                                        (xLibU8_t *) & keyIsdpCacheDeviceIdValue,
                                        &kwaIsdpCacheDeviceId.len);
  if (kwaIsdpCacheDeviceId.rc != XLIBRC_SUCCESS)
  {
    kwaIsdpCacheDeviceId.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaIsdpCacheDeviceId);
    return kwaIsdpCacheDeviceId.rc;
  }
 
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIsdpCacheDeviceIdValue, kwaIsdpCacheDeviceId.len);

  owa.l7rc = usmdbIsdpIntfNeighborVersionGet(keyIsdpCacheIfIndexValue, 
                                     keyIsdpCacheDeviceIdValue, objIsdpCacheVersionValue);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;  
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: IsdpCacheVersion */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objIsdpCacheVersionValue,
                           strlen(objIsdpCacheVersionValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseIsdpCacheTable_IsdpCacheDeviceId
*
* @purpose Get 'IsdpCacheDeviceId'
 *@description  [IsdpCacheDeviceId] The Device-ID string as reported in the most
* recent ISDP message.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseIsdpCacheTable_IsdpCacheDeviceId (void *wap, void *bufp)
{

  fpObjWa_t kwaIsdpCacheIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIsdpCacheIfIndexValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objIsdpCacheDeviceIdValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IsdpCacheIfIndex */
  kwaIsdpCacheIfIndex.rc = xLibFilterGet (wap, XOBJ_baseIsdpCacheTable_IsdpCacheIfIndex,
                                          (xLibU8_t *) & keyIsdpCacheIfIndexValue,
                                          &kwaIsdpCacheIfIndex.len);
  if (kwaIsdpCacheIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwaIsdpCacheIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaIsdpCacheIfIndex);
    return kwaIsdpCacheIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIsdpCacheIfIndexValue, kwaIsdpCacheIfIndex.len);

  /* retrieve key: IsdpCacheDeviceId */
  owa.rc = xLibFilterGet (wap, XOBJ_baseIsdpCacheTable_IsdpCacheDeviceId,
                                        (xLibU8_t *) & objIsdpCacheDeviceIdValue,
                                        &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    memset(objIsdpCacheDeviceIdValue, 0x00, sizeof(objIsdpCacheDeviceIdValue));
    keyIsdpCacheIfIndexValue = 0;
    /* get the value from application */
    owa.l7rc = usmdbIsdpNeighborGetNext(keyIsdpCacheIfIndexValue, objIsdpCacheDeviceIdValue,
                                             &keyIsdpCacheIfIndexValue, objIsdpCacheDeviceIdValue);
  }
  else 
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objIsdpCacheDeviceIdValue, owa.len);
    owa.l7rc = usmdbIsdpNeighborGetNext(keyIsdpCacheIfIndexValue, objIsdpCacheDeviceIdValue,
                                             &keyIsdpCacheIfIndexValue, objIsdpCacheDeviceIdValue);
  }

  if ((owa.l7rc != L7_SUCCESS))
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  /* return the object value: IsdpCacheDeviceId */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objIsdpCacheDeviceIdValue,
                           strlen (objIsdpCacheDeviceIdValue));

  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseIsdpCacheTable_IsdpCacheDevicePort
*
* @purpose Get 'IsdpCacheDevicePort'
 *@description  [IsdpCacheDevicePort] The Port-ID string as reported in the most
* recent ISDP message.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseIsdpCacheTable_IsdpCacheDevicePort (void *wap, void *bufp)
{

  fpObjWa_t kwaIsdpCacheIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIsdpCacheIfIndexValue;
  fpObjWa_t kwaIsdpCacheDeviceId = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyIsdpCacheDeviceIdValue;


  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objIsdpCacheDevicePortValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IsdpCacheIfIndex */
  kwaIsdpCacheIfIndex.rc = xLibFilterGet (wap, XOBJ_baseIsdpCacheTable_IsdpCacheIfIndex,
                                          (xLibU8_t *) & keyIsdpCacheIfIndexValue,
                                          &kwaIsdpCacheIfIndex.len);
  if (kwaIsdpCacheIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwaIsdpCacheIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaIsdpCacheIfIndex);
    return kwaIsdpCacheIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIsdpCacheIfIndexValue, kwaIsdpCacheIfIndex.len);

  memset(keyIsdpCacheDeviceIdValue, 0x0, sizeof(keyIsdpCacheDeviceIdValue));

  /* retrieve key: IsdpCacheDeviceId */
  kwaIsdpCacheDeviceId.rc = xLibFilterGet (wap, XOBJ_baseIsdpCacheTable_IsdpCacheDeviceId,
                                        (xLibU8_t *) & keyIsdpCacheDeviceIdValue,
                                        &kwaIsdpCacheDeviceId.len);
  if (kwaIsdpCacheDeviceId.rc != XLIBRC_SUCCESS)
  {
    kwaIsdpCacheDeviceId.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaIsdpCacheDeviceId);
    return kwaIsdpCacheDeviceId.rc;
  }
 
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIsdpCacheDeviceIdValue, kwaIsdpCacheDeviceId.len);

  owa.l7rc = usmdbIsdpIntfNeighborPortIdGet(keyIsdpCacheIfIndexValue, keyIsdpCacheDeviceIdValue, objIsdpCacheDevicePortValue);
  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: IsdpCacheDevicePort */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objIsdpCacheDevicePortValue,
                           strlen (objIsdpCacheDevicePortValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseIsdpCacheTable_IsdpCachePlatform
*
* @purpose Get 'IsdpCachePlatform'
 *@description  [IsdpCachePlatform] The Device's Hardware Platform as reported
* in the most recent ISDP message.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseIsdpCacheTable_IsdpCachePlatform (void *wap, void *bufp)
{

  fpObjWa_t kwaIsdpCacheIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIsdpCacheIfIndexValue;
  fpObjWa_t kwaIsdpCacheDeviceId = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyIsdpCacheDeviceIdValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibS8_t objIsdpCachePlatformValue[L7_ISDP_PLATFORM_LEN];

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IsdpCacheIfIndex */
  kwaIsdpCacheIfIndex.rc = xLibFilterGet (wap, XOBJ_baseIsdpCacheTable_IsdpCacheIfIndex,
                                          (xLibU8_t *) & keyIsdpCacheIfIndexValue,
                                          &kwaIsdpCacheIfIndex.len);
  if (kwaIsdpCacheIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwaIsdpCacheIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaIsdpCacheIfIndex);
    return kwaIsdpCacheIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIsdpCacheIfIndexValue, kwaIsdpCacheIfIndex.len);

  memset(keyIsdpCacheDeviceIdValue, 0x0, sizeof(keyIsdpCacheDeviceIdValue));

  /* retrieve key: IsdpCacheDeviceId */
  kwaIsdpCacheDeviceId.rc = xLibFilterGet (wap, XOBJ_baseIsdpCacheTable_IsdpCacheDeviceId,
                                        (xLibU8_t *) & keyIsdpCacheDeviceIdValue,
                                        &kwaIsdpCacheDeviceId.len);
  if (kwaIsdpCacheDeviceId.rc != XLIBRC_SUCCESS)
  {
    kwaIsdpCacheDeviceId.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaIsdpCacheDeviceId);
    return kwaIsdpCacheDeviceId.rc;
  }
 
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIsdpCacheDeviceIdValue, kwaIsdpCacheDeviceId.len);
  
  owa.l7rc = usmdbIsdpIntfNeighborPlatformGet(keyIsdpCacheIfIndexValue, keyIsdpCacheDeviceIdValue, objIsdpCachePlatformValue);
  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;   
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: IsdpCachePlatform */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objIsdpCachePlatformValue,
                           strlen(objIsdpCachePlatformValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseIsdpCacheTable_IsdpCacheCapabilities
*
* @purpose Get 'IsdpCacheCapabilities'
 *@description  [IsdpCacheCapabilities] The Device's Functional Capabilities as
* reported in the most recent ISDP message.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseIsdpCacheTable_IsdpCacheCapabilities (void *wap, void *bufp)
{

  fpObjWa_t kwaIsdpCacheIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIsdpCacheIfIndexValue;
  fpObjWa_t kwaIsdpCacheDeviceId = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyIsdpCacheDeviceIdValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objIsdpCacheCapabilitiesValue;

  xLibU32_t IsdpCacheCapabilities;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IsdpCacheIfIndex */
  kwaIsdpCacheIfIndex.rc = xLibFilterGet (wap, XOBJ_baseIsdpCacheTable_IsdpCacheIfIndex,
                                          (xLibU8_t *) & keyIsdpCacheIfIndexValue,
                                          &kwaIsdpCacheIfIndex.len);
  if (kwaIsdpCacheIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwaIsdpCacheIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaIsdpCacheIfIndex);
    return kwaIsdpCacheIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIsdpCacheIfIndexValue, kwaIsdpCacheIfIndex.len);

  memset(keyIsdpCacheDeviceIdValue, 0x0, sizeof(keyIsdpCacheDeviceIdValue));

  /* retrieve key: IsdpCacheDeviceId */
  kwaIsdpCacheDeviceId.rc = xLibFilterGet (wap, XOBJ_baseIsdpCacheTable_IsdpCacheDeviceId,
                                        (xLibU8_t *) & keyIsdpCacheDeviceIdValue,
                                        &kwaIsdpCacheDeviceId.len);
  if (kwaIsdpCacheDeviceId.rc != XLIBRC_SUCCESS)
  {
    kwaIsdpCacheDeviceId.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaIsdpCacheDeviceId);
    return kwaIsdpCacheDeviceId.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIsdpCacheDeviceIdValue, kwaIsdpCacheDeviceId.len);

  if((owa.l7rc = usmdbIsdpIntfNeighborCapabilitiesGet(keyIsdpCacheIfIndexValue, 
                         keyIsdpCacheDeviceIdValue, &IsdpCacheCapabilities)) == L7_SUCCESS)
  {
    owa.l7rc = usmdbIsdpCapabilityStringGet(IsdpCacheCapabilities, objIsdpCacheCapabilitiesValue, sizeof (xLibStr256_t));
  }
  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: IsdpCacheCapabilities */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objIsdpCacheCapabilitiesValue,
                           strlen (objIsdpCacheCapabilitiesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseIsdpCacheTable_IsdpCacheLastChange
*
* @purpose Get 'IsdpCacheLastChange'
 *@description  [IsdpCacheLastChange] Indicates the time when this cache entry
* was last changed.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseIsdpCacheTable_IsdpCacheLastChange (void *wap, void *bufp)
{

  fpObjWa_t kwaIsdpCacheIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIsdpCacheIfIndexValue;
  fpObjWa_t kwaIsdpCacheDeviceId = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyIsdpCacheDeviceIdValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objIsdpCacheLastChangeValue;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IsdpCacheIfIndex */
  kwaIsdpCacheIfIndex.rc = xLibFilterGet (wap, XOBJ_baseIsdpCacheTable_IsdpCacheIfIndex,
                                          (xLibU8_t *) & keyIsdpCacheIfIndexValue,
                                          &kwaIsdpCacheIfIndex.len);
  if (kwaIsdpCacheIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwaIsdpCacheIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaIsdpCacheIfIndex);
    return kwaIsdpCacheIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIsdpCacheIfIndexValue, kwaIsdpCacheIfIndex.len);

  memset(keyIsdpCacheDeviceIdValue, 0x0, sizeof(keyIsdpCacheDeviceIdValue));

  /* retrieve key: IsdpCacheDeviceId */
  kwaIsdpCacheDeviceId.rc = xLibFilterGet (wap, XOBJ_baseIsdpCacheTable_IsdpCacheDeviceId,
                                        (xLibU8_t *) & keyIsdpCacheDeviceIdValue,
                                        &kwaIsdpCacheDeviceId.len);
  if (kwaIsdpCacheDeviceId.rc != XLIBRC_SUCCESS)
  {
    kwaIsdpCacheDeviceId.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaIsdpCacheDeviceId);
    return kwaIsdpCacheDeviceId.rc;
  }
 
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIsdpCacheDeviceIdValue, kwaIsdpCacheDeviceId.len);

  owa.l7rc = usmdbIsdpIntfNeighborLastChangeTimeGet(keyIsdpCacheIfIndexValue, 
                      keyIsdpCacheDeviceIdValue, &objIsdpCacheLastChangeValue);
  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: IsdpCacheLastChange */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objIsdpCacheLastChangeValue,
                           sizeof (objIsdpCacheLastChangeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseIsdpCacheTable_IsdpCacheProtocolVersion
*
* @purpose Get 'IsdpCacheProtocolVersion'
 *@description  [IsdpCacheProtocolVersion] The Device's version of ISDP protcol
* as reported in the most recent ISDP message.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseIsdpCacheTable_IsdpCacheProtocolVersion (void *wap, void *bufp)
{

  fpObjWa_t kwaIsdpCacheIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIsdpCacheIfIndexValue;
  fpObjWa_t kwaIsdpCacheDeviceId = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyIsdpCacheDeviceIdValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibU8_t objIsdpCacheProtocolVersionValue = 0;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IsdpCacheIfIndex */
  kwaIsdpCacheIfIndex.rc = xLibFilterGet (wap, XOBJ_baseIsdpCacheTable_IsdpCacheIfIndex,
                                          (xLibU8_t *) & keyIsdpCacheIfIndexValue,
                                          &kwaIsdpCacheIfIndex.len);
  if (kwaIsdpCacheIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwaIsdpCacheIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaIsdpCacheIfIndex);
    return kwaIsdpCacheIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIsdpCacheIfIndexValue, kwaIsdpCacheIfIndex.len);

  memset(keyIsdpCacheDeviceIdValue, 0x0, sizeof(keyIsdpCacheDeviceIdValue));

  /* retrieve key: IsdpCacheDeviceId */
  kwaIsdpCacheDeviceId.rc = xLibFilterGet (wap, XOBJ_baseIsdpCacheTable_IsdpCacheDeviceId,
                                        (xLibU8_t *) & keyIsdpCacheDeviceIdValue,
                                        &kwaIsdpCacheDeviceId.len);
  if (kwaIsdpCacheDeviceId.rc != XLIBRC_SUCCESS)
  {
    kwaIsdpCacheDeviceId.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaIsdpCacheDeviceId);
    return kwaIsdpCacheDeviceId.rc;
  }
 
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIsdpCacheDeviceIdValue, kwaIsdpCacheDeviceId.len);

  owa.l7rc = usmdbIsdpIntfNeighborProtocolVersionGet(keyIsdpCacheIfIndexValue, 
                              keyIsdpCacheDeviceIdValue, &objIsdpCacheProtocolVersionValue);
  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: IsdpCacheProtocolVersion */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objIsdpCacheProtocolVersionValue,
                           sizeof(objIsdpCacheProtocolVersionValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}

/*******************************************************************************
* @function fpObjGet_baseIsdpCacheTable_IsdpCacheHoldtime
*
* @purpose Get 'IsdpCacheHoldtime'
 *@description  [IsdpCacheHoldtime] The Device's ISDP Holdtime as reported in
* the most recent ISDP message.   
* @notes       
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseIsdpCacheTable_IsdpCacheHoldtime (void *wap, void *bufp)
{

  fpObjWa_t kwaIsdpCacheIfIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyIsdpCacheIfIndexValue;
  fpObjWa_t kwaIsdpCacheDeviceId = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t keyIsdpCacheDeviceIdValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU8_t));
  xLibU8_t objIsdpCacheHoldtimeValue = 0;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: IsdpCacheIfIndex */
  kwaIsdpCacheIfIndex.rc = xLibFilterGet (wap, XOBJ_baseIsdpCacheTable_IsdpCacheIfIndex,
                                          (xLibU8_t *) & keyIsdpCacheIfIndexValue,
                                          &kwaIsdpCacheIfIndex.len);
  if (kwaIsdpCacheIfIndex.rc != XLIBRC_SUCCESS)
  {
    kwaIsdpCacheIfIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaIsdpCacheIfIndex);
    return kwaIsdpCacheIfIndex.rc;
  }

  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIsdpCacheIfIndexValue, kwaIsdpCacheIfIndex.len);
 
  memset(keyIsdpCacheDeviceIdValue, 0x0, sizeof(keyIsdpCacheDeviceIdValue));

  /* retrieve key: IsdpCacheDeviceId */
  kwaIsdpCacheDeviceId.rc = xLibFilterGet (wap, XOBJ_baseIsdpCacheTable_IsdpCacheDeviceId,
                                        (xLibU8_t *) & keyIsdpCacheDeviceIdValue,
                                        &kwaIsdpCacheDeviceId.len);
  if (kwaIsdpCacheDeviceId.rc != XLIBRC_SUCCESS)
  {
    kwaIsdpCacheDeviceId.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaIsdpCacheDeviceId);
    return kwaIsdpCacheDeviceId.rc;
  }
 
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyIsdpCacheDeviceIdValue, kwaIsdpCacheDeviceId.len);

  owa.l7rc = usmdbIsdpIntfNeighborHoldTimeGet(keyIsdpCacheIfIndexValue, keyIsdpCacheDeviceIdValue, &objIsdpCacheHoldtimeValue);
  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: IsdpCacheHoldtime */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &objIsdpCacheHoldtimeValue,
                           sizeof (objIsdpCacheHoldtimeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}
