/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  broad_stats.h
*
* @purpose   This file contains the prototypes for stats management
*
* @component hapi
*
* @comments
*
* @create    1/31/2001
*
* @author    nsummers
*
* @end
*
**********************************************************************/

#ifndef INCLUDE_BROAD_STATS_H
#define INCLUDE_BROAD_STATS_H

#include "l7_common.h"
#include "osapi.h"


/*********************************************************************
*
* @purpose Hooks in basic dapi cmds
*
* @param   DAPI_PORT_t *dapiPortPtr - pointer to the command table
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadStatisticsPortInit(DAPI_PORT_t *dapiPortPtr);

/*********************************************************************
*
* @purpose Initialize the statistics polling
*
* @param   DAPI_t *dapi_g - the driver object
*
* @returns L7_RC_t
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadStatisticsInit(DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Return the Statistics for the port
*
* @param   DAPI_USP_t *usp    - used to get the port pointer
* @param   DAPI_CMD_t  cmd    - the command, not used in this function
* @param   void       *data   - DAPI_INTF_MGMT_CMD_t.cmdData.statistics
* @param   DAPI_t      dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIntfStatistics(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose This is the task that polls the stats info
*
* @param   L7_ulong32  numArgs - not used for this function
* @param   DAPI_t     *dapi_g  - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
********************************************************************
L7_RC_t hapiBroadStatisticsTask(L7_ulong32 numArgs, DAPI_t *dapi_g); */

/*******************************************************************************
*
* @purpose Create a 64 bit stat from two 32 bit stats
*
* @param   L7_ulong64 *statPtr     - target of the addition
* @param   void       *statCurPtr  - current stat                
* @param   void       *statPrevPtr - previous stat
*
* @returns L7_RC_t  result
*
* @notes   none
*
* @end
*
*******************************************************************************/
L7_RC_t hapiBroadStatsPortIncrement(L7_ulong64 *statPtr, void *statCurPtr, void *statPrevPtr);


/*********************************************************************
*
* @purpose increment the stats counters for traffic on CPU ports
*
* @param   pkt      - Pointer to the start of data.
* @param   frameLength - Number of bytes in the packet.
* @param   usp      - the destination usp for this frame
* @param  *dapi_g   - system information
* @param   receive  - is this frame destined to CPU?
* @param   rx_untagged  - only valid when receive == TRUE. Indicates
*                         if packet was received untagged.
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadStatsCpuIncrement(L7_uchar8 *pkt,
                                   L7_uint32 frameLength,
                                   DAPI_USP_t *usp, DAPI_t *dapi_g,
                                   L7_BOOL receive,
                                   L7_BOOL rx_untagged);

/*********************************************************************
*
* @purpose Converts a Broadcom 64 bit stat to a LVL7 64 bit stat
*
* @param  *statPtr - points to 64 bit LVL7 stat
* @param   bcmStat - point to 64 bit broadcom stat
*
* @returns void
*
* @notes   none
*
* @end
*
*********************************************************************/
void hapiBroadStatsConvert(L7_ulong64 *statPtr, uint64 bcmStat);

#endif
