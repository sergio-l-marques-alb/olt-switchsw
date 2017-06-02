
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename   l7utils_inet_addr_apt.h
*
* @purpose    Inet Address APIs
*
* @component  osapi
*
* @comments   none
*
* @create     
*
* @author     jpp
* @end
*
**********************************************************************/
#include "l3_addrdefs.h"

/*********************************************************************
* @purpose  Convert a 32-bit network mask to a length
*
* @param    inetAddr    @b{(input)}Input Host address
*
* @returns   number of 1 bits in the mask 
*
* @notes   
*       
* @end
*
*********************************************************************/
L7_int32 inetMaskLengthGet(L7_int32 mask);

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
* @end
*
*********************************************************************/
L7_BOOL inetIsValidIpv4HostAddress(L7_uint32 ipv4Addr);

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
#define inetIsValidHostAddress(inetAddr)            \
            inetIsValidHostAddress_track(inetAddr, __FILE__, __LINE__)

EXT_API L7_BOOL inetIsValidHostAddress_track(L7_inet_addr_t * inetAddr,
                             L7_uchar8 *fileName,   
                             L7_uint32 lineNum);

/*********************************************************************
* @purpose  Convert inet addr to a string 
*
* @param   addr   @b{(input)} inetAddress
*	@param	 string @b{(input)} Output String
*
* @returns  L7_FAILURE 
* @returns  L7_SUCCESS
*
* @notes    
*           
* @end
*
*********************************************************************/
EXT_API L7_RC_t inetAddrHtop(L7_inet_addr_t *addr, L7_uchar8 *string);

/*********************************************************************
* @purpose  NtoH conversion
*
* @param  addr   @b{(input)} inetAddress
* @param	addr_h @b{(input)} inetAddress
*
* @returns   L7_FAILURE
* @returns   L7_SUCCESS
*
* @notes    
*           
* @end
*
*********************************************************************/
EXT_API L7_RC_t inetAddrNtoh(L7_inet_addr_t * addr_n, L7_inet_addr_t * addr_h);

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
EXT_API L7_RC_t inetAddrHton(L7_inet_addr_t * addr_h, L7_inet_addr_t * addr_n);

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
EXT_API L7_RC_t	inetAddressReset(L7_inet_addr_t * inetAddr);

/*********************************************************************
* @purpose  Get an IPV4/IPV6 address field
*
* @param    inetAddr @b{(input)} inetAddress
* @param    family   @b{(input)} Address family (L7_AF_INET, L7_AF_INET6, etc.)
*	@param    addr     @b{(input)} (L7_uint32  in addr for ipv4 ,
                                  L7_uchar8* in addr for ipv6)
* @returns   L7_FAILURE  
* @returns   L7_SUCCESS
*
* @notes    
*       
* @end
*
*********************************************************************/
EXT_API L7_RC_t inetAddressGet(L7_uchar8 family, L7_inet_addr_t *inetAddr, void *addr);

/*********************************************************************
* @purpose  Set an IPV4/IPV6 address field
*
* @param    family  @b{(input)}Address family (L7_AF_INET, L7_AF_INET6, etc.)
*	@param	  addr    @b{(input)}(L7_uint32*  in addr for ipv4 ,
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
EXT_API L7_RC_t inetAddressSet(L7_uchar8 family,  void *addr, L7_inet_addr_t *inetAddr);

/*********************************************************************
* @purpose  Set an IPV4/IPV6 address field
*
* @param    family   @b{(input)}Address family (L7_AF_INET, L7_AF_INET6, etc.)
* @param    inetAddr @b{(output)} inetAddress
*
* @returns   L7_FAILURE / L7_SUCCESS
*
* @notes    
*           
*       
* @end
*********************************************************************/
EXT_API L7_RC_t inetAddressZeroSet(L7_uchar8 family,  L7_inet_addr_t *inetAddr);

/*********************************************************************
* @purpose  Copy Address
*
* @param    src  @b{(input)}InetAddress
*	@param    dest @b{(output)}InetAddress
*
* @returns   L7_FAILURE 
* @returns   L7_SUCCESS
*
* @notes    
*       
* @end
*
*********************************************************************/
#define inetCopy(dest,src)               \
        inetCopy_track(dest,src, __FILE__, __LINE__)
EXT_API L7_RC_t inetCopy_track(L7_inet_addr_t *dest, L7_inet_addr_t *src,
                        L7_uchar8 *fileName,L7_uint32 lineNum);

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
EXT_API L7_BOOL inetIsLANScopedAddress(L7_inet_addr_t *addr);

/*********************************************************************
* @purpose  Set addr to INADDR_ANY
*
* @param   family @b{(input)} Address family (L7_AF_INET, L7_AF_INET6, etc.)
* @param	 addr   @b{(input)} InetAddress
*
* @returns   L7_FAILURE 
* @returns   L7_SUCCESS
*
* @notes    
*       
* @end
*
*********************************************************************/
EXT_API L7_RC_t inetInAddressAnyInit(L7_uchar8 family, L7_inet_addr_t *addr);

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
EXT_API L7_BOOL inetIsInAddressAny(L7_inet_addr_t *addr);

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
#define inetIsAddressZero(addr)            \
            inetIsAddressZero_track(addr, __FILE__, __LINE__)

EXT_API L7_BOOL inetIsAddressZero_track(L7_inet_addr_t *addr,
                             L7_uchar8 *fileName,   
                             L7_uint32 lineNum);

/*********************************************************************
* @purpose  Test if addr is a multicast addr
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
#define inetIsInMulticast(addr)            \
            inetIsInMulticast_track(addr, __FILE__, __LINE__)

EXT_API L7_BOOL inetIsInMulticast_track(L7_inet_addr_t *addr,
                             L7_uchar8 *fileName,   
                             L7_uint32 lineNum);

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
EXT_API L7_RC_t inetMaskToMaskLen(L7_inet_addr_t *mask, L7_uchar8 *masklen);

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
EXT_API L7_RC_t inetMaskLenToMask(L7_uchar8 family, L7_uchar8 masklen, 
                          L7_inet_addr_t *mask);

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
EXT_API L7_BOOL inetIsLinkLocalMulticastAddress(L7_inet_addr_t *addr);

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
EXT_API L7_BOOL inetIsMaskValid(L7_inet_addr_t *pMask);

/*********************************************************************
* @purpose  Bitwise AND two addresses
*
* @param  src1 @b{(input)} inetAddress
* @param  src2 @b{(input)} inetAddress
*	@param  dest @b{(input)} inetAddress
*
* @returns  L7_FAILURE 
* @returns  L7_SUCCESS
*
* @notes    
*           
* @end
*
*********************************************************************/
EXT_API L7_RC_t inetAddressAnd(L7_inet_addr_t *src1, L7_inet_addr_t *src2, L7_inet_addr_t *dest);

/*********************************************************************
* @purpose  Bitwise OR two addresses
*
* @param    src1 @b{(input)} inetAddress
* @param    src2 @b{(input)} inetAddress
*	@param    dest @b{(input)} inetAddress
*
* @returns  L7_FAILURE 
* @returns  L7_SUCCESS
*
* @notes    
*       
* @end
*
*********************************************************************/
EXT_API L7_RC_t inetAddressOr(L7_inet_addr_t *src1, L7_inet_addr_t *src2, L7_inet_addr_t *dest);

/*********************************************************************
* @purpose  Bitwise NOT the given address
*
* @param   src  @b{(input)} inetAddress
* @param	 dest @b{(input)} inetAddress
*
* @returns  L7_FAILURE 
* @returns  L7_SUCCESS
*
* @notes    
*           
* @end
*
*********************************************************************/
EXT_API L7_RC_t inetAddressNot(L7_inet_addr_t *src, L7_inet_addr_t *dest);

/*********************************************************************
* @purpose  Calculates the Checksum
*
* @param    header @b{(input)}
*           en     @b{(input)}
* @returns  checksum value.
*
* @notes    
*           
* @end
*
*********************************************************************/
EXT_API L7_int32 inetChecksum(void *ipHeader,  L7_int32 len);

/********************************************************************
* @purpose  Determines whether the given address is admin scoped.
*
* @param    addr @b{(input)}inetaddress
*
* @returns  L7_TRUE  adminscoped address.
* @returns  L7_FALSE not adminscoped address.
*
* @notes    
*       
* @end
*
*********************************************************************/
EXT_API L7_BOOL inetIsAddrMulticastScope(L7_inet_addr_t *addr);

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
EXT_API L7_int32  inetAddrCompareAddrWithMask(L7_inet_addr_t *addr1, L7_uint32 mask1,
                                      L7_inet_addr_t *addr2, L7_uint32 mask2);

EXT_API L7_int32  inetAddrCompareAddrWithMaskIndividual(L7_inet_addr_t *addr1, L7_uint32 mask1,
                                      L7_inet_addr_t *addr2, L7_uint32 mask2);
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
EXT_API L7_int32 inetAddrGetMulticastScopeId(L7_inet_addr_t *addr);

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
EXT_API L7_BOOL  inetAddrIsScopedInsideAnother(L7_inet_addr_t *addr1, L7_uint32 mask1,
                                       L7_inet_addr_t *addr2, L7_uint32 mask2);
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
EXT_API L7_uchar8* inetAddrPrint (L7_inet_addr_t *inetAddr, L7_uchar8 *buf);

/*********************************************************************
* @purpose  Get an inet6 address from inet address
*
* @param    inetAddr  @b{(input)} inetAddress
*	@param    inet6Addr @b{(output)} inet6 Addr
*                                  
* @returns   L7_FAILURE  
* @returns   L7_SUCCESS
*
* @notes    
*       
* @end
*
*********************************************************************/
EXT_API L7_RC_t inet6AddressGet(L7_inet_addr_t *inetAddr, L7_in6_addr_t *in6Addr);
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
EXT_API L7_BOOL inetAddrIsHostBitSet(L7_inet_addr_t *inetAddr, L7_inet_addr_t *inetMask);
/********************************************************************
* @purpose  check the class of given address for a given family
*
*
* @param   addrFamily @b{(input)}address Family
* @param   inetAddr   @b{(input)}InetAddress
*
* @returns   L7_TRUE ,if srcAddr has class A,B,C(except loop back)
* @returns   L7_FALSE, else case.
*
* @notes
*
* @end
********************************************************************/
EXT_API L7_RC_t inetIpAddressValidityCheck (L7_uchar8 addrFamily,L7_inet_addr_t *inetAddr);

