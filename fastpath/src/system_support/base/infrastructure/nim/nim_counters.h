/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename nim_counters.h
*
* @purpose Network Interface Manager port hardware counters
*
* @component nim
*
* @comments none
*
* @create 10/23/2000
*
* @author bmutz
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/



#ifndef INCLUDE_NIM_COUNTERS
#define INCLUDE_NIM_COUNTERS

#include "nim_util.h"

L7_RC_t nimGetCPUCounters ( counterValue_entry_t *pCounter );
L7_RC_t nimGetInterfaceCounters ( counterValue_entry_t *pCounter );

/*  */
#define NIM_CFG_EXT_COUNTERS()      \
   pTmpS = pStatsParmList;          \
                                    \
   for (i = 0 ; i < listSize ; i++) \
   {                                \
      pTmpS->cKey = intIfNum;       \
      pTmpS++;                      \
   }                                


#define NIM_CHECK_CFG_EXT_COUNTERS()                                                         \
   pTmpS = pStatsParmList;                                                                   \
                                                                                             \
   for (i = 0 ; i < listSize ; i++)                                                          \
   {                                                                                         \
      if (pTmpS->status != L7_SUCCESS)                                                       \
         NIM_LOG_MSG("Counter %d on interface %d failed to create\n", pTmpS->cId, pTmpS->cKey);  \
                                                                                             \
      pTmpS++;                                                                               \
   }

#define NIM_CFG_MUTANT_COUNTERS(j)                                   \
   {                                                                 \
      pMutling_id_t pTmpM;                                           \
      pTmpM = j;                                                     \
                                                                     \
      for (i = 0 ; i < sizeof( j ) / sizeof( mutling_id_t ) ; i++)   \
      {                                                              \
         pTmpM->cKey = intIfNum;                                     \
         pTmpM++;                                                    \
      }                                                              \
   }


static L7_uint32 statsFastEthernet_base_ctr_list[] =
{
  L7_PLATFORM_CTR_RX_TOTAL_BYTES,
  L7_PLATFORM_CTR_RX_64,
  L7_PLATFORM_CTR_RX_65_127,
  L7_PLATFORM_CTR_RX_128_255,
  L7_PLATFORM_CTR_RX_256_511,
  L7_PLATFORM_CTR_RX_512_1023,
  L7_PLATFORM_CTR_RX_1024_1518,
  L7_PLATFORM_CTR_RX_1519_1530,
  L7_PLATFORM_CTR_RX_GOOD_OVERSIZE,
  L7_PLATFORM_CTR_RX_ERROR_OVERSIZE,
  L7_PLATFORM_CTR_RX_GOOD_UNDERSIZE,
  L7_PLATFORM_CTR_RX_ERROR_UNDERSIZE,
  L7_PLATFORM_CTR_RX_UCAST_FRAMES,
  L7_PLATFORM_CTR_RX_MCAST_FRAMES,
  L7_PLATFORM_CTR_RX_BCAST_FRAMES,
  L7_PLATFORM_CTR_RX_ALIGN_ERRORS,
  L7_PLATFORM_CTR_RX_FCS_ERRORS,
  L7_PLATFORM_CTR_RX_OVERRUNS,
  L7_PLATFORM_CTR_RX_FRAME_TOO_LONG,
  L7_PLATFORM_CTR_TX_TOTAL_BYTES,
  L7_PLATFORM_CTR_TX_64,
  L7_PLATFORM_CTR_TX_65_127,
  L7_PLATFORM_CTR_TX_128_255,
  L7_PLATFORM_CTR_TX_256_511,
  L7_PLATFORM_CTR_TX_512_1023,
  L7_PLATFORM_CTR_TX_1024_1518,
  L7_PLATFORM_CTR_TX_1519_1530,
  L7_PLATFORM_CTR_TX_UCAST_FRAMES,
  L7_PLATFORM_CTR_TX_MCAST_FRAMES,
  L7_PLATFORM_CTR_TX_BCAST_FRAMES,
  L7_PLATFORM_CTR_TX_FCS_ERRORS,
  L7_PLATFORM_CTR_TX_OVERSIZED,
  L7_PLATFORM_CTR_TX_UNDERRUN_ERRORS,
  L7_PLATFORM_CTR_TX_ONE_COLLISION,
  L7_PLATFORM_CTR_TX_MULTIPLE_COLLISION,
  L7_PLATFORM_CTR_TX_EXCESSIVE_COLLISION,
  L7_PLATFORM_CTR_TX_LATE_COLLISION,

  L7_PLATFORM_CTR_TX_RX_64,
  L7_PLATFORM_CTR_TX_RX_65_127,
  L7_PLATFORM_CTR_TX_RX_128_255,
  L7_PLATFORM_CTR_TX_RX_256_511,
  L7_PLATFORM_CTR_TX_RX_512_1023,
  L7_PLATFORM_CTR_TX_RX_1024_1518,
  L7_PLATFORM_CTR_TX_RX_1519_1522,
  L7_PLATFORM_CTR_TX_RX_1523_2047,
  L7_PLATFORM_CTR_TX_RX_2048_4095,
  L7_PLATFORM_CTR_TX_RX_4096_9216,
  L7_PLATFORM_CTR_ETHER_STATS_DROP_EVENTS,
  L7_PLATFORM_CTR_SNMPIFOUTDISCARD_FRAMES,
  L7_PLATFORM_CTR_SNMPIFINDISCARD_FRAMES
};


static L7_uint32 statsCPU_ctr_list[] =
{
  L7_PLATFORM_CTR_RX_TOTAL_BYTES,
  L7_PLATFORM_CTR_RX_64,
  L7_PLATFORM_CTR_RX_65_127,
  L7_PLATFORM_CTR_RX_128_255,
  L7_PLATFORM_CTR_RX_256_511,
  L7_PLATFORM_CTR_RX_512_1023,
  L7_PLATFORM_CTR_RX_1024_1518,
  L7_PLATFORM_CTR_RX_1519_1530,
  L7_PLATFORM_CTR_RX_GOOD_OVERSIZE,
  L7_PLATFORM_CTR_RX_GOOD_UNDERSIZE,
  L7_PLATFORM_CTR_RX_UCAST_FRAMES,
  L7_PLATFORM_CTR_RX_MCAST_FRAMES,
  L7_PLATFORM_CTR_RX_BCAST_FRAMES,  /*ask Smanders*/
  L7_PLATFORM_CTR_TX_TOTAL_BYTES,
  L7_PLATFORM_CTR_TX_64,
  L7_PLATFORM_CTR_TX_65_127,
  L7_PLATFORM_CTR_TX_128_255,
  L7_PLATFORM_CTR_TX_256_511,
  L7_PLATFORM_CTR_TX_512_1023,
  L7_PLATFORM_CTR_TX_1024_1518,
  L7_PLATFORM_CTR_TX_1519_1530,
  L7_PLATFORM_CTR_TX_UCAST_FRAMES,
  L7_PLATFORM_CTR_TX_MCAST_FRAMES,
  L7_PLATFORM_CTR_TX_BCAST_FRAMES,      /*ask Smanders*/

  L7_PLATFORM_CTR_RX_ERROR_OVERSIZE,
  L7_PLATFORM_CTR_RX_ERROR_UNDERSIZE,
  L7_PLATFORM_CTR_RX_ALIGN_ERRORS,   
  L7_PLATFORM_CTR_RX_FCS_ERRORS,   
  L7_PLATFORM_CTR_RX_OVERRUNS,

  L7_PLATFORM_CTR_TX_FCS_ERRORS,
  L7_PLATFORM_CTR_TX_OVERSIZED,
  L7_PLATFORM_CTR_TX_UNDERRUN_ERRORS
};


/* ifInDiscards, ifOutDiscards, Local Traffic Discards, Total Transmit Discards, ifInErrors */

/* Go thru doc with Shekhar/Archana */


#endif


