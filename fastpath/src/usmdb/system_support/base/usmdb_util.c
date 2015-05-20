/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
*
* @filename usmdb_util.c
*
* @purpose Provide interface to hardware API's for unitmgr components
*
* @component unitmgr
*
* @comments tba
*
* @create 03-Nov-2000
*
* @author gaunce
* @end
*
**********************************************************************/

/* Copyright (c) 1996 by Internet Software Consortium.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all
 * copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND INTERNET SOFTWARE CONSORTIUM
 * DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL
 * INTERNET SOFTWARE CONSORTIUM BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * The following portions of this file are governed by the immediately
 * preceding copyright notice:
 *
 *     usmDbInetAton()
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "l7_common.h"
#include "usmdb_cnfgr_api.h"
#include "usmdb_common.h"
#include "usmdb_dot3ad_api.h"
#include "usmdb_ip_api.h"
#include "usmdb_mirror_api.h"
#include "usmdb_util_api.h"
#include "comm_mask.h"

#include "statsapi.h"
#include "osapi.h"
#include "osapi_support.h"
#include "nimapi.h"
#include "default_cnfgr.h"
#include "dot3ad_api.h"
#include "cnfgr.h"
#include "dhcp_bootp_api.h"
#include "usmdb_dns_client_api.h"
#include "l7utils_api.h"
#include "nvstoreapi.h"
#include "usmdb_slotmapper.h"
#include "dhcps_exports.h"
#include "poe_exports.h"
#include "cli_web_mgr_api.h"
#include "cda_api.h"
#include "sdm_api.h"
#include "user_mgr_util.h"

/* both of these for usmDbAton */
#include <ctype.h>
#ifdef _L7_OS_ECOS_
int isascii(int c);
#endif

extern L7_RC_t osapiCpuUtilizationStringGet(L7_char8 *buffer, L7_uint32 bufferSize);

/*********************************************************************
*
* @purpose Convert double64 integer into two unsigned integers.
*
* @param L7_double64 d64       double64 value
* @param L7_uint32   *highend  pointer to uint32 to recieve high order bits
* @param L7_uint32   *lowend   pointer to uint32 to recieve low order bits
*
* @returns L7_SUCCESS always
*
* @notes If any recieving value pointer is NULL, it will skip that part.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSplitDouble64(L7_double64 d64, L7_uint32 *highend, L7_uint32 *lowend)
{
  L7_uint32 *d64high;
  L7_uint32 *d64low;

  d64high = ( void* )&d64;                                                /* point to first 32 bits */
  d64low = d64high + ( sizeof(L7_uint32) );                                    /* point to second 32 bits */

  if (highend != NULL)
    *highend = *d64high;

  if (lowend != NULL)
    *lowend = *d64low;                                                         /* Mask low 32 bits */

  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose convert the provided char array into a 32 bit unsigned integer
*          the value is >= 0 and <= 4294967295.
*
* @param    buf     @b{(input)} character buffer which needs to be converted.
* @param    pVal    @b{(output)} converted integer  
*
* @returns  L7_SUCCESS  means that all chars are integers and together
*              they represent a valid 32 bit unsigned integer
* @returns  L7_FAILURE  means the value does not represent a valid
*              32 bit unsigned integer.  I.e. the value is negative, larger
*              than the max allowed 32 bit int or a non-numeric character
*              is included in buf.
*
*
* @end
*
*********************************************************************/
L7_RC_t convertTo32BitUnsignedInteger(const L7_char8 *buf, L7_uint32 *pVal)
{
  L7_int32 f;
  L7_int32 j;
  L7_uint32 tempval;
#define MAXUINT 4294967295U

  if ( buf[0] == '-' )
  {
    return L7_FAILURE;
  }

  j = strlen(buf);

  if ( j > 10 )
  {
    return L7_FAILURE;
  }

  for ( *pVal = 0, f=1, j=j-1; j >= 0; j--, f=f*10)
  {
    tempval = (L7_uint32)(buf[j] -'0');

    if ( ( tempval > 9 ) || ( tempval < 0 ) )
    {
      return L7_FAILURE;
    }

    tempval = tempval * f;

    if ( (MAXUINT - tempval) < *pVal )
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

/****************************************************************************
*
* @purpose Convert double64 integer into two unsigned integers.
*
* @param L7_uint32   *highend  pointer to uint32 to recieve high order bits
* @param L7_uint32   *lowend   pointer to uint32 to recieve low order bits
* @param L7_double64 d64       double64 value
*
* @returns L7_SUCCESS always
*
* @notes If any recieving value pointer is NULL, it will skip that part.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCreateDouble64(L7_uint32 highend, L7_uint32 lowend, L7_double64 *d64)
{
  L7_uint32 *d64high;
  L7_uint32 *d64low;

  d64high = ( L7_uint32* )d64;                                                 /* point to first 32 bits */
  d64low = d64high + ( sizeof(L7_uint32) );                                    /* point to second 32 bits */

  *d64high = highend;
  *d64low = lowend;

  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose Check to see if IPv4 address/subnet mask pairs are in the
*          same subnet.
*
* @param L7_uint32 ipaddr1   IPv4 address 1
* @param L7_uint32 netmask1  netmask 1
* @param L7_uint32 ipaddr2   IPv4 address 2
* @param L7_uint32 netmask2  netmask 2
*
* @returns L7_SUCCESS, if ipaddr1/netmask1 and ipaddr2/netmask2
*                      are in different subnets.
* @returns L7_FAILURE, if ipaddr1/netmask1 and ipaddr2/netmask2
*                      are in the same subnet.
*
* @notes    If either address or mask is 0, then they are not considered
*           to be in the same subnet.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSubnetCompare(L7_uint32 ipaddr1, L7_uint32 netmask1,
                           L7_uint32 ipaddr2, L7_uint32 netmask2)
{
  if ((ipaddr1 == 0) || (ipaddr2 == 0) || (netmask1 == 0) || (netmask2 == 0))
    return L7_SUCCESS;

  if ((netmask1 & netmask2 & ipaddr1) == (netmask1 & netmask2 & ipaddr2))
    return L7_FAILURE;
  else
    return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose Check to see if ip address/subnet mask/gateway triad is valid.
*
* @param L7_uint32 ipaddr   ip address
* @param L7_uint32 netMask  net mask
* @param L7_uint32 gateway  gateway
*
* @returns L7_SUCCESS, if triad is valid
* @returns L7_FAILURE, if triad is invalid
*
* @notes    none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbIpInfoValidate(L7_uint32 ipAddr, L7_uint32 netMask,
                            L7_uint32 gateway)
{
  if ((usmDbIpAddressValidate(ipAddr, netMask) == L7_SUCCESS) &&
      ((gateway == 0) ||
       ((usmDbIpAddressValidate(gateway, netMask) == L7_SUCCESS) &&
        ((ipAddr & netMask) == (gateway & netMask)))))
    return L7_SUCCESS;
  else
    return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Verify that a network mask is contiguous.
*
* @param   netMask @b{(input)} IPv4 network mask
*
* @returns L7_TRUE, if netMask is contiguous.
* @returns L7_FALSE, otherwise
*
* @notes
*
* @end
*
*********************************************************************/
L7_BOOL usmDbNetmaskIsContiguous(L7_uint32 netMask)
{
    switch (netMask)
  {
    case 0xFFFFFFFFUL: /* 255.255.255.255 passthru */
    case 0xFFFFFFFEUL: /* 255.255.255.254 passthru */
    case 0xFFFFFFFCUL: /* 255.255.255.252 passthru */
    case 0xFFFFFFF8UL: /* 255.255.255.248 passthru */
    case 0xFFFFFFF0UL: /* 255.255.255.240 passthru */
    case 0xFFFFFFE0UL: /* 255.255.255.224 passthru */
    case 0xFFFFFFC0UL: /* 255.255.255.192 passthru */
    case 0xFFFFFF80UL: /* 255.255.255.128 passthru */
    case 0xFFFFFF00UL: /* 255.255.255.0 passthru */
    case 0xFFFFFE00UL: /* 255.255.254.0 passthru */
    case 0xFFFFFC00UL: /* 255.255.252.0 passthru */
    case 0xFFFFF800UL: /* 255.255.248.0 passthru */
    case 0xFFFFF000UL: /* 255.255.240.0 passthru */
    case 0xFFFFE000UL: /* 255.255.224.0 passthru */
    case 0xFFFFC000UL: /* 255.255.192.0 passthru */
    case 0xFFFF8000UL: /* 255.255.128.0 passthru */
    case 0xFFFF0000UL: /* 255.255.0.0 passthru */
    case 0xFFFE0000UL: /* 255.254.0.0 passthru */
    case 0xFFFC0000UL: /* 255.252.0.0 passthru */
    case 0xFFF80000UL: /* 255.248.0.0 passthru */
    case 0xFFF00000UL: /* 255.240.0.0 passthru */
    case 0xFFE00000UL: /* 255.224.0.0 passthru */
    case 0xFFC00000UL: /* 255.192.0.0 passthru */
    case 0xFF800000UL: /* 255.128.0.0 passthru */
    case 0xFF000000UL: /* 255.0.0.0 passthru */
    case 0xFE000000UL: /* 254.0.0.0 passthru */
    case 0xFC000000UL: /* 252.0.0.0 passthru */
    case 0xF8000000UL: /* 248.0.0.0 passthru */
    case 0xF0000000UL: /* 240.0.0.0 passthru */
    case 0xE0000000UL: /* 224.0.0.0 passthru */
    case 0xC0000000UL: /* 192.0.0.0 passthru */
    case 0x80000000UL: /* 128.0.0.0 passthru */

        return L7_TRUE;
        break;

    default:
        return L7_FALSE;
        break;
  }
}

/*********************************************************************
*
* @purpose Verify that a network mask is a valid network mask for
*          an IP address assigned to an interface.
*
* @param   netMask @b{(input)} IPv4 network mask
*
* @returns L7_SUCCESS, if netMask is valid
* @returns L7_FAILURE, if netMask is invalid
*
* @notes   see RFC 1878. Verifies that the netmask is contiguous.
*          Does not allow a 31 or 32 bit netmask.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbNetmaskValidate(L7_uint32 netMask)
{
    if (usmDbNetmaskIsContiguous(netMask) == L7_FALSE)
    {
        return L7_FAILURE;
    }
    if ((netMask == osapiHtonl(0xFFFFFFFFUL)) || (netMask == osapiHtonl(0xFFFFFFFEUL)))
    {
        return L7_FAILURE;
    }
    return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose Verify that a network mask is a valid (up to 32 bit) network mask for
*          an IP address assigned to an interface.
*
* @param   netMask @b{(input)} IPv4 network mask
*
* @returns L7_SUCCESS, if netMask is valid
* @returns L7_FAILURE, if netMask is invalid
*
* @notes   Verifies that the netmask is contiguous.
*          Does allow a 31 or 32 bit netmask.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbNetmaskValidate32(L7_uint32 netMask)
{
    if (usmDbNetmaskIsContiguous(netMask) == L7_FALSE)
        return L7_FAILURE;
    else
       return L7_SUCCESS;
}



/*********************************************************************
*
* @purpose Check to see if ip address is within the valid range of addresses
*
* @param L7_uint32 ipAddress  IPv4 address
* @param L7_uint32 netMask    IPv4 netmask
*
* @returns L7_SUCCESS, if ipAddress/netMask is valid
* @returns L7_FAILURE, if ipAddress/netMask is invalid
*
* @notes   Checks that the ipAddress is not restricted.
*          If the netmask is not 0, it also checks that the ipAddress
*          is neither a network address, nor a broadcast address.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbIpAddressValidate(L7_uint32 ipAddress, L7_uint32 netMask)
{
  /* If a valid netmask is given, check the network/host portions of the address */
  if (netMask != 0x0000000UL &&
      ((usmDbNetmaskValidate(netMask) != L7_SUCCESS) ||  /* Validate the netmask */
       ((~netMask & ipAddress) == 0x00000000UL) ||       /* Network Address: If host bits are all 0 */
       ((~netMask & ipAddress) == ~netMask)))            /* Broadcast Address: If host bits are all 1 */
    return L7_FAILURE;

  /* Check that the address is not restricted */
  if ((ipAddress > 0x01000000UL) &&                   /* 0xE0000000 ==   1.0.0.0 */
      (ipAddress < 0xE0000000UL) &&                   /* 0xE0000000 == 224.0.0.0 */
      (ipAddress & 0xFF000000UL) != 0x7F000000UL)     /* 0x7F000000 == 127.0.0.0 */
    return L7_SUCCESS;

  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Check to see if the netowrk address is within the valid range of addresses
*
* @param L7_uint32 ipAddress  IPv4 address
*
* @returns L7_SUCCESS, if network address is valid
* @returns L7_FAILURE, if netowkr address is invalid
*
* @notes   Checks that the ipAddress is not restricted.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbNetworkAddressValidate(L7_uint32 ipAddress)
{
  /* Check that the address is not restricted */
  if ((ipAddress > 0x01000000UL) &&                   /* 0xE0000000 ==   1.0.0.0 */
      (ipAddress < 0xE0000000UL) &&                   /* 0xE0000000 == 224.0.0.0 */
      ((ipAddress & 0xFF000000UL) != 0x7F000000UL))   /* 0x7F000000 == 127.0.0.0 */
    return L7_SUCCESS;

  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Convert a 64 bit counter to a string.
*
* @param L7_ulong64 num   64 bit value
* @param L7_char8   *buf  dotted string representation
*
* @returns L7_SUCCESS, or
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDb64BitsToString(L7_ulong64 num, L7_char8 *buf)
{

  L7_uint32 i;
  L7_ulong32 value[4], part, carry = 0;

#define MAX_VALUE 4440000U


  /* If no num */
  if (&num == NULL)
  {
    return L7_FAILURE;
  }

  /* If only a 32 bit counter */
  if (num.high == 0)
  {
    sprintf(buf, "%lu", (L7_ulong32) num.low);
    return L7_SUCCESS;
  }

  /* If 64 bit counter, then initialize value[] */
  for (i = 0; i < 4; i++)
  {
    value[i] = 0;
  }

  /* calculate the "high" part */
  part = num.high;
  while ( part > MAX_VALUE )
  {
    value[0] += (67296 * MAX_VALUE);
    if (value[0] >= 100000)
    {
      carry = value[0] / 100000;
      value[0] = value[0] % 100000;
    }

    value[1] += (42949 * MAX_VALUE) + carry;
    carry = 0;
    if (value[1] >= 100000)
    {
      value[2] += (value[1] / 100000);
      value[1] = value[1] % 100000;
    }
    part -= MAX_VALUE;
  }

  value[0] += (67296 * part);
  if (value[0] >= 100000)
  {
    carry = value[0] / 100000;
    value[0] = value[0] % 100000;
  }

  value[1] += (42949 * part) + carry;
  carry = 0;
  if (value[1] >= 100000)
  {
    value[2] += (value[1] / 100000);
    value[1] = value[1] % 100000;
  }

  if (value[2] >= 100000 )
  {
    value[3] = (value[2] / 100000);
    value[2] = value[2] % 100000;
  }


  /* now add the "low" part */
  part = num.low;
  for (i = 0; i < 2; i++)
  {
    value[i] += part % 100000;
    if (value[i] >= 100000)
    {
      value[i+1] += value[i] / 100000;
      value[i] = value[i] % 100000;
    }
    part = part / 100000;
  }

  if (value[2] >= 100000)
  {
    value[3] += value[2] / 100000;
    value[2] = value[2] % 100000;
  }


  /* buf holds the 64 bit value */
  if (value[3])
    sprintf(buf,"%lu%05lu%05lu%05lu",value[3],value[2],value[1],value[0]);
  else if (value[2])
    sprintf(buf,"%lu%05lu%05lu",value[2],value[1],value[0]);
  else
    sprintf(buf,"%lu%05lu",value[1],value[0]);

  return L7_SUCCESS;

}


/*********************************************************************
*
* @purpose Convert a 32 bit address to a string.
*
* @param L7_ulong32 val      internet address
* @param L7_char8   *ipAddr  dotted string representation
*
* @returns L7_SUCCESS always
*
* @comments Caller must provide an output buffer of at least
*           L7_IP_ADDR_STRING_LEN bytes.  The ipAddr must be
*           supplied in host byte order.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbInetNtoa(L7_ulong32 val,  L7_char8 *ipAddr)
{
  L7_RC_t rc = L7_SUCCESS;

  osapiInetNtoa((L7_uint32) val,ipAddr);

  return rc;
}

/*********************************************************************
*
* @purpose  To find out if the given IP address is valid for the
*           given address family
*
* @param L7_uint32 @{(input)}   address_family
* @param L7_uchar8 @{(output)}  address
*
* @returntype L7_RC_t
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t usmDbInetAddrIsValid(L7_uint32 address_family, L7_uchar8 *address)
{
  L7_RC_t rc = L7_FAILURE;

  if(address_family == L7_AF_INET)
  {
    if((strstr(address, ".") != L7_NULL) && (strstr(address, ":") == L7_NULL))
    {
      rc = L7_SUCCESS;
    }
  }
  else if(address_family == L7_AF_INET6)
  {
    if((strstr(address, ":") != L7_NULL) && (strstr(address, ".") == L7_NULL))
    {
      rc = L7_SUCCESS;
    }
  }
  return rc;
}

/*********************************************************************
*
* @purpose  Parse the inet address in displayable format to address
*           in network byte order
*
* @param L7_uchar8 @{(input)}   address string
* @param L7_uchar8 @{(output)}  inet_address
*
* @returntype L7_RC_t
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t usmDbParseInetAddrFromStr(L7_uchar8 *address, L7_inet_addr_t *inet_address)
{
  L7_RC_t rc = L7_SUCCESS;

  if(inet_address != L7_NULL)
  {
    if((strstr(address, ".") != L7_NULL) && (strstr(address, ":") == L7_NULL))
    {
      inet_address->family = L7_AF_INET;
    }
    else if((strstr(address, ":") != L7_NULL) && (strstr(address, ".") == L7_NULL))
    {
      inet_address->family = L7_AF_INET6;
    }
    else
    {
      rc = L7_FAILURE;
    }
  }
  else
  {
    rc = L7_FAILURE;
  }

  if(rc == L7_SUCCESS)
  {
    if (inet_address->family == L7_AF_INET)
    {
      rc = usmDbInetAton(address, &(inet_address->addr.ipv4.s_addr));
    }
    else
    {
    rc = osapiInetPton((L7_uint32)(inet_address->family),
                       address, (L7_uchar8 *)(&(inet_address->addr)));
  }
  }

  return rc;
}

/*********************************************************************
*
* @purpose  Parse the inet address in displayable format to address
*           in network byte order from Hostname
*
* @param L7_uchar8      @{(input)}   address string
* @param L7_inet_addr_t @{(output)}  inet_address
*
* @returntype L7_RC_t
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t usmDbParseInetAddrFromIPAddrHostNameStr(L7_uchar8 *address, 
                                       L7_inet_addr_t *inet_address)
{
  L7_uint32               ipAddr = L7_NULL;
  dnsClientLookupStatus_t status =  DNS_LOOKUP_STATUS_FAILURE;
  L7_char8                hostFQDN[L7_DNS_DOMAIN_NAME_SIZE_MAX];
  L7_char8                hostAddr[L7_DNS_DOMAIN_NAME_SIZE_MAX];
  L7_RC_t                 rc = L7_SUCCESS;
  L7_uchar8               family = L7_AF_INET | L7_AF_INET6;
  L7_inet_addr_t          inetAddrs[2];

  memset(hostAddr, 0, L7_DNS_DOMAIN_NAME_SIZE_MAX);
  osapiStrncpySafe(hostAddr, address,sizeof(hostAddr));
  if(inet_address != L7_NULL)
  {
    if((strstr(address, ".") != L7_NULL) && (strstr(address, ":") == L7_NULL))
    {
      /* This case we have a dot in the address. This would either
          be  a domain Name or IPv4 address. */
      if ((rc =usmDbDnsIpAddressValidate((L7_char8 *)hostAddr, &ipAddr)) != L7_SUCCESS)
      {
        if (rc == L7_ERROR) /* Given String is in IPv4 Address format which is invalid */
        {
          return L7_FAILURE;
        }
        else
        {
          rc = L7_SUCCESS;
        }
        if (usmDbHostNameValidateWithSpace(hostAddr)!= L7_SUCCESS)
        {
          return L7_FAILURE;
        }
        ipAddr = 0;
        if (usmDbDNSClientInetNameLookup(family, hostAddr, &status,
                                         hostFQDN, inetAddrs) != L7_SUCCESS)
        {
          return L7_FAILURE;
        }
        memset(hostAddr, 0, strlen(hostAddr));
        inetAddressGet(L7_AF_INET, &inetAddrs[0], &ipAddr);
        osapiInetNtoa(ipAddr, hostAddr);
      }
      inet_address->family = L7_AF_INET;
    }
    else if((strstr(address, ":") != L7_NULL) && 
            (strstr(address, ".") == L7_NULL))
    {
      inet_address->family = L7_AF_INET6;
    }
    else
    {
      /* This may be a case of Non qualified domain name without a "."
         eg "lvl7". This would either be  a domain Name or IPv4 address  
       */
      if (usmDbHostNameValidateWithSpace(hostAddr)!= L7_SUCCESS)
      {
        return L7_FAILURE;
      }

      ipAddr = 0;
      if (usmDbDNSClientInetNameLookup(family, hostAddr, &status,
                                       hostFQDN, inetAddrs) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }
      memset(hostAddr, 0, strlen(hostAddr));
      inetAddressGet(L7_AF_INET, &inetAddrs[0], &ipAddr);
      osapiInetNtoa(ipAddr, hostAddr);
      inet_address->family = L7_AF_INET;
    }
  }
  else
  {
    rc = L7_FAILURE;
  }

  if(rc == L7_SUCCESS)
  {
    rc = osapiInetPton((L7_uint32)(inet_address->family),
                       hostAddr, (L7_uchar8 *)(&(inet_address->addr)));
  }
  return rc; 
}

/*********************************************************************
*
* @purpose Convert internet address string to a 32 bit integer.
*
* @param L7_uchar8 *ipAddr  dotted string representation
* @param L7_uint32 *ival    32 bit integer representation
*
* @returns L7_SUCCESS, or
* @returns L7_FAILURE
*
* @notes Can use hex, octal or decimal input.  Places result in
* @notes location pointed to by 'ival'in HOST BYTE ORDER.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbInetAton(L7_uchar8 *ipAddr, L7_uint32 *ival)
{
  register L7_uint64 val;
  register int base, n;
  register char c;
  u_int parts[4];
  register u_int *pp = parts;

  if (ipAddr == NULL) /* is ipAddr always going to be null if there is no ipa*/
    return(L7_FAILURE);

  c = *ipAddr;
  for (;;)
  {
    /*
     * Collect number up to ``.''.
     * Values are specified as for C:
     * 0x=hex, 0=octal, isdigit=decimal.
     */
    if (!isdigit((L7_int32)c))
      return(L7_FAILURE);
    val = 0; base = 10;
    if (c == '0')
    {
      c = *++ipAddr;
      if (c == 'x' || c == 'X')
        base = 16, c = *++ipAddr;
      else
        base = 8;
    }
    for (;;)
    {
      if (isascii((L7_int32)c) && isdigit((L7_int32)c))
      {
        val = (val * base) + (c - '0');
        c = *++ipAddr;
      }
      else if (base == 16 && isascii((L7_int32)c) && isxdigit((L7_int32)c))
      {
        val = (val << 4) |
              (c + 10 - (islower((L7_int32)c) ? 'a' : 'A'));
        c = *++ipAddr;
      }
      else
        break;
    }
    if (c == '.')
    {
      /*
       * Internet format:
       *    a.b.c.d
       *    a.b.c   (with c treated as 16 bits)
       *    a.b (with b treated as 24 bits)
       */
      if (pp >= parts + 3)
        return(L7_FAILURE);
      *pp++ = val;
      c = *++ipAddr;
    }
    else
      break;
  }
  /*
   * Check for trailing characters.
   */
  if (c != '\0' && (!isascii((L7_int32)c) || !isspace((L7_int32)c)))
    return(L7_FAILURE);
  /*
   * Verify that the user supplied 4 numbers that will each fit in
   * 8 bits.  We don't allow short IP addresses (i.e. implicit
   * address lengths).
   */
  n = pp - parts + 1;
  switch (n)
  {

  case 0:
    return(L7_FAILURE);                  /* initial nondigit */

  case 1:                                /* a -- 32 bits */
    if (strlen(ipAddr) > 10)
    {
      /* overly large number entered */
      return(L7_FAILURE);
    }
    break;

  case 2:
      return(L7_FAILURE);                /* a.b -- 8.24 bits */

  case 3:
      return(L7_FAILURE);               /* a.b.c -- 8.8.16 bits */

  case 4:                               /* a.b.c.d -- 8.8.8.8 bits */
    parts[3] = val;
    if ((parts[0] > 0xff) ||
        (parts[1] > 0xff) ||
        (parts[2] > 0xff) ||
        (parts[3] > 0xff))
    {
      return(L7_FAILURE);
    }

    /*
     * Now that the result is validated, and we know it
     * will fit, it is time to compute a host byte order
     * result from this and put it in 'ival'.
     */
    val = (parts[0] << 24) | (parts[1] << 16) | (parts[2] << 8) | parts[3];
    break;

    default:
        /*
         * This should never be reached, but just to be safe...
         */
        return(L7_FAILURE);
  }
  if (val > 0xffffffffUL)
  {
    return(L7_FAILURE);
  }
  *ival = val;

  return(L7_SUCCESS);
}

/*********************************************************************
*
* @purpose Validate Host address string and covert it to a 32 bit integer.
*
* @param L7_uchar8 *hostAddr  dotted string representation
* @param L7_uint32 *ival    32 bit integer representation
*
* @returns L7_SUCCESS, or
* @returns L7_FAILURE
* @returns L7_ERROR
*
* @notes Can use hex, octal or decimal input.  Places result in
* @notes location pointed to by 'ival'in HOST BYTE ORDER.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbDnsIpAddressValidate(L7_uchar8 *hostAddr, L7_uint32 *ival)
{
  register L7_uint64 val;
  register int base, n;
  register char c;
  u_int parts[4];
  register u_int *pp = parts;

  if (hostAddr == NULL) /* is hostAddr always going to be null if there is no ipa*/
    return(L7_FAILURE);

  c = *hostAddr;
  for (;;)
  {
    /*
     * Collect number up to ``.''.
     * Values are specified as for C:
     * 0x=hex, 0=octal, isdigit=decimal.
     */
    if ((!isdigit((L7_int32)c)) && (c != '\0' ))
      return(L7_FAILURE);
    val = 0; base = 10;
    if (c == '0')
    {
      c = *++hostAddr;
      if (c == 'x' || c == 'X')
        base = 16, c = *++hostAddr;
      else
        base = 8;
    }
    for (;;)
    {
      if (isascii((L7_int32)c) && isdigit((L7_int32)c))
      {
        val = (val * base) + (c - '0');
        c = *++hostAddr;
      }
      else if (base == 16 && isascii((L7_int32)c) && isxdigit((L7_int32)c))
      {
        val = (val << 4) |
              (c + 10 - (islower((L7_int32)c) ? 'a' : 'A'));
        c = *++hostAddr;
      }
      else
        break;
    }
    if (c == '.')
    {
      /*
       * Internet format:
       *    a.b.c.d
       *    a.b.c   (with c treated as 16 bits)
       *    a.b (with b treated as 24 bits)
       */
      if (pp >= parts + 3)
        return(L7_ERROR);
      c = *++hostAddr;
      *pp = val;
      if (c!= '\0')
      {
        pp++;
      }
    }
    else
      break;
  }
  /*
   * Check for trailing characters.
   */
  if (c != '\0' && (!isascii((L7_int32)c) || !isspace((L7_int32)c)))
    return(L7_FAILURE);
  /*
   * Verify that the user supplied 4 numbers that will each fit in
   * 8 bits.  We don't allow short IP addresses (i.e. implicit
   * address lengths).
   */
  n = pp - parts + 1;
  switch (n)
  {

  case 0:
    return(L7_FAILURE);                  /* initial nondigit */

  case 1:                                /* a -- 32 bits */
    if (strlen(hostAddr) > 10)
    {
      /* overly large number entered */
       return(L7_FAILURE);
    }
    return(L7_ERROR);

  case 2:
      return(L7_ERROR);

  case 3:
      return(L7_ERROR);

  case 4:                               /* a.b.c.d -- 8.8.8.8 bits */
    parts[3] = val;
    if ((parts[0] > 0xff) ||
        (parts[1] > 0xff) ||
        (parts[2] > 0xff) ||
        (parts[3] > 0xff))
    {
      return(L7_ERROR);
    }

    /*
     * Now that the result is validated, and we know it
     * will fit, it is time to compute a host byte order
     * result from this and put it in 'ival'.
     */
    val = (parts[0] << 24) | (parts[1] << 16) | (parts[2] << 8) | parts[3];
    break;

    default:
        /*
         * This should never be reached, but just to be safe...
         */
        return(L7_FAILURE);
  }
  if (val > 0xffffffffUL)
  {
    return(L7_ERROR);
  }
  *ival = val;

  return(L7_SUCCESS);
}

/*********************************************************************
*
* @purpose Validate Host name or IP address string and covert it to a 
*          32 bit integer  and return type of address (IPV4 or DNS).
*
* @param L7_uchar8            *hostAddr  Host Address.
* @param L7_uint32            *ival      32 bit integer representation
* @param L7_IP_ADDRESS_TYPE_t *addrType   Address Type.
*
* @returns L7_SUCCESS, or
* @returns L7_FAILURE
* @returns L7_ERROR
*
* @notes Can use hex, octal or decimal input.  Places result in
* @notes location pointed to by 'ival'in HOST BYTE ORDER.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbIPHostAddressValidate(L7_uchar8 *hostAddr, 
                                   L7_uint32 *ival, 
                                   L7_IP_ADDRESS_TYPE_t *addrType)
{
  L7_RC_t rc = L7_FAILURE;

  rc = usmDbDnsIpAddressValidate(hostAddr, ival);
  if (rc != L7_SUCCESS)
  {
    if (rc == L7_ERROR)
    {
      return L7_ERROR;
    }
    else
    {
      if (usmDbHostNameValidateWithSpace(hostAddr) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }
      *addrType = L7_IP_ADDRESS_TYPE_DNS;
    }
  }
  else
  {
    *addrType = L7_IP_ADDRESS_TYPE_IPV4;
  }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Validate Host name or IPv6 address string and covert it to a 
*          inet address and return type of address (IPV6 or DNS).
*
* @param L7_uchar8            *hostAddr      Host Address.
* @param L7_inet_addr_t       *hostInetAddr  inetAddress 
* @param L7_IP_ADDRESS_TYPE_t *addrType      Address Type.
*
* @returns L7_SUCCESS, or
* @returns L7_FAILURE
*
* @notes Can use hex, octal or decimal input.  Places result in
*
* @end
*
*********************************************************************/
L7_RC_t usmDbIPv6HostAddressValidate(L7_uchar8 *hostAddr, 
                                   L7_inet_addr_t *hostInetAddr, 
                                   L7_IP_ADDRESS_TYPE_t *addrType)
{
  L7_RC_t rc = L7_FAILURE;

  rc = osapiInetPton(L7_AF_INET6, hostAddr, (L7_uchar8 *)(&(hostInetAddr->addr)));
  if (rc != L7_SUCCESS)
  {
    if (usmDbHostNameValidateWithSpace(hostAddr) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
    *addrType = L7_IP_ADDRESS_TYPE_DNS;
  }
  else
  {
    *addrType = L7_IP_ADDRESS_TYPE_IPV6;
  }
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose Returns the external interface number associated with the
*          internal ifIndex.
*
* @param L7_uint32 intIfNum  the internal interface number
* @param L7_uint32 *ifIndex  pointer to the external ifIndex
*
* @returns L7_SUCCESS, if success
* @returns L7_NOT_EXISTS, if interface does not exist
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbExtIfNumFromIntIfNum(L7_uint32 intIfNum, L7_uint32 *ifIndex)
{
  return(nimGetIntfIfIndex(intIfNum, ifIndex));
}


/*********************************************************************
*
* @purpose Returns the internal interface number associated with the
*          external ifIndex.
*
* @param L7_uint32 ifIndex    the external ifIndex
* @param L7_uint32 *intIfNum  pointer to the internal interface number
*
* @returns L7_SUCCESS, if success
* @returns L7_NOT_EXISTS, if interface does not exist
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbIntIfNumFromExtIfNum(L7_uint32 ifIndex, L7_uint32 *intIfNum)
{
  return(nimGetIntfNumber(ifIndex, intIfNum));
}


/*********************************************************************
*
* @purpose Returns the internal interface number associated with the
*          Unit-Slot-Port.
*
* @param L7_uint32 UnitIndex  the unit for this operation
* @param L7_uint32 SlotIndex  the Slot number
* @param L7_uint32 Index      the Port number
* @param L7_uint32 *intIfNum  pointer to the internal interface number
*
* @returns L7_SUCCESS, if success
* @returns L7_NOT_EXISTS, if interface does not exist
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbIntIfNumFromUSPGet(L7_uint32 UnitIndex, L7_uint32 SlotIndex,
                                L7_uint32 Index, L7_uint32 *intIfNum)
{
  nimUSP_t usp;

  usp.port = Index;
  usp.unit = UnitIndex;
  usp.slot = SlotIndex;

  return(nimGetIntIfNumFromUSP(&usp, intIfNum));
}


/*********************************************************************
*
* @purpose Obtain the next **external** interface number, or ifIndex.
*
* @param L7_uint32 extIfNum       external interface number
* @param L7_uint32 *nextextIfNum  pointer to next external interface number
*                  (@b{Returns: Next External Interface Number})
*
* @returns L7_SUCCESS, if success
* @returns L7_NOT_EXISTS, if interface does not exist
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbGetNextExtIfNumber(L7_uint32 extIfNum, L7_uint32 *nextExtIfNum)
{
  return nimGetNextExtIfNumber(extIfNum, nextExtIfNum);
}


/*********************************************************************
* @purpose Determine if this **external** interface number, or ifIndex
*          exists.
*
* @param L7_uint32 UnitIndex  the unit for this operation
* @param L7_uint32 extIfNum   external interface number
*
* @returns L7_SUCCESS, if interface exists
* @returns L7_ERROR, if interface does not exist
* @returns L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbExtIfNumberCheck(L7_uint32 UnitIndex, L7_uint32 extIfNum)
{
  return nimCheckExtIfNumber(extIfNum);
}


/*********************************************************************
*
* @purpose Obtain the next **external** interface number, or ifIndex
*          to be displayed.
*
* @param L7_uint32 extIfNum       external interface number
* @param L7_uint32 *nextextIfNum  pointer to next external interface number
*                  (@b{Returns: Next External Interface Number}).
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbGetNextVisibleExtIfNumber(L7_uint32 extIfNum, L7_uint32 *nextExtIfNum)
{
  /* loop through available external interface numbers */
  while (nimGetNextExtIfNumber(extIfNum, nextExtIfNum) == L7_SUCCESS)
  {
    /* check to see if this is a visible external interface number */
    if (usmDbVisibleExtIfNumberCheck(USMDB_UNIT_CURRENT, *nextExtIfNum) == L7_SUCCESS)
      return L7_SUCCESS;

    /* setup for next-external-interface call */
    extIfNum = *nextExtIfNum;
  }

  return L7_FAILURE;
}


/*********************************************************************
* @purpose Determine if this **external** interface number, or ifIndex
*          to be displayed exists.
*
* @param L7_uint32 UnitIndex  unit number
* @param L7_uint32 extIfNum   external interface number
*
* @returns L7_SUCCESS, if interface exists
* @returns L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbVisibleExtIfNumberCheck(L7_uint32 UnitIndex, L7_uint32 extIfNum)
{
  L7_uint32 intIfNum;
  L7_RC_t rc;

  /* check to see if it's a valid external interface number*/
  if (usmDbExtIfNumberCheck(UnitIndex, extIfNum) == L7_SUCCESS)
  {
    /* convert external interface number to internal interface number */
    rc = usmDbIntIfNumFromExtIfNum(extIfNum, &intIfNum);

    return usmDbVisibleIntIfNumberCheck(UnitIndex, intIfNum);
  }
  return L7_FAILURE;
}


/*********************************************************************
*
* @purpose Obtain the next internal interface number
*          to be displayed.
*
* @param L7_uint32 intIfNum       internal interface number
* @param L7_uint32 *nextintIfNum  pointer to next internal interface number
*                  (@b{Returns: Next Internal Interface Number}).
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbGetNextVisibleIntIfNumber(L7_uint32 intIfNum, L7_uint32 *nextIntIfNum)
{
  /* loop through available internal interface numbers */
  while (nimNextValidIntfNumber(intIfNum, nextIntIfNum) == L7_SUCCESS)
  {
    /* check to see if this is a visible internal interface number */
    if (usmDbVisibleIntIfNumberCheck(USMDB_UNIT_CURRENT, *nextIntIfNum) == L7_SUCCESS)
      return L7_SUCCESS;

    /* setup for next-internal-interface call */
    intIfNum = *nextIntIfNum;
  }

  return L7_FAILURE;
}


/*********************************************************************
* @purpose Determine if this internal interface number
*          to be displayed exists.
*
* @param L7_uint32 UnitIndex  unit number
* @param L7_uint32 intIfNum   internale interface number
*
* @returns L7_SUCCESS, if interface exists
* @returns L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbVisibleIntIfNumberCheck(L7_uint32 UnitIndex, L7_uint32 intIfNum)
{
  L7_INTF_TYPES_t sysIntfType;
  L7_RC_t rc = L7_FAILURE;

  /* check to see if it's a valid internal interface number*/
  if (nimCheckIfNumber(intIfNum) == L7_SUCCESS &&
      nimGetIntfType(intIfNum, &sysIntfType) == L7_SUCCESS)
  {
    switch (sysIntfType)
    {
      /* Physical Interfaces */
    case L7_PHYSICAL_INTF:
    case L7_CPU_INTF:
      rc = L7_SUCCESS;
      break;

    case L7_STACK_INTF:
      break;

    /* Logical Interfaces */
    case L7_LAG_INTF:
      if (dot3adIsLagConfigured(intIfNum) == L7_TRUE)
      {
        rc = L7_SUCCESS;
      }
      break;

    case L7_LOGICAL_VLAN_INTF:
    case L7_LOOPBACK_INTF:
    case L7_TUNNEL_INTF:
    case L7_WIRELESS_INTF: 
    case L7_CAPWAP_TUNNEL_INTF:
    case L7_VLAN_PORT_INTF:     /* PTin added: virtual ports */

      rc = L7_SUCCESS;
      break;

      break;

    default:
      break;
    }
  }
  return rc;
}

/*********************************************************************
*
* @purpose Obtain the next internal interface number that is a
*          physical interface.
*
* @param L7_uint32 intIfNum       internal interface number
* @param L7_uint32 *nextintIfNum  pointer to next physical internal interface number
*                  (@b{Returns: Next Physical Internal Interface Number}).
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbGetNextPhysicalIntIfNumber(L7_uint32 intIfNum, L7_uint32 *nextIntIfNum)
{
  return(nimNextValidIntfNumberByType(L7_PHYSICAL_INTF, intIfNum, nextIntIfNum));
}

/*********************************************************************
*
* @purpose Obtain the next internal interface number that is a
*          lag interface.
*
* @param L7_uint32 intIfNum       internal interface number
* @param L7_uint32 *nextintIfNum  pointer to next lag internal interface number
*                  (@b{Returns: Next Lag Internal Interface Number}).
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbGetNextLagIntIfNumber(L7_uint32 intIfNum, L7_uint32 *nextIntIfNum)
{
  return(nimNextValidIntfNumberByType(L7_LAG_INTF, intIfNum, nextIntIfNum));
}

/*********************************************************************
*
* @purpose Obtain the next internal interface number of the specified 
*          interface type.
*
* @param L7_INTF_TYPES_t sysIntfType       internal type
* @param L7_uint32 intIfNum       internal interface number
* @param L7_uint32 *nextintIfNum  pointer to next vlan internal interface number
*                  (@b{Returns: Next vlan Internal Interface Number}).
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbNextIntIfNumberByTypeGet(L7_INTF_TYPES_t sysIntfType, L7_uint32 intIfNum, L7_uint32 *nextIntIfNum)
{
  return(nimNextValidIntfNumberByType(sysIntfType, intIfNum, nextIntIfNum));
}

/*********************************************************************
* @purpose Determine if this internal interface number exists and is
*          a physical interface
*
* @param L7_uint32 UnitIndex  unit number
* @param L7_uint32 intIfNum   internal interface number
*
* @returns L7_SUCCESS, if interface exists
* @returns L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbPhysicalIntIfNumberCheck(L7_uint32 UnitIndex, L7_uint32 intIfNum)
{
  L7_INTF_TYPES_t sysIntfType;

  if (intIfNum != 0 &&
      (nimGetIntfType(intIfNum, &sysIntfType) == L7_SUCCESS) &&
      (sysIntfType == L7_PHYSICAL_INTF) )
  {
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Obtain the next external interface number that is a
*          physical interface.
*
* @param L7_uint32 UnitIndex      unit number
* @param L7_uint32 extIfNum       external interface number
* @param L7_uint32 *nextExtIfNum  pointer to next physical external interface number
*                  (@b{Returns: Next Physical External Interface Number}).
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbNextPhysicalExtIfNumberGet(L7_uint32 UnitIndex, L7_uint32 extIfNum, L7_uint32 *nextExtIfNum)
{
  L7_uint32 intIfNum;
  L7_INTF_TYPES_t sysIntfType;

  while (nimGetNextExtIfNumber(extIfNum, nextExtIfNum) == L7_SUCCESS)
  {
    if ( (nimGetIntfNumber(*nextExtIfNum, &intIfNum) == L7_SUCCESS) &&
         (nimGetIntfType(intIfNum, &sysIntfType) == L7_SUCCESS) &&
         (sysIntfType == L7_PHYSICAL_INTF) )
    {
      return L7_SUCCESS;
    }

    /* setup for next-external-interface call */
    extIfNum = *nextExtIfNum;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose Determine if this external interface number exists and is
*          a physical interface
*
* @param L7_uint32 UnitIndex  unit number
* @param L7_uint32 extIfNum   external interface number
*
* @returns L7_SUCCESS, if interface exists
* @returns L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbPhysicalExtIfNumberCheck(L7_uint32 UnitIndex, L7_uint32 extIfNum)
{
  L7_uint32 intIfNum;
  L7_INTF_TYPES_t sysIntfType;

  if ( (nimGetIntfNumber(extIfNum, &intIfNum) == L7_SUCCESS) &&
       (nimGetIntfType(intIfNum, &sysIntfType) == L7_SUCCESS) &&
       (sysIntfType == L7_PHYSICAL_INTF) )
  {
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Returns the Unit-Slot-Port associated with the internal
*          interface number.
*
* @param L7_uint32 intIfNum    internal interface number
* @param L7_uint32 *UnitIndex  pointer to the unit number
* @param L7_uint32 *SlotIndex  pointer to the slot number
* @param L7_uint32 *Index      pointer to the port
*
* @returns L7_SUCCESS, if success
* @returns L7_NOT_EXISTS, if interface does not exist
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbUnitSlotPortGet(L7_uint32 intIfNum,
                             L7_uint32 *UnitIndex,
                             L7_uint32 *SlotIndex,
                             L7_uint32 *Index)
{
  L7_RC_t rc = L7_FAILURE;
  nimUSP_t usp;

  rc = nimGetUnitSlotPort(intIfNum, &usp);

  if (rc == L7_SUCCESS)
  {
    *Index = usp.port;
    *UnitIndex = usp.unit;
    *SlotIndex = usp.slot;
  }

  return(rc);
}


/*********************************************************************
*
* @purpose Return Internal Interface Number of the first valid port.
*
* @param L7_uint32 *intIfNum  pointer to the first valid internal
*                             interface number
*
* @returns L7_SUCCESS, if success
* @returns L7_NOT_EXISTS, if interface does not exist
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbValidIntIfNumFirstGet(L7_uint32 *intIfNum)
{
  return(nimFirstValidIntfNumber(intIfNum));
}


/*********************************************************************
*
* @purpose Return Internal Interface Number of the next valid port.
*
* @param L7_uint32 interface  internal interface number
* @param L7_uint32 *intIfNum  pointer to the next valid internal
*                             interface number
*
* @returns L7_SUCCESS, if success
* @returns L7_NOT_EXISTS, if interface does not exist
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbValidIntIfNumNext(L7_uint32 interface, L7_uint32 *intIfNum)
{
  return(nimNextValidIntfNumber(interface, intIfNum));
}


/*********************************************************************
*
* @purpose Returns the internal interface number associated with the
*          Trunk ifIndex.
*
* @param L7_uint32 UnitIndex     the unit for this operation
* @param L7_uint32 TrunkIfIndex  the ifIndex for the trunk
* @param L7_uint32 *intIfNum     pointer to the next valid internal
*                                interface number
*
* @returns L7_SUCCESS, if success
* @returns L7_NOT_EXISTS, if interface does not exist
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbIntIfNumFromTrunkGet(L7_uint32 UnitIndex, L7_uint32 TrunkIfIndex, L7_uint32 *intIfNum)
{
  return(nimGetIntfNumber(TrunkIfIndex, intIfNum));
}


/*********************************************************************
*
* @purpose Determine if an interface in question is to be displayed.
*
* @param L7_uint32 UnitIndex  unit number
* @param L7_uint32 intIfNum   interface number
* @param L7_uint32 *retVal    return value
*
* @returns L7_SUCCESS, if interface can be displayed
* @returns L7_FAILURE, if interface is not to be displayed
*
* @notes    none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbVisibleInterfaceCheck(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 *retVal)
{
  L7_INTF_TYPES_t sysIntfType;

  if (usmDbDot3adValidIntfCheck(UnitIndex, intIfNum) == L7_TRUE  &&  usmDbDot3adIsConfigured(UnitIndex, intIfNum) == L7_FALSE)
  {
    *retVal = -2;
    return L7_FAILURE;
  }
  else if (intIfNum == FD_CNFGR_NIM_MIN_CPU_INTF_NUM)
  {
    *retVal = -1;
    return L7_FAILURE;
  }
  else if (nimGetIntfType(intIfNum, &sysIntfType) == L7_SUCCESS &&
           (sysIntfType == L7_WIRELESS_INTF || sysIntfType == L7_CAPWAP_TUNNEL_INTF ||
            sysIntfType == L7_VLAN_PORT_INTF))      /* PTin added: virtual ports */
  {
    *retVal = -1;
    return L7_FAILURE;
  }
  else
  {
    *retVal = 0;
    return L7_SUCCESS;
  }
}


/*********************************************************************
*
* @purpose Checks if a MAC address value is all zeros.
*
* @param L7_uchar8 *pMac  ptr to Ethernet MAC address
*
* @returns L7_TRUE, if MAC address is all zeros
* @returns L7_FALSE, if MAC address is non-zero
*
* @notes Checks byte by byte to avoid any alignment restrictions
*        on certain CPUs.
*
* @end
*
*********************************************************************/
L7_BOOL usmDbIsMacZero(L7_uchar8 *pMac)
{
  L7_uint32     i;

  for (i = 0; i < L7_MAC_ADDR_LEN; i++)
  {
    if (pMac[i] != 0)
      return L7_FALSE;
  }
  return L7_TRUE;
}

/*********************************************************************
*
* @purpose Retrieves the current link up/down status of the service port.
*
* @param L7_uint32 UnitIndex    @b((input))  The unit for this operation
*
* @returns L7_UP, or
* @returns L7_DOWN
*
* @notes none
*
* @end
*
*********************************************************************/
L7_uint32 usmDbServicePortLinkStatusGet( L7_uint32 UnitIndex )
{
  return(osapiServicePortLinkStatusGet () );
}

/*********************************************************************
*
* @purpose Retrieves the current link up/down status of the serial port.
*
* @param L7_uint32 UnitIndex    @b((input))  The unit for this operation
*
* @returns L7_UP, or
* @returns L7_DOWN
*
* @notes none
*
* @end
*
*********************************************************************/
L7_uint32 usmDbSerialStatusGet( L7_uint32 UnitIndex )
{
  return(osapiSerialStatusGet () );
}

/*********************************************************************
*
* @purpose Check to see if a component is present.
*
* @param L7_uint32 UnitIndex    the unit for this operation
* @param L7_uint32 componentId  the component id number in
*                               L7_COMPONENT_IDS_t
*
* @returns L7_TRUE, or
* @returns L7_FALSE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_BOOL usmDbComponentPresentCheck(L7_uint32 UnitIndex,L7_COMPONENT_IDS_t  componentId)
{
  L7_BOOL enabled = L7_FALSE;
  L7_BOOL keyable = L7_FALSE;

  if (cnfgrIsComponentPresent(componentId) == L7_TRUE)
  {
    /* Check if the user is allowed to enter key for the protocol */
    /* If protocol does not support keying feature */
    if (sysapiFeatureKeyGet(componentId, &enabled, &keyable) == L7_SUCCESS &&
        keyable == L7_TRUE &&
        enabled == L7_FALSE)
    {
          /* If this box supports keying and the feature is keyable,
              check if the feature has been enabled */
          return L7_FALSE;
    }
    return cnfgrIsComponentPresent(componentId);
  }
  return L7_FALSE;
}

/*********************************************************************
*
* @purpose Check to see if a component is not Present.
*
* @param L7_uint32 UnitIndex    the unit for this operation
* @param L7_uint32 componentId  the component id number in
*                               L7_COMPONENT_IDS_t
*
* @returns L7_TRUE, or
* @returns L7_FALSE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_BOOL usmDbComponentNotPresentCheck(L7_uint32 UnitIndex,L7_COMPONENT_IDS_t  componentId)
{
  return !(cnfgrIsComponentPresent(componentId));
}
/*********************************************************************
*
* @purpose Check to see if a component is present.
*
* @param L7_uint32 UnitIndex    The unit for this operation
* @param L7_uint32 componentId  the component id number in
*                               L7_COMPONENT_IDS_t
*
* @returns L7_TRUE, or
* @returns L7_FALSE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_BOOL usmDbFeaturePresentCheck(L7_uint32 UnitIndex,L7_COMPONENT_IDS_t componentId, L7_uint32 featureId)
{
  return cnfgrIsFeaturePresent(componentId, featureId);
}

/*********************************************************************
*
* @purpose Check to see if a component is absent.
*
* @param L7_uint32 UnitIndex    The unit for this operation
* @param L7_uint32 componentId  the component id number in
*                               L7_COMPONENT_IDS_t
*
* @returns L7_TRUE, or
* @returns L7_FALSE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_BOOL usmDbFeatureNotPresentCheck(L7_uint32 UnitIndex,L7_COMPONENT_IDS_t componentId, L7_uint32 featureId)
{
  return !(cnfgrIsFeaturePresent(componentId, featureId));
}

/*********************************************************************
* @purpose  Re-init bootp protocol.
*
* @param L7_uint32 UnitIndex    @b((input))  The unit for this operation
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void usmDbBootpTaskReInit(L7_uint32 UnitIndex)
{
  bootpTaskReInit();
}

/*********************************************************************
*
* @purpose Return a count of the Internal Interfaces of valid ports
*          matching any of the specified interface types.
*
* @param L7_uint32 unit  unit index
*
* @param L7_uint32 inclintIfType  mask of interface types to include
*
* @param L7_uint32 exclintIfType  mask of interface types to exclude
*
* @returns number of matching interfaces
*
* @notes none
*
* @end
*
*********************************************************************/
L7_uint32 usmDbIntIfNumTypeCountGet(L7_uint32 unit, L7_uint32 inclintIfType, L7_uint32 exclintIfType)
{
  L7_uint32 maskIntfType;
  L7_uint32 intIfNum;
  L7_uint32 count;

  count = 0;

  for (intIfNum=1; intIfNum<L7_ALL_INTERFACES; intIfNum++)
  {
    if (usmDbIntIfNumTypeMaskGet(unit, intIfNum, &maskIntfType) == L7_SUCCESS)
    {
      if ((maskIntfType & inclintIfType) && !(maskIntfType & exclintIfType))
        count++;
    }
  }

  return(count);
}

/*********************************************************************
*
* @purpose Return a mask of the characteristics associated with this
*          interface
*
* @param L7_uint32 unit  unit index
*
* @param L7_uint32 intIfNum  internal interface number
*
* @param L7_uint32 *maskIntfType  mask of interface types for this interface
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes none
*
*
*********************************************************************/
L7_RC_t usmDbIntIfNumTypeMaskGet(L7_uint32 unit, L7_uint32 intIfNum, L7_uint32 *maskIntfType)
{
  L7_RC_t   rc = L7_FAILURE;
  L7_INTF_TYPES_t sysIntIfType;
  L7_uint32 rtr_intf_mode;
  L7_uint32 lag;

  *maskIntfType = 0;

  if (nimCheckIfNumber(intIfNum) == L7_SUCCESS)
  {
    rc = nimGetIntfType(intIfNum, &sysIntIfType);
    if (rc == L7_SUCCESS)
    {
      switch (sysIntIfType)
      {
      case L7_PHYSICAL_INTF:
        *maskIntfType += USM_PHYSICAL_INTF;
        break;

      case L7_STACK_INTF:
        *maskIntfType += USM_STACK_INTF;
        break;

      case L7_CPU_INTF:
        *maskIntfType += USM_CPU_INTF;            /* CPU interface */
        break;

      case L7_LAG_INTF:
        if (usmDbDot3adIsConfigured(unit, intIfNum) == L7_TRUE)
          *maskIntfType += USM_LAG_INTF;          /* Configured lag interface */
        break;

      case L7_LOGICAL_VLAN_INTF:
        *maskIntfType += USM_LOGICAL_VLAN_INTF;   /* Logical VLAN interface */
        break;

      case L7_LOOPBACK_INTF:
        *maskIntfType += USM_LOGICAL_LOOPBACK_INTF; /* Loopback interface */
        break;

      case L7_TUNNEL_INTF:
        *maskIntfType += USM_LOGICAL_TUNNEL_INTF; /* Tunnel interface */
        break;

      case L7_WIRELESS_INTF:
        *maskIntfType += USM_LOGICAL_WIRELESS_INTF; /* Wireless Network interface */
        break;

      case L7_CAPWAP_TUNNEL_INTF:
        *maskIntfType += USM_LOGICAL_L2_TUNNEL_INTF; /* CAPWAP tunnel interface */
        break;

      /* PTin added: virtual ports */
      case L7_VLAN_PORT_INTF:
        *maskIntfType += USM_LOGICAL_VLAN_PORT_INTF; /* Vlan port interface */
        break;

      default:
        break;
      }

      if (usmDbDot3adIntfIsMemberGet(unit, intIfNum, &lag) == L7_SUCCESS)
        *maskIntfType += USM_LAG_MEMBER_INTF;     /* Lag Member interface */

      if (usmDbSwPortMonitorIsSrcConfigured(unit,intIfNum) == L7_TRUE)
        *maskIntfType += USM_MIRROR_SRC_INTF;     /* Mirror Source interface */

      if (usmDbSwPortMonitorIsDestConfigured(unit,intIfNum,L7_NULLPTR) == L7_TRUE)
        *maskIntfType += USM_MIRROR_DST_INTF;     /* Mirror Destination interface */

      if (usmDbIpRtrIntfModeGet(unit, intIfNum, &rtr_intf_mode) == L7_SUCCESS)
      {
        if (rtr_intf_mode == L7_ENABLE)
          *maskIntfType += USM_ROUTER_INTF;       /* Router interface */
      }

    }
  }
  return(rc);
}

/*********************************************************************
*
* @purpose Return Internal Interface Number of the first valid port
*          matching any of the specified interface types.
*
* @param L7_uint32 unit  unit index
*
* @param L7_uint32 inclintIfType  mask of interface types to include
*
* @param L7_uint32 exclintIfType  mask of interface types to exclude
*
* @param L7_uint32 *intIfNum  pointer to the first valid internal
*                             interface number
*
* @returns L7_SUCCESS, if success
* @returns L7_NOT_EXISTS, if interface does not exist
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbIntIfNumTypeFirstGet(L7_uint32 unit, L7_uint32 inclintIfType, L7_uint32 exclintIfType, L7_uint32 *intIfNum)
{
  L7_RC_t   rc;
  L7_uint32 maskIntfType;
  L7_BOOL   matchFound;

  matchFound = L7_FALSE;

  rc = nimFirstValidIntfNumber(intIfNum);
  if (rc == L7_SUCCESS)
  {
    if (usmDbIntIfNumTypeMaskGet(unit, *intIfNum, &maskIntfType) == L7_SUCCESS)
    {
      if ((maskIntfType & inclintIfType) && !(maskIntfType & exclintIfType))
        matchFound = L7_TRUE;
    }
  }

  while ((matchFound == L7_FALSE) && (rc == L7_SUCCESS))
  {
    rc = nimNextValidIntfNumber(*intIfNum, intIfNum);
    if (rc == L7_SUCCESS)
    {
      if (usmDbIntIfNumTypeMaskGet(unit, *intIfNum, &maskIntfType) == L7_SUCCESS)
      {
        if ((maskIntfType & inclintIfType) && !(maskIntfType & exclintIfType))
          matchFound = L7_TRUE;
      }
    }
  }

  if ((matchFound == L7_TRUE) && (rc == L7_SUCCESS))
    rc = L7_SUCCESS;
  else
    rc = L7_NOT_EXISTS;

  return(rc);
}


/*********************************************************************
*
* @purpose Return Internal Interface Number of the next valid port
*          matching any of the specified interface types.
*
* @param L7_uint32 unit  unit index
*
* @param L7_uint32 inclintIfType  mask of interface types to include
*
* @param L7_uint32 exclintIfType  mask of interface types to exclude
*
* @param L7_uint32 interface  internal interface number
*
* @param L7_uint32 *intIfNum  pointer to the next valid internal
*                             interface number
*
* @returns L7_SUCCESS, if success
* @returns L7_NOT_EXISTS, if interface does not exist
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbIntIfNumTypeNextGet(L7_uint32 unit, L7_uint32 inclintIfType, L7_uint32 exclintIfType, L7_uint32 interface, L7_uint32 *intIfNum)
{
  L7_RC_t   rc;
  L7_uint32 maskIntfType;
  L7_BOOL   matchFound;

  matchFound = L7_FALSE;
  rc = L7_SUCCESS;
  *intIfNum = interface;

  while ((matchFound == L7_FALSE) && (rc == L7_SUCCESS))
  {
    rc = nimNextValidIntfNumber(*intIfNum, intIfNum);
    if (rc == L7_SUCCESS)
    {
      if (usmDbIntIfNumTypeMaskGet(unit, *intIfNum, &maskIntfType) == L7_SUCCESS)
      {
        if ((maskIntfType & inclintIfType) && !(maskIntfType & exclintIfType))
          matchFound = L7_TRUE;
      }
    }
  }

  if ((matchFound == L7_TRUE) && (rc == L7_SUCCESS))
    rc = L7_SUCCESS;
  else
    rc = L7_NOT_EXISTS;

  return(rc);
}

/*********************************************************************
*
* @purpose Checks that the external Interface Number is a valid port
*          matching any of the specified interface types.
*
* @param L7_uint32 unit  unit index
*
* @param L7_uint32 inclintIfType  mask of interface types to include
*
* @param L7_uint32 exclintIfType  mask of interface types to exclude
*
* @param L7_uint32 extIfNum       internal to check
*
* @returns L7_SUCCESS, if success
* @returns L7_NOT_EXISTS, if interface does not exist
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbExtIfNumTypeCheckValid(L7_uint32 unit, L7_uint32 inclintIfType, L7_uint32 exclintIfType, L7_uint32 extIfNum)
{
  L7_uint32 intIfNum;
  L7_uint32 maskIntfType;

  if (usmDbIntIfNumFromExtIfNum(extIfNum, &intIfNum) == L7_SUCCESS)
  {
    if (usmDbIntIfNumTypeMaskGet(unit, intIfNum, &maskIntfType) == L7_SUCCESS)
    {
      if ((maskIntfType & inclintIfType) && !(maskIntfType & exclintIfType))
        return L7_SUCCESS;
    }
  }

  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Checks that the internal Interface Number is a valid port
*          matching any of the specified interface types.
*
* @param L7_uint32 unit  unit index
*
* @param L7_uint32 inclintIfType  mask of interface types to include
*
* @param L7_uint32 exclintIfType  mask of interface types to exclude
*
* @param L7_uint32 intIfNum       internal to check
*
* @returns L7_SUCCESS, if success
* @returns L7_NOT_EXISTS, if interface does not exist
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbIntIfNumTypeCheckValid(L7_uint32 unit, L7_uint32 inclintIfType, L7_uint32 exclintIfType, L7_uint32 intIfNum)
{
  L7_uint32 maskIntfType;

  if (usmDbIntIfNumTypeMaskGet(unit, intIfNum, &maskIntfType) == L7_SUCCESS)
  {
    if ((maskIntfType & inclintIfType) && !(maskIntfType & exclintIfType))
      return L7_SUCCESS;
  }

  return L7_FAILURE;
}


/*********************************************************************
*
* @purpose Return Internal Interface Number of the next valid port
*          matching any of the specified interface types.
*
* @param L7_uint32 unit  unit index
*
* @param L7_uint32 inclintIfType  mask of interface types to include
*
* @param L7_uint32 exclintIfType  mask of interface types to exclude
*
* @param L7_uint32 extIfNum       internal interface number
*
* @param L7_uint32 *nextExtIfNum  pointer to the next valid exernternal
*                                 interface number
*
* @returns L7_SUCCESS, if success
* @returns L7_NOT_EXISTS, if interface does not exist
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbExtIfNumTypeNextGet(L7_uint32 unit, L7_uint32 inclintIfType, L7_uint32 exclintIfType, L7_uint32 extIfNum, L7_uint32 *nextExtIfNum)
{
  while (nimGetNextExtIfNumber(extIfNum, nextExtIfNum) == L7_SUCCESS)
  {
    if (usmDbExtIfNumTypeCheckValid(unit, inclintIfType, exclintIfType, *nextExtIfNum) == L7_SUCCESS)
    {
      return L7_SUCCESS;
    }

    /* setup for next-external-interface call */
    extIfNum = *nextExtIfNum;
  }

  return L7_FAILURE;
}


/*********************************************************************
*
* @purpose Verify the string contains only alpha-numeric, dash, or
*           underscore characters. (a-z, A-Z, 0-9, '-', or '_')
*
* @param L7_uchar8 *str    string to check
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbStringAlphaNumericCheck(const L7_char8 str[])
{
  L7_uint32 i=0; /* index into string */

  for (i=0; i<strlen(str); i++)
  {
    if (!isalnum((L7_int32)str[i]) && (str[i] != '-') && (str[i] != '_') && (str[i] != ' '))
      return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Verify the string contains only digits.
*
* @param L7_uchar8 *str    string to check
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbStringNumericCheck(const L7_char8 str[])
{
  L7_uint32 i=0; /* index into string */

  for (i=0; i<strlen(str); i++)
  {
    if (!isdigit((L7_int32)str[i]) )
      return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Verify the string contains only HEXADECIMAL characters.
*
* @param    L7_uchar8 *str    string to check
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbStringHexadecimalCheck(const L7_char8 input[])
{
  L7_int32 i;

  for (i = 0; i < strlen(input); i++)
  {
    switch (input[i])
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
      case 'a': case 'A':
      case 'b': case 'B':
      case 'c': case 'C':
      case 'd': case 'D':
      case 'e': case 'E':
      case 'f': case 'F':
        continue;
      default:
        return L7_FAILURE;
        break;
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Verify the string contains only alpha-numeric characters, or
            additional characters specified, as well as not allowing
            other specified characters. (a-z, A-Z, 0-9, or more/less)
*
* @param L7_uchar8 *str         string to check
* @param L7_uchar8 *include     string of additional characters to allow
* @param L7_uchar8 *exclude     string of additional characters to not allow
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbStringAlphaNumericPlusCheck(const L7_char8 str[], const L7_char8 include[], const L7_char8 exclude[])
{
  L7_uint32 i=0; /* index into string */
  L7_uint32 j=0; /* index into include/exclude string */
  L7_BOOL b_allowed;

  for (i=0; i<strlen(str); i++)
  {
    b_allowed = L7_FALSE;

    if (isalnum((L7_int32)str[i]))
      b_allowed = L7_TRUE;

    if (b_allowed != L7_TRUE && include != L7_NULLPTR)
    {
      for (j=0; j<strlen(include); j++)
      {
        if (include[j] == str[i])
        {
          b_allowed = L7_TRUE;
          break;
        }
      }
    }

    if (b_allowed == L7_TRUE && exclude != L7_NULLPTR)
    {
      for (j=0; j<strlen(exclude); j++)
      {
        if (exclude[j] == str[i])
        {
          b_allowed = L7_FALSE;
          break;
        }
      }
    }

    if (b_allowed == L7_FALSE)
      return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Verify the string contains only printable characters
*             (0x20 ' ' - 0x7E '~')
*
* @param L7_uchar8 *str    string to check
*
* @returns L7_SUCCESS, if success
* @returns L7_FAILURE, if other failure
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbStringPrintableCheck(const L7_char8 str[])
{
  L7_uint32 i; /* index into string */

  for (i=0; i<strlen(str); i++)
  {
    if (str[i] < ' ' || str[i] > '~')
      return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Compares two strings case-insensitive
*
* @param L7_uchar8 *str1    first string to check
*
* @param L7_uchar8 *str2    second string to check
*
* @returns L7_SUCCESS, if the strings match
* @returns L7_FAILURE, if one string is
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbStringCaseInsensitiveCompare(const L7_char8 str1[], const L7_char8 str2[])
{
  L7_uint32 i;                /* index into strings */
  L7_uint32 str1Len, str2Len; /* length of strings */

  if((str1 == L7_NULLPTR) || (str2 == L7_NULLPTR))
  {
    return L7_FAILURE;
  }

  /* if strings are differing length, then they don't match */
  str1Len = strlen(str1);
  str2Len = strlen(str2);

  if (str1Len != str2Len)
    return L7_FAILURE;

  for (i=0; i < str1Len; i++)
  {
    if (str1[i] >= 'A' && str1[i] <= 'Z' && str2[i] >= 'a' && str2[i] <= 'z')
    {
      if ((str1[i] + 'a' - 'A') != str2[i])
        return L7_FAILURE;
    }
    else if (str2[i] >= 'A' && str2[i] <= 'Z' && str1[i] >= 'a' && str1[i] <= 'z')
    {
      if ((str2[i] + 'a' - 'A') != str1[i])
        return L7_FAILURE;
    }
    else
    {
      if (str2[i] != str1[i])
        return L7_FAILURE;
    }
  }
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose  To convert the enumerated value of a protocol to hex.
*
* @param    L7_uint32  prtl  @b((input)) specified protocol
* @param    L7_uint32  *val  @b((output)) protocol's hex value
*
* @returns  L7_SUCCESS
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbPbVlanPrtlDecimalToHex(L7_uint32 prtl, L7_uint32 *val)
{
  switch (prtl)
  {
  case L7_USMDB_PROTOCOL_IP:
    *val = L7_USMDB_PROTOCOL_HEX_IP;
    break;

  case L7_USMDB_PROTOCOL_ARP:
    *val = L7_USMDB_PROTOCOL_HEX_ARP;
    break;

  case L7_USMDB_PROTOCOL_IPX:
    *val = L7_USMDB_PROTOCOL_HEX_IPX;
    break;

  default:
    break;
  }

  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose  To convert a protocol's hex value to an enumerated value.
*
* @param    L7_uint32  prtl  @b((input)) specified protocol
* @param    L7_uint32  *val  @b((output)) protocol's decimal value
*
* @returns  L7_SUCCESS
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbPbVlanPrtlHexToDecimal(L7_uint32 prtl, L7_uint32 *val)
{
  switch (prtl)
  {
  case L7_USMDB_PROTOCOL_HEX_IP:
    *val = L7_USMDB_PROTOCOL_IP;
    break;

  case L7_USMDB_PROTOCOL_HEX_ARP:
    *val = L7_USMDB_PROTOCOL_ARP;
    break;

  case L7_USMDB_PROTOCOL_HEX_IPX:
    *val = L7_USMDB_PROTOCOL_IPX;
    break;

  default:
    *val = L7_NULL;
    break;
  }

  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose  To convert a char array to an integer array.
*
* @param    L7_uchar8  *buf   @b((input)) pointer to the char array
* @param    L7_uint32  *list  @b((output)) list to be filled
*
* @returns  L7_SUCCESS
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmdbPortListGet(L7_uchar8 *buf, L7_uint32 *list)
{
  L7_uint32 i;
  L7_uint32 j = 1;

  for (i = 0; i < L7_CLI_MAX_STRING_LENGTH; i++)
  {
    if ((buf[i] == ',') || (buf[i] == '\0'))
    {
      continue;
    }
    else
    {
      list[j++] = atoi(&buf[i]);
    }
  }

  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose  To convert a list of internal interface numbers to a mask.
*
* @param    L7_uint32       *list      @b((input)) list of interfaces
* @param    L7_INTF_MASK_t  *mask      @b((input)) pointer to mask
*
* @returns  L7_SUCCESS
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbConvertListToMask(L7_uint32 *list, L7_INTF_MASK_t *mask)
{
  L7_uint32 i;
  L7_INTF_MASK_t tmp;

  memset(&tmp, 0, sizeof(L7_INTF_MASK_t));

  for (i = 1; i <= L7_MAX_INTERFACE_COUNT; i++)
  {
    if (list[i] != L7_NULL)
    {
      L7_INTF_SETMASKBIT(tmp, list[i]);
    }
  }

  memcpy(mask, &tmp, L7_INTF_INDICES);
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose  To convert a list of internal interface numbers to a mask.
*
* @param    L7_uint32       *list      @b((input)) list of interfaces
* @param    L7_uint32       listNum    @b((input)) count of interfaces
* @param    L7_INTF_MASK_t  *mask      @b((input)) pointer to mask
*
* @returns  L7_SUCCESS
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbConvertIntfListToMask(L7_uint32 *list, L7_uint32 listNum, L7_INTF_MASK_t *mask)
{
  L7_uint32 ct;
  L7_INTF_MASK_t tmp;

  memset(&tmp, 0, sizeof(L7_INTF_MASK_t));

  if(listNum == 0)
  {
    return L7_FAILURE;
  }

  for (ct = 0; ct < L7_MAX_INTERFACE_COUNT; ct++)
  {
    if (list[ct] != L7_NULL)
    {
      L7_INTF_SETMASKBIT(tmp, list[ct]);
      listNum--;
      if(listNum == 0)
        break;  
    }
  }

  memcpy(mask, &tmp, L7_INTF_INDICES);
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose  To convert a mask to a list of internal interface numbers.
*
* @param    L7_INTF_MASK_t  *mask      @b((input)) the port mask
* @param    L7_uint32       *list      @b((output)) list to be filled
* @param    L7_uint32       *numPorts  @b((output)) number of elements
*                                      filled
*
* @returns  L7_SUCCESS
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbConvertMaskToList(L7_INTF_MASK_t *mask, L7_uint32 *list, L7_uint32 *numPorts)
{
  L7_uint32 i;
  L7_uint32 j = 1;
  L7_INTF_MASK_t tmp;

  memcpy((void *)&tmp, (void *)mask, L7_INTF_INDICES);

  for (i = 1; i <= L7_MAX_INTERFACE_COUNT; i++)
  {
    if (L7_INTF_ISMASKBITSET(tmp, i) != L7_NULL)
    {
      list[j++] = i;
    }
  }

  *numPorts = j-1;  /* this is the addition */
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To convert a mask to SNMP octet-strings.
*
* @param    L7_INTF_MASK_t  maskIn    @b((input)) mask to be converted
* @param    L7_INTF_MASK_t  *maskOut  @b((output)) pointer to the
*                                                  converted mask
*
* @returns  L7_SUCCESS
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbReverseMask(L7_INTF_MASK_t maskIn, L7_INTF_MASK_t *maskOut)
{
  L7_uint32 j;
  L7_uint32 k;
  L7_INTF_MASK_t tmp;

  memset(&tmp, 0, sizeof(L7_INTF_MASK_t));

  /* bytes 0 to max-1 */
  for (k = 0; k < L7_INTF_INDICES; k++)
  {
    /* interfaces 1 to 8 within a byte */
    for (j = 1; j <= 8; j++)
    {
      /* kth byte, jth interface */
      if (L7_INTF_ISMASKBITSET(maskIn, 8*k + j))
      {
        L7_INTF_SETMASKBIT(tmp, 8*k + (8-j+1));
      }
    }
  }

  memcpy(maskOut, &tmp, L7_INTF_INDICES);
  return L7_SUCCESS;
}



/*********************************************************************
*
* @purpose  To combine a given VLANID and MAC address to get an
*           8-byte VLANID-MAC address combination.
*
* @param    L7_uint32  vlanId   @b((input)) vlan id
* @param    L7_uchar8  *mac     @b((input)) pointer to mac address
* @param    L7_uchar8  *vidMac  @b((output)) pointer to the vid-mac
*                               address combination
*                               (2-byte vlan id + 6-byte mac address)
*
* @returns  L7_SUCCESS
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbEntryVidMacCombine(L7_uint32 vlanId, L7_uchar8 *mac, L7_uchar8 *vidMac)
{
  L7_ushort16 vid;
  L7_uchar8 tmp[L7_MFDB_KEY_SIZE];

  memset((void *)tmp, 0x00, L7_MFDB_KEY_SIZE);

  vid = (L7_ushort16) osapiHtons(vlanId);
  memcpy(tmp, &vid, L7_MFDB_VLANID_LEN);
  memcpy(&tmp[L7_MFDB_VLANID_LEN], mac, L7_MAC_ADDR_LEN);

  memcpy(vidMac, tmp, L7_MFDB_KEY_SIZE);
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose  To separate an 8-byte VLANID-MAC address combination into
*           a VLANID and MAC address.
*
* @param    L7_uchar8  *vidMac  @b((input)) pointer to the vid-mac
*                               address combination
*                               (2-byte vlan id + 6-byte mac address)
* @param    L7_uint32  *vlanId  @b((output)) pointer to the vlan id
* @param    L7_uchar8  *mac     @b((output)) pointer to mac address
*
* @returns  L7_SUCCESS
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbEntryVidMacSeparate(L7_uchar8 *vidMac, L7_uint32 *vlanId, L7_uchar8 *mac)
{
  L7_ushort16 vid;
  L7_uchar8 tmp[L7_MAC_ADDR_LEN];

  memset((void *)tmp, 0x00, L7_MAC_ADDR_LEN);

  memcpy(&vid, vidMac, L7_MFDB_VLANID_LEN);
   /* vid is stored in network order for display purposes, 
    * so we must convert to host order on extraction.
    */
  *vlanId = (L7_uint32)osapiNtohs(vid);
  memcpy(tmp, &vidMac[L7_MFDB_VLANID_LEN], L7_MAC_ADDR_LEN);

  memcpy(mac, tmp, L7_MAC_ADDR_LEN);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Convert hexadecimal value to binary string.
*
* @param L7_uchar8* inputData       Input value to be converted
* @param L7_uchar8* binaryString    Input Binary String
* @param L7_uint32* pBinLength                  <Output>Length of binary data
*
* @returns L7_SUCCESS
*          L7_FAILURE
*
* @end
*
*********************************************************************/
L7_RC_t usmDbHexToBin(L7_char8* inputData, L7_uchar8 * binaryString, L7_uint32 * pBinLength)
{
  L7_RC_t    rc = L7_FAILURE;
  L7_uint32  hexStringLen;
  L7_uint32  indexHex = 0;
  L7_uint32  indexByte = 0;
  L7_char8  hexString[2*L7_DHCPS_POOL_OPTION_LENGTH+1];

  bzero(hexString, sizeof(hexString));
  *pBinLength = 0;

  if (usmDbConvertHex(inputData, hexString) == L7_SUCCESS)
  {
    hexStringLen = strlen(hexString);

    indexHex = 0;
    indexByte = 0;
    while(indexHex < hexStringLen && indexByte < L7_DHCPS_POOL_OPTION_LENGTH)
    {
      if(usmDbConvertHexToByte(&hexString[indexHex], &binaryString[indexByte]) != L7_SUCCESS)
        return L7_FAILURE;

      /* incr */
      indexHex += 2;
      indexByte += 1;
    }

    *pBinLength = indexByte;
    rc = L7_SUCCESS;
  }

  return rc;
}


/*********************************************************************
*
* @purpose Convert hexadecimal string to an integer value.
*
* @param L7_uchar8*  buff       2-character buffer
* @param L7_uint32  intDecVal  Decimal value for the buff string
*
* @returns L7_SUCCESS
*
* @end
*
*********************************************************************/
L7_RC_t usmDbConvertHexToByte(L7_char8* inputStr, L7_uchar8 *outputBuf)
{
  L7_uint32 counter = 0;
  L7_uchar8 value = 0;

  /* loop twice */
  do
  {
    switch (inputStr[counter])
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
      value = inputStr[counter] - '0';
      break;
    case 'a':
    case 'A':
      value = 10;
      break;
    case 'b':
    case 'B':
      value = 11;
      break;
    case 'c':
    case 'C':
      value = 12;
      break;
    case 'd':
    case 'D':
      value = 13;
      break;
    case 'e':
    case 'E':
      value = 14;
      break;
    case 'f':
    case 'F':
      value = 15;
      break;

   default:
      return L7_FAILURE;
      break;
    }

    if(counter == 0)
    {
      *outputBuf = value << 4;
    }
    else if(counter == 1)
    {
      *outputBuf = *outputBuf + value;
    }
    else
    {
      return L7_FAILURE;
    }

    counter++;

  } while (counter < 2);

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Removes delimiter
*
* @param    buf         String
* @param    hexString   return hex data
*
* @returns L7_SUCCESS
*          L7_FAILURE
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t usmDbConvertHex(L7_char8 *buf, L7_uchar8 *hexString)
{
  L7_uint32 i,j, digit_count=0;
        L7_uint32 buflen = 0;
  L7_BOOL   isColon = L7_FALSE;
  L7_BOOL   isPeriod = L7_FALSE;

  buflen = strlen(buf);
  for (i = 0, j = 0; i < buflen; i++, j++)
  {
    digit_count++;

    if (((digit_count != 3) && (buf[i] == ':'))
        || ((digit_count != 5) && (buf[i] == '.')))
    {
      return L7_FAILURE;
    }
    else if (((buf[i] == ':') || (buf[i] == 0x20) /* space */) && (isPeriod == L7_FALSE))
    {
      j--;
      digit_count = 0;
      isColon = L7_TRUE;
    }
    else if ((buf[i] == '.') && (isColon == L7_FALSE))
    {
      j--;
      digit_count = 0;
      isPeriod = L7_TRUE;
    }
    else
    {
      hexString[j] = buf[i];
    }
  }

  if ((j%2) != 0)
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Converts a two digit hex string to a hex number
*
* @param    buf         Two digit hex character string
* @param    twoDigitHex Returned two digit hex number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes
*
* @end
*
*********************************************************************/
L7_BOOL usmDbConvertTwoDigitHex(L7_uchar8 *buf, L7_uchar8 *twoDigitHex)
{
  L7_uint32 val;

  /* check for negative */
  if (buf[0] == '-')
    return L7_FALSE;

  /* test string length -- must be exactly two hex digits */
  if (strlen((char*)buf) != 2)
    return L7_FALSE;

  if (twoDigitHex == L7_NULLPTR)
    return L7_FALSE;

  if((isxdigit((int)buf[0]) == 0) || ((isxdigit((int)buf[1]) == 0)))
      return L7_FALSE;

  if (sscanf((char *)buf, "%x", (int *)&val) != 1)
    return L7_FALSE;

  *twoDigitHex = (L7_uchar8)val;

  return L7_TRUE;
}

/* DEBUG */
L7_RC_t usmDbDebugComponentNames()
{
  L7_uint32 index=0;
  L7_COMPONENT_IDS_t id=0;
  L7_char8 mnemonic[L7_COMPONENT_MNEMONIC_MAX_LEN];
  L7_char8 name[L7_COMPONENT_NAME_MAX_LEN];

  while (usmDbComponentIndexNextGet(&index, &id) == L7_SUCCESS)
  {
    bzero(mnemonic, L7_COMPONENT_MNEMONIC_MAX_LEN);
    if (usmDbComponentMnemonicGet(id, mnemonic) != L7_SUCCESS)
    {
      sysapiPrintf("ComponentName: Mnemonic Failed, id=%d\n", id);
    }

    bzero(name, L7_COMPONENT_NAME_MAX_LEN);
    if (usmDbComponentNameGet(id, name) != L7_SUCCESS)
    {
      sysapiPrintf("ComponentName: Name Failed, id=%d\n", id);
    }

    sysapiPrintf("%-4d %-4d %10s %25s\n", index, id, mnemonic, name);
  }
  sysapiPrintf("done...\n");

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
* @notes none
*
* @end
*********************************************************************/
L7_BOOL usmDbMacAddrStringToHex(L7_uchar8 *buf, L7_uchar8 *mac)
{
  L7_RC_t       rc;

  rc = l7utilsMacAddrStringToHex(buf, mac);

  return (rc == L7_SUCCESS) ? L7_TRUE : L7_FALSE;
}

/*********************************************************************
* @purpose  Converts a two digit hex string to a hex number
*
* @param    buf         Two digit hex character string
* @param    twoDigitHex Returned two digit hex number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes
*
* @end
*
*********************************************************************/
L7_BOOL usmDbOUIAddrStringToHex(L7_uchar8 *buf, L7_uchar8 *oui)
{

  L7_RC_t       rc;

  rc = l7utilsOUIValueStringToHex(buf, oui);

  return (rc == L7_SUCCESS) ? L7_TRUE : L7_FALSE;

}


/*
* @purpose  Convert Hex representation of MAC address to the format
*           xx:xx:xx:xx:xx:xx
*
* @param    mac                  @b{(input)} MAC address in hex format
* @param    macBuf               @b{(output)} MAC address in ASCII string 
*                                             as xx:xx:xx:xx:xx:xx
*
* @returns  L7_TRUE             If conversion succeeds 
* @returns  L7_FALSE            Else
*
* @comments none.
*
* @notes    mac and macBuf must be allocated with atleast 6 and 17 bytes 
*           respectively. 
* 
* @end
*
*********************************************************************/
L7_BOOL usmDbMacAddrHexToString(L7_uchar8 *mac, L7_uchar8 *macBuf)
{
#define USMDB_MAC_STR_BUF_SIZE    17
  L7_RC_t       rc;
  L7_uchar8     buf[USMDB_MAC_STR_BUF_SIZE+1];

  rc = l7utilsMacAddrHexToString(mac, sizeof(buf), buf);

  if (rc == L7_SUCCESS)
  {
    osapiStrncpySafe(macBuf, buf, USMDB_MAC_STR_BUF_SIZE+1);    /* don't know caller's macBuf size */
    return L7_TRUE;
  }

  return L7_FALSE;
}

/*********************************************************************
*
* @purpose  Convert Raw Time (unsigned int) to string format
*           WWd:XXh:YYm:ZZs.
*
* @param    L7_uint32 time        @b{(input)} Raw time
* @param    L7_char8 timeStr      @b{(output)} Time in ASCII string 
*                                       (null terminated) as WWd:XXh:YYm:ZZs
* @param    L7_uint32 timeStrLen  @b{(input)} Length of timeStr, min 16
*
* @returns  L7_TRUE               If conversion succeeds 
* @returns  L7_FALSE              Else
*
* @comments none.
*
* @notes    timeStr must be allocated with atleast 16 bytes.
* 
* @end
*
*********************************************************************/
L7_BOOL usmDbTimeToStringConvert(L7_uint32 time, L7_char8 *timeStr,
                            L7_uint32 timeStrLen)
{
  L7_timespec ts;

  if (timeStr == NULL || timeStrLen < 16)
    return L7_FALSE;

  osapiConvertRawUpTime(time, &ts);
  osapiSnprintf(timeStr, timeStrLen, "%ud:%02u:%02u:%02u",
                            ts.days, ts.hours, ts.minutes, ts.seconds);

  return L7_TRUE;
} /* usmDbTimeToStringConvert */

/*********************************************************************
*
* @purpose  Convert Raw Time (unsigned int) to string format
*           WWd:XXh:YYm:ZZs.
*
* @param    L7_uint32 time        @b{(input)} Raw time
* @param    L7_char8 timeStr      @b{(output)} Time in ASCII string 
*                                       (null terminated) as WWd:XXh:YYm:ZZs
* @param    L7_uint32 timeStrLen  @b{(input)} Length of timeStr, min 13
*
* @returns  L7_TRUE               If conversion succeeds 
* @returns  L7_FALSE              Else
*
* @comments none.
*
* @notes    timeStr must be allocated with atleast 13 bytes.
* 
* @end
*
*********************************************************************/
L7_BOOL usmDbTimeToShortStringConvert(L7_uint32 time, L7_char8 *timeStr,
                            L7_uint32 timeStrLen)
{
  L7_timespec ts;

  if (timeStr == NULL || timeStrLen < 13)
    return L7_FALSE;

  osapiConvertRawUpTime(time, &ts);
  osapiSnprintf(timeStr, timeStrLen, "%02u:%02u:%02u:%02u",
                            ts.days, ts.hours, ts.minutes, ts.seconds);

  return L7_TRUE;
} /* usmDbTimeToShortStringConvert */

/*********************************************************************
*
* @purpose  Convert Time Ticks (unsigned int) to days/hours/minutes/seconds
*           WWdays XXh:YYm:ZZs.
*
* @param    L7_uint32 *timeTicks @b{(input)} Time Ticks
* @param    L7_char8 *buf        @b{(output)} Time in WWdays XXh:YYm:ZZs 
*                              
* @returns  L7_SUCCESS               If conversion succeeds 
* @returns  L7_FAILURE               If timeTicks or buf is/are Null pointers
*
* @comments none.
* 
* @end
*
*********************************************************************/
L7_RC_t usmDbConvertTimeTicksToDaysHoursMinutesSeconds(L7_uint32 *timeTicks, L7_char8 *buf)
{ 
  L7_uint32 timeInSeconds;
  L7_uint32 days;
  L7_uint32 minutes;
  L7_uint32 hours;
  L7_uint32 seconds;
  
  if(timeTicks == NULL || buf == NULL)
  {
    return L7_FAILURE;
  }
  
  memcpy (&timeInSeconds, timeTicks, sizeof (L7_uint32));
  memset (buf, 0, sizeof (buf));
  
  timeInSeconds = timeInSeconds / 100;  /*to get no of seconds */  
  days = timeInSeconds / 86400;
  timeInSeconds = timeInSeconds % 86400;
  hours = timeInSeconds / 3600;
  timeInSeconds = timeInSeconds % 3600;
  minutes = timeInSeconds / 60;
  timeInSeconds = timeInSeconds % 60;
  seconds = timeInSeconds;
  
  sprintf (buf, "%d days %d h:%d m:%d s", days, hours, minutes, seconds);  
 
  return L7_SUCCESS;
} /* usmDbConvertTimeTicksToDaysHoursMinutesSeconds */


/*********************************************************************
* @purpose  convert date from integer to string.
*
* @param    L7_uint32  now
*
* returns   L7_char8
*
* @notes
*
* @end
*********************************************************************/
L7_char8 * usmDbConvertTimeToDateString(L7_uint32 now)
{
  return ctime((time_t *)&now) + 4;
}

/*********************************************************************
* @purpose   Mask the ip address
*
* @param    prefix
* @param    prefixlength
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
static L7_uchar8 ip6MapMaskByte[8] = {0,0x80,0xc0,0xe0,0xf0,0xf8,0xfc,0xfe};
L7_RC_t usmDbIpv6MapMaskApplyGet(L7_in6_addr_t *prefix, L7_uint32 plen)
{
   
   int i;

    if(plen > 128) return L7_FAILURE;

    /* mask by octet */
    for(i = 0; i < 16;i++)
    {
       if(plen < 8){
          /* mask some bits */
          prefix->in6.addr8[i] &= ip6MapMaskByte[plen];
          break;
       }
       plen -= 8;
    }

    /* rest of bytes are 0 */
    for(i++; i < 16;i++)
    {
       prefix->in6.addr8[i] = 0;
    }
    return L7_SUCCESS;


   
   
}

/*********************************************************************
* @purpose   Convert an IPv4 prefix length to a mask.
*
* @param    mask  @b{(input)) - resulting mask
* @param    plen  @b{(input)) - prefix length
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIpSubnetMaskGet(L7_uint32 *mask, L7_uint32 plen)
{
  if (!mask || (plen > 32)) 
  {
    return L7_FAILURE;
  }

  *mask = (0xFFFFFFFF << (32 - plen));

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Parse the ipv6 address string for prefix/prefix_length
*
* @param  *str_addr   @b{(input)) pointer to <prefix>/<prefix-length> string
* @param  *prefix  @b{(output)) pointer to the prefix index
* @param  *prefix-len  @b{(output)) pointer to the prefix length index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t usmDbIp6AddrParse(const L7_char8 *str_addr, L7_in6_addr_t *prefix,
                                L7_uint32 *prefixLen)
{

  L7_uint32   buf_len;
  L7_char8    input[IPV6_DISP_ADDR_LEN+4+1];
  L7_char8   *p;
  L7_char8   *plen;

  buf_len = (L7_uint32)strlen(str_addr);
  if ((buf_len >= sizeof(input)) || (buf_len == 0) || (prefixLen == L7_NULLPTR))
    return L7_FAILURE;

  osapiStrncpySafe(input, str_addr, sizeof(input));

  p = input;
  plen = strstr(input, "/");

  if (plen == L7_NULL)
    return L7_FAILURE;
  plen++;

  *prefixLen = (L7_uint32) atol(plen);

  plen--;
  *plen = '\0';

  if(osapiInetPton(L7_AF_INET6, p, (L7_uchar8 *) prefix) != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Gets the CPU Utilization 
*
* @param    L7_uint32 UnitIndex unit number
* @param    L7_char* buf buffer
*
* @returns L7_SUCCESS
*          L7_FAILURE
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t usmdbCpuUtilizationStringGet(L7_uint32 UnitIndex, L7_char8 *buf, 
                                     L7_uint32 bufferSize)
{
   return osapiCpuUtilizationStringGet(buf, bufferSize);
}

/*********************************************************************
* @purpose  Set the CPU Free memory threshold for monitoring
*
* @param    unitIndex {(input)} UnitIndex unit number
* @param    threshold {(input)} Free memory threshold in KB. A value of 0 
*                               indicates that threshold monitoring should be
*                               stopped.
*
* @returns L7_SUCCESS If threshold was successfully set
*          L7_REQUEST_DENIED If the threshold is configured to be more than the
*                            total available memory
*          L7_FAILURE Other errors
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t usmdbCpuFreeMemoryThresholdSet(L7_uint32 UnitIndex, 
                                       L7_uint32 threshold)
{
  return simCpuFreeMemoryThresholdSet(threshold);
}

/*********************************************************************
* @purpose  Get the configured CPU Free memory threshold 
*
* @param    unitIndex {(input)} UnitIndex unit number
* @param    threshold {(output)} Free memory threshold in KB. A value of 0 
*                                indicates that no threshold is configured.
*
* @returns L7_SUCCESS
*          L7_FAILURE
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t usmdbCpuFreeMemoryThresholdGet(L7_uint32 UnitIndex, 
                                       L7_uint32 *threshold)
{
  return simCpuFreeMemoryThresholdGet(threshold);
}


/*********************************************************************
* @purpose  Set a CPU Util monitor parameter
*
* @param    unitIndex {(input)} UnitIndex unit number
* @param    paramType {(input)} Parameter type
* @param    paramVal  {(input)} Value of the parameter
*
* @returns L7_SUCCESS If parameter was successfully set
*          L7_FAILURE Other errors
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t usmdbCpuUtilMonitorParamSet(L7_uint32 UnitIndex, 
                                    simCpuUtilMonitorParam_t paramType,
                                    L7_uint32 paramVal)
{
  return simCpuUtilMonitorParamSet(paramType, paramVal);
}

/*********************************************************************
* @purpose  Get a CPU Util monitor parameter
*
* @param    unitIndex {(input)} UnitIndex unit number
* @param    paramType {(input)} Parameter type
* @param    paramVal  {(output)} Value of the parameter
*
* @returns L7_SUCCESS If parameter was successfully set
*          L7_FAILURE Other errors
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t usmdbCpuUtilMonitorParamGet(L7_uint32 UnitIndex, 
                                    simCpuUtilMonitorParam_t paramType,
                                    L7_uint32 *paramVal)
{
  return simCpuUtilMonitorParamGet(paramType, paramVal);
}

/*********************************************************************
*
* @purpose Used to validate form and syntax of a hostname
*
* @param    hostName  @b((input)) host Name to validate. 
*
* @returns L7_SUCCESS for success
*          L7_FAILURE for Failure
*
* @comments
*
* @end
*
********************************************************************/
L7_RC_t usmDbHostNameValidate(L7_uchar8 *hostName)
{
  L7_uchar8 label[DNS_DOMAIN_NAME_SIZE_MAX + 1];
  L7_uint32 hostLength;
  L7_uint32 labelLength;
  L7_uint32 labelStart;
  L7_uint32 i;
  L7_uchar8 c;
  L7_BOOL   lastLabel = L7_FALSE;
  
  if (hostName == NULL) /* is hostAddr always going to be null if there is no ipa*/
      return(L7_FAILURE);

  hostLength = strlen(hostName);
  if (hostLength > DNS_DOMAIN_NAME_SIZE_MAX)
  {
    return L7_FAILURE;
  }

  /* verify each character and label length */
   labelLength = 0;
  labelStart = 0;
  for (i = 0; i < hostLength; i++)
  {
    c = hostName[i];
    if ((c != '.') && (i == hostLength - 1))
    {
      lastLabel = L7_TRUE;
      if (!(isalnum(c)))
      {
        return L7_FAILURE;
      }
    }
    if (isalnum(c))
    {
      labelLength++;
      if (lastLabel != L7_TRUE) continue;
    }
    if (c == '-')
    {
      labelLength++;
      if (lastLabel != L7_TRUE) continue;
    }
    if ((lastLabel == L7_TRUE) || ((c == '.') && (i > 0)))
    {
      if (hostName[i-1] == '.' && (c == '.'))
      {
        return L7_FAILURE;
      }
      memcpy(label, &hostName[labelStart], labelLength);
      label[labelLength] = 0;
      labelStart = i + 1;

      /* verify label syntax */
      /* must not exceed 63 characters */
      if (strlen(label) <= 63)
      {
         /* must start w/ letter or digit*/
        if (isalnum(label[0]))
        {
          /* must end w/ letter or digit */
          if (isalnum(label[labelLength - 1]))
          {
            labelLength = 0;
            continue;
          }
        }
      }

      return L7_FAILURE;
    }

    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Erase  startup-config file.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmdbEraseStartupConfig()
{
  /* If a next template ID has been saved persistently, clear it. */
  (void) sdmNextActiveTemplateSet(SDM_TEMPLATE_NONE);

  return nvStoreEraseStartupConfig();
}

/*********************************************************************
*
* @purpose Check to see if the project is not merlion
*
* @param none
*
* @returns L7_TRUE, or
* @returns L7_FALSE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_BOOL usmDbIsNotFeatMetroCpeV10Check()
{
#if defined(FEAT_METRO_CPE_V1_0) 
  return L7_FALSE;
#endif
  return L7_TRUE;
}
/*********************************************************************
*
* @purpose Check to see if the project is not merlion
*
* @param none
*
* @returns L7_TRUE, or
* @returns L7_FALSE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_BOOL usmDbIsFeatMetroCpeV10Check()
{
#if defined(FEAT_METRO_CPE_V1_0)
  return L7_TRUE;
#endif
  return L7_FALSE;
}
/*********************************************************************
*
* @purpose Check to see if the chip is Valcon ie 53115.
*
* @param L7_uint32 UnitIndex    the unit for this operation
*
* @returns L7_TRUE, or
* @returns L7_FALSE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_BOOL usmDbSocIs53115Check(L7_uint32 UnitIndex)
{
  if(cnfgrBaseTechnologySubtypeGet() == L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_ROBO_53115)
  {
    return L7_TRUE;
  }
  else
  {
    return L7_FALSE;
  }
}
/*********************************************************************
*
* @purpose Check to see if the chip is not Valcon ie 53115.
*
* @param L7_uint32 UnitIndex    the unit for this operation
*
* @returns L7_TRUE, or
* @returns L7_FALSE
*
* @notes none
*
* @end
*
*********************************************************************/


L7_BOOL usmDbSocIsnot53115Check(L7_uint32 UnitIndex)
{
  if(cnfgrBaseTechnologySubtypeGet()!= L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_ROBO_53115)
  {
    return L7_TRUE;
  }
  else
  {
    return L7_FALSE;
  }
}
/*********************************************************************
* @purpose  used to convert hex to decimal value
*
* @param    c         contains hex digit
*
* @returns decimal value
*
* @notes
*
* @end
*********************************************************************/
char usmDbConvertXtoD(char c) {
  if (c>='0' && c<='9') return c-'0';
  if (c>='A' && c<='F') return c-'A'+10;
  if (c>='a' && c<='f') return c-'a'+10;
  return c=0;        /* not Hex digit */

}

/*********************************************************************
* @purpose  used to convert hex string to decimal value
*
* @param    hex         contains hex string
*
* @returns decimal value
*
* @notes
*
* @end
*********************************************************************/
L7_uint32 usmDbConvertHextoDec(char *hex, L7_uint32 l)
{
  if (*hex==0) return(l);
  return usmDbConvertHextoDec(hex+1, l*16+usmDbConvertXtoD(*hex)); /* hex+1? */
}

/*********************************************************************
* @purpose  used to convert hex string to decimal value
*
* @param    hex         contains hex string
*
* @returns success
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbConvertXstrtoi(char *hex, L7_uint32 *dec)

{
  *dec = usmDbConvertHextoDec(hex,0);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Check to see if PoE component is present in any of the stack member.
*
* @returns L7_TRUE, or
* @returns L7_FALSE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_BOOL usmDbPoEFeaturePresentCheck()
{

  L7_uint32 unit=0;

  for ( unit = 1; unit < L7_ALL_UNITS; unit++ )
  {
    if (cnfgrIsFeaturePresent( L7_POE_COMPONENT_ID, L7_POE_FEATURE_ID) == L7_TRUE)
       return L7_TRUE;
  }

  return L7_FALSE;
}

#if L7_FEAT_BANNER_MOTD
/*********************************************************************
* @purpose  Sets the banner 
*
* @param    L7_uint32       UnitIndex unit number
* @param    L7_char8 *      buffer    pointer to banner buffer
*
* @returns L7_SUCCESS
*          L7_FAILURE
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t usmDbBannerSet(L7_uint32 UnitIndex, L7_char8 *buffer)
{
   return cliWebBannerSet(UnitIndex, buffer);
}

/*********************************************************************
* @purpose  Gets the banner
*
* @param    L7_uint32       UnitIndex unit number
* @param    L7_char8 *      buffer    pointer to banner buffer
*
* @returns L7_SUCCESS
*          L7_FAILURE
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t usmDbBannerGet(L7_uint32 UnitIndex, L7_char8 *buffer)
{
  return cliWebBannerGet(UnitIndex, buffer) ;
}

/*********************************************************************
* @purpose  Sets the banner ack
*
* @param    L7_uint32       UnitIndex unit number
* @param    L7_BOOL      val banner ack value
*
* @returns L7_SUCCESS
*          L7_FAILURE
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t usmDbBannerAckSet(L7_uint32 UnitIndex, L7_BOOL val)
{
   return cliWebBannerAckSet(UnitIndex, val);
}  

/*********************************************************************
* @purpose  Gets the banner ack
*
* @param    L7_uint32       UnitIndex unit number
* @param    L7_BOOL      val banner ack value  
*
* @returns L7_SUCCESS
*          L7_FAILURE
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t usmDbBannerAckGet(L7_uint32 UnitIndex, L7_BOOL *val)
{
  return cliWebBannerAckGet(UnitIndex, val);
}

#endif

/*********************************************************************
*
* @purpose This function converts emweb characters to corresponding 
	   HTML characters to represent the string	
*
* @returns L7_FAILURE, or
* @returns L7_SUCCESS
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbConvertStringToEmwebString(L7_uint32 UnitIndex, L7_char8 *buffer, L7_char8 *emwebBuffer)
{
  
   L7_uint32 i = 0;
   L7_char8 c[2];

   if(buffer == NULL  || emwebBuffer==NULL)
   {
      return L7_FAILURE;
   }
  
   while(buffer[i] != '\0')
   {
     memset(c, 0x0, sizeof(c)); 
     if(buffer[i] == '<')
     {
       osapiStrncat(emwebBuffer, "&lt;", strlen("&lt;"));
     }
     else if(buffer[i] == '>')
     {
       osapiStrncat(emwebBuffer, "&gt;", strlen("&gt;"));
     }
     else if(buffer[i] == '&')
     {
       osapiStrncat(emwebBuffer, "&amp;", strlen("&amp;"));
     }
     else if(buffer[i] == '"')
     {
       osapiStrncat(emwebBuffer, "&quot;",strlen("&quot;"));
     }
     else if(buffer[i] == '\'')
     {
       osapiStrncat(emwebBuffer, "&#39;",strlen("&#39;"));
     }
     else
     {
       *c = buffer[i];
       osapiStrncat(emwebBuffer, c, 1);
     }
     i++;
   }
   osapiStrncat(emwebBuffer, "",1);
   
   return L7_SUCCESS;
}

/*********************************************************************
* @purpose  converts Ethertype arg string to an L7_ushor16
*
* @param    buf         contains ascii string "0xNNNN"
* @param    etype       returne value
*
* @returns void
*
* @notes none
*
* @end
*********************************************************************/
L7_BOOL usmWebConvertEtypeCustomValue(const L7_uchar8 *buf, L7_uint32 *val)
{
  L7_uint32 i, j;
  L7_uchar8 val_digit[4];
  L7_uint32 value;

  if (strlen(buf) != 6)
  {      /* test string length */
    return L7_FALSE;
  }

  if (buf[0] != '0')
    return L7_FALSE;

  if ((buf[1] != 'x') && (buf[1] != 'X'))
    return L7_FALSE;

  for (i = 2, j = 0; i < 6; i++)
  {
    switch (buf[i])
    {
      case '0':
        val_digit[j++] = 0x0;
        break;
      case '1':
        val_digit[j++] = 0x1;
        break;
      case '2':
        val_digit[j++] = 0x2;
        break;
      case '3':
        val_digit[j++] = 0x3;
        break;
      case '4':
        val_digit[j++] = 0x4;
        break;
      case '5':
        val_digit[j++] = 0x5;
        break;
      case '6':
        val_digit[j++] = 0x6;
        break;
      case '7':
        val_digit[j++] = 0x7;
        break;
      case '8':
        val_digit[j++] = 0x8;
        break;
      case '9':
        val_digit[j++] = 0x9;
        break;
      case 'a':
      case 'A':
        val_digit[j++] = 0xA;
        break;
      case 'b':
      case 'B':
        val_digit[j++] = 0xB;
        break;
      case 'c':
      case 'C':
        val_digit[j++] = 0xC;
        break;
      case 'd':
      case 'D':
        val_digit[j++] = 0xD;
        break;
      case 'e':
      case 'E':
        val_digit[j++] = 0xE;
        break;
      case 'f':
      case 'F':
        val_digit[j++] = 0xF;
        break;
      default:
        return L7_FALSE;
        break;
    }
  }

  value = 0;
  for (j = 0; j < 4; j++)
  {
    value += val_digit[j] << (4 * (3 - j));
  }

  *val = value;
  return L7_TRUE;
}
/**************************************************************************
 *
 * @purpose  Convert codeVersion_t to human readable string format.
 *
 * @param    codeVersion_t  code version structure
 *
 * @param    L7_char8* version output buffer
 *
 * @returns  L7_SUCCESS/L7_ERROR.
 *
 * @notes
 *
 * @end
 *
 *************************************************************************/
L7_RC_t usmDbImageVersionToString(codeVersion_t code_version, L7_char8 *version)
{
  if (NULL == version)
  {
    return L7_ERROR;
  }
  if ( isalpha(code_version.rel) != 0 )
    sprintf(version, "%c.%d.%d.%d",code_version.rel, code_version.ver,
       code_version.maint_level, code_version.build_num);
  else
    sprintf(version, "%d.%d.%d.%d",code_version.rel, code_version.ver,
       code_version.maint_level, code_version.build_num);

  return L7_SUCCESS;
}

/**************************************************************************
 *
 * @purpose  Convert human readable version string to codeVersion_t structure.
 *
 * @param    L7_char8* code version in string format
 *
 * @param    codeVersion_t*  pointer code version structure
 *
 * @returns  L7_SUCCESS/L7_ERROR.
 *
 * @notes
 *
 * @end
 *
 *************************************************************************/
L7_RC_t usmDbImageStringToVersion(L7_char8 *version, codeVersion_t* code_version)
{
  L7_int32 rel;
  L7_int32 ver;
  L7_int32 maint;
  L7_int32 build;

  if (NULL == version || NULL == code_version)
  {
    return L7_ERROR;
  }
  sscanf(version,"%d.%d.%d.%d",&rel,&ver,&maint,&build);
  code_version->rel = rel;
  code_version->ver = ver;
  code_version->maint_level = maint;
  code_version->build_num = build;

  return L7_SUCCESS;
}



/*********************************************************************
*
* @purpose Used to validate form and syntax of a hostname with space
*
* @param    hostName  @b((input)) host Name to validate.
*
* @returns L7_SUCCESS for success
*          L7_FAILURE for Failure
*
* @comments
*
* @end
*
********************************************************************/
L7_RC_t usmDbHostNameValidateWithSpace(L7_uchar8 *hostName)
{
  L7_uchar8 label[DNS_DOMAIN_NAME_SIZE_MAX + 1];
  L7_uint32 hostLength = 0;
  L7_uint32 labelLength = 0;
  L7_uint32 labelStart = 0;
  L7_uint32 i = 0;
  L7_uchar8 c;

  hostLength = strlen(hostName);
  if (hostLength > DNS_DOMAIN_NAME_SIZE_MAX)
  {
    return L7_FAILURE;
  }

  /* verify each character and label length */
  if (hostName[hostLength-1] == ' ' ||  hostName[0] == ' ')
  {
    return L7_FAILURE;
  }

  for (i = 0; i < hostLength; i++)
  {
    c = hostName[i];
    if (isalnum(c))
    {
      labelLength++;
      continue;
    }
    if (c == '-')
    {
      labelLength++;
      continue;
    }
    if (c == '_')
    {
      labelLength++;
      continue;
    }
    if(c == ' ' && hostName[i - 1] != ' ')
    {
      labelLength++;
      continue;
    }
    if ((c == '.') && (i > 0))
    {      
      if (hostName[i - 1] == '.')
      {
        return L7_FAILURE;
      }
      memcpy(label, &hostName[labelStart], labelLength);
      label[labelLength] = 0;
      labelStart = i + 1;

      /* verify label syntax */
      /* must not exceed 63 characters */
      if (strlen(label) <= 63)
      {
        /* must start w/ letter */
        if (isalpha(label[0]))
        {
          /* must end w/ letter or digit */
          if (isalnum(label[labelLength - 1]))
          {
            labelLength = 0;
            continue;
          }
        }
      }

      return L7_FAILURE;
    }
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Check presence of DIM feature, L7_FEAT_DIM_USE_FILENAME
*           
* @param    void
*
* @returns  L7_TRUE   if feature is present
* @returns  L7_FALSE  if feature is absent
*
* @end
*********************************************************************/
L7_BOOL usmDbDimFeaturePresentCheck(void)
{
  if (L7_FEAT_DIM_USE_FILENAME == 1)
  {
    return L7_TRUE;
  }
  return L7_FALSE;
}

/*********************************************************************
* @purpose  Check absence of DIM feature, L7_FEAT_DIM_USE_FILENAME
*           
* @param    void
*
* @returns  L7_TRUE   if feature is absent
* @returns  L7_FALSE  if feature is present
*
* @end
*********************************************************************/
L7_BOOL usmDbDimFeatureNotPresentCheck(void)
{
  return !(usmDbDimFeaturePresentCheck());
}

/******************************************************************************
 * @purpose To compare two strings and check if the source string is present in
 *          the destination string
 *
 * @param   dstString - (input) Destination String is to compare for srcString
 *          srcString - (input) Source String is to be search for
 *
 * @returns L7_SUCCESS - if srcString is present in the dstString in any form
 *                       (case in-sensitive or reverse or substring) 
 *
 * @returns L7_FAILURE - if srcString is not present in the dstString (or)
 *                       src string length is greater than dst string length
 *
 * @returns L7_ERROR - if src or dst string are NULL pointers (or)
 *                     failed to Reverse the string
 *
 * @notes  
 * @end
 ******************************************************************************/
L7_RC_t usmDbUserMgrStrNoCaseReverseCompareCheck(L7_char8 *dstString, L7_char8 *srcString)  
{
  return userMgrStrNoCaseReverseCompareCheck(dstString, srcString);
}

