/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   usmdb_1213_api.h
*
* @purpose    externs for USMDB layer
*
* @component  unitmgr
*
* @comments
*
* @create     01/10/2001
*
* @author     rjindal
*
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/

  
#ifndef _USMDB_1213_API_H_
#define _USMDB_1213_API_H_
#include "usmdb_common.h"

/*********************************************************************
* @purpose Gets the IP Router Admin Mode
*
* @param UnitIndex  @b{(input)} the unit for this operation
* @param val        @b{(output)} L7_ENABLE, L7_DISABLE
*
* @returns L7_SUCCESS
* @returns L7_FAIlURE
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIpRtrAdminModeGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose Set the IP Router Admin Mode
*
* @param UnitIndex  @b{(input)} the unit for this operation
* @param val        @b{(input)} L7_ENABLE, L7_DISABLE
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
extern L7_RC_t usmDbIpRtrAdminModeSet(L7_uint32 UnitIndex, L7_uint32 val);

/*********************************************************************
* @purpose  Gets the System Description
*          
* @param    UnitIndex   @b{(input)} the unit for this operation
* @param    *buf        @b{(ouput)} placeSystem Description
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  This return code is not used currently
*
* @comments A textual description of the entity. This value should
*           include the full name and version identification of
*           the system's hardware type, software operating-system,
*           and networking software. It is mandatory that this only
*           contains printable ASCII characters.
*
* @end
*********************************************************************/
extern L7_RC_t usmDb1213SysDescrGet(L7_uint32 UnitIndex, L7_char8 *buf);

/*********************************************************************
* @purpose  Gets the identification of the network management subsystem
*          
* @param    UnitIndex    @b{(input)} the unit for this operation
* @param    *buf         @b{(output)} Network Management Subsystem Identification
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  This return code is not used currently
*
* @comments The vendor's authoritative identification of the network
*           management subsystem contained in the entity. This value
*           is allocated within the SMI enterprises subtree 
*           {1.3.6.1.4.1} and provides an easy and unambiguous means for
*           determining 'what kind of box' is being managed. For example
*           if vendor 'Flintstones, Inc.' was assigned the subtree
*           1.3.6.1.4.1.4242, it could assign the identifier 
*           1.3.6.1.4.1.4242.1.1 to its 'Fred Router'
*
* @end
*********************************************************************/
extern L7_RC_t usmDb1213SysObjectIDGet(L7_uint32 UnitIndex, L7_char8 *buf);

/**************************************************************************
* @purpose  Get the number of seconds since last reset and return the time
*           in number of days, hours, minutes, and seconds.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *ts        @b{(output)} ptr to L7_timespec structure
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  This return code is not used currently
*
* @comments
*
* @end
*************************************************************************/
extern L7_RC_t usmDb1213SysUpTimeGet(L7_uint32 UnitIndex, usmDbTimeSpec_t *ts);

/**************************************************************************
* @purpose  Get the number of seconds since last reset and return the time
*          in hundredths of a second
*
* @param    UnitIndex   @b{(input)} the unit for this operation
* @param    *sysUpTime  @b{(output)} ptr to variable to receive time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  This return code is not used currently
*
* @comments
*
* @end
*************************************************************************/
extern L7_RC_t usmDb1213SysUpTimeRawGet(L7_uint32 UnitIndex, L7_uint32 *sysUpTime);

/*********************************************************************
* @purpose  Gets the System Contact
*          
* @param    UnitIndex    @b{(input)} the unit for this operation
* @param    *sysContact  @b{(output)} System Contact
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  This return code is not used currently
*
* @comments The textual identification of the contact person for this
*           managed node, together with information on how to contact
*           this person.
*           Max length is L7_SYSCONTACT_SIZE
*        
* @end
*********************************************************************/
extern L7_RC_t usmDb1213SysContactGet(L7_uint32 UnitIndex, L7_char8 *sysContact);

/*********************************************************************
* @purpose  Set the System Contact
*          
* @param    UnitIndex    @b{(input)} the unit for this operation
* @param    *sysContact  @b{(input)} System Contact
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE
*
* @comments The textual identification of the contact person for this
*           managed node, together with information on how to contact
*           this person.
*           Max length is L7_SYS_SIZE-1
*        
* @end
*********************************************************************/
extern L7_RC_t usmDb1213SysContactSet(L7_uint32 UnitIndex, L7_char8 *sysContact);

/*********************************************************************
* @purpose  Gets the System Name
*          
* @param    UnitIndex   @b{(input)} the unit for this operation
* @param    *sysName    @b{(output)} System Name  
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  This return code is not used currently
*
* @comments An administratively-assigned name for this managed node.
*           By convention, this is the node's fully-qualified domain
*           name.
*           Max length is L7_SYSNAME_SIZE
*
* @end
*********************************************************************/
extern L7_RC_t usmDb1213SysNameGet(L7_uint32 UnitIndex, L7_char8 *sysName);

/*********************************************************************
* @purpose  Set the System Name
*          
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *sysName   @b{(input)} System Name  
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments An administratively-assigned name for this managed node.
*           By convention, this is the node's fully-qualified domain
*           name.
*           Max length is L7_SYS_SIZE-1
*        
* @end
*********************************************************************/
extern L7_RC_t usmDb1213SysNameSet(L7_uint32 UnitIndex, L7_char8 *sysName);

/*********************************************************************
* @purpose  Gets the System Location
*          
* @param    UnitIndex     @b{(input)} the unit for this operation
* @param    *sysLocation  @b{(output)} System Location  
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  This return code is not used currently
*
* @comments The physical location of this node (e.g. 'telephone closet,
*           3rd floor')
*           Max length is L7_SYSLOCATION_SIZE
*        
* @end
*********************************************************************/
extern L7_RC_t usmDb1213SysLocationGet(L7_uint32 UnitIndex, L7_char8 *sysLocation);

/*********************************************************************
* @purpose  Set the System Location
*          
* @param    UnitIndex     @b{(input)} the unit for this operation
* @param    *sysLocation  @b{(input)} System Location  
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE
*
* @comments The physical location of this node (e.g. 'telephone closet,
*           3rd floor')
*           Max length is L7_SYS_SIZE-1
*        
* @end
*********************************************************************/
extern L7_RC_t usmDb1213SysLocationSet(L7_uint32 UnitIndex, L7_char8 *sysLocation);

/*********************************************************************
* @purpose  Gets the number of services associated with this entity
*          
* @param    UnitIndex   @b{(input)} the unit for this operation
* @param    *val        @b{(output)} Number of services  
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  This return code is not used currently
*
* @comments A value which indicates the set of services that this
*           entity primarily offers.
*
*           The value is a sum. This sum initially takes the value
*           zero, then for each layer, L, in the range 1 through 7, 
*           that this node performs transactions for, 2 raised to
*           (L-1) is added to the sum. For example, a node which
*           performs primarily routing functions would have a value
*           of 4 (2^(3-1)). In contrast, a node which is a host
*           offering application services would have a value of 72
*           (2^(4-1) + 2^(7-1)). Note that in the context of the
*           Internet suite of protocols, values should be calculated
*           accordingly:
*
*           layer functionality:
*             1 - physical (e.g. repeaters)
*             2 - datalink/ subnetwork (e.g. bridges)
*             3 - internet (e.g. IP gateways)
*             4 - end-to-end (e.g. IP hosts)
*             7 - applications (e.g. mail relays)
*
*           For systems including OSI protocols, layer 5 and 6 may
*           also be counted.
*        
* @end
*********************************************************************/
extern L7_RC_t usmDb1213SysServicesGet(L7_uint32 UnitIndex, L7_int32 *val);

/*********************************************************************
* @purpose  Gets the (Layer 2) max frame size of the specified interface
*
* @param    UnitIndex       @b{(input)}  the unit for this operation
* @param    intIfNum        @b{(input)}  Internal Interface Number 
* @param    *val            @b{(output)} pointer to Max Frame Size
*
* @return   L7_SUCCESS  if success
*           L7_ERROR    if interface does not exist
*           L7_FAILURE
*
* @comments The size of the largest datagram which can be sent/ received
*           on the interface, specified in octets. For interfaces that
*           are used for transmitting network datagrams, this is the
*           size of the largest network datagram that can be sent on
*           the interface.
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbIfMtuGet(L7_uint32 UnitIndex, L7_uint32 interface, L7_uint32 *val);

/*********************************************************************
* @purpose  Get the Default Time to Live
*          
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *val       @b{(output)} Time to live
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIpDefaultTTLGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  Set the Default Time to Live
*          
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    val        @b{(input)} Time to live
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIpDefaultTTLSet(L7_uint32 UnitIndex, L7_uint32 val);

/*********************************************************************
* @purpose  Get the total number of input datagrams received from interfaces
*          
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *val       @b{(output)} Number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments includes errors received
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIpInReceivesGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  Get the total number of input datagrams discarded due to headers
*          
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *val       @b{(output)} Number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments The number of input datagrams discarded due to errors
*           in their IP headers, including bad checksums, version
*           number mismatch, other format errors, time-to-live
*           exceeded, errors discovered in processing their IP
*           options etc.
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIpInHdrErrorsGet(L7_uint32 Unitindex, L7_uint32 *val);

/*********************************************************************
* @purpose  Gets the number of input datagrams discarded because
*           the IP address in their IP header's destination
*           field was not a valid address to be received at
*           this entity.
*          
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *val       @b{(output)} Number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments   
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIpInAddrErrorsGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  Gets the number of input datagrams for which this
*           entity was not their final IP destination, as a
*           result of which an attempt was made to find a
*           route to forward them to that final destination.
*          
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *val       @b{(output)} Number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments   
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIpForwDatagramsGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  Gets the number of locally-addressed datagrams
*           received successfully but discarded because of an
*           unknown or unsupported protocol.
*          
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *val       @b{(output)} Number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments   
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIpInUnknownProtosGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  Gets the number of input IP datagrams for which no
*           problems were encountered to prevent their
*           continued processing, but which were discarded
*                     
*          
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *val       @b{(output)} Number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments   
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIpInDiscards(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  Gets the total number of input datagrams successfully
*           delivered to IP user-protocols (including ICMP).
*          
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *val       @b{(output)} Number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments   
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIpInDelivers(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  Gets the total number of IP datagrams which local IP
*           user-protocols (including ICMP) supplied to IP in
*           requests for transmission.  
*          
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *val       @b{(output)} Number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments Note that this counter
*           does not include any datagrams counted in
*           ipForwDatagrams
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIpOutRequests(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  Gets the number of output IP datagrams for which no
*           problem was encountered to prevent their
*           transmission to their destination, but which were
*           discarded (e.g., for lack of buffer space).  
*          
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *val       @b{(output)} Number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments   
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIpOutDiscardsGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  Gets the number of IP datagrams discarded because no
*           route could be found to transmit them to their
*           destination.  
*          
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *val       @b{(output)} Number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments   
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIpNoRoutesGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  Gets the maximum number of seconds which received
*           fragments are held while they are awaiting
*           reassembly at this entity
*          
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *val       @b{(output)} Number of seconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments   
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIpReasmTimeoutGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  The number of IP fragments received which needed to be
*           reassembled at this entity
*          
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *val       @b{(output)} Number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments   
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIpReasmReqdsGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  Gets the number of IP datagrams successfully re-assembled
*          
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *val       @b{(output)} Number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments   
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIpReasmOKsGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  Gets the number of failures detected by the IP re-
*           assembly algorithm (for whatever reason: timed
*           out, errors, etc).  
*          
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *val       @b{(output)} Number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments   
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIpReasmFailsGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  Gets the number of IP datagrams that have been
*           successfully fragmented at this entity
*          
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *val       @b{(output)} Number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments   
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIpFragOKsGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  Gets the number of IP datagrams that have been
*           discarded because they needed to be fragmented at
*           this entity but could not be, e.g., because their
*           Don't Fragment flag was set.
*          
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *val       @b{(output)} Number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments   
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIpFragFailsGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  Gets the number of IP datagram fragments that have
*           been generated as a result of fragmentation at
*           this entity.
*          
* @param    UnitIndex  @b{(input)}  the unit for this operation
* @param    *val       @b{(output)} Number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments   
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIpFragCreatesGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  Determines if the Ip Address entry corresponding to 
*           this ip address exists
*          
*          
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    ipAddr     @b{(input)} The ip address to which this entry's
*                               addressing information pertains.
* @param    intIfNum   @b{(input)} Internal interface number for entry
*
* @returns  L7_SUCCESS  if the specified address entry exists
* @returns  L7_FAILURE  if the specified address entry does not exist
*
* @comments Each entry contains addressing information for one of
*           this entity's IP addresses. 
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIpAdEntAddrGet(L7_uint32 UnitIndex, L7_uint32 ipAddr, L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Get the Ip Address entry which is next to the entry corresponding 
*           to this ip address entry
*          
*          
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *ipAddr    @b{(input)} The ip address to which this entry's
*                               addressing information pertains. @line
*                      @b{(output)} The ip address of the next entry
* @param    *intIfNum  @b{(input)} Internal interface number for entry
*                      @b{(output)} Internal interface of next entry
*
* @returns  L7_SUCCESS  if the next route entry exists
* @returns  L7_FAILURE  if the next route entry does not exist
*
* @comments Each entry contains addressing information for one of
*           this entity's IP addresses. 
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIpAdEntAddrGetNext(L7_uint32 UnitIndex, L7_uint32* ipAddr, L7_uint32 *intIfNum);

/*********************************************************************
* @purpose  Gets the index value which uniquely identifies the
*           interface to which this entry is applicable
*          
* @param    UnitIndex  @b{(input)} The unit for this operation
* @param    intIfNum   @b{(input)} The internal interface associated with
*                               this entry
* @param    *ifIndex   @b{(output)} The ifindex of this entry
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The ifIndex is the index value which uniquely identifies
*           the interface to which this entry is applicable. The interface
*           identified by a particular value of this index is the same
*           interface as identified by the same value of ifIndex.
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIpEntIfIndexGet(L7_uint32 UnitIndex, L7_uint32 ipAddr,
                                    L7_uint32 *ifIndex);

/*********************************************************************
* @purpose  Gets the subnet mask associated with the IP address of
*           this entry.  
*
* @param    UnitIndex  @b{(input)} The unit for this operation
* @param    intIfNum   @b{(input)} The internal interface associated with
*                               this entry
* @param    *netmask   @b{(output)} The subnet mask associated with the
*                                ip address of this entry. 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The value of the mask is an IP
*           address with all the network bits set to 1 and all
*           the hosts bits set to 0."  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIpAdEntNetMaskGet (L7_uint32 UnitIndex, L7_uint32 ipAddr,
                                       L7_uint32 *netmask);

/*********************************************************************
* @purpose  Gets the value of the least-significant bit in the IP
*           broadcast address used for sending datagrams on
*           the (logical) interface associated with the IP
*           address of this entry
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    intIfNum   @b{(input)} The internal interface associated with
*                               this entry
* @param    *bcastAddr @b{(output)} bcast address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The standard specifies that any host id consisting of all
*           1s is reserved for broadcast. 
*           bcast addr = (ipaddr & netmask) | ~netmask 
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIpAdEntBCastGet (L7_uint32 UnitIndex, L7_uint32 ipAddr,
                                     L7_uint32 *bcastAddr);

/*********************************************************************
* @purpose  Gets the size of the largest IP datagram which this
*           entity can re-assemble from incoming IP fragmented
*           datagrams received on this interface
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    ipAddr     @b{(input)} the ip address associated with this
*                               entry.
* @param    *maxSize   @b{(output)} size of largest re-assembled datagram
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE   This return code is not used currently
*
* @comments As the OS performs reassembly this value will be retrieved
*           from the OS.
*           For Linux: The value will be obtained from the file
*           /proc/sys/net/ipv4/ipfrag_high_thresh
*           For VxWorks: The value will be obtained from the mib
*           using the call m2IpAddrTblEntryGet()
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIpAdEntReasmMaxSizeGet(L7_uint32 UnitIndex, L7_uint32 ipAddr,
                                           L7_uint32 *maxSize);
                       
/*********************************************************************
* @purpose  Determines if a route entry exists for the specified route
*           destination.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    routeDest  @b{(input)} Route destination for this entry.
*
* @returns  L7_SUCCESS If the corresponding entry exists
* @returns  L7_FAILURE If the corresponding entry does not exist
*
* @comments   
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIpRouteEntryGet(L7_uint32 UnitIndex, L7_uint32 routeDest);

/*********************************************************************
* @purpose  Determines if the next route entry exists for the specified route
*           destination.
*
* @param    UnitIndex  @b{(input)}  the unit for this operation
* @param    *routeDest @b{(input)}  Route destination for this entry. @line
*                      @b{(output)} Route destination of next entry.
*
* @returns  L7_SUCCESS If the corresponding entry exists
* @returns  L7_FAILURE If the corresponding entry does not exist
*
* @comments   
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIpRouteEntryGetNext(L7_uint32 UnitIndex, L7_uint32 *routeDest);

/*********************************************************************
* @purpose  Gets the index value which uniquely identifies the
*           local interface through which the next hop of this
*           route should be reached.  
*
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    routeDest  @b{(input)} route destination for this entry
* @param    *intIfNum  @b{(output)} internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments   
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIpRouteIfIndexGet(L7_uint32 UnitIndex, L7_uint32 routeDest,
                                      L7_uint32 *intIfNum);

/*********************************************************************
* @purpose  Gets the primary routing metric for this route  
*
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    routeDest  @b{(input)} route destination for this entry
* @param    *metric1   @b{(output)} route metric 1
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The semantics of this metric are determined by the
*           routing-protocol specified in the route's
*           ipRouteProto value.  If this metric is not used,
*           its value should be set to -1.  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIpRouteMetric1Get(L7_uint32 UnitIndex, L7_uint32 routeDest,
                                      L7_uint32 *metric1);

/*********************************************************************
* @purpose  Sets the primary routing metric for this route  
*
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    routeDest  @b{(input)} route destination for this entry
* @param    metric1    @b{(input)} route metric 1
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_NOT_SUPPORTED
*
* @comments The semantics of this metric are determined by the
*           routing-protocol specified in the route's
*           ipRouteProto value.  If this metric is not used,
*           its value should be set to -1.  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIpRouteMetric1Set(L7_uint32 UnitIndex, L7_uint32 routeDest,
                                      L7_uint32 metric1);

/*********************************************************************
* @purpose  Gets the alternate routing metric for this route  
*
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    routeDest  @b{(input)} route destination for this entry
* @param    *metric2   @b{(output)} Route metric 2
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The semantics of this metric are determined by the
*           routing-protocol specified in the route's
*           ipRouteProto value.  If this metric is not used,
*           its value should be set to -1.  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIpRouteMetric2Get(L7_uint32 UnitIndex, L7_uint32 routDest,
                                      L7_uint32 *metric2);

/*********************************************************************
* @purpose  Sets the alternate routing metric for this route  
*
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    routeDest  @b{(input)} route destination for this entry
* @param    metric2    @b{(input)} Route
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_NOT_SUPPORTED
*
* @comments The semantics of this metric are determined by the
*           routing-protocol specified in the route's
*           ipRouteProto value.  If this metric is not used,
*           its value should be set to -1.  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIpRouteMetric2Set(L7_uint32 UnitIndex, L7_uint32 routeDest,
                                      L7_uint32 metric2);

/*********************************************************************
* @purpose  Gets the alternate routing metric for this route  
*
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    routeDest  @b{(input)} route destination for this entry
* @param    *metric3   @b{(output)} route metric 3
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The semantics of this metric are determined by the
*           routing-protocol specified in the route's
*           ipRouteProto value.  If this metric is not used,
*           its value should be set to -1.  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIpRouteMetric3Get(L7_uint32 UnitIndex, L7_uint32 routeDest,
                                      L7_uint32 *metric3);

/*********************************************************************
* @purpose  Sets the alternate routing metric for this route  
*
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    routeDest  @b{(input)} route destination for this entry
* @param    metric3    @b{(input)} Route metric 3
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_NOT_SUPPORTED
*
* @comments The semantics of this metric are determined by the
*           routing-protocol specified in the route's
*           ipRouteProto value.  If this metric is not used,
*           its value should be set to -1.  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIpRouteMetric3Set(L7_uint32 UnitIndex, L7_uint32 routeDest,
                                      L7_uint32 metric3);

/*********************************************************************
* @purpose  Gets the alternate routing metric for this route  
*
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    routeDest  @b{(input)} route destination for this entry
* @param    *metric4   @b{(output)} route metric 4
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The semantics of this metric are determined by the
*           routing-protocol specified in the route's
*           ipRouteProto value.  If this metric is not used,
*           its value should be set to -1.  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIpRouteMetric4Get(L7_uint32 UnitIndex, L7_uint32 routeDest,
                                      L7_uint32 *metric4);

/*********************************************************************
* @purpose  Sets the alternate routing metric for this route  
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    intIfNum   @b{(input)} the internal interface number associated
*                               with this entry
* @param    metric4    @b{(input)} route metric 4
*
* @returns  L7_SUCCESS
* @returns  L7_FAIlURE
* @returns  L7_NOT_SUPPORTED
*
* @comments The semantics of this metric are determined by the
*           routing-protocol specified in the route's
*           ipRouteProto value.  If this metric is not used,
*           its value should be set to -1.  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIpRouteMetric4Set(L7_uint32 UnitIndex, L7_uint32 routeDest,
                                      L7_uint32 *metric4);

/*********************************************************************
* @purpose  Gets the IP address of the next hop of this route  
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    routeDest  @b{(input)} route destination for this entry
* @param    *nextHop   @b{(output)} ip address of the next hop of this
*                                route entry
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIpRouteNextHopGet(L7_uint32 UnitIndex, L7_uint32 routeDest,
                                      L7_uint32 *nextHop);

/*********************************************************************
* @purpose  Sets the IP address of the next hop of this route  
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    routeDest  @b{(input)} route destination for this entry
* @param    nextHop    @b{(input)} ip address of the next hop of this
*                                route entry
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_NOT_SUPPORTED
*
* @comments  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIpRouteNextHopSet(L7_uint32 UnitIndex, L7_uint32 routeDest,
                                      L7_uint32 nextHop);

/*********************************************************************
* @purpose  Gets the type of route.   
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    routeDest  @b{(input)} route destination for this entry
* @param    *routeType @b{(output)} The type of route.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Note that the values direct(3) and indirect(4) refer to 
*           the notion of direct and indirect routing in the IP
*           architecture.  
*
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIpRouteTypeGet(L7_uint32 UnitIndex, L7_uint32 routeDest,
                                   L7_uint32 *routeType);

/*********************************************************************
* @purpose  Set the type of route.    
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    routeDest  @b{(input)} route destination for this entry
* @param    routeType  @b{(input)} The type of route.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_NOT_SUPPORTED
*
* @comments Note that the values
*           direct(3) and indirect(4) refer to the notion of
*           direct and indirect routing in the IP
*           architecture. 
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIpRouteTypeSet(L7_uint32 UnitIndex, L7_uint32 routeDest,
                                   L7_uint32 routeType);

/*********************************************************************
* @purpose  Gets the routing mechanism via which this route was
*           learned.  
*
* @param    UnitIndex   @b{(input)} the unit for this operation
* @param    routeDest   @b{(input)} route destination for this entry
* @param    *routeProto @b{(output)} protocol via which this route
*                               was learned
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIpRouteProtoGet(L7_uint32 UnitIndex, L7_uint32 routeDest,
                                    L7_uint32 *routeProto);

/*********************************************************************
* @purpose  Gets the number of seconds since this route was last
*           updated or otherwise determined to be correct
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    routeDest  @b{(input)} route destination for this entry
* @param    *routeAge  @b{(output)} seconds since the route was last updated
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_NOT_SUPPORTED
*
* @comments  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIpRouteAgeGet(L7_uint32 UnitIndex, L7_uint32 routeDest,
                                  L7_uint32 *routeAge);

/*********************************************************************
* @purpose  Sets the number of seconds since this route was last
*           updated or otherwise determined to be correct
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    routeDest  @b{(input)} route destination for this entry
* @param    routeAge   @b{(input)} Seconds since this route was last
*                               updated
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_NOT_SUPPORTED
*
* @comments  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIpRouteAgeSet(L7_uint32 UnitIndex, L7_uint32 routeDest,
                                  L7_uint32 routeAge);

/*********************************************************************
* @purpose  Gets the mask to be logical-ANDed with the
*           destination address before being compared to the
*           value in the ipRouteDest field
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    routeDest  @b{(input)} route destination for this entry
* @param    *routeMask @b{(output)} route mask
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIpRouteMaskGet(L7_uint32 UnitIndex, L7_uint32 routeDest,
                                   L7_uint32 *routeMask);

/*********************************************************************
* @purpose  Sets the mask to be logical-ANDed with the
*           destination address before being compared to the
*           value in the ipRouteDest field
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    routeDest  @b{(input)} route destination for this entry
* @param    routeMask  @b{(input)} route mask
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_NOT_SUPPORTED
*
* @comments  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIpRouteMaskSet(L7_uint32 UnitIndex, L7_uint32 routeDest,
                                   L7_uint32 routeMask);

/*********************************************************************
* @purpose  Gets an alternate routing metric for this route
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    routeDest  @b{(input)} route destination for this entry
* @param    *metric5   @b{(output)} route metric 5
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIpRouteMetric5Get(L7_uint32 UnitIndex, L7_uint32 routeDest,
                                      L7_uint32 *metric5);

/*********************************************************************
* @purpose  Sets an alternate routing metric for this route
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    routeDest  @b{(input)} route destination for this entry
* @param    *metric5    @b{(input)} route metric 5
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_NOT_SUPPORTED
*
* @comments
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIpRouteMetric5Set(L7_uint32 UnitIndex, L7_uint32 routeDest,
                                      L7_uint32 *metric5);

/*********************************************************************
* @purpose  Gets reference to MIB definitions specific to the
*           particular routing protocol which is responsible
*           for this route 
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    routeDest  @b{(input)} the destination network
* @param    *oid1      @b{(output)} object id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIpRouteInfoGet(L7_uint32 UnitIndex, L7_uint32 routeDest, L7_char8 *oid);

/*********************************************************************
* @purpose  determines if the entry exists in the ipNetToMedia table
*          
* @param    UnitIndex                  @b{(input)} the unit for this operation
* @param    ipNetToMediaIfIndex        @b{(input)} internal interface number
* @param    ipNetToMediaNetAddress     @b{(input)} network address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments includes errors received
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIpNetToMediaEntryGet(L7_uint32 UnitIndex, L7_int32 ipNetToMediaIfIndex, L7_uint32 ipNetToMediaNetAddress);

/*********************************************************************
* @purpose  get the next valid entry in the ipNetToMedia table
*          
* @param    UnitIndex                  @b{(input)}  the unit for this operation
* @param    *ipNetToMediaIfIndex       @b{(input)}  internal interface number of current entry @line
*                                      @b{(output)} internal interface number of next entry
* @param    *ipNetToMediaNetAddress    @b{(input)}  network address of current entry @line
*                                      @b{(output)} network address of next entry
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments includes errors received
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIpNetToMediaEntryNextGet(L7_uint32 UnitIndex, L7_int32 *ipNetToMediaIfIndex, L7_uint32 *ipNetToMediaNetAddress);

/*********************************************************************
* @purpose  get the ipNetToMediaPhysAddress object
*          
* @param    UnitIndex                  @b{(input)}  the unit for this operation
* @param    ipNetToMediaIfIndex        @b{(input)}  internal interface number
* @param    ipNetToMediaNetAddress     @b{(input)}  network address
* @param    *physAddress               @b{(output)} physAddress buffer
* @param    *addrLength                @b{(output)} physAddress buffer length
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments includes errors received
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIpNetToMediaPhysAddressGet(L7_uint32 UnitIndex, L7_int32 ipNetToMediaIfIndex, L7_uint32 ipNetToMediaNetAddress, L7_char8 *physAddress, L7_uint32 *addrLength);

/*********************************************************************
* @purpose  set the ipNetToMediaPhysAddress object
*          
* @param    UnitIndex                  @b{(input)}  the unit for this operation
* @param    ipNetToMediaIfIndex        @b{(input)}  internal interface number
* @param    ipNetToMediaNetAddress     @b{(input)}  network address
* @param    physAddress                @b{(input)}  physAddress buffer
* @param    addrLength                 @b{(input)}  physAddress buffer length
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments includes errors received
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIpNetToMediaPhysAddressSet(L7_uint32 UnitIndex, L7_int32 ipNetToMediaIfIndex, L7_uint32 ipNetToMediaNetAddress, L7_char8 *physAddress, L7_uint32 addrLength);

/*********************************************************************
* @purpose  get the ipNetToMediaType object
*          
* @param    UnitIndex                  @b{(input)}  the unit for this operation
* @param    ipNetToMediaIfIndex        @b{(input)}  internal interface number
* @param    ipNetToMediaNetAddress     @b{(input)}  network address
* @param    *ipNetToMediaType          @b{(output)} ipNetToMediaType
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments includes errors received
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIpNetToMediaTypeGet(L7_uint32 UnitIndex, L7_int32 ipNetToMediaIfIndex, L7_uint32 ipNetToMediaNetAddress, L7_uint32 *ipNetToMediaType);

/*********************************************************************
* @purpose  set the ipNetToMediaType object
*          
* @param    UnitIndex                  @b{(input)}  the unit for this operation
* @param    ipNetToMediaIfIndex        @b{(input)}  internal interface number
* @param    ipNetToMediaNetAddress     @b{(input)}  network address
* @param    ipNetToMediaType           @b{(input)}  ipNetToMediaType
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments includes errors received
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIpNetToMediaTypeSet(L7_uint32 UnitIndex, L7_int32 ipNetToMediaIfIndex, L7_uint32 ipNetToMediaNetAddress, L7_uint32 ipNetToMediaType);

/*********************************************************************
* @purpose  Gets the number of routing entries which were chosen
*           to be discarded even though they are valid.  
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *val       @b{(output)} Number of routing entries discarded
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIpRoutingDiscardsGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  Returns L7_MAX_INTERFACE_COUNT
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *val       @b{(output)} L7_MAX_INTERFACE_COUNT
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The number of network interfaces (regardless of
*           their current state) possible on this system  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDb1213MaxIfNumberGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  Returns the number of visible interfaces
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *val       @b{(output)} L7_MAX_INTERFACE_COUNT
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments The number of visible network interfaces present 
*           on this system
*        
* @end
*********************************************************************/
extern L7_RC_t usmDb1213IfNumberGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  Gets the algorithm used to determine the timeout value
*           used for retransmitting unacknowledged octets
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *val       @b{(output)} Algorithm
*
* @returns  L7_NOT_SUPPORTED
*
* @comments  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbTcpRtoAlgorithmGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  Gets the minimum value permitted by a TCP
*           implementation for the retransmission timeout,
*           measured in milliseconds.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *val       @b{(output)} Timeout
*
* @returns  L7_NOT_SUPPORTED
*
* @comments  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbTcpRtoMinGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  Gets the maximum value permitted by a TCP
*           implementation for the retransmission timeout,
*           measured in milliseconds
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *val       @b{(output)} Timeout
*
* @returns  L7_NOT_SUPPORTED
*
* @comments  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbTcpRtoMaxGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  The limit on the total number of TCP connections
*           the entity can support
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *val       @b{(output)} Number of Connections
*
* @returns  L7_NOT_SUPPORTED
*
* @comments  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbTcpMaxConnGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  Gets the number of times TCP connections have made a
*           direct transition to the SYN-SENT state from the
*           CLOSED state
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *val       @b{(output)} Number of times
*
* @returns  L7_NOT_SUPPORTED
*
* @comments  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbTcpActiveOpensGet(L7_uint32 UnitIndex, L7_uint32 *val); 

/*********************************************************************
* @purpose  Gets the number of times TCP connections have made a
*           direct transition to the SYN-RCVD state from the
*           LISTEN state."
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *val       @b{(output)} Number of times
*
* @returns  L7_NOT_SUPPORTED
*
* @comments  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbTcpPassiveOpensGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  Gets the number of times TCP connections have made a
*           direct transition to the CLOSED state from either
*           the SYN-SENT state or the SYN-RCVD state, plus the
*           number of times TCP connections have made a direct
*           transition to the LISTEN state from the SYN-RCVD
*           state."
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *val       @b{(output)} Number of times
*
* @returns  L7_NOT_SUPPORTED
*
* @comments  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbTcpAttemptFailsGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  The number of times TCP connections have made a
*           direct transition to the CLOSED state from either
*           the ESTABLISHED state or the CLOSE-WAIT state
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *val       @b{(output)} Number of times
*
* @returns  L7_NOT_SUPPORTED
*
* @comments  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbTcpEstabResetsGet(L7_uint32 UnitIndex, L7_uint32 *val); 

/*********************************************************************
* @purpose  The number of TCP connections for which the
*           current state is either ESTABLISHED or CLOSE-WAIT.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *val       @b{(output)} Number of Connections
*
* @returns  L7_NOT_SUPPORTED
*
* @comments  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbTcpCurrEstabGet(L7_uint32 UnitIndex, L7_uint32 *val);   

/*********************************************************************
* @purpose  Gets the total number of segments received, including
*           those received in error.  
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *val       @b{(output)} Number of segments
*
* @returns  L7_NOT_SUPPORTED
*
* @comments  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbTcpInSegsGet(L7_uint32 UnitIndex, L7_uint32 *val);      

/*********************************************************************
* @purpose  Gets the total number of segments sent, including
*           those on current connections but excluding those
*           containing only retransmitted octets
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *val       @b{(output)} Number of segments
*
* @returns  L7_NOT_SUPPORTED
*
* @comments  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbTcpOutSegsGet(L7_uint32 UnitIndex, L7_uint32 *val);       

/*********************************************************************
* @purpose  Gets the total number of segments retransmitted - that
*           is, the number of TCP segments transmitted
*           containing one or more previously transmitted
*           octets
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *val       @b{(output)} Number of segments
*
* @returns  L7_NOT_SUPPORTED
*
* @comments  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbTcpRetransSegsGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  Gets the state of this TCP connection.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *val       @b{(output)} State
*
* @returns  L7_NOT_SUPPORTED
*
* @comments  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbTcpConnStateGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  Set the state of this TCP connection.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    val        @b{(input)} State
*
* @returns  L7_NOT_SUPPORTED
*
* @comments  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbTcpConnStateSet(L7_uint32 UnitIndex, L7_uint32 val);

/*********************************************************************
* @purpose  Gets the local IP address for this TCP connection
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *val       @b{(output)} Local Ip Address
*
* @returns  L7_NOT_SUPPORTED
*
* @comments  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbTcpConnLocalAddressGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  Gets the local port number for this TCP connection
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *val       @b{(output)} Local port
*
* @returns  L7_NOT_SUPPORTED
*
* @comments  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbTcpConnLocalPortGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  Gets the remote IP address for this TCP connection
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *val       @b{(output)} Remote Ip Address
*
* @returns  L7_NOT_SUPPORTED
*
* @comments  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbTcpConnRemAddressGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  Gets the remote port number for this TCP connection
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *val       @b{(output)} Remote port
*
* @returns  L7_NOT_SUPPORTED
*
* @comments  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbTcpConnRemPortGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  Gets the total number of segments received in error
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *val       @b{(output)} Number of segments
*
* @returns  L7_NOT_SUPPORTED
*
* @comments  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbTcpInErsGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  Gets the number of TCP segments sent containing the
*           RST flag
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *val       @b{(output)} Number of segments
*
* @returns  L7_NOT_SUPPORTED
*
* @comments  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbTcpOutRstsGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  The total number of ICMP messages which the entity 
*           received.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *val       @b{(output)} Number of messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments  Note that this counter includes all those counted by
*            icmpInErrors
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIcmpInMsgsGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  The number of ICMP messages which the entity received
*           but determined as having ICMP specific errors (bad ICMP
*           checksums, bad length, etc.)
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *val       @b{(output)} Number of messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIcmpInErrorsGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  The number of ICMP Destination Unreachable messages
*           received.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *val       @b{(output)} Number of messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIcmpInDestUnreachsGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  The number of ICMP Time Exceeded messages received.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *val       @b{(output)} Number of messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIcmpInTimeExcdsGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  The number of ICMP Parameter Problem messages received
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *val       @b{(output)} Number of messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIcmpInParmProbsGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  The number of ICMP Source Quench messages.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *val       @b{(output)} Number of messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIcmpInSrcQuenchsGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  The number of ICMP Redirect messages received.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    val        @b{(output)} Number of messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIcmpInRedirectsGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  The number of ICMP Echo (request) messages received. 
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *val       @b{(output)} Number of messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIcmpInEchosGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  The number of ICMP Echo Reply messages received
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *val       @b{(output)} Number of messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIcmpInEchoRepsGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  The number of ICMP Timestamp (request) messages received
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *val       @b{(output)} Number of messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIcmpInTimestampsGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  The number of ICMP Timestamp Reply messages
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *val       @b{(output)} Number of messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIcmpInTimestampRepsGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  The number of ICMP Address Mask Request messages received
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *val       @b{(output)} Number of messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIcmpInAddrMasksGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  The number of ICMP Address Mask Reply messages received
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *val       @b{(output)} Number of messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIcmpInAddrMaskRepsGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  The total number of ICMP messages which this entity
*           attempted to send.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *val       @b{(output)} Number of messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments Note that this counter includes all those counted by
*           icmpOutErrors
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIcmpOutMsgsGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  The number of ICMP messages which this entity did not
*           send due to problems discovered within ICMP.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *val       @b{(output)} Number of messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments This includes problems such as lack of buffers. This
*           value should not include errors discovered outside of
*           the ICMP layer such as the inability of IP to route the
*           resultant datagram. In some implementations there may
*           be no types of error which contribute to this counter's
*           value.
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIcmpOutErrorsGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  The number of ICMP Destination Unreachable messages
*           sent
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *val       @b{(output)} Number of messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIcmpOutDestUnreachsGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  The number of ICMP Time Exceeded messages sent.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *val       @b{(output)} Number of messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIcmpOutTimeExcdsGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  The number of ICMP Parameter Problem messages sent.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *val       @b{(output)} Number of messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIcmpOutParmProbsGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  The number of ICMP Source Quench messages sent.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *val       @b{(output)} Number of messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIcmpOutSrcQuenchsGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  The number of ICMP Redirect messages sent.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *val       @b{(output)} Number of messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments For a host, this object will always be zero, since
*           hosts do not send redirects.
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIcmpOutRedirectsGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  The number of ICMP Echo (request) messages sent.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *val       @b{(output)} Number of messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIcmpOutEchosGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  The number of ICMP Echo Reply messages sent.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *val       @b{(output)} Number of messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIcmpOutEchoRepsGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  The number of ICMP Timestamp (request) messages sent.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *val       @b{(output)} Number of messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIcmpOutTimestampsGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  The number of ICMP Timestamp Reply messages sent
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *val       @b{(output)} Number of messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIcmpOutTimestampRepsGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  The number of ICMP Address mask Request messages sent.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *val       @b{(output)} Number of messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIcmpOutAddrMasksGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  The number of ICMP Address Mask Reply messages sent.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *val       @b{(output)} Number of messages
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIcmpOutAddrMaskRepsGet(L7_uint32 UnitIndex, L7_uint32 *val);

/*********************************************************************
* @purpose  Get the total number of input datagrams received from interfaces
*          
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *high      @b{(output)} High order 4 bytes of number of datagrams
* @param    *low       @b{(output)} Low order 4 bytes of number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments includes errors received
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIpInReceivesGet64(L7_uint32 UnitIndex, L7_uint32 *high, L7_uint32 *low);

/*********************************************************************
* @purpose  Get the total number of input datagrams discarded due to headers
*          
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *high      @b{(output)} High order 4 bytes of number of datagrams
* @param    *low       @b{(output)} Low order 4 bytes of number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments The number of input datagrams discarded due to errors
*           in their IP headers, including bad checksums, version
*           number mismatch, other format errors, time-to-live
*           exceeded, errors discovered in processing their IP
*           options etc.
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIpInHdrErrorsGet64(L7_uint32 UnitIndex, L7_uint32 *high, L7_uint32 *low);

/*********************************************************************
* @purpose  Gets the number of input datagrams discarded because
*           the IP address in their IP header's destination
*           field was not a valid address to be received at
*           this entity.
*          
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *high      @b{(output)} High order 4 bytes of number of datagrams
* @param    *low       @b{(output)} Low order 4 bytes of number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments   
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIpInAddrErrorsGet64(L7_uint32 UnitIndex, L7_uint32 *high, L7_uint32 *low);

/*********************************************************************
* @purpose  Gets the number of input datagrams for which this
*           entity was not their final IP destination, as a
*           result of which an attempt was made to find a
*           route to forward them to that final destination.
*          
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *high      @b{(output)} High order 4 bytes of number of datagrams
* @param    *low       @b{(output)} Low order 4 bytes of number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments   
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIpForwDatagramsGet64(L7_uint32 UnitIndex, L7_uint32 *high, L7_uint32 *low);

/*********************************************************************
* @purpose  Gets the number of locally-addressed datagrams
*           received successfully but discarded because of an
*           unknown or unsupported protocol.
*          
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *high      @b{(output)} High order 4 bytes of number of datagrams
* @param    *low       @b{(output)} Low order 4 bytes of number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments   
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIpInUnknownProtosGet64(L7_uint32 UnitIndex, L7_uint32 *high, L7_uint32 *low);

/*********************************************************************
* @purpose  Gets the number of input IP datagrams for which no
*           problems were encountered to prevent their
*           continued processing, but which were discarded
*                     
*          
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *high      @b{(output)} High order 4 bytes of number of datagrams
* @param    *low       @b{(output)} Low order 4 bytes of number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments   
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIpInDiscards64(L7_uint32 UnitIndex, L7_uint32 *high, L7_uint32 *low);

/*********************************************************************
* @purpose  Gets the total number of input datagrams successfully
*           delivered to IP user-protocols (including ICMP).
*          
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *high      @b{(output)} High order 4 bytes of number of datagrams
* @param    *low       @b{(output)} Low order 4 bytes of number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments   
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIpInDelivers64(L7_uint32 UnitIndex, L7_uint32 *high, L7_uint32 *low);

/*********************************************************************
* @purpose  Gets the total number of IP datagrams which local IP
*           user-protocols (including ICMP) supplied to IP in
*           requests for transmission.  
*          
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *high      @b{(output)} High order 4 bytes of number of datagrams
* @param    *low       @b{(output)} Low order 4 bytes of number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments Note that this counter
*           does not include any datagrams counted in
*           ipForwDatagrams
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIpOutRequests64(L7_uint32 UnitIndex, L7_uint32 *high, L7_uint32 *low);

/*********************************************************************
* @purpose  Gets the number of output IP datagrams for which no
*           problem was encountered to prevent their
*           transmission to their destination, but which were
*           discarded (e.g., for lack of buffer space).  
*          
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *high      @b{(output)} High order 4 bytes of number of datagrams
* @param    *low       @b{(output)} Low order 4 bytes of number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments   
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIpOutDiscardsGet64(L7_uint32 UnitIndex, L7_uint32 *high, L7_uint32 *low);

/*********************************************************************
* @purpose  Gets the number of IP datagrams discarded because no
*           route could be found to transmit them to their
*           destination.  
*          
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *high      @b{(output)} High order 4 bytes of number of datagrams
* @param    *low       @b{(output)} Low order 4 bytes of number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments   
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIpNoRoutesGet64(L7_uint32 UnitIndex, L7_uint32 *high, L7_uint32 *low);

/*********************************************************************
* @purpose  Gets the maximum number of seconds which received
*           fragments are held while they are awaiting
*           reassembly at this entity
*          
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *high      @b{(output)} High order 4 bytes of timeout
* @param    *low       @b{(output)} Low order 4 bytes of timeout
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments   
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIpReasmTimeoutGet64(L7_uint32 UnitIndex, L7_uint32 *high, L7_uint32 *low);

/*********************************************************************
* @purpose  The number of IP fragments received which needed to be
*           reassembled at this entity
*          
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *high      @b{(output)} High order 4 bytes of number of datagrams
* @param    *low       @b{(output)} Low order 4 bytes of number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments   
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIpReasmReqdsGet64(L7_uint32 UnitIndex, L7_uint32 *high, L7_uint32 *low);

/*********************************************************************
* @purpose  Gets the number of IP datagrams successfully re-assembled
*          
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *high      @b{(output)} High order 4 bytes of number of datagrams
* @param    *low       @b{(output)} Low order 4 bytes of number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments   
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIpReasmOKsGet64(L7_uint32 UnitIndex, L7_uint32 *high, L7_uint32 *low);

/*********************************************************************
* @purpose  Gets the number of failures detected by the IP re-
*           assembly algorithm (for whatever reason: timed
*           out, errors, etc).  
*          
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *high      @b{(output)} High order 4 bytes of number of failures
* @param    *low       @b{(output)} Low order 4 bytes of number of failures
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments   
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIpReasmFailsGet64(L7_uint32 UnitIndex, L7_uint32 *high, L7_uint32 *low);

/*********************************************************************
* @purpose  Gets the number of IP datagrams that have been
*           successfully fragmented at this entity
*          
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *high      @b{(output)} High order 4 bytes of number of datagrams
* @param    *low       @b{(output)} Low order 4 bytes of number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments   
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIpFragOKsGet64(L7_uint32 UnitIndex, L7_uint32 *high, L7_uint32 *low);

/*********************************************************************
* @purpose  Gets the number of IP datagrams that have been
*           discarded because they needed to be fragmented at
*           this entity but could not be, e.g., because their
*           Don't Fragment flag was set.
*          
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *high      @b{(output)} High order 4 bytes of number of datagrams
* @param    *low       @b{(output)} Low order 4 bytes of number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments   
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIpFragFailsGet64(L7_uint32 UnitIndex, L7_uint32 *high, L7_uint32 *low);

/*********************************************************************
* @purpose  Gets the number of IP datagram fragments that have
*           been generated as a result of fragmentation at
*           this entity.
*          
* @param    UnitIndex  @b{(input)}  the unit for this operation
* @param    *high      @b{(output)} High order 4 bytes of number of datagrams
* @param    *low       @b{(output)} Low order 4 bytes of number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments   
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIpFragCreatesGet64(L7_uint32 UnitIndex, L7_uint32 *high, L7_uint32 *low);

/*********************************************************************
* @purpose  Gets the number of routing entries which were chosen
*           to be discarded even though they are valid.  
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *high      @b{(output)} High order 4 bytes of number of entries
* @param    *low       @b{(output)} Low order 4 bytes of number of entries
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIpRoutingDiscardsGet64(L7_uint32 UnitIndex, L7_uint32 *high, L7_uint32 *low);

/*********************************************************************
* @purpose  The total number of ICMP messages which the entity 
*           received.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *high      @b{(output)} High order 4 bytes of number of datagrams
* @param    *low       @b{(output)} Low order 4 bytes of number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments Note that this counter includes all those counted by
*           icmpInErrors
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIcmpInMsgsGet64(L7_uint32 UnitIndex, L7_uint32 *high, L7_uint32 *low);

/*********************************************************************
* @purpose  The number of ICMP messages which the entity received
*           but determined as having ICMP specific errors (bad ICMP
*           checksums, bad length, etc.)
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *high      @b{(output)} High order 4 bytes of number of datagrams
* @param    *low       @b{(output)} Low order 4 bytes of number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIcmpInErrorsGet64(L7_uint32 UnitIndex, L7_uint32 *high, L7_uint32 *low);

/*********************************************************************
* @purpose  The number of ICMP Destination Unreachable messages
*           received.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *high      @b{(output)} High order 4 bytes of number of datagrams
* @param    *low       @b{(output)} Low order 4 bytes of number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIcmpInDestUnreachsGet64(L7_uint32 UnitIndex, L7_uint32 *high, L7_uint32 *low);

/*********************************************************************
* @purpose  The number of ICMP Time Exceeded messages received.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *high      @b{(output)} High order 4 bytes of number of datagrams
* @param    *low       @b{(output)} Low order 4 bytes of number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIcmpInTimeExcdsGet64(L7_uint32 UnitIndex, L7_uint32 *high, L7_uint32 *low);

/*********************************************************************
* @purpose  The number of ICMP Parameter Problem messages received
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *high      @b{(output)} High order 4 bytes of number of datagrams
* @param    *low       @b{(output)} Low order 4 bytes of number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIcmpInParmProbsGet64(L7_uint32 UnitIndex, L7_uint32 *high, L7_uint32 *low);

/*********************************************************************
* @purpose  The number of ICMP Source Quench messages.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *high      @b{(output)} High order 4 bytes of number of datagrams
* @param    *low       @b{(output)} Low order 4 bytes of number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIcmpInSrcQuenchsGet64(L7_uint32 UnitIndex, L7_uint32 *high, L7_uint32 *low);

/*********************************************************************
* @purpose  The number of ICMP Redirect messages received.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *high      @b{(output)} High order 4 bytes of number of datagrams
* @param    *low       @b{(output)} Low order 4 bytes of number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIcmpInRedirectsGet64(L7_uint32 UnitIndex, L7_uint32 *high, L7_uint32 *low);

/*********************************************************************
* @purpose  The number of ICMP Echo (request) messages received. 
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *high      @b{(output)} High order 4 bytes of number of datagrams
* @param    *low       @b{(output)} Low order 4 bytes of number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIcmpInEchosGet64(L7_uint32 UnitIndex, L7_uint32 *high, L7_uint32 *low);

/*********************************************************************
* @purpose  The number of ICMP Echo Reply messages received
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *high      @b{(output)} High order 4 bytes of number of datagrams
* @param    *low       @b{(output)} Low order 4 bytes of number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIcmpInEchoRepsGet64(L7_uint32 UnitIndex, L7_uint32 *high, L7_uint32 *low);

/*********************************************************************
* @purpose  The number of ICMP Timestamp (request) messages received
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *high      @b{(output)} High order 4 bytes of number of datagrams
* @param    *low       @b{(output)} Low order 4 bytes of number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIcmpInTimestampsGet64(L7_uint32 UnitIndex, L7_uint32 *high, L7_uint32 *low);

/*********************************************************************
* @purpose  The number of ICMP Timestamp Reply messages
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *high      @b{(output)} High order 4 bytes of number of datagrams
* @param    *low       @b{(output)} Low order 4 bytes of number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIcmpInTimestampRepsGet64(L7_uint32 UnitIndex, L7_uint32 *high, L7_uint32 *low);

/*********************************************************************
* @purpose  The number of ICMP Address Mask Request messages received
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *high      @b{(output)} High order 4 bytes of number of datagrams
* @param    *low       @b{(output)} Low order 4 bytes of number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments 
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIcmpInAddrMasksGet64(L7_uint32 UnitIndex, L7_uint32 *high, L7_uint32 *low);

/*********************************************************************
* @purpose  The number of ICMP Address Mask Reply messages received
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *high      @b{(output)} High order 4 bytes of number of datagrams
* @param    *low       @b{(output)} Low order 4 bytes of number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIcmpInAddrMaskRepsGet64(L7_uint32 UnitIndex, L7_uint32 *high, L7_uint32 *low);

/*********************************************************************
* @purpose  The total number of ICMP messages which this entity
*           attempted to send.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *high      @b{(output)} High order 4 bytes of number of datagrams
* @param    *low       @b{(output)} Low order 4 bytes of number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments Note that this counter includes all those counted by
*           icmpOutErrors
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIcmpOutMsgsGet64(L7_uint32 UnitIndex, L7_uint32 *high, L7_uint32 *low);

/*********************************************************************
* @purpose  The number of ICMP messages which this entity did not
*           send due to problems discovered within ICMP.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *high      @b{(output)} High order 4 bytes of number of datagrams
* @param    *low       @b{(output)} Low order 4 bytes of number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments This includes problems such as lack of buffers. This
*           value should not include errors discovered outside of
*           the ICMP layer such as the inability of IP to route the
*           resultant datagram. In some implementations there may
*           be no types of error which contribute to this counter's
*           value.
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIcmpOutErrorsGet64(L7_uint32 UnitIndex, L7_uint32 *high, L7_uint32 *low);

/*********************************************************************
* @purpose  The number of ICMP Destination Unreachable messages
*           sent
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *high      @b{(output)} High order 4 bytes of number of datagrams
* @param    *low       @b{(output)} Low order 4 bytes of number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIcmpOutDestUnreachsGet64(L7_uint32 UnitIndex, L7_uint32 *high, L7_uint32 *low);

/*********************************************************************
* @purpose  The number of ICMP Time Exceeded messages sent.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *high      @b{(output)} High order 4 bytes of number of datagrams
* @param    *low       @b{(output)} Low order 4 bytes of number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIcmpOutTimeExcdsGet64(L7_uint32 UnitIndex, L7_uint32 *high, L7_uint32 *low);

/*********************************************************************
* @purpose  The number of ICMP Parameter Problem messages sent.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *high      @b{(output)} High order 4 bytes of number of datagrams
* @param    *low       @b{(output)} Low order 4 bytes of number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIcmpOutParmProbsGet64(L7_uint32 UnitIndex, L7_uint32 *high, L7_uint32 *low);

/*********************************************************************
* @purpose  The number of ICMP Source Quench messages sent.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *high      @b{(output)} High order 4 bytes of number of datagrams
* @param    *low       @b{(output)} Low order 4 bytes of number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIcmpOutSrcQuenchsGet64(L7_uint32 UnitIndex, L7_uint32 *high, L7_uint32 *low);

/*********************************************************************
* @purpose  The number of ICMP Redirect messages sent.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *high      @b{(output)} High order 4 bytes of number of datagrams
* @param    *low       @b{(output)} Low order 4 bytes of number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments For a host, this object will always be zero, since
*           hosts do not send redirects.
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIcmpOutRedirectsGet64(L7_uint32 UnitIndex, L7_uint32 *high, L7_uint32 *low);

/*********************************************************************
* @purpose  The number of ICMP Echo Reply messages sent.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *high      @b{(output)} High order 4 bytes of number of datagrams
* @param    *low       @b{(output)} Low order 4 bytes of number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIcmpOutEchoRepsGet64(L7_uint32 UnitIndex, L7_uint32 *high, L7_uint32 *low);

/*********************************************************************
* @purpose  The number of ICMP Timestamp (request) messages sent.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *high      @b{(output)} High order 4 bytes of number of datagrams
* @param    *low       @b{(output)} Low order 4 bytes of number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIcmpOutTimestampsGet64(L7_uint32 UnitIndex, L7_uint32 *high, L7_uint32 *low);

/*********************************************************************
* @purpose  The number of ICMP Timestamp Reply messages sent
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *high      @b{(output)} High order 4 bytes of number of datagrams
* @param    *low       @b{(output)} Low order 4 bytes of number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIcmpOutTimestampRepsGet64(L7_uint32 UnitIndex, L7_uint32 *high, L7_uint32 *low);

/*********************************************************************
* @purpose  The number of ICMP Address mask Request messages sent.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *high      @b{(output)} High order 4 bytes of number of datagrams
* @param    *low       @b{(output)} Low order 4 bytes of number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIcmpOutAddrMasksGet64(L7_uint32 UnitIndex, L7_uint32 *high, L7_uint32 *low);

/*********************************************************************
* @purpose  The number of ICMP Address Mask Reply messages sent.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *high      @b{(output)} High order 4 bytes of number of datagrams
* @param    *low       @b{(output)} Low order 4 bytes of number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIcmpOutAddrMaskRepsGet64(L7_uint32 UnitIndex, L7_uint32 *high, L7_uint32 *low);

/*********************************************************************
* @purpose  The number of ICMP Echo (request) messages sent.
*
* @param    UnitIndex  @b{(input)} the unit for this operation
* @param    *high      @b{(output)} High order 4 bytes of number of datagrams
* @param    *low       @b{(output)} Low order 4 bytes of number of datagrams
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR   if requested counter does not exist 
*
* @comments  
*        
* @end
*********************************************************************/
extern L7_RC_t usmDbIcmpOutEchosGet64(L7_uint32 UnitIndex, L7_uint32 *high, L7_uint32 *low);

/*********************************************************************
* @purpose  Convert an interface IP address to an internal interface number
*
* @param    UnitIndex   @{(input)} Unit for this operation
* @param    IpAddr      @{(input)} IP Address of interest
* @param    *intIfNum   @{(output)} Internal interface number for the 
*                                   specified ip address
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments
*       
* @end
*********************************************************************/
extern L7_RC_t usmDbIPAddrToIntIf(L7_uint32 UnitIndex, L7_uint32 IpAddr,
                                  L7_uint32 *intIfNum);

/*********************************************************************
*
* @purpose  Determine if the specified interface is valid for routing 
*
* @param    unitIndex          Unit Index
* @param    iface  @b{(input)} interface to validate
*                     
* @returns  L7_SUCCESS  The interface is valid for routing
* @returns  L7_FAILURE  The interface is not valid for routing
*     
* @comments If port-based routing is supported, any slot.port is 
*           considered valid.  This routine assumes the range for
*           slot and port have already been verified.
* @end
*
*********************************************************************/
extern L7_RC_t usmDbValidateRtrIntf(L7_uint32 unitIndex, L7_uint32 iface);

#endif	/* _USMDB_1213_API_H_ */

