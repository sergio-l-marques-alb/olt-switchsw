/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename usmdb_util_api.h
*
* @purpose externs for USMDB layer
*
* @component unitmgr
*
* @comments tba
*
* @create 01/09/2001
*
* @author rjindal
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/


#ifndef USMDB_UTIL_API_H
#define USMDB_UTIL_API_H


#include "l7_common.h"
#include "comm_mask.h"
#include "l3_addrdefs.h"
#include "simapi.h"
#include "sim_exports.h"

/* Interface Characteristics  */

#define  USM_PHYSICAL_INTF          0x0001
#define  USM_STACK_INTF             0x0002
#define  USM_CPU_INTF               0x0004
#define  USM_LAG_INTF               0x0008
#define  USM_LAG_MEMBER_INTF        0x0010
#define  USM_LOGICAL_VLAN_INTF      0x0020
#define  USM_ROUTER_INTF            0x0040
#define  USM_MIRROR_SRC_INTF        0x0080
#define  USM_MIRROR_DST_INTF        0x0100
#define  USM_LOGICAL_LOOPBACK_INTF  0x0200
#define  USM_LOGICAL_TUNNEL_INTF    0x0400
#define  USM_LOGICAL_WIRELESS_INTF    0x0800
#define  USM_LOGICAL_L2_TUNNEL_INTF   0x1000
#define  USM_LOGICAL_VLAN_PORT_INTF   0x2000    /* PTin added: virtual ports */

/* USMDB protocol ID definition for PBVLANs */
#define L7_USMDB_PROTOCOL_IP      1          
#define L7_USMDB_PROTOCOL_ARP     2
#define L7_USMDB_PROTOCOL_IPX     3

#define L7_USMDB_PROTOCOL_HEX_IP      0x0800          
#define L7_USMDB_PROTOCOL_HEX_ARP     0x0806
#define L7_USMDB_PROTOCOL_HEX_IPX     0x8137


/*********************************************************************
*
* @purpose  Convert double64 integer into two unsigned integers
*
* @param    d64         @b{(input)}double64 value
* @param    *highend    @b{(output)}pointer to uint32 to recieve high order bits
* @param    *lowend     @b{(output)}pointer to uint32 to recieve low order bits
*
* @returns  L7_SUCCESS  always
*           
*
* @notes    if any recieving value pointer is NULL, it will skip that part
*
* @end
*********************************************************************/ 
extern L7_RC_t usmDbSplitDouble64(L7_double64 d64, L7_uint32 *highend, L7_uint32 *lowend);

/*********************************************************************
*
* @purpose  Convert two unsigned integers into a double64 integer 
*
* @param    d64        @b{(output)} pointer to a double64 value
* @param    *highend   @b{(input)}pointer to uint32 to recieve high order bits
* @param    *lowend    @b{(input)}pointer to uint32 to recieve low order bits
*
* @returns  L7_SUCCESS  always
*           
*
* @notes    if any recieving value pointer is NULL, it will skip that part
*
* @end
*********************************************************************/
extern L7_RC_t usmDbCreateDouble64(L7_uint32 highend, L7_uint32 lowend, L7_double64 *d64);

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
*
* @end
*
*********************************************************************/
extern L7_RC_t convertTo32BitUnsignedInteger(const L7_char8 *buf, L7_uint32 *pVal);

/*********************************************************************
*
* @purpose  Check to see if ip address / subnet mask pairs fall in the 
*           same subnet
*
* @param    ipaddr1     internet address 1
* @param    netmask1    netmask 1
* @param    ipaddr2     internet address 2
* @param    netmask2    netmask 2
*
* @returns  L7_SUCCESS  if ipaddr1/netmask1 and ipaddr2/netmask2 
*                       have different subnet
* @returns  L7_FAILURE  if ipaddr1/netmask1 and ipaddr2/netmask2 
*                       have the same subnet
*
* @notes    none
*
* @end
*********************************************************************/

extern L7_RC_t usmDbSubnetCompare(L7_uint32 ipaddr1, L7_uint32 netmask1, 
                                  L7_uint32 ipaddr2, L7_uint32 netmask2);

/*********************************************************************
*
* @purpose  Check to see if ip address / subnet mask / gateway triad
*           is valid
*
* @param    ipaddr      ip address
* @param    netMask     net mask
* @param    gateway     gateway
* @param    oldGateway  current default gateway
*
* @returns  L7_SUCCESS  if triad is valid
*
* @returns  L7_FAILURE  if triad is invalid
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbIpInfoValidate(L7_uint32 ipAddr, L7_uint32 netMask,
                            L7_uint32 gateway);

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
L7_BOOL usmDbNetmaskIsContiguous(L7_uint32 netMask);

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
L7_RC_t usmDbNetmaskValidate(L7_uint32 netMask);


/*********************************************************************
*
* @purpose Verify that a network mask is a valid (up to 32 bits) network mask for
*          an IP address assigned to an interface.
*
* @param   netMask @b{(input)} IPv4 network mask
*
* @returns L7_SUCCESS, if netMask is valid
* @returns L7_FAILURE, if netMask is invalid
*
* @notes   see RFC 1878. Verifies that the netmask is contiguous.
*          Does allow a 31 or 32 bit netmask.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbNetmaskValidate32(L7_uint32 netMask);


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
* @notes   If the netmask is not 0, it checks that the ipAddress is 
*          neither a network address, or a broadcast address. Also checks 
*          that the ipAddress is not restricted.
*
* @end
*
*********************************************************************/
L7_RC_t usmDbIpAddressValidate(L7_uint32 ipAddress, L7_uint32 netMask);

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
L7_RC_t usmDbNetworkAddressValidate(L7_uint32 ipAddress);

/*********************************************************************
*
* @purpose  Convert a 64 bit counter to a string  
*
* @param    num   64 bit value
* @param    *buf  dotted string representation
*
* @returns  L7_SUCCESS  or     
* @returns  L7_FAILURE  
*
* @notes    none
*
* @end
*********************************************************************/
extern L7_RC_t usmDb64BitsToString(L7_ulong64 num, L7_char8 *buf);

/*********************************************************************
*
* @purpose  Convert a 32 bit address to a string  
*
* @param    val         @b{(input)}internet address
* @param    *ipAddr     @b{(output)}pointer to dotted string reprensentation
*
* @returns  L7_SUCCESS      : Always 
*
* @comments Caller must provide an output buffer of at least
*           L7_IP_ADDR_STRING_LEN bytes.  The ipAddr must be
*           supplied in host byte order.
*
* @end
*********************************************************************/
extern L7_RC_t usmDbInetNtoa(L7_ulong32 val, L7_char8 *ipAddr);

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
extern L7_RC_t usmDbInetAddrIsValid(L7_uint32 address_family,
                                    L7_uchar8 *address);

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
extern L7_RC_t usmDbParseInetAddrFromStr(L7_uchar8 *address,
                                         L7_inet_addr_t *inet_address);
/*********************************************************************
*
* @purpose  Convert internet address string to a 32 bit integer
*
* @param    *ipAddr     @b{(input)}pointer to dotted string reprensentation
* @param    *ival       @b{(output)}pointer to 32 bit integer reprensentation
*
* @returns  1           success
*           0           failure, syntax, etc
*
* @notes    can use hex, octal or decimal input
*
* @end
*********************************************************************/
extern L7_RC_t usmDbInetAton(L7_uchar8 *ipAddr, L7_uint32 *val);

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
extern L7_RC_t usmDbDnsIpAddressValidate(L7_uchar8 *hostAddr, L7_uint32 *ival);

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
extern L7_RC_t usmDbParseInetAddrFromIPAddrHostNameStr(L7_uchar8 *address, 
                                       L7_inet_addr_t *inet_address);

/* utility functions */
/*********************************************************************
*
* @purpose  Returns the external interface number 
*           associated with the internal ifIndex
*
* @param intIfNum     L7_uint32 the internal interface number
* @param *ifIndex     L7_uint32 pointer to the external ifIndex
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbExtIfNumFromIntIfNum(L7_uint32 intIfNum, L7_uint32 *ifIndex);

/*********************************************************************
*
* @purpose  Returns the internal interface number associated with the external ifIndex
*           
*
* @param ifIndex    @b{(input)}the external ifIndex
* @param *intIfNum  @b{(output)}pointer to the internal interface number
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIntIfNumFromExtIfNum(L7_uint32 ifIndex, L7_uint32 *intIfNum);

/*********************************************************************
*
* @purpose  Returns the internal interface number associated with the Unit-Slot-Port
*           
*
* @param UnitIndex @b{(input)}L7_uint32 the unit for this operation
* @param SlotIndex @b{(input)}L7_uint32 the Slot number
* @param Index     @b{(input)}L7_uint32 the Port number
* @param *intIfNum @b{(output)}L7_uint32 pointer to the internal interface number
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIntIfNumFromUSPGet(L7_uint32 UnitIndex, L7_uint32 SlotIndex, 
                                       L7_uint32 Index, L7_uint32 *intIfNum);

/*********************************************************************
*
* @purpose  Obtain the next **external** interface number, or ifIndex
*
* @param    extIfNum      @b{(input)}external interface number
* @param    nextextIfNum  @b{(output)}pointer to next external interface number
*                       (@b{Returns: Next External Interface Number}).
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbGetNextExtIfNumber(L7_uint32 extIfNum, L7_uint32 *nextExtIfNum);

/*********************************************************************
* @purpose  Determine if this **external** interface number, or ifIndex exists
*           
*
* @param    L7_uint32     UnitIndex @b((input)) The unit for this operation
* @param    extIfNum      @b{(input)}external interface number
*
* @returns  L7_SUCCESS     if interface exists
* @returns  L7_ERROR       if interface does not exist
* @returns  L7_FAILURE     if other failure
*
* @notes    none 
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbExtIfNumberCheck(L7_uint32 UnitIndex, L7_uint32 extIfNum);

/*********************************************************************
*
* @purpose  Obtain the next **external** interface number, or ifIndex
*           to be displayed
*
* @param    extIfNum       external interface number
* @param    *nextextIfNum  pointer to next external interface number
*                       (@b{Returns: Next External Interface Number}).
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbGetNextVisibleExtIfNumber(L7_uint32 extIfNum, L7_uint32 *nextExtIfNum);

/*********************************************************************
* @purpose  Determine if this **external** interface number, or ifIndex
*           to be displayed exists
*
* @param    UnitIndex     unit number
* @param    extIfNum      external interface number
*
* @returns  L7_SUCCESS     if interface exists
* @returns  L7_FAILURE     if other failure
*
* @notes    none 
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbVisibleExtIfNumberCheck(L7_uint32 UnitIndex, L7_uint32 extIfNum);

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
extern L7_RC_t usmDbGetNextVisibleIntIfNumber(L7_uint32 intIfNum, L7_uint32 *nextIntIfNum);

/*********************************************************************
* @purpose Determine if this internal interface number
*          to be displayed exists.
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
extern L7_RC_t usmDbVisibleIntIfNumberCheck(L7_uint32 UnitIndex, L7_uint32 intIfNum);

/*********************************************************************
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
extern L7_RC_t usmDbGetNextPhysicalIntIfNumber(L7_uint32 intIfNum, L7_uint32 *nextIntIfNum);

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
extern L7_RC_t usmDbGetNextLagIntIfNumber(L7_uint32 intIfNum, L7_uint32 *nextIntIfNum);

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
extern L7_RC_t usmDbNextIntIfNumberByTypeGet(L7_INTF_TYPES_t sysIntfType, L7_uint32 intIfNum, L7_uint32 *nextIntIfNum);

/*********************************************************************
* @purpose Determine if this internal interface number exists and is
*          a physical interface
*
* @param L7_uint32 UnitIndex  unit number
* @param L7_uint32 intIfNum   physical internal interface number
*
* @returns L7_SUCCESS, if interface exists
* @returns L7_FAILURE, if other failure
*
* @notes    none 
*       
* @end
*
*********************************************************************/
extern L7_RC_t usmDbPhysicalIntIfNumberCheck(L7_uint32 UnitIndex, L7_uint32 intIfNum);

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
extern L7_RC_t usmDbNextPhysicalExtIfNumberGet(L7_uint32 UnitIndex, L7_uint32 extIfNum, L7_uint32 *nextExtIfNum);

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
extern L7_RC_t usmDbPhysicalExtIfNumberCheck(L7_uint32 UnitIndex, L7_uint32 extIfNum);

/*********************************************************************
*
* @purpose  Returns the Unit-Slot-Port associated with the internal interface number
*           
*
* @param    intIfNum    @b{(input)}L7_uint32 internal interface number
* @param   *UnitIndex   @b{(output)}L7_uint32 pointer to the unit number
* @param   *SlotIndex   @b{(output)}L7_uint32 pointer to the slot number
* @param   *Index       @b{(output)}L7_uint32 pointer to the port
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbUnitSlotPortGet(L7_uint32 intIfNum, L7_uint32 *UnitIndex, 
                                    L7_uint32 *SlotIndex, L7_uint32 *Index);

/*********************************************************************
*
* @purpose  Return Internal Interface Number of the first valid port
*
* @param    *intIfNum   @b{(output)}L7_uint32 pointer to the first valid Internal Interface Number
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbValidIntIfNumFirstGet(L7_uint32 *val);

/*********************************************************************
*
* @purpose  Return Internal Interface Number of the next valid port
*
* @param  interface   @b{(input)}L7_uint32 Internal Interface Number
* @param  *intIfNum   @b{(output)}L7_uint32 pointer to the next valid Internal Interface Number
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbValidIntIfNumNext(L7_uint32 interface, L7_uint32 *intIfNum);

/*********************************************************************
*
* @purpose  Returns the internal interface number associated with the Trunk ifIndex
*           
*
* @param UnitIndex    @b{(input)}L7_uint32 the unit for this operation
* @param TrunkIfIndex @b{(input)}L7_uint32 the ifIndex for the trunk
* @param *intfNum     @b{(output)}L7_uint32 pointer to the interface number
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_EXISTS, if interface does not exist
* @returns  L7_FAILURE, if other failure
*
* @notes none
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIntIfNumFromTrunkGet(L7_uint32 UnitIndex, L7_uint32 TrunkIfIndex, L7_uint32 *intIfNum);

/*********************************************************************
* @purpose  Determine if an interface in question is to be displayed.
*
* @param    UnitIndex   unit number
* @param    intIfNum    interface number
* @param    *retVal     return value
*
* @returns  L7_SUCCESS     if interface can be displayed
* @returns  L7_FAILURE     if interface is not to be displayed
*
* @notes    none 
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbVisibleInterfaceCheck(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_int32 *retVal);

/*********************************************************************
* @purpose  Checks if a MAC address value is all zeros.
*
* @param    pMac        ptr to Ethernet MAC address
*
* @returns  L7_TRUE     MAC address is all zeros
* @returns  L7_FALSE    MAC address is non-zero
*
* @notes    Checks byte by byte to avoid any alignment restrictions
*           on certain CPUs.
*       
* @end
*********************************************************************/
extern L7_BOOL usmDbIsMacZero(L7_uchar8 *pMac);


/*********************************************************************
* @purpose  Retrieves the current link up/down status of the service port 
*
* @param    none
*
* @returns  L7_UP
* @returns  L7_DOWN
*
* @end
*********************************************************************/
extern L7_uint32 usmDbServicePortLinkStatusGet( L7_uint32 unit );

/*********************************************************************
* @purpose  Retrieves the current link up/down status of the serial port 
*
* @param    none
*
* @returns  L7_UP
* @returns  L7_DOWN
*
* @end
*********************************************************************/
extern L7_uint32 usmDbSerialStatusGet( L7_uint32 unit );

/*********************************************************************
* @purpose  Check to see if a component is present.
*
* @param    none
*
* @returns  L7_TRUE or L7_FALSE
*
* @notes    none 
*       
* @end
*********************************************************************/
extern L7_BOOL usmDbComponentPresentCheck(L7_uint32 UnitIndex,L7_COMPONENT_IDS_t componentId);

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
extern L7_BOOL usmDbComponentNotPresentCheck(L7_uint32 UnitIndex,L7_COMPONENT_IDS_t  componentId);
/*********************************************************************
*
* @purpose Check to see if a component is present.
*
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
L7_BOOL usmDbFeaturePresentCheck(L7_uint32 UnitIndex,L7_COMPONENT_IDS_t componentId, L7_uint32 featureId);

/*********************************************************************
*
* @purpose Check to see if a component is not present.
*
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
L7_BOOL usmDbFeatureNotPresentCheck(L7_uint32 UnitIndex,L7_COMPONENT_IDS_t componentId, L7_uint32 featureId);


/*********************************************************************
* @purpose  Re-init bootp protocol.
*
* @param    none
*
* @returns  void
*
* @notes    none 
*       
* @end
*********************************************************************/
extern void usmDbBootpTaskReInit(L7_uint32 UnitIndex);

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
extern L7_uint32 usmDbIntIfNumTypeCountGet(L7_uint32 unit, L7_uint32 inclintIfType, L7_uint32 exclintIfType);

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
extern L7_RC_t usmDbIntIfNumTypeMaskGet(L7_uint32 unit, L7_uint32 intIfNum, L7_uint32 *maskIntfType);

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
extern L7_RC_t usmDbIntIfNumTypeFirstGet(L7_uint32 unit, L7_uint32 inclintIfType, L7_uint32 exclintIfType, L7_uint32 *intIfNum);

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
extern L7_RC_t usmDbIntIfNumTypeNextGet(L7_uint32 unit, L7_uint32 inclintIfType, L7_uint32 exclintIfType, L7_uint32 interface, L7_uint32 *intIfNum);

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
extern L7_RC_t usmDbExtIfNumTypeCheckValid(L7_uint32 unit, L7_uint32 inclintIfType, L7_uint32 exclintIfType, L7_uint32 extIfNum);

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
L7_RC_t usmDbIntIfNumTypeCheckValid(L7_uint32 unit, L7_uint32 inclintIfType, L7_uint32 exclintIfType, L7_uint32 intIfNum);

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
extern L7_RC_t usmDbExtIfNumTypeNextGet(L7_uint32 unit, L7_uint32 inclintIfType, L7_uint32 exclintIfType, L7_uint32 extIfNum, L7_uint32 *nextExtIfNum);

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
extern L7_RC_t usmDbStringAlphaNumericCheck(const L7_char8 str[]);

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
extern L7_RC_t usmDbStringNumericCheck(const L7_char8 str[]);

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
L7_RC_t usmDbStringHexadecimalCheck(const L7_char8 str[]);

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
extern L7_RC_t usmDbStringAlphaNumericPlusCheck(const L7_char8 str[], const L7_char8 include[], const L7_char8 exclude[]);

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
extern L7_RC_t usmDbStringPrintableCheck(const L7_char8 str[]);

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
extern L7_RC_t usmDbStringCaseInsensitiveCompare(const L7_char8 str1[], const L7_char8 str2[]);

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
extern L7_RC_t usmDbPbVlanPrtlDecimalToHex(L7_uint32 prtl, L7_uint32 *val);

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
extern L7_RC_t usmDbPbVlanPrtlHexToDecimal(L7_uint32 prtl, L7_uint32 *val);

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
extern L7_RC_t usmdbPortListGet(L7_uchar8 *buf, L7_uint32 *list);

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
extern L7_RC_t usmDbConvertListToMask(L7_uint32 *list, L7_INTF_MASK_t *mask);

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
extern L7_RC_t usmDbConvertMaskToList(L7_INTF_MASK_t *mask, L7_uint32 *list, L7_uint32 *numPorts);

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
extern L7_RC_t usmDbReverseMask(L7_INTF_MASK_t maskIn, L7_INTF_MASK_t *maskOut);

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
extern L7_RC_t usmDbEntryVidMacCombine(L7_uint32 vlanId, L7_uchar8 *mac, 
                                       L7_uchar8 *vidMac);

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
extern L7_RC_t usmDbEntryVidMacSeparate(L7_uchar8 *vidMac, 
                                        L7_uint32 *vlanId, L7_uchar8 *mac);



/*********************************************************************
*
* @purpose Convert hexadecimal value to binary string.
*
* @param L7_uchar8* inputData       Input value to be converted
* @param L7_uchar8* binaryString    <Input/Output> Binary String
* @param L7_unit32* BinLength       <Input/Output> Length of the binary string
*
* @returns L7_SUCCESS
*          L7_FAILURE
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbHexToBin(L7_char8* inputData, L7_uchar8 * binaryString, L7_uint32 * pBinLength);


/*********************************************************************
*
* @purpose Convert hexadecimal string to byte value.
*
* @param L7_uchar8*  inputStr       2-character buffer
* @param L7_uchar8*  outputBuf      Byte value for the buff string
*
* @returns L7_SUCCESS
*
* @end
*
*********************************************************************/
L7_RC_t usmDbConvertHexToByte(L7_char8* inputStr, L7_uchar8 *outputBuf);


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
L7_RC_t usmDbConvertHex(L7_char8 *buf, L7_uchar8 *hexString);

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
L7_BOOL usmDbConvertTwoDigitHex(L7_uchar8 *buf, L7_uchar8 *twoDigitHex);

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
L7_BOOL usmDbMacAddrStringToHex(L7_uchar8 *buf, L7_uchar8 *mac);

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
extern L7_RC_t usmDbConvertXstrtoi(char *hex, L7_uint32 *dec);

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
L7_uint32 usmDbConvertHextoDec(char *hex, L7_uint32 l);

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
char usmDbConvertXtoD(char c);

/*********************************************************************
*
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
L7_BOOL usmDbMacAddrHexToString(L7_uchar8 *mac, L7_uchar8 *macBuf);

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
* @notes
* 
* @end
*
*********************************************************************/
L7_BOOL usmDbTimeToStringConvert(L7_uint32 time, L7_char8 *timeStr,
                            L7_uint32 timeStrLen);

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
* @notes
* 
* @end
*
*********************************************************************/
L7_BOOL usmDbTimeToShortStringConvert(L7_uint32 time, L7_char8 *timeStr,
                            L7_uint32 timeStrLen);


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
L7_RC_t usmDbConvertTimeTicksToDaysHoursMinutesSeconds(L7_uint32 *timeTicks, L7_char8 *buf);											
							
							
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
L7_char8 * usmDbConvertTimeToDateString(L7_uint32 now);

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
                                L7_uint32 *prefixLen);

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
L7_RC_t usmDbIpv6MapMaskApplyGet(L7_in6_addr_t *prefix, L7_uint32 plen);


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
L7_RC_t usmdbCpuUtilizationStringGet(L7_uint32 UnitIndex, L7_char8 *buf, L7_uint32 bufSize);

/*********************************************************************
* @purpose  Set the CPU Free memory threshold for monitoring
*
* @param    unitIndex {(input)} UnitIndex unit number
* @param    threshold {(input)} Free memory threshold in KB. A value of 0 
*                               indicates that threshold monitoring should be
*                               stopped.
*
* @returns L7_SUCCESS
*          L7_FAILURE
*
* @notes none
*
* @end
*********************************************************************/
L7_RC_t usmdbCpuFreeMemoryThresholdSet(L7_uint32 UnitIndex, 
                                       L7_uint32 threshold);

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
                                       L7_uint32 *threshold);


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
                                    L7_uint32 paramVal);


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
                                    L7_uint32 *paramVal);

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
L7_RC_t usmDbHostNameValidate(L7_uchar8 *hostName);

/*********************************************************************
*
* @purpose Used to validate form and syntax of a hostname 
* 
* @param    hostName  @b((input)) host Name to validate.
*                     
* @returns L7_SUCCESS for success
*          L7_FAILURE for Failure 
*
* @notes
*     When hostname is entered with in double quotes("")
*     usmDbHostNameValidate() returns failure but 
*     usmDbHostNameValidateWithSpace() accepts spaces in hostname
*     and checks the following conditions.
*       1.Returns fail, If the space is in first or last position.  
*       2.Returns fail, If consecutive spaces are found     
* @end
*
********************************************************************/
L7_RC_t usmDbHostNameValidateWithSpace(L7_uchar8 *hostName);


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
                                   L7_IP_ADDRESS_TYPE_t *addrType);
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
L7_RC_t usmdbEraseStartupConfig();	
/*********************************************************************
* @purpose   Get the ipv4 Mask from prefix len
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
L7_RC_t usmDbIpSubnetMaskGet(L7_uint32 *mask, L7_uint32 plen);

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
                                   L7_IP_ADDRESS_TYPE_t *addrType);

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
L7_BOOL usmDbOUIAddrStringToHex(L7_uchar8 *buf, L7_uchar8 *oui);


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
L7_BOOL usmDbIsNotFeatMetroCpeV10Check();
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
L7_BOOL usmDbIsFeatMetroCpeV10Check();
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

L7_BOOL usmDbSocIs53115Check(L7_uint32 UnitIndex);

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
L7_BOOL usmDbSocIsnot53115Check(L7_uint32 UnitIndex);

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
L7_BOOL usmDbPoEFeaturePresentCheck();


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
L7_RC_t usmDbBannerSet(L7_uint32 UnitIndex, L7_char8 *buffer);

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
L7_RC_t usmDbBannerGet(L7_uint32 UnitIndex, L7_char8 *buffer);

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
L7_RC_t usmDbBannerAckSet(L7_uint32 UnitIndex, L7_BOOL val);

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
L7_RC_t usmDbBannerAckGet(L7_uint32 UnitIndex, L7_BOOL *val);
#endif
/*********************************************************************
*
* @purpose This function converts emweb characters to conrrespond
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
L7_RC_t usmDbConvertStringToEmwebString(L7_uint32 UnitIndex, L7_char8 *buffer, L7_char8 *emwebBuffer);

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
L7_BOOL usmWebConvertEtypeCustomValue(const L7_uchar8 *buf, L7_uint32 *val);
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
L7_RC_t usmDbImageVersionToString(codeVersion_t code_version, L7_char8 *version);

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
L7_RC_t usmDbImageStringToVersion(L7_char8 *version, codeVersion_t* code_version);

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
L7_BOOL usmDbDimFeaturePresentCheck(void);

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
L7_BOOL usmDbDimFeatureNotPresentCheck(void);

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
L7_RC_t usmDbUserMgrStrNoCaseReverseCompareCheck(L7_char8 *dstString, L7_char8 *srcString);  


#endif

