/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2009
*
**********************************************************************
*
* @filename timerange_cfg.h
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

#ifndef TIMERANGE_CFG_H
#define TIMERANGE_CFG_H

#define TIMERANGE_CFG_FILENAME        "timerange.cfg"
#define TIMERANGE_CFG_VER_1           0x1
#define TIMERANGE_CFG_VER_CURRENT     TIMERANGE_CFG_VER_1

/*Time ranges component configuration data*/
/*No default configuration exists*/
typedef struct {
  L7_fileHdr_t      cfgHdr;
  L7_uint32         checkSum;

} timeRangeCfgFileData_t;

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
L7_RC_t timeRangeSave();
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
L7_BOOL timeRangeHasDataChanged();

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
void timeRangeBuildDefaultConfigData(L7_uint32 ver);

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
L7_RC_t timeRangeApplyConfigData(void);

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
L7_uint32 timeRangeRestore(void);
void timeRangeResetDataChanged(void);

#endif /* TIMERANGE_CFG_H */
