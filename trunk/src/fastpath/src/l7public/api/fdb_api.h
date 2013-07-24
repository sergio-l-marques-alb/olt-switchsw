
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename fdb_api.h
*
* @purpose fdb APIs
*
* @component fdb
*
* @comments none
*
* @create 08/14/2000
*
* @author bmutz
*
* @end
*             
**********************************************************************/

#ifndef FDB_API_H
#define FDB_API_H

#include "dot1q_api.h"
                    
#define FDB_DEL           0
#define FDB_ADD           1

typedef struct Dot1dTpFdbData_s
{
  L7_uchar8               dot1dTpFdbAddress[L7_FDB_KEY_SIZE];
  L7_uint32               dot1dTpFdbPort;
  L7_ushort16             dot1dTpFdbEntryType;
  struct Dot1dTpFdbData_s *next;
} dot1dTpFdbData_t;

typedef struct  fdbMeberInfo_s
{
  L7_uchar8 macAddr[L7_MAC_ADDR_LEN];
  L7_uint32 vlanId;
  L7_uint32 intIfNum;
  L7_uchar8 entryType;
} fdbMeberInfo_t;

#define FDB_MEMBER_VLAN_INDEPNDENT 0
#define FDB_CPU_VLAN_ID 1
#include "fdb_exports.h"

/*********************************************************************
* @purpose  FDB find mac in avl tree    
*
* @param    mac_addr    pointer to a mac address
* @param    matchType   L7_MATCH_EXACT or L7_MATCH_NEXT
* @parm     pData       pointer to a fdb data struct
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t fdbFind( char *mac, L7_uint32 matchType, dot1dTpFdbData_t *pData );


/*********************************************************************
* @purpose  FDB learn entry call back    
*
* @param    mac_addr    pointer to a mac address
* @param    L7_uint32   internal interface number
* @param    L7_uchar8   message type
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
void fdbLearnEntryCallBack(L7_uchar8 *macAddr, L7_uint32 intIfNum,
                           L7_uint32 vlanId, L7_uchar8 msgsType);

/*********************************************************************
* @purpose  Maximum number of entries in FDB     
*
* @param    void
*
* @returns  count    current nnumber of FDB enties
*
* @notes    none
*       
* @end
*********************************************************************/
L7_uint32 fdbMaxEntries(void);

/*********************************************************************
*
* @purpose  To get the maximum number of entries that the FDB table
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
L7_RC_t fdbMaxTableEntriesGet(L7_uint32 *maxEntries);

 /*********************************************************************
* @purpose  Reset maximum number of entries in FDB     
*
* @param    void
*
* @returns  void 
*
* @notes    none
*       
* @end
*********************************************************************/
void fdbMaxEntriesReset(void);

/*********************************************************************
* @purpose  Current number of entries in FDB     
*
* @param    void
*
* @returns  count    current nnumber of FDB enties
*
* @notes    none
*       
* @end
*********************************************************************/
L7_uint32 fdbActiveEntries(void);

/*********************************************************************
* @purpose  Returns the Unit's System Address Aging Time Out 
*
* @param    fid      Filtering Database Id  
*
* @returns  timeOut  System Address Aging Time Out
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_uint32 fdbGetAddressAgingTimeOut(L7_uint32 fid);

/*********************************************************************
* @purpose  Returns the Unit's System Address Aging Time Out 
*
* @param    fid      Filtering Database Id  
*
* @returns  timeOut  System Address Aging Time Out
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t fdbSetAddressAgingTimeOut(L7_uint32 timeOut, L7_uint32 fid);

/*********************************************************************
* @purpose  Add an entry into cpu and network processor FDB table     
*
* @param    fdbMemberInfo   pointer to a member information
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t fdbAddEntry(fdbMeberInfo_t *fdbMemberInfo);

/*********************************************************************
* @purpose  Del an entry into cpu and network processor FDB table     
*
* @param    fdbMemberInfo   pointer to a member information
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t fdbDelEntry(fdbMeberInfo_t *fdbMemberInfo);

/*********************************************************************
* @purpose  Set the FDB ID in the dot1q VLAN current table
*
* @param    vlanId      VLAN ID
* @param    *fdbId      FDB ID
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t fdbFdbIDGet(L7_uint32 vlanId, L7_uint32 *fdbId);

/*********************************************************************
* @purpose  Del the systems cpu mac address to table     
*
* @param    mac_addr    pointer to a mac address
* @param    vlanId      management vlan id
* @param    intIfNum    internal interface number 
* @parm     entryType   L7_FDB_ADDR_FLAG_t 
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t fdbSysMacDelEntry(L7_uchar8 *macAddr, L7_uint32 vlanId, L7_uint32 intIfNum, L7_uchar8 entryType);

/*********************************************************************
* @purpose  Add the systems cpu mac address to table     
*
* @param    mac_addr    pointer to a mac address
* @param    intIfNum    internal interface number 
* @parm     entryType   L7_FDB_ADDR_FLAG_t 
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t fdbSysMacAddEntry(L7_uchar8 *macAddr, L7_uint32 vlanId, L7_uint32 intIfNum, L7_uchar8 entryType);

/*********************************************************************
* @purpose  wrapper routine to retrive the internal interface number associated   
*
* @param    *mac        pointer to the mac address to search for
* @param    *intIfNum   pointer to the interface number to be returned
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    with a MAC address from the arp table maintained in DTL 
*       
* @end
*********************************************************************/
L7_RC_t fdbMacToIntfGet(L7_uchar8 *mac, L7_uint32 *intIfNum);

/*********************************************************************
* @purpose  return the type of supported VL
*
* @param    type       pointer to the type
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    with a MAC address from the arp table maintained in DTL 
*       
* @end
*********************************************************************/
L7_RC_t fdbGetTypeOfVL(L7_FDB_TYPE_t *type);

/*********************************************************************
* @purpose  Callback function to process VLAN changes.
*
* @param    vlanId      VLAN ID
* @param    intIfnum    internal interface whose state has changed
* @param    event       VLAN event (see vlanNotifyEvent_t for list)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t fdbVlanChangeCallback(dot1qNotifyData_t *vlanData, L7_uint32 intIfNum, L7_uint32 event);

/*********************************************************************
* @purpose  get the next fdbID starting from a certain index.
*
* @param    startfrom   index to start search from
* @param    fdbId       pointer to the return value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t fdbIdNextGet(L7_uint32 startfrom, L7_uint32 *fdbId);

/*********************************************************************
* @purpose  check to see if an FDB exists based on an FDB ID.
*
* @param    fdbId       FDB ID to be checked
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t fdbIdGet(L7_uint32 fdbId);

/*********************************************************************
* @purpose  Returns the number of failed FDB insert attempts due to
*           failure to allocate space.
*
* @param    *badAdds     pointer to the number of failed FDB inserts to
*                        be returned
*
* @returns  L7_SUCCESS
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t fdbBadAddsGet(L7_uint32 *badAdds);

/*********************************************************************
* @purpose  Returns the number of Dynamic entries
*
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 fdbDynamicAddsGet(void);


/*********************************************************************
* @purpose  Flush all learned entries from the L2FDB.
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  
*       
* @end
*********************************************************************/
L7_RC_t fdbFlush(void);

/*********************************************************************
* @purpose  Flushes all entries in fdb learnt on this interface 
*
* @param    intIfNum @b((input)) internal interface number  
*
* @returns  L7_SUCCESS on a successful operation 
* @returns  L7_FAILURE for any error 
*
* @comments
*
* @notes 
*
* @end
*********************************************************************/
L7_RC_t fdbFlushByPort(L7_uint32 intfNum);

/*********************************************************************
* @purpose  Flushes specific VLAN entries in fdb.
*
* @param    vlanId @b((input)) VLAN number.
*
* @returns  L7_SUCCESS on a successful operation 
* @returns  L7_FAILURE for any error 
*
* @comments
*
* @notes 
*
* @end
*********************************************************************/
L7_RC_t fdbFlushByVlan(L7_uint32 vlanId);

/*********************************************************************
* @purpose  Flushes All MAC specific entries in fdb.
*
* @param    mac @b((input)) MAC address
*
* @returns  L7_SUCCESS on a successful operation 
* @returns  L7_FAILURE for any error 
*
* @comments
*
* @notes 
*
* @end
*********************************************************************/
L7_RC_t fdbFlushByMac(L7_enetMacAddr_t mac);

/*********************************************************************
* @purpose  Get count of static entries in the L2FDB.
*
* @param    L7_uint32     *entries   @{{output}}  Number of static entries
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  
*       
* @end
*********************************************************************/
L7_RC_t fdbStaticEntriesGet(L7_uint32 *entries);

/*********************************************************************
* @purpose  Get count of static entries in the L2FDB for a specific interface
*
* @param    L7_uint32     intIfNum   @((input))   Internal interface number
* @param    L7_uint32     *entries   @{{output}}  Number of static entries
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  
*       
* @end
*********************************************************************/
L7_RC_t fdbIntfStaticEntriesGet(L7_uint32 intIfNum,
                                L7_uint32 *entries);

/*********************************************************************
* @purpose  Get count of static entries in the L2FDB for a specific VLAN
*
* @param    L7_ushort16    vlanId    @((input))   VLAN identifier
* @param    L7_uint32     *entries   @{{output}}  Number of static entries
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  
*       
* @end
*********************************************************************/
L7_RC_t fdbVlanStaticEntriesGet(L7_ushort16  vlanId,
                                L7_uint32   *entries);

/*********************************************************************
* @purpose  Get count of dynamic entries in the L2FDB.
*
* @param    L7_uint32     *entries   @{{output}}  Number of dynamic entries
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  
*       
* @end
*********************************************************************/
L7_RC_t fdbDynamicEntriesGet(L7_uint32 *entries);

/*********************************************************************
* @purpose  Get count of dynamic entries in the L2FDB for a specific interface
*
* @param    L7_uint32     intIfNum   @((input))   Internal interface number
* @param    L7_uint32     *entries   @{{output}}  Number of static entries
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  
*       
* @end
*********************************************************************/
L7_RC_t fdbIntfDynamicEntriesGet(L7_uint32 intIfNum,
                                 L7_uint32 *entries);

/*********************************************************************
* @purpose  Get count of dynamic entries in the L2FDB for a specific VLAN
*
* @param    L7_ushort16    vlanId    @((input))   VLAN identifier
* @param    L7_uint32     *entries   @{{output}}  Number of static entries
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  
*       
* @end
*********************************************************************/
L7_RC_t fdbVlanDynamicEntriesGet(L7_ushort16 vlanId,
                                 L7_uint32   *entries);

/*********************************************************************
* @purpose  Get count of internally added entries in the L2FDB.
*
* @param    L7_uint32     *entries   @{{output}}  Number of dynamic entries
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  
*       
* @end
*********************************************************************/
L7_RC_t fdbInternalEntriesGet(L7_uint32 *entries);

/*********************************************************************
* @purpose  Get count of internal entries in the L2FDB for a specific interface
*
* @param    L7_uint32     intIfNum   @((input))   Internal interface number
* @param    L7_uint32     *entries   @{{output}}  Number of static entries
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  
*       
* @end
*********************************************************************/
L7_RC_t fdbIntfInternalEntriesGet(L7_uint32 intIfNum,
                                  L7_uint32 *entries);

/*********************************************************************
* @purpose  Get count of internal entries in the L2FDB for a specific VLAN
*
* @param    L7_ushort16    vlanId    @((input))   VLAN identifier
* @param    L7_uint32     *entries   @{{output}}  Number of static entries
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  
*       
* @end
*********************************************************************/
L7_RC_t fdbVlanInternalEntriesGet(L7_ushort16  vlanId,
                                  L7_uint32   *entries);


#endif
