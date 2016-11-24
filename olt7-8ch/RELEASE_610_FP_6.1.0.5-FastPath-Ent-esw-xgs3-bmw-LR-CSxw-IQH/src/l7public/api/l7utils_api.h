/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename   l7utils_api.h
*
* @purpose    API function prototypes for general LVL7 utility functions
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

#ifndef _L7UTILS_API_H_
#define _L7UTILS_API_H_

#include "l7_common.h"


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
L7_RC_t l7utilsMacAddrHexToString(L7_uchar8 *pHexBuf, L7_uint32 strBufLen, L7_uchar8 *pStrBuf);

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
L7_RC_t l7utilsMacAddrStringToHex(L7_uchar8 *pString, L7_uchar8 *pHexBuf);

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

L7_RC_t l7utilsOUIValHexToString(L7_uchar8 *pHexBuf, L7_uint32 strBufLen, L7_uchar8 *pStrBuf);

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
L7_RC_t l7utilsOUIValueStringToHex(L7_uchar8 *pString, L7_uchar8 *pHexBuf);


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
                                         L7_uint32 *pResult);

/*********************************************************************
* @purpose  Given a pointer to full path of a file, change the pointer
*           to point to the beginning of the file name
*
* @param    **fullPath    @{(input)}  Address of pointer to full path to file
*
* @returns  none
*
* @end
*********************************************************************/
void l7utilsFilenameStrip(L7_char8 **fullPath);

#endif /* _L7UTILS_API_H_ */
