/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2009
*
**********************************************************************
* @filename  timerange_control.h
*
* @purpose   timerange Control header file
*
* @component timerange
*
* @comments  none
*
* @create    29/11/2009
*
* @author    Siva Mannem
*
* @end
*
**********************************************************************/
#ifndef TIMERANGE_CONTROL_H
#define TIMERANGE_CONTROL_H

/*********************************************************************
* @purpose  Go through registered time range users and notify them of 
*           time range events.
*
* @param    timeRangeName @b{(input)} time range name
* @param    event         @b{(input)} time range event
*
* @returns  
*
* @comments    
*       
* @end
*********************************************************************/
L7_RC_t timeRangeEventNotify(L7_uchar8* timeRangeName, timeRangeEvent_t  event);

/**************************************************************************
*
* @purpose    actions to be performed when the periodic timer expires
*
* @param    arg1    @b((input))  first argument
* @param    arg2    @b((input))  second argument
*
* @returns    L7_SUCCESS or L7_FAILURE
*
* @end
*
*************************************************************************/
L7_RC_t timeRangeTimerExpired(L7_uint32 arg1, L7_uint32 arg2);

/*********************************************************************
* @purpose  Start the time range timer.
*
* @param    void
*
* @returns 
*
* @notes    
*
* @end
*********************************************************************/
void timeRangeTimerStart(void);

/*********************************************************************
*
* @purpose  Stop the time range timer. 
*
* @returns  L7_SUCCESS
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t timeRangeTimerStop(void);

/*********************************************************************
*
* @purpose  Send a time Range update message.
*
* @param    timeRangeName    @b{(input)} time range name 
*                             (1 to L7_TIMERANGE_NAME_LEN_MAX chars)
* @param    action            @b((input)) TIMERANGE_MODIFY or
*                                         TIMERANGE_DELETE
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
*
* @end
*
*********************************************************************/
L7_RC_t timeRangeUpdate(L7_uchar8* timeRangeName, timeRangeActions_t action);

/*********************************************************************
*
* @purpose task to handle all Time Range messages
*
* @param void
*
* @returns void
*
* @notes none
*
* @end
*
*********************************************************************/
void timeRangeProcTask();

/*********************************************************************
* @purpose  Start Time Range Processing task
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t timeRangeProcTaskStart();
/* End Function Prototypes */

#endif /* TIMERANGE_CONTROL_H */
