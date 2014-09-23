/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2009
*
**********************************************************************
*
* @filename timerange_cfg.c
*
* @purpose   time range component configuration handling
*
* @component timerange
*
* @comments  none
*
* @create 30-Nov-2009
*
* @author Siva Mannem
* @end
*
**********************************************************************/
#include <string.h>
#include "sysapi.h"
#include "nvstoreapi.h"
#include "timerange.h"
#include "timerange_exports.h"
#include "timerange_api.h"
#include "timerange_debug.h"
#include "timerange_cfg.h"

timeRangeCfgFileData_t *timeRangeCfgFileData    = L7_NULLPTR;
L7_BOOL                timeRangeCompInitialized = L7_FALSE;

/*********************************************************************
* @purpose  Saves Time Range configuration  to NVStore
*
* @param    none
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t timeRangeSave()
{
  L7_RC_t     rc = L7_SUCCESS;

  if (timeRangeHasDataChanged() == L7_TRUE)
  {
      timeRangeCfgFileData->cfgHdr.dataChanged = L7_FALSE;
      timeRangeCfgFileData->checkSum =
                             nvStoreCrc32((L7_char8 *)timeRangeCfgFileData,
                             (sizeof (timeRangeCfgFileData_t) -
                              sizeof (timeRangeCfgFileData->checkSum)));
      if (sysapiCfgFileWrite(L7_TIMERANGES_COMPONENT_ID,
                             timeRangeCfgFileData->cfgHdr.filename,
                             (L7_char8 *)timeRangeCfgFileData,
                             sizeof(timeRangeCfgFileData_t)) != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_TIMERANGES_COMPONENT_ID,
                "Error on call to sysapiCfgFileWrite file %s\n",
                timeRangeCfgFileData->cfgHdr.filename);
        rc = L7_FAILURE;
      }
  }

  return (rc);
}
/*********************************************************************
* @purpose  Checks if Time Range user config data has changed
*
* @param    none
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none
*
*
* @end
*********************************************************************/
L7_BOOL timeRangeHasDataChanged()
{
  return timeRangeCfgFileData->cfgHdr.dataChanged;
}
void timeRangeResetDataChanged(void)
{
   timeRangeCfgFileData->cfgHdr.dataChanged = L7_FALSE;
   return;
}

/*********************************************************************
* @purpose  Build default timeRange config data
*
* @param    ver @b{(input)} Software version of Config Data
*
* @returns  void
*
* @comments
*
* @end
*********************************************************************/
void timeRangeBuildDefaultConfigData(L7_uint32 ver)
{
  timeRangeCfgFileData->cfgHdr.version      = ver;
  timeRangeCfgFileData->cfgHdr.componentID  = L7_TIMERANGES_COMPONENT_ID;
  timeRangeCfgFileData->cfgHdr.type         = L7_CFG_DATA;
  timeRangeCfgFileData->cfgHdr.length       = (L7_uint32)sizeof(timeRangeCfgFileData_t);
  timeRangeCfgFileData->cfgHdr.dataChanged  = L7_FALSE;
  
  osapiStrncpy((L7_char8 *)timeRangeCfgFileData->cfgHdr.filename, TIMERANGE_CFG_FILENAME,
               sizeof(timeRangeCfgFileData->cfgHdr.filename));

}

/*********************************************************************
*
* @purpose  Apply config data
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FALIURE
*
* @comments None.
*
* @end
*
*********************************************************************/
L7_RC_t timeRangeApplyConfigData(void)
{
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Restore Time rangeuser config to defaults
*
* @param    void
*
* @returns  L7_SUCCESS or
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*
*********************************************************************/
L7_uint32 timeRangeRestore(void)
{
  L7_RC_t rc;
  L7_uint32 index;
  /* clear out all time ranges */
  rc = timeRangeIndexFirstGet(&index);
  while (rc == L7_SUCCESS)
  {
    (void)timeRangeDelete(index);          /* ignoring rc here */
    rc = timeRangeIndexNextGet(index, &index);
  }

  timeRangeBuildDefaultConfigData(timeRangeCfgFileData->cfgHdr.version);
  timeRangeCfgFileData->cfgHdr.dataChanged = L7_TRUE;

  return L7_SUCCESS;
}


