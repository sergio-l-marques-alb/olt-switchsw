/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename  usmdb_switch_cpu_api.h
*
* @purpose   
* @component 
*
* @comments
*
* @create    
*
* @author    
*
* @end
*
**********************************************************************/
#ifndef USMDB_SWITCH_CPU_API_H
#define USMDB_SWITCH_CPU_API_H

#include "snmp_util_api.h"
#include "osapi.h"

 
/*********************************************************************
*
* @structures  usmDbProcessEntry_t
*
* @purpose
*
* @notes       none
*
*********************************************************************/
typedef struct
{
  L7_char8 taskName[36];
  L7_char8 taskPercentString[80];
} usmDbProcessEntry_t;

/*********************************************************************
*
* @purpose Get the process entry from the table 
*
* @param L7_uint32 UnitIndex  @b((input))  The unit for this operation
* @param L7_uint32 *val       @b((output)) Returned value
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbSwitchCpuProcessEntryGet(L7_uint32 UnitIndex, L7_uint32 processIndex, usmDbProcessEntry_t  *dumpEntry);

/*********************************************************************
*
* @purpose Gets the Next Task Id
*
* @param L7_uint32 UnitIndex  @b((input))  The unit for this operation
* @param L7_uint32 processIndex @b((input))Process Index for this operation
* @param L7_uint32 *taskIdString @b((output)) Returned value
* @param L7_uint32 taskidSize @b((input))String Size
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbTaskIdGet(L7_uint32 UnitIndex,L7_uint32 processIndex,
                       L7_char8 *taskIdString, L7_uint32 taskIdSize);

/*********************************************************************
*
* @purpose Get the Total CPU Utilization.
*
* @param L7_uint32 UnitIndex  @b((input))  The unit for this operation
* @param L7_char8 *taskPercentString @b((input))Percentage CPU Task Utilization
* @param L7_uint32 taskPercentSize @b((output)) String size
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbTotalCpuUtilGet(L7_uint32 UnitIndex,L7_char8 *taskPercentString, L7_uint32 taskPercentSize);

/*********************************************************************
*
* @purpose Gets a total memory Free (measured in KB)
*
* @param L7_uint32 UnitIndex  @b((input))  The unit for this operation
* @param L7_uint32 *val @b((output)) Returned value
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbInfoSwitchCpuProcessMemFree(L7_uint32 UnitIndex,L7_uint32 *val);


/*********************************************************************
*
* @purpose Gets a total memory Available (measured in KB)
*
* @param L7_uint32 UnitIndex  @b((input))  The unit for this operation
* @param L7_uint32 *val @b((output)) Returned value
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
extern L7_RC_t usmDbInfoSwitchCpuProcessMemAvailable(L7_uint32 UnitIndex,L7_uint32 *val);

/*********************************************************************
*
* @purpose Gets a Allocated memory (measured in bytes)
*
* @param L7_uint32 UnitIndex  @b((input))  The unit for this operation
* @param L7_uint32 *val @b((output)) Returned value
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbInfoSwitchCpuProcessMemAllocated(L7_uint32 UnitIndex,L7_uint32 *val);

#endif

