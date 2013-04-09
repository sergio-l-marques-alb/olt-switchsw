/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename   l7utils_convert.c
*
* @purpose    Common conversion utilities
*
* @component  l7utils
*
* @comments   none
*
* @create     07/12/2006
*
* @author     gpaussa
* @end
*
**********************************************************************/

#include <stdio.h>
#include <string.h>
#include "l7_common.h"
#include "l7utils_api.h"
#include "osapi.h"
#include "sysapi.h"

static L7_uchar8  *pMacAddrStringTemplate = "xx:xx:xx:xx:xx:xx";
static L7_uchar8  *pMacAddrStringFormat = "%02x:%02x:%02x:%02x:%02x:%02x";

static L7_uchar8  *pOUIStringTemplate = "xx:xx:xx";
static L7_uchar8  *pOUIStringFormat = "%02x:%02x:%02x";

/*********************************************************************
* @purpose  Convert a MAC address from hex characters to an ASCII string
*
* @param    *pHexBuf    @{(input)}  Buffer containing 6-byte hex MAC addr value
* @param    strBufLen   @{(input)}  ASCII string output buffer length
* @param    *pStrBuf    @{(ouput)}  ASCII string output buffer pointer
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The caller must ensure that the input buffer pointed to by
*           pHexBuf is at at least L7_MAC_ADDR_LEN bytes in size.
*
* @comments The caller must ensure the output pStrBuf location is at least
*           18 bytes in length, as indicated in strBufLen.
*
* @comments The output MAC address string format will be formatted exactly
*           as "xx:xx:xx:xx:xx:xx".  That is, two characters are used
*           for each of the six octets in the MAC address, with each octet
*           separated by a colon character.
*
* @end
*********************************************************************/
L7_RC_t l7utilsMacAddrHexToString(L7_uchar8 *pHexBuf, L7_uint32 strBufLen, L7_uchar8 *pStrBuf)
{
  L7_int32      count;

  if ((pHexBuf == L7_NULLPTR) || (pStrBuf == L7_NULLPTR))
  {
    return L7_FAILURE;
  }

  if (strBufLen <= strlen(pMacAddrStringTemplate))  /* '<=' to allow for trailing EOS char */
  {
    return L7_FAILURE;
  }

  count = osapiSnprintf(pStrBuf, strBufLen, pMacAddrStringFormat,
                        *(pHexBuf+0), *(pHexBuf+1), *(pHexBuf+2),
                        *(pHexBuf+3), *(pHexBuf+4), *(pHexBuf+5));

  if (count != strlen(pMacAddrStringTemplate))
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Convert a MAC address from ASCII string to hex characters
*
* @param    *pString    @{(input)}  ASCII string in "xx:xx:xx:xx:xx:xx" format
* @param    *pHexBuf    @{(output)} Output buffer to store 6-byte hex value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The input MAC address string format must be exactly as shown
*           for the pString parameter.  That is, two characters are required
*           for each of the six octets in the MAC address, with each octet
*           separated by a colon character.
*
* @comments The caller must ensure that the output buffer pointed to by
*           pHexBuf is at least L7_MAC_ADDR_LEN bytes in size.
*
* @end
*********************************************************************/
L7_RC_t l7utilsMacAddrStringToHex(L7_uchar8 *pString, L7_uchar8 *pHexBuf)
{
  L7_uint32     temp[L7_MAC_ADDR_LEN];
  L7_uint32     i;
  L7_int32      count;

  if ((pString == L7_NULLPTR) || (pHexBuf == L7_NULLPTR))
  {
    return L7_FAILURE;
  }

  if (strlen(pString) != strlen(pMacAddrStringTemplate))
  {
    return L7_FAILURE;
  }

  count = sscanf(pString, pMacAddrStringFormat,
                 &temp[0], &temp[1], &temp[2], &temp[3], &temp[4], &temp[5]);

  if (count != L7_MAC_ADDR_LEN)
  {
    return L7_FAILURE;
  }

  for (i = 0; i < L7_MAC_ADDR_LEN; i++)
  {
    *(pHexBuf + i) = temp[i];
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Convert a OUI Value from hex characters to an ASCII string
*
* @param    *pHexBuf    @{(input)}  Buffer containing 3-byte hex OUI value
* @param    strBufLen   @{(input)}  ASCII string output buffer length
* @param    *pStrBuf    @{(ouput)}  ASCII string output buffer pointer
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The caller must ensure that the input buffer pointed to by
*           pHexBuf is at at least L7_OUIVAL_LEN bytes in size.
*
* @comments The output OUI VAlue string format will be formatted exactly
*           as "xx:xx:xx".  That is, two characters are used
*           for each of the three octets in the OUI Value, with each octet
*           separated by a colon character.
*
* @end
*********************************************************************/
L7_RC_t l7utilsOUIValHexToString(L7_uchar8 *pHexBuf, L7_uint32 strBufLen, L7_uchar8 *pStrBuf)
{
  L7_int32      count;

  if ((pHexBuf == L7_NULLPTR) || (pStrBuf == L7_NULLPTR))
  {
    return L7_FAILURE;
  }

  if (strBufLen <= strlen(pOUIStringTemplate))  /* '<=' to allow for trailing EOS char */
  {
    return L7_FAILURE;
  }

  count = osapiSnprintf(pStrBuf, strBufLen, pOUIStringFormat,
                        *(pHexBuf+0), *(pHexBuf+1), *(pHexBuf+2));

  if (count != strlen(pOUIStringTemplate))
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Convert a OUI Value from ASCII string to hex characters
* @param    *pString    @{(input)}  ASCII string in "xx:xx:xx" format
* @param    *pHexBuf    @{(output)} Output buffer to store 3-byte hex value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The input OUI Value string format must be exactly as shown
*           for the pString parameter.  That is, two characters are required
*           for each of the 3 octets in the OUI value, with each octet
*           separated by a colon character.
*
* @comments The caller must ensure that the output buffer pointed to by
*           pHexBuf is at least L7_OUIVAL_LEN bytes in size.
*
* @end
*********************************************************************/
L7_RC_t l7utilsOUIValueStringToHex(L7_uchar8 *pString, L7_uchar8 *pHexBuf)
{
  L7_uint32     temp[L7_OUIVAL_LEN];
  L7_uint32     i;
  L7_int32      count;

  if ((pString == L7_NULLPTR) || (pHexBuf == L7_NULLPTR))
  {
    sysapiPrintf("String is Null\n");
    return L7_FAILURE;
  }

  if (strlen(pString) != strlen(pOUIStringTemplate))
  {
    sysapiPrintf("strlen(pString)=%d,strlen(pOUIStringTemplate)=%d\n",strlen(pString),strlen(pOUIStringTemplate));
    return L7_FAILURE;
  }

  count = sscanf(pString, pOUIStringFormat,
                 &temp[0], &temp[1], &temp[2]);

  if (count != L7_OUIVAL_LEN)
  {
    sysapiPrintf("The ouilen is less %d and it has to be %d\n",count,L7_OUIVAL_LEN);
    return L7_FAILURE;
  }

  for (i = 0; i < L7_OUIVAL_LEN; i++)
  {
    *(pHexBuf + i) = temp[i];
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Count number of leading one bits in a generic byte array
*
* @param    *pByteArray @{(input)}  Byte array containing an arbitrary bit mask
* @param    arrayLength @{(input)}  Maximum number of array bytes to consider
* @param    *pResult    @{(output)} Output location to store count of leading 1's
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The bit mask passed to this function via the pByteArray parameter
*           need not be contiguous, although the resulting value only counts
*           the number of contiguous '1' bits encountered from the start of
*           the array.
*
* @end
*********************************************************************/
L7_RC_t l7utilsBitMaskToLeadingOnesCount(L7_uchar8 *pByteArray, L7_uint32 arrayLength,
                                         L7_uint32 *pResult)
{
  static const L7_uchar8 numberOfLeadingOnes[16] =
  {
    0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 3, 4    /* nibble values 0 - 15 */
  };

  L7_uint32     leadingOnesCount = 0;
  L7_uint32     i;
  L7_uchar8     *pByte;
  L7_uchar8     high4, low4;

  if ((pByteArray == L7_NULLPTR) || (pResult == L7_NULLPTR))
  {
    return L7_FAILURE;
  }

  /* process each byte one nibble at a time, stopping whenever a value other
   * than x'F' is found
   *
   * a maskLength of 0 returns a count of 0
   */
  for (i = 0, pByte = pByteArray; i < arrayLength; i++, pByte++)
  {
    high4 = *pByte >> 4;
    low4  = *pByte & 0x0F;

    leadingOnesCount += numberOfLeadingOnes[high4];
    if (high4 != 0x0F)
    {
      break;
    }

    leadingOnesCount += numberOfLeadingOnes[low4];
    if (low4 != 0x0F)
    {
      break;
    }
  } /* endfor */

  *pResult = leadingOnesCount;
  return L7_SUCCESS;
}

L7_uint32 l7utilsBytesToInt(L7_char8 *b)
{
#ifdef PC_LINUX_HOST
  return b[0]<<24 | (b[1]&0xff)<<16 | (b[2]&0xff)<<8 | (b[3]&0xff);
#else
  return *b<<24 | ((*++b)&0xff)<<16 | ((*++b)&0xff)<<8 | ((*++b)&0xff);
#endif
}

void l7utilsIntToBytes(L7_char8 *b, L7_uint32 i)
{
  *b   = (L7_char8)(i>>24);
  *++b = (L7_char8)(i>>16);
  *++b = (L7_char8)(i>>8);
  *++b = (L7_char8)i;
}

