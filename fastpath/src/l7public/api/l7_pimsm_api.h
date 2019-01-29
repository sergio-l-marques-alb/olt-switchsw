/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename   l7_pimsm_api.h
*
* @purpose    PIM-SM Mapping Layer APIs
*
* @component  PIM-SM Mapping Layer
*
* @comments   none
*
* @create     03/09/2002
*
* @author     Ratnakar
* @end
*
**********************************************************************/


#ifndef _L7_PIMSM_API_H_
#define _L7_PIMSM_API_H_

#include "l3_comm_structs.h"
#include "l3_mcast_commdefs.h"
#include "comm_mask.h"
/*********************************************************************
* @purpose  Set PIM-SM Mapping Layer tracing mode
*
* @param    mode        @b{(input)} L7_ENABLE or L7_DISABLE
* @param    familyType  @b{(input)} Address Family type
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL pimsmMapTraceModeSet(L7_uint32 mode, L7_uchar8 familyType);

/*********************************************************************
* @purpose  Get IP Mapping Layer tracing mode
*
* @param    familyType             Address Family type
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL  pimsmMapTraceModeGet(L7_uchar8 familyType);


/*---------------------------------------------------------------------
 *                    API FUNCTIONS  -  GLOBAL CONFIG
 *---------------------------------------------------------------------
 */
/*********************************************************************
* @purpose  Test is inhe PIM-SSM Range.
*
* @param    familyType       @b{(input)} Address Family type
* @param    groupAddr        @b{(input)} Group Address
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none    
*
* @end
*********************************************************************/
L7_BOOL pimsmMapIsInSsmRange(L7_uchar8 familyType, L7_inet_addr_t *groupAddr);

/*********************************************************************
* @purpose  Set the PIM-SM SSM Range.
*
* @param    familyType       @b{(input)} Address Family type
* @param    ssmMode          @b{(input)} ssm Mode
* @param    groupAddr        @b{(input)} SSM Range Addr
* @param    groupMask        @b{(input)} mask
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSsmRangeSet(L7_uchar8 familyType, L7_uint32 ssmMode,
                    L7_inet_addr_t *groupAddr, L7_inet_addr_t *groupMask);

/*********************************************************************
* @purpose  Check whether ssm range entry exists.
*
* @param    familyType       @b{(input)} Address Family type
* @param    ssmRangeAddr     @b{(input)} SSM Range Addr
* @param    prefixLen        @b{(input)} length of the mask
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSsmRangeEntryGet(L7_uchar8 familyType,
                                 L7_inet_addr_t *ssmRangeAddr,
                                 L7_uchar8 prefixLen);

/*********************************************************************
* @purpose  Obtain next  ssm range entry  in the ssmrange TABLE
*
* @param    familyType       @b{(input)} Address Family type
* @param    ssmRangeAddr     @b{(inout)} SSM Range Addr
* @param    prefixLen        @b{(inout)} length of the mask
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments next interface entry
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSsmRangeEntryNextGet(L7_uchar8 familyType,
                                     L7_inet_addr_t *ssmRangeAddr,
                                     L7_uchar8 *prefixLen);

/*********************************************************************
* @purpose  Gets the admin mode for the specified interface
*
* @param    familyType       @b{(input)} Address Family type
* @param    ssmRangeAddr     @b{(input)} SSM Range Addr
* @param    prefixLen        @b{(input)} length of the mask
* @param    mode             @b{(output)} ssm Mode
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR      if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSsmRangeModeGet(L7_uchar8 familyType,
                                L7_inet_addr_t *ssmRangeAddr, 
                                L7_uchar8 length, L7_uint32 *ssmMode);
/*********************************************************************
* @purpose  Sets the admin mode for the ssm range entry.
*
* @param    familyType       @b{(input)} Address Family type
* @param    ssmRangeAddr     @b{(input)} SSM Range Addr
* @param    prefixLen        @b{(input)} length of the mask
* @param    ssmMode          @b{(input)} ssm Mode
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR      if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSsmRangeModeSet(L7_uchar8 familyType,
                                L7_inet_addr_t *ssmRangeAddr, 
                                L7_uchar8 prefixLen, L7_uint32 ssmMode);

/*********************************************************************
* @purpose  Set the PIM-SM administrative mode
*
* @param    familyType      @b{(input)}     Address Family type
* @param    mode            @b{(input)}     L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Dynamic configurability is supported
*
* @end
*********************************************************************/
L7_RC_t pimsmMapPimsmAdminModeSet(L7_uchar8 familyType, L7_uint32 mode);

/*********************************************************************
* @purpose  Get the PIM-SM administrative mode
*
* @param    familyType         @b{(input)}   Address Family type
* @param    mode               @b{(input)}   L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapPimsmAdminModeGet(L7_uchar8 familyType,
                                  L7_uint32 *mode);

/*********************************************************************
* @purpose  Returns whether PIM-SM is operational on router or not
*
* @param    familyType       @b{(input)}     Address Family type
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none
*
* @end
*********************************************************************/
L7_BOOL pimsmMapPimsmIsOperational(L7_uchar8 familyType);

/*********************************************************************
* @purpose  Gets the PIM-SM Join/Prune Interval
*
* @param    familyType         @b{(input)}  Address Family type
* @param    joinpruneIntvl     @b{(output)} Join/Prune Interval in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapJoinPruneIntervalGet(L7_uchar8 familyType,
                                     L7_uint32 *joinpruneIntvl);

/*********************************************************************
* @purpose  Sets the data threshold rate
*
* @param    familyType          @b{(input)} Address Family type
* @param    dataThresholdRate   @b{(input)} Data Threshold Rate in kbits/sec
*

* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Range (0-2000) checking done here
*
* @end
*********************************************************************/
L7_RC_t pimsmMapDataThresholdRateSet(L7_uchar8 familyType,
                                     L7_uint32 dataThresholdRate);

/*********************************************************************
* @purpose  Gets the data threshold rate
*
* @param    familyType          @b{(input)}  Address Family type
* @param    dataThresholdRate   @b{(output)} Data Threshold Rate in kbits/sec
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapDataThresholdRateGet(L7_uchar8 familyType,
                                     L7_uint32 *dataThresholdRate);

/*********************************************************************
* @purpose  Sets the register threshold rate
*
* @param    familyType         @b{(input)}  Address Family type
* @param    regThresholdRate   @b{(input)}  Register Threshold Rate in kbits/sec
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Range (0-2000) checking done here
*
* @end
*********************************************************************/
L7_RC_t pimsmMapRegisterThresholdRateSet(L7_uchar8 familyType,
                                         L7_uint32 regThresholdRate);

/*********************************************************************
* @purpose  Gets the register threshold rate
*
* @param    familyType        @b{(input)}   Address Family type
* @param    regThresholdRate  @b{(output)}  Register Threshold Rate in kbits/sec
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapRegisterThresholdRateGet(L7_uchar8 familyType,
                                         L7_uint32 *regThresholdRate);

/*********************************************************************
* @purpose  Sets the static RP information
*
* @param    familyType       @b{(input)}    Address Family type
* @param    rpIpAddr         @b{(input)}    Ip address of the RP
* @param    rpGrpAddr        @b{(input)}    Group address supported by the RP
* @param    prefixLen        @b{(input)}    prefix Length.
* @param    conflict         @b{(input)}    over ride.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*********************************************************************/

L7_RC_t pimsmMapStaticRPSet(L7_uchar8 familyType, L7_inet_addr_t *rpIpAddr,
    L7_inet_addr_t *rpGrpAddr, L7_uchar8 prefixLen, L7_BOOL conflict);

/*********************************************************************
* @purpose  Sets the static RP information
*
* @param    familyType       @b{(input)}    Address Family type
* @param    rpIpAddr         @b{(input)}    Ip address of the RP
* @param    rpGrpAddr        @b{(input)}    Group address supported by the RP
* @param    prefixLen        @b{(input)}    prefix Length.
* @param    conflict         @b{(input)}    over ride.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStaticRPRemove(L7_uchar8 familyType, L7_inet_addr_t *rpIpAddr,
    L7_inet_addr_t *rpGrpAddr, L7_uchar8 prefixLen);

/*********************************************************************
* @purpose  Gets the static RP entry.
*
* @param    familyType     @b{(input)}      Address Family type
* @param    rpGrpAddr      @b{(input)}      Group address supported by the RP
* @param    prefixLen      @b{(input)}      prefix Length.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStaticRPEntryGet(L7_uchar8 familyType,
                                 L7_inet_addr_t *rpGrpAddr,
                                 L7_uchar8 prefixLen);
/*********************************************************************
* @purpose  Gets the next static RP information after the supplied info
*
* @param    familyType       @b{(input)}    Address Family type
* @param    rpGrpAddr        @b{(input)}    Group address supported by the RP
* @param    prefixLen        @b{(output)}   prefix Length
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStaticRPNextGet(L7_uchar8 familyType,
                                L7_inet_addr_t *rpGrpAddr,
                                L7_uchar8 *prefixLen);

/*********************************************************************
* @purpose  To get the StaticRPOverrideDynamic object.
*
* @param    familyType   @b{(input)}  Address Family type.
* @param    rpGrpAddr    @b{(input)}  static RP Group address.
* @param    prefixLength @b{(input)}  prefix Length.
* @param    overRide     @b{(output)} OverrideDynamic.
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStaticRPOverrideDynamicGet(L7_uchar8 familyType,
                                           L7_inet_addr_t *rpGrpAddr,
                                           L7_uchar8 prefixLength,
                                           L7_uint32 *overRide);

/*********************************************************************
* @purpose  To set the StaticRPOverrideDynamic object.
*
* @param    familyType   @b{(input)}  Address Family type.
* @param    rpGrpAddr    @b{(input)}  static RP Group address.
* @param    prefixLength @b{(input)}  prefix Length.
* @param    overRide     @b{(input)}  OverrideDynamic.
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStaticRPOverrideDynamicSet(L7_uchar8 familyType,
                                           L7_inet_addr_t *rpGrpAddr,
                                           L7_uchar8 prefixLength,
                                           L7_uint32 overRide);

/*********************************************************************
* @purpose  To get the static RP Address.
*
* @param    familyType    @b{(input)}  Address Family type.
* @param    rpGrpAddr     @b{(input)}  static RP Group address.
* @param    prefixLength  @b{(input)}  prefix Length.
* @param    staticRpAddr  @b{(output)}  Static RP Addr
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStaticRPAddrGet(L7_uchar8 familyType,
                                L7_inet_addr_t *rpGrpAddr,
                                L7_uchar8 prefixLength,
                                L7_inet_addr_t *staticRpAddr);

/*********************************************************************
* @purpose  To set the static RP Address.
*
* @param    familyType    @b{(input)}  Address Family type.
* @param    rpGrpAddr     @b{(input)}  static RP Group address.
* @param    prefixLength  @b{(input)}  prefix Length.
* @param    staticRpAddr  @b{(input)}  Static RP Addr
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStaticRPAddrSet(L7_uchar8 familyType, 
                                L7_inet_addr_t *rpGrpAddr, 
                                L7_uchar8 prefixLength,
                                L7_inet_addr_t *staticRpAddr);

/*********************************************************************
* @purpose  Gets the admin mode for the specified interface
*
* @param    familyType    @b{(input)}  Address Family type.
* @param    rpGrpAddr     @b{(input)}  static RP Group address.
* @param    prefixLength  @b{(input)}  prefix Length.
* @param    rowStatus     @b{(output)}  Static RP Addr
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR      if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStaticRPRowStatusGet(L7_uchar8 familyType,
                                      L7_inet_addr_t *rpGrpAddr, 
                                      L7_uchar8 prefixLength, 
                                      L7_uint32 *rowStatus);

/*********************************************************************
* @purpose  Sets the admin mode.
*
* @param    familyType    @b{(input)}  Address Family type.
* @param    rpGrpAddr     @b{(input)}  static RP Group address.
* @param    prefixLength  @b{(input)}  prefix Length.
* @param    rowStatus     @b{(input)}  Static RP Addr.
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR      if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStaticRPRowStatusSet(L7_uchar8 familyType,
                                     L7_inet_addr_t *rpGrpAddr, 
                                     L7_uchar8 prefixLength, 
                                     L7_uint32 rowStatus);

/*********************************************************************
* @purpose  Gets the number of static RP entries
*
* @param    familyType  @b{(input)}   Address Family type
* @param    num         @b{(output)}  Number of entries.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t pimsmMapNumStaticRPEntriesGet(L7_uchar8 familyType,
                                      L7_uint32 *num);

/*********************************************************************
* @purpose  Set the administrative mode of a PIM-SM routing interface
*
* @param    familyType @b{(input)}  Address Family type
* @param    intIfNum   @b{(input)}  Internal Interface Number
* @param    mode       @b{(input)}  L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapInterfaceModeSet(L7_uchar8 familyType,
                                 L7_uint32 intIfNum, L7_uint32 mode);

/*********************************************************************
* @purpose  Get the administrative mode of a PIM-SM routing interface
*
* @param    familyType  @b{(input)} Address Family type
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    mode        @b{(output)} L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapInterfaceModeGet(L7_uchar8 familyType,
           L7_uint32 intIfNum, L7_uint32 *mode);

/*********************************************************************
* @purpose  Returns whether PIM-SM is operational on interface or not
*
*
* @param    familyType  @b{(input)}     Address Family type
* @param    intIfNum    @b{(input)}     Internal Interface Number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
*
* @comments none
*
* @end
*********************************************************************/
L7_BOOL pimsmMapIntfIsOperational(L7_uchar8 familyType,
                                  L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Set the Hello interval of a PIM-SM routing interface
*
* @param    familyType  @b{(input)}   Address Family type
* @param    intIfNum    @b{(input)}   Internal Interface Number
* @param    helloIntvl  @b{(input)}   Hello Interval in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Range checking done here
*
* @end
*********************************************************************/
L7_RC_t pimsmMapInterfaceHelloIntervalSet(L7_uchar8 familyType,
                                          L7_uint32 intIfNum,
                                          L7_uint32 helloIntvl);

/*********************************************************************
* @purpose  Get the Hello interval of a PIM-SM routing interface
*
* @param    familyType  @b{(input)}   Address Family type
* @param    intIfNum    @b{(input)}   Internal Interface Number
* @param    helloIntvl  @b{(output)}  Hello Interval in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapInterfaceHelloIntervalGet(L7_uchar8 familyType,
                                          L7_uint32 intIfNum,
                                          L7_uint32 *helloIntvl);

/*********************************************************************
* @purpose  Sets the Join/Prune Interval for the specified interface
*
* @param    familyType     @b{(input)}     Address Family type
* @param    intIfNum       @b{(input)}     Internal Interface Number
* @param    joinpruneIntvl @b{(input)}     Join/Prune Interval in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Range checking done here
*
* @end
*********************************************************************/
L7_RC_t pimsmMapInterfaceJoinPruneIntervalSet(L7_uchar8 familyType,
                                              L7_uint32 intIfNum,
                                              L7_uint32 joinpruneIntvl);

/*********************************************************************
* @purpose  Gets the Join/Prune Interval for the specified interface
*
* @param    familyType      @b{(input)}  Address Family type
* @param    intIfNum        @b{(input)}  Internal Interface Number
* @param    joinpruneIntvl  @b{(output)}  Join/Prune Interval in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapInterfaceJoinPruneIntervalGet(L7_uchar8 familyType,
                                              L7_uint32 intIfNum,
                                              L7_uint32 *joinpruneIntvl);

/*********************************************************************
* @purpose  Sets the Candidate BSR Preference for the specified interface
*
* @param    familyType     @b{(input)}  Address Family type
* @param    intIfNum       @b{(input)}  Internal Interface Number
* @param    candBSRPref    @b{(input)}  Candidate BSR Preference
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Range checking done here
*
* @end
*********************************************************************/
L7_RC_t pimsmMapInterfaceCBSRPreferenceSet(L7_uchar8 familyType,
                                           L7_uint32 intIfNum,
                                           L7_int32 candBSRPref);

/*********************************************************************
* @purpose  Gets the Candidate BSR Preference for the specified interface
*
* @param    familyType    @b{(input)}   Address Family type
* @param    intIfNum      @b{(input)}   Interface Number
* @param    candBSRPref   @b{(input)}   Candidate BSR Preference
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapInterfaceCBSRPreferenceGet(L7_uchar8 familyType,
                                           L7_uint32 intIfNum,
                                           L7_int32 *candBSRPref);

/*********************************************************************
* @purpose  Sets the Candidate BSR Hash mask Length for the 
*           specified interface
*
* @param    familyType    @b{(input)}   Address Family type
* @param    intIfNum      @b{(input)}   Internal Interface Number
* @param    hashMskLen    @b{(input)}   Candidate BSR Hash Mask Length
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Range checking done here
*
* @end
*********************************************************************/
L7_RC_t pimsmMapInterfaceCBSRHashMaskLengthSet(L7_uchar8 familyType,
                                               L7_uint32 intIfNum, 
                                               L7_uint32 hashMskLen);
/*********************************************************************
* @purpose  Gets the Candidate BSR Hash mask Length for the specified 
*           interface.
*
* @param    familyType  @b{(input)}  Address Family type
* @param    intIfNum    @b{(input)}  Interface Number
* @param    hashMskLen  @b{(output)} Candidate BSR Hash mask Length
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapInterfaceCBSRHashMaskLengthGet(L7_uchar8 familyType, 
                             L7_uint32 intIfNum, L7_uint32 *hashMskLen);

/*********************************************************************
* @purpose  Gets the Elected BSR Hash mask Length for the specified 
*           interface
*
* @param    familyType     @b{(input)}  Address Family type
* @param    hashMskLen     @b{(output)} Elected BSR Hash mask Length
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapElectedBSRHashMaskLengthGet(L7_uchar8 familyType,
                                            L7_uint32 *hashMskLen);

/*********************************************************************
* @purpose  Sets the Candidate CRP Preference for the specified interface
*
* @param    familyType    @b{(input)}   Address Family type
* @param    intIfNum      @b{(input)}   Internal Interface Number
* @param    candRPPref    @b{(input)}   Candidate RP Preference
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Range checking done here
*
* @end
*********************************************************************/
L7_RC_t pimsmMapInterfaceCRPPreferenceSet(L7_uchar8 familyType,
                                          L7_uint32 intIfNum,
                                          L7_int32 candRPPref);

/*********************************************************************
* @purpose  Gets the Candidate RP Preference for the specified interface
*
* @param    familyType   @b{(input)}     Address Family type
* @param    intIfNum     @b{(input)}     Interface Number
* @param    candRPPref   @b{(input)}     Candidate RP Preference
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapInterfaceCRPPreferenceGet(L7_uchar8 familyType,
                                          L7_uint32 intIfNum,
                                          L7_int32 *candRPPref);

/*********************************************************************
* @purpose  Gets the IP Address for the specified interface
*
* @param    familyType  @b{(input)}  Address Family type
* @param    intIfNum    @b{(input)}  Interface Number
* @param    inetIpAddr  @b{(output)} IP Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapInterfaceIPAddressGet(L7_uchar8 familyType,
                                      L7_uint32 intIfNum,
                                      L7_inet_addr_t *inetIpAddr);

/*********************************************************************
* @purpose  Gets the Subnet Mask for the specified interface
*
* @param    familyType  @b{(input)}  Address Family type
* @param    intIfNum    @b{(input)}  Interface Number
* @param    netMask     @b{(output)} Subnet Mask
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapInterfaceNetMaskGet(L7_uchar8 familyType,
                                    L7_uint32 intIfNum,
                                    L7_inet_addr_t *inetNetMask);

/*********************************************************************
* @purpose  Get the DR IP address for the specified specified interface
*
* @param    familyType        @b{(input)}    Address Family type
* @param    intIfNum          @b{(input)}   Internal Interface Number
* @param    dsgRtrIpAddr      @b{(output)}   L7_uint32   IP Address of the DR
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapInterfaceDRGet(L7_uchar8 familyType,
                               L7_uint32 intIfNum,
                               L7_inet_addr_t *dsgRtrIpAddr);

/*********************************************************************
* @purpose  Get the Uptime for the specified neighbour
* 
* @param    familyType   @b{(input)}   Address Family type
* @param    intIfNum     @b{(input)}   Internal Interface Number
* @param    nbrIpAddr    @b{(input)}   IP Address of Neighbor
* @param    nbrUpTime    @b{(output)}  Uptime in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapNeighborUpTimeGet(L7_uchar8 familyType,
          L7_uint32 intIfNum, L7_inet_addr_t *nbrIpAddr,
          L7_uint32 *nbrUpTime);

/*********************************************************************
* @purpose  Get the Expiry time for the specified neighbour
*
* @param    familyType    @b{(input)}    Address Family type
* @param    intIfNum      @b{(input)}    Internal Interface Number
* @param    nbrIpAddr     @b{(input)}    IP Address of Neighbor
* @param    nbrExpiryTime @b{(output)}   Expiry time in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapNeighborExpiryTimeGet(L7_uchar8 familyType,
               L7_uint32 intIfNum, L7_inet_addr_t *nbrIpAddr,
               L7_uint32 *nbrExpiryTime);

/*********************************************************************
* @purpose  Get the DR Priority for the specified neighbour
*
* @param    familyType  @b{(input)}    Address Family type 
* @param    intIfNum    @b{(input)}    Internal Interface Number
* @param    nbrIpAddr   @b{(input)}    IP Address of Neighbor
* @param    drPriority  @b{(output)}    DR Priority.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapNeighborDrPriorityGet(L7_uchar8 familyType,
                                      L7_uint32 intIfNum,
                                      L7_inet_addr_t *nbrIpAddr,
                                      L7_uint32 *drPriority);


/*********************************************************************
* @purpose  Get time remaining before the router changes its upstream
*           neighbor back to its RPF neighbor
*
* @param    familyType         @b{(input)}   Address Family type
* @param    ipMRouteGroup      @b{(input)}   IP multicast group address
* @param    ipMRouteSource     @b{(input)}   Network Address
* @param    ipMRouteSourceMask @b{(input)}   Network Mask
* @param    assertTimer        @b{(output)}  Assert timer in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapIpMRouteUpstreamAssertTimerGet(L7_uchar8 familyType, 
                                               L7_inet_addr_t *ipMRouteGroup,
                                               L7_inet_addr_t *ipMRouteSource, 
                                               L7_inet_addr_t *ipMRouteSourceMask,
                                               L7_uint32 *assertTimer);

/*********************************************************************
* @purpose  Get metric advertised by the assert winner on the upstream
*           interface, or 0 if no such assert is in received
*
* @param    familyType          @b{(input)}  Address Family type
* @param    ipMRouteGroup	    @b{(input)}  IP multicast group address
* @param    ipMRouteSource	    @b{(input)}  Network Address
* @param    ipMRouteSourceMask  @b{(input)}  Network Mask
* @param    assertMetric        @b{(output)} Metric value advertised
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapIpMRouteAssertMetricGet(L7_uchar8 familyType, 
                                        L7_inet_addr_t *ipMRouteGroup,
                                        L7_inet_addr_t *ipMRouteSource, 
                                        L7_inet_addr_t *ipMRouteSourceMask,
                                        L7_uint32 *assertMetric);

/*********************************************************************
* @purpose  Get preference advertised by the assert winner on the upstream
*           interface, or 0 if no such assert is in received
*
* @param    familyType          @b{(input)}  Address Family type
* @param    ipMRouteGroup       @b{(input)}  IP multicast group address
* @param    ipMRouteSource	    @b{(input)}  Network Address
* @param    ipMRouteSourceMask  @b{(input)}  Network Mask
* @param    assertMetricPref    @b{(output)} Metric preference advertised
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapIpMRouteAssertMetricPrefGet(L7_uchar8 familyType,
     L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource,
     L7_inet_addr_t *ipMRouteSourceMask, L7_uint32 *assertMetricPref);

/*********************************************************************
* @purpose  Get the value of the RPT-bit advertised by the assert winner on
*           the upstream interface, or false if no such assert is in effect
*
* @param    familyType          @b{(input)}  Address Family type
* @param    ipMRouteGroup       @b{(input)}  IP multicast group address
* @param    ipMRouteSource	    @b{(input)}  Network Address
* @param    ipMRouteSourceMask  @b{(input)}  Network Mask
* @param    assertRPTBit        @b{(output)} Value of RPT bit (1 or 0)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapIpMRouteAssertRPTBitGet(L7_uchar8 familyType, 
                                        L7_inet_addr_t *ipMRouteGroup,
                                        L7_inet_addr_t *ipMRouteSource, 
                                        L7_inet_addr_t *ipMRouteSourceMask,
                                        L7_uint32 *assertRPTBit);

/*********************************************************************
* @purpose  Get PIM-specific flags related to a multicast state entry
*
* @param    familyType         @b{(input)} Address Family type
* @param    ipMRouteGroup      @b{(input)} IP multicast group address
* @param    ipMRouteSource     @b{(input)} Network Address
* @param    ipMRouteSourceMask @b{(input)} Network Mask
* @param    ipMRouteFlags      @b{(output)}Flag value is 0 or 1
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @comments Flag is L7_MCAST_PIMSM_FLAG_RPT (0) for RP shared tree, 
*                   L7_MCAST_PIMSM_FLAG_SPT (1) for source tree
*
* @end
*********************************************************************/
L7_RC_t pimsmMapIpMRouteFlagsGet(L7_uchar8 familyType, 
                                 L7_inet_addr_t *ipMRouteGroup,
                                 L7_inet_addr_t *ipMRouteSource, 
                                 L7_inet_addr_t *ipMRouteSourceMask,
                                 L7_uint32 *ipMRouteFlags);
/************************************************************************
* @purpose  Get the holdtime of a Candidate-RP.  If the local router is not
*           the BSR, this value is 0.
*
* @param    familyType        @b{(input)}  Address Family type
* @param    rpSetGroupAddress @b{(input)}  IP multicast group address
* @param    rpSetGroupMask    @b{(input)}  Multicast group address mask
* @param    rpSetAddress      @b{(input)}  IP address of the Candidate-RP
* @param    rpSetComponent    @b{(input)}  Number uniquely identifying the 
*                                          component
* @param    rpSetHoldTime     @b{(output)} Hold time of C-RP in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @comments none
*
* @end
************************************************************************/
L7_RC_t pimsmMapRPSetHoldTimeGet(L7_uchar8 familyType,
    L7_inet_addr_t *rpSetGroupAddress, L7_inet_addr_t *rpSetGroupMask,
    L7_inet_addr_t *rpSetAddress, L7_uint32 rpSetComponent,
    L7_uint32 *rpSetHoldTime);

/************************************************************************
* @purpose  Get the minimum time remaining before the Candidate-RP will be
*           declared down
*
* @param    familyType         @b{(input)} Address Family type
* @param    rpSetGroupAddress  @b{(input)} IP multicast group address
* @param    rpSetGroupMask     @b{(input)} Multicast group address mask
* @param    rpSetAddress       @b{(input)} IP address of the Candidate-RP
* @param    rpSetComponent     @b{(input)} Number uniquely identifying the 
*                                          component
* @param    rpSetExpiryTime    @b{(output)} Expiry time of C-RP in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
************************************************************************/
L7_RC_t pimsmMapRPSetExpiryTimeGet(L7_uchar8 familyType, 
    L7_inet_addr_t *rpSetGroupAddress, L7_inet_addr_t *rpSetGroupMask, 
    L7_inet_addr_t *rpSetAddress, L7_uint32 rpSetComponent, 
    L7_uint32 *rpSetExpiryTime);

/************************************************************************
* @purpose  Get the unicast address of the interface which will
*           be advertised as a Candidate RP
*
* @param    familyType         @b{(input)}  Address Family type
* @param    candRPGroupAddress @b{(input)}  Candidate RP Group Address
* @param    candRPGroupMask    @b{(input)}  Candidate RP Group Mask
* @param    candRPAddr         @b{(output)}  Candidate RP Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @comments none
*
* @end
************************************************************************/
L7_RC_t pimsmMapCandRPAddressGet(L7_uchar8 familyType,
    L7_inet_addr_t *candidateRPGroupAddress, L7_inet_addr_t *candidateRPGroupMask, 
    L7_inet_addr_t *candRPAddr);

/************************************************************************
* @purpose  Get IP address of the elected bootstrap router (BSR).
*
* @param    familyType       @b{(input)}    Address Family type
* @param    elecBSRAddress   @b{(output)}   Component BSR Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @comments none
*
* @end
************************************************************************/
L7_RC_t pimsmMapElectedBSRAddressGet(L7_uchar8 familyType, 
                                     L7_inet_addr_t *elecBSRAddress);

/************************************************************************
* @purpose  Get minimum time remaining before the bootstrap router in
*           the local domain will be declared down
*
* @param    familyType        @b{(input)}   Address Family type
* @param    compBSRExpiryTime @b{(output)}  Expiry time of BSR in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
************************************************************************/
L7_RC_t pimsmMapElectedBSRExpiryTimeGet(L7_uchar8 familyType,
                                        L7_uint32 *compBSRExpiryTime);

/************************************************************************
* @purpose  Get the Advertisement time of the elected BSR.
*
* @param    familyType       @b{(input)}  Address Family type
* @param    elecCRPAdvTime   @b{(input)}  Hold time of component when it is C-RP
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @comments none
*
* @end
************************************************************************/
L7_RC_t pimsmMapElectedCRPAdvTimeGet(L7_uchar8 familyType,
                                     L7_uint32 *elecCRPAdvTime);

/*********************************************************************
* @purpose  Check whether interface entry exists for the specified interface
*
* @param    familyType  @b{(input)} Address Family type
* @param    intIfNum    @b{(input)} Internal Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapInterfaceEntryGet(L7_uchar8 familyType,
                                  L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Obtain the next sequential routing interface entry
*           after the entry of the specified interface
*
* @param    familyType   @b{(input)} Address Family type
* @param    pIntIfNum    @b{(inout)} Internal Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments next interface entry
*
* @end
*********************************************************************/
L7_RC_t pimsmMapInterfaceEntryNextGet(L7_uchar8 familyType, 
                                      L7_uint32 *pIntIfNum);
/*********************************************************************
* @purpose  Check whether neighbor entry exists for the specified IP address
*
* @param    familyType  @b{(input)} Address Family type
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param    ipAddress   @b{(inout)} IP Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapNeighborEntryGet(L7_uchar8 familyType,
                                 L7_uint32 intIfNum, 
                                 L7_inet_addr_t *ipAddress);

/*********************************************************************
* @purpose  Obtain the next sequential neighbor entry after the entry of the
*           specified neighbor
*
* @param    familyType    @b{(input)}  Address Family type
* @param    pIntIfNum      @b{(input)}  Internal Interface Number
* @param    ipAddress     @b{(inout)} IP Address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapNeighborEntryNextGet(L7_uchar8 familyType,
       L7_uint32 *pIntIfNum, L7_inet_addr_t *ipAddress);


/*********************************************************************
* @purpose  Check whether IP Multicast route entry exists for the specified
*           group, source and mask
*
* @param    familyType          @b{(input)} Address Family type
* @param    ipMRouteGroup       @b{(input)} IP Multicast route group
* @param    ipMRouteSource      @b{(input)} IP Multicast route source
* @param    ipMRouteSourceMask  @b{(input)} IP Multicast route mask
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapIpMRouteEntryGet(L7_uchar8 familyType,
    L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource, 
    L7_inet_addr_t *ipMRouteSourceMask);

/*********************************************************************
* @purpose  Get the existing IP Multicast Route entry next to the entry of the
*           specified group, source and mask
*
* @param    familyType          @b{(input)}  Address Family type
* @param    ipMRouteGroup       @b{(inout)} IP Multicast route group
* @param    ipMRouteSource      @b{(inout)} IP Multicast route source
* @param    ipMRouteSourceMask  @b{(inout)} IP Multicast route mask
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapIpMRouteEntryNextGet(L7_uchar8 familyType,
    L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource, 
    L7_inet_addr_t *ipMRouteSourceMask);

/*********************************************************************
* @purpose  Check whether PIMSM routing entry exists for the specified
*           group, source and mask
*
* @param    familyType           @b{(input)}  Address Family type
* @param    ipMRouteGroup        @b{(input)}  IP Multicast route group
* @param    ipMRouteSource       @b{(input)}  IP Multicast route source
* @param    ipMRouteSourceMask   @b{(input)}  IP Multicast route source mask
*                                             (source mask not being used)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapRouteEntryGet(L7_uchar8 familyType, L7_inet_addr_t *ipMRouteGroup, 
    L7_inet_addr_t *ipMRouteSource, L7_inet_addr_t *ipMRouteSourceMask);

/*********************************************************************
* @purpose  Get the existing PIMSM routing entry next to the entry of the
*           specified group, source and mask
*
* @param    familyType           @b{(input)}  Address Family type
* @param    ipMRouteGroup        @b{(inout)}  IP Multicast route group
* @param    ipMRouteSource       @b{(inout)}  IP Multicast route source
* @param    ipMRouteSourceMask   @b{(inout)}   IP Multicast route source mask
*                                              (source mask not being used)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapRouteEntryNextGet(L7_uchar8 familyType, L7_inet_addr_t *ipMRouteGroup, 
    L7_inet_addr_t *ipMRouteSource, L7_inet_addr_t *ipMRouteSourceMask);

/*********************************************************************
* @purpose  Get incoming interface of the routing entry
*
* @param    familyType          @b{(input)}  Address Family type
* @param    ipMRouteGroup       @b{(input)}  IP multicast group address
* @param    ipMRouteSource      @b{(input)}  Network Address
* @param    ipMRouteSourceMask  @b{(input)}  Network Mask (currently not used)
* @param    inIfIndex           @b{(output)} Internal Interface Number 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapIpMRouteIfIndexGet(L7_uchar8 familyType,
    L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource, 
    L7_inet_addr_t *ipMRouteSourceMask, L7_uint32 *inIfIndex);

/*********************************************************************
* @purpose  Get expiry time of the routing entry
*
* @param    familyType         @b{(input)}     Address Family type
* @param    ipMRouteGroup      @b{(input)}     IP multicast group address
* @param    ipMRouteSource     @b{(input)}     Network Address
* @param    ipMRouteSourceMask @b{(input)}     Network Mask (currently not used)
* @param    expiryTime         @b{(output)}    Expiry time (in seconds)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapIpMRouteExpiryTimeGet(L7_uchar8 familyType, 
                                   L7_inet_addr_t *ipMRouteGroup, 
                                   L7_inet_addr_t *ipMRouteSource, 
                                   L7_inet_addr_t *ipMRouteSourceMask, 
                                   L7_uint32 *expiryTime);
/*********************************************************************
* @purpose  Get up time of the routing entry
*
* @param    familyType         @b{(input)} Address Family type
* @param    ipMRouteGroup      @b{(input)} IP multicast group address
* @param    ipMRouteSource     @b{(input)} Network Address
* @param    ipMRouteSourceMask @b{(input)} Network Mask (currently not used)
* @param    upTime             @b{(output)} Up time (in seconds)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapIpMRouteUpTimeGet(L7_uchar8 familyType, 
                               L7_inet_addr_t *ipMRouteGroup, 
                               L7_inet_addr_t *ipMRouteSource, 
                               L7_inet_addr_t *ipMRouteSourceMask, 
                               L7_uint32 *upTime);


/*********************************************************************
* @purpose  Get expiry time of the routing entry
*
* @param    familyType         @b{(input)}     Address Family type
* @param    ipMRouteGroup      @b{(input)}     IP multicast group address
* @param    ipMRouteSource     @b{(input)}     Network Address
* @param    ipMRouteSourceMask @b{(input)}     Network Mask (currently not used)
* @param    expiryTime         @b{(output)}    Expiry time (in seconds)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapRouteExpiryTimeGet(L7_uchar8 familyType, 
                                   L7_inet_addr_t *ipMRouteGroup, 
                                   L7_inet_addr_t *ipMRouteSource, 
                                   L7_inet_addr_t *ipMRouteSourceMask, 
                                   L7_uint32 *expiryTime);

/*********************************************************************
* @purpose  Get up time of the routing entry
*
* @param    familyType         @b{(input)} Address Family type
* @param    ipMRouteGroup      @b{(input)} IP multicast group address
* @param    ipMRouteSource     @b{(input)} Network Address
* @param    ipMRouteSourceMask @b{(input)} Network Mask (currently not used)
* @param    upTime             @b{(output)} Up time (in seconds)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapRouteUpTimeGet(L7_uchar8 familyType, 
                               L7_inet_addr_t *ipMRouteGroup, 
                               L7_inet_addr_t *ipMRouteSource, 
                               L7_inet_addr_t *ipMRouteSourceMask, 
                               L7_uint32 *upTime);

/*********************************************************************
* @purpose  Get RPF address of the routing entry
*
* @param    familyType          @b{(input)}  Address Family type
* @param    ipMRouteGroup       @b{(input)}  IP multicast group address
* @param    ipMRouteSource      @b{(input)}  Network Address
* @param    ipMRouteSourceMask  @b{(input)}  Network Mask (currently not used)
* @param    rpfAddr             @b{(output)} Address of RPF Neighbor
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapRouteRpfAddrGet(L7_uchar8 familyType, 
    L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource, 
    L7_inet_addr_t *ipMRouteSourceMask, L7_inet_addr_t *rpfAddr);

/*********************************************************************
* @purpose  Get flags of the routing entry
*
* @param    familyType           @b{(input)} Address Family type
* @param    ipMRouteGroup        @b{(input)} IP multicast group address
* @param    ipMRouteSource       @b{(input)} Network Address
* @param    ipMRouteSourceMask   @b{(input)} Network Mask (currently not used)
* @param    flags                @b{(output)}RPT or SPT flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapRouteFlagsGet(L7_uchar8 familyType,
    L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource, 
    L7_inet_addr_t *ipMRouteSourceMask, 
    L7_uint32 *flags);

/*********************************************************************
* @purpose  Get incoming interface of the routing entry
*
* @param    familyType          @b{(input)}  Address Family type
* @param    ipMRouteGroup       @b{(input)}  IP multicast group address
* @param    ipMRouteSource      @b{(input)}  Network Address
* @param    ipMRouteSourceMask  @b{(input)}  Network Mask (currently not used)
* @param    inIfIndex           @b{(output)} Internal Interface Number 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapRouteIfIndexGet(L7_uchar8 familyType,
    L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource, 
    L7_inet_addr_t *ipMRouteSourceMask, L7_uint32 *inIfIndex);

/*****************************************************************
*
* @purpose  Gets outgoing interfaces for the given source address,
*           group address and source mask.
*
* @param    familyType           @b{(input)}    Address Family type
* @param    ipMRouteGroup        @b{(input)}    Multicast Group Address
* @param    ipMRouteSource       @b{(input)}    Multicast Source Address
* @param    ipMRouteSourceMask   @b{(input)}    Multicast Source Address Mask
* @param    outIntIfNum          @b{(output)}   Outgoing Internal 
*                                               Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*****************************************************************/
L7_RC_t pimsmMapRouteOutIntfEntryNextGet(L7_uchar8 familyType,
    L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource,
    L7_inet_addr_t *ipMRouteSourceMask, L7_uint32 *outIntIfNum);

/***************************************************************
* @purpose  Gets number of forwarded packets for the given Index
*           through the router
*
* @param     familyType           @b{(input)}    Address Family type
* @param     pMRouteGroup        @b{(input)}    Multicast Group Address
* @param    ipMRouteSource       @b{(input)}    Multicast Source Address
* @param    ipMRouteSourceMask   @b{(input)}    Multicast Source Address Mask
* @param    numOfPackets         @b{(output)}   Number of Forwarded Packets
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @comments none
*
* @end
*
***************************************************************/
L7_RC_t pimsmMapRoutePktsGet(L7_uchar8 familyType, 
    L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource,
    L7_inet_addr_t *ipMRouteSourceMask, L7_uint32 *numOfPackets);

/*********************************************************************
* @purpose  Check whether IP Multicast route next hop entry exists for
*           the specified group, source, mask, interface index and address
*
* @param    familyType                @b{(input)}  Address Family type
* @param    ipMRouteNextHopGroup      @b{(input)}  IP Multicast route next 
*                                                  hop group
* @param    ipMRouteNextHopSource     @b{(input)}  IP Multicast route next hop 
*                                                  source
* @param    ipMRouteNextHopSourceMask @b{(input)}  IP Multicast route next hop 
*                                                  source mask
* @param    ipMRouteNextHopIfIndex    @b{(input)}  IP Multicast route next hop 
*                                                  interface index
* @param    ipMRouteNextHopAddress    @b{(output)}  IP Multicast route next hop 
*                                                  address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Next Hop table currently not supported
*
* @end
*********************************************************************/
L7_RC_t pimsmMapIpMRouteNextHopEntryGet(L7_uchar8 familyType,
   L7_inet_addr_t *ipMRouteNextHopGroup, L7_inet_addr_t *ipMRouteNextHopSource,
   L7_inet_addr_t *ipMRouteNextHopSourceMask, L7_uint32 ipMRouteNextHopIfIndex,
   L7_inet_addr_t *ipMRouteNextHopAddress);

/*********************************************************************
* @purpose  Get the existing IP Multicast route next hop entry next to the entry
*           of the specified group, source, mask, interface index and address
*
* @param    familyType                 @b{(input)}  Address Family type
* @param    ipMRouteNextHopGroup       @b{(inout)} IP Multicast route next 
*                                                   hop group
* @param    ipMRouteNextHopSource      @b{(inout)} IP Multicast route next 
*                                                   hop source
* @param    ipMRouteNextHopSourceMask  @b{(inout)} IP Multicast route next hop 
*                                                   source mask
* @param    ipMRouteNextHopIfIndex     @b{(inout)} IP Multicast route next hop 
*                                                  interface index
* @param    ipMRouteNextHopAddress     @b{(output)} IP Multicast route next hop address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Next Hop table currently not supported
*
* @end
*********************************************************************/
L7_RC_t pimsmMapIpMRouteNextHopEntryNextGet
                  (L7_uchar8 familyType, L7_inet_addr_t *ipMRouteNextHopGroup,
                   L7_inet_addr_t *ipMRouteNextHopSource, 
                   L7_inet_addr_t *ipMRouteNextHopSourceMask,
                   L7_uint32 *ipMRouteNextHopIfIndex, 
                   L7_inet_addr_t *ipMRouteNextHopAddress);

/*********************************************************************
* @purpose  Check whether RP Group entry exists for the specified
*           group address, group mask, address and component number
*
* @param    familyType       @b{(input)}  Address Family type
* @param    origin           @b{(input)}  Origin
* @param    rpGroupAddress   @b{(input)}  IP Multicast group address
* @param    rpGroupMask      @b{(input)}  Multicast group address mask
* @param    rpAddress        @b{(input)}  IP address of candidate RP
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapRPGroupEntryGet(L7_uchar8 familyType, 
    L7_uchar8 origin, L7_inet_addr_t *rpGroupAddress, 
    L7_inet_addr_t *rpGroupMask, L7_inet_addr_t *rpAddress);

/*********************************************************************
* @purpose  Get the existing Group RP entry next to the entry of 
*           the specified group address, group mask, 
*           address.
*
* @param    familyType     @b{(input)} Address Family type
* @param    origin         @b{(inout)} Origin
* @param    groupAddress   @b{(inout)} IP Multicast group address
* @param    groupMask      @b{(inout)} Multicast group address mask
* @param    rpAddress      @b{(inout)} IP address of candidate RP
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapRpGroupEntryNextGet(L7_uchar8 familyType, 
    L7_uchar8 *origin, L7_inet_addr_t *rpGroupAddress, 
    L7_inet_addr_t *rpGroupMask, L7_inet_addr_t *rpAddress);

/*********************************************************************
* @purpose  Check whether candidate RP entry exists for the specified 
*           group address and group mask.
*
* @param    familyType       @b{(input)}    Address Family type
* @param    candRpAddress    @b{(input)}    candidate Rp address.
* @param    cRPGroupAddress  @b{(input)}    candidate RP group address.
* @param    candPrefixLen    @b{(input)}    candidate prefix Length.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapCandRPEntryGet(L7_uint32 unitIndex,L7_uchar8 familyType,
    L7_inet_addr_t *candRpAddress, L7_inet_addr_t *candGroupAddress,
    L7_uchar8 candPrefixLen);

/*********************************************************************
* @purpose  Get the existing Cand-RP entry next to the entry of the specified
*           group address and group mask
*
* @param    familyType      @b{(input)}      Address Family type
* @param    candRpAddress   @b{(output)}     candidate Rp address.
* @param    cRPGroupAddress @b{(output)}     candidate RP group address.
* @param    candPrefixLen   @b{(output)}     candidate prefix Length.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments To get the first entry, candRpAddress, cRpGroupAddress, 
*           candPrefixLen should be zero.
*
* @end
*********************************************************************/
L7_RC_t pimsmMapCandRPEntryNextGet(L7_uchar8 familyType,
    L7_inet_addr_t *candRpAddress, L7_inet_addr_t *cRpGroupAddress, 
    L7_uchar8 *candPrefixLen);

/*********************************************************************
* @purpose  Get the existing elected BSR next entry. 
*
* @param    familyType   @b{(input)} Address Family type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapElectedBSREntryNextGet(L7_uchar8 familyType);

/******************************************
 *   Statistics Related Functions         *
 ******************************************/

/*********************************************************************
* @purpose  Get the Neighbor count for specified interface
*
* @param    familyType   @b{(input)}  Address Family type
* @param    intIfNum     @b{(input)}  Internal Interface Number
* @param    nbrCount     @b{(output)} Neighbor Count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapNeighborCountGet(L7_uchar8 familyType,
          L7_uint32 intIfNum, L7_uint32 *nbrCount);

/*********************************************************************
* @purpose  A pimNeighborLoss trap signifies the loss of an adjacency
*           with a neighbor.  This trap should be generated when the
*           neighbor timer expires, and the router has no other
*           neighbors on the same interface with a lower IP address than
*           itself.
*
* @param    familyType   @b{(input)} Address Family type
* @param    rtrIntNum    @b{(input)} Router interface on which neighbor is lost.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapPimTrapNeighborLoss(L7_uchar8 familyType, L7_uint32 rtrIntNum, L7_uint32 neighborUpTime);


/*********************************************************************
* @purpose  Check whether specified internal interface number exists
*
* @param    familyType  @b{(input)}  Address Family type
* @param    intIfNum    @b{(input)}  Internal Interface Number

*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSNMPInterfaceEntryGet(L7_uchar8 familyType, 
                                      L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Obtain the next sequential routing interface entry
*           after the entry of the specified interface
*
* @param    familyType  @b{(input)} Address Family type
* @param    pIntIfNum   @b{(inout)} Internal Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments next interface entry
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSNMPInterfaceEntryNextGet(L7_uchar8 familyType,
                                          L7_uint32 *pIntIfNum);

/*********************************************************************
* @purpose  Obtain the RP that the specified group will be mapped to.
*
* @param    familyType     @b{(input)} Address Family type
* @param    grpIpAddr      @b{(inout)} Group ip address
* @param    rpIpAddr       @b{(inout)} RP ip address
* @param    origin         @b{(inout)} Origin
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t pimsmMapGroupToRPMappingGet(L7_uchar8 familyType,
    L7_inet_addr_t *grpIpAddr, L7_inet_addr_t *rpIpAddr, 
    L7_uchar8 *origin);

/*********************************************************************
* @purpose  Determine if the interface type is valid in PIMSM
*
* @param    sysIntfType  @b{(input)}  interface type
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none
*       
* @end
*********************************************************************/
L7_BOOL pimsmIntfIsValidType(L7_uint32 sysIntfType);

/*********************************************************************
* @purpose  Determine if the interface is valid in PIMSM
*
* @param    intIfNum  @b{(input)} internal interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none
*       
* @end
*********************************************************************/
L7_BOOL pimsmIntfIsValid(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Sets the BSR Border for the specified interface
*
* @param    familyType    @b{(input)}   Address Family type
* @param    intIfNum      @b{(input)}   Internal Interface Number
* @param    mode          @b{(input)}   Mode (ENABLE/DISABLE)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t pimsmMapInterfaceBsrBorderSet(L7_uchar8 familyType,
               L7_uint32 intIfNum, L7_uint32 mode);

/*********************************************************************
* @purpose  Gets the BSR Border for the specified interface
*
* @param    familyType     @b{(input)}  Address Family type
* @param    intIfNum       @b{(input)}  Internal Interface Number
* @param    bsrBorder      @b{(output)}  BSR Border (ENABLE/DISABLE)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*********************************************************************/

L7_RC_t pimsmMapInterfaceBsrBorderGet(L7_uchar8 familyType,
              L7_uint32 intIfNum, L7_uint32 *bsrBorder);

/*********************************************************************
* @purpose  Sets the DR Priroity for the specified interface 
*
* @param    familyType   @b{(input)}  Address Family type
* @param    intIfNum     @b{(input)}  Internal Interface Number
* @param    priority     @b{(output)} Designated Router priority.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Range checking done here
*
* @end
*********************************************************************/
L7_RC_t pimsmMapInterfaceDRPrioritySet(L7_uchar8 familyType,
          L7_uint32 intIfNum, L7_uint32 priority);

/*********************************************************************
* @purpose  Sets the RP candidate
*
* @param    familyType   @b{(input)}        Address Family type
* @param    intIfNum     @b{(input)}        interface Number.
* @param    rpGrpAddr    @b{(input)}        Group address supported by the RP
* @param    rpGrpMask    @b{(input)}        Group mask for the group address
* @param    mode         @b{(input)}        mode (ENABLE/DISABLE).
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t pimsmMapCandRPSet(L7_uchar8 familyType,
                               L7_uint32 intIfNum, L7_inet_addr_t *rpGrpAddr, 
                               L7_inet_addr_t *rpGrpMask, L7_uint32 mode);
/*********************************************************************
* @purpose  Gets the interface on which Candidate Rp is configured
*
* @param    familyType   @b{(input)}        Address Family type
* @param    intIfNum     @b{(output)}       interface Number.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t pimsmMapCandRPConfiguredIntfGet(L7_uchar8 familyType, L7_uint32 *intIfNum);

/*********************************************************************
* @purpose  Gets the Rp Candidate admin mode.
*
* @param    familyType    @b{(input)}       Address Family type
* @param    candRPAddr    @b{(input)}       Candidate Addr
* @param    candRPGrpAddr @b{(input)}       Candidate RP Grp Addr
* @param    prefixLen     @b{(input)}       prefix length of the address
* @param    mode          @b{(output)}      L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapCandRPModeGet(L7_uchar8 familyType,
                                   L7_inet_addr_t *candRPAddr,
                                   L7_inet_addr_t *candRPGrpAddr, 
                                   L7_uint32 prefixLen, L7_uint32 *mode);

/*********************************************************************
* @purpose  Gets the Rp Candidate interface number.
*
* @param    familyType    @b{(input)}       Address Family type
* @param    candRPAddr    @b{(input)}       Candidate Addr
* @param    candRPGrpAddr @b{(input)}       Candidate RP Grp Addr
* @param    prefixLen     @b{(input)}       prefix length of the address
* @param    intIfNum      @b{(output)}      interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapCandRPInterfaceGet(L7_uchar8 familyType,
                                   L7_inet_addr_t *candRPAddr,
                                   L7_inet_addr_t *candRPGrpAddr, 
                                   L7_uint32 prefixLen, L7_uint32 *intIfNum);

/*********************************************************************
* @purpose  Get the Ip Mroute Rt Protocol used.
*
* @param    familyType         @b{(input)}  Address Family type
* @param    ipMRouteGroup      @b{(input)}  Group Address.
* @param    ipMRouteSource     @b{(input)}  Source Address.
* @param    ipMRouteSourceMask @b{(input)}  Source Mask.
* @param    rtProtocol         @b{(output)} routing protocol used
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapIpMRouteRtProtoGet(L7_uchar8 familyType,
    L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource, 
    L7_inet_addr_t  *ipMRouteSourceMask, L7_uint32*  rtProtocol);

/*********************************************************************
* @purpose  Get the Ip Mroute Rt Address.
*
* @param    familyType         @b{(input)}  Address Family type
* @param    ipMRouteGroup      @b{(input)}  Group Address.
* @param    ipMRouteSource     @b{(input)}  Source Address.
* @param    ipMRouteSourceMask @b{(input)}  Source Mask.
* @param    rtAddr             @b{(output)}  rtAddr 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapIpMRouteRtAddressGet(L7_uchar8 familyType,
    L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource, 
    L7_inet_addr_t  *ipMRouteSourceMask, L7_inet_addr_t*  rtAddr);

/*********************************************************************
* @purpose  Get the Ip Mroute Rt Mask.
*
* @param    familyType         @b{(input)}  Address Family type
* @param    ipMRouteGroup      @b{(input)}  Group Address.
* @param    ipMRouteSource     @b{(input)}  Source Address.
* @param    ipMRouteSourceMask @b{(input)}  Source Mask.
* @param    rtMask             @b{(output)} rtMask
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapIpMRouteRtMaskGet(L7_uchar8 familyType, 
    L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource, 
    L7_inet_addr_t  *ipMRouteSourceMask, L7_inet_addr_t*  rtMask);

/*********************************************************************
* @purpose  Get the Ip Mroute Rt Type.
*
* @param    familyType          @b{(input)} Address Family type
* @param    ipMRouteGroup       @b{(input)} Group Address.
* @param    ipMRouteSource      @b{(input)} Source Address.
* @param    ipMRouteSourceMask  @b{(input)} Source Mask.
* @param    rtType              @b{(output)} rtType
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapIpMRouteRtTypeGet(L7_uchar8 familyType,
    L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource, 
    L7_inet_addr_t  *ipMRouteSourceMask, L7_uint32 *rtType);

/*********************************************************************
* @purpose  Get the Ip Mroute Rpf Address.
*
* @param    familyType         @b{(input)}    Address Family type
* @param    ipMRouteGroup      @b{(input)}    Group Address.
* @param    ipMRouteSource     @b{(input)}    Source Address.
* @param    ipMRouteSourceMask @b{(input)}    Source Mask.
* @param    rpfAddr            @b{(output)}   rpfAddr
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmMapIpMRouteRpfAddrGet(L7_uchar8 familyType,
    L7_inet_addr_t *ipMRouteGroup, L7_inet_addr_t *ipMRouteSource, 
    L7_inet_addr_t  *ipMRouteSourceMask, L7_inet_addr_t *rpfAddr);

/*********************************************************************
* @purpose  Get the Ip Mroute Entry Count Get.
*
* @param    familyType   @b{(input)}        Address Family type
* @param    entryCount   @b{(output)}       entryCount
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapIpMRouteEntryCountGet(L7_uchar8 familyType,
                                      L7_uint32 *entryCount);

/*********************************************************************
* @purpose  Get the Ip Mroute Highest Entry Count Get.
*
* @param    familyType   @b{(input)}        Address Family type
* @param    heCount      @b{(output)}       entryCount
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapIpMRouteHighestEntryCountGet(L7_uchar8 familyType,
                                             L7_uint32 *heCount);


/*********************************************************************
* @purpose  Sets the BSR candidate
*
* @param    familyType  @b{(input)}         Address Family type
* @param    intIfNum    @b{(input)}         Interface Number.
* @param    priority    @b{(inout)}         Priority
* @param    mode        @b{(inout)}         mode (ENABLE/DISABLE).
* @param    length      @b{(inout)}         maskLen
* @param    scope       @b{(inout)}         scope Value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapCandBSRSet(L7_uchar8 familyType, L7_uint32 intIfNum,
    L7_uint32 priority, L7_uint32 mode, 
    L7_uint32 maskLen, L7_uint32 scope);

/*********************************************************************
* @purpose  Gets the  DR Priority for the specified interface.
*
* @param    familyType   @b{(input)}     Address Family type
* @param    intIfNum     @b{(input)}     Internal Interface Number
* @param    drPriority   @b{(output)}     DR Prioirity
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapInterfaceDRPriorityGet(L7_uchar8 familyType, 
        L7_uint32 intIfNum, L7_uint32 *drPriority);

/*********************************************************************
* @purpose  Gets the Elected BSR Priority. 
*
* @param    familyType       @b{(input)}  Address Family type
* @param    elecBsrPriority  @b{(output)} Elected BSR Priority
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapElectedBSRPriorityGet(L7_uchar8 familyType,
                                      L7_int32 *elecBsrPriority);

/*********************************************************************
* @purpose  Send event to PIMSM Vendor
*
* @param    familyType   @b{(input)}        Address Family type
* @param    eventType    @b{(input)}        Event Type 
* @param    msgLen       @b{(input)}        Message Length
* @param    eventMsg     @b{(input)}        Event Message
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmMapEventChangeNotify(L7_uchar8 familyType, L7_uint32  eventType, 
                                  L7_uint32  msgLen, void *eventMsg);


/******************************************************************************
* @purpose	Get routine for DR-priority
*
* @param	familyType      @b{(input)}   Address Family type.
* @param        rtrIfNum        @b{(input)}   pointer to router interface number
* @param        drPriority      @b{(output)}  DR-priority	
*
* @returns	L7_SUCCESS
* @returns	L7_FAILURE
*
* @comments 	 
*		
* @end
******************************************************************************/
L7_RC_t pimsmMapProtocolInterfaceDRPriorityGet( L7_uchar8 familyType, 
                  L7_uint32 rtrIfNum,L7_uint32 *drPriority);


/******************************************************************************
* @purpose	Get the bsr border on an interface
*
* @param    familyType        @b{(input)}  Address Family type.
* @param    rtrIfNum	      @b{(input)}  pointer to router interface number
* @param    bsrborder	      @b{(output)} bsr border
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none	 
*		
* @end
******************************************************************************/
L7_RC_t pimsmMapProtocolInterfaceBsrBorderGet(L7_uchar8 familyType, 
                           L7_uint32 rtrIfNum, L7_uint32 *bsrBorder);

/*********************************************************************
* @purpose  Check whether candidateBSR entry exists.
*
* @param    familyType       @b{(input)}  Address Family type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapCandBSREntryGet(L7_uchar8 familyType);

/*********************************************************************
* @purpose  Obtain the next entry in the bsrCandidateRP TABLE
*
* @param    familyType       @b{(input)}  Address Family type
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @notes    next interface entry
*
* @end
*********************************************************************/
L7_RC_t pimsmMapCandBSREntryNextGet(L7_uchar8 familyType);

/*********************************************************************
* @purpose  To get the Candidate BSR interface.
*
* @param    familyType       @b{(input)}  Address Family type
* @param    interface        @b{(output)} interface number.
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapCandBSRInterfaceGet(L7_uchar8 familyType, 
                                         L7_uint32 *interface);
/*********************************************************************
* @purpose  To get the Candidate BSR Address.
*
* @param    familyType       @b{(input)}  Address Family type
* @param    *cbsrAddr        @b{(output)} To get bsr Addr
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapCandBSRAddrGet(L7_uchar8 familyType, 
                                    L7_inet_addr_t *cbsrAddr);

/*********************************************************************
* @purpose  To set the candidate BSR Address.
*
* @param    familyType       @b{(input)}  Address Family type
* @param    *cbsrAddr        @b{(input)} bsr Addr
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapCandBSRAddrSet(L7_uchar8 familyType, 
                                    L7_inet_addr_t *cbsrAddr);

/*********************************************************************
* @purpose  To get the Candidate BSR priority.
*
* @param    familyType       @b{(input)}  Address Family type
* @param    *cbsrPriority    @b{(output)} To get bsr priority.
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapCandBSRPriorityGet(L7_uchar8 familyType, 
                                        L7_uint32 *cbsrPriority);
/*********************************************************************
* @purpose  To set the bsrCandtBSR priority.
*
* @param    familyType       @b{(input)}  Address Family type
* @param    cbsrPriority     @b{(input)}  bsr priority.
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapCandBSRPrioritySet(L7_uchar8 familyType, 
                                        L7_uint32 cbsrPriority);

/*********************************************************************
* @purpose  To get the bsrCandtBSR hashMaskLen.
*
* @param    familyType       @b{(input)}  Address Family type
* @param    *hashMaskLen     @b{(output)} get Hash mask Length.
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapCandBSRHashMaskLengthGet(L7_uchar8 familyType, 
                                              L7_uint32 *hashMaskLen);

/*********************************************************************
* @purpose  To set the bsrCandtBSR hashMaskLen.
*
* @param    familyType       @b{(input)}  Address Family type
* @param    hashMaskLen      @b{(output)} get Hash mask Length.
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapCandBSRHashMaskLengthSet(L7_uchar8 familyType, 
                                              L7_uint32 hashMaskLen);

/*********************************************************************
* @purpose  To get the bsrCandtBSR electedBSR.
*
* @param    familyType       @b{(input)}  Address Family type
* @param    *electedBsr      @b{(output)} BSR is elected/not.
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapCandBSRElectedBSRGet(L7_uchar8 familyType, 
                                          L7_uint32 *electedBsr);

/*********************************************************************
* @purpose  To get the bsrCandidate Bootstrap Timer.
*
* @param    familyType       @b{(input)}  Address Family type
* @param    *bsTimer         @b{(output)} BootStap Timer.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapCandBSRBootstrapTimerGet(L7_uchar8 familyType, 
                                              L7_uint32 *bsTimer);

/*********************************************************************
* @purpose  Gets the admin mode for the specified interface
*
* @param    familyType       @b{(input)}  Address Family type
* @param    *mode            @b{(output)} get Hash mask Length.
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR      if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapCandBSRModeGet(L7_uchar8 familyType, 
                                    L7_uint32 *mode);

/*********************************************************************
* @purpose  Sets the admin mode for the BSR Candidate.
*
* @param    familyType       @b{(input)}  Address Family type
* @param    mode             @b{(input)}  BSR mode.
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR      if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapCandBSRModeSet(L7_uchar8 familyType, 
                                    L7_uint32 mode);


/*********************************************************************
* @purpose  To get the next entry in SG TABLE.
*
* @param    familyType    @b{(input)} Address Family type
* @param    pSGGrpAddr    @b{(inout)} Group Address
* @param    pSGSrcAddr    @b{(inout)} Source Address
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments    next interface entry
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGEntryNextGet(L7_uchar8 familyType,  
                               L7_inet_addr_t *pSGGrpAddr, 
                               L7_inet_addr_t *pSGSrcAddr);
/*********************************************************************
* @purpose  To get the interface in SG TABLE.
*
* @param    familyType    @b{(input)} Address Family type
* @param    pSGGrpAddr    @b{(inout)} Group Address
* @param    pSGSrcAddr    @b{(inout)} Source Address
* @param    pIntIfNum     @b{(inout)} Internal Interface Number
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments    next interface entry
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGIEntryNextGet(L7_uchar8 familyType,  
                               L7_inet_addr_t *pSGGrpAddr, 
                               L7_inet_addr_t *pSGSrcAddr,
                               L7_uint32      *pIntIfNum);

/*********************************************************************
* @purpose  To get the expiry time in SG TABLE.
*
* @param    familyType    @b{(input)} Address Family type
* @param    pSGGrpAddr    @b{(input)} Group Address
* @param    pSGSrcAddr    @b{(input)} Source Address
* @param    pExpiryTime   @b{(output)}expiry Time
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGExpiryTimeGet(L7_uchar8 familyType,
                                L7_inet_addr_t *pSGGrpAddr, 
                                L7_inet_addr_t *pSGSrcAddr,
                                L7_uint32      *pExpiryTime);

/*********************************************************************
* @purpose  To get the Up time in SG TABLE.
*
* @param    familyType    @b{(input)} Address Family type
* @param    pSGGrpAddr    @b{(input)} Group Address
* @param    pSGSrcAddr    @b{(input)} Source Address
* @param    pUpTime       @b{(output)}Up Time
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGUpTimeGet(L7_uchar8 familyType,
                            L7_inet_addr_t *pSGGrpAddr, 
                            L7_inet_addr_t *pSGSrcAddr,
                            L7_uint32      *pUpTime);

/*********************************************************************
* @purpose  To get the Rpf Address in SG TABLE.
*
* @param    familyType    @b{(input)} Address Family type
* @param    pSGGrpAddr    @b{(input)} Group Address
* @param    pSGSrcAddr    @b{(input)} Source Address
* @param    pRpfAddr      @b{(output)} Rpf Address
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGRpfAddrGet(L7_uchar8 familyType,
                             L7_inet_addr_t *pSGGrpAddr, 
                             L7_inet_addr_t *pSGSrcAddr,
                             L7_inet_addr_t *pRpfAddr);

/*********************************************************************
* @purpose  To get the Flags in SG TABLE.
*
* @param    familyType    @b{(input)} Address Family type
* @param    pSGGrpAddr    @b{(input)} Group Address
* @param    pSGSrcAddr    @b{(input)} Source Address
* @param    pSGFalgs      @b{(output)} Flags.
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGFlagsGet(L7_uchar8 familyType,
                           L7_inet_addr_t *pSGGrpAddr, 
                           L7_inet_addr_t *pSGSrcAddr,
                           L7_uint32      *pSGFlags);
/*********************************************************************
* @purpose  To get the If Index in SG TABLE.
*
* @param    familyType    @b{(input)} Address Family type
* @param    pSGGrpAddr    @b{(input)} Group Address
* @param    pSGSrcAddr    @b{(input)} Source Address
* @param    pSGIfIndex    @b{(output)} SG If Index.
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGIfIndexGet(L7_uchar8 familyType,
                             L7_inet_addr_t *pSGGrpAddr, 
                             L7_inet_addr_t *pSGSrcAddr,
                             L7_uint32      *pSGIfIndex);
/*********************************************************************
* @purpose  Obtain the next incoming interface in the Star G TABLE.
*
* @param    familyType    @b{(input)} Address Family type
* @param    pGrpAddr      @b{(inout)} Group Address
* @param    pIntIfNum     @b{(inout)} Internal Interface Number
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments    next interface entry
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarGIEntryNextGet(L7_uchar8 familyType,  
                                   L7_inet_addr_t *pGrpAddr,
                                   L7_uint32      *pIntIfNum);
/*********************************************************************
* @purpose  To get the expiry time in Star G TABLE.
*
* @param    familyType    @b{(input)} Address Family type
* @param    pGrpAddr      @b{(input)} Group Address
* @param    pExpiryTime   @b{(output)}expiry Time
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarGExpiryTimeGet(L7_uchar8 familyType,
                                   L7_inet_addr_t *pGrpAddr, 
                                   L7_uint32      *pExpiryTime);
/*********************************************************************
* @purpose  To get the Up time in Star G TABLE.
*
* @param    familyType    @b{(input)} Address Family type
* @param    pGrpAddr      @b{(input)} Group Address
* @param    pUpTime       @b{(output)}Up Time
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarGUpTimeGet(L7_uchar8 familyType,
                               L7_inet_addr_t *pGrpAddr, 
                               L7_uint32      *pUpTime);
/*********************************************************************
* @purpose  To get the Rpf Address in Star G TABLE.
*
* @param    familyType    @b{(input)} Address Family type
* @param    pGrpAddr      @b{(input)} Group Address
* @param    pRpfAddr      @b{(output)} Rpf Address
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarGRpfAddrGet(L7_uchar8 familyType,
                                L7_inet_addr_t *pGrpAddr, 
                                L7_inet_addr_t *pRpfAddr);
/*********************************************************************
* @purpose  To get the Flags in Star G TABLE.
*
* @param    familyType    @b{(input)}  Address Family type
* @param    pGrpAddr      @b{(input)}  Group Address
* @param    pFalgs        @b{(output)} Flags.
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarGFlagsGet(L7_uchar8 familyType,
                              L7_inet_addr_t *pGrpAddr, 
                              L7_uint32      *pFlags);

/*********************************************************************
* @purpose  To get the If Index in Star G TABLE.
*
* @param    familyType    @b{(input)} Address Family type
* @param    pGrpAddr      @b{(input)} Group Address
* @param    pIfIndex      @b{(output)} StarG If Index.
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarGIfIndexGet(L7_uchar8 familyType,
                                L7_inet_addr_t *pGrpAddr, 
                                L7_uint32      *pIfIndex);

#ifdef PIMSM_MAP_TBD
/*********************************************************************
* @purpose  To get the next entry in Star Star RP TABLE.
*
* @param    familyType    @b{(input)} Address Family type
* @param    pRpAddr       @b{(inout)} RP Address
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments    next interface entry
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarStarRPEntryNextGet(L7_uchar8 familyType,  
                                       L7_inet_addr_t *pRpAddr);
/*********************************************************************
* @purpose  Obtain the next incoming interface in the Star 
*           Star RP TABLE.
*
* @param    familyType    @b{(input)} Address Family type
* @param    pRpAddr       @b{(inout)} RP Address.
* @param    pIntIfNum     @b{(inout)} Internal Interface Number
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments    next interface entry
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarStarRPIEntryNextGet(L7_uchar8 familyType,  
                                        L7_inet_addr_t *pRpAddr,
                                        L7_uint32      *pIntIfNum);
/*********************************************************************
* @purpose  To get the expiry time in Star Star RP TABLE.
*
* @param    familyType    @b{(input)} Address Family type
* @param    pRpAddr       @b{(input)} RP Address
* @param    pExpiryTime   @b{(output)} expiry Time
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarStarRPExpiryTimeGet(L7_uchar8 familyType,
                                        L7_inet_addr_t *pRpAddr, 
                                        L7_uint32      *pExpiryTime);
/*********************************************************************
* @purpose  To get the Up time in Star Star RP TABLE.
*
* @param    familyType    @b{(input)} Address Family type
* @param    pRpAddr       @b{(input)} RP Address
* @param    pUpTime       @b{(output)} Up Time
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarStarRPUpTimeGet(L7_uchar8 familyType,
                                    L7_inet_addr_t *pRpAddr, 
                                    L7_uint32      *pUpTime);
/*********************************************************************
* @purpose  To get the Rpf Address in Star Star RP TABLE.
*
* @param    familyType    @b{(input)}  Address Family type
* @param    pRpAddr       @b{(input)}  RP Address
* @param    pRpfAddr      @b{(output)} Rpf Address
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarStarRPRpfAddrGet(L7_uchar8 familyType,
                                     L7_inet_addr_t *pRpAddr, 
                                     L7_inet_addr_t *pRpfAddr);
/*********************************************************************
* @purpose  To get the Flags in Star Star RP TABLE.
*
* @param    familyType    @b{(input)}  Address Family type
* @param    pRpAddr       @b{(input)}  RP Address
* @param    pFlags        @b{(output)} Flags
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarStarRPFlagsGet(L7_uchar8 familyType,
                                   L7_inet_addr_t *pRpAddr, 
                                   L7_uint32      *pFlags);
/*********************************************************************
* @purpose  To get the If Index in Star Star RP TABLE.
*
* @param    familyType    @b{(input)} Address Family type
* @param    pGrpAddr      @b{(input)} RP Address
* @param    pIfIndex      @b{(output)} If Index.
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarStarRPIfIndexGet(L7_uchar8 familyType,
                                     L7_inet_addr_t *pRpAddr, 
                                     L7_uint32      *pIfIndex);
#endif
/*********************************************************************
* @purpose  Send event to PIMSM Vendor
*
* @param    familyType    @b{(input)}     Address Family type
* @param    eventType     @b{(input)}     Event Type
* @param    msgLen        @b{(input)}     Message Length
* @param    eventMsg      @b{(input)}     Event Message
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapCommonCallback(L7_uchar8 familyType,
                                  L7_uint32  eventType, 
                                  L7_uint32  msgLen, 
                                  void *eventMsg);



 /*****************************************************************
*
* @purpose  Gets outgoing interfaces for the given source address,
*           group address and source mask.
*
* @param    familyType           @b{(input)}     Address Family type
* @param    ipMRouteGroup        @b{(input)}    Multicast Group Address
* @param    ipMRouteSource       @b{(input)}    Multicast Source Address
* @param    ipMRouteSourceMask   @b{(input)}    Multicast Source Address Mask
* @param    outIntIfNum          @b{(output)}   Outgoing Internal Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*****************************************************************/
L7_RC_t pimsmMapSGOIFGet(L7_uchar8 familyType,
                                           L7_inet_addr_t *ipMRouteGroup,
                                           L7_inet_addr_t *ipMRouteSource,
                                           L7_inet_addr_t *ipMRouteSourceMask,
                                           L7_INTF_MASK_t     * outIntIfMask);

/*****************************************************************
*
* @purpose  Gets outgoing interfaces for the given source address,
*           group address and source mask.
*
* @param    familyType           @b{(input)}     Address Family type
* @param    ipMRouteGroup        @b{(input)}    Multicast Group Address
* @param    ipMRouteSource       @b{(input)}    Multicast Source Address
* @param    ipMRouteSourceMask   @b{(input)}    Multicast Source Address Mask
* @param    outIntIfNum          @b{(output)}   Outgoing Internal Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*****************************************************************/
L7_RC_t pimsmMapStarGOIFGet(L7_uchar8 familyType,
                                           L7_inet_addr_t *ipMRouteGroup,
                                           L7_INTF_MASK_t      *outIntIfMask);


/*****************************************************************
*
* @purpose  Gets outgoing interfaces for the given source address,
*           group address and source mask.
*
* @param    familyType           @b{(input)}     Address Family type
* @param    ipMRouteGroup        @b{(input)}    Multicast Group Address
* @param    ipMRouteSource       @b{(input)}    Multicast Source Address
* @param    ipMRouteSourceMask   @b{(input)}    Multicast Source Address Mask
* @param    outIntIfNum          @b{(output)}   Outgoing Internal Interface Number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*****************************************************************/
L7_RC_t pimsmMapOIFGet(L7_uchar8 familyType,
                       L7_inet_addr_t *ipMRouteGroup,
                       L7_inet_addr_t *ipMRouteSource,
                       L7_inet_addr_t *ipMRouteSourceMask,
                       L7_INTF_MASK_t      *outIntIfMask);

/*********************************************************************
* @purpose  Gets the inetAddressType for the specified interface.
*
* @param    intIfNum     Internal Interface number
* @param    familyType    IPV4/V6
* @param    familyType     L7_ENABLE or L7_DISABLE
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR      if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapBInterfaceAddressTypeGet(L7_uint32 intIfNum, L7_uint32 familyType, L7_int32 *addrType);



/*********************************************************************
* @purpose  Gets the Generation ID value for the specified interface.
*
* @param    intIfNum @b{(input)}  internal Interface number
* @param    familyType    IPV4/V6
* @param    *ipAddr   @b{(output)} IP Address
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR, if interface does not exist
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapInterfaceGenerationIDValueGet(L7_uint32 intIfNum,
                                       L7_uint32 familyType, L7_uint32 *genIDValue);





/*********************************************************************
* @purpose Gets the UseDRPriority option for this interface
*
* @param    familyType       @b{(input)} Address Family type
* @param    intIfNum         @b{(input)} Internal Interface number
* @param    *useDRPriority   @b{(output)} Use DR Priority Option
*
* @returns  L7_SUCCESS       if success
* @returns  L7_ERROR         if interface does not exist
* @returns  L7_FAILURE       if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapInterfaceUseeRPriorityGet(L7_uint32 familyType,
                                          L7_uint32 intIfNum,
                                          L7_int32 *useDRPriority);


/*********************************************************************
* @purpose  Sets the UseDRPriority option for this interface
*
* @param    intIfNum        Internal Interface number
* @param    familyType    IPV4/V6
* @param    useDRPriority   Use DR Priority Option
*
* @returns  L7_SUCCESS       if success
* @returns  L7_FAILURE       if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapInterfaceBSRBorderSet(L7_uint32 intIfNum,
                                         L7_uint32 familyType,
                                        L7_uint32 bSRBorder);
                                        

/*********************************************************************
* @purpose  Gets the BSRBorder option for this interface
*
* @param    intIfNum        Internal Interface number
* @param    familyType    IPV4/V6
* @param    *bSRBorder      BSRBorder option (TRUE/FALSE)
*
* @returns  L7_SUCCESS       if success
* @returns  L7_ERROR         if interface does not exist
* @returns  L7_FAILURE       if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pimsmMapInterfaceBSRBorderGet(L7_uint32 intIfNum,
                                                L7_uint32 familyType,
                                                L7_int32 *bSRBorder);
                                        

/************ END OF PIMSM INTERFACE TABLE USMDB APIs ***************/





/*********************************************************************
* @purpose  To get if the neighbor DR Priority is present.
*
* @param    intIfNum    internal Interface number
* @param    familyType   IP Version
* @param    *inetIPAddr   IP Address
* @param    *bNbrDRPrio   Is Nbr Priority Set.
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapNeighborDRPriorityPresentGet(L7_uint32 intIfNum, L7_uint32 familyType,
                                          L7_inet_addr_t *inetIPAddr, L7_uint32 *bNbrDRPrio);

/************ END OF PIMSM NEIGHBOUR TABLE USMDB APIs ***************/

/*********************************************************************
* @purpose  Check whether SG entry exists for the specified index.
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGEntryGet(L7_uint32 familyType, L7_inet_addr_t *pimSGGrpAddr,
                                 L7_inet_addr_t *pimSGSrcAddr);

/*********************************************************************
* @purpose  Obtain the next sequential INDEX in the SG TABLE
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @notes    next interface entry
*
* @end
*********************************************************************/
/*L7_RC_t pimsmMapSGEntryNextGet(L7_int32 familyType, L7_inet_addr_t *pimSGGrpAddr,
                                     L7_inet_addr_t *pimSGSrcAddr);*/



/*********************************************************************
* @purpose  To get the SG UpstreamJoinState
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    sgUpTime   SG Up Time
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGUpstreamJoinStateGet(L7_uint32 familyType, L7_inet_addr_t *pimSGGrpAddr,
                                  L7_inet_addr_t *pimSGSrcAddr, L7_uint32 *sgUpTime);

/*********************************************************************
* @purpose  To get the SG PIM Mode
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    sgPimMode      SG PIM Mode
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGPimModeGet (L7_uint32 familyType,
                              L7_inet_addr_t *pimSGGrpAddr, 
                              L7_inet_addr_t *pimSGSrcAddr,
                              L7_uint32 *sgPimMode);

/*********************************************************************
* @purpose  To get the SG UpstreamJoinTimer.
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    sgUpTime   SG Up Time
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGUpstreamJoinTimerGet(L7_uint32 familyType, L7_inet_addr_t *pimSGGrpAddr,
                                  L7_inet_addr_t *pimSGSrcAddr, L7_uint32 *sgUpTime);

/*********************************************************************
* @purpose  To get the SG RPF UpstreamNeighbor.
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    rpfNextHop   RPF NextHop
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGUpstreamNeighborGet(L7_uint32 familyType, L7_inet_addr_t *pimSGGrpAddr,
                                      L7_inet_addr_t *pimSGSrcAddr, L7_inet_addr_t *rpfNextHop);

/*********************************************************************
* @purpose  To get the SG RPFIfIndex.
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    sgUpTime   SG Up Time
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGRPFIfIndexGet(L7_uint32 familyType, L7_inet_addr_t *pimSGGrpAddr,
                                  L7_inet_addr_t *pimSGSrcAddr, L7_uint32 *sgUpTime);
                                       

/*********************************************************************
* @purpose  To get the SG RPFRouteProtocol.
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    sgUpTime   SG Up Time
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGRPFRouteProtocolGet(L7_uint32 familyType, L7_inet_addr_t *pimSGGrpAddr,
                                  L7_inet_addr_t *pimSGSrcAddr, L7_uint32 *sgUpTime);


/*********************************************************************
* @purpose  To get the SG RPFRouteProtocol.
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    rpfNextHop     NextHop Addr
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGRPFRouteAddressGet(L7_uint32 familyType, L7_inet_addr_t *pimSGGrpAddr,
                                      L7_inet_addr_t *pimSGSrcAddr, L7_inet_addr_t *rpfNextHop);
                                  

/*********************************************************************
* @purpose  To get the SG RPFRoutePrefixLength.
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    sgUpTime   SG Up Time
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGRPFRoutePrefixLengthGet(L7_uint32 familyType, L7_inet_addr_t *pimSGGrpAddr,
                                  L7_inet_addr_t *pimSGSrcAddr, L7_uint32 *sgUpTime);

/*********************************************************************
* @purpose  To get the SG RPFRouteMetricPref.
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    sgUpTime   SG Up Time
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGRPFRouteMetricPrefGet(L7_uint32 familyType, L7_inet_addr_t *pimSGGrpAddr,
                                  L7_inet_addr_t *pimSGSrcAddr, L7_uint32 *sgUpTime);

/*********************************************************************
* @purpose  To get the SG RPFRouteMetric.
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    sgUpTime   SG Up Time
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGRPFRouteMetricGet(L7_uint32 familyType, L7_inet_addr_t *pimSGGrpAddr,
                                  L7_inet_addr_t *pimSGSrcAddr, L7_uint32 *sgUpTime);

/*********************************************************************
* @purpose  To get the SG KeepaliveTimer.
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    sgUpTime   SG Up Time
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGKeepaliveTimerGet(L7_uint32 familyType, L7_inet_addr_t *pimSGGrpAddr,
                                  L7_inet_addr_t *pimSGSrcAddr, L7_uint32 *sgUpTime);

/*********************************************************************
* @purpose  To get the SG DRRegisterState.
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    sgUpTime   SG Up Time
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGDRRegisterStateGet(L7_uint32 familyType, L7_inet_addr_t *pimSGGrpAddr,
                                  L7_inet_addr_t *pimSGSrcAddr, L7_uint32 *sgUpTime);

/*********************************************************************
* @purpose  To get the SG DRRegisterStopTimer.
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    sgUpTime   SG Up Time
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGDRRegisterStopTimerGet(L7_uint32 familyType, L7_inet_addr_t *pimSGGrpAddr,
                                  L7_inet_addr_t *pimSGSrcAddr, L7_uint32 *sgUpTime);

/*********************************************************************
* @purpose  To get the SG RPF RPRegisterPMBRAddress
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    rpfNextHop   RPF NextHop
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGRPRegisterPMBRAddressGet(L7_uint32 familyType, L7_inet_addr_t *pimSGGrpAddr,
                                      L7_inet_addr_t *pimSGSrcAddr, L7_inet_addr_t *rpfNextHop);

/*********************************************************************
* @purpose  To get the SG RPF nextHop.
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    rpfNextHop   RPF NextHop
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGRPFNextHopGet(L7_uint32 familyType, L7_inet_addr_t *pimSGGrpAddr, 
                                      L7_inet_addr_t *pimSGSrcAddr, L7_inet_addr_t *rpfNextHop);

/*********************************************************************
* @purpose  To get the SG SPTBit.
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    sgSPTBit   SG SPTBit
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGSPTBitGet(L7_uint32 familyType, L7_inet_addr_t *pimSGGrpAddr, 
                                   L7_inet_addr_t *pimSGSrcAddr, L7_uint32 *sgSPTBit);

/************ END OF PIMSM SG TABLE USMDB APIs ***************/

/*********************************************************************
* @purpose  Check whether SGRpt entry exists for the specified index.
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t  pimsmMapSGRptEntryGet(L7_uint32 familyType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr);

/*********************************************************************
* @purpose  Obtain the next sequential INDEX in the SGRpt TABLE
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @notes    next interface entry
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGRptEntryNextGet(L7_uchar8 familyType, L7_inet_addr_t *pimSGGrpAddr, 
                                                 L7_inet_addr_t *pimSGSrcAddr);

/*********************************************************************
* @purpose  To get the SGRPT uptime.
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    sgRPTUpTime   SGRPT Up Time
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGRptUpTimeGet(L7_uint32 familyType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,
                                    L7_uint32 *sgRPTUpTime);

/*********************************************************************
* @purpose  To get the SGRPT upstream prune state.
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    sgRPTUpTime   SGRPT Up Time
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGRPTUpstreamPruneStateGet(L7_uint32 familyType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,
                                    L7_int32 *sgRPTPruneState);

/*********************************************************************
* @purpose  To get the SGRPT upstream Override Timer.
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    sgRPTUpTime   SGRPT Up Time
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGRPTUpstreamOverrideTimerGet(L7_uint32 familyType, L7_inet_addr_t *pimSGGrpAddr,
                                               L7_inet_addr_t *pimSGSrcAddr, L7_uint32 *sgRPTPruneState);

/************ END OF PIMSM SGRPT TABLE USMDB APIs ***************/




/************ END OF PIMSM SSM RANGE TABLE USMDB APIs ***************/





/*********************************************************************
* @purpose  Check whether Group mapping entry exists for the specified index.
*
* @param    origin               mapping origin
* @param    familyType             whether IPV4/V6
* @param    pimMappingGrpAddr    Group address
* @param    *length              length of the address
* @param    pimMappingRPAddr     mapping RP Addr
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapGroupMappingEntryGet(L7_uchar8 origin, L7_uchar8 familyType, 
                                             L7_inet_addr_t *pimMappingGrpAddr, L7_uchar8 length, 
                                             L7_inet_addr_t *pimMappingRPAddr);

/*********************************************************************
* @purpose  Obtain the next sequential INDEX in the Group mapping TABLE
*
* @param    origin               mapping origin
* @param    familyType             whether IPV4/V6
* @param    pimMappingGrpAddr    Group address
* @param    *length              length of the address
* @param    pimMappingRPAddr     mapping RP Addr
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @notes    next interface entry
*
* @end
*********************************************************************/
L7_RC_t pimsmMapGroupMappingEntryNextGet(L7_uchar8 *origin, L7_uchar8 *familyType, 
                                                 L7_inet_addr_t *pimMappingGrpAddr, L7_uchar8 *length, 
                                                 L7_inet_addr_t *pimMappingRPAddr);

/*********************************************************************
* @purpose  To get the groupMapping pim mode
*
* @param    origin               mapping origin
* @param    familyType             whether IPV4/V6
* @param    *length              length of the address
* @param    pimMappingGrpAddr    Group address
* @param    pimGroupMappingMode  Group mapping mode
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapGroupMappingPimModeGet(L7_uchar8 origin, L7_uchar8 familyType, 
                                               L7_inet_addr_t *pimMappingGrpAddr,
                                               L7_uchar8 length, L7_inet_addr_t *pimMappingRPAddr,
                                               L7_uint32 *pimGroupMappingMode);

/************ END OF PIMSM GROUP MAPPING TABLE USMDB APIs ***************/


/************ END OF PIMSM BSR CANDIDATE RP TABLE USMDB APIs ***************/


 L7_RC_t pimsmMapbsrElectedBSREntryGet(L7_uint32 familyType);

  
/*********************************************************************
* @purpose  Obtain the next sequential INDEX in the bsrCandidateRP TABLE
*
* @param    familyType             whether IPV4/V6
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @notes    next interface entry
*
* @end
*********************************************************************/
L7_RC_t pimsmMapbsrElectedBSREntryNextGet(L7_int32 *familyType);

/*********************************************************************
* @purpose  To get the bsrCandtBSR electedBSR.
*
* @param    familyType             whether IPV4/V6
* @param    bsrElectedBSRAddr    To get the bsrElectedBSR Addr
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t  pimsmMapbsrElectedBSRAddrGet(L7_uint32 familyType, L7_uint32 *bsrElectedBSRAddr);

/*********************************************************************
* @purpose  To get the bsrCandtBSR electedBSR.
*
* @param    familyType             whether IPV4/V6
* @param    bsrElectedBSRPriority  To get the bsrElectedBSR Priority
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapbsrElectedBSRPriorityGet(L7_uint32 familyType, L7_uint32 *bsrElectedBSRPriority);
                                        
/*********************************************************************
* @purpose  To get the bsrCandtBSR electedBSR.
*
* @param    familyType             whether IPV4/V6
* @param    hashMaskLen  To get the bsrElectedBSR hashMaskLen
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapbsrElectedBSRHashMaskLengthGet(L7_uint32 familyType, L7_uint32 *hashMaskLen);
/*********************************************************************
* @purpose  To get the bsrCandtBSR electedBSR.
*
* @param    familyType             whether IPV4/V6
* @param    bsrElectedBSRExpiryTime  To get the bsrElectedBSR ExpiryTime
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapbsrElectedBSRExpiryTimeGet(L7_uint32 familyType, L7_uint32 *bsrElectedBSRExpiryTime);
/********************************************************************
* @purpose  To get the bsrCandtBSR electedBSR.
*
* @param    familyType             whether IPV4/V6
* @param    bsrCRPAdvTimer  To get the bsrElectedBSR AdvTimer
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapbsrElectedBSRCRPAdvTimerGet(L7_uint32 familyType, L7_uint32 *bsrCRPAdvTimer);

/*********************************************************************
* @purpose  Check whether StarG entry exists for the specified index.
*
* @param    addrType             whether IPV4/V6
* @param    pimStarGGrpAddr      StarG group address
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarGEntryGet(L7_uchar8 familyType, L7_inet_addr_t *pimStarGGrpAddr); 

/*********************************************************************
* @purpose  Obtain the next sequential INDEX in the StarG TABLE
*
* @param    familyType             whether IPV4/V6
* @param    pimStarGGrpAddr      StarG group address
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @notes    next interface entry
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarGEntryNextGet(L7_uchar8 familyType, L7_inet_addr_t *pimStarGGrpAddr);


/*********************************************************************
* @purpose  To get the starG RP address.
*
* @param    familyType             whether IPV4/V6
* @param    pimStarGGrpAddr      StarG group address
* @param    pimStarGRPAddr  To get the pimStarGRPAddr
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarGRPAddrGet(L7_uchar8 familyType, L7_inet_addr_t *pimStarGGrpAddr,
                                          L7_inet_addr_t *pimStarGRPAddr);

/*********************************************************************
* @purpose  To get the StarGRPOrigin
*
* @param    familyType             whether IPV4/V6
* @param    pimStarGGrpAddr      StarG group address
* @param    pimStarGRPOrigin  To get the StarGRPOrigin
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarGRPOriginGet(L7_uchar8 familyType, L7_inet_addr_t *pimStarGGrpAddr,
                                       L7_uint32 *pimStarGRPOrigin);
/*********************************************************************
* @purpose  To get the pimStarGRPIsLocal
*
* @param    familyType             whether IPV4/V6
* @param    pimStarGGrpAddr      StarG group address
* @param    pimStarGRPIsLocal  To get the pimStarGRPIsLocal
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarGRPIsLocalGet(L7_uchar8 familyType, L7_inet_addr_t *pimStarGGrpAddr, 
                                    L7_uint32 *pimStarGRPIsLocal);
/*********************************************************************
* @purpose  To get the JoinState
*
* @param    familyType             whether IPV4/V6
* @param    pimStarGGrpAddr      StarG group address
* @param    JoinState  To get the JoinState
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmStarGUpstreamJoinStateGet(L7_uchar8 familyType, L7_inet_addr_t *pimStarGGrpAddr,
                                       L7_uint32 *JoinState);

/*********************************************************************
* @purpose  To get the UpstreamNeighbor
*
* @param    familyType             whether IPV4/V6
* @param    pimStarGGrpAddr      StarG group address
* @param    UpstreamNbr  To get the UpstreamNbr
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarGUpstreamNbrGet(L7_uchar8 familyType, L7_inet_addr_t *pimStarGGrpAddr,
                                    L7_inet_addr_t *UpstreamNbr);

/*********************************************************************
* @purpose  To get the RPFIfIndex
*
* @param    familyType             whether IPV4/V6
* @param    pimStarGGrpAddr      StarG group address
* @param    RPFIfIndex  To get the RPFIfIndex
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarGRPFIfIndexGet(L7_uchar8 familyType, L7_inet_addr_t *pimStarGGrpAddr, 
                                           L7_uint32 *RPFIfIndex);

/*********************************************************************
* @purpose  To get the RPFNextHop
*
* @param    familyType             whether IPV4/V6
* @param    pimStarGGrpAddr      StarG group address
* @param    RPFNextHop  To get the RPFNextHop
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarGRPFNextHopGet(L7_uchar8 familyType, L7_inet_addr_t *pimStarGGrpAddr, 
                                   L7_inet_addr_t *RPFNextHop);

/*********************************************************************
* @purpose  To get the RPFRouteProtocol
*
* @param    familyType             whether IPV4/V6
* @param    pimStarGGrpAddr      StarG group address
* @param    RPFRouteProtocol  To get the RouteProtocol
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarGRPFRouteProtocolGet(L7_uchar8 familyType, L7_inet_addr_t *pimStarGGrpAddr,
                                               L7_uint32 *RouteProtocol);

/*********************************************************************
* @purpose  To get the RPFRouteAddress
*
* @param    familyType             whether IPV4/V6
* @param    pimStarGGrpAddr      StarG group address
* @param    RPFRouteAddress  To get the RPFRouteAddress
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarGRPFRouteAddrGet(L7_uchar8 familyType, L7_inet_addr_t *pimStarGGrpAddr,
                                     L7_inet_addr_t *RPFRouteAddr);

/*********************************************************************
* @purpose  To get the RPFRoutePrefixLength
*
* @param    familyType             whether IPV4/V6
* @param    pimStarGGrpAddr      StarG group address
* @param    RPFRoutePrefixLength  To get the RPFRoutePrefixLength
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarGRPFRoutePrefixLenGet(L7_uchar8 familyType, L7_inet_addr_t *pimStarGGrpAddr,
                                          L7_uint32 *RPFRoutePrefixLen);

/*********************************************************************
* @purpose  To get the RPFRouteMetricPref
*
* @param    familyType             whether IPV4/V6
* @param    pimStarGGrpAddr      StarG group address
* @param    RPFRouteMetricPref  To get the RPFRouteMetricPref
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t  pimsmMapStarGRPFRouteMetricPrefGet(L7_uchar8 familyType, L7_inet_addr_t *pimStarGGrpAddr, 
                                           L7_uint32 *RPFRouteMetricPref);

/*********************************************************************
* @purpose  To get the RPFRouteMetric
*
* @param    familyType             whether IPV4/V6
* @param    pimStarGGrpAddr      StarG group address
* @param    RPFRouteMetric  To get the RPFRouteMetric
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarGRPFRouteMetricGet(L7_uchar8 familyType, L7_inet_addr_t *pimStarGGrpAddr, 
                                       L7_uint32 *RPFRouteMetric);
/************ END OF PIMSM STARG TABLE USMDB APIs ***************/

/*********************************************************************
* @purpose  Check whether StarGI entry exists for the specified index.
*
* @param    familyType             whether IPV4/V6
* @param    pimStarGIGrpAddr      StarGI group address
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarGIEntryGet(L7_uchar8 familyType, L7_inet_addr_t *pimStarGIGrpAddr, L7_uint32 IfIndex);


/*********************************************************************
* @purpose  To get the StarGI uptime
*
* @param    familyType             whether IPV4/V6
* @param    pimStarGIGrpAddr, StarGI group address
* @param    pimStarGIUpTime    To get the pimStarGIUpTime 
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarGIUpTimeGet(L7_uchar8 familyType, L7_inet_addr_t *pimStarGIGrpAddr, L7_uint32 IfIndex, L7_uint32 *pimStarGIUpTime);

/*********************************************************************
* @purpose  To get the starG LocalMembership
*
* @param    familyType             whether IPV4/V6
* @param    pimStarGIGrpAddr, StarGI group address
* @param    pimStarGIRPAddr  To get the pimStarGIRPAddr
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarGILocalMembershipGet(L7_uchar8 familyType, L7_inet_addr_t *pimStarGIGrpAddr, L7_uint32 IfIndex, L7_uint32 *localMemShip);

/*********************************************************************
* @purpose  To get the JoinPruneState
*
* @param    familyType             whether IPV4/V6
* @param    pimStarGIGrpAddr, StarGI group address
* @param    JoinState  To get the JoinState
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarGIJoinPruneStateGet(L7_uchar8 familyType, L7_inet_addr_t *pimStarGIGrpAddr, L7_uint32 IfIndex, L7_uint32 *JoinState);

/*********************************************************************
* @purpose  To get the PrunePendingTimer
*
* @param    familyType             whether IPV4/V6
* @param    pimStarGIGrpAddr, StarGI group address
* @param    JoinTimer  To get the JoinTimer
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarGIPrunePendingTimerGet(L7_uchar8 familyType, L7_inet_addr_t *pimStarGIGrpAddr, L7_uint32 IfIndex, L7_uint32 *JoinTimer);

/*********************************************************************
* @purpose  To get the JoinExpiryTimer
*
* @param    familyType             whether IPV4/V6
* @param    pimStarGIGrpAddr, StarGI group address
* @param    JoinExpiry  To get the JoinExpiry
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarGIJoinExpiryTimerGet(L7_uchar8 familyType, L7_inet_addr_t *pimStarGIGrpAddr, L7_uint32 IfIndex, L7_uint32 *JoinExpiry);
/*********************************************************************
* @purpose  To get the AssertState
*
* @param    familyType             whether IPV4/V6
* @param    pimStarGIGrpAddr, StarGI group address
* @param    AssertState  To get the AssertState
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarGIAssertStateGet(L7_uchar8 familyType, L7_inet_addr_t *pimStarGIGrpAddr, L7_uint32 IfIndex, L7_uint32 *AssertState);

/*********************************************************************
* @purpose  To get the AssertTimer
*
* @param    familyType             whether IPV4/V6
* @param    pimStarGIGrpAddr, StarGI group address
* @param    AssertTimer  To get the AssertTimer
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarGIAssertTimerGet(L7_uchar8 familyType, L7_inet_addr_t *pimStarGIGrpAddr, L7_uint32 IfIndex, L7_uint32 *AssertTimer);

/*********************************************************************
* @purpose  To get the AssertWinnerAddr
*
* @param    familyType             whether IPV4/V6
* @param    pimStarGIGrpAddr, StarGI group address
* @param    AssertWinnerAddr  To get the RouteProtocol
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarGIAssertWinnerAddrGet(L7_uchar8 familyType, L7_inet_addr_t *pimStarGIGrpAddr, L7_uint32 IfIndex, L7_inet_addr_t *AssertWinner);


/*********************************************************************
* @purpose  To get the AssertWinnerMetric
*
* @param    familyType             whether IPV4/V6
* @param    pimStarGGrpAddr      StarG group address
* @param    AssertWinnerMetric  To get the AssertWinnerMetric
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarGIAssertWinnerMetricGet(L7_uchar8 familyType, L7_inet_addr_t *pimStarGGrpAddr, L7_uint32 ifindex, L7_uint32 *AssertWinnerMetric);

/************ END OF PIMSM StarGI TABLE USMDB APIs ***************/

/*********************************************************************
* @purpose  Check whether SG entry exists for the specified index.
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGIEntryGet(L7_uchar8 familyType, L7_inet_addr_t *pimSGGrpAddr, L7_inet_addr_t *pimSGSrcAddr,L7_int32 ifindexg);


/*********************************************************************
* @purpose  To get the SG uptime.
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    sgUpTime   SG Up Time
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGIUpTimeGet(L7_uchar8 familyType, L7_inet_addr_t *pimSGGrpAddr,
                      L7_inet_addr_t *pimSGSrcAddr,L7_int32 ifindexg,L7_uint32 *sgUpTime);

/*********************************************************************
* @purpose  To get the LocalMembership
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    LocalMembership    LocalMembership
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGILocalMembershipGet(L7_uchar8 familyType, L7_inet_addr_t *pimSGGrpAddr,
                                  L7_inet_addr_t *pimSGSrcAddr,L7_int32 ifindexg,L7_uint32 *LocalMembership);

/*********************************************************************
* @purpose  To get the JoinPruneState
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    JoinPruneState JoinPruneState
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGIJoinPruneStateGet(L7_uchar8 familyType, L7_inet_addr_t *pimSGGrpAddr,
                         L7_inet_addr_t *pimSGSrcAddr,L7_int32 ifindexg, L7_uint32 *JoinPruneState);

/*********************************************************************
* @purpose  To get the PrunePendingTimer
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    JoinTimer  To get the JoinTimer
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGIPrunePendingTimerGet(L7_uchar8 familyType, L7_inet_addr_t *pimSGGrpAddr,
                               L7_inet_addr_t *pimSGSrcAddr,L7_int32 IfIndex,L7_uint32 *JoinTimer);

/*********************************************************************
* @purpose  To get the JoinExpiryTimer
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    JoinExpiry  To get the JoinExpiry
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGIJoinExpiryTimerGet(L7_uchar8 familyType, L7_inet_addr_t *pimSGGrpAddr,
                                L7_inet_addr_t *pimSGSrcAddr,L7_int32 IfIndex,L7_uint32 *JoinExpiry);

/*********************************************************************
* @purpose  To get the AssertState
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    AssertState  To get the AssertState
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGIAssertStateGet(L7_uchar8 familyType, L7_inet_addr_t *pimSGGrpAddr,
                              L7_inet_addr_t *pimSGSrcAddr,L7_int32 IfIndex,L7_uint32 *AssertState);

/*********************************************************************
* @purpose  To get the AssertTimer
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    AssertTimer  To get the AssertTimer
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGIAssertTimerGet(L7_uchar8 familyType, L7_inet_addr_t *pimSGGrpAddr, 
                              L7_inet_addr_t *pimSGSrcAddr,L7_int32 IfIndex, L7_uint32 *AssertTimer);

/*********************************************************************
* @purpose  To get the AssertWinnerAddr
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    AssertWinnerAddr  To get the RouteProtocol
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGIAssertWinnerAddrGet(L7_uchar8 familyType, L7_inet_addr_t *pimSGGrpAddr, 
                       L7_inet_addr_t *pimSGSrcAddr,L7_int32 IfIndex,L7_inet_addr_t *AssertWinner);

/*********************************************************************
* @purpose  To get the AssertWinnerMetricPref
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    AssertWinnerMetricPref  To get the AssertWinnerMetricPref
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGIRPFRouteMetricPrefGet(L7_uchar8 familyType, L7_inet_addr_t *pimSGGrpAddr,
                        L7_inet_addr_t *pimSGSrcAddr,L7_int32 IfIndex,L7_uint32 *AssertWinnerMetricPref);

/*********************************************************************
* @purpose  To get the AssertWinnerMetric
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    AssertWinnerMetric  To get the AssertWinnerMetric
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGIAssertWinnerMetricGet(L7_uchar8 familyType, L7_inet_addr_t *pimSGGrpAddr,
                         L7_inet_addr_t *pimSGSrcAddr,L7_int32 IfIndex,L7_uint32 *AssertWinnerMetric);

/************ END OF PIMSM SGI TABLE USMDB APIs ***************/

/*********************************************************************
* @purpose  Check whether SGRptI entry exists for the specified index.
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGRptIEntryGet(L7_uchar8 familyType, L7_inet_addr_t *pimSGGrpAddr,
                                         L7_inet_addr_t *pimSGSrcAddr,L7_uint32 ifindexg);

/*********************************************************************
* @purpose  Obtain the next sequential INDEX in the SGRptI TABLE
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @notes    next interface entry
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGRptIEntryNextGet(L7_uchar8 familyType, L7_inet_addr_t *pimSGGrpAddr,
                                          L7_inet_addr_t *pimSGSrcAddr,L7_uint32 *ifindexg);

/*********************************************************************
* @purpose  To get the SG uptime.
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    sgUpTime   SG Up Time
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGRptIUpTimeGet(L7_uchar8 familyType, L7_inet_addr_t *pimSGGrpAddr, 
                       L7_inet_addr_t *pimSGSrcAddr,L7_uint32 ifindexg,L7_uint32 *sgUpTime);

/*********************************************************************
* @purpose  To get the LocalMembership
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    LocalMembership    LocalMembership
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGRptILocalMembershipGet(L7_uchar8 familyType, L7_inet_addr_t *pimSGGrpAddr,
                    L7_inet_addr_t *pimSGSrcAddr,L7_uint32 ifindexg, L7_uint32 *LocalMembership);

/*********************************************************************
* @purpose  To get the JoinPruneState
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    JoinPruneState JoinPruneState
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGRptIJoinPruneStateGet(L7_uchar8 familyType, L7_inet_addr_t *pimSGGrpAddr, 
                           L7_inet_addr_t *pimSGSrcAddr,L7_uint32 ifindexg, L7_uint32 *JoinPruneState);

/*********************************************************************
* @purpose  To get the PrunePendingTimer
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    JoinTimer  To get the JoinTimer
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGRptIPrunePendingTimerGet(L7_uchar8 familyType, L7_inet_addr_t *pimSGGrpAddr, 
                           L7_inet_addr_t *pimSGSrcAddr,L7_uint32 IfIndex, L7_uint32 *JoinTimer);

/*********************************************************************
* @purpose  To get the JoinExpiryTimer
*
* @param    familyType    ipv4/v6
* @param    pimSGGrpAddr   SG Grp Addr
* @param    pimSGSrcAddr   SG Src Addr
* @param    JoinExpiry  To get the JoinExpiry
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapSGRptIJoinExpiryTimerGet(L7_uchar8 familyType, L7_inet_addr_t *pimSGGrpAddr,
                             L7_inet_addr_t *pimSGSrcAddr,L7_uint32 IfIndex,L7_uint32 *JoinExpiry);

/*********************************************************************
* @purpose  Check whether Group mapping entry exists for the specified index.
*
* @param    origin               mapping origin
* @param    familyType             whether IPV4/V6
* @param    pimMappingGrpAddr    Group address
* @param    *length              length of the address
* @param    pimMappingRPAddr     mapping RP Addr
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapbsrRPSetEntryGet(L7_uchar8 origin, L7_uchar8 familyType,
                                             L7_inet_addr_t *pimMappingGrpAddr, L7_uchar8 length,
                                             L7_inet_addr_t *pimMappingRPAddr);

/*********************************************************************
* @purpose  Obtain the next sequential INDEX in the Group mapping TABLE
*
* @param    origin               mapping origin
* @param    familyType             whether IPV4/V6
* @param    pimMappingGrpAddr    Group address
* @param    *length              length of the address
* @param    pimMappingRPAddr     mapping RP Addr
*
* @returns  L7_SUCCESS  if next interface entry exists
* @returns  L7_FAILURE  if next interface entry does not exist
*
* @notes    next interface entry
*
* @end
*********************************************************************/
L7_RC_t pimsmMapbsrRPSetEntryNextGet(L7_uchar8 *origin,L7_uchar8 *familyType,
                                                 L7_inet_addr_t *pimMappingGrpAddr,L7_uchar8 *length,
                                                 L7_inet_addr_t *pimMappingRPAddr);

/*********************************************************************
* @purpose  To get the RPSetPriority
*
* @param    origin               mapping origin
* @param    familyType             whether IPV4/V6
* @param    *length              length of the address
* @param    pimMappingGrpAddr    Group address
* @param    RPSetPriority  RPSetPriority
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapbsrRPSetPriorityGet(L7_uchar8 origin, L7_uchar8 familyType,
                                               L7_inet_addr_t *pimMappingGrpAddr,
                                               L7_uchar8 length, L7_inet_addr_t *pimMappingRPAddr,
                                               L7_uint32 *RPSetPriority);
/*********************************************************************
* @purpose  To get the RPSetHoldtime
*
* @param    origin               mapping origin
* @param    familyType             whether IPV4/V6
* @param    *length              length of the address
* @param    pimMappingGrpAddr    Group address
* @param    RPSetHoldtime        RPSetHoldtime
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapbsrRPSetHoldtimeGet(L7_uchar8 origin, L7_uchar8 familyType,
                                               L7_inet_addr_t *pimMappingGrpAddr,
                                               L7_uchar8 length, L7_inet_addr_t *pimMappingRPAddr,
                                               L7_uint32 *RPSetHoldtime);


/*********************************************************************
* @purpose  To get the UpstreamJoinTimer
*
* @param    familyType             whether IPV4/V6
* @param    pimStarGGrpAddr      StarG group address
* @param    JoinTimer  To get the JoinTimer
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarGUpstreamJoinTimerGet(L7_uchar8 familyType, L7_inet_addr_t *pimStarGGrpAddr,
                                                 L7_uint32 *JoinTimer);


/*********************************************************************
* @purpose  To get the JoinState
*
* @param    familyType             whether IPV4/V6
* @param    pimStarGGrpAddr      StarG group address
* @param    JoinState  To get the JoinState
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapStarGUpstreamJoinStateGet(L7_uchar8 familyType, L7_inet_addr_t *pimStarGGrpAddr,
                                                L7_uint32 *JoinState);

/*********************************************************************
* @purpose  To get the RPSetExpiryTime
*
* @param    origin               mapping origin
* @param    familyType             whether IPV4/V6
* @param    *length              length of the address
* @param    pimMappingGrpAddr    Group address
* @param    RPSetExpiryTime        RPSetExpiryTime
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t pimsmMapbsrRPSetExpiryTimeGet(L7_uchar8 origin, L7_uchar8 familyType,
                                               L7_inet_addr_t *pimMappingGrpAddr,
                                               L7_uchar8 length, L7_inet_addr_t *pimMappingRPAddr,
                                               L7_uint32 *RPSetExpiryTime);
/********************************************************************
* @purpose  To get the next staticRpentry from staticRp table
*
* @param familyType        whether IPV4/IPV6
* @param rpAddr            Rp address
* @param rpGrpAddr         Rp GroupAddress
* @param prefixLen         prefixlength of Rpgroupaddress
* @param indexstart        pointer to the next entry in the staticRp table
*
* @returns  L7_SUCCESS  if entry exists
* @returns  L7_FAILURE  if entry does not exist
*
* @notes
*
* @end

**********************************************************************/

 L7_RC_t pimsmMapCliStaticRpNextGet(L7_uchar8 familyType,L7_inet_addr_t *rpAddr, L7_inet_addr_t *rpGrpAddr, 
                                      L7_uchar8 *prefixLen, L7_uint32 *indexstart,L7_BOOL *override);



/***********************************************************************
* @purpose To get the next candidateRpentry from candRp Table
*
* @param familyType       whether IPV4/IPV6
* @param intIfNum         internal interface Number
* @param cRPGroupAddress  candidate Rp Group address
* @param cRPprefixLen     prefix length of group address
* @param indexstart       pointer to next entry in the table
*
*
* @returns L7_SUCCESS if entry exists
* @returns L7_FAILURE if entry does not exits
*
* @notes
*
* @end

***********************************************************************/

 L7_RC_t pimsmMapCliCandidateRPEntryNextGet(L7_uchar8 familyType,L7_uint32 *intIfNum,L7_inet_addr_t *cRPGroupAddress,
                                             L7_uchar8 *cRPprefixLen,  L7_uint32 *indexstart);



#endif /* _L7_PIMSM_API_H_ */
