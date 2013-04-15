/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_SwitchingMFDB.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to MFDB-object.xml
*
* @create  12 March 2008
*
* @author  Radha K
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_SwitchingMFDB_obj.h"
#include "usmdb_mfdb_api.h"
#include "usmdb_common.h"
#include "usmdb_util_api.h"


/*******************************************************************************
* @function fpObjGet_SwitchingMFDB_CurrentEntries
*
* @purpose Get 'CurrentEntries'
*
* @description [CurrentEntries]: The number of current entries in the MFDB
*              table 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingMFDB_CurrentEntries (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objCurrentEntriesValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbMfdbCurrEntriesGet (L7_UNIT_CURRENT, &objCurrentEntriesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objCurrentEntriesValue,
                     sizeof (objCurrentEntriesValue));

  /* return the object value: CurrentEntries */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objCurrentEntriesValue,
                           sizeof (objCurrentEntriesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingMFDB_MaxTableEntries
*
* @purpose Get 'MaxTableEntries'
*
* @description [MaxTableEntries]: The maximum number of entries that the MFDB
*              table can hold 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingMFDB_MaxTableEntries (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMaxTableEntriesValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc =
    usmDbMfdbMaxTableEntriesGet (L7_UNIT_CURRENT, &objMaxTableEntriesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMaxTableEntriesValue,
                     sizeof (objMaxTableEntriesValue));

  /* return the object value: MaxTableEntries */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMaxTableEntriesValue,
                           sizeof (objMaxTableEntriesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingMFDB_MostEntries
*
* @purpose Get 'MostEntries'
*
* @description [MostEntries]: The most number of entries ever in the MFDB
*              table 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingMFDB_MostEntries (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMostEntriesValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* get the value from application */
  owa.l7rc = usmDbMfdbMostEntriesGet (L7_UNIT_CURRENT, &objMostEntriesValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objMostEntriesValue, sizeof (objMostEntriesValue));

  /* return the object value: MostEntries */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMostEntriesValue,
                           sizeof (objMostEntriesValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_SwitchingMFDB_ProtocolType
*
* @purpose Get 'ProtocolType'
*
* @description [ProtocolType]: MFDB Protocol Type 
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingMFDB_ProtocolType (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objProtocolTypeValue;
  xLibU32_t nextProtocolTypeValue;

	
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: ProtocolType */
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingMFDB_ProtocolType,
                          (xLibU8_t *) & objProtocolTypeValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    nextProtocolTypeValue = L7_MFDB_PROTOCOL_STATIC;	
	 owa.l7rc = L7_SUCCESS;
  }
  else
  {
    owa.l7rc = L7_SUCCESS;
    if(objProtocolTypeValue == L7_MFDB_PROTOCOL_STATIC)
			nextProtocolTypeValue = L7_MFDB_PROTOCOL_GMRP;
	else if(objProtocolTypeValue == L7_MFDB_PROTOCOL_GMRP)
			nextProtocolTypeValue = L7_MFDB_PROTOCOL_IGMP;
	else if(objProtocolTypeValue == L7_MFDB_PROTOCOL_IGMP)
			nextProtocolTypeValue = L7_MFDB_PROTOCOL_MLD;
	else 
		owa.l7rc = L7_FAILURE;
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }


  FPOBJ_TRACE_NEW_KEY (bufp, &nextProtocolTypeValue, owa.len);

  /* return the object value: ProtocolType */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextProtocolTypeValue,
                           sizeof (nextProtocolTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
/*******************************************************************************
* @function fpObjGet_SwitchingMFDB_MFDBVIDMACAddress
*
* @purpose Get 'MFDBVIDMACAddress'
*
* @description [MFDBVIDMACAddress]: VLAN Id and Mac combination
*
* @note  This is KEY Object
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingMFDB_MFDBVIDMACAddress (void *wap, void *bufp)
{
 
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t kwaProtocolTypeValue;
 
  L7_uchar8 objMFDBVIDMACAddress[L7_MFDB_KEY_SIZE];
  L7_uchar8 nextMFDBVIDMACAddress[L7_MFDB_KEY_SIZE];
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (objMFDBVIDMACAddress));
 
  usmdbMfdbUserInfo_t compInfo;
 
 
  FPOBJ_TRACE_ENTER (bufp);
 
  /* retrieve key: ProtocolType */
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingMFDB_ProtocolType,
                          (xLibU8_t *) & kwaProtocolTypeValue, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
 
  memset(objMFDBVIDMACAddress, 0x00, sizeof(objMFDBVIDMACAddress));
  memset(&compInfo, 0x00, sizeof(compInfo));
 
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingMFDB_MFDBVIDMACAddress,
                          (xLibU8_t *)  objMFDBVIDMACAddress, &owa.len); 
  if(owa.rc != XLIBRC_SUCCESS)
  {
     memset(nextMFDBVIDMACAddress, 0x00, sizeof(nextMFDBVIDMACAddress));
  }
  else
  {
     memset(nextMFDBVIDMACAddress, 0x00, sizeof(nextMFDBVIDMACAddress));
     memcpy(nextMFDBVIDMACAddress, objMFDBVIDMACAddress, sizeof(objMFDBVIDMACAddress));
  } 
  owa.l7rc = usmDbMfdbComponentEntryGetNext(L7_UNIT_CURRENT, 
                                             nextMFDBVIDMACAddress, 
                                             kwaProtocolTypeValue, 
                                             &compInfo);
 if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
 
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *)  nextMFDBVIDMACAddress,
                           sizeof (nextMFDBVIDMACAddress));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingMFDB_MFDBType
*
* @purpose Get 'MFDBType'
*
* @description [MFDBType]: MFDB Type 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingMFDB_MFDBType (void *wap, void *bufp)
{
  char kwaMFDBVIDMACAddress[L7_MFDB_KEY_SIZE];
  /* xLibStr256_t kwaMFDBVIDMACAddress;  */
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (kwaMFDBVIDMACAddress));

  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyProtocolTypeValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMFDBTypeValue;
	
  usmdbMfdbUserInfo_t info;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: MFDBVlanId */
  memset(kwaMFDBVIDMACAddress, 0x00, sizeof(kwaMFDBVIDMACAddress));
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingMFDB_MFDBVIDMACAddress,
                           (xLibU8_t *)  kwaMFDBVIDMACAddress, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, kwaMFDBVIDMACAddress, kwa.len);
	
  /* retrieve key: ProtocolType */
  kwa3.rc = xLibFilterGet (wap, XOBJ_SwitchingMFDB_ProtocolType,
                          (xLibU8_t *) & keyProtocolTypeValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyProtocolTypeValue, kwa3.len);

  /* get the value from application */
  owa.l7rc =
    usmDbMfdbEntryProtocolInfoGet (L7_UNIT_CURRENT,
                                   kwaMFDBVIDMACAddress,
                                   keyProtocolTypeValue,
                                   &info);
	
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: MFDBType */
  objMFDBTypeValue = info.usmdbMfdbType;
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMFDBTypeValue,
                           sizeof (objMFDBTypeValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingMFDB_MFDBCompId
*
* @purpose Get 'MFDBCompId'
*
* @description [MFDBCompId]: Mfdb Component Id 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingMFDB_MFDBCompId (void *wap, void *bufp)
{
  char kwaMFDBVIDMACAddress[L7_MFDB_KEY_SIZE];
  /*  xLibStr256_t kwaMFDBVIDMACAddress;  */
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (kwaMFDBVIDMACAddress));

  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyProtocolTypeValue;
	
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objMFDBCompIdValue;
	
  usmdbMfdbUserInfo_t info;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: MFDBVlanId */
  memset(kwaMFDBVIDMACAddress, 0x00, sizeof(kwaMFDBVIDMACAddress));
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingMFDB_MFDBVIDMACAddress,
                           (xLibU8_t *)  kwaMFDBVIDMACAddress, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, kwaMFDBVIDMACAddress, kwa.len);

  /* retrieve key: ProtocolType */
  kwa3.rc = xLibFilterGet (wap, XOBJ_SwitchingMFDB_ProtocolType,
                          (xLibU8_t *) & keyProtocolTypeValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyProtocolTypeValue, kwa3.len);

  /* get the value from application */
  owa.l7rc =
    usmDbMfdbEntryProtocolInfoGet (L7_UNIT_CURRENT, 
                                   kwaMFDBVIDMACAddress,
                                   keyProtocolTypeValue,
                                   &info);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: MFDBCompId */
  objMFDBCompIdValue = info.usmdbMfdbCompId;
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objMFDBCompIdValue,
                           sizeof (objMFDBCompIdValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingMFDB_MFDBDescription
*
* @purpose Get 'MFDBDescription'
*
* @description [MFDBDescription]: Mfdb Description 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingMFDB_MFDBDescription (void *wap, void *bufp)
{
  char kwaMFDBVIDMACAddress[L7_MFDB_KEY_SIZE];
  /* xLibStr256_t kwaMFDBVIDMACAddress; */
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (kwaMFDBVIDMACAddress));

  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyProtocolTypeValue;
	
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objMFDBDescriptionValue;
  usmdbMfdbUserInfo_t info;
  memset(objMFDBDescriptionValue, 0x00, sizeof(objMFDBDescriptionValue));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: MFDBVlanId */
  memset(kwaMFDBVIDMACAddress, 0x00, sizeof(kwaMFDBVIDMACAddress));
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingMFDB_MFDBVIDMACAddress,
                           (xLibU8_t *)  kwaMFDBVIDMACAddress, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, kwaMFDBVIDMACAddress, kwa.len);

  /* retrieve key: ProtocolType */
  kwa3.rc = xLibFilterGet (wap, XOBJ_SwitchingMFDB_ProtocolType,
                          (xLibU8_t *) & keyProtocolTypeValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyProtocolTypeValue, kwa3.len);

  /* get the value from application */
  owa.l7rc =
    usmDbMfdbEntryProtocolInfoGet (L7_UNIT_CURRENT, 
                                   kwaMFDBVIDMACAddress, 
                                   keyProtocolTypeValue,
                                   &info);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: MFDBDescription */
  memcpy(objMFDBDescriptionValue, info.usmdbMfdbDescr, strlen(info.usmdbMfdbDescr));
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objMFDBDescriptionValue,
                           strlen (objMFDBDescriptionValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingMFDB_MFDBForwardingPortMask
*
* @purpose Get 'MFDBForwardingPortMask'
*
* @description [MFDBForwardingPortMask]: forwarding ports mask 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingMFDB_MFDBForwardingPortMask (void *wap, void *bufp)
{
  char kwaMFDBVIDMACAddress[L7_MFDB_KEY_SIZE];  
  /*  xLibStr256_t kwaMFDBVIDMACAddress;  */
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (kwaMFDBVIDMACAddress));
	
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyProtocolTypeValue;
	
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objMFDBForwardingPortMaskValue;

  xLibStr256_t stat;
  usmdbMfdbUserInfo_t info;
  L7_uint32 listOfIntf[256];
  xLibU32_t numFwd, indx;
  xLibBool_t commaFlag = XLIB_TRUE;
  memset(objMFDBForwardingPortMaskValue, 
		         0x00, sizeof(objMFDBForwardingPortMaskValue));
	
  FPOBJ_TRACE_ENTER (bufp);

/* retrieve key: MFDBVlanId */
  memset(kwaMFDBVIDMACAddress, 0x00, sizeof(kwaMFDBVIDMACAddress));
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingMFDB_MFDBVIDMACAddress,
                           (xLibU8_t *)  kwaMFDBVIDMACAddress, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, kwaMFDBVIDMACAddress, kwa.len);
	
  /* retrieve key: ProtocolType */
  kwa3.rc = xLibFilterGet (wap, XOBJ_SwitchingMFDB_ProtocolType,
                          (xLibU8_t *) & keyProtocolTypeValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyProtocolTypeValue, kwa3.len);

  /* get the value from application */
  owa.l7rc =
    usmDbMfdbEntryProtocolInfoGet (L7_UNIT_CURRENT,
                                   kwaMFDBVIDMACAddress, 
                                   keyProtocolTypeValue,
                                   &info);
   if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  owa.l7rc = usmDbConvertMaskToList(&info.usmdbMfdbFwdMask, listOfIntf, &numFwd);
	
  if ( (owa.l7rc != L7_SUCCESS) || (numFwd <= 0) )
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

   commaFlag = XLIB_FALSE;
  for (indx = 1; indx <= numFwd; indx++)
   {
      memset(stat, 0x0, sizeof(stat));
      sprintf(stat, "%d",listOfIntf[indx]);
      if(commaFlag == XLIB_FALSE)
      {
        OSAPI_STRNCAT(objMFDBForwardingPortMaskValue,stat);
        commaFlag = XLIB_TRUE;
      }
      else
      {
        OSAPI_STRNCAT(objMFDBForwardingPortMaskValue,",");
        OSAPI_STRNCAT(objMFDBForwardingPortMaskValue, stat);
      }
  }
	
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: MFDBForwardingPortMask */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objMFDBForwardingPortMaskValue,
                           strlen (objMFDBForwardingPortMaskValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingMFDB_MFDBFilteringPortMask
*
* @purpose Get 'MFDBFilteringPortMask'
*
* @description [MFDBFilteringPortMask]: filtering ports mask 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingMFDB_MFDBFilteringPortMask (void *wap, void *bufp)
{
  char kwaMFDBVIDMACAddress[L7_MFDB_KEY_SIZE];  
/* xLibStr256_t kwaMFDBVIDMACAddress;  */
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (kwaMFDBVIDMACAddress));
	
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyProtocolTypeValue;
	
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objMFDBFilteringPortMaskValue;
	
  xLibStr256_t stat;
  usmdbMfdbUserInfo_t info;
  L7_uint32 listOfIntf[256];
  xLibU32_t numFwd, indx;
  xLibBool_t commaFlag = XLIB_TRUE;
  memset(objMFDBFilteringPortMaskValue, 
		         0x00, sizeof(objMFDBFilteringPortMaskValue));

	
  FPOBJ_TRACE_ENTER (bufp);

/* retrieve key: MFDBVlanId */
  memset(kwaMFDBVIDMACAddress, 0x00, sizeof(kwaMFDBVIDMACAddress));
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingMFDB_MFDBVIDMACAddress,
                           (xLibU8_t *)  kwaMFDBVIDMACAddress, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, kwaMFDBVIDMACAddress, kwa.len);
	

  /* retrieve key: ProtocolType */
  kwa3.rc = xLibFilterGet (wap, XOBJ_SwitchingMFDB_ProtocolType,
                          (xLibU8_t *) & keyProtocolTypeValue, &kwa3.len);

  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyProtocolTypeValue, kwa3.len);

  /* get the value from application */
  owa.l7rc =
    usmDbMfdbEntryProtocolInfoGet (L7_UNIT_CURRENT, 
                                   kwaMFDBVIDMACAddress, 
                                   keyProtocolTypeValue,
                                   &info);

  owa.l7rc = usmDbConvertMaskToList(&info.usmdbMfdbFltMask, listOfIntf, &numFwd);
	
  if ( (owa.l7rc != L7_SUCCESS) || (numFwd <= 0) )
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  commaFlag = XLIB_FALSE;
  for (indx = 1; indx <= numFwd; indx++)
  {
      memset(stat, 0x0, sizeof(stat));
      sprintf(stat, "%d",listOfIntf[indx]);
      if(commaFlag == XLIB_FALSE)
      {
        OSAPI_STRNCAT(objMFDBFilteringPortMaskValue,stat);
        commaFlag = XLIB_TRUE;
      }
      else
      {
        OSAPI_STRNCAT(objMFDBFilteringPortMaskValue,",");
        OSAPI_STRNCAT(objMFDBFilteringPortMaskValue, stat);
      }
  }
	
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: MFDBFilteringPortMask */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objMFDBFilteringPortMaskValue,
                           strlen (objMFDBFilteringPortMaskValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_SwitchingMFDB_MFDBFwdPortList
* @purpose Get 'MFDBFwdPortList'
*
* @description [MFDBFwdPortList]: forwarding ports mask 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingMFDB_MFDBFwdPortList (void *wap, void *bufp)
{
  char kwaMFDBVIDMACAddress[L7_MFDB_KEY_SIZE];  
/*   xLibStr256_t kwaMFDBVIDMACAddress;  */
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (kwaMFDBVIDMACAddress));
	
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyProtocolTypeValue;
	
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objMFDBForwardingPortMaskValue;
  xLibStr256_t objMFDBFilteringPortMaskValue;

  xLibStr256_t stat;
  usmdbMfdbUserInfo_t info;
  L7_uint32 listOfIntf[256], filterList[256];
  xLibU32_t numFwd, numFilter,indx;
  xLibBool_t commaFlag = XLIB_TRUE;
  memset(objMFDBForwardingPortMaskValue, 
		         0x00, sizeof(objMFDBForwardingPortMaskValue));
  memset(objMFDBFilteringPortMaskValue, 
		         0x00, sizeof(objMFDBFilteringPortMaskValue));
	
  FPOBJ_TRACE_ENTER (bufp);

/* retrieve key: MFDBVlanId */
  memset(kwaMFDBVIDMACAddress, 0x00, sizeof(kwaMFDBVIDMACAddress));
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingMFDB_MFDBVIDMACAddress,
                           (xLibU8_t *)  kwaMFDBVIDMACAddress, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, kwaMFDBVIDMACAddress, kwa.len);
	
  /* retrieve key: ProtocolType */
  kwa3.rc = xLibFilterGet (wap, XOBJ_SwitchingMFDB_ProtocolType,
                          (xLibU8_t *) & keyProtocolTypeValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyProtocolTypeValue, kwa3.len);

  /* get the value from application */
  owa.l7rc =
    usmDbMfdbEntryProtocolInfoGet (L7_UNIT_CURRENT,
                                   kwaMFDBVIDMACAddress, 
                                   keyProtocolTypeValue,
                                   &info);
   if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  owa.l7rc = usmDbConvertMaskToList(&info.usmdbMfdbFwdMask, listOfIntf, &numFwd);
	
  if ( (owa.l7rc != L7_SUCCESS) )
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  commaFlag = XLIB_FALSE;
  for (indx = 1; indx <= numFwd; indx++)
   {
      memset(stat, 0x0, sizeof(stat));
      sprintf(stat, "%d",listOfIntf[indx]);
      if(commaFlag == XLIB_FALSE)
      {
        OSAPI_STRNCAT(objMFDBForwardingPortMaskValue,stat);
        commaFlag = XLIB_TRUE;
      }
      else
      {
        OSAPI_STRNCAT(objMFDBForwardingPortMaskValue,",");
        OSAPI_STRNCAT(objMFDBForwardingPortMaskValue, stat);
      }
  }
	
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

    owa.l7rc = usmDbConvertMaskToList(&info.usmdbMfdbFltMask, filterList, &numFilter);
	
  if ( (owa.l7rc != L7_SUCCESS) )
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  commaFlag = XLIB_FALSE;
  for (indx = 1; indx <= numFilter; indx++)
   {
      memset(stat, 0x0, sizeof(stat));
      sprintf(stat, "%d",filterList[indx]);
      if(commaFlag == XLIB_FALSE)
      {
        OSAPI_STRNCAT(objMFDBFilteringPortMaskValue,stat);
        commaFlag = XLIB_TRUE;
      }
      else
      {
        OSAPI_STRNCAT(objMFDBFilteringPortMaskValue,",");
        OSAPI_STRNCAT(objMFDBFilteringPortMaskValue, stat);
      }
  }

  if (numFilter <= 0 )
  {
    memset(objMFDBFilteringPortMaskValue, 0x00, sizeof(objMFDBFilteringPortMaskValue));
  }
  else
  {
    if (numFwd <= 0)
    {
      memset(objMFDBForwardingPortMaskValue, 0x00, sizeof(objMFDBForwardingPortMaskValue));
	  strncpy(objMFDBForwardingPortMaskValue, objMFDBFilteringPortMaskValue, strlen(objMFDBFilteringPortMaskValue));
    }
	else
	{
      strcat(objMFDBForwardingPortMaskValue, objMFDBFilteringPortMaskValue);
	}
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }


  /* return the object value: MFDBForwardingPortMask */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objMFDBForwardingPortMaskValue,
                           strlen (objMFDBForwardingPortMaskValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}



/*******************************************************************************
* @function fpObjGet_SwitchingMFDB_MFDBFwdIntf
*
* @purpose Get 'MFDBFwdIntf'
*
* @description [MFDBFwdIntf]: the forwarding interfaces of the next VLANID-MAC
*              entry. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingMFDB_MFDBFwdIntf (void *wap, void *bufp)
{
  char kwaMFDBVIDMACAddress[L7_MFDB_KEY_SIZE];    
/*  xLibStr256_t kwaMFDBVIDMACAddress;  */
  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (kwaMFDBVIDMACAddress));
	
  fpObjWa_t kwa3 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyProtocolTypeValue;
	
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objMFDBForwardingPortMaskValue;

  xLibStr256_t stat;
  L7_INTF_MASK_t absMask;
  L7_uint32 listOfIntf[256];
  xLibU32_t numFwd, indx;
  xLibBool_t commaFlag = XLIB_TRUE;
  memset(objMFDBForwardingPortMaskValue, 
		         0x00, sizeof(objMFDBForwardingPortMaskValue));
	
  FPOBJ_TRACE_ENTER (bufp);

/* retrieve key: MFDBVlanId */
  memset(kwaMFDBVIDMACAddress, 0x00, sizeof(kwaMFDBVIDMACAddress));
  kwa.rc = xLibFilterGet (wap, XOBJ_SwitchingMFDB_MFDBVIDMACAddress,
                           (xLibU8_t *)  kwaMFDBVIDMACAddress, &kwa.len);
  if (kwa.rc != XLIBRC_SUCCESS)
  {
    kwa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa);
    return kwa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, kwaMFDBVIDMACAddress, kwa.len);
	
  /* retrieve key: ProtocolType */
  kwa3.rc = xLibFilterGet (wap, XOBJ_SwitchingMFDB_ProtocolType,
                          (xLibU8_t *) & keyProtocolTypeValue, &kwa3.len);
  if (kwa3.rc != XLIBRC_SUCCESS)
  {
    kwa3.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa3);
    return kwa3.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyProtocolTypeValue, kwa3.len);

  /* get the value from application */
  owa.l7rc =
    usmDbMfdbEntryFwdIntfGetNext(L7_UNIT_CURRENT, 
		                            kwaMFDBVIDMACAddress, 
		                            L7_MATCH_EXACT, 
		                            &absMask);
    
   if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  owa.l7rc = usmDbConvertMaskToList(&absMask, listOfIntf, &numFwd);
	
  if ( (owa.l7rc != L7_SUCCESS) || (numFwd <= 0) )
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

   commaFlag = XLIB_FALSE;
  for (indx = 1; indx <= numFwd; indx++)
   {
      memset(stat, 0x0, sizeof(stat));
      sprintf(stat, "%d",listOfIntf[indx]);
      if(commaFlag == XLIB_FALSE)
      {
        OSAPI_STRNCAT(objMFDBForwardingPortMaskValue,stat);
        commaFlag = XLIB_TRUE;
      }
      else
      {
        OSAPI_STRNCAT(objMFDBForwardingPortMaskValue,",");
        OSAPI_STRNCAT(objMFDBForwardingPortMaskValue, stat);
      }
  }
	
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: MFDBForwardingPortMask */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objMFDBForwardingPortMaskValue,
                           strlen (objMFDBForwardingPortMaskValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
/*******************************************************************************
* @function fpObjGet_SwitchingMFDB_MACAddress
*
* @purpose Get 'MFDBMACAddress'
*
* @description [MFDBMACAddress]: MFDB MAC Address
*
* @note
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingMFDB_MACAddress (void *wap, void *bufp)
{

  L7_uchar8 objMFDBVIDMACAddress[L7_MFDB_KEY_SIZE];
  L7_uchar8 nextMFDBVIDMACAddress[L7_MFDB_KEY_SIZE*4];
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (objMFDBVIDMACAddress));

  FPOBJ_TRACE_ENTER (bufp);

  memset(objMFDBVIDMACAddress, 0x00, sizeof(objMFDBVIDMACAddress));

  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingMFDB_MFDBVIDMACAddress,
                          (xLibU8_t *)  objMFDBVIDMACAddress, &owa.len);
 if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  sprintf(nextMFDBVIDMACAddress,"%02x:%02x:%02x:%02x:%02x:%02x",objMFDBVIDMACAddress[2],objMFDBVIDMACAddress[3],objMFDBVIDMACAddress[4],objMFDBVIDMACAddress[5],objMFDBVIDMACAddress[6],objMFDBVIDMACAddress[7]);
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *)  nextMFDBVIDMACAddress,
                           sizeof (nextMFDBVIDMACAddress));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
/*******************************************************************************
* @function fpObjGet_SwitchingMFDB_VlanID
*
* @purpose Get 'MFDBVlanID'
*
* @description [MFDBVlanID]: MFDB VlanID
*
* @note
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_SwitchingMFDB_VlanID (void *wap, void *bufp)
{

  L7_uchar8 objMFDBVIDMACAddress[L7_MFDB_KEY_SIZE];
  L7_ushort16 vlanId;
  L7_uchar8 objvlanId[L7_MFDB_KEY_SIZE];
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (objMFDBVIDMACAddress));

  FPOBJ_TRACE_ENTER (bufp);

  memset(objMFDBVIDMACAddress, 0x00, sizeof(objMFDBVIDMACAddress));
  owa.rc = xLibFilterGet (wap, XOBJ_SwitchingMFDB_MFDBVIDMACAddress,
                          (xLibU8_t *)  objMFDBVIDMACAddress, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  memcpy(&vlanId,objMFDBVIDMACAddress,sizeof(vlanId));
  sprintf(objvlanId,"%d",vlanId);
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *)  objvlanId,
                           sizeof (objvlanId));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

