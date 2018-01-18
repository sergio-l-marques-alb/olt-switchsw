/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2009
*
**********************************************************************
*
* @filename  broad_hpc_helper.h
*
* @purpose   Technology specific functions and data for the hardware
*            platform control component
*
* @component
*
* @create    02/04/2009
*
* @author    colinw
* @end
*
*********************************************************************/
#ifndef INCLUDE_BROAD_HPC_HELPER_H
#define INCLUDE_BROAD_HPC_HELPER_H

#include "datatypes.h"

typedef enum
{
  HPC_HELPER_STATS_TX_BYTES = 0,
  HPC_HELPER_STATS_TX_64_BYTE_FRAMES,
  HPC_HELPER_STATS_TX_65_TO_127_BYTE_FRAMES,
  HPC_HELPER_STATS_TX_128_TO_255_BYTE_FRAMES,
  HPC_HELPER_STATS_TX_256_TO_511_BYTE_FRAMES,
  HPC_HELPER_STATS_TX_512_TO_1023_BYTE_FRAMES,
  HPC_HELPER_STATS_TX_1024_TO_1518_BYTE_FRAMES,
  HPC_HELPER_STATS_TX_1519_TO_1530_BYTE_FRAMES,
  HPC_HELPER_STATS_TX_UNICAST_FRAMES,
  HPC_HELPER_STATS_TX_MULTICAST_FRAMES,
  HPC_HELPER_STATS_TX_BROADCAST_FRAMES,
  HPC_HELPER_STATS_LAST

} HPC_HELPER_STATS_t;

/*********************************************************************
*
* @purpose Get a stat from the HPC helper
*
* @param   
*
* @returns L7_RC_t result
*
* @notes   This function is entered only by one task at a time.
*
*          The cache is optimized for use by RMON, which asks for
*          multiple counters for the same interface before proceeding
*          to the next interface.
*
* @end
*
*********************************************************************/
int hpcHardwareHelperStatDeltaGet(HPC_HELPER_STATS_t  helperStatId,
                                  L7_ulong64         *helperStatValue);

#endif
