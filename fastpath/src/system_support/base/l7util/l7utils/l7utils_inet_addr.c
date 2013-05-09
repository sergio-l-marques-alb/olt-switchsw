/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename   inet_addr.c
*
* @purpose    layer 3 address defs  APIS
*
*
* @comments
*
* @create     01/16/2006
*
* @author     Satya Dillikar
* @end
*
**********************************************************************/
#include <stdio.h>
#include "osapi.h"
#include "osapi_support.h"
#include "l7_ip_api.h"
#include "l3_addrdefs.h"
#include "l7utils_inet_addr_api.h"
#include "l7_socket.h"

#define INET_IPV4_INADDR_ANY              (L7_uint32)0
                                                        /* 0.0.0.0 */
#define INET_IPV4_INADDR_MAX_LOCAL_GROUP  (L7_uint32)0xe00000ff
                                                        /* 224.0.0.255 */
#define INET_IPV4_INADDR_IP_ADDR_MASK                0xFFFFFF00
                                                        /* 255.255.255.0 */

#define     L7_IP4_IN_BADCLASS(a)       ((((a)) & 0xf0000000) == 0xf0000000)

#define L7_IP6_LEN                        40
#define L7_IP4_STR_LEN                    20

#ifdef TBD
static L7_RC_t inetV6IsLANScopedAddress(L7_in6_addr_t *addr);
#endif

/*********************************************************************
* @purpose  Convert a 32-bit network mask to a length
*
* @param    inetAddr    @b{(input)}Input Host address
*
* @returns   L7_TRUE
* @returns   L7_FALSE
*
* @notes   Validation includes mask to be contiguous bits of 1
           return zero if mask is invalid
           11100100 - is invalid mask --->masklen = 0
           11100000 - is valid mask  -->masklen=3
*
* @end
*
*********************************************************************/
L7_int32 inetMaskLengthGet(L7_int32 mask)
{
 L7_int32 maskSize,maskLen, tempLen;
 L7_int32 maskBit;
 L7_BOOL zeroFound = L7_FALSE;

   maskSize = sizeof(L7_int32) <<3;
   maskBit =0x1 << (maskSize-1);
   maskLen=0;
   for(tempLen =0;tempLen <maskSize;tempLen++)
     {
       if(!(mask & maskBit))
       {
         if(zeroFound == L7_FALSE)
         {
           zeroFound = L7_TRUE;
         }
       }
       else
       {
         if(zeroFound == L7_TRUE)
         {
            return -1;
         }
         maskLen++;
       }
       mask = mask << 1;
     }

     return maskLen;
}

/*********************************************************************
* @purpose  Determine whether a given IPv4 address is a valid IPv4
*           host address.
*
* @param    ipv4Addr @b{(input)}  IPv4 address
*
* @returns   L7_TRUE
* @returns   L7_FALSE
*
* @notes
*
*
* @end
*
*********************************************************************/
L7_BOOL inetIsValidIpv4HostAddress(L7_uint32 ipv4Addr)
{
   return (!(L7_IP4_IN_MULTICAST(ipv4Addr) ||
             (L7_IP4_IN_BADCLASS(ipv4Addr)) ||
             (ipv4Addr == 0xffffffff) ||
             (ipv4Addr & 0xff000000) == 0));
}

/*********************************************************************
* @purpose  Validate an addr
*
* @param    inetAddr @b{(input)}Input Host address
*
* @returns   L7_TRUE
* @returns   L7_FALSE
*
* @notes
*
*
* @end
*
*********************************************************************/
L7_BOOL inetIsValidHostAddress_track(L7_inet_addr_t * inetAddr,
                             L7_uchar8 *fileName,
                             L7_uint32 lineNum)
{
    if (inetAddr == L7_NULLPTR)
    {
        return L7_FALSE;
    }
    if (inetAddr->family == L7_AF_INET)
    {
      if (inetIsValidIpv4HostAddress(inetAddr->addr.ipv4.s_addr))
      {
        return L7_TRUE;
      }
    }
    else if (inetAddr->family == L7_AF_INET6)
    {
      /* Broadcast is not checked in IPv6, as the same is represented
         using multicast itself.*/
      if(!(L7_IP6_IS_ADDR_MULTICAST(&inetAddr->addr.ipv6)) &&
          !(L7_IP6_IS_ADDR_UNSPECIFIED(&inetAddr->addr.ipv6)))
      {
        return L7_TRUE;
      }
    }
    else
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LOG_COMPONENT_DEFAULT,
              "INET_ADDR:Invalid FamilyType - %d from %s(%d)", inetAddr->family,
        fileName, lineNum);
    }
    /*Should never reach here */
    return L7_FALSE;
}

/*********************************************************************
* @purpose  is a multicast addr
*
* @param    addr @b{(input)}Input address
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes
*
*
* @end
*
*********************************************************************/
L7_BOOL inetIsInMulticast_track(L7_inet_addr_t *addr,
                             L7_uchar8 *fileName,
                             L7_uint32 lineNum)
{
    if (addr == L7_NULLPTR)
    {
        return L7_FALSE;
    }
    if (addr->family == L7_AF_INET)
    {
        if (L7_IP4_IN_MULTICAST(addr->addr.ipv4.s_addr))
        {
            return L7_TRUE;
        }
    }
    else if (addr->family == L7_AF_INET6)
    {
        return L7_IP6_IS_ADDR_MULTICAST(&addr->addr.ipv6);
    }
    else
    {
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LOG_COMPONENT_DEFAULT,
                "INET_ADDR:Invalid FamilyType - %d from %s(%d)", addr->family,
        fileName, lineNum);
    }
    return L7_FALSE;
}

/*********************************************************************
* @purpose  Convert mask to masklen
*
* @param    mask    @b{(input)} mask
* @param    masklen @b{(input)} masklen
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes
*
*
* @end
*
*********************************************************************/
L7_RC_t inetMaskToMaskLen(L7_inet_addr_t *mask, L7_uchar8 *masklen)
{
    L7_uint32 maskval = 0, totalLen = 0;
    L7_int32 ii=0, tempMaskLen;
    L7_in6_addr_t Ipv6Addr;
    L7_uchar8 addrBuff[IPV6_DISP_ADDR_LEN];

    if (mask == L7_NULLPTR)
    {
        return L7_FAILURE;
    }

    *masklen =  0;
    if (mask->family == L7_AF_INET)
    {
        if(inetAddressGet(L7_AF_INET, mask, (void*)&maskval) != L7_SUCCESS)
        {
          return L7_FAILURE;
        }
        tempMaskLen = inetMaskLengthGet(maskval);
        if(tempMaskLen == -1)
        {
          L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LOG_COMPONENT_DEFAULT,
                  "INETADDR: Invalid mask = %s", inetAddrPrint(mask, addrBuff));
          return L7_FAILURE;
        }
       *masklen= tempMaskLen;
    }
    else if (mask->family == L7_AF_INET6)
    {
        memset(&Ipv6Addr,0,sizeof(L7_in6_addr_t));
        if(inetAddressGet(L7_AF_INET6,mask,(void *)&Ipv6Addr) != L7_SUCCESS)
        {
          return L7_FAILURE;
        }
        totalLen = 0;
        for(ii=0;ii<4;ii++)
        {
          maskval = Ipv6Addr.in6.addr32[ii];
          tempMaskLen= inetMaskLengthGet(maskval);
          if(tempMaskLen != -1)
          {
           totalLen+=tempMaskLen;
            if (tempMaskLen !=32)
            {
             break;
            }
          }
          else
          {
            L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LOG_COMPONENT_DEFAULT,
                    "INETADDR: Invalid mask = %s", inetAddrPrint(mask, addrBuff));
            return L7_FAILURE;
          }
        }
       *masklen= totalLen;
    } /*end of else if */
    else
    {
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LOG_COMPONENT_DEFAULT,
                "INET_ADDR: Invalid FamilyType - %d", mask->family);
    }
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Convert masklen to mask
*
* @param    family  @b{(input)} Address family (L7_AF_INET, L7_AF_INET6, etc.)
* @param    masklen @b{(input)} masklen
* @param    mask    @b{(input)} mask
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes
*
*
* @end
*
*********************************************************************/
L7_RC_t inetMaskLenToMask(L7_uchar8 family, L7_uchar8 masklen,
                          L7_inet_addr_t *mask) {
    L7_uint32 maskAddr,ctr=0,mod=0,ii;
    L7_uchar8  tmp_masklen;
    L7_in6_addr_t Ipv6Addr;

    if (mask == L7_NULLPTR )
    {
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LOG_COMPONENT_DEFAULT,
                "INET_ADDR:Invalid input maks ");
        return L7_FAILURE;
    }
    inetAddressZeroSet(family, mask);
    if (family == L7_AF_INET)
    {
        if (masklen > 32)
        {
            L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LOG_COMPONENT_DEFAULT,
                    "INET_ADDR:Invalid MaskLength - %d", masklen);
            return L7_FAILURE;
        }
        if (masklen != 0)
        {
            tmp_masklen = sizeof((maskAddr)) << 3;
            tmp_masklen -= masklen;
            maskAddr = (~0 << tmp_masklen);
            inetAddressSet(L7_AF_INET, (void *)&maskAddr, mask);
        }
    }
    else if (family == L7_AF_INET6)
    {
      if (masklen > 128)
      {
          L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LOG_COMPONENT_DEFAULT,
                  "INET_ADDR:Invalid MaskLength - %d", masklen);
          return L7_FAILURE;
      }
      memset(&Ipv6Addr,0,sizeof(L7_in6_addr_t));
      ctr = masklen/32;
      mod = masklen%32;
      for(ii=0;ii <ctr;ii++)
      {
        Ipv6Addr.in6.addr32[ii] = (~0);
      }
      if(mod !=0)
      {
        mod = 32 - mod;
        Ipv6Addr.in6.addr32[ii] = ((~0)<< mod);
      }
      inetAddressSet(L7_AF_INET6, (void *)&Ipv6Addr, mask);
    }
    else
    {
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LOG_COMPONENT_DEFAULT,
                "INET_ADDR:Invalid FamilyType - %d", family);
        return L7_FAILURE;
    }
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Convert inet addr to a string
*
* @param   addr   @b{(input)} inetAddress
*   @param   string @b{(input)} Output String
*
* @returns  L7_FAILURE
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t inetAddrHtop(L7_inet_addr_t *addr, L7_uchar8 *string)
{

    if (addr == L7_NULLPTR || string == L7_NULLPTR)
    {
        return L7_FAILURE;
    }
    if (addr->family == L7_AF_INET)
    {
        osapiInetNtoa(addr->addr.ipv4.s_addr,string);
    }
    else if (addr->family == L7_AF_INET6)
    {
        osapiInetNtop(L7_AF_INET6,(L7_uchar8 *)&addr->addr.ipv6,string,
                     L7_IP6_LEN);
    }
    else {
      osapiSnprintf(string, L7_IP6_LEN, "Wrong-family-addr");
    }
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  NtoH conversion
*
* @param  addr   @b{(input)} inetAddress
* @param    addr_h @b{(input)} inetAddress
*
* @returns   L7_FAILURE
* @returns   L7_SUCCESS
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t inetAddrNtoh(L7_inet_addr_t * addr, L7_inet_addr_t * addr_h)
{

    if (addr == L7_NULLPTR || addr_h == L7_NULLPTR)
    {
        return L7_FAILURE;
    }

    inetAddressReset(addr_h);
    if (addr->family == L7_AF_INET)
    {
        addr_h->family = L7_AF_INET;
        addr_h->addr.ipv4.s_addr = osapiNtohl(addr->addr.ipv4.s_addr);
    }
    else if (addr->family == L7_AF_INET6)
    {
        /*In case of IPv6 ,nothing has to be done  */
        inetCopy(addr_h, addr);
    }
    else
    {
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LOG_COMPONENT_DEFAULT,
                "INET_ADDR:Invalid FamilyType - %d", addr->family);
    }
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Hton conversion
*
* @param    addr   @b{(input)} InetAddress
* @param    addr_n @b{(input)} InetAddress
*
* @returns   L7_FAILURE
* @returns   L7_SUCCESS
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t inetAddrHton(L7_inet_addr_t * addr, L7_inet_addr_t * addr_n)
{

    if (addr == L7_NULLPTR || addr_n == L7_NULLPTR)
    {
        return L7_FAILURE;
    }
    inetAddressReset(addr_n);
    if (addr->family == L7_AF_INET)
    {
        addr_n->family = L7_AF_INET;
        addr_n->addr.ipv4.s_addr = osapiHtonl(addr->addr.ipv4.s_addr);
    }
    else if (addr->family == L7_AF_INET6)
    {
        /*In case of IPv6 ,nothing has to be done  */
       inetCopy(addr_n, addr);
    }
    else
    {
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LOG_COMPONENT_DEFAULT,
                "INET_ADDR:Invalid FamilyType - %d", addr->family);
    }
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set to zero
*
* @param    inetAddr @b{(input)} inetAddress
*
* @returns   L7_FAILURE
* @returns   L7_SUCCESS
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t inetAddressReset(L7_inet_addr_t * inetAddr)
{
    if (inetAddr != L7_NULLPTR)
    {
        memset(inetAddr, 0, sizeof(L7_inet_addr_t));
        return L7_SUCCESS;
    }
    return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get an IPV4/IPV6 address field
*
* @param    inetAddr @b{(input)} inetAddress
* @param    family   @b{(input)} Address family (L7_AF_INET, L7_AF_INET6, etc.)
*   @param    addr     @b{(input)} (L7_uint32  in addr for ipv4 ,
                                  L7_uchar8* in addr for ipv6)
* @returns   L7_FAILURE
* @returns   L7_SUCCESS
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t inetAddressGet(L7_uchar8 family, L7_inet_addr_t *inetAddr, void *addr)
{
    L7_uint32 *value;
    if ( addr == L7_NULLPTR || inetAddr == L7_NULLPTR)
    {
        return L7_FAILURE;
    }
    if (family == L7_AF_INET)
    {
        value = (L7_uint32*) addr;
        *value = (inetAddr->addr.ipv4.s_addr);
    }
    else if (family == L7_AF_INET6)
    {
        memcpy((L7_in6_addr_t *)addr,&inetAddr->addr.ipv6,
               sizeof(L7_in6_addr_t));
    }
    else
    {
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LOG_COMPONENT_DEFAULT,
                "INET_ADDR:Invalid FamilyType - %d", family);
    }
    return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Set an IPV4/IPV6 address field
*
* @param    family  @b{(input)}Address family (L7_AF_INET, L7_AF_INET6, etc.)
*   @param    addr    @b{(input)}(L7_uint32*  in addr for ipv4 ,
* @param                               L7_uchar8* in addr for ipv6)
* @param    inetAddr @b{(input)}InetAddress
*
* @returns   L7_FAILURE
* @returns   L7_SUCCESS
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t inetAddressSet(L7_uchar8 family,void *addr, L7_inet_addr_t *inetAddr)
{
    L7_uint32 *value;
    if ( addr == L7_NULLPTR || inetAddr == L7_NULLPTR)
    {
        return L7_FAILURE;
    }
    inetAddressReset(inetAddr);
    if (family == L7_AF_INET)
    {
        value = addr;
        memcpy(&(inetAddr->addr.ipv4.s_addr),value,sizeof(L7_uint32));
        inetAddr->family = L7_AF_INET;
    }
    else if (family == L7_AF_INET6)
    {
        inetAddr->family = L7_AF_INET6;
        memcpy(&inetAddr->addr.ipv6,(L7_in6_addr_t *)addr,
                  sizeof(L7_in6_addr_t));
    }
    else
    {
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LOG_COMPONENT_DEFAULT,
                "INET_ADDR:Invalid FamilyType - %d", family);
    }
    return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Set an IPV4/IPV6 address field
*
* @param    Input: family ,
*                        addr (L7_uint32*  in addr for ipv4 ,
                                 L7_uchar8* in addr for ipv6)
                Output: inetAddr
* @returns   L7_FAILURE / L7_SUCCESS
*
* @notes
*
*
* @end
*********************************************************************/
L7_RC_t inetAddressZeroSet(L7_uchar8 family, L7_inet_addr_t *inetAddr)
{
    L7_in6_addr_t addripv6;
    L7_uint32  addripv4;

    if (inetAddr == L7_NULLPTR) {
        return L7_FAILURE;
    }
    if (family == L7_AF_INET) {
      addripv4 = 0;
      inetAddressSet(family,(void *) &addripv4, inetAddr);
    } else if (family == L7_AF_INET6) {
       memset(&addripv6, 0, sizeof(L7_in6_addr_t));
       inetAddressSet(family, (void *)&addripv6, inetAddr);
    }
    else
    {
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LOG_COMPONENT_DEFAULT,
                "INET_ADDR:Invalid FamilyType - %d", family);
    }
    return L7_SUCCESS;

}
/*********************************************************************
* @purpose  Copy Address
*
* @param    src  @b{(input)}InetAddress
*   @param    dest @b{(input)}InetAddress
*
* @returns   L7_FAILURE
* @returns   L7_SUCCESS
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t inetCopy_track(L7_inet_addr_t *dest, L7_inet_addr_t *src,
                L7_uchar8 *fileName, L7_uint32 lineNum)
{
    if ( dest == L7_NULLPTR || src == L7_NULLPTR)
    {
        return L7_FAILURE;
    }
    if(src->family != L7_AF_INET  && src->family != L7_AF_INET6)
    {
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LOG_COMPONENT_DEFAULT,
                "\n INET_ADDR:Invalid FamilyType - %d from %s(%d)",
        src->family, fileName, lineNum);
     /*return L7_FAILURE;*/
    }
    memcpy(dest, src, sizeof(L7_inet_addr_t));
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set addr to INADDR_ANY
*
* @param   family @b{(input)} Address family (L7_AF_INET, L7_AF_INET6, etc.)
* @param     addr   @b{(input)} InetAddress
*
* @returns   L7_FAILURE
* @returns   L7_SUCCESS
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t inetInAddressAnyInit(L7_uchar8 family, L7_inet_addr_t *addr)
{
    if (addr == L7_NULLPTR)
    {
        return L7_FAILURE;
    }
    inetAddressReset(addr);
    if (family == L7_AF_INET)
    {
        addr->family = L7_AF_INET;
        addr->addr.ipv4.s_addr = INET_IPV4_INADDR_ANY;
    }
    else if (family == L7_AF_INET6)
    {
      addr->family = L7_AF_INET6;
      memset(&addr->addr.ipv6 ,0,sizeof(L7_in6_addr_t));
    }
    else
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LOG_COMPONENT_DEFAULT,
              "INET_ADDR:Invalid FamilyType - %d", family);
    }
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Test addr to match INADDR_ANY
*
* @param    addr @b{(input)} InetAddress
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL inetIsInAddressAny(L7_inet_addr_t *addr)
{

    if (addr == L7_NULLPTR)
    {
        return L7_FALSE;
    }
    if (addr->family == L7_AF_INET)
    {
        if (addr->addr.ipv4.s_addr == INET_IPV4_INADDR_ANY)
        {
            return L7_TRUE;
        }
    }
    else if (addr->family == L7_AF_INET6)
    {
      if(L7_IP6_IS_ADDR_UNSPECIFIED(&addr->addr.ipv6))
        return L7_TRUE;
    }
    else
    {
      L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LOG_COMPONENT_DEFAULT,
              "INET_ADDR:Invalid FamilyType - %d", addr->family);
    }
    return L7_FALSE;
}

/*********************************************************************
* @purpose  Test addr to match zero
*
* @param   addr @b{(input)} InetAddress
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes
*
* @end
*
*********************************************************************/
L7_BOOL inetIsAddressZero_track(L7_inet_addr_t *addr,
                             L7_uchar8 *fileName,
                             L7_uint32 lineNum)
{
  L7_inet_addr_t zeroaddr;

  if(addr == L7_NULLPTR)
  {
    return L7_FALSE;
  }

  memset(&zeroaddr, 0, sizeof(L7_inet_addr_t));
  if(addr->family == L7_AF_INET)
  {
    if(addr->addr.ipv4.s_addr == 0)
      return L7_TRUE;
    return L7_FALSE;
  }
  else if(addr->family == L7_AF_INET6)
  {
    if(memcmp(&(addr->addr.ipv6), &(zeroaddr.addr.ipv6), sizeof(struct L7_in6_addr_s)) == 0)
    {
      return L7_TRUE;
    }
    return L7_FALSE;
  }
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LOG_COMPONENT_DEFAULT,
            "INET_ADDR:Invalid FamilyType - %d from %s(%d)", addr->family,
        fileName, lineNum);
    return L7_FALSE;
  }
}

/*********************************************************************
* @purpose Test addr to match INADDR_MAX_LOCAL_GROUP (LAN scoped addresses )
*
* @param    addr @b{(input)} InetAddress
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes
*
* @end
*
*********************************************************************/
L7_BOOL inetIsLANScopedAddress(L7_inet_addr_t *addr)
{
   if (addr == L7_NULLPTR)
   {
    return L7_FALSE;
   }

  if(addr->family == L7_AF_INET)
  {

    if((addr->addr.ipv4.s_addr >= INET_IPV4_ALL_MCAST_GROUPS_ADDR ) &&
       (addr->addr.ipv4.s_addr <= INET_IPV4_INADDR_MAX_LOCAL_GROUP))
    {
      return L7_TRUE;
    }
  }
  else if(addr->family == L7_AF_INET6)
  {
    if(L7_IP6_IS_ADDR_MULTICAST_LOC_SCOPE(&addr->addr.ipv6))
    return L7_TRUE;
  }
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LOG_COMPONENT_DEFAULT,
            "INET_ADDR:Invalid FamilyType - %d", addr->family);
  }
  return L7_FALSE;
}
#ifdef TBD
/*********************************************************************
* @purpose  Test addr to match  reserved multicast  addresses
*
* @param   addr @b{(input)} InetAddress
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*
*********************************************************************/
static L7_RC_t inetV6IsLANScopedAddress(L7_in6_addr_t *addr)
{

  if(L7_NULLPTR == addr)
    return L7_FAILURE;

  if(((osapiNtohs(addr->in6.addr16[0]) & 0xff00) == 0xff00) &&
     (osapiNtohs(addr->in6.addr16[1]) == 0x0) &&
     (osapiNtohl(addr->in6.addr32[1]) == 0x0) &&
     (osapiNtohl(addr->in6.addr32[2]) == 0x0) &&
     (osapiNtohl(addr->in6.addr32[3]) == 0x0)
     )
   {
     return L7_SUCCESS;
   }

  return L7_FAILURE;
}
#endif
/*********************************************************************
* @purpose  Test addr to match ALL_MCAST_GROUPS_ADDR
*
* @param    addr @b{(input)} InetAddress
*
* @returns   L7_TRUE
* @returns   L7_FALSE
*
* @notes
*
* @end
*
*********************************************************************/
L7_BOOL inetIsLinkLocalMulticastAddress(L7_inet_addr_t *addr)
{
    L7_uint32   ipAddr;

    if (addr == L7_NULLPTR)
    {
        return L7_FALSE;
    }
    if (addr->family == L7_AF_INET)
    {
        ipAddr = addr->addr.ipv4.s_addr;
        ipAddr = osapiNtohl(ipAddr);
        if (ipAddr >= L7_IP_MCAST_BASE_ADDR &&
            ipAddr <= L7_IP_MAX_LOCAL_MULTICAST)
            return L7_TRUE;

    }
    else if (addr->family == L7_AF_INET6)
    {
       if(L7_IP6_IS_ADDR_MULTICAST_LOC_SCOPE(&addr->addr.ipv6))
        return L7_TRUE;
    }
    else
    {
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LOG_COMPONENT_DEFAULT,
                "INET_ADDR:Invalid FamilyType - %d", addr->family);
    }
    return L7_FALSE;
}

/*********************************************************************
* @purpose  Verify if the given NetMask is valid : A series of 1s followed
*           by a series of 0s, with not discontinuous 1s.
*
* @param    pMask @b{(input)} pointer to the Network mask in IP address format
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes
*
* @end
*
*********************************************************************/
L7_BOOL inetIsMaskValid(L7_inet_addr_t *pMask)
{
  /* Basic sanity checks */
  if(pMask == L7_NULLPTR)
    return L7_FALSE;

  /* Which Family is it? */
  if(pMask->family == L7_AF_INET)
  {
    if(~((((pMask->addr.ipv4.s_addr) & -(pMask->addr.ipv4.s_addr)) - 1) | (pMask->addr.ipv4.s_addr)) != 0)
    {
      /* Mask is not contiguous */
      return L7_FALSE;
    }
    return L7_TRUE;
  }
  else if(pMask->family == L7_AF_INET6)
  {
    L7_uint32 ii;
    for(ii=0;ii<4;ii++)
    {
      if(~((((osapiNtohl(pMask->addr.ipv6.in6.addr32[ii])) &
             -(osapiNtohl(pMask->addr.ipv6.in6.addr32[ii]))) - 1) |
              (osapiNtohl(pMask->addr.ipv6.in6.addr32[ii]))) != 0)
      {
        /* Mask is not contiguous */
        return L7_FALSE;
      }
    } /*end of for loop */
    return L7_TRUE;
  }
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LOG_COMPONENT_DEFAULT,
            "INET_ADDR:Invalid FamilyType - %d", pMask->family);
  }
  return L7_FALSE;
}

/*********************************************************************
* @purpose  Bitwise AND two addresses
*
* @param  src1 @b{(input)} inetAddress
* @param  src2 @b{(input)} inetAddress
*   @param  dest @b{(input)} inetAddress
*
* @returns  L7_FAILURE
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t inetAddressAnd(L7_inet_addr_t *src1, L7_inet_addr_t *src2, L7_inet_addr_t *dest)
{
  if(src1 == L7_NULLPTR || src2 == L7_NULLPTR || dest == L7_NULLPTR)
    return L7_FAILURE;

  if(src1->family == L7_AF_INET6)
  {
    dest->addr.ipv6.in6.addr32[0] = src1->addr.ipv6.in6.addr32[0] & src2->addr.ipv6.in6.addr32[0];
    dest->addr.ipv6.in6.addr32[1] = src1->addr.ipv6.in6.addr32[1] & src2->addr.ipv6.in6.addr32[1];
    dest->addr.ipv6.in6.addr32[2] = src1->addr.ipv6.in6.addr32[2] & src2->addr.ipv6.in6.addr32[2];
    dest->addr.ipv6.in6.addr32[3] = src1->addr.ipv6.in6.addr32[3] & src2->addr.ipv6.in6.addr32[3];
    dest->family = L7_AF_INET6;
  }
  else if(src1->family == L7_AF_INET)
  {
    dest->addr.ipv4.s_addr = src1->addr.ipv4.s_addr & src2->addr.ipv4.s_addr;
    dest->family = L7_AF_INET;
  }
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LOG_COMPONENT_DEFAULT,
            "INET_ADDR:Invalid FamilyType - %d", src1->family);
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Bitwise OR two addresses
*
* @param    src1 @b{(input)} inetAddress
* @param    src2 @b{(input)} inetAddress
*   @param    dest @b{(input)} inetAddress
*
* @returns  L7_FAILURE
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t inetAddressOr(L7_inet_addr_t *src1, L7_inet_addr_t *src2, L7_inet_addr_t *dest)
{
  if(src1 == L7_NULLPTR || src2 == L7_NULLPTR || dest == L7_NULLPTR)
    return L7_FAILURE;

  if(src1->family == L7_AF_INET6)
  {
    dest->addr.ipv6.in6.addr32[0] = src1->addr.ipv6.in6.addr32[0] | src2->addr.ipv6.in6.addr32[0];
    dest->addr.ipv6.in6.addr32[1] = src1->addr.ipv6.in6.addr32[1] | src2->addr.ipv6.in6.addr32[1];
    dest->addr.ipv6.in6.addr32[2] = src1->addr.ipv6.in6.addr32[2] | src2->addr.ipv6.in6.addr32[2];
    dest->addr.ipv6.in6.addr32[3] = src1->addr.ipv6.in6.addr32[3] | src2->addr.ipv6.in6.addr32[3];
    dest->family = L7_AF_INET6;
  }
  else if(src1->family == L7_AF_INET)
  {
    dest->addr.ipv4.s_addr = src1->addr.ipv4.s_addr | src2->addr.ipv4.s_addr;
    dest->family = L7_AF_INET;
  }
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LOG_COMPONENT_DEFAULT,
            "INET_ADDR:Invalid FamilyType - %d", src1->family);
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Bitwise NOT the given address
*
* @param   src  @b{(input)} inetAddress
* @param     dest @b{(input)} inetAddress
*
* @returns  L7_FAILURE
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t inetAddressNot(L7_inet_addr_t *src, L7_inet_addr_t *dest)
{
  if(src == L7_NULLPTR || dest == L7_NULLPTR)
    return L7_FAILURE;

  if(src->family == L7_AF_INET6)
  {
    dest->addr.ipv6.in6.addr32[0] = ~(src->addr.ipv6.in6.addr32[0]);
    dest->addr.ipv6.in6.addr32[1] = ~(src->addr.ipv6.in6.addr32[1]);
    dest->addr.ipv6.in6.addr32[2] = ~(src->addr.ipv6.in6.addr32[2]);
    dest->addr.ipv6.in6.addr32[3] = ~(src->addr.ipv6.in6.addr32[3]);
    dest->family = L7_AF_INET6;
  }
  else if(src->family == L7_AF_INET)
  {
    dest->addr.ipv4.s_addr = ~(src->addr.ipv4.s_addr);
    dest->family = L7_AF_INET;
  }
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LOG_COMPONENT_DEFAULT,
            "INET_ADDR:Invalid FamilyType - %d", src->family);
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Calculates the Checksum
*
* @param    header @b{(input)}
*           len @b{(input)}
* @returns   checksum value.
*
* @notes
*
* @end
*
*********************************************************************/

L7_int32 inetChecksum(void *header,  L7_int32 len)
{
  L7_ulong32 sum = 0;

    L7_ushort16 *sp = header;

    L7_int32 odd = (len % 2);

    len /= 2;
    while (--len >= 0)
        sum += *sp++;
    if (odd)
    {
        L7_uchar8 pad[2];
    pad[0] = *(L7_uchar8 *)sp;
    pad[1] = 0;
    sp = (L7_ushort16 *)pad;
    sum += *sp;
    }
    while (sum > 0xffff)
    sum = (sum & 0xffff) + (sum >> 16);
    sum = ~sum & 0xffff;
    return (sum);
}

/********************************************************************
* @purpose  Determines whether the given address is admin scoped.
*
* @param    addr @b{(input)}inetaddress
*
* @returns  L7_TRUE  adminscoped address.
* @returns  L7_FALSE not adminscoped address.
*
* @notes    This API is used for checking scopes in PIMSM BSR
*
* @end
*
*********************************************************************/
L7_BOOL inetIsAddrMulticastScope(L7_inet_addr_t *addr)
{
  if(addr == L7_NULLPTR)
    return L7_FAILURE;

  if(addr->family == L7_AF_INET)
  {
    /*There is no concept of scoping in IPv4.Hence returning L7_FALSE*/
      return L7_FALSE;
  }
  else if(addr->family == L7_AF_INET6)
  {
    if((addr->addr.ipv6.in6.addr8[0] == 0xFF)
       && ((addr->addr.ipv6.in6.addr8[1]) & 0x0f) != 0xe)
      return L7_TRUE;
  }
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LOG_COMPONENT_DEFAULT,
            "INET_ADDR:Invalid FamilyType - %d", addr->family);
  }
  return L7_FALSE;
}

/********************************************************************
* @purpose  Determines addr1&mask1 == addr2&mask2
*
* @param    addr1 @b{(input)} inetaddress
* @param    mask  @b{(input)} mask
* @param    addr2 @b{(input)} inetaddress
* @param    mask  @b{(input)} mask
*
* @returns  integer (same as memcmp)
*
* @notes
*
* @end
*
*********************************************************************/
L7_int32  inetAddrCompareAddrWithMask(L7_inet_addr_t *addr1, L7_uint32 masklen1,
                                      L7_inet_addr_t *addr2, L7_uint32 masklen2)
{
  L7_inet_addr_t mask1, mask2;
  L7_inet_addr_t node1,node2;


  if (addr1->family != addr2->family)
    return(addr1->family - addr2->family);

  /* compare the grp address & mask combined together */
  inetMaskLenToMask(addr1->family,masklen1,&mask1);
  inetMaskLenToMask(addr2->family,masklen2,&mask2);
  inetAddressAnd(addr1, &mask1, &node1);
  inetAddressAnd(addr2, &mask2, &node2);

  return L7_INET_ADDR_COMPARE(&node1,&node2);
}

L7_int32   inetAddrCompareAddrWithMaskIndividual(L7_inet_addr_t *addr1, L7_uint32 masklen1,
                                      L7_inet_addr_t *addr2, L7_uint32 masklen2)
{
  L7_int32 retVal = 0;

  if (addr1->family != addr2->family)
    return(addr1->family - addr2->family);

#if 0
  /* compare the grp address & mask combined together */
  inetMaskLenToMask(addr1->family,masklen1,&mask1);
  inetMaskLenToMask(addr2->family,masklen2,&mask2);
  inetAddressAnd(addr1, &mask1, &node1);
  inetAddressAnd(addr2, &mask2, &node2);
#endif
  if((retVal = L7_INET_ADDR_COMPARE( addr1, addr2 )) == L7_NULL)
  {
    return (masklen1 - masklen2);
  }

  return retVal;
}


/********************************************************************
* @purpose  This function return the scope id for ipv6 multicast address
*           for the rest of the addresses it returns -1.
*
* @param    addr1 @b{(input)} inetaddress
*
* @returns  scopeid
*
* @notes
*
* @end
*
*********************************************************************/
L7_int32 inetAddrGetMulticastScopeId(L7_inet_addr_t *addr)
{
  if(addr->family == L7_AF_INET6)
  {
    if(addr->addr.ipv6.in6.addr8[0] == 0xFF)
    {
      return ((addr->addr.ipv6.in6.addr8[1]) & 0x0f);
    }
  }
  return -1;
}

/********************************************************************
* @purpose  Determines addr1/mask is it better prefix match than addr2/mask
*
* @param    addr1 @b{(input)} inetaddress
* @param    mask1 @b{(input)} mask
* @param    addr2 @b{(input)} inetaddress
* @param    mask2 @b{(input)} mask
*
* @returns  L7_TRUE if addr1/mask is better match.
* @returns  L7_FALSE
*
* @notes
*
* @end
*
*********************************************************************/
L7_BOOL  inetAddrIsScopedInsideAnother(L7_inet_addr_t *addr1, L7_uint32 mask1,
                                       L7_inet_addr_t *addr2, L7_uint32 mask2)
{
  if(addr1->family == L7_AF_INET)
  {
    if(inetAddrCompareAddrWithMask(addr1,mask1,addr2,mask2) >= 0)
      return L7_TRUE;
  }
  else if(addr1->family == L7_AF_INET6)
  {
    if(inetAddrGetMulticastScopeId(addr1) < inetAddrGetMulticastScopeId(addr2))
      return L7_TRUE;
    if(inetAddrGetMulticastScopeId(addr1) == inetAddrGetMulticastScopeId(addr2))
      if(mask1 > mask2)
        return L7_TRUE;
  }
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LOG_COMPONENT_DEFAULT,
            "INET_ADDR:Invalid FamilyType - %d", addr1->family);
  }
  return L7_FALSE;
}

/*********************************************************************
*
* @purpose  To Print an Inet Address
*
* @param    inetAddr   @b{ (input) } Pointer to the Inet Address Type
*           buf        @b{ (input) } Buffer to be printed (48 bytes)
 *
* @returns  String buffer, if success
* @returns  L7_NULLPTR, if failure
*
* @notes
*
* @end
*********************************************************************/
L7_uchar8*
inetAddrPrint (L7_inet_addr_t *inetAddr, L7_uchar8 *buf)
{
  if ((inetAddr == L7_NULLPTR) || (buf == L7_NULLPTR))
    return L7_NULLPTR;

  memset (buf, 0, IPV6_DISP_ADDR_LEN);

  inetAddrHtop (inetAddr, buf);

  return buf;
}

/*********************************************************************
* @purpose  Get an inet6 address from inet address
*
* @param    inetAddr @b{(input)} inetAddress
*   @param    inet6Addr @b{(output)} inet6 Addr
*
* @returns   L7_FAILURE
* @returns   L7_SUCCESS
*
* @notes
*
* @end
*
*********************************************************************/
L7_RC_t inet6AddressGet(L7_inet_addr_t *inetAddr, L7_in6_addr_t *in6Addr)
{
    if (in6Addr == L7_NULLPTR || inetAddr == L7_NULLPTR)
    {
        return L7_FAILURE;
    }
    if (inetAddr->family == L7_AF_INET6)
    {
        memcpy(in6Addr, &inetAddr->addr.ipv6, sizeof(L7_in6_addr_t));
    }
    else
    {
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_LOG_COMPONENT_DEFAULT,
                "INET_ADDR:Invalid FamilyType - %d", inetAddr->family);
        return L7_FAILURE;
    }
    return L7_SUCCESS;
}
/*********************************************************************
* @purpose  check if host bits are set in the inetAddr when compared
*           with the inetMask
*
* @param    inetAddr  @b{(input)}InetAddress
* @param    inetMask  @b{(input)}InetAddress
*
* @returns   L7_TRUE , if host bits are set
* @returns   L7_FALSE, else case.
*
* @notes
*
* @end
*
*********************************************************************/
L7_BOOL inetAddrIsHostBitSet(L7_inet_addr_t *inetAddr, L7_inet_addr_t *inetMask)
{
  L7_inet_addr_t inetTemp;

  /* Check if host bits are set in the inetAddr when compared with the inetMask.*/
  inetAddressZeroSet(inetAddr->family, &inetTemp);
  inetAddressAnd(inetAddr, inetMask, &inetTemp);

  if (L7_INET_IS_ADDR_EQUAL(inetAddr, &inetTemp) == L7_FALSE)
  {
    /* If the given address is not equal to the ANDed address, then host bits are set.*/
    return L7_TRUE;
  }
  return L7_FALSE;
}
/*********************************************************************
* @purpose  check the class of given address for a given family
*
*
* @param  adddrFamily @b{(input)}AddressFamily
* @param  inetAddr    @b{(input)}inet address
*
* @returns   L7_TRUE ,if srcAddr has class A,B,C(except loop back)
* @returns   L7_FALSE, else case.
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t inetIpAddressValidityCheck (L7_uchar8 addrFamily,
                                                                 L7_inet_addr_t *inetAddr)
{
  if (addrFamily == L7_AF_INET)
  {
    /* reject the class A net 0 address */
    if ((( (inetAddr->addr.ipv4.s_addr) & IN_CLASSA_NET) >> IN_CLASSA_NSHIFT) == 0)
    {
      return L7_NOT_SUPPORTED;
    }
     /* reject the class A net 127 (loopback) address */
    if ((( (inetAddr->addr.ipv4.s_addr) & IN_CLASSA_NET) >> IN_CLASSA_NSHIFT) == IN_LOOPBACKNET)
    {
      return L7_NOT_SUPPORTED;
    }
     /* accept all other class A */
    if ((L7_BOOL)IN_CLASSA(inetAddr->addr.ipv4.s_addr) == L7_TRUE)
    {
      return L7_SUCCESS;
    }
     /* accept all class B */
    if ((L7_BOOL)IN_CLASSB(inetAddr->addr.ipv4.s_addr) == L7_TRUE)
    {
      return L7_SUCCESS;
    }
    if ((L7_BOOL)IN_CLASSC(inetAddr->addr.ipv4.s_addr) == L7_TRUE)
    {
      return L7_SUCCESS;
    }
     /* reject everything else (class D, E, etc.) */
    return L7_FAILURE;
  }
  else if (addrFamily == L7_AF_INET6)
  {
    if(L7_IP6_IS_ADDR_LOOPBACK(inetAddr->addr.ipv6.in6. addr32))
    {
      return(L7_NOT_SUPPORTED);
    }
    if(L7_IP6_IS_ADDR_LINK_LOCAL(inetAddr->addr.ipv6.in6. addr32))
    {
      return(L7_NOT_SUPPORTED);
    }
    if(L7_IP6_IS_ADDR_MULTICAST(inetAddr->addr.ipv6.in6. addr32))
    {
      return(L7_NOT_SUPPORTED);
    }
    if(L7_IP6_IS_ADDR_V4COMPAT(inetAddr->addr.ipv6.in6. addr32))
    {
      return(L7_FAILURE);
    }
    if(L7_IP6_IS_ADDR_V4MAPPED(inetAddr->addr.ipv6.in6. addr32))
    {
      return(L7_FAILURE);
    }
    return L7_SUCCESS;
  }
  else
  {
   return L7_FAILURE;
  }
}
