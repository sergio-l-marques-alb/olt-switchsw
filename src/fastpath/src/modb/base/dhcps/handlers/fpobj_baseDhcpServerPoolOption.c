/*******************************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
********************************************************************************
*
* @filename fpobj_baseDhcpServerPoolOption.c
*
* @purpose
*
* @component object handlers
*
* @comments  Refer to sntp-object.xml
*
* @create  18 January 2008
*
* @author  Rama Sasthri, Kristipati
* @end
*
********************************************************************************/
#include "fpobj_util.h"
#include "_xe_baseDhcpServerPoolOption_obj.h"
#include "usmdb_dhcps_api.h"
#include "dhcps_exports.h"
#include "usmdb_common.h"
#include "usmdb_util_api.h"


/*******************************************************************************
* @function fpObjGet_baseDhcpServerPoolOption_OptionIndex
*
* @purpose Get 'OptionIndex'
*
* @description [OptionIndex] The Pool Index alongwith option-code, acts as index for the DHCP Server Option table. 
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDhcpServerPoolOption_OptionIndex (void *wap, void *bufp)
{

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objOptionIndexValue;
	
  xLibU32_t nextObjOptionIndexValue;
  owa.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerPoolOption_OptionIndex,
                          (xLibU8_t *) & objOptionIndexValue, &owa.len);

  if (owa.rc != XLIBRC_SUCCESS)
  {
   /* nextObjPoolIndexValue =0;
    owa.l7rc = usmDbDhcpsPoolEntryFirst(L7_UNIT_CURRENT, poolNameTemp, &nextObjPoolIndexValue);*/
    objOptionIndexValue = 0;
    owa.l7rc = usmDbDhcpsPoolGetNext (L7_UNIT_CURRENT, objOptionIndexValue,
                                      &nextObjOptionIndexValue);
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
  }
  else
  {
     owa.l7rc = usmDbDhcpsPoolGetNext (L7_UNIT_CURRENT, objOptionIndexValue,
                                      &nextObjOptionIndexValue);
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objOptionIndexValue, owa.len);
  }
  
  if(owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjOptionIndexValue, sizeof(nextObjOptionIndexValue));

  /* return the object value: OptionIndex */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjOptionIndexValue,
                           sizeof (objOptionIndexValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseDhcpServerPoolOption_OptionCode
*
* @purpose Get 'OptionCode'
*
* @description [OptionCode] This specifies the DHCP option code. This alongwith pool-index acts as the index of the DHCP Server Option table.
*
* @notes  
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDhcpServerPoolOption_OptionCode (void *wap, void *bufp)
{

  fpObjWa_t kwa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objOptionIndexValue;
	
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU8_t));
  xLibU8_t objOptionCodeValue;	
  xLibU8_t nextObjOptionCodeValue;
	
  xLibStr256_t poolName;
  objOptionCodeValue = 0x00;
  nextObjOptionCodeValue = 0x00;
	
  FPOBJ_TRACE_ENTER (bufp);

  kwa.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerPoolOption_OptionIndex,
                          (xLibU8_t *) & objOptionIndexValue, &kwa.len);
  if(kwa.rc != XLIBRC_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  

  if( usmDbDhcpsNumberToPoolNameGet(L7_UNIT_CURRENT, objOptionIndexValue, poolName) != L7_SUCCESS)
  {
         owa.rc = XLIBRC_FAILURE;
         return owa.rc;
  }

  /* retrieve key: OptionCode */
  owa.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerPoolOption_OptionCode,
                          (xLibU8_t *) &objOptionCodeValue, &owa.len);
  nextObjOptionCodeValue = 0x00;
	
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
    owa.l7rc = usmDbDhcpsOptionGetFirst(L7_UNIT_CURRENT, poolName,&nextObjOptionCodeValue ); 
  }
  else
  {
    FPOBJ_TRACE_CURRENT_KEY (bufp, &objOptionCodeValue, owa.len);
    owa.l7rc = usmDbDhcpsOptionGetNext(L7_UNIT_CURRENT, poolName, (L7_uchar8)objOptionCodeValue,
                               &nextObjOptionCodeValue);
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_ENDOF_TABLE;
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  FPOBJ_TRACE_NEW_KEY (bufp, &nextObjOptionCodeValue, owa.len);

  /* return the object value: OptionCode */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) &nextObjOptionCodeValue, sizeof (xLibU8_t));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;

}


/*******************************************************************************
* @function fpObjGet_baseDhcpServerPoolOption_PoolName
*
* @purpose Get 'PoolName'
*
* @description [PoolName]: The name of the DHCP Address pool. This value cannot
*              be modified 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDhcpServerPoolOption_PoolName (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyOptionIndexValue;

  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr256_t));
  xLibStr256_t objPoolNameValue;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: OptionIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerPoolOption_OptionIndex,
                           (xLibU8_t *) & keyOptionIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyOptionIndexValue, kwa1.len);

  /* get the value from application */
  owa.l7rc = usmDbDhcpsNumberToPoolNameGet(L7_UNIT_CURRENT, keyOptionIndexValue, objPoolNameValue);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: PoolName */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objPoolNameValue,
                           strlen (objPoolNameValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseDhcpServerPoolOption_AsciiData
*
* @purpose Get 'AsciiData'
*
* @description [AsciiData]: This specifies an NVT ASCII character string.
*              ASCII character strings that contain white space must be delimited
*              by quotation marks. If an ascii optiondata is entered
*              again, it gets concatenated with the 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDhcpServerPoolOption_AsciiData (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyOptionIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU8_t));
  xLibU8_t keyOptionCodeValue;
	
  fpObjWa_t owa = FPOBJ_INIT_WA (L7_DHCPS_POOL_OPTION_LENGTH);
  xLibU8_t objAsciiDataValue[L7_DHCPS_POOL_OPTION_LENGTH];
  
  xLibStr256_t poolName;
  xLibStr256_t strOptionData ;
  xLibU32_t optionDataLen;
  xLibU8_t optionDataFormat;
  xLibU8_t optionStatus;
  memset(objAsciiDataValue,0x00,sizeof(objAsciiDataValue));
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: OptionIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerPoolOption_OptionIndex,
                           (xLibU8_t *) &keyOptionIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyOptionIndexValue, kwa1.len);

  /* retrieve key: OptionCode */
  kwa2.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerPoolOption_OptionCode,
                           (xLibU8_t *) &keyOptionCodeValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyOptionCodeValue, kwa2.len);

  /* get the value from application */
  memset(poolName, 0x00,sizeof(poolName));
  owa.l7rc  = L7_FAILURE;
  if(usmDbDhcpsNumberToPoolNameGet(L7_UNIT_CURRENT, keyOptionIndexValue, poolName) == L7_SUCCESS)
  {
     memset(strOptionData, 0x00, sizeof(strOptionData));
     optionDataFormat = 0x00;
     optionStatus = 0x00;
     if(usmDbDhcpsOptionGet(L7_UNIT_CURRENT, poolName, keyOptionCodeValue, strOptionData, &optionDataLen, &optionDataFormat, &optionStatus) == L7_SUCCESS)
     {
		if( (L7_uint32)optionDataFormat ==  L7_DHCPS_ASCII)
		{
		   strcpy(objAsciiDataValue,strOptionData);
		   owa.l7rc = L7_SUCCESS;
		}
		else
		{
		   owa.l7rc = L7_FAILURE;
		}
     }
  }
  else
  {
     owa.l7rc = L7_FAILURE;
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: AsciiData */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objAsciiDataValue,
                           strlen (objAsciiDataValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseDhcpServerPoolOption_AsciiData
*
* @purpose Set 'AsciiData'
*
* @description [AsciiData]: This specifies an NVT ASCII character string.
*              ASCII character strings that contain white space must be delimited
*              by quotation marks. If an ascii optiondata is entered
*              again, it gets concatenated with the 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseDhcpServerPoolOption_AsciiData (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (L7_DHCPS_POOL_OPTION_LENGTH);
  xLibU8_t objAsciiDataValue[L7_DHCPS_POOL_OPTION_LENGTH];
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyOptionIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU8_t));
  xLibU8_t keyOptionCodeValue;
  xLibStr256_t poolName;
  memset(poolName, 0x00, sizeof(poolName));
  memset(objAsciiDataValue, 0x00, sizeof(objAsciiDataValue));

  L7_uchar8 strOptionData [L7_DHCPS_POOL_OPTION_LENGTH];
  L7_uchar8 dataFormat;
  char status;
  L7_uint32 intAsciiLen, intDataLen;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: AsciiData */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objAsciiDataValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objAsciiDataValue, owa.len);


  /* retrieve key: OptionIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerPoolOption_OptionIndex,
                           (xLibU8_t *) & keyOptionIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyOptionIndexValue, kwa1.len);

  /* retrieve key: OptionCode */
  kwa2.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerPoolOption_OptionCode,
                           (xLibU8_t *) &keyOptionCodeValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyOptionCodeValue, kwa2.len);


  /* Getting the pool name corresponding to this pool Index */
  if( usmDbDhcpsNumberToPoolNameGet(L7_UNIT_CURRENT, keyOptionIndexValue, poolName) != L7_SUCCESS)
  {
      owa.rc = XLIBRC_FAILURE;
      return owa.rc;
  }

  /* If get operation for this poolName-optionCode combination is successful, then
     this row-entry of pool-option table is validated */
  if (usmDbDhcpsOptionGet(L7_UNIT_CURRENT, poolName, (L7_uchar8)keyOptionCodeValue, strOptionData,
                          &intDataLen, &dataFormat, &status) != L7_SUCCESS)
  {
      owa.rc = XLIBRC_FAILURE;
      return owa.rc;
  }

  if(((L7_uint32)status == L7_DHCPS_NOT_READY) && ((L7_uint32)dataFormat !=  L7_DHCPS_NONE))
  {
      owa.rc = XLIBRC_DHCP_POOL_OPTIONS_MUST_SPECIFY;
      return owa.rc;
  }

  if(((L7_uint32)status == L7_DHCPS_ACTIVE) && ((L7_uint32)dataFormat !=  L7_DHCPS_ASCII))
  {
      owa.rc = XLIBRC_DHCP_POOL_OPTIONS_NOT_ALLOWED;
      return owa.rc;
  }

  /* Get the number of characters in the ascii string */
  intAsciiLen = strlen(objAsciiDataValue);
  
  /* Concatenate all the strings */
  if((intDataLen + intAsciiLen) <= L7_DHCPS_POOL_OPTION_LENGTH)
  {
    memcpy(&strOptionData[intDataLen], objAsciiDataValue, intAsciiLen);

    intDataLen += intAsciiLen;
  }
  else
  {
      owa.rc = XLIBRC_DHCP_POOL_OPTIONS_LENGTH_EXCEEDED;
      return owa.rc;
  }
 
  owa.l7rc = usmDbDhcpsOptionSet(L7_UNIT_CURRENT,poolName, 
                           (L7_uchar8)keyOptionCodeValue,
                           strOptionData,
                           intDataLen,
                           L7_DHCPS_ASCII);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_DHCP_POOL_OPTIONS_FAILED;    
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}

/*******************************************************************************
* @function fpObjGet_baseDhcpServerPoolOption_HexData
*
* @purpose Get 'HexData'
*
* @description [HexData]: This specifies semi-colon separated hexadecimal
*              data. Two hexadecimal digits in hexadecimal character string
*              represents one byte . Hexadecimal strings can be entered
*              in the following formats : 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDhcpServerPoolOption_HexData (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyOptionIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU8_t));
  xLibU8_t keyOptionCodeValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (L7_DHCPS_POOL_OPTION_LENGTH);
  xLibU8_t objHexDataValue[L7_DHCPS_POOL_OPTION_LENGTH];
  memset(objHexDataValue,0x00,sizeof(objHexDataValue));

  xLibStr256_t poolName;
  xLibU32_t optionDataLen;
  xLibU8_t optionDataFormat;
  xLibU8_t optionStatus;

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: OptionIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerPoolOption_OptionIndex,
                           (xLibU8_t *) & keyOptionIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyOptionIndexValue, kwa1.len);

  /* retrieve key: OptionCode */
  kwa2.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerPoolOption_OptionCode,
                           (xLibU8_t *) &keyOptionCodeValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyOptionCodeValue, kwa2.len);

  /* get the value from application */
  memset(poolName, 0x00,sizeof(poolName));
  owa.l7rc = L7_FAILURE;
  if(usmDbDhcpsNumberToPoolNameGet(L7_UNIT_CURRENT, keyOptionIndexValue, poolName) == L7_SUCCESS)
  {
     optionDataFormat = 0x00;
     optionStatus = 0x00;
     if(usmDbDhcpsOptionGet(L7_UNIT_CURRENT, poolName, keyOptionCodeValue, objHexDataValue, &optionDataLen, &optionDataFormat, &optionStatus) == L7_SUCCESS)
     {
		if( (L7_uint32)optionDataFormat ==  L7_DHCPS_HEX)
		{
		    owa.l7rc = L7_SUCCESS;
		}
		else
		{
		   owa.l7rc = L7_FAILURE;
		}
     }
  }
  else
  {
     owa.l7rc = L7_FAILURE;
  }

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: HexData */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objHexDataValue,
                           optionDataLen);
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseDhcpServerPoolOption_HexData
*
* @purpose Set 'HexData'
*
* @description [HexData]: This specifies semi-colon separated hexadecimal
*              data. Two hexadecimal digits in hexadecimal character string
*              represents one byte . Hexadecimal strings can be entered
*              in the following formats : 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseDhcpServerPoolOption_HexData (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (L7_DHCPS_POOL_OPTION_LENGTH);
  xLibU8_t objHexDataValue[L7_DHCPS_POOL_OPTION_LENGTH];
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyOptionIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU8_t));
  xLibU8_t keyOptionCodeValue;

  xLibStr256_t poolName;
  memset(poolName, 0x00, sizeof(poolName));
  memset(objHexDataValue, 0x00, sizeof(objHexDataValue));

  L7_uchar8 strOptionData [L7_DHCPS_POOL_OPTION_LENGTH];
  L7_uchar8 dataFormat;
  char status;
  
  L7_uint32 intHexLen, intDataLen;

  memset(strOptionData, 0x00, sizeof(strOptionData));

	
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: HexData */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objHexDataValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objHexDataValue, owa.len);

  /* retrieve key: OptionIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerPoolOption_OptionIndex,
                           (xLibU8_t *) & keyOptionIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyOptionIndexValue, kwa1.len);

  /* retrieve key: OptionCode */
  kwa2.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerPoolOption_OptionCode,
                           (xLibU8_t *) &keyOptionCodeValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyOptionCodeValue, kwa2.len);

  

    /* Getting the pool name corresponding to this pool Index */
  if( usmDbDhcpsNumberToPoolNameGet(L7_UNIT_CURRENT, keyOptionIndexValue, poolName) != L7_SUCCESS)
  {
      owa.rc = XLIBRC_FAILURE;
      return owa.rc;
  }

  /* Getting the pool name corresponding to this pool Index */
  if( usmDbDhcpsNumberToPoolNameGet(L7_UNIT_CURRENT, keyOptionIndexValue, poolName) != L7_SUCCESS)
  {
      owa.rc = XLIBRC_FAILURE;
      return owa.rc;
  }

  /* If get operation for this poolName-optionCode combination is successful, then
     this row-entry of pool-option table is validated */
  if (usmDbDhcpsOptionGet(L7_UNIT_CURRENT, poolName, (L7_uchar8)keyOptionCodeValue, strOptionData,
                          &intDataLen, &dataFormat, &status) != L7_SUCCESS)
  {
      owa.rc = XLIBRC_FAILURE;
      return owa.rc;
  }

  if(((L7_uint32)status == L7_DHCPS_NOT_READY) && ((L7_uint32)dataFormat !=  L7_DHCPS_NONE))
  {
      owa.rc = XLIBRC_DHCP_POOL_OPTIONS_MUST_SPECIFY;
      return owa.rc;
  }

  if(((L7_uint32)status == L7_DHCPS_ACTIVE) && ((L7_uint32)dataFormat !=  L7_DHCPS_HEX))
  {
      owa.rc = XLIBRC_DHCP_POOL_OPTIONS_NOT_ALLOWED;
      return owa.rc;
  }

  intHexLen =  owa.len; /*length of the current string */
          

  /* Concatenate all the strings */
  if((intDataLen + intDataLen) <= L7_DHCPS_POOL_OPTION_LENGTH)
  {
    memcpy(&strOptionData[intDataLen], objHexDataValue, intHexLen);

    intDataLen += intHexLen;
  }
  else 
  {
    owa.rc = XLIBRC_DHCP_POOL_OPTIONS_LENGTH_EXCEEDED;  
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
 
 owa.l7rc = usmDbDhcpsOptionSet(USMDB_UNIT_CURRENT,poolName, 
                           (L7_uchar8)keyOptionCodeValue,
                           strOptionData,
                           intDataLen,
                           L7_DHCPS_HEX);

  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_DHCP_POOL_OPTIONS_FAILED;    
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjGet_baseDhcpServerPoolOption_IpAddressData
*
* @purpose Get 'IpAddressData'
*
* @description [IpAddressData]: This specifies the IP-address list(each IP-address
*              is separated by comma and the list is terminated by
*              semi-colon) for a DHCP client, an example would be 10.10.1.1,192.168.36.1,157.227.44.1;(no
*              spaces in between). 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDhcpServerPoolOption_IpAddressData (void *wap, void *bufp)
{
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyOptionIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU8_t));
  xLibU8_t keyOptionCodeValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (L7_DHCPS_POOL_OPTION_LENGTH);
  xLibU8_t objIpAddressDataValue[L7_DHCPS_POOL_OPTION_LENGTH];
  memset(objIpAddressDataValue, 0x00, sizeof(objIpAddressDataValue));

  xLibStr256_t poolName;
  xLibU32_t optionDataLen;
  xLibU8_t optionDataFormat;
  xLibU8_t optionStatus;
  
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: OptionIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerPoolOption_OptionIndex,
                           (xLibU8_t *) & keyOptionIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyOptionIndexValue, kwa1.len);

  /* retrieve key: OptionCode */
  kwa2.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerPoolOption_OptionCode,
                           (xLibU8_t *) &keyOptionCodeValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyOptionCodeValue, kwa2.len);

  /* get the value from application */
  memset(poolName, 0x00,sizeof(poolName));
  owa.l7rc = L7_FAILURE;
  if(usmDbDhcpsNumberToPoolNameGet(L7_UNIT_CURRENT, keyOptionIndexValue, poolName) == L7_SUCCESS)
  {
     optionDataFormat = 0x00;
     optionStatus = 0x00;
     if(usmDbDhcpsOptionGet(L7_UNIT_CURRENT, poolName, keyOptionCodeValue, objIpAddressDataValue, &optionDataLen, &optionDataFormat, &optionStatus) == L7_SUCCESS)
     {
		   if( (L7_uint32)optionDataFormat ==  L7_DHCPS_IP)
		   {
		     owa.l7rc = L7_SUCCESS;
		   }
     }
  }
  
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* return the object value: IpAddressData */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) objIpAddressDataValue,
                           optionDataLen);
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseDhcpServerPoolOption_IpAddressData
*
* @purpose Set 'IpAddressData'
*
* @description [IpAddressData]: This specifies the IP-address list(each IP-address
*              is separated by comma and the list is terminated by
*              semi-colon) for a DHCP client, an example would be 10.10.1.1,192.168.36.1,157.227.44.1;(no
*              spaces in between). 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseDhcpServerPoolOption_IpAddressData (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (3*L7_DHCPS_POOL_OPTION_LENGTH);
  xLibU8_t objIpAddressDataValue[3*L7_DHCPS_POOL_OPTION_LENGTH];
  fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyOptionIndexValue;
  fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibU8_t));
  xLibU8_t keyOptionCodeValue;

  xLibStr256_t poolName;
  memset(poolName, 0x00, sizeof(poolName));
  memset(objIpAddressDataValue, 0x00, sizeof(objIpAddressDataValue));

  L7_uchar8 strOptionData [L7_DHCPS_POOL_OPTION_LENGTH];
  L7_uchar8 dataFormat;
  char status;
  
  L7_uint32 currentDataLen = 0;
  L7_uint32 intDataLen = L7_NULL;
		
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: IpAddressData */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) objIpAddressDataValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, objIpAddressDataValue, owa.len);


  /* retrieve key: OptionIndex */
  kwa1.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerPoolOption_OptionIndex,
                           (xLibU8_t *) & keyOptionIndexValue, &kwa1.len);
  if (kwa1.rc != XLIBRC_SUCCESS)
  {
    kwa1.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa1);
    return kwa1.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyOptionIndexValue, kwa1.len);

  /* retrieve key: OptionCode */
  kwa2.rc = xLibFilterGet (wap, XOBJ_baseDhcpServerPoolOption_OptionCode,
                           (xLibU8_t *) &keyOptionCodeValue, &kwa2.len);
  if (kwa2.rc != XLIBRC_SUCCESS)
  {
    kwa2.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwa2);
    return kwa2.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyOptionCodeValue, kwa2.len);

  memset(strOptionData, 0x00, sizeof(strOptionData));

   /* Getting the pool name corresponding to this pool Index */
  if( usmDbDhcpsNumberToPoolNameGet(L7_UNIT_CURRENT, keyOptionIndexValue, poolName) != L7_SUCCESS)
  {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  /* If get operation for this poolName-optionCode combination is successful, then
     this row-entry of pool-option table is validated */
  if (usmDbDhcpsOptionGet(L7_UNIT_CURRENT, poolName, (L7_uchar8)keyOptionCodeValue, strOptionData,
                          &intDataLen, &dataFormat, &status) != L7_SUCCESS)
 {
    owa.rc = XLIBRC_FAILURE;    /* TODO: Change if required */
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if(((L7_uint32)status == L7_DHCPS_NOT_READY) && ((L7_uint32)dataFormat !=  L7_DHCPS_NONE))
  {
    owa.rc = XLIBRC_DHCP_POOL_OPTIONS_MUST_SPECIFY;   
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  if(((L7_uint32)status == L7_DHCPS_ACTIVE) && ((L7_uint32)dataFormat !=  L7_DHCPS_IP))
  {
    owa.rc = XLIBRC_DHCP_POOL_OPTIONS_NOT_ALLOWED; 
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }

  currentDataLen = owa.len;
  if((intDataLen + currentDataLen) > L7_DHCPS_POOL_OPTION_LENGTH)
  {
    owa.rc = XLIBRC_DHCP_POOL_OPTIONS_LENGTH_EXCEEDED; 
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  else
  {
    memcpy(&strOptionData[intDataLen], objIpAddressDataValue, currentDataLen);
    intDataLen += currentDataLen;
  }

  owa.l7rc = usmDbDhcpsOptionSet(L7_UNIT_CURRENT,poolName, 
                                 (L7_uchar8)keyOptionCodeValue,
                                  strOptionData,
                                  intDataLen,
                                  L7_DHCPS_IP);
  if (owa.l7rc != L7_SUCCESS)
  {
    owa.rc = XLIBRC_DHCP_POOL_OPTIONS_FAILED;    
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}
/*******************************************************************************
* @function fpObjGet_baseDhcpServerPoolOption_Status
*
* @purpose Get 'Status'
*
* @description [Status]: This specifies the row-status of the DHCP Server
*              Pool option. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjGet_baseDhcpServerPoolOption_Status (void *wap, void *bufp)
{
  fpObjWa_t kwaOptionIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyOptionIndexValue;
  fpObjWa_t kwaOptionCode = FPOBJ_INIT_WA (sizeof (xLibU8_t));
  xLibU8_t keyOptionCodeValue;
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStatusValue;
  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve key: OptionIndex */
  kwaOptionIndex.rc =
    xLibFilterGet (wap, XOBJ_baseDhcpServerPoolOption_OptionIndex,
                   (xLibU8_t *) & keyOptionIndexValue, &kwaOptionIndex.len);
  if (kwaOptionIndex.rc != XLIBRC_SUCCESS)
  {
    kwaOptionIndex.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaOptionIndex);
    return kwaOptionIndex.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyOptionIndexValue, kwaOptionIndex.len);

  /* retrieve key: OptionCode */
  kwaOptionCode.rc =
    xLibFilterGet (wap, XOBJ_baseDhcpServerPoolOption_OptionCode,
                   (xLibU8_t *) &keyOptionCodeValue, &kwaOptionCode.len);
  if (kwaOptionCode.rc != XLIBRC_SUCCESS)
  {
    kwaOptionCode.rc = XLIBRC_FILTER_MISSING;
    FPOBJ_TRACE_EXIT (bufp, kwaOptionCode);
    return kwaOptionCode.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyOptionCodeValue, kwaOptionCode.len);

  /* get the value from application */
  objStatusValue = L7_ROW_STATUS_ACTIVE;

  /* return the object value: Status */
  owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objStatusValue,
                           sizeof (objStatusValue));
  FPOBJ_TRACE_EXIT (bufp, owa);
  return owa.rc;
}


/*******************************************************************************
* @function fpObjSet_baseDhcpServerPoolOption_Status
*
* @purpose Set 'Status'
*
* @description [Status]: This specifies the row-status of the DHCP Server
*              Pool option. 
*
* @return
*******************************************************************************/
xLibRC_t fpObjSet_baseDhcpServerPoolOption_Status (void *wap, void *bufp)
{
  fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t objStatusValue;
  fpObjWa_t kwaOptionIndex = FPOBJ_INIT_WA (sizeof (xLibU32_t));
  xLibU32_t keyOptionIndexValue;
  fpObjWa_t kwaOptionCode = FPOBJ_INIT_WA (sizeof (xLibU8_t));
  xLibU8_t keyOptionCodeValue;
  L7_uchar8 prevFormat, newFormat=0;
  L7_uchar8 status;
  L7_char8  prevData[L7_DHCPS_POOL_OPTION_LENGTH+1];
  L7_uint32 prevLength;
  xLibStr256_t poolName ;

  memset(poolName, 0x00, sizeof(poolName));

  FPOBJ_TRACE_ENTER (bufp);

  /* retrieve object: Status */
  owa.rc = xLibBufDataGet (bufp, (xLibU8_t *) & objStatusValue, &owa.len);
  if (owa.rc != XLIBRC_SUCCESS)
  {
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
  }
  FPOBJ_TRACE_VALUE (bufp, &objStatusValue, owa.len);

  /* retrieve key: OptionIndex */
  kwaOptionIndex.rc =
    xLibFilterGet (wap, XOBJ_baseDhcpServerPoolOption_OptionIndex,
                   (xLibU8_t *) & keyOptionIndexValue, &kwaOptionIndex.len);
  if (kwaOptionIndex.rc != XLIBRC_SUCCESS)
  {
    /* This Filter will fail if page is loaded and delete is pressed in this case we now that the index should be the first Index*/
     owa.rc = XLIBRC_FILTER_MISSING;
     FPOBJ_TRACE_EXIT (bufp, owa);
     return owa.rc;
  }
  FPOBJ_TRACE_CURRENT_KEY (bufp, &keyOptionIndexValue, kwaOptionIndex.len);

  /* call the usmdb only for add and delete */
  if (objStatusValue == L7_ROW_STATUS_ACTIVE)
  {
      owa.rc = XLIBRC_SUCCESS;  /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
  }

	 /* retrieve key: OptionCode */
	kwaOptionCode.rc =
		xLibFilterGet (wap, XOBJ_baseDhcpServerPoolOption_OptionCode,
									 (xLibU8_t *) &keyOptionCodeValue, &kwaOptionCode.len);
	if (kwaOptionCode.rc != XLIBRC_SUCCESS)
	{
		kwaOptionCode.rc = XLIBRC_FILTER_MISSING;
		FPOBJ_TRACE_EXIT (bufp, kwaOptionCode);
		return kwaOptionCode.rc;
	}
	FPOBJ_TRACE_CURRENT_KEY (bufp, &keyOptionCodeValue, kwaOptionCode.len);

  if (objStatusValue == L7_ROW_STATUS_CREATE_AND_GO)
  {     
      /* Getting the pool name corresponding to this pool Index */
	  if( usmDbDhcpsNumberToPoolNameGet(L7_UNIT_CURRENT, keyOptionIndexValue, poolName) != L7_SUCCESS)
	  {
		   owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
	      FPOBJ_TRACE_EXIT (bufp, owa);
	      return owa.rc;
	  }

	  owa.l7rc  = usmDbDhcpsOptionCreate(L7_UNIT_CURRENT, poolName, (L7_uchar8)keyOptionCodeValue);

	  if (owa.l7rc == L7_ALREADY_CONFIGURED) 
	  {
      owa.l7rc = usmDbDhcpsOptionGet(L7_UNIT_CURRENT, poolName, (L7_uchar8)keyOptionCodeValue, prevData, 
                                      &prevLength, &prevFormat, &status);
       if(owa.l7rc == L7_SUCCESS)
       {
         if(newFormat == prevFormat)
         {
            if((L7_uint32)prevFormat == L7_DHCPS_ASCII)
            {
              owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
              FPOBJ_TRACE_EXIT (bufp, owa);
              return owa.rc;
            }
            else if((L7_uint32)prevFormat == L7_DHCPS_HEX)
            {
              owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
              FPOBJ_TRACE_EXIT (bufp, owa);
              return owa.rc;
            }
            else if((L7_uint32)prevFormat == L7_DHCPS_IP)
            {
              owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
              FPOBJ_TRACE_EXIT (bufp, owa);
              return owa.rc;
            }
         }
       }
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
  else if (objStatusValue == L7_ROW_STATUS_DESTROY)
  {
    /* Delete the existing row */

    memset(poolName,0x00,sizeof(poolName));

    /* Getting the pool name corresponding to this pool Index */
    if( usmDbDhcpsNumberToPoolNameGet(L7_UNIT_CURRENT,keyOptionIndexValue, poolName) != L7_SUCCESS)
    {
      owa.rc = XLIBRC_FAILURE;  /* TODO: Change if required */
      FPOBJ_TRACE_EXIT (bufp, owa);
      return owa.rc;
    }


    owa.l7rc = usmDbDhcpsOptionDelete (L7_UNIT_CURRENT, poolName,
                                keyOptionCodeValue);
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

/* Empty Function for compilation , as PoolName is now a pseudo-key */
xLibRC_t fpObjSet_baseDhcpServerPoolOption_PoolName (void *wap, void *bufp)
{
   return XLIBRC_SUCCESS;
}
