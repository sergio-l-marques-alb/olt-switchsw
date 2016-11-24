/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2009
 *
 **********************************************************************
 *
 * @filename   usmdb_wdm_rrm_ch_load_api.h
 *
 * @purpose    Wireless Data Manager Radio Resource Measurement (RRM)
 *             channel load request/report/history USMDB API header
 *
 * @component  USMDB
 *
 * @comments   none
 *
 * @create     1/26/09
 *
 * @author     dcaugherty
 *
 * @end
 *             
 *********************************************************************/
#ifndef INCLUDE_USMDB_WDM_RRM_CH_LOAD_API_H
#define INCLUDE_USMDB_WDM_RRM_CH_LOAD_API_H


#include "wdm_rrm_ch_load_api.h"



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
usmDbWdmRrmChLoadNewRequestClientGet(L7_enetMacAddr_t * client);


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
usmDbWdmRrmChLoadNewRequestChannelGet(L7_uchar8 * channel);


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
usmDbWdmRrmChLoadNewRequestDurationGet(L7_ushort16 *  duration);


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
usmDbWdmRrmChLoadCurrentRequestAPMacGet(L7_enetMacAddr_t * pAP);


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
usmDbWdmRrmChLoadCurrentRequestClientMacGet(L7_enetMacAddr_t * pClient);


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
usmDbWdmRrmChLoadCurrentRequestStatusGet(rrmChLoadRequestStatus_t * pStat);


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
usmDbWdmRrmChLoadCurrentRequestTimeRemainingGet(L7_uint32 * pTR);

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
usmDbWdmRrmChLoadCurrentRequestDurationGet(L7_ushort16 * pD);

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
usmDbWdmRrmChLoadNewRequestFailureReasonGet(void);

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
usmDbWdmRrmChLoadCurrentRequestChannelGet(L7_uchar8 * pCh);



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
L7_RC_t usmDbWdmRrmChLoadHistoryEntryGet(L7_uint32 stamp);


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
L7_RC_t 
usmDbWdmRrmChLoadHistoryEntryGetNext(L7_uint32 stamp, L7_uint32 * pStamp);


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
usmDbWdmRrmChLoadHistoryEntryAPMacGet(L7_uint32 stamp, L7_enetMacAddr_t * pAP);



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
usmDbWdmRrmChLoadHistoryEntryClientMacGet(L7_uint32 stamp, L7_enetMacAddr_t * pCli);


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
usmDbWdmRrmChLoadHistoryEntryDurationGet(L7_uint32 stamp, L7_ushort16 * pTUs);


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
usmDbWdmRrmChLoadHistoryEntryLoadGet(L7_uint32 stamp, L7_uchar8 * pPctg);


/*********************************************************************
* @purpose  To retrieve the channel mode value
*           in the record in the RRM channel load history 
*           tree with the given time stamp
*
* @param     L7_uint32    @b{(input)}  timeStamp
* @param     L7_uchar8 *  @b{(output)} pMode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*********************************************************************/

L7_RC_t 
usmDbWdmRrmChLoadHistoryEntryModeGet(L7_uint32 stamp, L7_uchar8 * pMode);


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
usmDbWdmRrmChLoadHistoryEntryChannelGet(L7_uint32 stamp, L7_uchar8 * pCh);


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
usmDbWdmRrmChLoadHistoryEntryAgeGet(L7_uint32 stamp, L7_uint32* pSecs);


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
L7_RC_t usmDbWdmRrmChLoadHistoryPurge(void);


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
usmDbWdmRrmChLoadNewRequestClientSet(L7_enetMacAddr_t client);

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
usmDbWdmRrmChLoadNewRequestChannelSet(L7_uchar8   channel);

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
usmDbWdmRrmChLoadNewRequestDurationSet(L7_ushort16   duration);


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
usmDbWdmRrmChLoadNewRequestClientGet(L7_enetMacAddr_t * client);


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
usmDbWdmRrmChLoadNewRequestChannelGet(L7_uchar8 * channel);


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
usmDbWdmRrmChLoadNewRequestDurationGet(L7_ushort16 *  duration);


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
usmDbWdmRrmChLoadRequestSubmit(void);


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
usmDbWdmRrmChLoadCurrentRequestAbort(void);

/*********************************************************************
* @purpose  To get the count of entries in our 1 and only tree
*
* @param    L7_uint32 * @b{(output)} pCount
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    The API fails if parm is null.
*
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmRrmChLoadHistoryCountGet(L7_uint32 * pCount);

/*********************************************************************
* @purpose  To get the maximum count of entries in our 1 and only tree
*
* @param    L7_uint32 * @b{(output)} pCount
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    The API fails if parm is null.
*
*
* @end
*********************************************************************/
L7_RC_t usmDbWdmRrmChLoadHistoryMaxCountGet (L7_uint32 * pCount);

#endif
