/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename  usmdb_switch_cpu_api.c
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

#include "l7_common.h"

#include "usmdb_switch_cpu_api.h"
#include "commdefs.h"
#include "sysapi.h"
#include "osapi.h"


/*********************************************************************
*
* @purpose Gets an entry for process from the cache table
*
* @param L7_uint32 UnitIndex  @b((input))  The unit for this operation
* @param L7_uint32 processIndex @b((input))  The unit for this operation
* @param L7_uint32 *dumpEntry @b((output)) Returned value
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbSwitchCpuProcessEntryGet(L7_uint32 UnitIndex, L7_uint32 processIndex, usmDbProcessEntry_t  *dumpEntry)
{
  return osapiSwitchCpuUtilizationGet( processIndex,
                                       dumpEntry->taskName, sizeof(dumpEntry->taskName), 
                                       dumpEntry->taskPercentString, sizeof(dumpEntry->taskPercentString));
}

/*********************************************************************
*
* @purpose Gets the Next Task Id
*
* @param L7_uint32 UnitIndex  @b((input))  The unit for this operation
* @param L7_uint32 processIndex @b((input))Process Index for this operation
* @param L7_char8 *taskIdString @b((output)) Returned value
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
L7_RC_t usmDbTaskIdGet(L7_uint32 UnitIndex,L7_uint32 processIndex, 
                       L7_char8 *taskIdString, L7_uint32 taskIdSize)
{
  return osapiTaskIdGet( processIndex, taskIdString, taskIdSize);
}

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
L7_RC_t usmDbTotalCpuUtilGet(L7_uint32 UnitIndex,L7_char8 *taskPercentString, L7_uint32 taskPercentSize)
{
  return osapiTotalCpuUtilGet( taskPercentString, taskPercentSize);
}

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
L7_RC_t usmDbInfoSwitchCpuProcessMemFree(L7_uint32 UnitIndex,L7_uint32 *val)
{
  L7_uint32 numBytesTotal, numBytesAlloc, numBytesFree;

  if (osapiGetMemInfo(&numBytesTotal, &numBytesAlloc, &numBytesFree) != L7_SUCCESS)
  {
    return L7_ERROR;
  }

  *val = numBytesFree / 1024;

  return L7_SUCCESS;
}


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
L7_RC_t usmDbInfoSwitchCpuProcessMemAvailable(L7_uint32 UnitIndex,L7_uint32 *val)
{
  L7_uint32 numBytesTotal, numBytesAlloc, numBytesFree;

  if (osapiGetMemInfo(&numBytesTotal, &numBytesAlloc, &numBytesFree) != L7_SUCCESS)
  {
    return L7_ERROR;
  }

  *val = numBytesTotal / 1024;

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Gets a Allocated memory (measured in KB)
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
L7_RC_t usmDbInfoSwitchCpuProcessMemAllocated(L7_uint32 UnitIndex,L7_uint32 *val)
{
  L7_uint32 numBytesTotal, numBytesAlloc, numBytesFree;

  if (osapiGetMemInfo(&numBytesTotal, &numBytesAlloc, &numBytesFree) != L7_SUCCESS)
  {
    return L7_ERROR;
  }

  *val = numBytesAlloc / 1024;

  return L7_SUCCESS;
}
