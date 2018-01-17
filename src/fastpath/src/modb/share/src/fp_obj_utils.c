/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename fp_obj_utils.c
 *
 * @purpose
 *
 * @component object handlers
 *
 * @comments
 *
 * @create  30 May 2007
 *
 * @author  Rama Sasthri, Kristipati
 * @end
 *
 **********************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "xlib.h"
#include "commdefs.h"
#include "nimapi.h"
#include "usmdb_util_api.h"
#include "dot1q_exports.h"
#include "fpobj_util.h"
#include "usmdb_dot1s_api.h"
#include "usmdb_nim_api.h"
#include "usmdb_mib_vlan_api.h"
#include "timezone_exports.h"
#include "config_script_api.h"
#include "comm_structs.h"

const L7_uint32 daysInAMonth[] = {0, 31,28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};


xLibRC_t fpObjAppGetU32ValueWithUnit (void *wap, void *bufp, usmDbU32ValGetWithUnit_t usmdbFunc,
                                      L7_uint32 unit)
{
  L7_uint32 val;

  if (usmdbFunc (unit, &val) != L7_SUCCESS)
  {
    return XLIBRC_APP_FAIL;
  }
  return xLibBufDataSet (bufp, (xLibU8_t *) & val, sizeof (val));
}

xLibRC_t fpObjAppSetU32ValueWithUnit (void *wap, void *bufp, usmDbU32ValSetWithUnit_t usmdbFunc,
                                      L7_uint32 unit)
{
  L7_uint32 val;
  xLibU16_t len = sizeof (val);

  if (xLibBufDataGet (bufp, (xLibU8_t *) & val, &len) != XLIBRC_SUCCESS)
  {
    return XLIBRC_FAILURE;
  }

  if (usmdbFunc (unit, val) != L7_SUCCESS)
  {
    return XLIBRC_APP_FAIL;
  }
  return XLIBRC_SUCCESS;
}

xLibRC_t fpObjAppGetU32Value (void *wap, void *bufp, usmDbU32ValGet_t usmdbFunc)
{
  L7_uint32 val;

  if (usmdbFunc (&val) != L7_SUCCESS)
  {
    return XLIBRC_APP_FAIL;
  }
  return xLibBufDataSet (bufp, (xLibU8_t *) & val, sizeof (val));
}

xLibRC_t fpObjAppSetU32Value (void *wap, void *bufp, usmDbU32ValSet_t usmdbFunc)
{
  L7_uint32 val;
  xLibU16_t len = sizeof (val);

  if(xLibBufDataGet (bufp, (xLibU8_t *) & val, &len) != XLIBRC_SUCCESS)
  {
    return XLIBRC_FAILURE;
  }

  if (usmdbFunc (val) != L7_SUCCESS)
  {
    return XLIBRC_APP_FAIL;
  }
  return XLIBRC_SUCCESS;
}

xLibRC_t fpObjAppGetU32KeyU32Value (void *wap, void *bufp, xLibId_t keyId,
                                    usmDbU32KeyU32ValGet_t usmdbFunc)
{
  xLibU32_t keyVal;
  L7_uint32 val;
  xLibU16_t len = sizeof (keyVal);

  if (XLIBRC_SUCCESS != xLibFilterGet (wap, keyId, (xLibU8_t *) & keyVal, &len))
  {
    return XLIBRC_FILTER_MISSING;
  }

  if (usmdbFunc (keyVal, &val) != L7_SUCCESS)
  {
    return XLIBRC_APP_FAIL;
  }
  return xLibBufDataSet (bufp, (xLibU8_t *) & val, sizeof (val));
}

xLibRC_t fpObjAppSetU32KeyU32Value (void *wap, void *bufp, xLibId_t keyId,
                                    usmDbU32KeyU32ValSet_t usmdbFunc)
{
  xLibU32_t keyVal;
  L7_uint32 val;
  xLibU16_t len = sizeof (keyVal);

  if (XLIBRC_SUCCESS != xLibFilterGet (wap, keyId, (xLibU8_t *) & keyVal, &len))
  {
    return XLIBRC_FILTER_MISSING;
  }

  len = sizeof (val);
  if(xLibBufDataGet (bufp, (xLibU8_t *) & val, &len) != XLIBRC_SUCCESS)
  {
    return XLIBRC_FAILURE;
  }
  if (usmdbFunc (keyVal, val) != L7_SUCCESS)
  {
    return XLIBRC_APP_FAIL;
  }
  return XLIBRC_SUCCESS;
}

xLibRC_t fpObjAppGetU32KeyU32ValueWithUnit (void *wap, void *bufp, xLibId_t keyId,
                                            usmDbU32KeyU32ValGetWithUnit_t usmdbFunc,
                                            L7_uint32 unit)
{
  xLibU32_t keyVal;
  L7_uint32 val;
  xLibU16_t len = sizeof (keyVal);

  if (XLIBRC_SUCCESS != xLibFilterGet (wap, keyId, (xLibU8_t *) & keyVal, &len))
  {
    return XLIBRC_FILTER_MISSING;
  }

  if (usmdbFunc (unit, keyVal, &val) != L7_SUCCESS)
  {
    return XLIBRC_APP_FAIL;
  }
  return xLibBufDataSet (bufp, (xLibU8_t *) & val, sizeof (val));
}

xLibRC_t fpObjAppSetU32KeyU32ValueWithUnit (void *wap, void *bufp, xLibId_t keyId,
                                            usmDbU32KeyU32ValSetWithUnit_t usmdbFunc,
                                            L7_uint32 unit)
{
  xLibU32_t keyVal;
  L7_uint32 val;
  xLibU16_t len = sizeof (keyVal);

  if (XLIBRC_SUCCESS != xLibFilterGet (wap, keyId, (xLibU8_t *) & keyVal, &len))
  {
    return XLIBRC_FILTER_MISSING;
  }

  len = sizeof (val);
  if (xLibBufDataGet (bufp, (xLibU8_t *) & val, &len) != XLIBRC_SUCCESS)
  {
    return XLIBRC_FAILURE;
  }
  if (usmdbFunc (unit, keyVal, val) != L7_SUCCESS)
  {
    return XLIBRC_APP_FAIL;
  }
  return XLIBRC_SUCCESS;
}

xLibRC_t fpObjAppGetKey_PhyicalInterface (void *wap, void *bufp, xLibId_t oid)
{
  xLibU32_t intIfNum;
  xLibU32_t nextIntf;
  xLibU16_t len = sizeof (intIfNum);
  L7_RC_t rv;

  if (XLIBRC_NO_FILTER == xLibFilterGet (wap, oid, (xLibU8_t *) & intIfNum, &len))
  {
    rv = usmDbValidIntIfNumFirstGet (&nextIntf);
  }
  else
  {
    rv = usmDbNextIntIfNumberByTypeGet (L7_PHYSICAL_INTF, intIfNum, &nextIntf);
  }

  if (rv != L7_SUCCESS)
  {
    return XLIBRC_ENDOF_TABLE;
  }
  return xLibBufDataSet (bufp, (xLibU8_t *) & nextIntf, sizeof (nextIntf));
}

xLibRC_t fpObjAppGetKey_PhysicalLagInterface (void *wap, void *bufp, xLibId_t oid)
{
  xLibU32_t intIfNum;
  xLibU32_t nextIntf;
  xLibU16_t len = sizeof (intIfNum);
  xLibU32_t nextInterf;
  xLibU32_t ifType;

  if (XLIBRC_NO_FILTER == xLibFilterGet (wap, oid, (xLibU8_t *) & intIfNum, &len))
  {
    if (L7_SUCCESS == usmDbValidIntIfNumFirstGet (&intIfNum))
    {
      xLibBufDataSet (bufp, (xLibU8_t *) & intIfNum, sizeof (intIfNum));
      return XLIBRC_SUCCESS;
    }
    else
    {
      return XLIBRC_ENDOF_TABLE;
    }
  }
  else
  {
    if (L7_SUCCESS == usmDbValidIntIfNumNext (intIfNum, &nextIntf))
    {
      if (usmDbIntfTypeGet (nextIntf, &ifType) == L7_SUCCESS)
      {
        if (ifType == L7_CPU_INTF)
        {
          if (usmDbValidIntIfNumNext (nextIntf, &nextInterf) == L7_SUCCESS)
          {
            xLibBufDataSet (bufp, (xLibU8_t *) & nextInterf, sizeof (nextInterf));
            return XLIBRC_SUCCESS;
          }
          return XLIBRC_ENDOF_TABLE;
        }
      }
      xLibBufDataSet (bufp, (xLibU8_t *) & nextIntf, sizeof (nextIntf));
      return XLIBRC_SUCCESS;
    }
    return XLIBRC_ENDOF_TABLE;
  }
  /*should not come here */
  return XLIBRC_SUCCESS;
}

xLibRC_t fpObjAppGetKey_Dot1sInstance (void *wap, void *bufp, xLibId_t oid)
{
  L7_RC_t rv;
  xLibU32_t mstId;
  xLibU32_t nextId;
  xLibU16_t bufLen = sizeof (mstId);

  if (XLIBRC_NO_FILTER == xLibFilterGet (wap, oid, (xLibU8_t *) & mstId, &bufLen))
  {
    rv = usmDbDot1sInstanceFirstGet (L7_USMDB_UNIT_ZERO, &nextId);
  }
  else
  {
    rv = usmDbDot1sInstanceNextGet (L7_USMDB_UNIT_ZERO, mstId, &nextId);
  }

  if (rv != L7_SUCCESS)
  {
    return XLIBRC_ENDOF_TABLE;
  }

  return xLibBufDataSet (bufp, (xLibU8_t *) & nextId, sizeof (nextId));
}

xLibRC_t fpObjAppGetKey_VlanInstance (void *wap, void *bufp, xLibId_t oid)
{
  xLibU32_t vlanId;
  xLibU16_t bufLen = sizeof (vlanId);
  xLibU32_t nextVID;
  L7_RC_t rv;

  if (XLIBRC_NO_FILTER == xLibFilterGet (wap, oid, (xLibU8_t *) & vlanId, &bufLen))
  {
    nextVID = L7_DOT1Q_DEFAULT_VLAN;
    rv = L7_SUCCESS;
  }
  else
  {
    rv = usmDbNextVlanGet (0, vlanId, &nextVID);
  }

  if (rv != L7_SUCCESS)
  {
    return XLIBRC_ENDOF_TABLE;
  }

  return xLibBufDataSet (bufp, (xLibU8_t *) & nextVID, sizeof (nextVID));
}

xLibRC_t fpObjAppGetKey_U32Number (void *wap, void *bufp, xLibId_t oid, L7_uint32 min,
                                   L7_uint32 max, L7_uint32 inc)
{
  L7_uint32 val;
  xLibU16_t len = sizeof (val);

  if (XLIBRC_NO_FILTER == xLibFilterGet (wap, oid, (xLibU8_t *) & val, &len))
  {
    val = min;
  }
  else
  {
    val += inc;
  }

  if (val > max)
  {
    return XLIBRC_ENDOF_TABLE;
  }

  return xLibBufDataSet (bufp, (xLibU8_t *) & val, sizeof (val));
}

int fpObjPortFromMaskGet (L7_INTF_MASK_t intfmask)
{
  int i = 0;
  int j = 0;

  for (i = 0; i < sizeof (intfmask); i++)
  {
    for (j = 0; j < sizeof (char) * 8; j++)
    {
      if ((intfmask.value[i] & (0x01 << j)) != 0x0)
      {
        return i * 8 + j + 1;
      }
    }
  }
  return -1;
}

int fpObjNextIntfGet (L7_INTF_MASK_t * intfMask, xLibS32_t prevPort)
{
  int i = 0;
  int j = 0;
  for (i = (prevPort) / 8; i < sizeof (L7_INTF_MASK_t); i++)
  {
    for (j = (prevPort) % 8, prevPort = 0; j < sizeof (char) * 8; j++)
    {
      if ((intfMask->value[i] & (0x01 << j)) != 0)
      {
        return i * 8 + j + 1;
      }
    }
  }
  return -1;
}

L7_BOOL fpObjIsStackingSupported (void)
{
#ifdef L7_STACKING_PACKAGE
  return L7_TRUE;
#else
  return L7_FALSE;
#endif
}

L7_RC_t fpObjScanUSP (char *str, L7_uint32 * unit, L7_uint32 * slot, L7_uint32 * port)
{
  *unit = 1;

  if (fpObjIsStackingSupported ())
  {
    sscanf (str, "%d/%d/%d\n", unit, slot, port);
  }
  else
  {
    sscanf (str, "%d/%d\n", slot, port);
  }
  return L7_SUCCESS;
}

L7_RC_t fpObjPrintUSP (char *str, L7_uint32 size, L7_uint32 unit, L7_uint32 slot, L7_uint32 port)
{
  if (fpObjIsStackingSupported ())
  {
    sprintf (str, "%d/%d/%d", unit, (xLibU32_t) slot, port);
  }
  else
  {
    sprintf (str, "%d/%d", (xLibU32_t) slot, port);
  }
  return L7_SUCCESS;
}

L7_RC_t fpObjUtil__ValidateSummerTime(L7_uint32 stMonth,
                              L7_uint32 stDay, L7_uint32 stYear, L7_uint32 stWeek,
                              L7_uint32 endMonth, L7_uint32 endDay,
                              L7_uint32 endYear, L7_uint32 endWeek, L7_BOOL recurFlag)
{
  if (recurFlag == L7_FALSE)
  {

    if (((stDay == L7_LEAP_YEAR_DAY && stMonth == L7_MONTH_FEB && L7_LEAP_YEAR_CHECK(stYear)) ||
         ((stMonth >= 1 && stDay <= daysInAMonth [stMonth]) &&
          (stDay >= 1 && stDay <= daysInAMonth [stMonth]) &&
          (stYear >= L7_START_SUMMER_TIME_YEAR && stYear <= L7_END_SUMMER_TIME_YEAR))) &&
        ((endDay == L7_LEAP_YEAR_DAY && endMonth == L7_MONTH_FEB && L7_LEAP_YEAR_CHECK(endYear)) ||
         ((endMonth >= 1 && endMonth <= daysInAMonth [endMonth]) &&
          (endDay >= 1 && endDay <= daysInAMonth [endMonth]) &&
          (endYear >= L7_START_SUMMER_TIME_YEAR && endYear <= L7_END_SUMMER_TIME_YEAR))))
    {
      return L7_SUCCESS;
    }
  }
  else if ((stWeek >= L7_WEEK_CNT_MIN && stWeek <= L7_WEEK_CNT_MAX) &&
      (stDay >= L7_WEEK_DAY_CNT_MIN && stDay <= L7_WEEK_DAY_CNT_MAX) &&
      (stMonth >= L7_MONTH_CNT_MIN && stMonth <= L7_MONTH_CNT_MAX) &&
      (endWeek >= L7_WEEK_CNT_MIN && endWeek <= L7_WEEK_CNT_MAX) &&
      (endDay >= L7_WEEK_DAY_CNT_MIN && endDay <= L7_WEEK_DAY_CNT_MAX) &&
      (endMonth >= L7_MONTH_CNT_MIN && endMonth <= L7_MONTH_CNT_MAX))
  {
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}


/* Parser Routine to extract time Stamp from a given log message */
L7_RC_t extractTime(L7_uchar8 *logBuff,L7_uchar8 *buf,L7_uint32 buffMsgLen)
{
  L7_uchar8 tokenBuf[L7_LOG_MESSAGE_LENGTH+1],delim[2] = " ";
  L7_uchar8 *strTokPtr;

  if ((logBuff == NULL) || logBuff[0] != '<')
    return L7_FAILURE;

  memset(tokenBuf,0x00,sizeof(tokenBuf));

  /* Search until you reach the Date String */
  sprintf(tokenBuf,"%s",logBuff);
  strTokPtr = (char *)strtok(tokenBuf,delim);

  if((strTokPtr == L7_NULL) ||(buffMsgLen < strlen(strTokPtr)))
    return L7_FAILURE;
  strTokPtr = (char *)strtok(NULL,delim);

  if(strTokPtr == L7_NULL)
    return L7_FAILURE;

  strcat(buf,strTokPtr);
  strTokPtr = (char *)strtok(NULL,delim);

  if((strTokPtr == L7_NULL) ||(buffMsgLen < strlen(strTokPtr)))
    return L7_FAILURE;

  strcat(buf,delim);
  strcat(buf,strTokPtr);

  strTokPtr = (char *)strtok(NULL,delim);

  if((strTokPtr == L7_NULL) ||(buffMsgLen < strlen(strTokPtr)))
    return L7_FAILURE;

  strcat(buf,delim);
  strcat(buf,strTokPtr);

  return L7_SUCCESS;
} 
/* Parser Routine to extract message from a given log message */
L7_RC_t extractLogDesc(L7_uchar8 *logBuff,L7_uchar8 *buffMsg,L7_uint32 buffMsgLen)
{
  L7_uchar8 *buffPointer;

  if ((logBuff == NULL) || logBuff[0] != '<')
    return L7_FAILURE;

  buffPointer = strstr(logBuff,"%% ");
  if(buffPointer == NULL)
    return L7_FAILURE;

  buffPointer = buffPointer+3;
  if(buffMsgLen < strlen(buffPointer))
    return L7_FAILURE;
  strncpy(buffMsg,buffPointer,strlen(buffPointer));
  return L7_SUCCESS;
}

/* Parser Routine to extract messaage severity from a given log message */
L7_RC_t extractSeverity(L7_uchar8 *logBuff,L7_uint32 *severity)
{
  L7_uchar8 buff1[10];

  if ((logBuff == NULL) || logBuff[0] != '<')
    return L7_FAILURE;

  memset(buff1,0x00,10);
  strncpy(buff1,logBuff+1,3);

  *severity = atoi(buff1);
  *severity %= 8;
  return L7_SUCCESS;
}

L7_RC_t extractComponent(L7_uchar8 *logBuff,L7_uchar8 *destBuff)
{

  L7_uchar8 tokenBuf[L7_LOG_MESSAGE_LENGTH+1],delim[2] = " ";

  L7_uchar8 *strTokPtr,*searchCompDelim = NULL;
  L7_uint32 rc = L7_FAILURE;

  if ((logBuff == NULL) || logBuff[0] != '<')
    return L7_FAILURE;

  memset(tokenBuf,0x00,sizeof(tokenBuf));

  /* Search until you reach the component */
  sprintf(tokenBuf,"%s",logBuff);

  strTokPtr = (L7_uchar8 *)strtok(tokenBuf,delim);
  while((searchCompDelim == NULL )&& (strTokPtr != NULL))
  {
    strTokPtr = (L7_uchar8 *)strtok(NULL,delim);

    if(strTokPtr != NULL)
    {
      searchCompDelim = strstr(strTokPtr,"[");
    }
  }

  if((searchCompDelim != NULL )|| (strTokPtr != NULL))
  {
    strncpy(destBuff,strTokPtr,(sizeof(L7_uchar8))*(searchCompDelim - strTokPtr));
    rc = L7_SUCCESS;
  }
  else
  {
    rc  = L7_FAILURE;
  }
  return rc;
}

xLibDouble64_t power(L7_double64 x,L7_double64 y)
{
  xLibU32_t ctr = 1;
  xLibU32_t  sign = 0;
  xLibDouble64_t sum = 0;

  if(y == 0)
  {
    return 1;
  }
  if(y < 0)
  {
    y = -1 * y;
    sign = 1;
    sum = 1.0/x;
  }
  else
  {
    sign = 0;
    sum = x;
  }

  while( ctr < y)
  {
    if(sign == 1)
    {
      sum = sum * (1.0/x);
    }
    else
    {
      sum *= x;
    }
    ctr++;
  }
  return sum;
}

xLibRC_t strToHex(xLibU8_t* ptr,xLibU8_t* value)
{
  xLibU8_t i = 0;
  xLibU8_t  sum = 0;

  if((ptr == NULL) || (value == NULL) )
    return XLIBRC_FAILURE;

  for(i = 0 ;i < strlen(ptr);i++ )
  {
    switch(ptr[i])
    {
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        sum = sum+((ptr[i]-'0')*(power(16,(1-i))));
        break;
      case 'A':
      case 'a':
      case 'B':
      case 'b':
      case 'C':
      case 'c':
      case 'D':
      case 'd':
      case 'E':
      case 'e':
      case 'F':
      case 'f':
        sum = sum+((toupper(ptr[i])-'A'+0x0A)*(power(16,(1-i))));
        break;
      default:
        return XLIBRC_FAILURE;

    }
  }
  *value = sum;
  return XLIBRC_SUCCESS;
}

L7_RC_t stripQuotes(xLibU8_t*  inputString )
{
   int i=0, j =0;
   while ( inputString[i] != '\0' )
   {
     if ( inputString[i] != '"' )
     {
       inputString[j] = inputString[i];
       j++;
     }
     i++;
   }
   inputString[j] = '\0';
   return L7_SUCCESS;
}

/**************************************************************************
*
* @purpose  Creates a text file from config script
*
* @param    script_file_name   @b{(input)} name of file to create
*
* @returns  0 if successful
* @returns  1 if file can not be opened
*
* @end
*
*************************************************************************/
L7_RC_t configScriptCopy(L7_char8 *sourceFile)
{
  L7_char8          tempConfigScriptBuffer[CONFIG_SCRIPT_MAX_COMMAND_SIZE];
  L7_uint32         inFiledesc, outFiledesc;
  L7_int32          charsRead;
  
  if (osapiFsOpen(sourceFile, &inFiledesc) == L7_ERROR)
  {
    /* osapiFsClose(inFiledesc); */
    return (L7_FAILURE);
  }

   /*remove temporary file in case it still exists*/
   osapiFsDeleteFile(TEMP_CONFIG_SCRIPT_FILE_NAME);

	/*create temp file*/
  if ( L7_SUCCESS != osapiFsFileCreate(TEMP_CONFIG_SCRIPT_FILE_NAME, &outFiledesc) )
  {
	  osapiFsClose(inFiledesc);
	  return (L7_FAILURE);
  } 
      
  if (osapiFileRead(inFiledesc, tempConfigScriptBuffer, sizeof(L7_fileHdr_t) + CONFIG_SCRIPT_DELIMITER_LEN) != L7_ERROR)
  {
     charsRead = sizeof(tempConfigScriptBuffer) - 1;
     while ( ( L7_ERROR != osapiFileReadWithLen(inFiledesc, tempConfigScriptBuffer, &charsRead) ) &&
            ( charsRead > 0 ) )
     {
       osapiFsWriteNoClose(outFiledesc, tempConfigScriptBuffer, charsRead);
       charsRead = sizeof(tempConfigScriptBuffer) - 1;
     }
  }
  else
  {
    osapiFsClose(inFiledesc);
    osapiFsClose(outFiledesc);
    return L7_FAILURE;
  }
  
  osapiFsClose(inFiledesc);
  osapiFsClose(outFiledesc);
  return(L7_SUCCESS);
}

