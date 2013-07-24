/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2008
 *
 **********************************************************************
 *
 * @filename   usmdb_wdm_rrm_nbr_api.h
 *
 * @purpose    Wireless Data Manager Radio Resource Measurement (RRM)
 *             neighbor list USMDB API header
 *
 * @component  USMDB
 *
 * @comments   none
 *
 * @create     12/15/08
 *
 * @author     dcaugherty
 *
 * @end
 *             
 *********************************************************************/
#ifndef INCLUDE_USMDB_WDM_RRM_NBR_API_H
#define INCLUDE_USMDB_WDM_RRM_NBR_API_H

#include "wdm_rrm_nbr_api.h"

/*********************************************************************
* @purpose  To purge all entries in the RRM neighbor list tree
*
* @param    void
* 
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Will NOT send updated (empty) neighbor lists to
*           managed APs. TBFixed in a later version.
*
* @end
*********************************************************************/

L7_RC_t 
usmDbWdmRrmNbrListTreePurge(void);

/*********************************************************************
 * @purpose  To delete an AP (and all of its VAPs) from the RRM 
 *           neighbor list tree
 *
 * @param    L7_enetMacAddr_t   @b{(input)}  apMac
 * 
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *
 * @end
 *********************************************************************/

L7_RC_t 
usmDbWdmRrmNbrListApDelete(L7_enetMacAddr_t apMac);

/*********************************************************************
 * @purpose  To delete an AP/VAP pair from the RRM neighbor list tree
 *
 * @param    apVapPair_t *    @b{(input)}  pAvp
 * 
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *
 * @end
 *********************************************************************/

L7_RC_t 
usmDbWdmRrmNbrListApVapDelete(apVapPair_t * pAvp);

/*********************************************************************
 * @purpose  To confirm existence of an AP/VAP pair in 
 *           the RRM neighbor list tree
 *
 * @param    apVapPair_t *    @b{(input)}  pAvp
 * 
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *           
 * @end
 *********************************************************************/

L7_RC_t 
usmDbWdmRrmNbrListApVapGet(apVapPair_t * pAvp);

/*********************************************************************
 * @purpose  To retrieve the next ordered AP/VAP pair in the 
 *           RRM neighbor list tree
 *
 * @param    apVapPair_t *    @b{(input)}  pAvp
 * @param    apVapPair_t *    @b{(output)} pNextAvp
 * 
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *           
 * @end
 *********************************************************************/

L7_RC_t 
usmDbWdmRrmNbrListApVapGetNext(apVapPair_t * pAvp,
                               apVapPair_t * pNextAvp);

/*********************************************************************
 * @purpose  To retrieve the number of maintained nbrs of the AP/VAP pair
 *           in the RRM neighbor list tree
 *
 * @param    apVapPair_t *  @b{(input)}  pAvp
 * @param    L7_uint32   *  @b{(output)} pCount
 * 
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    
 *           
 * @end
 *********************************************************************/

L7_RC_t 
usmDbWdmRrmNbrListApVapNbrCountGet(apVapPair_t * pAvp, 
                                   L7_uint32 * pCount);

/*********************************************************************
 * @purpose  To confirm existence of a neighbor in the list used by
 *           an AP/VAP pair in the RRM neighbor list tree
 *
 * @param    apVapPair_t *    @b{(input)}  pAvp
 * @param    L7_enetMacAddr_t @b{(input)} nbrMac
 * 
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    Will fail if neighbor does not exist, if AP/VAP pair does
 *           not exist in tree, or if input pointer parms are null.
 *           
 * @end
 *********************************************************************/

L7_RC_t 
usmDbWdmRrmNbrListNbrGet(apVapPair_t * pAvp, 
                         L7_enetMacAddr_t nbrMac);

/*********************************************************************
 * @purpose  To find the next neighbor for given AP/VAP pair
 *           in the RRM neighbor list tree 
 *
 * @param    apVapPair_t *      @b{(input)}  pAvp
 * @param    L7_enetMacAddr_t   @b{(input)}  nbrMac
 * @param    L7_enetMacAddr_t * @b{(output)} pNbrMac
 * 
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    Will fail if AP/VAP pair does not exist in tree, or 
 *           if input pointer parms are null.
 *           
 * @end
 *********************************************************************/
L7_RC_t 
usmDbWdmRrmNbrListNbrGetNext(apVapPair_t * pAvp, 
                             L7_enetMacAddr_t nbrMac,
                             L7_enetMacAddr_t * pNbrMac);

/*********************************************************************
* @purpose  To find the next neighbor 
*           in the RRM neighbor list tree
*
* @param    apVapPair_t *      @b{(input)}  pAvp
* @param    L7_enetMacAddr_t   @b{(input)}  nbrMac
* @param    apVapPair_t *      @b{(output)} pNextAvp
* @param    L7_enetMacAddr_t * @b{(output)} pNbrMac
* 
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Will fail if AP/VAP pair does not exist in tree, or 
*           if input pointer parms are null.
*           
* @end
*********************************************************************/
L7_RC_t usmDbWdmRrmNbrGetNext(apVapPair_t * pAvp, 
                              L7_enetMacAddr_t nbrMac,
                              apVapPair_t * pNextAvp, 
                              L7_enetMacAddr_t * pNbrMac);

/*********************************************************************
 * @purpose  To delete a neighbor from the RRM neighbor list tree
 *
 * @param    apVapPair_t *      @b{(input)}  pAvp
 * @param    L7_enetMacAddr_t   @b{(input)}  nbrMac
 * 
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    Will fail if AP/VAP pair does not exist in tree, or 
 *           if input pointer parms are null.
 * @notes    If MAC of input neighbor is last in the list of the
 *           AP/VAP pair, the MAC of the first neighbor of the first 
 *           succeeding AP/VAP pair with a non-empty neighbor list
 *           will be found (along with the new AP/VAP pair).
 *           
 * @end
 *********************************************************************/
L7_RC_t 
usmDbWdmRrmNbrListNbrDelete(apVapPair_t * pAvp, 
                            L7_enetMacAddr_t nbrMac);

/*********************************************************************
 * @purpose  To retrieve the channel of a neighbor
 *
 * @param    apVapPair_t *    @b{(input)}  pAvp
 * @param    L7_enetMacAddr_t @b{(input)}  nbrMac
 * @param    L7_uchar8    *   @b{(output)} pChan
 * 
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    Will fail if any pointer parms are null, or if
 *           neighbor entry does not exist.
 *
 * @end
 *********************************************************************/

L7_RC_t 
usmDbWdmRrmNbrListNbrChannelGet(apVapPair_t * pAvp, 
                                L7_enetMacAddr_t nbrMac,
                                L7_uchar8 * pChan);

/*********************************************************************
 * @purpose  To retrieve the RSSI of a neighbor
 *
 * @param    apVapPair_t *    @b{(input)}  pAvp
 * @param    L7_enetMacAddr_t @b{(input)}  nbrMac
 * @param    L7_uchar8    *   @b{(output)} pRSSI
 * 
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    Will fail if any pointer parms are null, or if
 *           neighbor entry does not exist.
 *
 * @end
 *********************************************************************/

L7_RC_t 
usmDbWdmRrmNbrListNbrRSSIGet(apVapPair_t * pAvp,
                             L7_enetMacAddr_t nbrMac,
                             L7_uchar8   * pRSSI);


/*********************************************************************
 * @purpose  To retrieve the age of a neighbor entry, in seconds
 *
 * @param    apVapPair_t *    @b{(input)}  pAvp
 * @param    L7_enetMacAddr_t @b{(input)}  nbrMac
 * @param    L7_uint32   *    @b{(output)} pAge
 * 
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    Will fail if any pointer parms are null, or if
 *           neighbor entry does not exist.
 *
 * @end
 *********************************************************************/

L7_RC_t 
usmDbWdmRrmNbrListNbrAgeGet(apVapPair_t * pAvp, 
                            L7_enetMacAddr_t nbrMac,
                            L7_uint32   * pAge);


/*********************************************************************
 * @purpose  To retrieve the SSID of a neighbor
 *
 * @param    apVapPair_t *    @b{(input)}  pAvp
 * @param    L7_enetMacAddr_t @b{(input)}  nbrMac
 * @param    L7_uchar8    *   @b{(output)} pSSID
 * 
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @notes    Will fail if any pointer parms are null, or if
 *           neighbor entry does not exist.
 *
 * @end
 *********************************************************************/

L7_RC_t 
usmDbWdmRrmNbrListNbrSSIDGet(apVapPair_t * pAvp, 
                             L7_enetMacAddr_t nbrMac,
                             L7_uchar8   * pSSID);


#endif /* INCLUDE_USMDB_WDM_RRM_NBR_API_H */
