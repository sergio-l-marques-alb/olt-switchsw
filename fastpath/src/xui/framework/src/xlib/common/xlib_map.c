/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename xlib_map.c
 *
 * @purpose
 *
 * @component XLIB
 *
 * @comments
 *
 * @create 04/17/2007
 *
 * @author Rama Sasthri, Kristipati
 * @end
 *
 **********************************************************************/

/*! \file */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "xlib_private.h"

#define MAX_IPV4_ADDR_STRLEN 15
static xLibEncDec_t *xLibEncDecInfo = NULL;
static xLibU16_t xLibEncDecInfoCount = 0;
static xLibU16_t xLibEncDecInfoMax = 0;

static xLibEncDec_t *xLibEncDecInfoFind (xLibU16_t type)
{
  xLibU16_t index;
  xLibEncDec_t *infop = xLibEncDecInfo;
  xLibU16_t size = xLibEncDecInfoCount;

  for (index = 0; index < size; index++, infop++)
  {
    if (infop->type == type)
    {
      return infop;
    }
  }
  return NULL;
}

xLibEnumInfo_t *xLibEnumInfoFind (xLibU16_t type, xLibU16_t * left)
{
  xLibU16_t size;
  xLibU16_t index;
  xLibEnumInfo_t *infop;

  infop = xLibEnumInfoGet (&size);
  if (infop == NULL)
  {
    return NULL;
  }

  for (index = 0; index < size; index++, infop++)
  {
    if (infop->type == type)
    {
      *left = size - index;
      return infop;
    }
  }
  return NULL;
}

/*
  This function initalizes the enums for the first time
  previously when we access the UI(web page) for the 
  first time, memory for enums was being allocated.
  This can look like a memory leak to the user.
  To avoid it we are initalizing the enums before
*/

void xLibEnumInfoInit()
{
  xLibEnumInfo_t *infop = NULL;
	xLibU16_t size = 0;
		
  infop = xLibEnumInfoGet (&size);
  if(infop  == NULL)
  {
    XLIB_ERROR("Failed to initialize the enums!!!");
    return;
  }
  
}

void xLibEnumInfoSet (xLibEnumInfo_t * infop, xLibU16_t type, xLibU32_t ewVal,
                      xLibU32_t appVal, xLibCS8_t * webName, xLibCS8_t * cliName,
                      xLibCS8_t * optType)
{
  infop->type = type;
  infop->appVal = appVal;
  infop->ewVal = ewVal;
  infop->webName = webName;
  infop->cliName = cliName;
  infop->optType = optType;
}

xLibRC_t xLibDataMapRegister (char *name, xLibEncodeFunc_t encode, xLibDecodeFunc_t decode)
{
  xLibU16_t index;
  xLibEncDec_t *infop = xLibEncDecInfo;
  xLibU16_t size = xLibEncDecInfoCount;

  for (index = 0; index < size; index++, infop++)
  {
    if (!strcmp (infop->name, name))
    {
      infop->encode = encode;
      infop->decode = decode;
      return XLIBRC_SUCCESS;
    }
  }
  return XLIBRC_FAILURE;
}

/*
 * All the standard encode and decode functions provided by XLIB default are below
 */
xLibRC_t xLibEncode_uint (xLibIf_t intf, xLibU8_t * in, xLibU16_t inLen, char *out,
                          xLibU16_t * outLen)
{
  if (inLen == sizeof(xLibU8_t))
  {
    xLibU8_t val;
    memcpy (&val, in, inLen);
    sprintf (out, "%u", val);
  }
  else if (inLen == sizeof(xLibU16_t))
  {
    xLibU16_t val;
    memcpy (&val, in, inLen);
    sprintf (out, "%u", val);
  }
  else if (inLen == sizeof(xLibU32_t))
  {
    xLibU32_t val;
    memcpy (&val, in, inLen);
    sprintf (out, "%u", val);
  }
  else if (inLen == sizeof(xLibUL64_t))
  {
    xLibUL64_t val;
    memcpy (&val, in, inLen);
    sprintf (out, "%llu", val);
  }
  else
  {
    XLIB_WARN ("Invalid length for encode = %d", inLen);
    return XLIBRC_ENCODE_FAIL;
  }
  *outLen = strlen (out);
  return XLIBRC_SUCCESS;
}

xLibRC_t xLibEncode_int (xLibIf_t intf, xLibU8_t * in, xLibU16_t inLen, char *out,
                         xLibU16_t * outLen)
{
  if (inLen == sizeof(xLibS8_t))
  {
    xLibS8_t val;
    memcpy (&val, in, inLen);
    sprintf (out, "%d", val);
  }
  else if (inLen == sizeof(xLibS16_t))
  {
    xLibS16_t val;
    memcpy (&val, in, inLen);
    sprintf (out, "%d", val);
  }
  else if (inLen == sizeof(xLibS32_t))
  {
    xLibS32_t val;
    memcpy (&val, in, inLen);
    sprintf (out, "%d", val);
  }
  else
  {
    XLIB_WARN ("Invalid length for encode = %d", inLen);
    return XLIBRC_ENCODE_FAIL;
  }
  *outLen = strlen (out);
  return XLIBRC_SUCCESS;
}
xLibRC_t xLibSpecialCharValGet(xLibU8_t *in, xLibU16_t inLen, char *out)
{
  xLibS8_t tmpString[XLIB_MAX_VAL_LEN];
  xLibS8_t buf[XLIB_BUF_SIZE_100];
  xLibU32_t i = 0;
  xLibU32_t j = 0;
  xLibBool_t status_flag;

  memset(tmpString, 0x00, sizeof(tmpString));
  osapiStrncpy(tmpString, (char *) in, inLen);
  
  while (tmpString[i] != '\0')
  {
    status_flag = XLIB_FALSE;
    switch(tmpString[i])
    {
      case '<':
      sprintf(buf, "%s", "BRCM_escape_lt;");
      status_flag = XLIB_TRUE;
      break;

      case '"':
      sprintf(buf, "%s", "&quot;");
      status_flag = XLIB_TRUE;
      break;

      case '>':
      sprintf(buf, "%s", "BRCM_escape_gt;");
      status_flag = XLIB_TRUE;
      break;

      case 39:
      sprintf(buf, "%s", "BRCM_escape_apos;");
      status_flag = XLIB_TRUE;
      break;

      default:
      status_flag = XLIB_FALSE;
      break;
    }

    if (status_flag != XLIB_FALSE)
    {
      strcat(out, buf);
      j = j + strlen(buf);
    }
    else
    {
      out[j] = tmpString[i];
      j++;
    }
    i++;
  }

  return XLIBRC_SUCCESS;
}

xLibRC_t xLibEncode_string (xLibIf_t intf, xLibU8_t * in, xLibU16_t inLen, char *out,
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


xLibRC_t xLibEncode_ipv4 (xLibIf_t intf, xLibU8_t * in, xLibU16_t inLen, char *out,
                          xLibU16_t * outLen)
{
  xLibU32_t ipAddr;
  memcpy (&ipAddr, in, sizeof (ipAddr));
  ipAddr = (xLibU32_t) osapiNtohl (ipAddr);
  memcpy (in, &ipAddr, sizeof (ipAddr));
  osapiInetNtop (L7_AF_INET, in, (void *) out, *outLen);
  *outLen = strlen (out);
  return XLIBRC_SUCCESS;
}

xLibRC_t xLibEncode_ipv6 (xLibIf_t intf, xLibU8_t * in, xLibU16_t inLen, char *out,
                          xLibU16_t * outLen)
{
  osapiInetNtop (L7_AF_INET6, in, (void *) out, *outLen);
  *outLen = strlen (out);
  return XLIBRC_SUCCESS;
}

xLibRC_t xLibEncode_mac (xLibIf_t intf, xLibU8_t * m, xLibU16_t inLen, char *out,
                         xLibU16_t * outLen)
{
  osapiSnprintf (out, *outLen, "%02X:%02X:%02X:%02X:%02X:%02X", m[0], m[1], m[2], m[3], m[4], m[5]);
  *outLen = strlen (out);
  return XLIBRC_SUCCESS;
}

xLibRC_t xLibDecode_uint (xLibIf_t intf, xLibS8_t * in, xLibU8_t * out, xLibU16_t * outLen)
{
  xLibU32_t tmpint;

  memset ((void *)&tmpint, 0x0, sizeof (tmpint));
  
  if (osapiStrtoul(in, 10, &tmpint)!= L7_SUCCESS)
  {
     return XLIBRC_DECODE_FAIL;
  }

  /* return the decoded value */
  memcpy (out, &tmpint, sizeof (tmpint));
  *outLen = sizeof (tmpint);

  return XLIBRC_SUCCESS;
}

xLibRC_t xLibDecode_int (xLibIf_t intf, xLibS8_t * in, xLibU8_t * out, xLibU16_t * outLen)
{
  signed long tmp ;
  memset ((void *)&tmp, 0x0, sizeof (tmp));

  tmp = strtol(in, NULL, 10);

  /* return the decoded value */
  memcpy (out, &tmp, sizeof (tmp));
  *outLen = sizeof (tmp);

  return XLIBRC_SUCCESS;

}  

xLibRC_t xLibDecode_string (xLibIf_t intf, xLibS8_t * in, xLibU8_t * out, xLibU16_t * outSizeLen)
{
  xLibU16_t len = strlen (in);
  osapiStrncpy ((xLibS8_t *) out, in, len);
  *outSizeLen = len + 1;        /* account the terminating null also */
  out[len] = 0;
  return XLIBRC_SUCCESS;
}

xLibRC_t xLibDecode_ipv4 (xLibIf_t intf, xLibS8_t * in, xLibU8_t * out, xLibU16_t * outSizeLen)
{
  xLibU8_t buff[MAX_IPV4_ADDR_STRLEN+1];
  xLibU8_t *str_token;
  xLibU8_t outbuff[MAX_IPV4_ADDR_STRLEN+1];
  xLibU8_t delim[]=".";
  xLibU32_t len;
  xLibU32_t count=0;
  xLibU32_t i=0;
  xLibU32_t index;
  xLibU32_t int_token[4];
  xLibU32_t ipAddr;
  if((in == NULL)||(strlen(in)==0)||(strlen(in)>MAX_IPV4_ADDR_STRLEN ))
  {
    return XLIBRC_DECODE_FAIL;
  }
  memset(buff, 0x0, sizeof(buff));
  memset(outbuff, 0x0, sizeof(outbuff));
  memcpy(buff,in,strlen(in));
  str_token = strtok(buff,delim);
  i = 0;
  count = 0;
  index =0;
  while( str_token != NULL)
  {
   len=strlen(str_token);
   if( (len <= 0 ) ||  (len >=4) ){
    return XLIBRC_DECODE_FAIL;
   }
   i =0;
   do {
   if(!(isdigit( (unsigned char) str_token[i])))
       return XLIBRC_DECODE_FAIL;
     i++;
   } while (i<len);
   int_token[index] = atoi(str_token);
   str_token = strtok(NULL,delim);
   index++;
   }
  
  if(index !=4)
  {
    return XLIBRC_DECODE_FAIL;
  }
  osapiSnprintf(outbuff, MAX_IPV4_ADDR_STRLEN+1, "%d.%d.%d.%d",int_token[0],int_token[1],int_token[2],int_token[3]); 
  if(osapiInetPton (L7_AF_INET, (xLibU8_t *) outbuff, (void *) out) != L7_SUCCESS)
  {
    return XLIBRC_DECODE_FAIL;
  }
  memcpy (&ipAddr, out, sizeof (ipAddr));
  ipAddr = osapiHtonl (ipAddr);
  memcpy (out, &ipAddr, sizeof (ipAddr));
  *outSizeLen = sizeof(xLibIpV4_t);
  return XLIBRC_SUCCESS;
}

xLibRC_t xLibDecode_ipv6 (xLibIf_t intf, xLibS8_t * in, xLibU8_t * out, xLibU16_t * outSizeLen)
{
  if(osapiInetPton (L7_AF_INET6, (xLibU8_t *) in, (void *) out) != L7_SUCCESS)
  {
    return XLIBRC_DECODE_FAIL;
  }
  *outSizeLen = sizeof(xLibIpV6_t);
  return XLIBRC_SUCCESS;
}

xLibRC_t xLibDecode_mac (xLibIf_t intf, xLibS8_t * in, xLibU8_t * out, xLibU16_t * outSizeLen)
{
  *outSizeLen = xLibUtilHexDecode (in, out, sizeof(xLibMac_t), ':');
  return XLIBRC_SUCCESS;
}

/****************************** END OF encode/decode funcs **************/
void xLibTypeInfoDataInit (int max)
{
  /* This memory doesn't get freed up and it is expected behavior */
  xLibEncDecInfo = (xLibEncDec_t *) xLibMalloc (max * sizeof (*xLibEncDecInfo));
  XLIB_ASSERT (xLibEncDecInfo);
  xLibEncDecInfoCount = 0;
  xLibEncDecInfoMax = max;
}

void xLibTypeInfoSet (char *name, int type, xLibEncodeFunc_t encode,
                      xLibDecodeFunc_t decode)
{
  xLibEncDec_t *infop;

  XLIB_ASSERT (xLibEncDecInfo);
  XLIB_ASSERT (xLibEncDecInfoCount < xLibEncDecInfoMax);

  infop = &xLibEncDecInfo[xLibEncDecInfoCount++];
  infop->name = xLibStrDup (name);
  infop->type = type;
  infop->encode = encode;
  infop->decode = decode;
}

xLibRC_t xLibTypeEncode (xLibIf_t intf, xLibU16_t type, xLibU8_t * inBuf, xLibU16_t inLen,
                         xLibS8_t * outBuf, xLibU16_t * outSizeLen, xLibBool_t dyn)
{
  xLibU16_t left = 0;
  xLibU32_t appVal = 0;
  xLibEnumInfo_t *enump;
  xLibEncDec_t *infop;
  xLibU16_t min;

  min = xLibTypeGetMinEnum ();
  if (type < min)
  {
    infop = xLibEncDecInfoFind (type);
    if (infop == NULL)
    {
      XLIB_TRACE ("TODO: Failed to get encode function for type %d", type);
      return XLIBRC_ENCODE_FAIL;
    }
    
    memset(outBuf, 0, *outSizeLen);
    return infop->encode (intf, inBuf, inLen, outBuf, outSizeLen);
  }

  enump = xLibEnumInfoFind (type, &left);
  if (enump == NULL)
  {
    XLIB_TRACE ("TODO");
    return XLIBRC_ENCODE_FAIL;
  }

  memcpy (&appVal, inBuf, sizeof (appVal));
  for (; left-- > 0; enump++)
  {
    if (enump->type != type)
    {
      XLIB_TRACE ("TODO: CMP %d %d: min = %d left = %d appVal = %d",
                  enump->type, type, min, left, appVal);
      return XLIBRC_ENCODE_FAIL;
    }

    /* Match the enum with value to be encoded */
    if (enump->appVal != appVal)
    {
      continue;
    }

    if (dyn == XLIB_TRUE && intf == XLIB_IF_WEB)
    {
      osapiSnprintf (outBuf, *outSizeLen, "%s", enump->webName);
      *outSizeLen = strlen ((char *) enump->webName);
    }
    else if (intf == XLIB_IF_WEB)
    {
      memcpy (outBuf, &enump->ewVal, sizeof (enump->ewVal));
      *outSizeLen = sizeof (enump->ewVal);
    }
    else if (intf == XLIB_IF_CLI)
    {
      osapiSnprintf (outBuf, *outSizeLen, "%s", enump->cliName);
      *outSizeLen = strlen ((char *) enump->cliName);
    }
    else
    {
      sprintf (outBuf, "%u", enump->appVal);
      *outSizeLen = sizeof (enump->appVal);
    }
    return XLIBRC_SUCCESS;
  }
  XLIB_TRACE ("Failed to locate enum for appVal = 0x%x type = %d\r\n", appVal, type);
  return XLIBRC_ENCODE_FAIL;
}

xLibRC_t xLibTypeDecode (xLibIf_t intf, xLibU16_t type, xLibS8_t * inBuf, xLibU8_t * outBuf,
                         xLibU16_t * outSizeLen, xLibBool_t dyn)
{
  xLibU16_t left = 0;
  xLibU32_t ewVal = 0;
  xLibU32_t snmpVal = 0;
  xLibEnumInfo_t *enump;
  xLibEncDec_t *infop;

  if (type < xLibTypeGetMinEnum ())
  {
    infop = xLibEncDecInfoFind (type);
    if (infop == NULL)
    {
      XLIB_TRACE ("TODO: Failed to find decode function for type %d\r\n", type);
      return XLIBRC_DECODE_FAIL;
    }
    return infop->decode (intf, inBuf, outBuf, outSizeLen);
  }

  enump = xLibEnumInfoFind (type, &left);
  if (enump == NULL)
  {
    XLIB_TRACE ("TODO: Failed to find enum info for type %d min = %d", type, xLibTypeGetMinEnum ());
    return XLIBRC_DECODE_FAIL;
  }

  memcpy (&ewVal, inBuf, sizeof (ewVal));
  sscanf (inBuf, "%d", &snmpVal);
  for (; left-- > 0; enump++)
  {
    if (enump->type != type)
    {
      XLIB_TRACE ("TODO: CMP %d %d: min = %d left = %d", enump->type, type, xLibTypeGetMinEnum (),
                  left);
      return XLIBRC_DECODE_FAIL;
    }

    if (intf == XLIB_IF_WEB)
    {
      if (dyn == XLIB_TRUE)
      {
        if (strcmp (enump->webName, (char *) inBuf))
        {
          continue;
        }
      }
      else if (enump->ewVal != ewVal)
      {
        continue;
      }
      memcpy (outBuf, &enump->appVal, sizeof (enump->appVal));
      *outSizeLen = sizeof (enump->appVal);
      return XLIBRC_SUCCESS;
    }
    else if (intf == XLIB_IF_CLI)
    {
      if (strcmp (enump->cliName, (char *) inBuf))
      {
        continue;
      }
      memcpy (outBuf, &enump->appVal, sizeof (enump->appVal));
      *outSizeLen = sizeof (enump->appVal);
      return XLIBRC_SUCCESS;
    }
    else
    {
      if (enump->appVal != snmpVal)
      {
        continue;
      }
      memcpy (outBuf, &enump->appVal, sizeof (enump->appVal));
      *outSizeLen = sizeof (enump->appVal);
      return XLIBRC_SUCCESS;
    }
  }
  XLIB_TRACE ("Failed to locate enum for ewVal = 0x%x type = %d\r\n", ewVal, type);
  return XLIBRC_DECODE_FAIL;
}

xLibRC_t xLibTypeColate (xLibIf_t intf, xLibU16_t type, xLibS8_t * inBuf, xLibU16_t inLen,
                         void **colWap)
{
  char *str = *colWap;

  if (str == NULL)
  {
    /* this gets freed in xLibTypeColateFinish */
    str = *colWap = xLibMalloc (1024);
  }
  else
  {
    strcat (str, ",");
  }

  strcat (str, inBuf);

  XLIB_TRACE ("colated: %s\r\n", str);

  return XLIBRC_SUCCESS;
}

char *xLibTypeColatedGet (void **colWap)
{
  return (char *) (*colWap);
}

void *xLibTypeColateFinish (void **colWap)
{
  char *str = *colWap;
  if (str != NULL)
  {
    xLibFree (str);
    *colWap = NULL;
  }
  return NULL;
}

xLibRC_t xLibRowInfoCheck (xLibId_t oid, xLibU16_t type, xLibU8_t * val)
{
  xLibU16_t left = 0;
  xLibU32_t appVal = 0;
  xLibEnumInfo_t *enump;

  enump = xLibEnumInfoFind (type, &left);
  if (enump == NULL)
  {
    XLIB_TRACE ("TODO: Failed to find row_status info for type %d min = %d", type,
                xLibTypeGetMinEnum ());
    return XLIBRC_DECODE_FAIL;
  }
  memcpy (&appVal, val, sizeof (val));
  for (; left-- > 0; enump++)
  {
    if (enump->type != type)
    {
      XLIB_TRACE ("TODO: CMP %d %d: min = %d left = %d", enump->type, type, xLibTypeGetMinEnum (),
                  left);
      return XLIBRC_DECODE_FAIL;
    }
    if (enump->appVal != appVal)
    {
      continue;
    }

    /* check on the type value */
    if ((strcmp (enump->optType, "delete") == 0))
    {
      return XLIBRC_ROW_STATUS_DELETE;
    }
    else if ((strcmp (enump->optType, "add") == 0))
    {
      return XLIBRC_ROW_STATUS_ADD;
    }
  }
  return XLIBRC_SUCCESS;
}

xLibRC_t xLibEncode_uint64 (xLibIf_t intf, xLibU8_t * in, xLibU16_t inLen, char *out,
                            xLibU16_t * outLen)
{
  xLibU64_t num;

  num.high = ((xLibU64_t *) in)->high;
  num.low = ((xLibU64_t *) in)->low;

  xLibUtil64BitsToString (num, out);

  *outLen = strlen (out);
  return XLIBRC_SUCCESS;
}

xLibRC_t xLibDecode_uint64 (xLibIf_t intf, xLibS8_t * in, xLibU8_t * out, xLibU16_t * outLen)
{
  return XLIBRC_SUCCESS;
}

