/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename src\l7public\api\filter_api.h
*
* @purpose Externs for Static MAC filtering
*
* @component Filter
*
* @comments none
*
* @create 1/7/2002
*
* @author skalyanam
* @end
*
**********************************************************************/

/*********************************************************************
 *                   
 ********************************************************************/

#ifndef FILTER_API_H
#define FILTER_API_H
#include "default_cnfgr.h"
#include "l7_packet.h"
#include "comm_mask.h"
#include "filter_exports.h"

// PTin added
extern L7_FILTER_VLAN_FILTER_MODE_t ptin_filter_vlan_mode;

#define FILTER_MAX_INTF_NUM   FD_CNFGR_NIM_MAX_LAG_INTF_NUM

/* Interface storage */
typedef struct
{
  L7_uchar8 value[FILTER_INTF_INDICES];
} FILTER_INTF_MASK_t;


/*********************************************************************
* @purpose  Get multicast address filtering configuration.
*
* @param    L7_BOOL        enable    @{{input}}  enable/disable filtering
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  
*       
* @end
*********************************************************************/
L7_RC_t filterAdminModeSet(L7_BOOL enable);

/*********************************************************************
* @purpose  Get multicast address filtering configuration.
*
* @param    L7_BOOL        *enable    @{{output}}  enable/disable filtering
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  
*       
* @end
*********************************************************************/
L7_RC_t filterAdminModeGet(L7_BOOL *enable);

/*********************************************************************
*
* @purpose  Create a MAC Static Filter
*
* @param    L7_uchar8*  macAddr   @b((input)) Static MAC Address for the filter
* @param    L7_uint32   vlanId    @b{(input)} Vlan ID for the Filter  
*                            
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t filterCreate(L7_uchar8* macAddr,
                     L7_uint32 vlanId);


/*********************************************************************
*
* @purpose  Adds a source interface to the static MAC Filter 
*
* @param    L7_uchar8*  macAddr   @b((input)) MAC Address of the Filter
* @param    L7_uint32   vlanId    @b{(input)} Vlan ID for the Filter
* @param    L7_uint32   srcIntf   @b{(input)} Internal Interface number of the source port  
*                            
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes    adds src interface at the end of densely packed list
*
* @end
*********************************************************************/
L7_RC_t filterSrcIntfAdd(L7_uchar8* macAddr,
                         L7_uint32 vlanId, 
                         L7_uint32 srcIntf);

/*********************************************************************
*
* @purpose  Deletes a source port from the source port list of the MAC Filter 
*
* @param    L7_uchar8*  macAddr   @b((input)) MAC Address of the filter
* @param    L7_uint32   vlanId    @b{(input)} Vlan ID for the Filter
* @param    L7_uint32   srcIntf   @b{(input)} Internal Interface number of the source port  
*                            
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes    deletes src from list and reorders list densely
*
* @end
*********************************************************************/
L7_RC_t filterSrcIntfDelete(L7_uchar8* macAddr,
                            L7_uint32 vlanId,
                            L7_uint32 srcIntf);

/*********************************************************************
*
* @purpose  Adds a destination interface to the static MAC Filter 
*
* @param    L7_uchar8* macAddr   @b((input)) MAC Address of the Filter
* @param    L7_uint32  vlanId    @b{(input)} Vlan ID for the Filter
* @param    L7_uint32  dstIntf   @b{(input)} Internal Interface number of the destination port  
*                            
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t filterDstIntfAdd(L7_uchar8* macAddr,
                         L7_uint32 vlanId, 
                         L7_uint32 dstIntf);

/*********************************************************************
*
* @purpose  Deletes a destination port from the destination port list of the MAC Filter 
*
* @param    L7_uchar8*  macAddr   @b((input)) MAC Address of the filter
* @param    L7_uint32   vlanId    @b{(input)} Vlan ID for the Filter
* @param    L7_uint32   srcIntf   @b{(input)} Internal Interface number of the destination port  
*                            
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t filterDstIntfDelete(L7_uchar8* macAddr,
                            L7_uint32 vlanId,
                            L7_uint32 dstIntf);

/*********************************************************************
*
* @purpose  Adds destination interfaces to the static MAC Filter 
*
* @param    L7_ushort16             vlanId     @{{input}}  VLAN ID
* @param    L7_enetMacAddr_t        macAddr    @{{input}}  MAC Address
* @param    L7_INTF_MASK_t          forward    @{{input}}  ports to forward
* @param    L7_INTF_MASK_t          filter     @{{input}}  ports to forbid
*
* @returns  L7_SUCCESS, entry added or exact entry exists
* @returns  L7_FAILURE
*
* @notes  If the entry already exists, this will add the indicated ports.
*       
* @end
*
* @end
*********************************************************************/
L7_RC_t filterDstIntfMaskAdd( L7_uint32 vlanId, 
                              L7_enetMacAddr_t macAddr,
                              L7_INTF_MASK_t fwdMask,
                              L7_INTF_MASK_t filterMask);

/*********************************************************************
*
* @purpose  Removes destination interfaces from the static MAC Filter 
*
* @param    L7_ushort16             vlanId     @{{input}}  VLAN ID
* @param    L7_enetMacAddr_t        macAddr    @{{input}}  MAC Address
* @param    L7_INTF_MASK_t          forward    @{{input}}  ports to forward
* @param    L7_INTF_MASK_t          filter     @{{input}}  ports to forbid
*
* @returns  L7_SUCCESS, entry added or exact entry exists
* @returns  L7_FAILURE
*
* @notes  This function is only for destination /multicast entries and 
*         hence no processing for unicast addresses.
* @end
*
* @end
*********************************************************************/
L7_RC_t filterDstIntfMaskDelete( L7_uint32 vlanId, 
                              L7_enetMacAddr_t macAddr,
                              L7_INTF_MASK_t fwdMask,
                              L7_INTF_MASK_t filterMask);

/*********************************************************************
*
* @purpose  Removes a configured MAC static Filter 
*
* @param    L7_uchar8*  macAddr   @b((input)) MAC Address of the filter
* @param    L7_uint32   vlanId    @b{(input)} Vlan ID for the Filter
*                            
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 filterRemove(L7_uchar8* macAddr, L7_uint32 vlanId);

/*********************************************************************
*
* @purpose  Gets the first configured MAC static Filter 
*
* @param    L7_uchar8*  macAddr   @b((input)) MAC Address of the filter
* @param    L7_uint32*  vlanId    @b{(input)} Vlan ID for the Filter
*                            
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if there is no MAC Filter
*
* @notes    macAddr is returned as L7_NULL if there are no configured MAC Filters
*
* @end
*********************************************************************/
L7_RC_t filterFirstGet(L7_uchar8* macAddr,
                       L7_uint32* vlanId);

/*********************************************************************
*
* @purpose  Gets the next configured MAC static Filter 
*
* @param    L7_uchar8*  macAddr     @b((input)) MAC Address of the filter
* @param    L7_uint32   vlanId      @b{(input)} Vlan ID for the filter
* @param    L7_uchar8*  nextAddr    @b((output) MAC Address of the next filter
* @param    L7_uint32*  nextVlanId  @b{(input)} Vlan ID for the next filter
*                            
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if there are no further MAC Filters
*
* @notes    nextAddr is returned as L7_NULL if there is no further MAC filters
*
* @end
*********************************************************************/
L7_RC_t filterNextGet(L7_uchar8* macAddr,
                      L7_uint32 vlanId,
                      L7_uchar8* nextAddr,
                      L7_uint32* nextVlanId);

/*********************************************************************
*
* @purpose  Gets list source interfaces for a MAC static Filter 
*
* @param    L7_uchar8*  macAddr      @b((input))  MAC Address of the filter
* @param    L7_uint32   vlanId       @b{(input)}  Vlan ID for the Filter
* @param    L7_uint32*  numSrcIntf   @b((output)) Number of source interfaces
* @param    L7_uint32*  srcIntfList  @b((output)) Densely packed array of source interfaces                           
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if there is no MAC Filter
*
* @notes    numSrcIntf will be returned as zero if there is no src port for the MAC Filter
*           Only the first numSrcIntf entries in the srcIntfList have valid 
*           interfaces, rest are garbage
*
* @end
*********************************************************************/
L7_RC_t filterSrcIntfListGet(L7_uchar8* macaddr,
                             L7_uint32 vlanId,
                             L7_uint32* numSrcIntf,
                             L7_uint32 srcIntfList[]);
/*********************************************************************
*
* @purpose  Gets list destination interfaces for a MAC static Filter 
*
* @param    L7_uchar8*  macAddr      @b((input))  MAC Address of the filter
* @param    L7_uint32   vlanId       @b{(input)}  Vlan ID for the Filter
* @param    L7_uint32*  numDstIntf   @b((output)) Number of destination interfaces
* @param    L7_uint32*  dstIntfList  @b((output)) Densely packed array of destination interfaces                           
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if there is no MAC Filter
*
* @notes    numDstIntf will be returned as zero if there is no dst port for the MAC Filter
*           Only the first numDstIntf entries in the dstIntfList have valid 
*           interfaces, rest are garbage
*
* @end
*********************************************************************/
L7_RC_t filterDstIntfListGet(L7_uchar8* macaddr,
                             L7_uint32 vlanId,
                             L7_uint32* numDstIntf,
                             L7_uint32 dstIntfList[]);
/*********************************************************************
*
* @purpose  Gets list destination filter interfaces  for a MAC static Filter 
*
* @param    L7_uchar8*  macAddr      @b((input))  MAC Address of the filter
* @param    L7_uint32   vlanId       @b{(input)}  Vlan ID for the Filter
* @param    L7_uint32*  numDstIntf   @b((output)) Number of destination interfaces
* @param    L7_uint32*  dstIntfList  @b((output)) Densely packed array of destination interfaces                           
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if there is no MAC Filter
*
* @notes    numDstIntf will be returned as zero if there is no dst port for the MAC Filter
*           Only the first numDstIntf entries in the dstIntfList have valid 
*           interfaces, rest are garbage
*
* @end
*********************************************************************/
L7_RC_t filterDstFiltIntfListGet(L7_uchar8* macaddr,
                             L7_uint32 vlanId,
                             L7_uint32* numFiltIntf,
                             L7_uint32 dstFiltIntfList[]);
/*********************************************************************
*
* @purpose  Gets the number of MAC static Filters configured 
*
* @param    L7_uint32* count      @b((output))  NumberOf Filters configured
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if there is MAC Filter
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t filterCountGet(L7_uint32* count);
/*********************************************************************
*
* @purpose  Is a particular MAC Filter configured 
*
* @param    L7_uchar8*  macAddr  @b((input)) MAC Address of the filter
* @param    L7_uint32   vlanId   @b{(input)} Vlan ID for the Filter
*
* @returns  L7_TRUE, if a MAC filter exists
* @returns  L7_FALSE, if there is no MAC Filter
*
* @notes    
*
* @end
*********************************************************************/
L7_BOOL filterIsConfigured(L7_uchar8* macAddr, L7_uint32 vlanId);
/*********************************************************************
* @purpose  Convert filter bitmasks to SNMP octet-strings 
*
* @param    in          mask to be converted  
*           out         pointer to storage to hold the converted mask
*
* @returns  L7_SUCCESS
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t filterReverseMask(FILTER_INTF_MASK_t in, 
                          FILTER_INTF_MASK_t* out);

/*********************************************************************
* @purpose  Convert a mask to a densely packed list of internal interface numbers 
*
* @param    FILTER_INTF_MASK_t*  mask     @b((input))  mask to be converted  
* @param    L7_uint32            list[]   @b{(output)} densely packed list 
* @param    L7_uint32*           numList  @b{(output)} number of interfaces in the above list
*
* @returns  L7_SUCCESS
*
* @notes    mask is filter oriented i.e. lsb/rightmost bit represents interafce 1
*       
* @end
*********************************************************************/
L7_RC_t filterMaskToList(FILTER_INTF_MASK_t *mask, 
                         L7_uint32 list[], 
                         L7_uint32 *numList);

/*********************************************************************
* @purpose  Convert a densely packed list of internal interface numbers to a mask
*
* @param    L7_uint32            list[]   @b{(input)} densely packed list 
* @param    L7_uint32            numList  @b{(input)} number of interfaces in the above list
* @param    FILTER_INTF_MASK_t*  mask     @b((output))  mask to be generated  
*
* @returns  L7_SUCCESS
*
* @notes    mask is filter oriented i.e. lsb/rightmost bit represents interafce 1
*       
* @end
*********************************************************************/
L7_RC_t filterListToMask(L7_uint32 list[], 
                         L7_uint32 numList, 
                         FILTER_INTF_MASK_t *mask);

/*********************************************************************
*
* @purpose To get convert a Multicast MAC Addr into ip format
*
* @param macBuf  Array of 6 bytes containing the mac address
*
*
* @returns L7_char8
*
* @end
*
*********************************************************************/
L7_uchar8 *filterMulticastMacToIpFormat(L7_uchar8 *macBuf);

/*********************************************************************
*
* @purpose  Checks if a particular interface is present in any MAC filter 
*
* @param    L7_uint32  intIfNum  @b((input)) Internal Interface Number
*
* @returns  L7_TRUE, if the interface is in any filter
* @returns  L7_FALSE, if the interface is not associated with any MAC filter
*
* @notes    An interface is associated with a MAC filter if it is in the filter's
*           source list or destination list. If an interface is found on any list
*           of any configured MAC filter then this routine will return a L7_TRUE
*           Only if the interface is NOT found on ANY list of ANY filter, a 
*           L7_FALSE is returned
*           
*
* @end
*********************************************************************/
L7_BOOL filterIsIntfInAnyFilter(L7_uint32 intIfNum);

/*********************************************************************
*
* @purpose  To check the mac address type, unicast or multicast.
*
* @param    L7_uchar8  *macAddr   @b((input)) MAC Address
*
* @returns  L7_TRUE, if MAC address is of type multicast
* @returns  L7_FALSE, if MAC address is of type unicast
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL filterCheckMacAddrTypeMulticast(L7_uchar8 *macAddr);

/*********************************************************************
* @purpose  Check if the interface is valid for filtering
*
* @param 	intIfNum			Internal interface number
*
* @returns L7_TRUE if valid
* @returns L7_FALSE otherwise
*
* @end
*
*********************************************************************/
extern L7_BOOL filterIsValidIntf(L7_uint32 intIfNum);

/*********************************************************************
* @purpose  Set multicast address filtering configuration.
*
* @param    L7_uint32           vlanId  @{{intput}} VLAN
* @param    L7_MFDB_VLAN_FILTER_MODE_t mode    @{{intput}}  MFDB_FORWARD_ALL,
*                                                    MFDB_FORWARD_UNREGISTERED,
*                                                    MFDB_FILTER_UNREGISTERED.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, vlan is not configurable
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t filterVlanFilteringModeSet(L7_uint32 vlanId, L7_FILTER_VLAN_FILTER_MODE_t mode);

/*********************************************************************
* @purpose  Get multicast address filtering configuration.
*
* @param    L7_uint32           vlanId  @{{intput}} VLAN
* @param    L7_MFDB_VLAN_FILTER_MODE_t *mode   @{{output}}  MFDB_FORWARD_ALL,
*                                                    MFDB_FORWARD_UNREGISTERED,
*                                                    MFDB_FILTER_UNREGISTERED.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t filterVlanFilteringModeGet(L7_uint32 vlanId, L7_FILTER_VLAN_FILTER_MODE_t *mode);
/********************************************************************/
/******************FILTER MASK MACROS********************************/
/********************************************************************/

/*-----------------------*/
/* Interface Mask Macros */
/*-----------------------*/

/* Least significant bit/rightmost bit is lowest interface # */
/* this is opposite of what SNMP wants */
/* Application will reverse it when talking with SNMP*/

/* SETMASKBIT turns on bit index # k in mask j. */
#define FILTER_INTF_SETMASKBIT(j, k)                                    \
            ((j).value[((k-1)/(8*sizeof(L7_uchar8)))] \
                         |= 1 << ((k-1) % (8*sizeof(L7_uchar8))))   



/* CLRMASKBIT turns off bit index # k in mask j. */
#define FILTER_INTF_CLRMASKBIT(j, k)                                    \
           ((j).value[((k-1)/(8*sizeof(L7_uchar8)))]   \
                        &= ~(1 << ((k-1) % (8*sizeof(L7_uchar8)))))      



/* MASKEQ sets mask j equal to mask k. */
#define FILTER_INTF_MASKEQ(j, k) { \
        L7_uint32 x; \
 \
        for (x = 0; x < FILTER_INTF_INDICES; x++) { \
                (j).value[x] = (k).value[x]; \
        } \
}


/* MASKANDEQ turns-on the bits in mask j that are on in both mask j and k. */
#define FILTER_INTF_MASKANDEQ(j, k) { \
        L7_uint32 x; \
 \
        for (x = 0; x < FILTER_INTF_INDICES; x++) { \
                j.value[x] &= k.value[x]; \
        } \
}

/* MASKEXOREQ turns-on the bits in mask j that are on in either mask j and k but not in both. */
#define FILTER_INTF_MASKEXOREQ(j, k) { \
        L7_uint32 x; \
 \
        for (x = 0; x < FILTER_INTF_INDICES; x++) { \
                j.value[x] ^= k.value[x]; \
        } \
}


/* ISMASKBITSET returns 0 if the interface k is not set in mask j */
#define FILTER_INTF_ISMASKBITSET(j, k)                               \
        ((j).value[((k-1)/(8*sizeof(L7_uchar8)))]  \
                         & ( 1 << ((k-1) % (8*sizeof(L7_char8)))) )  





#endif /*FILTER_API_H*/

