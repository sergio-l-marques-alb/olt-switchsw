/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename   alarm_controller.c
*
* @purpose    Binds for alarm manager functionality
*
* @component  alarm manager
*
* @create     13/07/2006
*
* @author     siva
* @end
*
******************************************************************************/
#include "commdefs.h"
#include "osapi.h"
#include  "datatypes.h"
#include "chassis_alarmmgr_api.h"

/*********************************************************************
* @purpose  This function provides a bind call for alarmMgrAlarmRaise
* for non-chassis platforms
*
* @param    alarmId - @b{(input)}  Alarm ID.
* @param    alarmTimeStamp - @b{(input)}  Time stamp of alarm occurance.
* @param    addInfo - @b{(input)}  Additional information about alarm. This
*                                  can be passed as L7_NULL, if there is no
*                                  additional information.
*
*
* @returns  L7_SUCCESS
* @end
*********************************************************************/
L7_RC_t alarmMgrAlarmRaise(L7_ALARM_ID_t alarmId, L7_clocktime alarmTimeStamp,
        L7_uchar8* addInfo)
{
    return L7_SUCCESS;
}
/*********************************************************************
* @purpose  This function provides a bind call for alarmMgrAlarmClear
* for non-chassis platforms
*
* @param    alarmId - @b{(input)}  Alarm ID.
* @param    alarmTimeStamp - @b{(input)}  Time stamp of alarm clearance.
* @param    addInfo - @b{(input)}  Additional information about alarm. This
*                                  can be passed as L7_NULL, if there is no
*                                  additional information.
*
*
* @returns  L7_SUCCESS 
* @end
*********************************************************************/
L7_RC_t alarmMgrAlarmClear(L7_ALARM_ID_t alarmId, L7_clocktime alarmTimeStamp,
        L7_uchar8* addInfo)
{
   return L7_SUCCESS;
}
