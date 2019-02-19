/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename src\l7public\api\mfdb_api.h
*
* @purpose Multicast Forwarding Database API Header File
*
* @component Multicast Forwarding Database

* @comments None
*
* @create 03/26/2002
*
* @author rjindal
*
* @end
*
*********************************************************************/

/*************************************************************
                    
*************************************************************/


#ifndef INCLUDE_MFDB_API_H
#define INCLUDE_MFDB_API_H


#include "l7_common.h"
#include "fdb_api.h"
#include "comm_mask.h"
#include "defaultconfig.h"
#include "mfdb_exports.h"


#define MFDB_MAX_FUNC_NAME  32

typedef struct
{
 L7_COMPONENT_IDS_t componentId;
#if 0
 L7_uint32         componentId; 
#endif                                    /* user component's ID          */
  L7_uint32         type;                                            /* user component's type        */
  L7_uchar8         description[L7_MFDB_COMPONENT_DESCR_STRING_LEN]; /* user component's description */
  L7_INTF_MASK_t    fwdPorts;                                        /* list of forwarding ports     */
  L7_INTF_MASK_t    fltPorts;                                        /* list of filtering ports      */
} mfdbUserInfo_t;

typedef struct
{
  L7_uint32         vlanId;                                          /* entry's vlan id              */
  L7_uchar8         macAddr[L7_MAC_ADDR_LEN];                        /* entry's mac address          */
  mfdbUserInfo_t    user;                                            /* user information structure   */
} mfdbMemberInfo_t;

/* AVL tree entry structure */
typedef struct mfdbData_s
{
  L7_uchar8         mfdbVidMacAddr[L7_MFDB_KEY_SIZE];                /* AVL key: vlan id + mac address */
  mfdbUserInfo_t    mfdbUser[L7_MFDB_MAX_USERS];                     /* entry-user's information       */
  struct mfdbData_s *next;                                           /* pointer to the next entry      */
} mfdbData_t;

/* MFDB outcall notification events */
typedef enum
{
  MFDB_ENTRY_ADD_NOTIFY,                                             /* add a new entry              */
  MFDB_ENTRY_DELETE_NOTIFY,                                          /* delete an existing entry     */
  MFDB_ENTRY_PORT_ADD_NOTIFY,                                        /* add the port(s) to an entry  */
  MFDB_ENTRY_PORT_DELETE_NOTIFY                                      /* delete port(s) from an entry */
} mfdbNotifyEvents_t;

typedef struct
{
  L7_uint32   registrarID;
  L7_uchar8   func_name[MFDB_MAX_FUNC_NAME];
  L7_RC_t     (*notify_request)(L7_uint32 vlanId, 
                                L7_uchar8 *mac, 
                                L7_INTF_MASK_t fwdMask, 
                                L7_INTF_MASK_t fltMask, 
                                L7_uint32 event, 
                                L7_uint32 userID);
} mfdbNotifyRequest_t;


// PTin added
extern L7_RC_t mfdbEntryModifyType(L7_uint16 vid, L7_uchar8 *macAddr, L7_uint32 newType);

/*********************************************************************
*
* @purpose  To check if a VLANID-MAC entry exists in the MFDB table.
*
* @param    char       *mac    the entry's mac address
* @param    L7_uint32  vlanId  the entry's vlan id
*
* @returns  L7_SUCCESS, if the entry exists
* @returns  L7_NOT_EXIST, if the entry does not exist
*
* @comments The user component has to perform this check before
*           adding the VLANID-MAC entry interfaces. 
*           If L7_SUCCESS, call mfdbEntryPortsAdd()
*           If L7_NOT_EXIST, call mfdbEntryAdd()
*       
* @end
*
*********************************************************************/
extern L7_RC_t mfdbEntryExist(char *mac, L7_uint32 vlanId);

/*********************************************************************
*
* @purpose  To add a new VLANID-MAC entry to the MFDB table.
*
* @param    mfdbMemberInfo_t  *mfdbMemberInfo  pointer to the member
*                                              info structure
*
* @returns  L7_SUCCESS, if the entry is added
* @returns  L7_FAILURE, if the entry already exists
*
* @comments The user provides the VLAN ID, MAC address, component ID,
*           entry description, and forwarding-filtering port masks.
*       
* @end
*
*********************************************************************/
extern L7_RC_t mfdbEntryAdd(mfdbMemberInfo_t *mfdbMemberInfo);

/*********************************************************************
*
* @purpose  To add ports to an existing VLANID-MAC entry in MFDB table.
*
* @param    mfdbMemberInfo_t  *mfdbMemberInfo  pointer to the member
*                                              info structure
*
* @returns  L7_SUCCESS, if the ports are added
* @returns  L7_FAILURE, if the ports are not added
* @returns  L7_NOT_EXIST, if the entry does not exist
*
* @comments The user provides the VLAN ID, MAC address, component ID,
*           entry description, and forwarding-filtering port masks.
*       
* @end
*
*********************************************************************/
extern L7_RC_t mfdbEntryPortsAdd(mfdbMemberInfo_t *mfdbMemberInfo);

/*********************************************************************
*
* @purpose  To delete a VLANID-MAC entry from the MFDB table.
*
* @param    mfdbMemberInfo_t  *mfdbMemberInfo  pointer to the member
*                                              info structure
*
* @returns  L7_SUCCESS, if the entry is deleted
* @returns  L7_NOT_EXIST, if the entry does not exist, or
*                         the user-component wanting to delete itself
*                         from the entry does not exist
*
* @comments The user provides the VLAN ID, MAC address, and
*           component ID.
*       
* @end
*
*********************************************************************/
extern L7_RC_t mfdbEntryDelete(mfdbMemberInfo_t *mfdbMemberInfo);

/*********************************************************************
*
* @purpose  To delete ports from a VLANID-MAC entry in MFDB table.
*
* @param    mfdbMemberInfo_t  *mfdbMemberInfo  pointer to the member
*                                              info structure
*
* @returns  L7_SUCCESS, if the ports are deleted
* @returns  L7_NOT_EXIST, if the entry does not exist, or
*                         the user-component wanting to delete ports
*                         does not exist
*
* @comments The user provides the VLAN ID, MAC address, component ID,
*           and forwarding-filtering port masks.
*       
* @end
*
*********************************************************************/
extern L7_RC_t mfdbEntryPortsDelete(mfdbMemberInfo_t *mfdbMemberInfo);

/*********************************************************************
*
* @purpose  Given a user's VLANID-MAC entry, get the next VLANID-MAC
*           entry of the same user from the MFDB table. 
*
* @param    mfdbData_t      *pData     pointer to the entry structure
* @param    L7_uint32       userId     specific user component's id
* @param    mfdbUserInfo_t  *userInfo  pointer to the user component's
*                                      info structure
*
* @returns  L7_SUCCESS, if the next user entry is found
* @returns  L7_FAILURE, if the next user entry is not found
*
* @notes    The entry is indexed by vlanId, mac address, and user id
*           which are provided by the user.
* @notes    To get the first entry, the user sent info (i.e., the
*           vlan id, mac address, and user id) shall be 0 (zero).
* @notes    The entry's information gets copied in the input
*           'userInfo' structure.
*
* @end
*
*********************************************************************/
extern L7_RC_t mfdbComponentEntryGetNext(mfdbData_t *pData, L7_uint32 userId, 
                                         mfdbUserInfo_t *userInfo);

/*********************************************************************
*
* @purpose  To get the maximum number of entries that the MFDB table
*           can hold.
*
* @param    L7_uint32  *maxEntries  total number of entries
*
* @returns  L7_SUCCESS
*
* @comments None.
*       
* @end
*
*********************************************************************/
extern L7_RC_t mfdbMaxTableEntriesGet(L7_uint32 *maxEntries);

/*********************************************************************
*
* @purpose  To get the most number of entries ever in the MFDB table.
*
* @param    L7_uint32  *mostEntries  most number of entries ever
*
* @returns  L7_SUCCESS
*
* @comments None.
*       
* @end
*
*********************************************************************/
extern L7_RC_t mfdbMostEntriesGet(L7_uint32 *mostEntries);

/*********************************************************************
*
* @purpose  To get the current number of entries in the MFDB table.
*
* @param    L7_uint32  *currEntries  number of current entries
*
* @returns  L7_SUCCESS
*
* @comments None.
*       
* @end
*
*********************************************************************/
extern L7_RC_t mfdbCurrEntriesGet(L7_uint32 *currEntries);

/*********************************************************************
*
* @purpose  To find a VLANID-MAC entry in the MFDB table.
*
* @parm     mfdbData_t  *pData     pointer to the data structure
* @param    L7_uint32   matchType  L7_MATCH_EXACT or L7_MATCH_GETNEXT
*
* @returns  L7_SUCCESS, if the entry is found
* @returns  L7_NOT_EXIST, if the entry is not found
*
* @comments None.
*       
* @end
*
*********************************************************************/
extern L7_RC_t mfdbEntryFind(mfdbData_t *pData, L7_uint32 matchType);


/*********************************************************************
*
* @purpose  To register a routine to be called when an MFDB table
*           entry is created, deleted, or modified.
*
* @param    *notify  Notification routine with the following parms:
*                    @param  L7_uint32       vlanId   entry vlan id
*                    @param  L7_uchar8       *mac     mac address
*                    @param  L7_INTF_MASK_t  fwdMask  forward mask
*                    @param  L7_INTF_MASK_t  fltMask  filter mask
*                    @param  L7_uint32       event    the event:
*                                       MFDB_ENTRY_ADD_NOTIFY,
*                                       MFDB_ENTRY_DELETE_NOTIFY,
*                                       MFDB_ENTRY_PORT_ADD_NOTIFY, or
*                                       MFDB_ENTRY_PORT_DELETE_NOTIFY
*                    @param  L7_uint32         userId   component's id
*
* @param    componentID  user component's ID (see L7_COMPONENT_IDS_t)
*                                       
* @returns  L7_SUCCESS, if notification is successful
* @returns  L7_FAILURE, if invalid component ID
*
* @comments None.
*       
* @end
*
*********************************************************************/
extern L7_RC_t mfdbRegisterForEntryChange(L7_RC_t (*notify)(L7_uint32 vlanId, 
                                                            L7_uchar8 *mac, 
                                                            L7_INTF_MASK_t fwdMask, 
                                                            L7_INTF_MASK_t fltMask, 
                                                            L7_uint32 event, 
                                                            L7_uint32 userID), 
                                       L7_COMPONENT_IDS_t    componentID);

/*********************************************************************
*
* @purpose  To get all the forwarding ports of a VLANID-MAC entry
*           in the MFDB table.
*
* @param    L7_uchar8       *vidMac   pointer to entry vid, mac combo.
*                                     (2-byte vlanId) + (6-byte mac)
* @param    L7_INTF_MASK_t  *absMask  pointer to the resultant mask
*
* @returns  L7_SUCCESS
*
* @comments The absolute mask of a given VLANID-MAC address entry is
*           obtained by combining all its user-components' forwarding
*           interfaces and removing its static filtering interfaces.
*       
* @end
*
*********************************************************************/
extern L7_RC_t mfdbEntryAbsolutePortMaskGet(L7_uchar8 *vidMac, 
                                            L7_INTF_MASK_t *absMask);

/*********************************************************************
*
* @purpose  To get only the forwarding ports of a VLANID-MAC entry
*           in the MFDB table.
*
* @param    L7_uchar8       *vidMac   pointer to entry vid, mac combo.
*                                     (2-byte vlanId) + (6-byte mac)
* @param    L7_INTF_MASK_t  *absMask  pointer to the resultant mask
*
* @returns  L7_SUCCESS
*
* @comments The egress port mask of a given VLANID-MAC address entry is
*           obtained by combining all its user-components' forwarding
*           interfaces.
*       
* @end
*
*********************************************************************/
extern L7_RC_t mfdbEntryLearntPortMaskGet(L7_uchar8 *vidMac, 
                                           L7_INTF_MASK_t *absMask);

/*********************************************************************
*
* @purpose  To combine a given VLANID and MAC address to get an
*           8-byte VLANID-MAC address combination.
*
* @param    L7_uint32  vlanId   vlan id
* @param    L7_uchar8  *mac     pointer to mac address
* @param    L7_uchar8  *vidMac  pointer to vid-mac address combination
*                               (2-byte vlan id + 6-byte mac address)
*
* @returns  L7_SUCCESS
*
* @comments None.
*       
* @end
*
*********************************************************************/
extern L7_RC_t mfdbEntryVidMacCombine(L7_uint32 vlanId, L7_uchar8 *mac, L7_uchar8 *vidMac);

/*********************************************************************
*
* @purpose  To separate an 8-byte VLANID-MAC address combination into
*           a VLANID and MAC address.
*
* @param    L7_uchar8  *vidMac  pointer to vid-mac address combination
*                               (2-byte vlan id + 6-byte mac address)
* @param    L7_uint32  *vlanId  pointer to the vlan id
* @param    L7_uchar8  *mac     pointer to mac address
*
* @returns  L7_SUCCESS
*
* @comments None.
*       
* @end
*
*********************************************************************/
extern L7_RC_t mfdbEntryVidMacSeparate(L7_uchar8 *vidMac, L7_uint32 *vlanId, L7_uchar8 *mac);

/*********************************************************************
*
* @purpose  To check if a VLANID-MAC entry, that uses a specific
*           protocol, exists in the MFDB table.
*
* @param    L7_uchar8  *vidMac   pointer to vid-mac combo.
*                                (2-byte vlan id + 6-byte mac address)
* @param    L7_uint32  protocol  the protocol type
*                                (see L7_MFDB_PROTOCOL_TYPE_t)   
*
* @returns  L7_SUCCESS, if the entry using that protocol exists
* @returns  L7_FAILURE, if the entry does not exist, or
*                       the specified protocol is not in use.
*
* @comments None.
*
* @end
*
*********************************************************************/
extern L7_RC_t mfdbEntryProtocolGetExact(L7_uchar8 *vidMac, L7_uint32 protocol);

/*********************************************************************
*
* @purpose  Given a VLANID-MAC entry, that uses a specific protocol, 
*           get the protocol of the next VLANID-MAC entry in the
*           MFDB table.
*
* @param    L7_uchar8  *vidMac    pointer to vid-mac combo.
*                                 (2-byte vlan id + 6-byte mac address)
* @param    L7_uint32  *nextProt  pointer to protocol type
*                                 (see L7_MFDB_PROTOCOL_TYPE_t)   
*
* @returns  L7_SUCCESS, if the next protocol, in use, is found
* @returns  L7_FAILURE, if the entry identified by 'vidMac' is not
*                       found, or the next protocol is not found
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
extern L7_RC_t mfdbEntryProtocolGetNext(L7_uchar8 *vidMac, L7_uint32 *nextProt);

/*********************************************************************
*
* @purpose  To get the information of a given VLANID-MAC entry
*           that uses a specific protocol.
*
* @param    L7_uchar8  *vidMac   pointer to vid-mac combo.
*                                (2-byte vlan id + 6-byte mac address)
* @param    L7_uint32  protocol  the protocol type
*                                (see L7_MFDB_PROTOCOL_TYPE_t)   
* @param    mfdbUserInfo_t  *userInfo  pointer to the user component's
*                                      info structure
*
* @returns  L7_SUCCESS, if the entry using this protocol is found
* @returns  L7_FAILURE, if the entry using this protocol is not found
*
* @comments None.
*
* @end
*
*********************************************************************/
extern L7_RC_t mfdbEntryProtocolInfoGet(L7_uchar8 *vidMac, L7_uint32 protocol, 
                                        mfdbUserInfo_t *userInfo);


#endif /* MFDB_API_H */

