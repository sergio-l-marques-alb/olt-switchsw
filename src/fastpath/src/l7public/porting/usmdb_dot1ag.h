/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2009
 *
 **********************************************************************
 *
 * @filename   "src/usmdb/metro/dot1ag/usmdb_dot1ag.c"
 *
 * @purpose    USMDB file for Dot1ag CFM-OAM
 *
 * @component  USMDB
 *
 * @comments   none
 *
 * @create     03/03/2009
 *
 * @author     Siva Mukka
 * @end
 *
 **********************************************************************/
#ifndef INCLUDE_USMDB_DOT1AG_H
#define INCLUDE_USMDB_DOT1AG_H
#include "l7_common.h"
#include "usmdb_common.h"
#include "dot1ag_exports.h"

/*********************************************************************
* @purpose  To find out if more maintenance domains can be created and
*           what is the next valid mdIndex available
*
* @param    mdIndex    @b{(output)} Index value used by SNMP. If no
*                                   more domains can be created then
*                                   this value is L7_NULL
*
* @returns  none  
*
* @notes    none
*
* @end
**********************************************************************/
void usmDbDot1agMdTableNextIndexGet(L7_uint32 *mdIndex);
/*********************************************************************
* @purpose  Create a maintenance domain with specified name and level.
*           Optionally you can provide the mdIndex value as well.
*
* @param    mdIndex    @b{(input)} Index value used by SNMP.
* @param    mdLevel    @b{(input)} Domain level to be set
* @param    mdName     @b{(input)} Maintenance Domain name. This has to
*                                  be unique for the CLI/Web to address
*                                  domains using name instead of index as
*                                  in SNMP.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
* @returns  L7_ALREADY_CONFIGURED
* @returns  L7_REQUEST_DENIED      Domain exists that has only one of
*                                  the given name or level
*
* @notes    none
*
* @end
**********************************************************************/
L7_RC_t usmDbDot1agMdCreate(L7_uint32 mdIndex, L7_int32 mdLevel, 
                       L7_uchar8 *mdName);
/*********************************************************************
* @purpose  Delete specified maintenance domain
*
* @param    mdIndex    @b{(input)} Index value used by SNMP. Use
*                                  L7_NULL for other UI's
* @param    mdLevel    @b{(input)} Domain level
* @param    mdName     @b{(input)} Maintenance Domain name. This has to
*                                  be unique for the CLI/Web to address
*                                  domains using name instead of index as
*                                  in SNMP.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
* @returns  L7_REQUEST_DENIED      Domain exists that has only one of
*                                  the given name or level
* @returns  L7_NOT_EXIST           Domain with a given name and
*                                  level does not exist
*
* @notes    none
*
* @end
**********************************************************************/
L7_RC_t usmDbDot1agMdDelete(L7_uint32 mdIndex, L7_int32 mdLevel,
                            L7_uchar8 *mdName);

/*********************************************************************
* @purpose  Verify if a give maintenance domain exists
*
* @param    mdIndex    @b{(input)} Index value used by SNMP.
* @param    mdLevel    @b{(input)} Domain level. This has to be unique
*                                  for the CLI/Web to address domains using
*                                  name instead of index as in SNMP.
* @param    mdName     @b{(input)} Maintenance Domain name. This has to
*                                  be unique for the CLI/Web to address
*                                  domains using name instead of index as
*                                  in SNMP.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @notes    none
*
* @end
**********************************************************************/
L7_RC_t usmDbDot1agMdIsValid(L7_uint32 mdIndex, L7_int32 mdLevel,
                             L7_uchar8 *mdName);

/*********************************************************************
* @purpose  Obtain configured maintenance domain name given mdIndex
*
* @param    mdIndex    @b{(input)} Index value used by SNMP. 
* @param    mdName     @b{(output)} Maintenance Domain name 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @notes    none
*
* @end
**********************************************************************/
L7_RC_t usmDbDot1agMdNameGet(L7_uint32 mdIndex, L7_uchar8 *mdName);
/*********************************************************************
* @purpose  Obtain configured maintenance domain level given mdIndex
*
* @param    mdIndex    @b{(input)} Index value used by SNMP.
* @param    mdLevel    @b{(output)} Maintenance Domain level
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @notes    none
*
* @end
**********************************************************************/
L7_RC_t usmDbDot1agMdLevelGet(L7_uint32 mdIndex, L7_int32 *mdLevel);
/*********************************************************************
* @purpose  Obtain configured maintenance domain mdIndex given mdLevel
*           or mdName
*
* @param    mdName     @b{(input)} Maintenance Domain name. Can be 
*                                  NULLPTR, if so use mdLevel as input
* @param    mdLevel    @b{(input)} Maintenance Domain level. Can be -1
*                                  of so use mdName as key
* @param    mdIndex    @b{(output)} Index value used by SNMP.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @notes    none
*
* @end
**********************************************************************/
L7_RC_t usmDbDot1agMdMdIndexGet(L7_uchar8 *mdName, L7_int32 mdLevel, 
                           L7_uint32 *mdIndex);
/*********************************************************************
* @purpose  Obtain number of MA's configured in a given maintenance domain
*           or mdName
*
* @param    mdIndex    @b{(input)}  Index for MD identification
* @param    mdCount    @b{(output)} Number of maintenance associations
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @notes    none
*
* @end
**********************************************************************/
L7_RC_t usmDbDot1agMdMaCountGet(L7_uint32 mdIndex, L7_uint32 *maCount);

/*********************************************************************
* @purpose  Obtain valid maIndex to be used to create a new 
*           maintenance association for this domain mdIndex
*
* @param    mdIndex      @b{(input)}  Index value used by SNMP.
* @param    maNextIndex  @b{(output)} New maintenance Assoc index. 
*                                     L7_NULL if no new MAs can be
*                                     created
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @notes    none
*
* @end
**********************************************************************/
L7_RC_t usmDbDot1agMdMaNextIndexGet(L7_uint32 mdIndex, L7_uint32 *maNextIndex);
/*********************************************************************
* @purpose  Obtain a next valid mdIndex value given a maintenance domain
*
* @param    nextMdIndex    @b{(input/output)} domain index value. Use L7_NULL
*                                         to obatain the first valid 
*                                         value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @notes    none
*
* @end
**********************************************************************/
L7_RC_t usmDbDot1agMdIndexNextGet(L7_uint32 *nextMdIndex);
/*********************************************************************
* @purpose  Create a maintenance association component table entry
*           given (mdIndex or mdName) and (maIndex or maName) and 
*           primVID
*
* @param    mdIndex    @b{(input)} Index used for MD indentification
*                                  used by SNMP.
* @param    maIndex    @b{(input)} Index for MA identification used by
*                                  SNMP.
* @param    primVID    @b{(input)} VLAN ID associated with the MA
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @notes
*
* @end
**********************************************************************/
L7_RC_t usmDbDot1agMaCompCreate(L7_uint32 mdIndex, L7_uint32 maIndex,
                           L7_uint32 primVID);
/*********************************************************************
* @purpose  Create a maintenance association given (mdIndex)
*           and (maIndex or maName) and primVID
*
* @param    mdIndex    @b{(input)} Index used for MD indentification
*                                  used by SNMP. Use L7_NULL for 
*                                  other UI's
* @param    maIndex    @b{(input)} Index for MA identification used by 
*                                  SNMP.
* @param    maName     @b{(input)} Maintenance assoc name. Can be NULLPTR
*                                  for SNMP
* @param    primVID    @b{(input)} VLAN ID associated with the MA. Can be
*                                  NULL when used with SNMP.
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
* @returns  L7_REQUEST_DENIED
* @returns  L7_TABLE_IS_FULL
* @returns  L7_ALREADY_CONFIGURED
*
* @notes   
*
* @end
**********************************************************************/
L7_RC_t usmDbDot1agMaCreate(L7_uint32 mdIndex, L7_uint32 maIndex,
                       L7_uchar8 *maName, L7_uint32 primVID);
/*********************************************************************
* @purpose  Delete a maintenance association given (mdIndex)
*           and (maIndex or primVID)
*
* @param    mdIndex    @b{(input)} Index used for MD indentification
*                                  used by SNMP.
* @param    maIndex    @b{(input)} Index for MA identification used by
                                   SNMP
* @param    maName     @b{(input)} Maintenance assoc name. Can be NULLPTR
*                                  for SNMP
* @param    primVID    @b{(input)} VLAN ID associated with the MA. Can be
*                                  NULL when used with SNMP.
*
* @returns  L7_FAILURE
* @returns  L7_SUCCESS
* @returns  L7_ERROR
* @returns  L7_NOT_EXIST
* @returns  L7_REQUEST_DENIED
*
* @notes
*
* @end
**********************************************************************/
L7_RC_t usmDbDot1agMaDelete(L7_uint32 mdIndex, L7_uint32 maIndex,
                            L7_uchar8 *maName, L7_uint32 primVID);

/*********************************************************************
* @purpose  Obtain next valid maintenance association given 
*           (mdIndex)
*
* @param    mdIndex    @b{(input/output)} Index used for MD indentification
*                                         used by SNMP.
* @param    maIndex    @b{(input/output)} Index for MA identification used by
*                                         SNMP.
*
* @returns  L7_FAILURE
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @notes Use L7_NULL as input to obtain first valid entry
*
* @end
**********************************************************************/
L7_RC_t usmDbDot1agMaNextGet(L7_uint32 *mdIndex, L7_uint32 *maIndex);
/*********************************************************************
* @purpose  Obtain maintenance association name given mdIndex and 
*           maIndex
*
* @param    mdIndex    @b{(input)} Index used for MD indentification
*                                  used by SNMP.
* @param    maIndex    @b{(input)} Index for MA identification used by
*                                  SNMP.
* @param    maName     @b{(output)} Maintenance association name
* @param    maNameFormat @b{(output)} Maintenance association name format
*
* @returns  L7_FAILURE
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @notes Use 
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1agMaNameGet(L7_uint32 mdIndex, L7_uint32 maIndex, 
                        L7_uchar8 *maName, L7_uint32 *maNameFormat);
/*********************************************************************
* @purpose  Obtain primary vlan associated with an MA
*
* @param    mdIndex    @b{(input)} Index used for MD indentification
*                                  used by SNMP.
* @param    maIndex    @b{(input)} Index for MA identification used by
*                                  SNMP.
* @param    primVID    @b{(output)} Primary VLAN associated with the MA
*
* @returns  L7_FAILURE
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @notes Use
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1agMaPrimVLANGet(L7_uint32 mdIndex, L7_uint32 maIndex,
                            L7_uint32 *primVID);
/*********************************************************************
* @purpose  Obtain maIndex given primary vlan associated with an MA
*
* @param    mdIndex    @b{(input)} Index used for MD indentification
*                                  used by SNMP.
* @param    primVID    @b{(input)} Primary VLAN associated with the MA
* @param    maIndex    @b{(output)} Index for MA identification used by
*                                  SNMP.
*
* @returns  L7_FAILURE
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @notes Use
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1agMaIndexGet(L7_uint32 mdIndex,  L7_uint32 primVID, 
                         L7_uint32 *maIndex);
/*********************************************************************
* @purpose  Obtain CCM interval associated with this MA
*
* @param    mdIndex     @b{(input)} Index used for MD indentification
* @param    maIndex     @b{(input)} Index for MA identification
* @param    ccmInterval @b{(output)} Continuity Check Interval
*
* @returns  L7_FAILURE
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @notes Use
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1agMaCCMIntervalGet(L7_uint32 mdIndex, L7_uint32 maIndex,
                               L7_uint32 *ccmInterval);
/*********************************************************************
* @purpose  Set CCM interval associated with this MA
*
* @param    mdIndex     @b{(input)} Index used for MD indentification
* @param    maIndex     @b{(input)} Index for MA identification
* @param    ccmInterval @b{(input)} Continuity Check Interval
*
* @returns  L7_FAILURE
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @notes Use
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1agMaCCMIntervalSet(L7_uint32 mdIndex, L7_uint32 maIndex,
                               L7_uint32 ccmInterval);
/*********************************************************************
* @purpose  Verify if an entry with given values exists in MEP Table
*
* @param    mdIndex    @b{(input)} Index used for MD indentification
* @param    maIndex    @b{(input)} Index for MA identification
* @param    mepId      @b{(input)} Index to identify a MEP
* @param    intIfNum   @b{(input)} Interface on which mep is configured
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes This functions ignores interface number
*
* @end
**********************************************************************/
L7_RC_t usmDbDot1agMepIsValid(L7_uint32 mdIndex, L7_uint32 maIndex,
                         L7_uint32 mepId, L7_uint32 intIfNum);
/*********************************************************************
* @purpose  Obtain next valid maintenance end point on an interface
*
* @param    mdIndex    @b{(input)} Index used for MD indentification
*                                  used by SNMP.
* @param    maIndex    @b{(input)} Index for MA identification used by
*                                  SNMP.
* @param    mepId      @b{(input)} VLAN ID associated with the MA.
* @param    intIfNum   @b{(input)} Interface on which mep is to be 
*                                  created
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @notes  Use L7_NULL values to obtain the first valid entry
*
* @end
**********************************************************************/
L7_RC_t usmDbDot1agMepNextGet(L7_uint32 *mdIndex, L7_uint32 *maIndex, 
                         L7_uint32 *mepId, L7_uint32 *intIfNum);
/*********************************************************************
* @purpose  Obtain direction of maintenance end point on an interface 
*
* @param    mdIndex    @b{(input)} Index used for MD indentification
*                                  used by SNMP. Use L7_NULL for
*                                  other UI's
* @param    maIndex    @b{(input)} Index for MA identification used by
*                                  SNMP.
* @param    mepId      @b{(input)} VLAN ID associated with the MA.
* @param    intIfNum   @b{(input)} Interface on which mep is to be
*                                  created
* @param    direction  @b{(output)} Direction of MEP (Up or Down)
*
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @notes 
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1agMepDirectionGet(L7_uint32 mdIndex, L7_uint32 maIndex,
                              L7_uint32 mepId, L7_uint32 intIfNum, 
                              L7_uint32 *direction);
/*********************************************************************
* @purpose  Obtain active mode of maintenance end point on an interface
*
* @param    mdIndex    @b{(input)} Index used for MD indentification
*                                  used by SNMP.
* @param    maIndex    @b{(input)} Index for MA identification used by
*                                  SNMP.
* @param    mepId      @b{(input)} VLAN ID associated with the MA.
* @param    intIfNum   @b{(input)} Interface on which mep is to be
*                                  created
* @param    mode       @b{(output)}  Active mode of MEP (L7_TRUE or 
*                                    L7_FALSE)
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1agMepActiveModeGet(L7_uint32 mdIndex, L7_uint32 maIndex,
                               L7_uint32 mepId, L7_uint32 intIfNum,
                               L7_BOOL *mode);
/*********************************************************************
* @purpose  Set active mode of maintenance end point on an interface
*
* @param    mdIndex    @b{(input)} Index used for MD indentification
*                                  used by SNMP.
* @param    maIndex    @b{(input)} Index for MA identification used by
*                                  SNMP.
* @param    mepId      @b{(input)} VLAN ID associated with the MA.
* @param    intIfNum   @b{(input)} Interface on which mep is to be
*                                  created
* @param    mode       @b{(input)} Active mode of MEP (L7_TRUE or L7_FALSE)
*
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1agMepActiveModeSet(L7_uint32 mdIndex, L7_uint32 maIndex,
                               L7_uint32 mepId, L7_uint32 intIfNum,
                               L7_BOOL mode);
/*********************************************************************
* @purpose  Obtain CCI enabled flag of maintenance end point on an 
*           interface
*
* @param    mdIndex    @b{(input)} Index used for MD indentification
*                                  used by SNMP.
* @param    maIndex    @b{(input)} Index for MA identification used by
*                                  SNMP.
* @param    mepId      @b{(input)} VLAN ID associated with the MA.
* @param    intIfNum   @b{(input)} Interface on which mep is to be
*                                  created
* @param    mode       @b{(output)}  CCI enabled flag of MEP (L7_TRUE or
*                                    L7_FALSE)
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1agMepCCIEnabledGet(L7_uint32 mdIndex, L7_uint32 maIndex,
                               L7_uint32 mepId, L7_uint32 intIfNum,
                               L7_BOOL *mode);
/*********************************************************************
* @purpose  Set CCI enabled flag of maintenance end point on an interface
*
* @param    mdIndex    @b{(input)} Index used for MD indentification
*                                  used by SNMP.
* @param    maIndex    @b{(input)} Index for MA identification used by
*                                  SNMP.
* @param    mepId      @b{(input)} VLAN ID associated with the MA.
* @param    intIfNum   @b{(input)} Interface on which mep is to be
*                                  created
* @param    mode       @b{(output)}  CCI enabled flag of MEP (L7_TRUE or
*                                    L7_FALSE)
*
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1agMepCCIEnabledSet(L7_uint32 mdIndex, L7_uint32 maIndex,
                               L7_uint32 mepId, L7_uint32 intIfNum,
                               L7_BOOL mode);
/*********************************************************************
* @purpose  Clear RMEP entries learned on a maintenance end point
*
* @param    mdIndex    @b{(input)} Index used for MD indentification
*                                  used by SNMP.
* @param    maIndex    @b{(input)} Index for MA identification used by
*                                  SNMP.
* @param    mepId      @b{(input)} Index to identify a MEP
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1agRmepClear(L7_uint32 mdIndex, L7_uint32 maIndex,
                               L7_uint32 mepId);
/*********************************************************************
* @purpose  Obtain Fault Notification Genreator SM status
*           of maintenance end point on an interface
*
* @param    mdIndex    @b{(input)} Index used for MD indentification
*                                  used by SNMP. 
* @param    maIndex    @b{(input)} Index for MA identification used by
*                                  SNMP.
* @param    mepId      @b{(input)} VLAN ID associated with the MA.
* @param    intIfNum   @b{(input)} Interface on which mep is to be
*                                  created
* @param    val       @b{(output)} Fault Notification Genreator SM 
*                                  status Dot1agCfmFngState_t
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1agMepFngStateGet(L7_uint32 mdIndex, L7_uint32 maIndex,
                             L7_uint32 mepId, L7_uint32 intIfNum,
                             L7_uint32 *val);
/*********************************************************************
* @purpose  Obtain maintenance end point on an interface parameter
*           defined by section 17.5 802.1ag d8-1
*
* @param    mdIndex    @b{(input)} Index used for MD indentification
*                                  used by SNMP.
* @param    maIndex    @b{(input)} Index for MA identification used by
*                                  SNMP.
* @param    mepId      @b{(input)} VLAN ID associated with the MA.
* @param    intIfNum   @b{(input)} Interface on which mep is to be
*                                  created
* @param    paramCode  @b{(input)} MEP object parameter defined by
*                                  Dot1agCfmMepEntry_t
* @param    val       @b{(output)} Param value
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1agMepParamGet(L7_uint32 mdIndex, L7_uint32 maIndex,
                          L7_uint32 mepId, L7_uint32 intIfNum, 
                          L7_uint32 paramCode, void *val);
/*********************************************************************
* @purpose  Obtain priority to be used in LTMs, CCMs sent by the
*           maintenance end point on an interface
*
* @param    mdIndex    @b{(input)} Index used for MD indentification
*                                  used by SNMP.
* @param    maIndex    @b{(input)} Index for MA identification used by
*                                  SNMP.
* @param    mepId      @b{(input)} VLAN ID associated with the MA.
* @param    intIfNum   @b{(input)} Interface on which mep is to be
*                                  created
* @param    val       @b{(output)} Priority value for CCMs and LTMs
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1agMepCcmLtmPriorityGet(L7_uint32 mdIndex, L7_uint32 maIndex,
                                   L7_uint32 mepId, L7_uint32 intIfNum,
                                   L7_uint32 *val);
/*********************************************************************
* @purpose  Set priority to be used in LTMs, CCMs sent by the
*           maintenance end point on an interface
*
* @param    mdIndex    @b{(input)} Index used for MD indentification
*                                  used by SNMP.
* @param    maIndex    @b{(input)} Index for MA identification used by
*                                  SNMP.
* @param    mepId      @b{(input)} VLAN ID associated with the MA.
* @param    intIfNum   @b{(input)} Interface on which mep is to be
*                                  created
* @param    val        @b{(input)} Priority value for CCMs and LTMs
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1agMepCcmLtmPrioritySet(L7_uint32 mdIndex, L7_uint32 maIndex,
                                   L7_uint32 mepId, L7_uint32 intIfNum,
                                   L7_uint32 val);
/*********************************************************************
* @purpose  Obtain MAC Address of the
*           maintenance end point on an interface
*
* @param    mdIndex    @b{(input)} Index used for MD indentification
*                                  used by SNMP.
* @param    maIndex    @b{(input)} Index for MA identification used by
*                                  SNMP.
* @param    mepId      @b{(input)} VLAN ID associated with the MA.
* @param    intIfNum   @b{(input)} Interface on which mep is to be
*                                  created
* @param    macAddr    @b{(output)} MAC Addr of the MEP
*
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1agCfmMepMacAddressGet(L7_uint32 mdIndex, L7_uint32 maIndex,
                                  L7_uint32 mepId, L7_uint32 intIfNum,
                                  L7_uchar8 *macAddr);
/*********************************************************************
* @purpose  Obtain "dot1agCfmMepLowPrDef -
*           An integer value specifying the lowest priority defect
*           that is allowed to generate fault alarm" of 
*           maintenance end point on an interface
*
* @param    mdIndex    @b{(input)} Index used for MD indentification
*                                  used by SNMP.
* @param    maIndex    @b{(input)} Index for MA identification used by
*                                  SNMP.
* @param    mepId      @b{(input)} VLAN ID associated with the MA.
* @param    intIfNum   @b{(input)} Interface on which mep is to be
*                                  created
* @param    val       @b{(output)} Lowest priority defect value
*                                  Dot1agCfmLowestAlarmPri_t
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1agCfmMepLowPrDefGet(L7_uint32 mdIndex, L7_uint32 maIndex,
                                L7_uint32 mepId, L7_uint32 intIfNum,
                                L7_uint32 *val);
/*********************************************************************
* @purpose  Set "dot1agCfmMepLowPrDef -
*           An integer value specifying the lowest priority defect
*           that is allowed to generate fault alarm" of 
*           maintenance end point on an interface
*
* @param    mdIndex    @b{(input)} Index used for MD indentification
*                                  used by SNMP.
* @param    maIndex    @b{(input)} Index for MA identification used by
*                                  SNMP.
* @param    mepId      @b{(input)} VLAN ID associated with the MA.
* @param    intIfNum   @b{(input)} Interface on which mep is to be
*                                  created
* @param    val       @b{(input)} Lowest priority defect value
*                                  Dot1agCfmLowestAlarmPri_t
*
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1agCfmMepLowPrDefSet(L7_uint32 mdIndex, L7_uint32 maIndex,
                                L7_uint32 mepId, L7_uint32 intIfNum,
                                L7_uint32 val);
/*********************************************************************
* @purpose  Obtain dot1agCfmMepFngAlarmTime - 
*           The time that defects must be present 
*           before a Fault Alarm is issued (fngAlarmTime. 20.33.3) 
*           (default 250 = 2.5s)
*
* @param    mdIndex    @b{(input)} Index used for MD indentification
*                                  used by SNMP.
* @param    maIndex    @b{(input)} Index for MA identification used by
*                                  SNMP.
* @param    mepId      @b{(input)} VLAN ID associated with the MA.
* @param    intIfNum   @b{(input)} Interface on which mep is to be
*                                  created
* @param    val    @b{(output)}    Time interval expressed as t*10 where
*                                  t is Time interval in seconds
*                                  ex: 2.5s = 250
*
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1agCfmMepFngAlarmTimeGet(L7_uint32 mdIndex, L7_uint32 maIndex,
                                    L7_uint32 mepId, L7_uint32 intIfNum,
                                    L7_uint32 *val);
/*********************************************************************
* @purpose Set dot1agCfmMepFngAlarmTime -
*          The time that defects must be present
*          before a Fault Alarm is issued (fngAlarmTime. 20.33.3)
*          (default 250 = 2.5s)
*
* @param    mdIndex    @b{(input)} Index used for MD indentification
*                                  used by SNMP.
* @param    maIndex    @b{(input)} Index for MA identification used by
*                                  SNMP.
* @param    mepId      @b{(input)} VLAN ID associated with the MA.
* @param    intIfNum   @b{(input)} Interface on which mep is to be
*                                  created
* @param    val        @b{(input)} Time interval expressed as t*100 
*                                  where t is Time interval in seconds
*                                  ex: 2.5s = 250
*
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1agCfmMepFngAlarmTimeSet(L7_uint32 mdIndex, L7_uint32 maIndex,
                                    L7_uint32 mepId, L7_uint32 intIfNum,
                                    L7_uint32 val);
/*********************************************************************
* @purpose Set dot1agCfmMepFngResetTime -
*          The time that defects must be absent before resetting a
*          Fault Alarm (fngResetTime, 20.33.4) (default 10s).
*
* @param    mdIndex    @b{(input)} Index used for MD indentification
*                                  used by SNMP.
* @param    maIndex    @b{(input)} Index for MA identification used by
*                                  SNMP.
* @param    mepId      @b{(input)} VLAN ID associated with the MA.
* @param    intIfNum   @b{(input)} Interface on which mep is to be
*                                  created
* @param    val        @b{(input)} Time interval expressed as t*100
*                                  where t is Time interval in seconds
*                                  ex: 2.5s = 250
*
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1agCfmMepFngResetTimeGet(L7_uint32 mdIndex, L7_uint32 maIndex,
                                    L7_uint32 mepId, L7_uint32 intIfNum,
                                    L7_uint32 *val);
/*********************************************************************
* @purpose Set dot1agCfmMepFngResetTime -
*          The time that defects must be absent before resetting a
*          Fault Alarm (fngResetTime, 20.33.4) (default 10s).
*
* @param    mdIndex    @b{(input)} Index used for MD indentification
*                                  used by SNMP.
* @param    maIndex    @b{(input)} Index for MA identification used by
*                                  SNMP.
* @param    mepId      @b{(input)} VLAN ID associated with the MA.
* @param    intIfNum   @b{(input)} Interface on which mep is to be
*                                  created
* @param    val        @b{(input)} Time interval expressed as t*100
*                                  where t is Time interval in seconds
*                                  ex: 2.5s = 250
*
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1agCfmMepFngResetTimeSet(L7_uint32 mdIndex, L7_uint32 maIndex,
                                    L7_uint32 mepId, L7_uint32 intIfNum,
                                    L7_uint32 val);
/*********************************************************************
* @purpose Set tdot1agCfmMepTransmitLbmStatus -
*          A Boolean flag set to true by the bridge port to indicate
*          that another LBM may be transmitted.
*
* @param    mdIndex    @b{(input)} Index used for MD indentification
*                                  used by SNMP.
* @param    maIndex    @b{(input)} Index for MA identification used by
*                                  SNMP.
* @param    mepId      @b{(input)} VLAN ID associated with the MA.
* @param    intIfNum   @b{(input)} Interface on which mep is to be
*                                  created
* @param    flag       @b{(input)} A value of L7_TRUE indicates LBMs
*                                  can be sent by the MEP
*
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1agCfmMepTransmitLbmStatusSet(L7_uint32 mdIndex, L7_uint32 maIndex,
                                         L7_uint32 mepId, L7_uint32 intIfNum,
                                         L7_BOOL flag);
/*********************************************************************
* @purpose Set dot1agCfmMepTransmitLbmMessages -
*          The number of Loopback messages to be transmitted.
*
* @param    mdIndex    @b{(input)} Index used for MD indentification
*                                  used by SNMP.
* @param    maIndex    @b{(input)} Index for MA identification used by
*                                  SNMP.
* @param    mepId      @b{(input)} VLAN ID associated with the MA.
* @param    intIfNum   @b{(input)} Interface on which mep is to be
*                                  created
* @param    val        @b{(input)} Number of LBMs to be sent
*
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1agCfmMepTransmitLbmMessagesSet(L7_uint32 mdIndex, L7_uint32 maIndex,
                                           L7_uint32 mepId, L7_uint32 intIfNum,
                                           L7_uint32 val);
/*********************************************************************
* @purpose Set dot1agCfmMepTransmitLbmDestMacAddress
*          dot1agCfmMepTransmitLbmDestIsMepId -
*          The Target MAC Address Field to be transmitted: A unicast
*          destination MAC address. This address will be used if the 
*          value of the column dot1agCfmMepTransmitLbmDestIsMepId is 
*          'false'.
*
* @param    mdIndex    @b{(input)} Index used for MD indentification
*                                  used by SNMP.
* @param    maIndex    @b{(input)} Index for MA identification used by
*                                  SNMP.
* @param    mepId      @b{(input)} VLAN ID associated with the MA.
* @param    intIfNum   @b{(input)} Interface on which mep is to be
*                                  created
* @param    dmac       @b{(input)} DMAC for the MEP to which LBM is 
*                                  sent
*
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1agCfmMepTransmitLbmDestMacAddressSet(L7_uint32 mdIndex, 
                                                 L7_uint32 maIndex,
                                                 L7_uint32 mepId, 
                                                 L7_uint32 intIfNum,
                                                 L7_uchar8 *dmac);
/*********************************************************************
* @purpose Set dot1agCfmMepTransmitLbmDestIsMepId -
*
*          True indicates that MEPID of the target MEP is used for
*          Loopback transmission. False indicates that unicast destination 
*          MAC address of the target MEP is used for Loopback transmission.
*
* @param    mdIndex    @b{(input)} Index used for MD indentification
*                                  used by SNMP.
* @param    maIndex    @b{(input)} Index for MA identification used by
*                                  SNMP.
* @param    mepId      @b{(input)} VLAN ID associated with the MA.
* @param    intIfNum   @b{(input)} Interface on which mep is to be
*                                  created
* @param    flag       @b{(input)} L7_TRUE/L7_FALSE
*
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1agCfmMepTransmitLbmDestIsMepIdSet(L7_uint32 mdIndex,
                                              L7_uint32 maIndex,
                                              L7_uint32 mepId,
                                              L7_uint32 intIfNum,
                                              L7_BOOL flag);
/*********************************************************************
* @purpose Set dot1agCfmMepTransmitLbmDestMepId,
*          dot1agCfmMepTransmitLbmDestIsMepId -
*          The Maintenance association End Point Identifier of another
*          MEP in the same Maintenance Association to which the LBM is
*          to be sent.This address will be used if the value of the column
*          dot1agCfmMepTransmitLbmDestIsMepId is 'true'.
*
* @param    mdIndex    @b{(input)} Index used for MD indentification
*                                  used by SNMP.
* @param    maIndex    @b{(input)} Index for MA identification used by
*                                  SNMP.
* @param    mepId      @b{(input)} VLAN ID associated with the MA.
* @param    intIfNum   @b{(input)} Interface on which mep is to be
*                                  created
* @param    destMepid  @b{(input)} Desstination MEPID to which LBM is
*                                  to be sent
*
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1agCfmMepTransmitLbmDestMepIdSet(L7_uint32 mdIndex, L7_uint32 maIndex,
                                            L7_uint32 mepId, L7_uint32 intIfNum,
                                            L7_uint32 destMepId);
/*********************************************************************
* @purpose Send an LBM on a give MEP
*
* @param    mdIndex    @b{(input)} Index used for MD indentification
*                                  used by SNMP.
* @param    maIndex    @b{(input)} Index for MA identification used by
*                                  SNMP.
* @param    mepId      @b{(input)} Index to identify a MEP
* @param    destMac    @b{(input)} MAC address of MP to which LBM is
*                                  sent
* @param    destMepid  @b{(input)} Destination MEPID to which LBM is
*                                  to be sent
*
*
* @returns  L7_SUCCESS
* @returns  L7_REQUEST_DENIED
* @returns  L7_ERROR
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1agCfmLbmSend(L7_uint32 mdIndex, L7_uint32 maIndex,
                         L7_uint32 mepId, L7_uchar8 *destMac,
                         L7_uint32 destMepId, L7_uint32 msgCount);
/*********************************************************************
* @purpose Obtain dot1agCfmMepHighestPrDefect - 
*          The highest priority defect that has been present since the
*          MEPs Fault Notification Generator State Machine was last in
*          the FNG_RESET state.
*
* @param    mdIndex    @b{(input)} Index used for MD indentification
*                                  used by SNMP.
* @param    maIndex    @b{(input)} Index for MA identification used by
*                                  SNMP.
* @param    mepId      @b{(input)} VLAN ID associated with the MA.
* @param    intIfNum   @b{(input)} Interface on which mep is to be
*                                  created
* @param    val        @b{(input)} Highest priorty defect value
*                                  Dot1agCfmHighestDefectPri_t
*
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1agCfmMepHighestPrDefectGet(L7_uint32 mdIndex, L7_uint32 maIndex,
                                       L7_uint32 mepId, L7_uint32 intIfNum,
                                       L7_uint32 *val);
/*********************************************************************
* @purpose Obtain dot1agCfmMepDefects - MEP defects
*
*
* @param    mdIndex    @b{(input)} Index used for MD indentification
*                                  used by SNMP.
* @param    maIndex    @b{(input)} Index for MA identification used by
*                                  SNMP.
* @param    mepId      @b{(input)} VLAN ID associated with the MA.
* @param    intIfNum   @b{(input)} Interface on which mep is to be
*                                  created
* @param    val        @b{(input)} MEP defect
*                                  Dot1agCfmMepDefects_t
*
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1agCfmMepDefectsGet(L7_uint32 mdIndex, L7_uint32 maIndex,
                               L7_uint32 mepId, L7_uint32 intIfNum,
                               L7_uint32 *val);
/*********************************************************************
* @purpose Obtain dot1agCfmMepCcmSequenceErrors - The total number of 
*          out-of-sequence CCMs received from all remote MEPs.
*
*
* @param    mdIndex    @b{(input)} Index used for MD indentification
*                                  used by SNMP.
* @param    maIndex    @b{(input)} Index for MA identification used by
*                                  SNMP.
* @param    mepId      @b{(input)} VLAN ID associated with the MA.
* @param    intIfNum   @b{(input)} Interface on which mep is to be
*                                  created
* @param    val        @b{(input)}  CCM out of sequence rx pkt count
*
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1agCfmMepCcmSequenceErrors(L7_uint32 mdIndex, L7_uint32 maIndex,
                               L7_uint32 mepId, L7_uint32 intIfNum,
                               L7_uint32 *val);
/*********************************************************************
* @purpose Obtain dot1agCfmMepCcmSequenceErrors - The total number of
*          out-of-sequence CCMs received from all remote MEPs.
*
*
* @param    mdIndex    @b{(input)} Index used for MD indentification
*                                  used by SNMP.
* @param    maIndex    @b{(input)} Index for MA identification used by
*                                  SNMP.
* @param    mepId      @b{(input)} VLAN ID associated with the MA.
* @param    intIfNum   @b{(input)} Interface on which mep is to be
*                                  created
* @param    val        @b{(input)}  CCM out of sequence rx pkt count
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1agCfmMepCcmSequenceErrorsGet(L7_uint32 mdIndex, L7_uint32 maIndex,
                                         L7_uint32 mepId, L7_uint32 intIfNum,
                                         L7_uint32 *val);
/*********************************************************************
* @purpose Obtain dot1agCfmMepCciSentCcms - The total number of
*          Continuity Check messages transmitted.
*
*
* @param    mdIndex    @b{(input)} Index used for MD indentification
*                                  used by SNMP.
* @param    maIndex    @b{(input)} Index for MA identification used by
*                                  SNMP.
* @param    mepId      @b{(input)} VLAN ID associated with the MA.
* @param    intIfNum   @b{(input)} Interface on which mep is to be
*                                  created
* @param    val        @b{(input)}  CCM pkt tx count
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1agCfmMepCciSentCcmsGet(L7_uint32 mdIndex, L7_uint32 maIndex,
                                   L7_uint32 mepId, L7_uint32 intIfNum,
                                   L7_uint32 *val);
/*********************************************************************
* @purpose Obtain dot1agCfmMepNextLbmTransId - 
*          Next sequence number/transaction identifier to be sent in a
*          Loopback message. This sequence number can be zero because
*          it wraps around.
*
*
* @param    mdIndex    @b{(input)} Index used for MD indentification
*                                  used by SNMP.
* @param    maIndex    @b{(input)} Index for MA identification used by
*                                  SNMP.
* @param    mepId      @b{(input)} VLAN ID associated with the MA.
* @param    intIfNum   @b{(input)} Interface on which mep is to be
*                                  created
* @param    val        @b{(input)}  Next LBM transaction Identifier
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1agCfmMepNextLbmTransIdGet(L7_uint32 mdIndex, L7_uint32 maIndex,
                                      L7_uint32 mepId, L7_uint32 intIfNum,
                                      L7_uint32 *val);
/*********************************************************************
* @purpose Obtain dot1agCfmMepLbrIn -
*          Total number of valid, in-order Loopback Replies received.
*
*
* @param    mdIndex    @b{(input)} Index used for MD indentification
*                                  used by SNMP.
* @param    maIndex    @b{(input)} Index for MA identification used by
*                                  SNMP.
* @param    mepId      @b{(input)} VLAN ID associated with the MA.
* @param    intIfNum   @b{(input)} Interface on which mep is to be
*                                  created
* @param    val        @b{(input)}  LBR rx count
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1agCfmMepLbrInGet(L7_uint32 mdIndex, L7_uint32 maIndex,
                             L7_uint32 mepId, L7_uint32 intIfNum,
                             L7_uint32 *val);
/*********************************************************************
* @purpose Obtain dot1agCfmMepLbrInOutOfOrder -
*          The total number of valid, out-of-order Loopback Replies
*          received.
*
*
* @param    mdIndex    @b{(input)} Index used for MD indentification
*                                  used by SNMP.
* @param    maIndex    @b{(input)} Index for MA identification used by
*                                  SNMP.
* @param    mepId      @b{(input)} VLAN ID associated with the MA.
* @param    intIfNum   @b{(input)} Interface on which mep is to be
*                                  created
* @param    val        @b{(input)} LBR rx out-of-order count
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1agCfmMepLbrInOutOfOrderGet(L7_uint32 mdIndex, L7_uint32 maIndex,
                                       L7_uint32 mepId, L7_uint32 intIfNum,
                                       L7_uint32 *val);
/*********************************************************************
* @purpose Obtain dot1agCfmMepLbrBadMsdu -
*          The total number of LBRs received whose
*          mac_service_data_unit did not match (except for the OpCode)
*          that of the corresponding LBM (20.2.3).
*
*
* @param    mdIndex    @b{(input)} Index used for MD indentification
*                                  used by SNMP.
* @param    maIndex    @b{(input)} Index for MA identification used by
*                                  SNMP.
* @param    mepId      @b{(input)} VLAN ID associated with the MA.
* @param    intIfNum   @b{(input)} Interface on which mep is to be
*                                  created
* @param    val        @b{(input)} Lbr rx count with mismatch 
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1agCfmMepLbrBadMsduGet(L7_uint32 mdIndex, L7_uint32 maIndex,
                                  L7_uint32 mepId, L7_uint32 intIfNum,
                                  L7_uint32 *val);

/*********************************************************************
* @purpose Set dot1agCfmMepTransmitLtmTargetMacAddress
*          The Target MAC Address Field to be transmitted: A unicast
*          destination MAC address. This address will be used if the 
*          value of the column dot1agCfmMepTransmitLtmTargetIsMepId is 
*          'false'.
*
* @param    mdIndex    @b{(input)} Index used for MD indentification
*                                  used by SNMP.
* @param    maIndex    @b{(input)} Index for MA identification used by
*                                  SNMP.
* @param    mepId      @b{(input)} VLAN ID associated with the MA.
* @param    intIfNum   @b{(input)} Interface on which mep is to be
*                                  created
* @param    dmac       @b{(input)} DMAC for the MEP to which LTM is
*                                  sent
*
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1agCfmMepTransmitLtmTargetMacAddressSet(L7_uint32 mdIndex,
                                                   L7_uint32 maIndex,
                                                   L7_uint32 mepId,
                                                   L7_uint32 intIfNum,
                                                   L7_uchar8 *dmac);
/*********************************************************************
* @purpose  Set dot1agCfmMepTransmitLtmTargetMepId
*           An indication of the Target MAC Address Field to be
*           transmitted: The Maintenance association End Point Identifier 
*           of another MEP in the same Maintenance Association
*           This address will be used if the value of the column
*           dot1agCfmMepTransmitLtmTargetIsMepId is 'true'.
*
* @param    mdIndex    @b{(input)} Index used for MD indentification
*                                  used by SNMP.
* @param    maIndex    @b{(input)} Index for MA identification used by
*                                  SNMP.
* @param    mepId      @b{(input)} VLAN ID associated with the MA.
* @param    intIfNum   @b{(input)} Interface on which mep is to be
*                                  created
* @param    destMepId  @b{(input)} MEPID for the MEP to which LTM is
*                                  sent
*
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @notes
*
* @end
*******************************************************************/
L7_RC_t usmDbDot1agCfmMepTransmitLtmTargetMepIdSet(L7_uint32 mdIndex,
                                              L7_uint32 maIndex,
                                              L7_uint32 mepId,
                                              L7_uint32 intIfNum,
                                              L7_uint32 destMepId);
/*********************************************************************
* @purpose  Set dot1agCfmMepTransmitLtmTargetIsMepId
*           True indicates that MEPID of the target MEP is used for
*           Linktrace transmission. False indicates that unicast 
*           destination MAC address of the target MEP is used for 
*           Loopback transmission.
*
* @param    mdIndex    @b{(input)} Index used for MD indentification
*                                  used by SNMP.
* @param    maIndex    @b{(input)} Index for MA identification used by
*                                  SNMP.
* @param    mepId      @b{(input)} VLAN ID associated with the MA.
* @param    intIfNum   @b{(input)} Interface on which mep is to be
*                                  created
* @param    flag  @b{(input)}  L7_TRUE/L7_FALSE
*
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @notes
*
* @end
*******************************************************************/
L7_RC_t usmDbDot1agCfmMepTransmitLtmTargetIsMepIdSet(L7_uint32 mdIndex,
                                                L7_uint32 maIndex,
                                                L7_uint32 mepId,
                                                L7_uint32 intIfNum,
                                                L7_BOOL flag);
/*********************************************************************
* @purpose  Set dot1agCfmMepTransmitLtmTtl - 
*           The LTM TTL field. Default value, if not specified, is 64.
*           The TTL field indicates the number of hops remaining to the
*           LTM.  Decremented by 1 by each Linktrace Responder that
*           handles the LTM.  The value returned in the LTR is one less
*           than that received in the LTM.  If the LTM TTL is 0 or 1, the
*           LTM is not forwarded to the next hop, and if 0, no LTR is
*           generated.
*
* @param    mdIndex    @b{(input)} Index used for MD indentification
*                                  used by SNMP.
* @param    maIndex    @b{(input)} Index for MA identification used by
*                                  SNMP.
* @param    mepId      @b{(input)} VLAN ID associated with the MA.
* @param    intIfNum   @b{(input)} Interface on which mep is to be
*                                  created
* @param    val        @b{(input)}  TTL for the LTM message
*
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @notes
*
* @end
*******************************************************************/
L7_RC_t usmDbDot1agCfmMepTransmitLtmTtlSet(L7_uint32 mdIndex, L7_uint32 maIndex,
                                      L7_uint32 mepId, L7_uint32 intIfNum,
                                      L7_uint32 val);
/*********************************************************************
* @purpose Set dot1agCfmMepTransmitLtmFlags - The flags field for LTMs
*          transmitted by the MEP. Transmission is initiated by writing
*          a value to the dot1agCfmMepTransmitLtmFlags object
*
*
* @param    mdIndex    @b{(input)} Index used for MD indentification
*                                  used by SNMP.
* @param    maIndex    @b{(input)} Index for MA identification used by
*                                  SNMP.
* @param    mepId      @b{(input)} Index to identify a MEP
* @param    intIfNum   @b{(input)} Interface on which mep is to be
*                                  created
* @param    flag       @b{(input)} A value of 0x01 to start transmission
*
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1agCfmMepTransmitLtmFlagsSet(L7_uint32 mdIndex, L7_uint32 maIndex,
                                         L7_uint32 mepId, L7_uint32 intIfNum,
                                         L7_uchar8 flag);
/*********************************************************************
* @purpose  Create a maintenance end point on an interface given
*           (mdIndex) and (maIndex) and mepId
*           and direction
*
* @param    intIfNum   @b{(input)} Interface on which mep is to be
*                                  created
* @param    mdIndex    @b{(input)} Index used for MD indentification
*                                  used by SNMP.
* @param    maIndex    @b{(input)} Index for MA identification used by
*                                  SNMP. Use L7_NULL for other UI's
* @param    mepId      @b{(input)} Index to identify a MEP
* @param    direction  @b{(input)} Direction of MEP to be created(UP/Down)
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
* @returns  L7_ALREADY_CONFIGURED
*
* @notes
*
* @end
**********************************************************************/
L7_RC_t usmDbDot1agMepCreate(L7_uint32 intIfNum, L7_uint32 mdIndex,
                        L7_uint32 maIndex, L7_uint32 mepId,
                        Dot1agCfmMpDirection_t direction);
/*********************************************************************
* @purpose  Delete a maintenance end point on an interface given
*           (mdIndex) and (maIndex) and mepId
*
* @param    intIfNum   @b{(input)} Interface on which mep is to be
*                                  deleted
* @param    mdIndex    @b{(input)} Index used for MD indentification
*                                  used by SNMP. Use L7_NULL for
*                                  other UI's
* @param    maIndex    @b{(input)} Index for MA identification used by
*                                  SNMP. Use L7_NULL for other UI's
* @param    mepId      @b{(input)} Index used to identify a MEP.
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @notes
*
* @end
**********************************************************************/
L7_RC_t usmDbDot1agMepDelete(L7_uint32 intIfNum, L7_uint32 mdIndex,
                             L7_uint32 maIndex, L7_uint32 mepId);
/*********************************************************************
* @purpose  Verify if an entry with given values exists in LTR Table
*
* @param    mdIndex    @b{(input)} Index used for MD indentification
* @param    maIndex    @b{(input)} Index for MA identification
* @param    mepId      @b{(input)} Index to identify a MEP
* @param    ltrSeqNumber    @b{(input)} Index used to identify the
*                                       remote MEP which sent the LTR
* @param    ltrReceiveOrder @b{(input)} An index to distinguish
*                                       among multiple LTRs with the same
*                                       LTR Transaction Identifier field
*                                       value.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1agCfmLTREntryIsValid(L7_uint32 mdIndex, L7_uint32 maIndex,
                                 L7_uint32 mepId, L7_uint32 ltrSeqNumber,
                                 L7_uint32 ltrReceiveOrder);
/*********************************************************************
* @purpose  Obtain Linktrace replies received by a specific MEP in
*           response to a linktrace message. As
*           defined by section 17.5 802.1ag d8-1
*
* @param    mdIndex    @b{(input)} Index used for MD indentification
*                                  used by SNMP.
* @param    maIndex    @b{(input)} Index for MA identification used by
 *                                  SNMP.
* @param    mepId      @b{(input)} VLAN ID associated with the MA.
* @param    ltrSeqNumber    @b{(input)} Index used to identify the
*                                       remote MEP which sent the LTR
* @param    ltrReceiveOrder @b{(input)} An index to distinguish
*                                       among multiple LTRs with the same
*                                       LTR Transaction Identifier field
*                                       value.
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @notes  Use L7_NULL to obtain first valid entry
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1agCfmLTREntryNextGet(L7_uint32 *mdIndex, L7_uint32 *maIndex,
                                 L7_uint32 *mepId, L7_uint32 *ltrSeqNumber,
                                 L7_uint32 *ltrReceiveOrder);
/*********************************************************************
* @purpose  Obtain Linktrace replies received by a specific MEP in 
*           response to a linktrace message. As
*           defined by section 17.5 802.1ag d8-1
*
* @param    mdIndex    @b{(input)} Index used for MD indentification
*                                  used by SNMP.
* @param    maIndex    @b{(input)} Index for MA identification used by
*                                  SNMP.
* @param    mepId      @b{(input)} VLAN ID associated with the MA.
* @param    ltrSeqNumber    @b{(input)} Index used to identify the 
*                                       remote MEP which sent the LTR
* @param    ltrReceiveOrder @b{(input)} An index to distinguish 
*                                       among multiple LTRs with the same 
*                                       LTR Transaction Identifier field 
*                                       value.
* @param    paramCode  @b{(input)} LTR parameter defined by
*                                  dot1agCfmLtrEntry_t
* @param    val       @b{(output)} Param value
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1agCfmLTRParamGet(L7_uint32 mdIndex, L7_uint32 maIndex,
                             L7_uint32 mepId, L7_uint32 ltrSeqNumber,
                             L7_uint32 ltrReceiveOrder, L7_uint32 paramCode, 
                             void *val);
/*********************************************************************
* @purpose  Verify if an entry with given values exists in RMEP Table
*
* @param    mdIndex    @b{(input)} Index used for MD indentification
* @param    maIndex    @b{(input)} Index for MA identification
* @param    mepId      @b{(input)} Index to identify a MEP
* @param    rmepId     @b{(input)} Index used to identify the
*                                       remote MEP
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1agCfmRMepEntryIsValid(L7_uint32 mdIndex, L7_uint32 maIndex,
                                  L7_uint32 mepId, L7_uint32 rmepId);
/*********************************************************************
* @purpose  Obtain RMep entries collected by a specific MEP.
*           As defined by section 17.5 802.1ag d8-1
*
* @param    mdIndex    @b{(input)} Index used for MD indentification
*                                  used by SNMP.
* @param    maIndex    @b{(input)} Index for MA identification used by
*                                  SNMP.
* @param    mepId      @b{(input)} VLAN ID associated with the MA.
* @param    rmepId     @b{(input)} Index used to identify the
*                                       remote MEP
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @notes Use L7_NULL to obtain next valid entry
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1agCfmRMepEntryNextGet(L7_uint32 *mdIndex, L7_uint32 *maIndex,
                                  L7_uint32 *mepId, L7_uint32 *rmepId);
/*********************************************************************
* @purpose  Obtain RMep entries collected by a specific MEP.
*           As defined by section 17.5 802.1ag d8-1
*
* @param    mdIndex    @b{(input)} Index used for MD indentification
*                                  used by SNMP.
* @param    maIndex    @b{(input)} Index for MA identification used by
*                                  SNMP.
* @param    mepId      @b{(input)} VLAN ID associated with the MA.
* @param    rmepId     @b{(input)} Index used to identify the
*                                       remote MEP
* @param    paramCode  @b{(input)} RMep parameter defined by
*                                  dot1agCfmMepDbEntry_t
* @param    val       @b{(output)} Param value
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1agCfmRMepParamGet(L7_uint32 mdIndex, L7_uint32 maIndex,
                              L7_uint32 mepId, L7_uint32 rmepId,
                              L7_uint32 paramCode, void *val);
/*********************************************************************
* @purpose  Verify if an entry with given values exists in CFM Stack
*
* @param    intIfNum   @b{(input)} Interface on which MP is configured
* @param    primVID    @b{(input)} VLAN ID associated with the MA.
* @param    mdLevel    @b{(input)} Domain level
* @param    direction  @b{(output)} Direction of MP (Up or Down)
*                                   For MIP it is always Up
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1agCfmStackEntryIsValid(L7_uint32 intIfNum, L7_uint32 primVID,
                                   L7_int32 mdLevel,
                                   Dot1agCfmMpDirection_t direction);
/*********************************************************************
* @purpose  Obtain CFM MP Stack entries configured
*           As defined by section 17.5 802.1ag d8-1
*
*
* @param    intIfNum   @b{(input)} Interface on which MP is configured
* @param    primVID    @b{(input)} VLAN ID associated with the MA.
* @param    mdLevel    @b{(input)} Domain level to be set
* @param    direction  @b{(output)} Direction of MP (Up or Down)
*                                   For MIP it is always Up
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @notes Use L7_NULL to obtain next valid entry
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1agCfmStackEntryNextGet(L7_uint32 *intIfNum, L7_uint32 *primVID,
                                   L7_int32 *mdLevel, 
                                   Dot1agCfmMpDirection_t *direction);

/*********************************************************************
* @purpose  Obtain CFM MP Stack entries configured
*           As defined by section 17.5 802.1ag d8-1
*
* @param    intIfNum   @b{(input)} Interface on which MP is configured
* @param    primVID    @b{(input)} VLAN ID associated with the MA.
* @param    mdLevel    @b{(input)} Domain level to be set
* @param    direction  @b{(output)} Direction of MP (Up or Down)
*                                   For MIP it is always Up
* @param    paramCode  @b{(input)} CFM Stack MP parameter defined by
*                                  dot1agCfmStackEntry_t
* @param    val       @b{(output)} Param value
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
* @returns  L7_NOT_EXIST
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1agCfmStackParamGet(L7_uint32 intIfNum, L7_uint32 primVID,
                               L7_int32 mdLevel, Dot1agCfmMpDirection_t direction,
                               L7_uint32 paramCode, void *val);
/*********************************************************************
* @purpose  Enable/Disable a maintenance intermediate point on an 
*           interface for a given domain level
*
* @param    intIfNum   @b{(input)} Interface on which mep is to be
*                                  created/deleted
* @param    mdLevel    @b{(input)} MD level of the MD for which MEP
*                                  is to be created/deleted.
* @param    mode       @b{(input)} L7_ENABLE/L7_DISABLE
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @notes    Refer to 802.1ag d8-1 section 22.2.3
*
* @end
**********************************************************************/
L7_RC_t usmDbDot1agMipModeSet(L7_uint32 intIfNum, L7_int32 mdLevel, 
                         L7_uint32 mode);

/*********************************************************************
* @purpose  Enable/Disable a maintenance intermediate point on an
*           interface for a given domain level
*
* @param    intIfNum   @b{(input)} Interface on which mep is to be
*                                  created/deleted
* @param    mdLevel    @b{(input)} MD level of the MD for which MEP
*                                  is to be created/deleted.
* @param    mode       @b{(output)} L7_ENABLE/L7_DISABLE
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @notes    Refer to 802.1ag d8-1 section 22.2.3
*
* @end
**********************************************************************/
L7_RC_t usmDbDot1agMipModeGet(L7_uint32 intIfNum, L7_int32 mdLevel,
                         L7_uint32 *mode);
/*********************************************************************
* @purpose  Obtain MAC Address of the
*           maintenance intermediate point on an interface
*
* @param    intIfNum   @b{(input)} Interface on which mep is to be
*                                  created
* @param    mdIndex    @b{(input)} Index used for MD indentification
*                                  used by SNMP.
* @param    macAddr    @b{(output)} MAC Addr of the MEP
*
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1agCfmMipMacAddressGet(L7_uint32 intIfNum, L7_uint32 mdIndex,
                                  L7_uchar8 *macAddr);
/*********************************************************************
* @purpose  Enable/Disable CFM
*
* @param    mode       @b{(input)} L7_ENABLE/L7_DISABLE
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @notes    Not standard config param
*
* @end
**********************************************************************/
L7_RC_t usmDbDot1agCfmModeSet(L7_uint32 mode);
/*********************************************************************
* @purpose  Enable/Disable mode of CFM
*
* @param    mode       @b{(output)} L7_ENABLE/L7_DISABLE
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @notes    Non standard config param
*
* @end
**********************************************************************/
L7_RC_t usmDbDot1agCfmModeGet(L7_uint32 *mode);
/*********************************************************************
* @purpose  Obtain archive hold time. The time interval after which
*           RMEPs in RMEP_FAILED state are removed
*
* @param    timerVal       @b{(output)}
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @notes    Non standard config param
*
* @end
**********************************************************************/
L7_RC_t usmDbDot1agCfmArchiveHoldTimeGet(L7_uint32 *timerVal);
/*********************************************************************
* @purpose  Set archive hold time. The time interval after which
*           RMEPs in RMEP_FAILED state are removed
*
* @param    timerVal       @b{(input)}
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @notes    Non standard config param
*
* @end
**********************************************************************/
L7_RC_t usmDbDot1agCfmArchiveHoldTimeSet(L7_uint32 timerVal);
/*********************************************************************
* @purpose  Determine if the interface type is valid for dot1ag
*
* @param    sysIntfType  @b{(input)} interface type
*
* @returns  L7_TRUE   Valid interface type
* @returns  L7_FALSE
*
* @comments none
*
* @end
*********************************************************************/
L7_BOOL usmDbDot1agIsValidIntfType(L7_uint32 sysIntfType);
/*********************************************************************
* @purpose  Determine if the interface is valid for dot1ag
*
* @param    intIfNum  @b{(input)} internal interface number
*
* @returns  L7_TRUE   Supported interface
* @returns  L7_FALSE
*
* @comments none
*
* @end
*********************************************************************/
L7_BOOL usmDbDot1agIsValidIntf(L7_uint32 intIfNum);
/*********************************************************************
* @purpose  Get the next interface that is valid for dot1ag
*
* @param    *pIntIfNum  @b{(input/output)} internal interface number
*
* @returns  L7_SUCCESS, if a valid interface is found
* @returns  L7_FAILURE, otherwise
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1agValidIntfNextGet(L7_uint32 *pIntIfNum);
/*********************************************************************
* @purpose  Checks if the interface/VLAN ID can be enabled for dot1ag
*
* @param    intIfNum  @b{(input)} Internal Interface Number
*
* @returns  L7_TRUE, if interface can be enabled
* @returns  L7_FALSE, if interface cannot be enabled
*
* @notes    Acceptability criteria
*              1. Must not be the mirror dest port   &&
*              2. Must not be a LAG member           &&
*              3. Must not be enabled for routing    &&
*              4. Must be in the Forwarding state
*           This function only checks the above criteria.  All other
*           checking (i.e. validity of interface number) can be done
*           in dot1agIntfCheck() or dot1agIntfValidate().  This function
*           can be used to determine when an interface can be configured,
*           but, not enabled.
*
*
* @end
*********************************************************************/
L7_BOOL usmDbDot1agIntfCanBeEnabled(L7_uint32 intIfNum);
/*********************************************************************
* @purpose  Checks if an MP on this intIfNum can be operational
*           for dot1ag
*
* @param    intIfNum  @b{(input)} Internal Interface Number
*
* @returns  L7_TRUE, if MP can be operational
* @returns  L7_FALSE, if MP cannot be operational
*
* @notes    Acceptability criteria
*              1. Must not be the mirror dest port   &&
*              2. Must not be a LAG member           &&
*              3. Must not be enabled for routing    &&
*              4. Must be in the Forwarding state
*           This function only checks the above criteria.  All other
*           checking (i.e. validity of interface number) can be done
*           in dot1agIntfCheck() or dot1agIntfValidate().  This function
*           can be used to determine when an interface can be configured,
*           but, not enabled.
*
*
* @end
*********************************************************************/
L7_BOOL usmDbDot1agIntfCanBeOperational(L7_uint32 intIfNum);
/*********************************************************************
* @purpose Send an LTM on a give MEP
*
* @param    mdIndex    @b{(input)} Index used for MD indentification
*                                  used by SNMP.
* @param    maIndex    @b{(input)} Index for MA identification used by
*                                  SNMP.
* @param    mepId      @b{(input)} Index to identify a MEP
* @param    destMac    @b{(input)} MAC address of MP to which LTM is
*                                  sent
* @param    destMepid  @b{(input)} Destination MEPID to which LTM is
*                                  to be sent
* @param    ttl        @b{(input)} Max hops to be used in LTM
*
*
* @returns  L7_SUCCESS
* @returns  L7_REQUEST_DENIED
* @returns  L7_ERROR
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1agCfmLtmSend(L7_uint32 mdIndex, L7_uint32 maIndex,
                              L7_uint32 mepId, L7_uchar8 *destMac,
                              L7_uint32 destMepId, L7_uchar8 ttl);
/*********************************************************************
* @purpose Clear Traceroute Cache
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_REQUEST_DENIED
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t usmDbDot1agCfmTraceRouteCacheClear();
/* Missing APIs */
/*********************************************************************
* @purpose  Set usmDbDot1agCfmMepTransmitLtmEgressIdentifierSet -
*           The LTM TTL field. Default value, if not specified, is 64.
*           The TTL field indicates the number of hops remaining to the
*           LTM.  Decremented by 1 by each Linktrace Responder that
*           handles the LTM.  The value returned in the LTR is one less
*           than that received in the LTM.  If the LTM TTL is 0 or 1, the
*           LTM is not forwarded to the next hop, and if 0, no LTR is
*           generated.
*
* @param    mdIndex    @b{(input)} Index used for MD indentification
*                                  used by SNMP.
* @param    maIndex    @b{(input)} Index for MA identification used by
*                                  SNMP.
* @param    mepId      @b{(input)} VLAN ID associated with the MA.
* @param    intIfNum   @b{(input)} Interface on which mep is to be
*                                  created
* @param    val        @b{(input)}  Egress Identifier for the LTM message
*
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR
*
* @notes
*
* @end
*******************************************************************/
L7_RC_t usmDbDot1agCfmMepTransmitLtmEgressIdentifierSet(L7_uint32 mdIndex, L7_uint32 maIndex,
                                      L7_uint32 mepId, L7_uint32 intIfNum,
                                      L7_uchar8 *val);
L7_RC_t usmDbDot1agCfmMepTransmitLbmVlanDropEnableSet(L7_uint32 mdIndex, L7_uint32 maIndex,
                                      L7_uint32 mepId, L7_uint32 intIfNum,
                                      L7_uint32 val);
L7_RC_t usmDbDot1agCfmMepTransmitLbmVlanPrioritySet(L7_uint32 mdIndex, L7_uint32 maIndex,
                                      L7_uint32 mepId, L7_uint32 intIfNum,
                                      L7_uint32 val);
L7_RC_t usmDbDot1agCfmMepTransmitLbmDataTlvSet(L7_uint32 mdIndex, L7_uint32 maIndex,
                                      L7_uint32 mepId, L7_uint32 intIfNum,
                                      L7_uchar8 *data);
L7_BOOL usmDbDot1agIsMaIndexValid(L7_uint32 mdIndex, L7_uint32 maIndex);
L7_RC_t usmDbDot1agMdFormatGet(L7_uint32 mdIndex, L7_uint32 *mdFormat);
L7_RC_t usmDbDot1agCfmMdFormatSet(L7_uint32 mdIndex, L7_uint32 mdFormat);

/*********************************************************************
* @purpose  Obtain a next valid mip value given a maintenance domain
*
* @param    mdIndex  @b{(input/output)} Index value used by SNMP
*                                       Use L7_NULL to obtain first valid
*                                       value.
*           ifIndex  @b{(input/output)} Interface Index.
*                                       Use L7_NULL to obtain first valid
*                                       value.  
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @returns  L7_ERROR
*
* @notes    none
*
* @end
**********************************************************************/
L7_RC_t usmDbDot1agMipNextGet(L7_uint32 *mdIndex, L7_uint32 *ifIndex);

#endif /* INCLUDE_USMDB_DOT1AG_H */
