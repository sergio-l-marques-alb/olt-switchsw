/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename init.c
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
 *          Madan Mohan Goud, Kanala
 * @end
 *
 **********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "xlib.h"
#include "commdefs.h"
#include "nimapi.h"
#include "usmdb_util_api.h"
#include "usmdb_common.h"
#include "usmdb_nim_api.h"
#include "dot1q_exports.h"
#include "cli_web_exports.h"
#include "dhcps_exports.h"
#include "snmp_exports.h"
#include "usmdb_sim_api.h"

#include "osapi_support.h"
#include "l3_commdefs.h"
#include "l7utils_inet_addr_api.h"
#ifndef L7_PRODUCT_SMARTPATH
#include "web.h"
#endif
#include "sflow_exports.h"

#ifdef L7_DOT3AH_PACKAGE
#include "dot3ah_exports.h"
#endif
#ifdef L7_DOT1AG_PACKAGE
#include "dot1ag_exports.h"
#endif
#include "usmdb_rlim_api.h"
#include "xlib_protect.h"

#include "fpobj_util.h"

#define IPV6_ADDR_SIZE               39 /* 128 bit IPv6 Address 1111:2222:...:8888 */

#ifdef L7_CAPTIVE_PORTAL_PACKAGE
char *ewaUrlCpHook (void* context, char *url,char* dsturl);
#endif
extern L7_RC_t emwebUrlRegisterHook(char* srcurl,void* func);


#ifdef _L7_OS_LINUX_
extern int osapiWebFileHandler (int op, char *filename, void **handle, char *data, int length);
#endif
extern xLibRC_t xLibDecode_ipv4 (xLibIf_t intf, xLibS8_t * in, xLibU8_t * out, xLibU16_t * outSizeLen);
extern xLibRC_t xLibEncode_ipv4 (xLibIf_t intf, xLibU8_t * in, xLibU16_t inLen, char *out, xLibU16_t * outLen);

extern void fpObjRangeHandlersInit (void);
extern L7_RC_t usmDbIntfTypeGet(L7_uint32 intIfNum, L7_uint32 *sysIntfType);
extern  xLibRC_t strToHex(xLibU8_t* ptr,xLibU8_t* value);

extern L7_RC_t usmDbIntfTypeGet(L7_uint32 intIfNum, L7_uint32 *sysIntfType);
extern xLibU16_t xLibUtilHexDecode (xLibS8_t * in, xLibU8_t * out, xLibU16_t maxCount, xLibS8_t sep);
extern xLibRC_t xLibSpecialCharValGet(xLibU8_t *in, xLibU16_t inLen, char *out);

/*********************************************************************
 *
 * @purpose convert the provided char array into a 32 bit unsigned integer
 *          the value is >= 0 and <= 4294967295.
 *
 * @param L7_char8 *buf, L7_uint32 * pVal
 *
 * @returns  L7_SUCCESS  means that all chars are integers and together
 *              they represent a valid 32 bit unsigned integer
 * @returns  L7_FAILURE  means the value does not represent a valid
 *              32 bit unsigned integer.  I.e. the value is negative, larger
 *              than the max allowed 32 bit int or a non-numeric character
 *              is included in buf.
 *
 * @notes This f(x) checks each letter inside the buf to make certain
 *         it is an integer.  It initially verifies the number of digits
 *         does not exceed the number of digits in Max 32 bit unsigned int.
 *         Start with the last digit, convert each ascii character into its
 *         integer equivalent.  Multiply each consecutive digit by the next
 *         power of 10.  Verify adding the new digit to the old sum will not
 *         exceed MAXINT.  If so, this indicates the value is too large to be
 *         represented by a 32 bit int.  If ok, add the new digit.
 *
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cliWebXuiConvertTo32BitUnsignedInteger (const L7_char8 * buf, L7_uint32 * pVal)
{
  L7_int32 f;
  L7_int32 j;
  L7_uint32 tempval;
#define MAXUINT 4294967295U
#define MAXMSB 4

  if (buf[0] == '-')
  {
    return L7_FAILURE;
  }

  j = strlen (buf);

  if (j > 10)
  {
    return L7_FAILURE;
  }

  if ((strlen (buf) == 10))
  {
    tempval = (L7_uint32) (buf[0] - '0');
    if (tempval > MAXMSB)
    {
      return L7_FAILURE;
    }
  }

  for (*pVal = 0, f = 1, j = j - 1; j >= 0; j--, f = f * 10)
  {
    tempval = (L7_uint32) (buf[j] - '0');

    if ((tempval > 9) || (tempval < 0))
    {
      return L7_FAILURE;
    }

    tempval = tempval * f;

    if ((MAXUINT - tempval) < *pVal)
    {
      return L7_FAILURE;
    }
    else
    {
      *pVal = *pVal + tempval;
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose convert the provided char array in HEX form into a 32 bit unsigned integer
 *          the value is >= 0 and <= 0xffffffff.
 *
 * @param L7_char8 *buf, L7_uint32 * pVal
 *
 * @returns  L7_SUCCESS  means that all chars are HEX integers and together
 *              they represent a valid 32 bit unsigned integer
 * @returns  L7_FAILURE  means the value does not represent a valid
 *              32 bit unsigned integer.  I.e. the value is negative, larger
 *              than the max allowed 32 bit int or a non-hex character
 *              is included in buf.
 *
 * @notes
 *
 *
 * @end
 *
 *********************************************************************/
L7_RC_t cliWebXuiConvertHexTo32BitUnsignedInteger(const L7_char8 *buf, L7_uint32 *pVal)
{

  L7_uint32 f;
  L7_int32 j;
  L7_uint32 tempval;


  if ( buf[0] == '-' )
    return L7_FAILURE;

  j = strlen(buf);
  if ( j > 10 )
    return L7_FAILURE;

  *pVal=0;

  for(f=1,j=j-1;j>=2;j--,f=f*16)
  {
    if((buf[j]>='0') && (buf[j]<='9'))
      tempval = (L7_uint32)(buf[j] -'0');
    else if((buf[j]>='a') && (buf[j]<='z'))
      tempval = (L7_uint32)((buf[j] -'a')+10);
    else if((buf[j]>='A') && (buf[j]<='Z'))
      tempval = (L7_uint32)((buf[j] -'A')+10);
    else
      return L7_FAILURE;

    tempval = tempval * f;
    *pVal = *pVal + tempval;
  }
  return L7_SUCCESS;
}

/*********************************************************************
 * @purpose  used for laging, creates a char array of slot.ports included in lag
 *
 * @param    buf         contains ascii string "xx:xx:xx:xx:xx:xx"
 * @param    macLength   provides the length of the MAC address (different between IVL/SVL)
 * @param    mac         return hex data
 *
 * @returns void
 *
 * @notes
 *
 * @end
 *********************************************************************/
static L7_BOOL cliConvertMac (L7_uchar8 * buf, L7_uchar8 * mac)
{
  L7_uint32 i, j, digit_count = 0;
  L7_uchar8 mac_address[L7_CLI_MAX_STRING_LENGTH];

  if (strlen (buf) != 17)
  {                             /* test string length */
    return L7_FALSE;
  }

  for (i = 0, j = 0; i < 17; i++, j++)
  {
    digit_count++;
    switch (buf[i])
    {
      case '0':
        mac_address[j] = 0x0;
        break;
      case '1':
        mac_address[j] = 0x1;
        break;
      case '2':
        mac_address[j] = 0x2;
        break;
      case '3':
        mac_address[j] = 0x3;
        break;
      case '4':
        mac_address[j] = 0x4;
        break;
      case '5':
        mac_address[j] = 0x5;
        break;
      case '6':
        mac_address[j] = 0x6;
        break;
      case '7':
        mac_address[j] = 0x7;
        break;
      case '8':
        mac_address[j] = 0x8;
        break;
      case '9':
        mac_address[j] = 0x9;
        break;
      case 'a':
      case 'A':
        mac_address[j] = 0xA;
        break;
      case 'b':
      case 'B':
        mac_address[j] = 0xB;
        break;
      case 'c':
      case 'C':
        mac_address[j] = 0xC;
        break;
      case 'd':
      case 'D':
        mac_address[j] = 0xD;
        break;
      case 'e':
      case 'E':
        mac_address[j] = 0xE;
        break;
      case 'f':
      case 'F':
        mac_address[j] = 0xF;
        break;
      case ':':
        if (digit_count != 3)
        {                         /* if more or less than 2 digits return false */
          return L7_FALSE;
        }
        j--;
        digit_count = 0;
        break;
      default:
        return L7_FALSE;
        break;
    }
  }

  for (i = 0; i < 6; i++)
  {
    mac[i] = ((mac_address[(i * 2)] << 4) + mac_address[(i * 2) + 1]);
  }
  return L7_TRUE;
}
/*Encoding routine for user type OUIVal_t */
static xLibRC_t encode_OUIVal_t (xLibIf_t intf, xLibU8_t * inBuf, xLibU16_t inLen,
                                     xLibS8_t * outBuf, xLibU16_t * outSizeLen)
{
  osapiSnprintf (outBuf, *outSizeLen, "%02X:%02X:%02X", inBuf[0], inBuf[1], inBuf[2]);
  *outSizeLen = strlen (outBuf);
  return XLIBRC_SUCCESS;
}

/* Decoding routine for user type OUIVal_t */
static xLibRC_t decode_OUIVal_t (xLibIf_t intf, xLibS8_t * inBuf, xLibU8_t * outBuf,
                                     xLibU16_t * outSizeLen)
{
  *outSizeLen = xLibUtilHexDecode (inBuf, outBuf, sizeof(L7_OUIVAL_LEN), ':');
  return XLIBRC_SUCCESS;
}

/*Encoding routine for user type sFlowIndex_t */
static xLibRC_t encode_sFlowIndex_t (xLibIf_t intf, xLibU8_t * inBuf, xLibU16_t inLen,
    xLibS8_t * outBuf, xLibU16_t * outSizeLen)
{
  nimUSP_t usp;
  xLibU32_t ifIndex;

  memset (outBuf, 0x0, sizeof (outBuf));
  if (inBuf == NULL)
  {
    outBuf = NULL;
    *outSizeLen = 0;
    return XLIBRC_SUCCESS;
  }
  /* if request is from SNMP, no need to do encode/ decode */
  if(intf == XLIB_IF_SNMP)
  {

    osapiStrncpySafe(outBuf, inBuf, strlen(inBuf));
    *outSizeLen = strlen (outBuf);

  }

  else
  {
    ifIndex = atoi(inBuf+(sizeof(L7_SFLOW_IFINDEX)));
    if (nimGetUnitSlotPort (ifIndex, &usp) == L7_SUCCESS)
    {
      fpObjPrintUSP (outBuf, *outSizeLen, usp.unit, (xLibU32_t) usp.slot, usp.port);
      *outSizeLen = strlen (outBuf);
    }
    else
    {
      return XLIBRC_FAILURE;
    }
  }
  return XLIBRC_SUCCESS;
}

/* Decoding routine for user type sFlowIndex_t */
static xLibRC_t decode_sFlowIndex_t (xLibIf_t intf, xLibS8_t * inBuf, xLibU8_t * outBuf,
    xLibU16_t * outSizeLen)
{
  xLibU32_t intIfNum;
  xLibU32_t unit = 1;
  xLibU32_t slot;
  xLibU32_t port;
  xLibU8_t temp[20]; 

  fpObjScanUSP (inBuf, &unit, &slot, &port);
  memset (outBuf, 0x0, sizeof (outBuf));
  memset(temp, L7_EOS, sizeof(temp));

  /* if request is from SNMP, no need to do encode/ decode */
  if(intf == XLIB_IF_SNMP)
  {

    osapiStrncpySafe(outBuf, inBuf, strlen(inBuf));
    *outSizeLen = strlen(inBuf);

  }
  else
  {
    if (usmDbIntIfNumFromUSPGet (unit, slot, port, &intIfNum) == L7_SUCCESS)
    {
      osapiSnprintf(temp,sizeof(temp), "%s.%d",L7_SFLOW_IFINDEX, intIfNum);
      *outSizeLen = strlen(temp);
      osapiStrncpySafe(outBuf, temp, sizeof(temp)); 
    }
    else
    {
      return XLIBRC_FAILURE;
    }
  }
  return XLIBRC_SUCCESS;
}





/* Encoding routine for user type usp_t */
static xLibRC_t encode_usp_t (xLibIf_t intf, xLibU8_t * inBuf, xLibU16_t inLen,
    xLibS8_t * outBuf, xLibU16_t * outSizeLen)
{
  nimUSP_t usp;
  xLibU32_t intIfNum;

  switch (intf)
  {
    case XLIB_IF_SNMP: 
      return xLibEncode_uint (intf, inBuf, inLen, outBuf, outSizeLen);
      break;
    default: 
      /*  memset (outBuf, 0x0, sizeof (outBuf)); */
      memcpy (&intIfNum, inBuf, sizeof (intIfNum));
      if (intIfNum != L7_ALL_INTERFACES)
      {
        if (nimGetUnitSlotPort (intIfNum, &usp) == L7_SUCCESS)
        {
          fpObjPrintUSP (outBuf, *outSizeLen, usp.unit, (xLibU32_t) usp.slot, usp.port);
          *outSizeLen = strlen (outBuf);
          return XLIBRC_SUCCESS;
        }
      }
      break;
  }

  return XLIBRC_FAILURE;
}

/* Encoding routine for user type usplist_t */
static xLibRC_t encode_usplist_t (xLibIf_t intf, xLibU8_t * inBuf, xLibU16_t inLen,
    xLibS8_t * outBuf, xLibU16_t * outSizeLen)
{
  nimUSP_t usp;
  xLibU32_t intIfNum;
  xLibU32_t flag = 1;
  xLibU32_t lineFlag = 0;
  xLibU32_t size = 1;
  xLibU8_t *temp;
  xLibS8_t tempBuf[100];

  memset (outBuf, 0x0, sizeof (outBuf));
  if (inBuf == NULL)
  {
    outBuf = NULL;
    *outSizeLen = 0;
    return XLIBRC_SUCCESS;
  }

  temp = strtok (inBuf, ",");

  while (temp != NULL)
  {
    sscanf (temp, "%d", &intIfNum);
    memset (tempBuf, 0x0, sizeof (tempBuf));
    size=1;
    if (nimGetUnitSlotPort (intIfNum, &usp) == L7_SUCCESS)
    {
      if (flag == 1)
      {
        fpObjPrintUSP (tempBuf, sizeof(tempBuf), usp.unit, usp.slot, usp.port);
        lineFlag++;
        flag = 0;
      }
      else
      {
        tempBuf[0] = ',';
        if (fpObjIsStackingSupported ())
        {
          if(lineFlag == 2)
          {
            tempBuf[1] = '\n';
            size=2;
            lineFlag=0;
          }
        }
        else
        {
          if(lineFlag == 3)
          {
            tempBuf[1] = '\n';
            size=2;
            lineFlag=0;
          }
        }
        fpObjPrintUSP (&tempBuf[size], sizeof(tempBuf) - size, usp.unit, usp.slot, usp.port);
        lineFlag++;
      }
    }
    else
    {
      return XLIBRC_FAILURE;
    }
    strcat (outBuf, tempBuf);

    temp = strtok (NULL, ",");
  }

  *outSizeLen = strlen (outBuf);
  return XLIBRC_SUCCESS;
}

/* Decoding routine for user type usp_t */
static xLibRC_t decode_usp_t (xLibIf_t intf, xLibS8_t * inBuf, xLibU8_t * outBuf,
    xLibU16_t * outSizeLen)
{
  xLibU32_t intIfNum;
  xLibU32_t unit = 1;
  xLibU32_t slot=0;
  xLibU32_t port=0;

  switch (intf)
  { 
    case XLIB_IF_SNMP :
      return xLibDecode_uint (intf, inBuf, outBuf, outSizeLen);
    default : /* WEB & CLI */
      if (L7_SUCCESS == fpObjScanUSP (inBuf, &unit, &slot, &port))
      {
        if (usmDbIntIfNumFromUSPGet (unit, slot, port, &intIfNum) == L7_SUCCESS)
        {
          memcpy (outBuf, &intIfNum, sizeof (intIfNum));
          *outSizeLen = sizeof (intIfNum);
          return XLIBRC_SUCCESS;
        }
      }
      break;
  }
  return XLIBRC_FAILURE;
}

/* Decoding routine for user type usplist_t */
static xLibRC_t decode_usplist_t (xLibIf_t intf, xLibS8_t * inBuf, xLibU8_t * outBuf,
    xLibU16_t * outSizeLen)
{
  xLibU32_t intIfNum;
  xLibU32_t unit;
  xLibU32_t slot;
  xLibU32_t port;
  xLibU32_t flag = 1;
  xLibU8_t *temp;
  xLibS8_t tempBuf[100];

  memset (outBuf, 0x0, sizeof (outBuf));

  if (inBuf == NULL)
  {
    outBuf = NULL;
    *outSizeLen = 0;
    return XLIBRC_SUCCESS;
  }

  temp = strtok (inBuf, ",");

  while (temp != NULL)
  {
    fpObjScanUSP (temp, &unit, &slot, &port);
    memset (tempBuf, 0x0, sizeof (tempBuf));

    if (usmDbIntIfNumFromUSPGet (unit, slot, port, &intIfNum) == L7_SUCCESS)
    {
      if (flag == 1)
      {
        sprintf (tempBuf, "%d", intIfNum);
        flag = 0;
      }
      else
      {
        sprintf (tempBuf, ",%d", intIfNum);
      }

    }
    else
    {
      return XLIBRC_FAILURE;
    }
    strcat (outBuf, tempBuf);

    temp = strtok (NULL, ",");
  }

  *outSizeLen = strlen (outBuf);
  return XLIBRC_SUCCESS;
}
static xLibRC_t decode_hex_t (xLibIf_t intf, xLibS8_t * inBuf, xLibU8_t * outBuf,
    xLibU16_t * outSizeLen)
{
  L7_uint32 value;
  (void)usmDbConvertXstrtoi(inBuf, &value);

  memcpy (outBuf, &value, sizeof (value));
  *outSizeLen = sizeof (value);

  return XLIBRC_SUCCESS;
}

static xLibRC_t encode_hex_t (xLibIf_t intf, xLibU8_t * inBuf, xLibU16_t inLen,
    xLibS8_t * outBuf, xLibU16_t * outSizeLen)
{
  L7_uint32 value;

  memset (outBuf, 0x0, sizeof (outBuf));
  memcpy (&value, inBuf, sizeof (value));

  sprintf(outBuf,"0x%X",value);
  *outSizeLen = strlen (outBuf);

  return XLIBRC_SUCCESS;

}

static xLibRC_t decode_hex_uint_t (xLibIf_t intf, xLibS8_t * inBuf, xLibU8_t * outBuf,
    xLibU16_t * outSizeLen)
{
  L7_uint32 protoIdValue;

  if((inBuf[0]=='0') && ((inBuf[1]=='x') || (inBuf[1]=='X')))
  {
    if (cliWebXuiConvertHexTo32BitUnsignedInteger(inBuf,
          &protoIdValue) != L7_SUCCESS)
    {
      return XLIBRC_FAILURE;
    }

    memcpy (outBuf, &protoIdValue, sizeof (protoIdValue));
    *outSizeLen = sizeof (protoIdValue);
  }
  else
  {
    if (cliWebXuiConvertTo32BitUnsignedInteger(inBuf,
          (L7_int32 *)&protoIdValue) != L7_SUCCESS)
    {
      return XLIBRC_FAILURE;
    }
    memcpy (outBuf, &protoIdValue, sizeof (protoIdValue));
    *outSizeLen = sizeof (protoIdValue);
  }

  return XLIBRC_SUCCESS;
}

static xLibRC_t encode_hex_uint_t (xLibIf_t intf, xLibU8_t * inBuf, xLibU16_t inLen,
    xLibS8_t * outBuf, xLibU16_t * outSizeLen)
{
  L7_uint32 protoIdValue;

  memset (outBuf, 0x0, sizeof (outBuf));
  memcpy (&protoIdValue, inBuf, sizeof (protoIdValue));

  sprintf(outBuf,"0x%X",protoIdValue);
  *outSizeLen = strlen (outBuf);

  return XLIBRC_SUCCESS;

}




/* Encoding routine for user type usplist_t */
static xLibRC_t encode_vlanIDMac_t (xLibIf_t intf, xLibU8_t * inBuf, xLibU16_t inLen,
    xLibS8_t * outBuf, xLibU16_t * outSizeLen)
{

  memset (outBuf, 0x0, sizeof (outBuf));
  if (inBuf == NULL)
  {
    outBuf = NULL;
    *outSizeLen = 0;
    return XLIBRC_SUCCESS;
  }

  /*By default it will combindly comes to input. Just we need to send as is output */
  osapiStrncat (outBuf, inBuf, strlen (inBuf));

  *outSizeLen = strlen (outBuf);

  return XLIBRC_SUCCESS;

}

/* Decoding routine for user type usplist_t */
static xLibRC_t decode_vlanIDMac_t (xLibIf_t intf, xLibS8_t * inBuf, xLibU8_t * outBuf,
    xLibU16_t * outSizeLen)
{

  xLibU32_t vlanID, len;
  xLibU8_t *vidBuf;
  xLibS8_t *macBuf;
  xLibS8_t strMacAddr[L7_ENET_MAC_ADDR_LEN];
  xLibS8_t buf[256];

  memset (outBuf, 0x0, sizeof (outBuf));

  if (inBuf == NULL)
  {
    outBuf = NULL;
    *outSizeLen = 0;
    return XLIBRC_SUCCESS;
  }
  vidBuf = NULL;
  vidBuf = strtok (inBuf, "-");

  if (vidBuf == NULL)
  {
    return XLIBRC_FAILURE;
  }

  /*Check given VLAN id is in the valid range or not */
  if (cliWebXuiConvertTo32BitUnsignedInteger (vidBuf, &vlanID) != L7_SUCCESS)
  {
    return XLIBRC_FAILURE;
  }
  else
  {
    if ((vlanID < L7_DOT1Q_MIN_VLAN_ID) || (vlanID > L7_DOT1Q_MAX_VLAN_ID))
    {
      return XLIBRC_FAILURE;
    }
  }

  macBuf = strtok (NULL, "-");

  OSAPI_STRNCPY_SAFE (buf, macBuf);

  memset (strMacAddr, 0x0, sizeof (strMacAddr));

  if (cliConvertMac (buf, strMacAddr) != L7_TRUE)
  {
    return XLIBRC_FAILURE;
  }

  osapiStrncat (outBuf, vidBuf, strlen (vidBuf));

  osapiStrncat (outBuf, "-", strlen ("-"));

  len = strlen (outBuf);

  outBuf[len] = strMacAddr[0];
  outBuf[len + 1] = strMacAddr[1];
  outBuf[len + 2] = strMacAddr[2];
  outBuf[len + 3] = strMacAddr[3];
  outBuf[len + 4] = strMacAddr[4];
  outBuf[len + 5] = strMacAddr[5];

  *outSizeLen = len + L7_ENET_MAC_ADDR_LEN;

  return XLIBRC_SUCCESS;

}
 
static xLibRC_t encode_string_t (xLibIf_t intf, xLibU8_t * in, xLibU16_t inLen, char *out,
    xLibU16_t * outSizeLen)
{
  xLibU16_t tmp_len = 0;

  if (intf == XLIB_IF_WEB)
  {
    if (xLibSpecialCharValGet(in, inLen, out) == XLIBRC_SUCCESS)
    {
      tmp_len = strlen(out);
    }
  }

  out[tmp_len] = 0;
  *outSizeLen = tmp_len;
  return XLIBRC_SUCCESS;
}


static xLibRC_t decode_string_t (xLibIf_t intf, xLibS8_t * in, xLibU8_t * out, xLibU16_t * outSizeLen)
{
  xLibU16_t len = strlen (in);
  osapiStrncpy ((xLibS8_t *) out, in, len);
  *outSizeLen = len + 1;        /* account the terminating null also */
  out[len] = 0;
  return XLIBRC_SUCCESS;
}

/* Encoding routine for user type uspGlobal_t */
static xLibRC_t encode_uspGlobal_t (xLibIf_t intf, xLibU8_t * inBuf, xLibU16_t inLen,
    xLibS8_t * outBuf, xLibU16_t * outSizeLen)
{
  nimUSP_t usp;
  /* Here the input will be an intIfNum, where as output will be a string which is an USP or "Global" */
  xLibU32_t intIfNum;
  memcpy (&intIfNum, inBuf, sizeof (intIfNum));
  if (intIfNum == L7_ALL_INTERFACES)
  {
    strcpy (outBuf, "Global");
    *outSizeLen = strlen (outBuf);
  }
  else
  {
    if (nimGetUnitSlotPort (intIfNum, &usp) == L7_SUCCESS)
    {
      fpObjPrintUSP (outBuf, *outSizeLen, (xLibU32_t) usp.unit, (xLibU32_t) usp.slot, usp.port);
      *outSizeLen = strlen (outBuf);
    }
    else
    {
      return XLIBRC_FAILURE;
    }
  }
  return XLIBRC_SUCCESS;
}

/* Decoding routine for user type uspGlobal_t */
static xLibRC_t decode_uspGlobal_t (xLibIf_t intf, xLibS8_t * inBuf, xLibU8_t * outBuf,
    xLibU16_t * outSizeLen)
{
  xLibU32_t intIfNum;
  xLibU32_t unit = 1;
  xLibU32_t slot;
  xLibU32_t port;

  /* Here the input will be an USP, where as output will be an intIfNum */

  if (!strcmp (inBuf, "Global"))
  {
    intIfNum = L7_ALL_INTERFACES;
    memcpy (outBuf, &intIfNum, sizeof (intIfNum));
    *outSizeLen = sizeof (intIfNum);
  }
  else
  {
    fpObjScanUSP (inBuf, &unit, &slot, &port);
    if (usmDbIntIfNumFromUSPGet (unit, slot, port, &intIfNum) == L7_SUCCESS)
    {
      memcpy (outBuf, &intIfNum, sizeof (intIfNum));
      *outSizeLen = sizeof (intIfNum);
    }
    else
    {
      return XLIBRC_FAILURE;
    }
  }
  return XLIBRC_SUCCESS;
}

/* Encoding routine for user type uspRouteNextHop_t */
static xLibRC_t encode_uspRouteNextHop_t (xLibIf_t intf, xLibU8_t * inBuf, xLibU16_t inLen,
    xLibS8_t * outBuf, xLibU16_t * outSizeLen)
{
  /* Here the input will be an intIfNum, where as output will be a string which is an USP or "Null0" */
  nimUSP_t usp;
  xLibU32_t intIfNum, cpuIntf, rc;

  memcpy (&intIfNum, inBuf, sizeof (intIfNum));

  rc = usmDbMgtSwitchintIfNumGet(0, &cpuIntf);

  if(rc == L7_SUCCESS)
  {
    if(intIfNum == L7_INVALID_INTF)
    {
      /* If nexthop interface is zero, it means the nexthop interface is not resolved */

      strcpy (outBuf, "Unresolved");
      *outSizeLen = strlen (outBuf);
    }
    else if(cpuIntf == intIfNum)
    {
      /* In case of reject route the next hop interface is the CPU interface */

      strcpy (outBuf, "Null0");
      *outSizeLen = strlen (outBuf);
    }
    else if (nimGetUnitSlotPort (intIfNum, &usp) == L7_SUCCESS)
    {
      fpObjPrintUSP (outBuf, *outSizeLen, (xLibU32_t) usp.unit, (xLibU32_t) usp.slot, usp.port);
      *outSizeLen = strlen (outBuf);
    }
  }
  else
  {
    return XLIBRC_FAILURE;
  }

  return XLIBRC_SUCCESS;
}

/* Decoding routine for user type uspRouteNextHop_t */
static xLibRC_t decode_uspRouteNextHop_t(xLibIf_t intf, xLibS8_t * inBuf, xLibU8_t * outBuf,
    xLibU16_t * outSizeLen)
{
  xLibU32_t intIfNum, cpuIntf;
  xLibU32_t unit = 1;
  xLibU32_t slot;
  xLibU32_t port;

  /* Here the input will be an USP, where as output will be an intIfNum */

  if (strcmp (inBuf, "Unresolved") == 0)
  {
    intIfNum = L7_INVALID_INTF;
    memcpy (outBuf, &intIfNum, sizeof (intIfNum));
    *outSizeLen = sizeof (intIfNum);
  }
  else if (strcmp (inBuf, "Null0") == 0)
  {
    if(usmDbMgtSwitchintIfNumGet(0, &cpuIntf) == L7_SUCCESS)
    {
      intIfNum = cpuIntf;
      memcpy (outBuf, &intIfNum, sizeof (intIfNum));
      *outSizeLen = sizeof (intIfNum);
    }
    else
    {
      return XLIBRC_FAILURE;
    }
  }
  else
  {
    fpObjScanUSP (inBuf, &unit, &slot, &port);
    if (usmDbIntIfNumFromUSPGet (unit, slot, port, &intIfNum) == L7_SUCCESS)
    {
      memcpy (outBuf, &intIfNum, sizeof (intIfNum));
      *outSizeLen = sizeof (intIfNum);
    }
    else
    {
      return XLIBRC_FAILURE;
    }
  }
  return XLIBRC_SUCCESS;
}

/* Encoding routine for user type uspNone_t */
static xLibRC_t encode_uspNone_t (xLibIf_t intf, xLibU8_t * inBuf, xLibU16_t inLen,
    xLibS8_t * outBuf, xLibU16_t * outSizeLen)
{
  nimUSP_t usp;

  /* Here the input will be an intIfNum, where as output will be a string which is an USP or "Global" */

  xLibU32_t intIfNum;

  memcpy (&intIfNum, inBuf, sizeof (intIfNum));
  if (intIfNum == 0)
  {
    strcpy (outBuf, "None");
    *outSizeLen = strlen (outBuf);
  }
  else
  {
    if (nimGetUnitSlotPort (intIfNum, &usp) == L7_SUCCESS)
    {
      fpObjPrintUSP (outBuf, *outSizeLen, (xLibU32_t) usp.unit, (xLibU32_t) usp.slot, usp.port);
      *outSizeLen = strlen (outBuf);
    }
    else
    {
      return XLIBRC_FAILURE;
    }
  }
  return XLIBRC_SUCCESS;
}

/* Decoding routine for user type uspNone_t */
static xLibRC_t decode_uspNone_t (xLibIf_t intf, xLibS8_t * inBuf, xLibU8_t * outBuf,
    xLibU16_t * outSizeLen)
{
  xLibU32_t intIfNum;
  xLibU32_t unit = 1;
  xLibU32_t slot;
  xLibU32_t port;

  /* Here the input will be an USP, where as output will be an intIfNum */
  if (!strcmp (inBuf, "None"))
  {
    intIfNum = 0;
    memcpy (outBuf, &intIfNum, sizeof (intIfNum));
    *outSizeLen = sizeof (intIfNum);
  }
  else
  {
    fpObjScanUSP (inBuf, &unit, &slot, &port);
    if (usmDbIntIfNumFromUSPGet (unit, slot, port, &intIfNum) == L7_SUCCESS)
    {
      memcpy (outBuf, &intIfNum, sizeof (intIfNum));
      *outSizeLen = sizeof (intIfNum);
    }
    else
    {
      return XLIBRC_FAILURE;
    }
  }
  return XLIBRC_SUCCESS;
}

/* Encoding routine for user type vidMac */
static xLibRC_t xLibAppvidMacEncode (xLibIf_t intf, xLibU8_t * inBuf, xLibU16_t inLen,
    xLibS8_t * outBuf, xLibU16_t * outSizeLen)
{
  /* Here input will be a combination of VANID and MAC Address
   * without having each byte seperated by ':'.
   * Now, we will encode it to this Format.
   */
  sprintf (outBuf, "%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X", inBuf[0], inBuf[1], inBuf[2],
      inBuf[3], inBuf[4], inBuf[5], inBuf[6], inBuf[7]);
  *outSizeLen = strlen (outBuf);
  return XLIBRC_SUCCESS;
}

static xLibRC_t xLibAppsnmpMacEncode (xLibIf_t intf, xLibU8_t * inBuf, xLibU16_t inLen,
    xLibS8_t * outBuf, xLibU16_t * outSizeLen)
{
  memcpy (outBuf, inBuf, L7_MAC_ADDR_LEN);
  *outSizeLen = L7_MAC_ADDR_LEN + 1;
  return XLIBRC_SUCCESS;
}

static xLibRC_t xLibAppsnmpMacDecode (xLibIf_t intf, xLibS8_t * inBuf, xLibU8_t * outBuf,
    xLibU16_t * outSizeLen)
{
  return XLIBRC_FAILURE;
}



/* Decoding routine for user type uspGlobal_t */
static xLibRC_t xLibAppvidMacDecode (xLibIf_t intf, xLibS8_t * inBuf, xLibU8_t * outBuf,
    xLibU16_t * outSizeLen)
{
  xLibU16_t inLen = strlen (inBuf);
  xLibU8_t mac_address[8];
  xLibU8_t input_address[24];
  xLibU8_t* temp;
  xLibS8_t* saveptr;
  xLibU8_t  i = 0;
  xLibU8_t  hexValue = 0;
  /* Here input will be a combination of VLANID and MAC Address with each byte seperated by a ':'.
   * Now, we will remove ':' signs which are present after each byte and send the resultant string to application.
   * This is the format which is understood by application
   */


  if (inLen < 23)
  {
    return XLIBRC_FAILURE;
  }
  memset(mac_address,0x0,sizeof(mac_address));
  memset(input_address,0x0,sizeof(input_address));
  memcpy(input_address,inBuf,inLen);
  temp = strtok_r(input_address,":",&saveptr);
  i = 0;
  while(temp != NULL)
  {
    if((strlen(temp)!=2) || strToHex(temp,&hexValue)==XLIBRC_FAILURE)
    {
      return XLIBRC_FAILURE;
    }
    mac_address[i++] = hexValue;
    temp = strtok_r(NULL,":",&saveptr);
  }
  memcpy(outBuf,mac_address,sizeof(mac_address)); 
  *outSizeLen = sizeof(mac_address);
  return XLIBRC_SUCCESS;
}

static xLibRC_t ipv4_ipv6_Encode (xLibIf_t intf, xLibU8_t * inBuf, xLibU16_t inLen,
    xLibS8_t * outBuf, xLibU16_t * outSizeLen)
{
  L7_inet_addr_t temp;
  
  inetAddressReset(&temp);
  memset(outBuf,0,sizeof(outBuf));  
  if (inetAddrNtoh((L7_inet_addr_t *) inBuf, &temp) != L7_SUCCESS)
  {
    return XLIBRC_FAILURE;
  }
  if (inetAddrHtop (&temp, outBuf) == L7_SUCCESS)
  {
    *outSizeLen = strlen (outBuf);
    return XLIBRC_SUCCESS;
  }
  else
  {
    return XLIBRC_FAILURE;
  }
}

static xLibRC_t ipv4_ipv6_Decode (xLibIf_t intf, xLibS8_t * inBuf, xLibU8_t * outBuf,
    xLibU16_t * outSizeLen)
{
  L7_inet_addr_t inet_address;

  if (usmDbParseInetAddrFromIPAddrHostNameStr (inBuf, &inet_address) == L7_SUCCESS)
  {
    memcpy (outBuf, (void *) &inet_address, sizeof (L7_inet_addr_t));
    *outSizeLen = sizeof (L7_inet_addr_t);
    return XLIBRC_SUCCESS;
  }
  else
  {
    return XLIBRC_FAILURE;
  }
}

static xLibRC_t ipv4_uint_Encode (xLibIf_t intf, xLibU8_t * inBuf, xLibU16_t inLen,
    xLibS8_t * outBuf, xLibU16_t * outSizeLen)
{
  L7_uint32 ipAddr;
  memcpy (&ipAddr, inBuf, sizeof (ipAddr));

  osapiInetNtoa(ipAddr, outBuf);
  *outSizeLen = strlen (outBuf);
  return XLIBRC_SUCCESS;
}

static xLibRC_t ipv4_uint_Decode (xLibIf_t intf, xLibS8_t * inBuf, xLibU8_t * outBuf,
    xLibU16_t * outSizeLen)
{

  L7_uint32 ipAddress, length, i;
  L7_BOOL flag = L7_FALSE;

  memset (outBuf, 0, sizeof (outBuf));
  length = strlen(inBuf);
  if (length == 0)
  {
     return XLIBRC_FAILURE;
    
  }
  for(i=0; i < length; i++)
  {
    if(inBuf[i] == '.')
    {
      flag = L7_TRUE;
      break;
    }
  }

  if(flag == L7_TRUE)
  {   
    if (usmDbInetAton ((xLibU8_t *) inBuf, (void *) outBuf) == L7_SUCCESS)
    {
      memcpy (&ipAddress, outBuf, sizeof (ipAddress));
      *outSizeLen = sizeof(ipAddress);
      return XLIBRC_SUCCESS;
    }
    else
    {
      return XLIBRC_FAILURE;
    }
  }
  else
  {
    if (cliWebXuiConvertTo32BitUnsignedInteger(inBuf, &ipAddress) != L7_SUCCESS)
    {
      return XLIBRC_FAILURE;
    }
    memcpy (outBuf, &ipAddress, sizeof (ipAddress));
    *outSizeLen = sizeof (ipAddress);
  }

  return XLIBRC_SUCCESS;

}

static xLibRC_t encode_dot3ahCap_t(xLibIf_t intf, xLibU8_t * inBuf, xLibU16_t inLen,
    xLibS8_t * outBuf, xLibU16_t * outSizeLen)
{
#ifdef L7_DOT3AH_PACKAGE
  xLibU8_t oamConfig = *inBuf;

  memset(outBuf, 0x00, sizeof(xLibStr256_t));

  if( L7_DOT3AH_CFG_REM_LB_SUPPORT & oamConfig )
    strcat(outBuf, "Remote Loopback<BR>");
  if( L7_DOT3AH_CFG_LINK_EVENT_SUPPORT & oamConfig )
    strcat(outBuf, "Link Event PDU<BR>");
  if( L7_DOT3AH_CFG_ORG_SPECIFIC_INFO_TLV & oamConfig )
    strcat(outBuf, "Org Spec Info TLV<BR>");
  if( L7_DOT3AH_CFG_ORG_SPECIFIC_EVENT & oamConfig )
    strcat(outBuf, "Org Spec Event TLV<BR>");
  if( L7_DOT3AH_CFG_ORG_SPECIFIC_PDU & oamConfig )
    strcat(outBuf, "Org Spec PDU");

  *outSizeLen = strlen(outBuf);
#endif

  return XLIBRC_SUCCESS;  

}

/* This is just read only object  Decode is just dummy*/
static xLibRC_t decode_dot3ahCap_t (xLibIf_t intf, xLibS8_t * inBuf, xLibU8_t * outBuf,
    xLibU16_t * outSizeLen)
{
  *outSizeLen = 0;
  return XLIBRC_SUCCESS;  
}


static xLibRC_t encode_dot3ahTime_t(xLibIf_t intf, xLibU8_t * inBuf, xLibU16_t inLen,
    xLibS8_t * outBuf, xLibU16_t * outSizeLen)
{
  usmDbTimeSpec_t *timeVal = (usmDbTimeSpec_t *)inBuf ;

  memset(outBuf, 0x00, sizeof(xLibStr256_t));

  osapiSnprintf(outBuf, sizeof(xLibStr256_t), "%XDays:%XHours:%XMins:%Xsecs", timeVal->days,timeVal->hours,timeVal->minutes,timeVal->seconds );
  *outSizeLen = strlen(outBuf);

  return XLIBRC_SUCCESS;  

}

static xLibRC_t decode_dot3ahTime_t (xLibIf_t intf, xLibS8_t * inBuf, xLibU8_t * outBuf,
    xLibU16_t * outSizeLen)
{
  *outSizeLen = 0;
  return XLIBRC_SUCCESS;  
}


static xLibRC_t encode_dot3ahOui_t(xLibIf_t intf, xLibU8_t * inBuf, xLibU16_t inLen,
    xLibS8_t * outBuf, xLibU16_t * outSizeLen)
{

  memset(outBuf, 0x00, sizeof(xLibStr256_t));
  osapiSnprintf(outBuf, sizeof(xLibStr256_t), "%02X:%02X:%02X", inBuf[0]&0xFF,inBuf[1]&0xFF,inBuf[2]&0xFF);
  *outSizeLen = strlen(outBuf);

  return XLIBRC_SUCCESS;

}

static xLibRC_t decode_dot3ahOui_t (xLibIf_t intf, xLibS8_t * inBuf, xLibU8_t * outBuf,
    xLibU16_t * outSizeLen)
{
  *outSizeLen = 0;
  return XLIBRC_SUCCESS;
}


static xLibRC_t encode_dot3ahDevVendorID_t(xLibIf_t intf, xLibU8_t * inBuf, xLibU16_t inLen,
    xLibS8_t * outBuf, xLibU16_t * outSizeLen)
{

  memset(outBuf, 0x00, sizeof(xLibStr256_t));
  osapiSnprintf(outBuf, sizeof(xLibStr256_t), "%02X:%02X", inBuf[0]&0xFF,inBuf[1]&0xFF);
  *outSizeLen = strlen(outBuf);

  return XLIBRC_SUCCESS;

}

static xLibRC_t decode_dot3ahDevVendorID_t(xLibIf_t intf, xLibS8_t * inBuf, xLibU8_t * outBuf,
    xLibU16_t * outSizeLen)
{
  *outSizeLen = 0;
  return XLIBRC_SUCCESS;
}

static xLibRC_t ipv4_ipv6_serial_Encode (xLibIf_t intf, xLibU8_t * inBuf, xLibU16_t inLen,
    xLibS8_t * outBuf, xLibU16_t * outSizeLen)
{

  L7_inet_addr_t aval;


  if(inLen != sizeof(L7_inet_addr_t) )
  {
    osapiStrncpy (outBuf, "--", strlen("--"));
    return XLIBRC_SUCCESS;
  }
  memcpy(&aval,inBuf,inLen);

  if ( aval.family == L7_AF_INET )
  {
    if (usmDbInetNtoa(osapiNtohl(aval.addr.ipv4.s_addr), outBuf) != L7_SUCCESS)
    {
      osapiStrncpySafe(outBuf, "unKnown", strlen("unKnown"));
    }
  }
  else
  {
    if (aval.family == L7_AF_INET6)
    {
      osapiInetNtop(L7_AF_INET6,(L7_uchar8 *)&aval.addr.ipv6, outBuf, 256);
    }
    else
    {
      osapiStrncpySafe(outBuf, "EIA-232", strlen("EIA-232"));
    }
  }
  *outSizeLen = strlen (outBuf);
  return XLIBRC_SUCCESS;

}

static xLibRC_t ipv4_ipv6_serial_Decode (xLibIf_t intf, xLibS8_t * inBuf, xLibU8_t * outBuf,
    xLibU16_t * outSizeLen)
{
  L7_inet_addr_t inet_address;

  if (usmDbParseInetAddrFromIPAddrHostNameStr (inBuf, &inet_address) == L7_SUCCESS)
  {
    memcpy (outBuf, (void *) &inet_address, sizeof (L7_inet_addr_t));
    *outSizeLen = sizeof (L7_inet_addr_t);
    return XLIBRC_SUCCESS;
  }
  else
  {
    return XLIBRC_FAILURE;
  }
}

static xLibRC_t encode_stats64_t (xLibIf_t intf, xLibU8_t * inBuf, xLibU16_t inLen,
    xLibS8_t * outBuf, xLibU16_t * outSizeLen)
{

  L7_ulong64 num;

  memcpy ((void *) &num, inBuf, sizeof (L7_ulong64));

  memset (outBuf, 0x00, 100);
  if (usmDb64BitsToString (num, outBuf) == L7_SUCCESS)
  {
    *outSizeLen = strlen (outBuf);
    return XLIBRC_SUCCESS;
  }
  else
  {
    return XLIBRC_FAILURE;
  }

}

static xLibRC_t decode_stats64_t (xLibIf_t intf, xLibS8_t * inBuf, xLibU8_t * outBuf,
    xLibU16_t * outSizeLen)
{
  return XLIBRC_SUCCESS;
}

static xLibRC_t encode_ipv6Prefix_t (xLibIf_t intf, xLibU8_t * in, xLibU16_t inLen, char *out,
    xLibU16_t * outLen)
{
  xLibS8_t buf[IPV6_ADDR_SIZE];
  L7_in6_prefix_t *prefix = (L7_in6_prefix_t *) in;

  osapiInetNtop (L7_AF_INET6, (char *) &(prefix->in6Addr.in6.addr8), buf, IPV6_ADDR_SIZE);
  sprintf (out, "%s/%d", buf, prefix->in6PrefixLen);
  *outLen = strlen (out);
  return XLIBRC_SUCCESS;
}

static xLibRC_t decode_ipv6Prefix_t (xLibIf_t intf, xLibS8_t * in, xLibU8_t * out,
    xLibU16_t * outSizeLen)
{
  xLibU32_t buf_len;
  xLibS8_t input[IPV6_DISP_ADDR_LEN + 4 + 1];
  xLibS8_t *p;
  xLibS8_t *plen;
  L7_in6_prefix_t prefix;

  buf_len = (L7_uint32) strlen (in);
  if ((buf_len >= sizeof (input)) || (buf_len == 0))
  {
    return XLIBRC_FAILURE;
  }

  osapiStrncpy (input, in, buf_len);

  p = input;
  plen = strstr (input, "/");

  if (plen == L7_NULL)
  {
    return XLIBRC_FAILURE;
  }
  plen++;

  prefix.in6PrefixLen = (L7_uint32) atol (plen);
  if (prefix.in6PrefixLen == 0)
  {
    return XLIBRC_FAILURE;
  }

  plen--;
  *plen = '\0';

  if (osapiInetPton (L7_AF_INET6, (xLibU8_t *) p, (xLibU8_t *) & prefix) != L7_SUCCESS)
  {
    return XLIBRC_FAILURE;
  }

  memcpy ((void *) out, (void *) (&prefix), sizeof (L7_in6_prefix_t));
  *outSizeLen = sizeof (L7_in6_prefix_t);

  return XLIBRC_SUCCESS;
}

static xLibRC_t encode_ospfv3_trap_flags_t (xLibIf_t intf, xLibU8_t * in,
    xLibU16_t inLen, char *out, xLibU16_t * outLen)
{
  xLibU32_t val;
  L7_uchar8 buf[256];

  bzero (buf, sizeof (buf));
  memcpy (&val, in, sizeof (val));

  if ((val & L7_OSPF_TRAP_ALL) == 0)
  {
    sprintf (buf, " %s", "Disabled");
  }
  else
  {
    if (val & L7_OSPF_TRAP_ERRORS_ALL)
    {
      if (val & L7_OSPF_TRAP_RX_BAD_PACKET)
      {
        sprintf (buf, " %s", "bad-packet");
      }

      if (val & L7_OSPF_TRAP_IF_CONFIG_ERROR)
      {
        sprintf (buf, " %s  %s", buf, "config-error");
      }

      if (val & L7_OSPF_TRAP_VIRT_IF_RX_BAD_PACKET)
      {
        sprintf (buf, " %s  %s", buf, "virt-bad-packet");
      }

      if (val & L7_OSPF_TRAP_VIRT_IF_CONFIG_ERROR)
      {
        sprintf (buf, " %s  %s", buf, "virt-config-error");
      }
    }                           /* endif checking enabled error flags */

    if (val & L7_OSPF_TRAP_IF_RX_ALL)
    {
      if (val & L7_OSPF_TRAP_IF_RX_PACKET)
      {
        sprintf (buf, " %s  %s", buf, "if-rx-packet");
      }
    }                           /* endif checking enabled if-rx  flags */
    if (val & L7_OSPF_TRAP_LSA_ALL)
    {
      if (val & L7_OSPF_TRAP_MAX_AGE_LSA)
      {
        sprintf (buf, " %s  %s", buf, "lsa-maxage");
      }
      if (val & L7_OSPF_TRAP_ORIGINATE_LSA)
      {
        sprintf (buf, " %s  %s", buf, "lsa-originate");
      }
    }                           /* endif checking enabled lsa flags */

    if (val & L7_OSPF_TRAP_OVERFLOW_ALL)
    {
      if (val & L7_OSPF_TRAP_LS_DB_OVERFLOW)
      {
        sprintf (buf, " %s  %s", buf, "lsdb-overflow");
      }
      if (val & L7_OSPF_TRAP_LS_DB_APPROACHING_OVERFLOW)
      {
        sprintf (buf, " %s  %s", buf, "lsdb-approaching-overflow");
      }
    }                           /* endif checking enabled overflow flags */

    if (val & L7_OSPF_TRAP_RETRANSMIT_ALL)
    {
      if (val & L7_OSPF_TRAP_TX_RETRANSMIT)
      {
        sprintf (buf, " %s  %s", buf, "packets");
      }
      if (val & L7_OSPF_TRAP_VIRT_IF_TX_RETRANSMIT)
      {
        sprintf (buf, " %s  %s", buf, "virt-packets");
      }
    }                           /* endif checking enabled retransmit flags */

    if (val & L7_OSPF_TRAP_RTB_ALL)
    {
      if (val & L7_OSPF_TRAP_RTB_ENTRY_INFO)
      {
        sprintf (buf, " %s  %s", buf, "rtb-entry-info");
      }
    }                           /* endif checking enabled RTB flags */

    if (val & L7_OSPF_TRAP_STATE_CHANGE_ALL)
    {
      if (val & L7_OSPF_TRAP_IF_STATE_CHANGE)
      {
        sprintf (buf, " %s  %s", buf, "if-state-change");
      }
      if (val & L7_OSPF_TRAP_NBR_STATE_CHANGE)
      {
        sprintf (buf, " %s  %s", buf, "neighbor-state-change");
      }
      if (val & L7_OSPF_TRAP_VIRT_IF_STATE_CHANGE)
      {
        sprintf (buf, " %s  %s", buf, "virtif-state-change");
      }
      if (val & L7_OSPF_TRAP_VIRT_NBR_STATE_CHANGE)
      {
        sprintf (buf, " %s  %s", buf, "virtneighbor-state-change");
      }
    }
  }

  sprintf (out, "%s", buf);
  *outLen = strlen (out);

  return XLIBRC_SUCCESS;
}

static xLibRC_t decode_ospfv3_trap_flags_t (xLibIf_t intf, xLibS8_t * in,
    xLibU8_t * out, xLibU16_t * outSizeLen)
{
  return XLIBRC_SUCCESS;
}

/* Encoding routine for user type formattedTimeDHMS_t  (elapsed time in days,Hours,minutes and seconds format)*/
static xLibRC_t encode_formattedTimeDHMS_t (xLibIf_t intf, xLibU8_t * inBuf, xLibU16_t inLen,
    xLibS8_t * outBuf, xLibU16_t * outSizeLen)
{
  xLibU32_t timeInSeconds;
  L7_timespec ts;

  if (inLen != sizeof (xLibU32_t))
  {
    return XLIBRC_FAILURE;
  }

  memcpy (&timeInSeconds, inBuf, sizeof (xLibU32_t)); /*Get the total no of seconds from inBuf*/

  osapiConvertRawUpTime(timeInSeconds, &ts);

  osapiSnprintf(outBuf, 256, "%d days, %d hours, %d minutes, %d secs", ts.days, ts.hours, ts.minutes, ts.seconds);

  *outSizeLen = strlen (outBuf);

  return XLIBRC_SUCCESS;
}
  
/* Decoding routine for user type formattedTimeDHMS_t  (elapsed time in days,Hours,minutes and seconds format) */
static xLibRC_t decode_formattedTimeDHMS_t (xLibIf_t intf, xLibS8_t * inBuf, xLibU8_t * outBuf,
    xLibU16_t * outSizeLen)
{

  return XLIBRC_SUCCESS;
}

/* Encoding routine for user type elapseTime_t  (elapsed time in days,Hours,minutes format)*/
static xLibRC_t encode_elapsedTime_t (xLibIf_t intf, xLibU8_t * inBuf, xLibU16_t inLen,
    xLibS8_t * outBuf, xLibU16_t * outSizeLen)
{
  xLibU32_t timeInSeconds;
  xLibU32_t days;
  xLibU32_t minutes;
  xLibU32_t hours;
  xLibU32_t seconds;	

  if (inLen != sizeof (xLibU32_t))
  {
    return XLIBRC_FAILURE;
  }

  memcpy (&timeInSeconds, inBuf, sizeof (xLibU32_t));
  memset (outBuf, 0, sizeof (outBuf));

  switch (intf)
  {
    case XLIB_IF_SNMP:
      return xLibEncode_uint (intf, inBuf, inLen, outBuf, outSizeLen);
      break;
    default:
      timeInSeconds = timeInSeconds / 100;  /*to get no of seconds */
      days    = timeInSeconds / 86400;
      timeInSeconds     = timeInSeconds % 86400;
      hours   = timeInSeconds / 3600;
      timeInSeconds     = timeInSeconds % 3600;
      minutes = timeInSeconds / 60;
      timeInSeconds = timeInSeconds % 60;
      seconds = timeInSeconds;

      sprintf (outBuf, "%d days, %d hours, %d mins, %d secs", days, hours, minutes, seconds);
      *outSizeLen = strlen (outBuf);
      break;
  }

  return XLIBRC_SUCCESS;
}

/* Decoding routine for user type elapseTime_t  (elapsed time in days,Hours,minutes format) */
static xLibRC_t decode_elapsedTime_t (xLibIf_t intf, xLibS8_t * inBuf, xLibU8_t * outBuf,
    xLibU16_t * outSizeLen)
{
  xLibU32_t timeInSeconds;
  xLibU32_t days;
  xLibU32_t minutes;
  xLibU32_t hours;
  xLibU32_t seconds;

  if (inBuf == NULL)
  {
    return XLIBRC_FAILURE;
  }

  switch (intf)
  {
    case XLIB_IF_SNMP:
      return xLibDecode_uint (intf, inBuf, outBuf, outSizeLen);
      break;
    default: /* WEB & CLI */
      sscanf (inBuf, "%d days, %d hours, %d mins %d secs", &days, &hours, &minutes, &seconds);
      timeInSeconds = days * 86400 + hours * 3600 + minutes * 60 + seconds;
      timeInSeconds *= 100;
      sprintf (outBuf, "%u", timeInSeconds);
      *outSizeLen = strlen (outBuf);
      break;
  }

  return XLIBRC_SUCCESS;
}

/* Encoding routine for user type elapsedTimeDHCP_t  (elapsed time in days,Hours,minutes format)*/
static xLibRC_t encode_elapsedTimeDHCP_t (xLibIf_t intf, xLibU8_t * inBuf, xLibU16_t inLen,
    xLibS8_t * outBuf, xLibU16_t * outSizeLen)
{
  xLibU32_t timeInSeconds;
  xLibU32_t days;
  xLibU32_t minutes;
  xLibU32_t hours;
  xLibU32_t seconds;
  xLibS8_t tempBufIn[128];

  if (inLen != sizeof (xLibU32_t))
  {
    return XLIBRC_FAILURE;
  }

  memcpy (&timeInSeconds, inBuf, sizeof (xLibU32_t));
  memset (outBuf, 0, sizeof (outBuf));

  days    = timeInSeconds / SECONDS_PER_DAY;
  timeInSeconds     = timeInSeconds % SECONDS_PER_DAY;
  hours   = timeInSeconds / SECONDS_PER_HOUR;
  timeInSeconds     = timeInSeconds % SECONDS_PER_HOUR;
  minutes = timeInSeconds / SECONDS_PER_MINUTE;
  timeInSeconds = timeInSeconds % SECONDS_PER_MINUTE;
  seconds = timeInSeconds;
	
  osapiSnprintf(tempBufIn, sizeof(tempBufIn), "%d days %02dh:%02dm:%02ds", days, hours, minutes, seconds); 
  osapiStrncat(outBuf, tempBufIn, strlen(tempBufIn));
  *outSizeLen = strlen (outBuf);

  return XLIBRC_SUCCESS;
}

/* Decoding routine for user type elapsedTimeDHCP_t (elapsed time in days,Hours,minutes format) */
static xLibRC_t decode_elapsedTimeDHCP_t (xLibIf_t intf, xLibS8_t * inBuf, xLibU8_t * outBuf,
    xLibU16_t * outSizeLen)
{

  return XLIBRC_SUCCESS;
}


/* Encoding routine for user type elapsedTimeDHCP_t  (elapsed time in days,Hours,minutes format)*/
static xLibRC_t encode_RemainingTimeDHCP_t (xLibIf_t intf, xLibU8_t * inBuf, xLibU16_t inLen,
    xLibS8_t * outBuf, xLibU16_t * outSizeLen)
{
  xLibU32_t RemainingTime;
  xLibU32_t days;
  xLibU32_t minutes;
  xLibU32_t hours;
  xLibS8_t tempBufIn[128];

  if (inLen != sizeof (xLibU32_t))
  {
    return XLIBRC_FAILURE;
  }

  memcpy (&RemainingTime, inBuf, sizeof (xLibU32_t));
  memset (outBuf, 0, sizeof (outBuf));
  memset (tempBufIn, 0, sizeof (tempBufIn));

  /* Get days from total lease time (in minutes) */
  RemainingTime = RemainingTime * 60;
  days                     = RemainingTime / 86400;
  RemainingTime    = RemainingTime % 86400;
  /* Get hours from total lease time (in minutes) */
  hours                    = RemainingTime / 3600;
  RemainingTime    = RemainingTime % 3600;
  minutes                  = RemainingTime / 60;

  osapiSnprintf(tempBufIn, sizeof(tempBufIn), "%02d:%02d:%02d", days, hours, minutes);
  osapiStrncat(outBuf, tempBufIn, strlen(tempBufIn));
  *outSizeLen = strlen (outBuf);

  return XLIBRC_SUCCESS;
}

/* Decoding routine for user type elapsedTimeDHCP_t (elapsed time in days,Hours,minutes format) */
static xLibRC_t decode_RemainingTimeDHCP_t (xLibIf_t intf, xLibS8_t * inBuf, xLibU8_t * outBuf,
    xLibU16_t * outSizeLen)
{

  return XLIBRC_SUCCESS;
}


/* Encoding routine for user type ipAddrList_t  */
static xLibRC_t encode_ipAddrList_t (xLibIf_t intf, xLibU8_t * inBuf, xLibU16_t inLen,
    xLibS8_t * outBuf, xLibU16_t * outSizeLen)
{
  xLibU32_t tempIPAddr = 0;
  xLibS8_t tempBufIn[25];
  xLibS8_t tempBufOut[25];
  xLibU32_t tempLen;
  xLibU32_t indx = 0;

  memset (outBuf, 0x00, *outSizeLen);
  if ((inBuf == NULL) || (inLen != 32))
  {
    outBuf = NULL;
    *outSizeLen = 0;
    return XLIBRC_FAILURE;
  }

  for (indx = 0; indx < 8; indx++)
  {
    /*get the IpAddress */
    memcpy (&tempIPAddr, inBuf + (indx * 4), sizeof (tempIPAddr));
    tempIPAddr = (xLibU32_t) osapiNtohl (tempIPAddr);

    memset (tempBufIn, 0x00, sizeof (tempBufIn));
    memset (tempBufOut, 0x00, sizeof (tempBufOut));

    memcpy (tempBufIn, &tempIPAddr, sizeof (tempIPAddr));
    osapiInetNtop (L7_AF_INET, tempBufIn, (void *) tempBufOut, sizeof (tempBufOut));
    tempLen = strlen (tempBufOut);
    if (indx == 0)
    {
      strncpy (outBuf, tempBufOut, tempLen);
      strcat (outBuf, ",");
    }
    else if (indx == 7)
    {
      strcat (outBuf, tempBufOut);
    }
    else
    {
      strcat (outBuf, tempBufOut);
      strcat (outBuf, ",");
    }
  }

  *outSizeLen = strlen (outBuf);
  return XLIBRC_SUCCESS;
}

/* Decoding routine for user type ipAddrList_t  */
static xLibRC_t decode_ipAddrList_t (xLibIf_t intf, xLibS8_t * inBuf, xLibU8_t * outBuf,
    xLibU16_t * outSizeLen)
{

  xLibU8_t *temp;
  xLibS8_t tempBufIn[25];
  xLibS8_t tempBufOut[25];
  xLibU32_t ipAddrTemp;
  xLibU32_t count = 0;
  xLibU32_t indx = 0;

  memset (outBuf, 0x00, sizeof (outBuf));

  if (inBuf == NULL)
  {
    for (indx = 0; indx < 8; indx++)
    {
      sprintf (tempBufIn, "0.0.0.0");
      memset (tempBufOut, 0x00, sizeof (tempBufOut));
      osapiInetPton (L7_AF_INET, (xLibU8_t *) tempBufIn, (void *) tempBufOut);
      memcpy (&ipAddrTemp, tempBufOut, sizeof (ipAddrTemp));
      ipAddrTemp = osapiHtonl (ipAddrTemp);
      memcpy (outBuf + (indx * 4), &ipAddrTemp, sizeof (ipAddrTemp));

    }
  }
  else
  {
    temp = strtok (inBuf, ",");
    if (temp != NULL)
    {

      while (temp != NULL)
      {
        if (count > 8)
        {
          return XLIBRC_FAILURE;
        }

        if (strlen (temp) > 19)
        {
          return XLIBRC_FAILURE;
        }

        memset (tempBufOut, 0x00, sizeof (tempBufOut));
        if (osapiInetPton (L7_AF_INET, (xLibU8_t *) temp, (void *) tempBufOut) != L7_SUCCESS)
        {
          return XLIBRC_FAILURE;
        }
        memcpy (&ipAddrTemp, tempBufOut, sizeof (ipAddrTemp));
        ipAddrTemp = osapiHtonl (ipAddrTemp);
        memcpy (outBuf + (count * 4), &ipAddrTemp, sizeof (ipAddrTemp));

        temp = strtok (NULL, ",");
        count++;
      }

      for (indx = count; indx < 8; indx++)
      {
        sprintf (tempBufIn, "0.0.0.0");
        memset (tempBufOut, 0x00, sizeof (tempBufOut));
        osapiInetPton (L7_AF_INET, (xLibU8_t *) tempBufIn, (void *) tempBufOut);
        memcpy (&ipAddrTemp, tempBufOut, sizeof (ipAddrTemp));
        ipAddrTemp = osapiHtonl (ipAddrTemp);
        memcpy (outBuf + (indx * 4), &ipAddrTemp, sizeof (ipAddrTemp));
      }
    }
  }

  *outSizeLen = 8 * 4;
  return XLIBRC_SUCCESS;
}

/* Encoding routine for user type clientIDDHCP_t  */
static xLibRC_t encode_clientIDDHCP_t (xLibIf_t intf, xLibU8_t * inBuf, xLibU16_t inLen,
    xLibS8_t * outBuf, xLibU16_t * outSizeLen)
{
  xLibU32_t counter;
  xLibU8_t strConvertClientId[(L7_DHCPS_CLIENT_ID_MAXLEN * 3) + 1];
  xLibStr256_t tempBuf;

  memset (strConvertClientId, 0x00, sizeof (strConvertClientId));
  memset (tempBuf, 0x00, sizeof (tempBuf));

  for (counter = 0; counter < inLen; counter++)
  {
    sprintf (tempBuf, "%02x", inBuf[counter]);
    strcat (strConvertClientId, tempBuf);

    if (counter != inLen - 1)
    {
      strcat (strConvertClientId, ":");
    }
  }
  strcpy (outBuf, strConvertClientId);
  *outSizeLen = strlen (outBuf);

  return XLIBRC_SUCCESS;
}

/* Decoding routine for user type clientIDDHCP_t. Converts Ascii to Hex data*/
static xLibRC_t decode_clientIDDHCP_t (xLibIf_t intf, xLibS8_t * inBuf, xLibU8_t * outBuf,
    xLibU16_t * outSizeLen)
{

  xLibU32_t i, j, digit_count = 0;
  xLibU32_t clientLen;
  xLibU8_t client_identifier[L7_SNMP_SUPPORTED_MIB_DESCRIPTION_SIZE];

  clientLen = strlen (inBuf);

  if ((clientLen % 3) != 2)
  {
    return XLIBRC_FAILURE;
  }

  if (((clientLen + 1) / 3) > L7_DHCPS_CLIENT_ID_MAXLEN)
  {
    return XLIBRC_FAILURE;
  }

  for (i = 0, j = 0; i < clientLen; i++, j++)
  {
    digit_count++;
    switch (inBuf[i])
    {
      case '0':
        client_identifier[j] = 0x0;
        break;
      case '1':
        client_identifier[j] = 0x1;
        break;
      case '2':
        client_identifier[j] = 0x2;
        break;
      case '3':
        client_identifier[j] = 0x3;
        break;
      case '4':
        client_identifier[j] = 0x4;
        break;
      case '5':
        client_identifier[j] = 0x5;
        break;
      case '6':
        client_identifier[j] = 0x6;
        break;
      case '7':
        client_identifier[j] = 0x7;
        break;
      case '8':
        client_identifier[j] = 0x8;
        break;
      case '9':
        client_identifier[j] = 0x9;
        break;
      case 'a':
      case 'A':
        client_identifier[j] = 0xA;
        break;
      case 'b':
      case 'B':
        client_identifier[j] = 0xB;
        break;
      case 'c':
      case 'C':
        client_identifier[j] = 0xC;
        break;
      case 'd':
      case 'D':
        client_identifier[j] = 0xD;
        break;
      case 'e':
      case 'E':
        client_identifier[j] = 0xE;
        break;
      case 'f':
      case 'F':
        client_identifier[j] = 0xF;
        break;
      case ':':
        if (digit_count != 3)
        {                         /* if more or less than 2 digits return false */
          return XLIBRC_FAILURE;
        }
        j--;
        digit_count = 0;
        break;
      default:
        return XLIBRC_FAILURE;
        break;
    }
  }

  *outSizeLen = (clientLen + 1) / 3;

  for (i = 0; i < *outSizeLen; i++)
  {
    outBuf[i] = ((client_identifier[(i * 2)] << 4) + client_identifier[(i * 2) + 1]);
  }
  return XLIBRC_SUCCESS;
}

/* Encoding routine for user type hexDataDHCP_t  */
static xLibRC_t encode_hexDataDHCP_t (xLibIf_t intf, xLibU8_t * inBuf, xLibU16_t inLen,
    xLibS8_t * outBuf, xLibU16_t * outSizeLen)
{
  L7_uint32 firstChar, lastChar, counter;
  L7_char8  temp[3];

  memset (outBuf, 0x0, sizeof (outBuf));
  for (counter = 0; counter < inLen; counter++)
  {
    memset (temp, 0, sizeof (temp));
    firstChar = (inBuf[counter] & 0xF0) >> 4;
    lastChar  = inBuf[counter] & 0x0F;
    osapiSnprintf (temp, sizeof (temp), "%x%x", firstChar, lastChar);

    if ((counter % 2) == 1)
    {
      if (counter != (inLen - 1))
      {
        OSAPI_STRNCAT (temp, ".");
      }
    }
    OSAPI_STRNCAT (outBuf, temp);
  }
  *outSizeLen = strlen (outBuf);
  return XLIBRC_SUCCESS;
}

/* Decoding routine for user type hexDataDHCP_t. Converts Hex to binary*/
static xLibRC_t decode_hexDataDHCP_t (xLibIf_t intf, xLibS8_t * inBuf, xLibU8_t * outBuf,
    xLibU16_t * outSizeLen)
{
  L7_uint32 outLength = 0;

  if (strlen (inBuf) > (L7_DHCPS_POOL_OPTION_LENGTH * 3))
  {
    return XLIBRC_FAILURE;
  }

  /* Convert the input Hex string to binary data */
  if (usmDbHexToBin (inBuf, outBuf, &outLength) != L7_SUCCESS)
  {
    *outSizeLen = 0;
    return XLIBRC_FAILURE;
  }
  *outSizeLen = outLength;
  return XLIBRC_SUCCESS;
}

/* Encoding routine for user type ipAddrDataStr_t  */
static xLibRC_t encode_ipAddrDataStr_t (xLibIf_t intf, xLibU8_t * inBuf, xLibU16_t inLen,
    xLibS8_t * outBuf, xLibU16_t * outSizeLen)
{
  xLibU32_t tempIPAddr = 0;
  xLibS8_t tempBufIn[25];
  xLibS8_t tempBufOut[25];
  xLibU32_t tempLen;
  xLibU32_t counter;
  memset (outBuf, 0x00, sizeof (outBuf));
  if ((inBuf == NULL))
  {
    outBuf = NULL;
    *outSizeLen = 0;
    return XLIBRC_FAILURE;
  }

  counter = 0;

  while (counter < inLen)
  {
    /*get the IpAddress */
    memcpy (&tempIPAddr, inBuf + counter, sizeof (tempIPAddr));
    tempIPAddr = (xLibU32_t) osapiNtohl (tempIPAddr);

    memset (tempBufIn, 0x00, sizeof (tempBufIn));
    memset (tempBufOut, 0x00, sizeof (tempBufOut));

    memcpy (tempBufIn, &tempIPAddr, sizeof (tempIPAddr));
    osapiInetNtop (L7_AF_INET, tempBufIn, (void *) tempBufOut, sizeof (tempBufOut));
    tempLen = strlen (tempBufOut);
    if (counter == 0)
    {
      memset(outBuf,0,tempLen+1);
      strncpy (outBuf, tempBufOut, tempLen);
    }
    else
    {
      strcat (outBuf, ",");
      strcat (outBuf, tempBufOut);
    }
    counter = counter + 4;

  }

  *outSizeLen = strlen (outBuf);
  return XLIBRC_SUCCESS;
}

/* Decoding routine for user type ipAddrDataStr_t. */
static xLibRC_t decode_ipAddrDataStr_t (xLibIf_t intf, xLibS8_t * inBuf, xLibU8_t * outBuf,
    xLibU16_t * outSizeLen)
{

  xLibU8_t *temp;
  xLibS8_t tempBufOut[25];
  xLibU32_t ipAddrTemp;
  xLibU32_t count = 0;

  memset (outBuf, 0x00, sizeof (outBuf));

  if (inBuf == NULL)
  {
    outBuf = NULL;
    *outSizeLen = 0;
    return XLIBRC_FAILURE;
  }
  else
  {
    temp = strtok (inBuf, ",");
    if (temp != NULL)
    {
      while (temp != NULL)
      {
        if (strlen (temp) > 19)
        {
          return XLIBRC_FAILURE;
        }

        memset (tempBufOut, 0x00, sizeof (tempBufOut));
        if (osapiInetPton (L7_AF_INET, (xLibU8_t *) temp, (void *) tempBufOut) != L7_SUCCESS)
        {
          return XLIBRC_FAILURE;
        }
        memcpy (&ipAddrTemp, tempBufOut, sizeof (ipAddrTemp));
        ipAddrTemp = osapiHtonl (ipAddrTemp);
        memcpy (outBuf + (count * 4), &ipAddrTemp, sizeof (ipAddrTemp));

        temp = strtok (NULL, ",");
        count++;
      }
    }

  }

  *outSizeLen = count * 4;
  return XLIBRC_SUCCESS;

}

/* Encoding routine for user type optionCode_t  */
static xLibRC_t encode_optionCode_t (xLibIf_t intf, xLibU8_t * inBuf, xLibU16_t inLen,
    xLibS8_t * outBuf, xLibU16_t * outSizeLen)
{
  L7_char8  temp[10];
  memset (temp, 0x00, sizeof (temp));

  if (inLen != sizeof (xLibU8_t))
  {
    return XLIBRC_FAILURE;
  }

  sprintf (temp, "%d", inBuf[0]);
  strcpy (outBuf, temp);
  *outSizeLen = strlen (outBuf);
  return XLIBRC_SUCCESS;
}

/* Decoding routine for user type optionCode_t. Converts str to uint */
static xLibRC_t decode_optionCode_t (xLibIf_t intf, xLibS8_t * inBuf, xLibU8_t * outBuf,
    xLibU16_t * outSizeLen)
{
  L7_int32 opCode = 0;

  if (strlen (inBuf) > 3)
  {
    return XLIBRC_FAILURE;
  }

  opCode = atoi (inBuf);
  outBuf[0] = (xLibU8_t) opCode;

  *outSizeLen = sizeof (xLibU8_t);
  return XLIBRC_SUCCESS;
}

/* display time in HH:MM:SS format */
static xLibRC_t encode_timeStampHHMMSS_t (xLibIf_t intf, xLibU8_t * inBuf, xLibU16_t inLen,
    xLibS8_t * outBuf, xLibU16_t * outSizeLen)
{
  xLibU32_t timeInSeconds;
  L7_timespec ts;

  if (inLen != sizeof (xLibU32_t))
  {
    return XLIBRC_FAILURE;
  }

  memcpy (&timeInSeconds, inBuf, sizeof (xLibU32_t)); /*Get the total no of seconds from inBuf*/

  osapiConvertRawUpTime(timeInSeconds, &ts); 

  osapiSnprintf(outBuf, 256, "%02d:%02d:%02d", (ts.days * 24) + ts.hours, ts.minutes, ts.seconds);

  *outSizeLen = strlen (outBuf);

  return XLIBRC_SUCCESS;

}

/* display time in HH:MM:SS format */
static xLibRC_t decode_timeStampHHMMSS_t (xLibIf_t intf, xLibS8_t * inBuf, xLibU8_t * outBuf,
    xLibU16_t * outSizeLen)
{

  return XLIBRC_SUCCESS;
}

/* display time in Days HH:MM:SS format */
static xLibRC_t encode_timeStampDHHMMSS_t (xLibIf_t intf, xLibU8_t * inBuf, xLibU16_t inLen,
                                      xLibS8_t * outBuf, xLibU16_t * outSizeLen)
{
xLibU32_t timeInSeconds;
L7_timespec ts;

  if (inLen != sizeof (xLibU32_t))
  {
    return XLIBRC_FAILURE;
  }

  memcpy (&timeInSeconds, inBuf, sizeof (xLibU32_t)); /*Get the total no of seconds from inBuf*/

  osapiConvertRawUpTime(timeInSeconds, &ts);

  osapiSnprintf(outBuf, 256, "%d Days %02d:%02d:%02d", ts.days, ts.hours, ts.minutes, ts.seconds);

  *outSizeLen = strlen (outBuf);

  return XLIBRC_SUCCESS;

}

/* display time in Days HH:MM:SS format */
static xLibRC_t decode_timeStampDHHMMSS_t (xLibIf_t intf, xLibS8_t * inBuf, xLibU8_t * outBuf,
                                      xLibU16_t * outSizeLen)
{

  return XLIBRC_SUCCESS;
}


  /* Encoding routine for user type VRRPPriority_t */
static xLibRC_t encode_VRRPPriority_t (xLibIf_t intf, xLibU8_t * inBuf, xLibU16_t inLen,
                                     xLibS8_t * outBuf, xLibU16_t * outSizeLen)
{
  L7_char8 temp[10];
  memset (temp, 0x00, sizeof (temp));

  if (inLen != sizeof (xLibU8_t))
  {
    return XLIBRC_FAILURE;
  }

  sprintf (temp, "%d", inBuf[0]);
  strcpy (outBuf, temp);
  *outSizeLen = strlen (outBuf);
  return XLIBRC_SUCCESS;
}

/* Decoding routine for user type VRRPPriority_t. Converts str to uint */
static xLibRC_t decode_VRRPPriority_t (xLibIf_t intf, xLibS8_t * inBuf, xLibU8_t * outBuf,
                                     xLibU16_t * outSizeLen)
{
  L7_int32 priorityValue = 0;

  if (strlen (inBuf) > 3)
  {
    return XLIBRC_FAILURE;
  }

  priorityValue = atoi (inBuf);

  if( (priorityValue < 0) || (priorityValue > 255) )
  {
    return XLIBRC_FAILURE;
  }
  outBuf[0] = (xLibU8_t) priorityValue;

  *outSizeLen = sizeof (xLibU8_t);
  return XLIBRC_SUCCESS;
}

/* Encoding routine for user type IPMask_t  */
static xLibRC_t encode_IPMask_t (xLibIf_t intf, xLibU8_t * inBuf, xLibU16_t inLen,
    xLibS8_t * outBuf, xLibU16_t * outSizeLen)
{
  L7_char8 temp[10];
  memset (temp, 0x00, sizeof (temp));
  xLibU32_t mask = 0;
  xLibU32_t prefixLength = 0;

  L7_uint32 b;
  b = 0X80000000;


  if (inLen != sizeof (xLibU32_t))
  {
    return XLIBRC_FAILURE;
  }
  memcpy(&mask,inBuf,sizeof(xLibU32_t));

  while(mask & b)
  {
    prefixLength++;
    b = b >> 1;
  }
  osapiSnprintf(temp, 10, "%d", prefixLength);
  osapiStrncpy (outBuf, temp,sizeof(temp));
  *outSizeLen = strlen (outBuf);
  return XLIBRC_SUCCESS;
}

/* Decoding routine for user type IPMask_t. Converts prefix length to mask */
static xLibRC_t decode_IPMask_t (xLibIf_t intf, xLibS8_t * inBuf, xLibU8_t * outBuf,
    xLibU16_t * outSizeLen)
{
  xLibU32_t prefixLength = 0;
  xLibU32_t mask = 0;

  if (strlen (inBuf) > 3)
  {
    return XLIBRC_FAILURE;
  }

  prefixLength = atoi (inBuf);

  if(prefixLength < 0 || prefixLength > 32)
  {
    return XLIBRC_FAILURE;
  }

  if (prefixLength == 0)
  {
    mask = 0;
  }
  else 
  {
    if (usmDbIpSubnetMaskGet(&mask,prefixLength)!= L7_SUCCESS)
    {
      return XLIBRC_FAILURE;
    }
  }

  memcpy (outBuf, &mask, sizeof (mask));
  *outSizeLen = sizeof (mask);

  return XLIBRC_SUCCESS;
}

/* Encoding routine for user type elapseTimeDHMS_t  (elapsed time in days,Hours,minutes and seconds format)*/
static xLibRC_t encode_elapsedTimeDHMS_t (xLibIf_t intf, xLibU8_t * inBuf, xLibU16_t inLen,
    xLibS8_t * outBuf, xLibU16_t * outSizeLen)
{
  if (inLen != sizeof (xLibU32_t))
  {
    return XLIBRC_FAILURE;
  }
  
  if((usmDbConvertTimeTicksToDaysHoursMinutesSeconds((xLibU32_t *)&inBuf, outBuf))!= L7_SUCCESS)
  {
    return XLIBRC_FAILURE;
  }  
  *outSizeLen = strlen (outBuf);
  
  return XLIBRC_SUCCESS;
}

/* Decoding routine for user type elapseTimeDHMS_t  (elapsed time in days,Hours,minutes and seconds format) */
static xLibRC_t decode_elapsedTimeDHMS_t (xLibIf_t intf, xLibS8_t * inBuf, xLibU8_t * outBuf,
    xLibU16_t * outSizeLen)
{

  return XLIBRC_SUCCESS;
}

static xLibRC_t decode_IPV6_link_t (xLibIf_t intf, xLibS8_t * inBuf, xLibU8_t * outBuf,
    xLibU16_t * outSizeLen)
{
  xLibU32_t intIfNum;
  xLibU32_t unit = 1;
  xLibU32_t slot;
  xLibU32_t port;
  xLibU32_t loopback;
  xLibU32_t tunnel;
  L7_uchar8 string[20];

  unit = 1;

  if(strstr(inBuf, "loopback") != NULL)
  {
    sscanf (inBuf, "%s %d",string, &loopback);
#ifdef L7_RLIM_PACKAGE
    if(usmDbRlimLoopbackIntIfNumGet(loopback, &intIfNum) != L7_SUCCESS)
    {
      return XLIBRC_FAILURE;
    }
#endif
    memcpy (outBuf, &intIfNum, sizeof (intIfNum));
    *outSizeLen = sizeof (intIfNum);

  }
  else if(strstr(inBuf, "tunnel") != NULL)
  {
    sscanf (inBuf, "%s %d", string, &tunnel);
#ifdef L7_RLIM_PACKAGE
    if(usmDbRlimTunnelIntIfNumGet(tunnel, &intIfNum) != L7_SUCCESS)
    {
      return XLIBRC_FAILURE;
    }
#endif
    memcpy (outBuf, &intIfNum, sizeof (intIfNum));
    *outSizeLen = sizeof (intIfNum);

  }
  else if(strstr(inBuf, "Network") != NULL)
  {
    intIfNum = L7_XUI_NETWORK_PORT_ID;
    memcpy (outBuf, &intIfNum, sizeof (intIfNum));
    *outSizeLen = sizeof (intIfNum);
  }
  else if(strstr(inBuf, "ServicePort") != NULL)
  {
    intIfNum = L7_XUI_SERVICE_PORT_ID;
    memcpy (outBuf, &intIfNum, sizeof (intIfNum));
    *outSizeLen = sizeof (intIfNum);
  }
  else
  {
    if (fpObjIsStackingSupported ())
    {
      sscanf (inBuf, "%d/%d/%d\n", &unit, &slot, &port);
    }
    else
    {
      sscanf (inBuf, "%d/%d\n", &slot, &port);
    }
    if (usmDbIntIfNumFromUSPGet (unit, slot, port, &intIfNum) == L7_SUCCESS)
    {
      memcpy (outBuf, &intIfNum, sizeof (intIfNum));
      *outSizeLen = sizeof (intIfNum);
    }
    else
    {
      return XLIBRC_FAILURE;
    }
  }

  return XLIBRC_SUCCESS;
}
static xLibRC_t encode_IPV6_link_t (xLibIf_t intf, xLibU8_t * inBuf, xLibU16_t inLen,
    xLibS8_t * outBuf, xLibU16_t * outSizeLen)
{
  nimUSP_t usp;
  xLibU32_t intIfNum;
  L7_uint32 sysIntfType;
#ifdef L7_RLIM_PACKAGE
  L7_uint32 loopbackId;
  L7_uint32 tunnelId;
#endif
  memset (outBuf, 0x0, sizeof (outBuf));
  memcpy (&intIfNum, inBuf, sizeof (intIfNum));

  if(intIfNum == L7_XUI_SERVICE_PORT_ID)
  {
    sprintf(outBuf, "%s", "ServicePort");
    *outSizeLen = strlen (outBuf);
    return XLIBRC_SUCCESS;
  }
  else if(intIfNum == L7_XUI_NETWORK_PORT_ID)
  {
    sprintf(outBuf, "%s", "Network");
    *outSizeLen = strlen (outBuf);
    return XLIBRC_SUCCESS;
  }

  if (intIfNum == L7_ALL_INTERFACES)
  {
    return XLIBRC_FAILURE;
  }
  else
  {
    if (usmDbIntfTypeGet(intIfNum, &sysIntfType) == L7_SUCCESS)
    {
      switch (sysIntfType)
      {
#ifdef L7_RLIM_PACKAGE
        case L7_LOOPBACK_INTF:
          if (usmDbRlimLoopbackIdGet(intIfNum, &loopbackId) == L7_SUCCESS)
          {
            sprintf(outBuf, "%s %d", "loopback", loopbackId);
            *outSizeLen = strlen (outBuf);
          }
          else
          {
            return XLIBRC_FAILURE;
          }
          break;

        case L7_TUNNEL_INTF:
          if (usmDbRlimTunnelIdGet(intIfNum, &tunnelId) == L7_SUCCESS)
          {
            sprintf(outBuf, "%s %d", "tunnel", tunnelId);
            *outSizeLen = strlen (outBuf);
          }
          else
          {
            return XLIBRC_FAILURE;
          }
          break;
#endif
        case L7_PHYSICAL_INTF:
        case L7_STACK_INTF:
        case L7_CPU_INTF:
        case L7_LAG_INTF:
        case L7_LOGICAL_VLAN_INTF:
          if (nimGetUnitSlotPort (intIfNum, &usp) == L7_SUCCESS)
          {
            if (fpObjIsStackingSupported ())
            {
              sprintf (outBuf, "%d/%d/%d", usp.unit, (xLibU32_t) usp.slot, usp.port);
            }
            else
            {
              sprintf (outBuf, "%d/%d", (xLibU32_t) usp.slot, usp.port);
            }

            *outSizeLen = strlen (outBuf);
          }
          else
          {
            return XLIBRC_FAILURE;
          }
          break;
        default:
          break;
      }
    }
    else
    {
      return XLIBRC_FAILURE;
    }
  }
  return XLIBRC_SUCCESS;
}
/* Encoding routine for user type standbyStackUnit_t   (Standby Stack Unit Id including the option of "None") */
static xLibRC_t encode_standbyStackUnit_t (xLibIf_t intf, xLibU8_t * inBuf, xLibU16_t inLen,
    xLibS8_t * outBuf, xLibU16_t * outSizeLen)
{
  xLibU32_t unitNum;

  if (inLen != sizeof (xLibU32_t))
  {
    return XLIBRC_FAILURE;
  }

  memset (outBuf, 0x0, sizeof (outBuf));
  memcpy (&unitNum, inBuf, sizeof (xLibU32_t));

  if(unitNum <= L7_MAX_UNITS_PER_STACK)
  {
    sprintf(outBuf,"%d", unitNum);
  }
  else
  {
    sprintf(outBuf,"None");
  }

  *outSizeLen = strlen (outBuf);
  return XLIBRC_SUCCESS;

}

static xLibRC_t decode_standbyStackUnit_t (xLibIf_t intf, xLibS8_t * inBuf, xLibU8_t * outBuf,
    xLibU16_t * outSizeLen)
{
  xLibU32_t unitNum = 0;

  if (strlen (inBuf) > sizeof("None"))
  {
    return XLIBRC_FAILURE;
  }

  if(osapiStrncmp("None", inBuf, sizeof("None")) == 0)
  {
    unitNum = L7_MAX_UNITS_PER_STACK + 1;
  }
  else
  {
    unitNum = atoi (inBuf);
  }

  if((unitNum <= 0) || (unitNum > (L7_MAX_UNITS_PER_STACK + 1)))
  {
    return XLIBRC_FAILURE;
  }
  *((xLibU32_t *)outBuf) = unitNum;
  *outSizeLen = sizeof (xLibU32_t);

  return XLIBRC_SUCCESS;
}

/* Encoding routine for user type usplist_t */
static xLibRC_t encode_ppsPercent_t (xLibIf_t intf, xLibU8_t * inBuf, xLibU16_t inLen,
    xLibS8_t * outBuf, xLibU16_t * outSizeLen)
{
  xLibU32_t stormControlLevel;
  xLibU32_t stormControlLevelUnit;
  xLibU8_t *temp;

  memset (outBuf, 0x0, sizeof (outBuf));

  if (inBuf == NULL)
  {
    outBuf = NULL;
    *outSizeLen = 0;
    return XLIBRC_SUCCESS;
  }

  temp = strtok (inBuf, ",");
  if (temp == NULL)
  {
    outBuf = NULL;
    *outSizeLen = 0;
    return XLIBRC_SUCCESS;
  }

  sscanf (temp, "%d", &stormControlLevel);

  temp = strtok (NULL, ",");
  if (temp == NULL)
  {
    outBuf = NULL;
    *outSizeLen = 0;
    return XLIBRC_SUCCESS;
  }
  
  sscanf (temp, "%d", &stormControlLevelUnit);

  osapiSnprintf(outBuf, 10,"%d", stormControlLevel); 

  switch(stormControlLevelUnit)
  {
    case L7_RATE_UNIT_PERCENT: osapiStrncat(outBuf,"%", strlen("%"));break;    
    case L7_RATE_UNIT_PPS:     osapiStrncat(outBuf," pps", strlen(" pps"));break; 
    default:                   osapiStrncat(outBuf," err", strlen(" err"));break; 
  }

  *outSizeLen = strlen (outBuf);
  return XLIBRC_SUCCESS;
}

static xLibRC_t encode_v4v6Mask_t (xLibIf_t intf, xLibU8_t * inBuf, xLibU16_t inLen,
                                  xLibS8_t * outBuf, xLibU16_t * outSizeLen)
{

  xLibV4V6Mask_t aval;
  L7_inet_addr_t mask;


  memset(outBuf,0,sizeof(outBuf));  
  memcpy(&aval,inBuf,inLen);

  
  if ( aval.family == L7_AF_INET )
  {
    inetAddressZeroSet(L7_AF_INET,&mask);
    inetMaskLenToMask(L7_AF_INET,aval.addr,&mask);
    if (usmDbInetNtoa(osapiNtohl(mask.addr.ipv4.s_addr), outBuf) != L7_SUCCESS)
    {
      osapiStrncpySafe(outBuf, "unKnown", strlen("unKnown"));
    }
  
  }
  else
  {
    if (aval.family == L7_AF_INET6)
    {
      osapiSnprintf(outBuf,4,"%d",aval.addr);
    }
  }
  *outSizeLen = strlen (outBuf);
  return XLIBRC_SUCCESS;

}
static xLibRC_t decode_v4v6Mask_t (xLibIf_t intf, xLibS8_t * inBuf,
                                  xLibU8_t * outBuf, xLibU16_t * outSizeLen)
{

  L7_uint32 maskLen=0;
  L7_inet_addr_t mask;
  xLibV4V6Mask_t prefix;
  L7_uchar8 tempLen=0;

  if(inBuf == NULL)
  {
    return XLIBRC_FAILURE;
  }
  if(strcmp(inBuf,"") ==0)
  {
    return XLIBRC_SUCCESS;
  }
  if((strstr(inBuf, ".") != L7_NULL) && (strstr(inBuf, ":") == L7_NULL))
    {
      /* This case we have a dot in the address. This would either
          be  a domain Name or IPv4 address. */
      inetAddressZeroSet(L7_AF_INET,&mask);
      if (usmDbInetAton(inBuf,&mask.addr.ipv4.s_addr) != L7_SUCCESS)
      {
        return XLIBRC_FAILURE;
      }
      inetMaskToMaskLen(&mask,&tempLen);
      prefix.family = L7_AF_INET;
      maskLen = tempLen;
  
    }
    else
    {
      prefix.family = L7_AF_INET6;
      cliWebXuiConvertTo32BitUnsignedInteger(inBuf,&maskLen); 
    }
    prefix.addr = maskLen;
  
    memcpy (outBuf, (xLibU8_t *)&prefix, sizeof(xLibV4V6Mask_t));
    *outSizeLen = sizeof(xLibV4V6Mask_t); 
 
   
  return XLIBRC_SUCCESS;

}



/* Encoding routine for user type protectedPortGroupId_t   ( Protected Port group  Id  including the option of "None") */
static xLibRC_t encode_protectedPortGroupId_t (xLibIf_t intf, xLibU8_t * inBuf, xLibU16_t inLen,
                                  xLibS8_t * outBuf, xLibU16_t * outSizeLen)
{
  xLibU32_t groupNum;

  if (inLen != sizeof (xLibU32_t))
  {
    return XLIBRC_FAILURE;
  }

  memset (outBuf, 0x0, sizeof (outBuf));
  memcpy (&groupNum, inBuf, sizeof (xLibU32_t));

  if(groupNum <= L7_PROTECTED_PORT_MAX_GROUPS)
  {
    sprintf(outBuf,"%d", groupNum);
  }
  else
  {
    sprintf(outBuf,"None");
  }

  *outSizeLen = strlen (outBuf);
  return XLIBRC_SUCCESS;

}

/* Decoding routine for user type protectedPortGroupId_t  ( Protected Port group  Id including the option of "None") */
static xLibRC_t decode_protectedPortGroupId_t (xLibIf_t intf, xLibS8_t * inBuf, xLibU8_t * outBuf,
                                      xLibU16_t * outSizeLen)
{
  xLibU32_t groupNum = 0;

  if (strlen (inBuf) > sizeof("None"))
  {
    return XLIBRC_FAILURE;
  }

  if(osapiStrncmp("None", inBuf, sizeof("None")) == 0)
  {
    groupNum = L7_PROTECTED_PORT_MAX_GROUPS + 1;
  }
  else
  {
    groupNum = atoi (inBuf);
  }

  if((groupNum < 0) || (groupNum > (L7_PROTECTED_PORT_MAX_GROUPS + 1)))
  {
    return XLIBRC_FAILURE;
  }
  *((xLibU32_t *)outBuf) = groupNum;
  *outSizeLen = sizeof (xLibU32_t);

  return XLIBRC_SUCCESS;
}


static xLibRC_t encode_cp_hex_str_t(xLibIf_t intf, xLibU8_t * inBuf, xLibU16_t inLen,
    xLibS8_t * outBuf, xLibU16_t * outSizeLen)
{
  xLibU32_t idx = 0;
  xLibU32_t jdx = 0;
  xLibU32_t len = 0;
  div_t res;
 
  if ((L7_NULLPTR == inBuf) || (L7_NULLPTR == outBuf))
  {
    return XLIBRC_FAILURE;
  }
  
  /* Validate incoming hex string length */
  len = strlen(inBuf);
  res = div(len,4);
  if (0 != res.rem)
  {
    return XLIBRC_FAILURE;
  }
 
 if (L7_SUCCESS != usmDbStringHexadecimalCheck(inBuf))
  {
    return XLIBRC_FAILURE;
  }
 
  while (inBuf[idx] != '\0')
  {
    outBuf[jdx++] = '&';
    outBuf[jdx++] = '#';
    outBuf[jdx++] = 'x';
    outBuf[jdx++] = inBuf[idx++];
    outBuf[jdx++] = inBuf[idx++];
    outBuf[jdx++] = inBuf[idx++];
    outBuf[jdx++] = inBuf[idx++];
    outBuf[jdx++] = ';';
  }
  outBuf[jdx] = '\0';
  return XLIBRC_SUCCESS;
}

static xLibRC_t decode_cp_hex_str_t(xLibIf_t intf, xLibS8_t * inBuf, xLibU8_t * outBuf,
    xLibU16_t * outSizeLen)
{
  xLibU32_t len = strlen(inBuf);
  xLibS8_t ret[1024];
  xLibS8_t temp[1024];
  xLibS8_t hexstr[3];
  xLibS8_t * def="0000";
  xLibU32_t i,j = 0;
 
  memset(ret,0x0,sizeof(ret));
  memset(hexstr,0x0,sizeof(hexstr));
  for(i = 0,j = 0; i < len;i++){
    memset(temp,0x0,sizeof(temp));
    sprintf(hexstr,"%2x",inBuf[i]);

    hexstr[0] = toupper(hexstr[0]);
    hexstr[1] = toupper(hexstr[1]);

    memcpy(temp,def,strlen(def));
    strncat(temp,hexstr, (sizeof(hexstr)));
    memcpy(&ret[j],temp+2,strlen(temp)-2);
    j += strlen(temp)-2;
  }
  memset(outBuf, 0x0, sizeof(outBuf));
  memcpy(outBuf,ret,strlen(ret));
  *outSizeLen = strlen(outBuf);
  return XLIBRC_SUCCESS;
}

/* Encoding routine for user type eventControlTimeFormat_t  */
static xLibRC_t encode_eventControlTimeFormat_t (xLibIf_t intf, xLibU8_t * inBuf, xLibU16_t inLen,
    xLibS8_t * outBuf, xLibU16_t * outSizeLen)
{
  xLibU32_t timeInSeconds;
  struct tm date, *datep;
  L7_uint32 utcTime;
  L7_char8 mon_name[12][4] = {"Jan\0", "Feb\0", "Mar\0", "Apr\0", "May\0",
                              "Jun\0", "Jul\0", "Aug\0", "Sep\0", "Oct\0",
                              "Nov\0", "Dec\0" };

	
  if (inLen != sizeof (xLibU32_t))
  {
    return XLIBRC_FAILURE;
  }

  memcpy (&timeInSeconds, inBuf, sizeof (xLibU32_t));
  memset (outBuf, 0, sizeof (outBuf));
  timeInSeconds = timeInSeconds / 100;  /* to get no of seconds */

  /* Convert int to string format */
  osapiRawTimeToUTCTime(timeInSeconds, &utcTime);

  datep = &date;
  datep = localtime((time_t *)&utcTime);
  osapiSnprintf(outBuf, 256, "%s %.2d %d %.2d:%.2d:%.2d", mon_name[datep->tm_mon], datep->tm_mday, datep->tm_year + 1900,
          datep->tm_hour, datep->tm_min, datep->tm_sec);
  *outSizeLen = strlen (outBuf);

  return XLIBRC_SUCCESS;
}

/* Decoding routine for user type eventControlTimeFormat_t  */
static xLibRC_t decode_eventControlTimeFormat_t (xLibIf_t intf, xLibS8_t * inBuf, xLibU8_t * outBuf,
    xLibU16_t * outSizeLen)
{

  return XLIBRC_SUCCESS;
}

static int checkTransferProgress(void *arg)
{
 return (usmDbTransferInProgressGet(1) == L7_TRUE) ? 1 : 0;
}


void xLibAppInit (void)
{
#ifdef _L7_OS_LINUX_
  xLibFileHandlerRegister (osapiWebFileHandler);
#endif
#ifdef L7_CAPTIVE_PORTAL_PACKAGE
  emwebUrlRegisterHook(L7_NULL,ewaUrlCpHook);
#endif

  /* Registration for URL redirect for Transfer Progress */
  xLibTransferProgressCheckSet(checkTransferProgress, NULL, "/progress_file_transfer.html", NULL);

  /* Registration for uspGlobal_t type */
  xLibDataMapRegister ("uspGlobal_t", encode_uspGlobal_t, decode_uspGlobal_t);

  /* Registration for usp_t type */
  xLibDataMapRegister ("usp_t", encode_usp_t, decode_usp_t);

  /* Registration for uspNone_t type */
  xLibDataMapRegister ("uspNone_t", encode_uspNone_t, decode_uspNone_t);

  /* Registration for vidMac type */
  xLibDataMapRegister ("vidMac", xLibAppvidMacEncode, xLibAppvidMacDecode);

  /* Registration for snmpMac_t type */
  xLibDataMapRegister ("snmpMac_t", xLibAppsnmpMacEncode, xLibAppsnmpMacDecode);

  /* Registration for ipv4_ipv6 type */
  xLibDataMapRegister ("ipv4_ipv6", ipv4_ipv6_Encode, ipv4_ipv6_Decode);

  /* Registration for ipv4_ipv6 with_zeros type */
  xLibDataMapRegister ("ipv4_ipv6_with_zeros", ipv4_ipv6_Encode, ipv4_ipv6_Decode);

  /* Registration for ipv6Prefix_t type */
  xLibDataMapRegister ("ipv6Prefix_t", encode_ipv6Prefix_t, decode_ipv6Prefix_t);

  /* Registration for stats64_t type */
  xLibDataMapRegister ("stats64_t", encode_stats64_t, decode_stats64_t);

  /* Registration for OSPFv3_TRAP_FLAGS_t type */
  xLibDataMapRegister ("ospfv3_trap_flags_t", encode_ospfv3_trap_flags_t,
      decode_ospfv3_trap_flags_t);

  /* Registration for snmpMac_t type */
  xLibDataMapRegister ("usplist_t", encode_usplist_t, decode_usplist_t);

  xLibDataMapRegister ("vlanIDMac_t", encode_vlanIDMac_t, decode_vlanIDMac_t);

  /* Registration for elapseTime_t type */
  xLibDataMapRegister ("elapsedTime_t", encode_elapsedTime_t, decode_elapsedTime_t);
  /* Registration for elapseTime_t type */
  xLibDataMapRegister ("elapsedTimeDHCP_t", encode_elapsedTimeDHCP_t, decode_elapsedTimeDHCP_t);

  /* Registration for RemainingTimeDHCP_t*/
  xLibDataMapRegister ("RemainingTimeDHCP_t", encode_RemainingTimeDHCP_t, decode_RemainingTimeDHCP_t);

  /* Registration for ipAddrList_t type */
  xLibDataMapRegister ("ipAddrList_t", encode_ipAddrList_t, decode_ipAddrList_t);

  /* Registration for clientIDDHCP_t type */
  xLibDataMapRegister ("clientIDDHCP_t", encode_clientIDDHCP_t, decode_clientIDDHCP_t);

  /* Registaration for ipv4 mask and ipv6 prefix len conversion  */
  xLibDataMapRegister ("formatv4v6mask_t", encode_v4v6Mask_t, decode_v4v6Mask_t);

  /* Registration for hexDataDHCP_t type */
  xLibDataMapRegister ("hexDataDHCP_t", encode_hexDataDHCP_t, decode_hexDataDHCP_t);

  /* Registration for hexDataDHCP_t type */
  xLibDataMapRegister ("ipAddrDataStr_t", encode_ipAddrDataStr_t, decode_ipAddrDataStr_t);

  /* Registration for hexDataDHCP_t type */
  xLibDataMapRegister ("optionCode_t", encode_optionCode_t, decode_optionCode_t);

  /* Registration for hexDataDHCP_t type */
  xLibDataMapRegister ("hex_uint_t", encode_hex_uint_t, decode_hex_uint_t);

  /* Registration for hex type */
  xLibDataMapRegister ("hex_t", encode_hex_t, decode_hex_t);

  /* Display time in HH:MM:SS format */
  xLibDataMapRegister ("timeStampHHMMSS_t", encode_timeStampHHMMSS_t, decode_timeStampHHMMSS_t);

  /* Display time in Days HH:MM:SS format */
  xLibDataMapRegister ("timeStampDHHMMSS_t", encode_timeStampDHHMMSS_t, decode_timeStampDHHMMSS_t);

  /* Registration for hex_uint_t type */
  xLibDataMapRegister ("hex_uint_t", encode_hex_uint_t, decode_hex_uint_t);

  /* Registration for string_t type */
  xLibDataMapRegister ("string_t", encode_string_t, decode_string_t);

  /* Registration for ipv4_ipv6_serial type */
  xLibDataMapRegister ("ipv4_ipv6_serial", ipv4_ipv6_serial_Encode, ipv4_ipv6_serial_Decode);

  /* Registration for ipv4_uint type */
  xLibDataMapRegister ("ipv4_uint", ipv4_uint_Encode, ipv4_uint_Decode);

  /* Registration for dot3ah Cap object */
  xLibDataMapRegister ("dot3ahCap_t", encode_dot3ahCap_t, decode_dot3ahCap_t);

  /* Registration for dot3ah Time object */
  xLibDataMapRegister ("dot3ahTime_t", encode_dot3ahTime_t, decode_dot3ahTime_t);

  /* Registration for dot3ah OUI object */
  xLibDataMapRegister ("dot3ahOui_t", encode_dot3ahOui_t, decode_dot3ahOui_t);
  /* Registration for dot3ah Device and Vendor ID object */
  xLibDataMapRegister ("dot3ahDevVendorID_t", encode_dot3ahDevVendorID_t, decode_dot3ahDevVendorID_t);

  /* Registration for VRRPPriority_t type */
  xLibDataMapRegister ("VRRPPriority_t", encode_VRRPPriority_t, decode_VRRPPriority_t);

  /* Registration for VRRPPriority_t type */
  xLibDataMapRegister ("IPMask_t", encode_IPMask_t, decode_IPMask_t);

  /* Registration for elapseTimeDHMS_t type */
  xLibDataMapRegister ("elapsedTimeDHMS_t", encode_elapsedTimeDHMS_t, decode_elapsedTimeDHMS_t);

  /* Registaration for ipv4 address which takes 0.0.0.0 as valid input, which is used in rto table */
  xLibDataMapRegister ("rtoipv4_t", xLibEncode_ipv4, xLibDecode_ipv4);

  /* Registaration for Decimal notation ipv4 address which takes in IPv4 addresss in the format a.b.c.d */
  xLibDataMapRegister ("formatipv4_t", xLibEncode_ipv4, xLibDecode_ipv4);
  
    /* Registration for ping page. This datatype shows in combo box all interfaces along with loopback and tunnelID's */
  xLibDataMapRegister ("IPV6_link_t", encode_IPV6_link_t, decode_IPV6_link_t);


  /* Registaration for Standby Stack Unit ID that will take a stackID or "None" as values */
  xLibDataMapRegister ("standbyStackUnit_t", encode_standbyStackUnit_t, decode_standbyStackUnit_t);

  /* Registaration for Standby Stack Unit ID that will take a stackID or "None" as values */
  xLibDataMapRegister ("ppsPercent_t", encode_ppsPercent_t, NULL);

  /* Registration for sFlowIndex_t type */
  xLibDataMapRegister ("sFlowIndex_t", encode_sFlowIndex_t, decode_sFlowIndex_t);

  /* Registration for uspRouteNextHop_t type */
  xLibDataMapRegister ("uspRouteNextHop_t", encode_uspRouteNextHop_t, decode_uspRouteNextHop_t);

  /* Registaration for Protected Port group  that will take a groupID or "None" as values */
  xLibDataMapRegister ("protectedPortGroupId_t", encode_protectedPortGroupId_t, decode_protectedPortGroupId_t);

 /* Registration for OUIVal_t type */
  xLibDataMapRegister ("OUIVal_t", encode_OUIVal_t, decode_OUIVal_t);

 /* Registration for OUIVal_t type */
  xLibDataMapRegister ("cp_hex_str_t", encode_cp_hex_str_t, decode_cp_hex_str_t);

  /* Registration for eventControlTimeFormat_t type */
  xLibDataMapRegister ("eventControlTimeFormat_t", encode_eventControlTimeFormat_t, decode_eventControlTimeFormat_t);

  /* Registration for elapseTimeDHMS_t type */
  xLibDataMapRegister ("formattedTimeDHMS_t", encode_formattedTimeDHMS_t, decode_formattedTimeDHMS_t);

  fpObjRangeHandlersInit();
}

xLibRC_t join_ipv6IpAndMaskLen(xlibValen_t *ipv6IpAndMaskLen,xlibValen_t *ipv6Ip,xlibValen_t *ipv6MaskLen){

 if(strlen(ipv6Ip->value) == 0){
    return XLIBRC_FAILURE;
 }

 if(strlen(ipv6MaskLen->value) == 0){
    sprintf(ipv6IpAndMaskLen->value,"%s",ipv6Ip->value);
 }
 else{
    sprintf(ipv6IpAndMaskLen->value,"%s/%s",ipv6Ip->value,ipv6MaskLen->value);
 }
 ipv6IpAndMaskLen->valen = strlen(ipv6IpAndMaskLen->value);

 return XLIBRC_SUCCESS;

}

xLibRC_t split_ipv6IpAndMaskLen(xlibValen_t  *ipv6IpAndMaskLen, xlibValen_t *ipv6Ip,xlibValen_t *ipv6MaskLen){

 xLibU16_t aclIpv6Index=0,aclIpv6MaskIndex=0;
 xLibU16_t i,j;

 memset(ipv6Ip->value,0,sizeof(ipv6Ip->value));
 memset(ipv6MaskLen->value,0,sizeof(ipv6MaskLen->value));

 if(strlen(ipv6IpAndMaskLen->value) == 0){
    return XLIBRC_FAILURE;
 }

 for(i=0;ipv6IpAndMaskLen->value[i] != '\0';i++){
     if(ipv6IpAndMaskLen->value[i] == '/'){
        for(j=i+1;ipv6IpAndMaskLen->value[j] != '\0';j++){
            ipv6MaskLen->value[aclIpv6MaskIndex++]=ipv6IpAndMaskLen->value[j];
        }
        ipv6Ip->value[aclIpv6Index]='\0';
        ipv6MaskLen->value[aclIpv6MaskIndex]='\0';
        break;
     }
     else{
       ipv6Ip->value[aclIpv6Index++]=ipv6IpAndMaskLen->value[i];
     }
 }


 return XLIBRC_SUCCESS;

}




