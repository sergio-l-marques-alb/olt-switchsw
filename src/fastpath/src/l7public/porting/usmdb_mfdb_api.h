/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename src\l7public\porting\usmdb_mfdb_api.h
*
* @purpose Provide interface to MFDB APIs for unitmgr component
*
* @component unitmgr
*
* @comments In an SVL environment, the VLAN ID, in the VLAN ID-MAC 
*           address combination shall be 0 (zero), or the default value.
*
* @create 03/27/2002
*
* @author rjindal
*
* @end
*
*********************************************************************/

/*************************************************************
                    
*************************************************************/


#ifndef USMDB_MFDB_API_H
#define USMDB_MFDB_API_H


#include "l7_common.h"
#include "usmdb_common.h"
#include "comm_mask.h"


/*********************************************************************
*
* @purpose  To get the information of a VLANID-MAC entry in the
*           MFDB table.
*
* @param    L7_uint32        UnitIndex  @b((input)) the operation unit
* @param    usmdbMfdbEntry_t  *entry     @b((input)) pointer to structure
*
* @returns  L7_SUCCESS, if the entry is found
* @returns  L7_FAILURE, if the entry is not found
*
* @comments The entry's information gets copied in the input
*           'entry' structure.
*
* @end
*
*********************************************************************/
 L7_RC_t usmDbMfdbEntryInfoGet(L7_uint32 UnitIndex, 
                                     usmdbMfdbEntry_t *entry);

/*********************************************************************
*
* @purpose  Given a VLANID-MAC entry, get the next VLANID-MAC entry.
*
* @param    L7_uint32  UnitIndex  @b((input)) the operation unit
* @param    L7_uchar8  *vidMac    @b((input)) pointer to vid-mac combo.
*                                 (2-byte vlan id + 6-byte mac address)
* @param    L7_uint32  *nextProt  @b((input)) pointer to protocol type
*                                 (see L7_MFDB_PROTOCOL_TYPE_t)   
* @param    usmdbMfdbUserInfo_t   *compInfo  @b((output)) pointer to next
*                                            component's info structure
*
* @returns  L7_SUCCESS, if the next entry is found
* @returns  L7_FAILURE, if the next entry is not found
*
* @notes    The entry is indexed by vlanId, mac address,
*           and the protocol type.
* @notes    To get the first entry, the reference entry shall
*           be 0 (zero).
* @notes    The next entry's information gets copied in the input
*           'compInfo' structure.
*
* @end
*
*********************************************************************/
 L7_RC_t usmDbMfdbEntryGetNext(L7_uint32 UnitIndex, L7_uchar8 *vidMac, 
                                     L7_uint32 *nextProt, usmdbMfdbUserInfo_t *compInfo);

/*********************************************************************
*
* @purpose  Given a user's VLANID-MAC entry, get the next VLANID-MAC
*           entry of the same user.
*
* @param    L7_uint32  UnitIndex  @b((input)) the operation unit
* @param    L7_uchar8  *vidMac    @b((input)) pointer to vid-mac combo.
*                                 (2-byte vlan id + 6-byte mac address)
* @param    L7_uint32  compId     @b((input)) component id, start
* @param    usmdbMfdbUserInfo_t  *compInfo  @b((output)) pointer to next
*                                           component's info structure
*
* @returns  L7_SUCCESS, if the next entry is found
* @returns  L7_FAILURE, if the next entry is not found
*
* @notes    The entry is indexed by vlanId, mac address,
*           and component id.
* @notes    To get the first entry, the reference entry shall
*           be 0 (zero).
* @notes    The entry's information gets copied in the input
*           'compInfo' structure.
*
* @end
*
*********************************************************************/
 L7_RC_t usmDbMfdbComponentEntryGetNext(L7_uint32 UnitIndex, L7_uchar8 *vidMac, 
                                              L7_uint32 compId, usmdbMfdbUserInfo_t *compInfo);

/*********************************************************************
*
* @purpose  To get the maximum number of entries that the MFDB table
*           can hold.
*
* @param    L7_uint32  UnitIndex    @b((input)) the operation unit
* @param    L7_uint32  *maxEntries  @b((output)) pointer to the number
*                                                of total entries
*
* @returns  L7_SUCCESS
*
* @comments None.
*       
* @end
*
*********************************************************************/
 L7_RC_t usmDbMfdbMaxTableEntriesGet(L7_uint32 UnitIndex, L7_uint32 *maxEntries);

/*********************************************************************
*
* @purpose  To get the most number of entries ever in the MFDB table.
*               
* @param    L7_uint32  UnitIndex     @b((input)) the operation unit
* @param    L7_uint32  *mostEntries  @b((output)) pointer to the number
*                                                 of most entries
*	
* @returns  L7_SUCCESS
*
* @comments None.
*
* @end
*
*********************************************************************/
 L7_RC_t usmDbMfdbMostEntriesGet(L7_uint32 UnitIndex, L7_uint32 *mostEntries);

/*********************************************************************
*
* @purpose  To get the number of current entries in the MFDB table.
*               
* @param    L7_uint32  UnitIndex     @b((input)) the operation unit
* @param    L7_uint32  *currEntries  @b((output)) pointer to the number
*                                                 of current entries
*	
* @returns  L7_SUCCESS
*
* @comments None.
*
* @end
*
*********************************************************************/
 L7_RC_t usmDbMfdbCurrEntriesGet(L7_uint32 UnitIndex, L7_uint32 *currEntries);

/*********************************************************************
*
* @purpose  To check if a VLANID-MAC entry exists in the MFDB table.
*          
* @param    L7_uint32  UnitIndex  @b((input)) the operation unit
* @param    L7_uchar8  *vidMac    @b((input)) pointer to vid-mac combo.
*                                 (2-byte vlan id + 6-byte mac address)
*	
* @returns  L7_SUCCESS, if the entry is found
* @returns  L7_FAILURE, if the entry is not found
*
* @comments None.
*
* @end
*
*********************************************************************/
 L7_RC_t usmDbMfdbEntryFwdIntfGetExact(L7_uint32 UnitIndex, L7_uchar8 *vidMac);

/*********************************************************************
*
* @purpose  Given a VLANID-MAC entry, get the forwarding interfaces
*           of the next VLANID-MAC entry.
*
* @param    L7_uint32       UnitIndex  @b((input)) the operation unit
* @param    L7_uchar8       *vidMac    @b((input)) pointer to vid-mac combo.
*                                      (2-byte vlan id + 6-byte mac address)
* @param    L7_uint32       matchType  @b((input)) L7_MATCH_EXACT or
*                                                  L7_MATCH_GETNEXT
* @param    L7_INTF_MASK_t  *absMask   @b((input)) pointer to mask
*
* @returns  L7_SUCCESS, if the next entry is found
* @returns  L7_FAILURE, if the next entry is not found
*
* @notes    The entry is indexed by VLANID-MAC address combination.
* @notes    To get the first entry's forwarding interfaces, the 
*           reference entry's VLANID-MAC address shall be 0 (zero).
* @notes    To get the forwarding interfaces of a given VLANID-MAC
*           entry, the 'matchType' shall be L7_MATCH_EXACT.
* @notes    To get the forwarding interfaces of all VLANID-MAC entries,
*           the 'matchType' shall be L7_MATCH_GETNEXT.
*
* @end
*
*********************************************************************/
 L7_RC_t usmDbMfdbEntryFwdIntfGetNext(L7_uint32 UnitIndex, L7_uchar8 *vidMac, 
                                            L7_uint32 matchType, L7_INTF_MASK_t *absMask);

/*********************************************************************
*
* @purpose  Given a VLANID-MAC entry, get the forwarding interfaces
*           of the next VLANID-MAC entry.
*
* @param    L7_uint32       UnitIndex  @b((input)) the operation unit
* @param    L7_uchar8       *vidMac    @b((input)) pointer to vid-mac combo.
*                                      (2-byte vlan id + 6-byte mac address)
* @param    L7_uint32       matchType  @b((input)) L7_MATCH_EXACT or
*                                                  L7_MATCH_GETNEXT
* @param    L7_INTF_MASK_t  *absMask   @b((input)) pointer to mask
*
* @returns  L7_SUCCESS, if the next entry is found
* @returns  L7_FAILURE, if the next entry is not found
*
* @notes    The entry is indexed by VLANID-MAC address combination.
* @notes    To get the first entry's forwarding interfaces, the 
*           reference entry's VLANID-MAC address shall be 0 (zero).
* @notes    To get the forwarding interfaces of a given VLANID-MAC
*           entry, the 'matchType' shall be L7_MATCH_EXACT.
* @notes    To get the forwarding interfaces of all VLANID-MAC entries,
*           the 'matchType' shall be L7_MATCH_GETNEXT.
*
* @end
*
*********************************************************************/
 L7_RC_t usmDbMfdbEntryLearntIntfGetNext(L7_uint32 UnitIndex, L7_uchar8 *vidMac, 
                                               L7_uint32 matchType, L7_INTF_MASK_t *absMask);

/*********************************************************************
*
* @purpose  To check if a VLANID-MAC entry, that uses a specific
*           protocol, exists in the MFDB table.
*          
* @param    L7_uint32  UnitIndex  @b((input)) the operation unit
* @param    L7_uchar8  *vidMac    @b((input)) pointer to vid-mac combo.
*                                 (2-byte vlan id + 6-byte mac address)
* @param    L7_uint32  protocol   @b((input)) the protocol type
*                                 (see L7_MFDB_PROTOCOL_TYPE_t)   
*	
* @returns  L7_SUCCESS, if the entry using that protocol exists
* @returns  L7_FAILURE, if the entry using that protocol does not exist
*
* @comments None.
*
* @end
*
*********************************************************************/
 L7_RC_t usmDbMfdbEntryProtocolGetExact(L7_uint32 UnitIndex, 
                                              L7_uchar8 *vidMac, L7_uint32 protocol);

/*********************************************************************
*
* @purpose  Given a VLANID-MAC entry, that uses a specific protocol, 
*           get the protocol of the next VLANID-MAC entry in the
*           MFDB table.
*
* @param    L7_uint32  UnitIndex  @b((input)) the operation unit
* @param    L7_uchar8  *vidMac    @b((input)) pointer to vid-mac combo.
*                                 (2-byte vlan id + 6-byte mac address)
* @param    L7_uint32  *nextProt  @b((input)) pointer to protocol type
*                                 (see L7_MFDB_PROTOCOL_TYPE_t)   
*
* @returns  L7_SUCCESS, if the next protocol is found
* @returns  L7_FAILURE, if the next protocol is not found
*
* @notes    The entry is indexed by vlanId, mac address, and the
*           protocol type.
* @notes    To get the first entry, the reference entry shall
*           be 0 (zero).
* @notes    The next protocol gets copied in the input pointer
*           'nextProt'.
*
* @end
*
*********************************************************************/
 L7_RC_t usmDbMfdbEntryProtocolGetNext(L7_uint32 UnitIndex, 
                                             L7_uchar8 *vidMac, L7_uint32 *nextProt);

/*********************************************************************
*
* @purpose  To get the information of a given VLANID-MAC entry
*           that uses a specific protocol.
*
* @param    L7_uint32  UnitIndex  @b((input)) the operation unit
* @param    L7_uchar8  *vidMac    @b((input)) pointer to vid-mac combo.
*                                 (2-byte vlan id + 6-byte mac address)
* @param    L7_uint32  protocol   @b((input)) the protocol type
*                                 (see L7_MFDB_PROTOCOL_TYPE_t)   
* @param    usmdbMfdbUserInfo_t  *compInfo  @b((output)) pointer to the
*                                           component's info structure
*
* @returns  L7_SUCCESS, if the entry using that protocol is found
* @returns  L7_FAILURE, if the entry using that protocol is not found
*
* @comments None.
*
* @end
*
*********************************************************************/
 L7_RC_t usmDbMfdbEntryProtocolInfoGet(L7_uint32 UnitIndex, L7_uchar8 *vidMac, 
                                             L7_uint32 protocol, usmdbMfdbUserInfo_t *compInfo);



#endif /* USMDB_MFDB_API_H */
