/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2008
*
**********************************************************************
*
* @filename     wdm_rrm_nbr_api.h
*
* @purpose      To provide interface prototypes for the RRM nbr list 
*
* @component    RF Scan
*
* @comments     none
*
* @create       12/10/2008
*
* @author       dcaugherty
*
* @end
*
**********************************************************************/

#ifndef WDM_RRM_NBR_API_H
#define WDM_RRM_NBR_API_H

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "datatypes.h"
#include "l7_packet.h"
#include "commdefs.h"
#include "wireless_commdefs.h"
#include "wireless_comm_structs.h"




/*********************************************************************
* @purpose  To create the one and only RRM neighbor list tree
*
* @param    void
* 
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Should only be called during Phase 1 of the configurator.
*           wdmRrmNbrListTreeDelete() must be invoked for a second
*           call to succeed.
*           
* @end
*********************************************************************/

L7_RC_t wdmRrmNbrListTreeCreate(void);


/*********************************************************************
* @purpose  To delete the one and only RRM neighbor list tree
*
* @param    void
* 
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Should only be called in the context of the configurator.
*           Call will fail if there is no tree to delete.
*           
* @end
*********************************************************************/

L7_RC_t wdmRrmNbrListTreeDelete(void);

/*********************************************************************
* @purpose  To purge all entries in the RRM neighbor list tree
*
* @param    void
* 
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/

L7_RC_t wdmRrmNbrListTreePurge(void);

/*********************************************************************
* @purpose  To check neighbor entries for timeouts, and trigger
*           neighbor list updates
*           
* @param    L7_uint32        @b{(input)}  timeoutInSeconds
* 
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*           
* @end
*********************************************************************/
L7_RC_t
wdmRrmNbrListTreeTimeoutCheck(L7_uint32  timeInSeconds);

/*********************************************************************
* @purpose  To check "dirty" neighbor lists, and trigger
*           neighbor list updates
*           
* @param    void
* 
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Will eliminate an AP/VAP pair from our tree if
*           dirty flag is not set and list is empty
*           
* @end
*********************************************************************/
L7_RC_t wdmRrmNbrListTreeUpdate(void);

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

L7_RC_t wdmRrmNbrListApDelete(L7_enetMacAddr_t apMac);

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

L7_RC_t wdmRrmNbrListApVapDelete(apVapPair_t * pAvp);

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

L7_RC_t wdmRrmNbrListApVapGet(apVapPair_t * pAvp);

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

L7_RC_t wdmRrmNbrListApVapGetNext(apVapPair_t * pAvp,
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
wdmRrmNbrListApVapNbrCountGet(apVapPair_t * pAvp, 
                              L7_uint32 * pCount);

/*********************************************************************
* @purpose  To add or update data for one neighbor in the
*           RRM neighbor list tree
*
* @param    apVapPair_t *         @b{(input)} pAvp
* @param    L7_enetMacAddr_t      @b{(input)} nbrMac
* @param    rrmNbrReportInput_t * @b{(input)} pRNRI
* @param    L7_uchar8             @b{(input)} rssi
* @param    rrmEnabCapab_t *      @b{(input)} pCapab
* @param    L7_uchar8   *         @b{(input)} pSSID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Will fail if neighbor list is full, if AP/VAP pair does
*           not exist in tree, or if input pointer parms are null.
*           
* @end
*********************************************************************/

L7_RC_t wdmRrmNbrListNbrAddUpdate(apVapPair_t * pAvp, 
                                  L7_enetMacAddr_t nbrMac,
                                  rrmNbrReportInput_t * pRNRI,
                                  L7_uchar8 rssi,
                                  rrmEnabCapab_t * pCapab,
                                  L7_uchar8 * pSSID);

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

L7_RC_t wdmRrmNbrListNbrGet(apVapPair_t * pAvp, 
                            L7_enetMacAddr_t nbrMac);

/*********************************************************************
* @purpose  To find the next neighbor 
*           in the RRM neighbor list tree for a given AP/VAP pair
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
L7_RC_t wdmRrmNbrListNbrGetNext(apVapPair_t * pAvp, 
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
L7_RC_t wdmRrmNbrGetNext(apVapPair_t * pAvp, 
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
L7_RC_t wdmRrmNbrListNbrDelete(apVapPair_t * pAvp, 
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

L7_RC_t wdmRrmNbrListNbrChannelGet(apVapPair_t * pAvp, 
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

L7_RC_t wdmRrmNbrListNbrRSSIGet(apVapPair_t * pAvp,
                                L7_enetMacAddr_t nbrMac,
                                L7_uchar8   * pRSSI);



/*********************************************************************
* @purpose  To retrieve the age of a neighbor entry
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

L7_RC_t wdmRrmNbrListNbrAgeGet(apVapPair_t * pAvp, 
                               L7_enetMacAddr_t nbrMac,
                               L7_uint32   * pAge);

/*********************************************************************
* @purpose  To retrieve the RRM capabilities of a neighbor entry
*
* @param    apVapPair_t *    @b{(input)}  pAvp
* @param    L7_enetMacAddr_t @b{(input)}  nbrMac
* @param    rrmEnabCapab_t * @b{(output)} pCapab
* 
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Will fail if any pointer parms are null, or if
*           neighbor entry does not exist.
*
* @end
*********************************************************************/

L7_RC_t wdmRrmNbrListNbrCapabGet(apVapPair_t * pAvp, 
                                 L7_enetMacAddr_t nbrMac,
                                 rrmEnabCapab_t * pCapab);


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

L7_RC_t wdmRrmNbrListNbrSSIDGet(apVapPair_t * pAvp, 
                                L7_enetMacAddr_t nbrMac,
                                L7_uchar8   * pSSID);



void wdmRrmNbrPrintMac(L7_enetMacAddr_t mac);
void wdmRrmNbrPrintApVapPair(apVapPair_t * pAvp, L7_uchar8 * delim);

#endif
