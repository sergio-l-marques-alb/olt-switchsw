#ifndef WDM_RRM_CH_LOAD_API_H
#define WDM_RRM_CH_LOAD_API_H

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "datatypes.h"
#include "l7_packet.h"
#include "commdefs.h"
#include "wireless_commdefs.h"
#include "wireless_comm_structs.h"


/********************************************************************
* 
*               CHANNEL LOAD REQUEST FUNCTIONALITY
*
*********************************************************************/

/*********************************************************************
* @purpose  To set the client MAC address to use for the next 
*           request for channel load measurement (to a wireless
*           that supports 802.11k and Radio Resource Measurement 
*           (RRM)).
*
* @param    L7_enetMacAddr_t @b{(input)} MAC addr of client
* 
* @returns  void
*
*           
* @end
*********************************************************************/
void
wdmRrmChLoadNewRequestClientSet(L7_enetMacAddr_t client);

/*********************************************************************
* @purpose  To set the channel to use for the next 
*           request for channel load measurement (to a wireless
*           that supports 802.11k and Radio Resource Measurement 
*           (RRM)).
*
* @param    L7_uchar8 @b{(input)} channel
* 
* @returns  void
*           
* @notes    Channel value of 0 implies measurement should be
*           taken on ALL channels supported. 
* @end
*********************************************************************/
void
wdmRrmChLoadNewRequestChannelSet(L7_uchar8   channel);

/*********************************************************************
* @purpose  To set the duration to use for the next 
*           request for channel load measurement (to a wireless
*           that supports 802.11k and Radio Resource Measurement 
*           (RRM)).
*
* @param    L7_ushort16      @b{(input)} duration of measurement 
*                                        expressed in TUs
* 
* @returns  void
*
*           
* @end
*********************************************************************/
void
wdmRrmChLoadNewRequestDurationSet(L7_ushort16   duration);


/*********************************************************************
* @purpose  To get the client MAC address to use for the next 
*           request for channel load measurement (to a wireless
*           that supports 802.11k and Radio Resource Measurement 
*           (RRM)).
*
* @param    L7_enetMacAddr_t * @b{(output)} MAC addr of client
* 
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*           
* @end
*********************************************************************/
L7_RC_t
wdmRrmChLoadNewRequestClientGet(L7_enetMacAddr_t * client);


/*********************************************************************
* @purpose  To get the channel to use for the next 
*           request for channel load measurement (to a wireless
*           that supports 802.11k and Radio Resource Measurement 
*           (RRM)).
*
* @param    L7_uchar8 * @b{(output)} channel
* 
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*           
* @notes    Channel value of 0 implies measurement will be
*           taken on ALL channels supported. 
* @end
*********************************************************************/
L7_RC_t
wdmRrmChLoadNewRequestChannelGet(L7_uchar8 * channel);


/*********************************************************************
* @purpose  To get the duration to use for the next 
*           request for channel load measurement (to a wireless
*           that supports 802.11k and Radio Resource Measurement 
*           (RRM)).
*
* @param    L7_ushort16   *  @b{(output)} duration of measurement 
*                                        expressed in TUs
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*           
* @end
*********************************************************************/

L7_RC_t
wdmRrmChLoadNewRequestDurationGet(L7_ushort16 *  duration);


/*********************************************************************
* @purpose  To get the reason for the last failure to submit
*           a request for channel load measurement (to a wireless
*           client that supports 802.11k and Radio Resource 
*           Measurement  (RRM)).
*
* @param    L7_ushort16   *  @b{(output)} duration of measurement 
*                                        expressed in TUs
* @returns  rrmChLoadRequestFailure_t
*           
* @end
*********************************************************************/

rrmChLoadRequestFailure_t
wdmRrmChLoadNewRequestFailureReasonGet(void);


/*********************************************************************
* @purpose  To submit a request for channel load measurement to 
*           a wireless client that supports 802.11k and Radio 
*           Resource Measurement (RRM).
*
* @param    void
* 
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Will fail if: client is not associated with a
*           managed AP, channel is not supported by client or AP, 
*           or if duration is outside acceptable range.
*
*           
* @end
*********************************************************************/
L7_RC_t
wdmRrmChLoadRequestSubmit(void);
 		          

/*********************************************************************
* @purpose  To abort the pending request for channel load measurement
*
* @param    none
* 
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Will fail if no request is pending.  Caller may choose
*           to ignore this.
*
*           
* @end
*********************************************************************/
L7_RC_t
wdmRrmChLoadCurrentRequestAbort(void);


/*********************************************************************
* @purpose  To get the status of the current channel load request
*
* @param    rrmChLoadRequestStatus_t * @b{(output)} status
* 
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Failure implies a NULL parm or something catastrophic.
*           
* @end
*********************************************************************/

L7_RC_t
wdmRrmChLoadCurrentRequestStatusGet(rrmChLoadRequestStatus_t * pStat);

/*********************************************************************
* @purpose  To set the status of the current request.
* @param    rrmChLoadRequestStatus_t @b{(input)} newSt
* @returns  void
* @notes    
* @end
*********************************************************************/
void rrmChLoadRequestStatusSet(rrmChLoadRequestStatus_t newSt);

/*********************************************************************
* @purpose  To get the AP MAC of the current channel load request
*
* @param    L7_enetMacAddr_t * @b{(output)} pAP
* 
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Failure implies a NULL parm or something catastrophic.
*           
* @end
*********************************************************************/
L7_RC_t
wdmRrmChLoadCurrentRequestAPMacGet(L7_enetMacAddr_t * pAP);

/*********************************************************************
* @purpose  To get the client MAC of the current channel load request
*
* @param    L7_enetMacAddr_t * @b{(output)} pCli
* 
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Failure implies a NULL parm or something catastrophic.
*           
* @end
*********************************************************************/
L7_RC_t
wdmRrmChLoadCurrentRequestClientMacGet(L7_enetMacAddr_t * pClient);

/*********************************************************************
* @purpose  To get the channel of the current channel load request
*
* @param    L7_uchar8 * @b{(output)} pCh
* 
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
r*
* @notes    Failure implies a NULL parm or something catastrophic.
*           
* @end
*********************************************************************/
L7_RC_t
wdmRrmChLoadCurrentRequestChannelGet(L7_uchar8 * pCh);

/*********************************************************************
* @purpose  To get the duration of the current channel load request
*
* @param    L7_ushort16 * @b{(output)} pDur
* 
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Failure implies a NULL parm or something catastrophic.
*           
* @end
*********************************************************************/
L7_RC_t
wdmRrmChLoadCurrentRequestDurationGet(L7_ushort16 * pD);

/*********************************************************************
* @purpose  To get the time remaining for the current channel load 
*           request
*
* @param    L7_uint32 * @b{(output)} time remaining
* 
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Failure implies a NULL parm or something catastrophic.
*           
* @end
*********************************************************************/
L7_RC_t
wdmRrmChLoadCurrentRequestTimeRemainingGet(L7_uint32 * pTR);



/*********************************************************************
* 
*               CHANNEL LOAD REPORT FUNCTIONALITY
*
*********************************************************************/

void
wirelessRrmChLoadReportRecvHandle(L7_uchar8 * buf,
				  L7_enetMacAddr_t fromMAC);

/*********************************************************************
* 
*               CHANNEL LOAD HISTORY FUNCTIONALITY
*
*********************************************************************/




/*********************************************************************
* @purpose  To create the one and only channel load history tree
*
* @param    none
* 
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Should only be called during Phase 1 of the configurator.
*           wdmRrmChLoadHistoryTreeDelete() must be invoked for a
*           second call to succeed.
*           
* @end
*********************************************************************/

L7_RC_t wdmRrmChLoadHistoryTreeCreate(void);

/*********************************************************************
* @purpose  To delete the one and only channel load history tree
*
* @param    none
* 
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Should only be called in the context of the configurator.
*           Call will fail if there is no tree to delete.
*           
* @end
*********************************************************************/

L7_RC_t wdmRrmChLoadHistTreeDelete(void);

/*********************************************************************
* @purpose  To get the count of entries in our 1 and only tree
*
* @param    L7_uint32 * @b{(output)} pCount
* 
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Will fail if parm is null.
*           
*           
* @end
*********************************************************************/

L7_RC_t wdmRrmChLoadHistoryCountGet(L7_uint32 * pCount);

/*********************************************************************
* @purpose  To get the maximum count of entries in our 1 and only tree
*
* @param    L7_uint32 * @b{(output)} pCount
* 
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Will fail if parm is null.
*           
*           
* @end
*********************************************************************/

L7_RC_t wdmRrmChLoadHistoryMaxCountGet(L7_uint32 * pCount);

/*********************************************************************
* @purpose  To purge all entries in the chan load history tree
*
* @param    void
* 
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/

L7_RC_t wdmRrmChLoadHistoryPurge(void);


/*********************************************************************
* @purpose  To add the data of a channel load report 
*           to the RRM channel load history tree
*
* @param     L7_enetMacAddr_t @b{(input)} AP MAC address
* @param     L7_enetMacAddr_t @b{(input)} client MAC address
* @param     L7_uint32        @b{(input)} duration in TUs 
*                                         (1024 msec units)
* @param     L7_uchar8        @b{(input)} channel used
* @param     L7_uchar8        @b{(input)} report mode
* @param     L7_uchar8        @b{(input)} load reported
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    If tree is full, will cause oldest record to be
*           deleted to make room.
*           
* @end
*********************************************************************/

L7_RC_t wdmRrmChLoadHistoryAdd(L7_enetMacAddr_t ap,
                            L7_enetMacAddr_t client,
                            L7_uint32 duration, /* in TUs */
                            L7_uchar8 channel,
                            L7_uchar8 mode,
                            L7_uchar8 load);

/*********************************************************************
* @purpose  To test for the presence of a record 
*           in the RRM channel load history tree with the
*           given time stamp
*
* @param     L7_uint32        @b{(input)} timeStamp
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t wdmRrmChLoadHistoryEntryGet(L7_uint32 stamp);


/*********************************************************************
* @purpose  To find the next record in the RRM channel load history 
*           tree with a greater time stamp
*
* @param     L7_uint32    @b{(input)}  timeStamp
* @param     L7_uint32 *  @b{(output)} pTimeStamp
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t wdmRrmChLoadHistoryEntryGetNext(L7_uint32 stamp, L7_uint32 * pStamp);

/*********************************************************************
* @purpose  To retrieve the AP MAC
*           in the record in the RRM channel load history 
*           tree with the given time stamp
*
* @param     L7_uint32          @b{(input)}  timeStamp
* @param     L7_enetMacAddr_t * @b{(output)} pAP
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/

L7_RC_t 
wdmRrmChLoadHistoryEntryAPMacGet(L7_uint32 stamp, L7_enetMacAddr_t * pAP);


/*********************************************************************
* @purpose  To retrieve the client MAC
*           in the record in the RRM channel load history 
*           tree with the given time stamp
*
* @param     L7_uint32          @b{(input)}  timeStamp
* @param     L7_enetMacAddr_t * @b{(output)} pCli
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/

L7_RC_t 
wdmRrmChLoadHistoryEntryClientMacGet(L7_uint32 stamp,
                                  L7_enetMacAddr_t * pCli);

/*********************************************************************
* @purpose  To retrieve the measurement duration (in TUs)
*           in the record in the RRM channel load history 
*           tree with the given time stamp
*
* @param     L7_uint32      @b{(input)}  timeStamp
* @param     L7_ushort16 *  @b{(output)} pTUs
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/

L7_RC_t 
wdmRrmChLoadHistoryEntryDurationGet(L7_uint32 stamp,
                                 L7_ushort16 * pTUs);


/*********************************************************************
* @purpose  To retrieve the channel load value (AS A PERCENT)
*           in the record in the RRM channel load history 
*           tree with the given time stamp
*
* @param     L7_uint32    @b{(input)}  timeStamp
* @param     L7_uchar8 *  @b{(output)} pPctg
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/

L7_RC_t 
wdmRrmChLoadHistoryEntryLoadGet(L7_uint32 stamp,
                             L7_uchar8 * pPctg);


/*********************************************************************
* @purpose  To retrieve the channel
*           of the record in the RRM channel load history 
*           tree with the given time stamp
*
* @param     L7_uint32    @b{(input)}  timeStamp
* @param     L7_uchar8 *  @b{(output)} pCh
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/

L7_RC_t 
wdmRrmChLoadHistoryEntryChannelGet(L7_uint32 stamp, 
                                L7_uchar8 * pCh);


/*********************************************************************
* @purpose  To retrieve the age
*           of the record in the RRM channel load history 
*           tree with the given time stamp
*
* @param     L7_uint32    @b{(input)}  timeStamp
* @param     L7_uint32 *  @b{(output)} age in seconds
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/

L7_RC_t 
wdmRrmChLoadHistoryEntryAgeGet(L7_uint32 stamp, L7_uint32* pSecs);


#endif
