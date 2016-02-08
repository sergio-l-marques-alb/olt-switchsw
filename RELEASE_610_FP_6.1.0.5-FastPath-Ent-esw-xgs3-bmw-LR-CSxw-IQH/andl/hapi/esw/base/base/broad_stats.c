/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  broad_stats.c
*
* @purpose   This file contains the code for stats management
*
* @component hapi
*
* @comments
*
* @create    2/22/2001
*
* @author    nsummers
*
* @end
*
**********************************************************************/

#define L7_MAC_ENET_BCAST  /* Use the definition of BCAST from l7_packet.h */

#include <string.h>
#include "sysapi.h"
#include "osapi.h"

#include "cnfgr.h"
#include "default_cnfgr.h"

#include "broad_common.h"


#ifdef L7_QOS_PACKAGE
  #include "broad_qos.h"
#endif

#ifdef L7_MCAST_PACKAGE
  #include "broad_l3_mcast.h"
#endif

/* broadcom hw */
#include "bcm/stat.h"

#include "bcmx/lport.h"
#include "bcmx/stat.h"

#include "bcm/custom.h"
#include "bcm/stat.h"
#include "bcmx/custom.h"
#include "ibde.h"
#include "broad_utils.h"

/* Maximum age of the cache in milliseconds.
*/
#define BROAD_MAX_CACHE_AGE 2000

/* Semaphore to protect the stat database */
static void *hapiBroadStatSema;
void hapiBroadStatPortUpdate(DAPI_USP_t *usp, DAPI_INTF_STATS_t hapiCounterId, DAPI_t *dapi_g);
void hapiBroadStatCpuL3Update(DAPI_USP_t *usp, DAPI_INTF_STATS_t hapiCounterId, DAPI_t *dapi_g);

/* Statistics for the stats cache.
*/
static L7_uint32 hapiStatsCacheHits = 0;
static L7_uint32 hapiStatsCacheWrongIf = 0;
static L7_uint32 hapiStatsCacheTime = 0;
static L7_uint32 hapiStatsBadRc = 0;

/*********************************************************************
** Display cached stats access statistics.
*********************************************************************/
int hapiBroadDebugCacheStatsInfo (void)
{
  printf("Cache Hits: %d\n", hapiStatsCacheHits);
  printf("Cache miss due to wrong IF: %d\n", hapiStatsCacheWrongIf);
  printf("Cache miss due to timestamp: %d\n", hapiStatsCacheTime);
  printf("bcm stat get failed : %d\n", hapiStatsBadRc);

  return 0;
}

/*********************************************************************
*
* @purpose Get cached statistics for specified interface.
*
* @param   HAPI_PORT_t *hapiPortPtr - pointer to the command table
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
int hapiBroadCachedStatsGet (BROAD_PORT_t  *hapiPortPtr,
                              bcm_stat_val_t         bcmStatId,
                              uint64                 *bcmStat)
{
  int rc = BCM_E_NONE;
  static uint64         stats[snmpValCount];
  static L7_BOOL        cache_valid = L7_FALSE;
  static L7_uint32      cache_time_stamp;
  static bcmx_lport_t   cache_lport;
  L7_uint32 time = 0;
  
  if (cache_valid == L7_TRUE)
  {
    if (hapiPortPtr->bcmx_lport != cache_lport)
    {
      cache_valid = L7_FALSE;
      hapiStatsCacheWrongIf++;
    } else
    {
      time = osapiTimeMillisecondsGet ();
      //if ((time - cache_time_stamp) > BROAD_MAX_CACHE_AGE)
      if (osapiTimeMillisecondsDiff(time,cache_time_stamp) > BROAD_MAX_CACHE_AGE)
      {
        cache_valid = L7_FALSE;
        hapiStatsCacheTime++;
      }
    }
  }

  /* If cache is valid then return cahced counter, otherwise
  ** get counter from the port.
  */
  if (cache_valid == L7_FALSE)
  {
    if  (BCM_GPORT_IS_WLAN_PORT(hapiPortPtr->bcmx_lport))
    {
      rc = customx_port_stat_get(hapiPortPtr->bcmx_lport, stats);
      stats[snmpIfHCInOctets] = stats[bcmPortStatIngressBytes];
      stats[snmpIfHCInUcastPkts] = stats[bcmPortStatIngressPackets];
      stats[snmpIfHCOutOctets] = stats[bcmPortStatEgressBytes];
      stats[snmpIfHCOutUcastPkts] = stats[bcmPortStatEgressPackets];

      stats[bcmPortStatEgressBytes] = 0;    
      stats[bcmPortStatEgressBytes] = 0;    
      stats[bcmPortStatIngressBytes] = 0; 
      stats[bcmPortStatIngressPackets] = 0; 
    }
    else
    {
      rc = customx_stat_get (hapiPortPtr->bcmx_lport, stats);
    }
    if (L7_BCMX_OK(rc) != L7_TRUE)
    {
      hapiStatsBadRc++;
      return rc;
    }

    /* If we successfuly retrieved the statistics then
    ** reset the cache characteristics.
    */
    cache_time_stamp = time;
    cache_valid = L7_TRUE;
    cache_lport = hapiPortPtr->bcmx_lport;
  } else
  {
    hapiStatsCacheHits++;
  }

  *bcmStat = stats[bcmStatId];

  return rc;
}


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
L7_RC_t hapiBroadStatisticsPortInit(DAPI_PORT_t *dapiPortPtr)
{

  L7_RC_t   result = L7_SUCCESS;

  dapiPortPtr->cmdTable[DAPI_CMD_INTF_STATISTICS] = (HAPICTLFUNCPTR_t)hapiBroadIntfStatistics;

  return result;

}

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
L7_RC_t hapiBroadIntfStatistics(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t          result  = L7_SUCCESS;
  DAPI_INTF_MGMT_CMD_t  *dapiCmd = (DAPI_INTF_MGMT_CMD_t*)data;
  DAPI_PORT_t           *dapiPortPtr;
  DAPI_INTF_STATS_t      hapiCounterId;
  L7_ulong64            *valuePtr;

  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);

  /* If set, enable or disable the stats */
  if (dapiCmd->cmdData.statistics.getOrSet == DAPI_CMD_SET)
  {
    if (dapiCmd->cmdData.statistics.enable == L7_TRUE)
    {
      dapiPortPtr->statsEnable = L7_TRUE;
    }
  }
  else if (dapiCmd->cmdData.statistics.getOrSet == DAPI_CMD_GET)
  {
    if (dapiPortPtr->statsEnable == L7_TRUE)
    {
      hapiCounterId = dapiCmd->cmdData.statistics.counterId;

      if (hapiCounterId >= DAPI_STATS_NUM_OF_INTF_ENTRIES)
      {
        return L7_FAILURE;
      }

      valuePtr = dapiCmd->cmdData.statistics.valuePtr;

      if (valuePtr == L7_NULL)
      {
        return L7_FAILURE;
      }

      /* based off of the type of port, return the appropriate statistics */
      if ((IS_PORT_TYPE_PHYSICAL(dapiPortPtr) == L7_TRUE) ||
         (IS_PORT_TYPE_CAPWAP_TUNNEL(dapiPortPtr) == L7_TRUE))
      {
        hapiBroadStatPortUpdate(usp, hapiCounterId, dapi_g);

        valuePtr->high = dapiPortPtr->modeparm.physical.stats[hapiCounterId].high;
        valuePtr->low  = dapiPortPtr->modeparm.physical.stats[hapiCounterId].low;
      }
      else if (IS_PORT_TYPE_CPU(dapiPortPtr) == L7_TRUE)
      {
        hapiBroadStatCpuL3Update(usp, hapiCounterId, dapi_g);

        valuePtr->high = dapiPortPtr->modeparm.cpu.stats[hapiCounterId].high;
        valuePtr->low  = dapiPortPtr->modeparm.cpu.stats[hapiCounterId].low;
      }
      else
      {
        result = L7_FAILURE;
      }
    } else
    {
      valuePtr = dapiCmd->cmdData.statistics.valuePtr;

      if (valuePtr == L7_NULL)
      {
        return L7_FAILURE;
      }
      valuePtr->high = 0;
      valuePtr->low = 0;
    }
  }
  return result;

}



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
L7_RC_t hapiBroadStatisticsInit(DAPI_t *dapi_g)
{
  L7_RC_t                 result = L7_SUCCESS;


  if ((hapiBroadStatSema = osapiSemaBCreate(OSAPI_SEM_Q_FIFO,OSAPI_SEM_FULL)) == L7_NULL)
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\n%s %d: In %s call to 'osapiSemaBCreate'\n",
                   __FILE__, __LINE__, __FUNCTION__);
    return result;
  }

  return result;

}


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
* @notes   This is specific for the Master Driver.  Most
*          other platforms will handle this in the hardware.
*
* @end
*
*******************************************************************************/
L7_RC_t hapiBroadStatsPortIncrement(L7_ulong64 *statPtr, void *statCurPtr, void *statPrevPtr)
{

  L7_RC_t  result = L7_SUCCESS;
  L7_ulong32    *statCurPtr32;
  L7_ulong32    *statPrevPtr32;
  L7_ulong32     delta32;

  statCurPtr32  = (L7_ulong32*)statCurPtr;
  statPrevPtr32 = (L7_ulong32*)statPrevPtr;

  delta32 = *statCurPtr32 - *statPrevPtr32;

  if (statPtr->low > (statPtr->low + delta32))
    statPtr->high++;

  statPtr->low += delta32;

  return result;

}



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
                                   L7_BOOL rx_untagged)
{

  L7_RC_t  result = L7_SUCCESS;
  DAPI_PORT_t   *dapiPortPtr;
  L7_ulong64    *stats_p;
  L7_uint32      newVal;

  DAPI_USP_t         cpuUsp;

  if (CPU_USP_GET(&cpuUsp) == L7_FAILURE)
  {
    result = L7_FAILURE;
    return result;
  }

  for (cpuUsp.port=0; cpuUsp.port <dapi_g->unit[cpuUsp.unit]->slot[cpuUsp.slot]->numOfPortsInSlot; cpuUsp.port++)
  {
    dapiPortPtr = dapi_g->unit[cpuUsp.unit]->slot[cpuUsp.slot]->port[cpuUsp.port];

    if (IS_PORT_TYPE_CPU(dapiPortPtr) == L7_TRUE)
    {
      if (receive == L7_TRUE) /* received frames */
      {
        frameLength += 4; /* frameLength was decremented in hapiBroadReceive to strip CRC. We want to include
                             the CRC for purposes of statistics. */

        if (rx_untagged)
        {
          frameLength -= 4; /* HW always adds a VLAN tag, but if the frame was received from the wire as untagged,
                               then ensure the statistics reflect the actual frame length. */
        }

        stats_p = dapiPortPtr->modeparm.cpu.stats;

        newVal=stats_p[DAPI_STATS_RECEIVED_BYTES].low + frameLength;
        hapiBroadStatsPortIncrement(&stats_p[DAPI_STATS_RECEIVED_BYTES],                        &newVal, &stats_p[DAPI_STATS_RECEIVED_BYTES].low );

        /* perform size classification */
        if (frameLength < 64)
        {
          newVal=stats_p[DAPI_STATS_RECEIVED_GOOD_UNDERSIZED_FRAMES].low + 1;
          hapiBroadStatsPortIncrement(&stats_p[DAPI_STATS_RECEIVED_GOOD_UNDERSIZED_FRAMES],   &newVal, &stats_p[DAPI_STATS_RECEIVED_GOOD_UNDERSIZED_FRAMES].low);
        }
        else if (frameLength==64)
        {
          newVal=stats_p[DAPI_STATS_RECEIVED_64_BYTE_FRAMES].low + 1;
          hapiBroadStatsPortIncrement(&stats_p[DAPI_STATS_RECEIVED_64_BYTE_FRAMES],           &newVal, &stats_p[DAPI_STATS_RECEIVED_64_BYTE_FRAMES].low);
        }
        else if (frameLength<=127)
        {
          newVal=stats_p[DAPI_STATS_RECEIVED_65_TO_127_BYTE_FRAMES].low + 1;
          hapiBroadStatsPortIncrement(&stats_p[DAPI_STATS_RECEIVED_65_TO_127_BYTE_FRAMES],    &newVal, &stats_p[DAPI_STATS_RECEIVED_65_TO_127_BYTE_FRAMES].low);
        }
        else if (frameLength<=255)
        {
          newVal=stats_p[DAPI_STATS_RECEIVED_128_TO_255_BYTE_FRAMES].low + 1;
          hapiBroadStatsPortIncrement(&stats_p[DAPI_STATS_RECEIVED_128_TO_255_BYTE_FRAMES],   &newVal, &stats_p[DAPI_STATS_RECEIVED_128_TO_255_BYTE_FRAMES].low);
        }
        else if (frameLength<=511)
        {
          newVal=stats_p[DAPI_STATS_RECEIVED_256_TO_511_BYTE_FRAMES].low + 1;
          hapiBroadStatsPortIncrement(&stats_p[DAPI_STATS_RECEIVED_256_TO_511_BYTE_FRAMES],   &newVal, &stats_p[DAPI_STATS_RECEIVED_256_TO_511_BYTE_FRAMES].low);
        }
        else if (frameLength<=1023)
        {
          newVal=stats_p[DAPI_STATS_RECEIVED_512_TO_1023_BYTE_FRAMES].low + 1;
          hapiBroadStatsPortIncrement(&stats_p[DAPI_STATS_RECEIVED_512_TO_1023_BYTE_FRAMES],  &newVal, &stats_p[DAPI_STATS_RECEIVED_512_TO_1023_BYTE_FRAMES].low);
        }
        else if (frameLength<=1518)
        {
          newVal=stats_p[DAPI_STATS_RECEIVED_1024_TO_1518_BYTE_FRAMES].low + 1;
          hapiBroadStatsPortIncrement(&stats_p[DAPI_STATS_RECEIVED_1024_TO_1518_BYTE_FRAMES], &newVal, &stats_p[DAPI_STATS_RECEIVED_1024_TO_1518_BYTE_FRAMES].low);
        }
        else
        {
          newVal=stats_p[DAPI_STATS_RECEIVED_GOOD_OVERSIZED_FRAMES].low + 1;
          hapiBroadStatsPortIncrement(&stats_p[DAPI_STATS_RECEIVED_GOOD_OVERSIZED_FRAMES],    &newVal, &stats_p[DAPI_STATS_RECEIVED_GOOD_OVERSIZED_FRAMES].low);
        }

        /* determine if packet is bcast, mcast, or ucast */
        if (memcmp(&pkt[0],&L7_ENET_BCAST_MAC_ADDR,6)==0)
        {
          newVal=stats_p[DAPI_STATS_RECEIVED_BROADCAST_FRAMES].low + 1;
          hapiBroadStatsPortIncrement(&stats_p[DAPI_STATS_RECEIVED_BROADCAST_FRAMES],         &newVal, &stats_p[DAPI_STATS_RECEIVED_BROADCAST_FRAMES].low);
        }
        else if (pkt[0] & 0x01)
        {
          newVal=stats_p[DAPI_STATS_RECEIVED_MULTICAST_FRAMES].low + 1;
          hapiBroadStatsPortIncrement(&stats_p[DAPI_STATS_RECEIVED_MULTICAST_FRAMES],         &newVal, &stats_p[DAPI_STATS_RECEIVED_MULTICAST_FRAMES].low);
        }
        else
        {
          newVal=stats_p[DAPI_STATS_RECEIVED_UNICAST_FRAMES].low + 1;
          hapiBroadStatsPortIncrement(&stats_p[DAPI_STATS_RECEIVED_UNICAST_FRAMES],           &newVal, &stats_p[DAPI_STATS_RECEIVED_UNICAST_FRAMES].low);
        }

      }
      else
      {
        stats_p = dapiPortPtr->modeparm.cpu.stats;

        newVal=stats_p[DAPI_STATS_TRANSMITTED_BYTES].low + frameLength;
        hapiBroadStatsPortIncrement(&stats_p[DAPI_STATS_TRANSMITTED_BYTES],                        &newVal, &stats_p[DAPI_STATS_TRANSMITTED_BYTES].low );

        /* perform size classification */
        if (frameLength==64)
        {
          newVal=stats_p[DAPI_STATS_TRANSMITTED_64_BYTE_FRAMES].low + 1;
          hapiBroadStatsPortIncrement(&stats_p[DAPI_STATS_TRANSMITTED_64_BYTE_FRAMES],           &newVal, &stats_p[DAPI_STATS_TRANSMITTED_64_BYTE_FRAMES].low);
        }
        else if (frameLength<=127)
        {
          newVal=stats_p[DAPI_STATS_TRANSMITTED_65_TO_127_BYTE_FRAMES].low + 1;
          hapiBroadStatsPortIncrement(&stats_p[DAPI_STATS_TRANSMITTED_65_TO_127_BYTE_FRAMES],    &newVal, &stats_p[DAPI_STATS_TRANSMITTED_65_TO_127_BYTE_FRAMES].low);
        }
        else if (frameLength<=255)
        {
          newVal=stats_p[DAPI_STATS_TRANSMITTED_128_TO_255_BYTE_FRAMES].low + 1;
          hapiBroadStatsPortIncrement(&stats_p[DAPI_STATS_TRANSMITTED_128_TO_255_BYTE_FRAMES],   &newVal, &stats_p[DAPI_STATS_TRANSMITTED_128_TO_255_BYTE_FRAMES].low);
        }
        else if (frameLength<=511)
        {
          newVal=stats_p[DAPI_STATS_TRANSMITTED_256_TO_511_BYTE_FRAMES].low + 1;
          hapiBroadStatsPortIncrement(&stats_p[DAPI_STATS_TRANSMITTED_256_TO_511_BYTE_FRAMES],   &newVal, &stats_p[DAPI_STATS_TRANSMITTED_256_TO_511_BYTE_FRAMES].low);
        }
        else if (frameLength<=1023)
        {
          newVal=stats_p[DAPI_STATS_TRANSMITTED_512_TO_1023_BYTE_FRAMES].low + 1;
          hapiBroadStatsPortIncrement(&stats_p[DAPI_STATS_TRANSMITTED_512_TO_1023_BYTE_FRAMES],  &newVal, &stats_p[DAPI_STATS_TRANSMITTED_512_TO_1023_BYTE_FRAMES].low);
        }
        else if (frameLength<=1518)
        {
          newVal=stats_p[DAPI_STATS_TRANSMITTED_1024_TO_1518_BYTE_FRAMES].low + 1;
          hapiBroadStatsPortIncrement(&stats_p[DAPI_STATS_TRANSMITTED_1024_TO_1518_BYTE_FRAMES], &newVal, &stats_p[DAPI_STATS_TRANSMITTED_1024_TO_1518_BYTE_FRAMES].low);
        }

        /* determine if packet is bcast, mcast, or ucast */
        if (memcmp(&pkt[0],&L7_ENET_BCAST_MAC_ADDR,6)==0)
        {
          newVal=stats_p[DAPI_STATS_TRANSMITTED_BROADCAST_FRAMES].low + 1;
          hapiBroadStatsPortIncrement(&stats_p[DAPI_STATS_TRANSMITTED_BROADCAST_FRAMES],         &newVal, &stats_p[DAPI_STATS_TRANSMITTED_BROADCAST_FRAMES].low);
        }
        else if (pkt[0] & 0x01)
        {
          newVal=stats_p[DAPI_STATS_TRANSMITTED_MULTICAST_FRAMES].low + 1;
          hapiBroadStatsPortIncrement(&stats_p[DAPI_STATS_TRANSMITTED_MULTICAST_FRAMES],         &newVal, &stats_p[DAPI_STATS_TRANSMITTED_MULTICAST_FRAMES].low);
        }
        else
        {
          newVal=stats_p[DAPI_STATS_TRANSMITTED_UNICAST_FRAMES].low + 1;
          hapiBroadStatsPortIncrement(&stats_p[DAPI_STATS_TRANSMITTED_UNICAST_FRAMES],           &newVal, &stats_p[DAPI_STATS_TRANSMITTED_UNICAST_FRAMES].low);
        }
      }
    }
  }
  return result;
}

void hapiBroadStatCpuL3Update(DAPI_USP_t *usp, DAPI_INTF_STATS_t hapiCounterId, DAPI_t *dapi_g)
{
  BROAD_PORT_t          *hapiPortPtr;
  DAPI_PORT_t           *cpuDapiPortPtr;
  int                    rc;
  uint64                 bcmStat;
  uint64                 totalBcmStat = 0;
  L7_ulong64            *stats_p;
  DAPI_USP_t             searchUsp;
  bcm_stat_val_t         bcmStatId;

  cpuDapiPortPtr = DAPI_PORT_GET(usp, dapi_g);

  osapiSemaTake(hapiBroadStatSema,L7_WAIT_FOREVER);

  stats_p = cpuDapiPortPtr->modeparm.cpu.stats;

  switch (hapiCounterId)
  {
  case DAPI_STATS_RECEIVED_L3_IN:
    bcmStatId = snmpIpInReceives;
    break;

  case DAPI_STATS_RECEIVED_L3_IN_HDR_ERRORS:
    bcmStatId = snmpIpInHdrErrors;
    break;

  case DAPI_STATS_RECEIVED_L3_FORW_DATAGRAMS:
    bcmStatId = snmpIpForwDatagrams;
    break;

  case DAPI_STATS_RECEIVED_L3_ADDR_ERRORS:
    bcmStatId = snmpIpInDiscards;
    break;

  default:
    osapiSemaGive(hapiBroadStatSema);
    return;
    break;
  }

  for (searchUsp.unit=0; searchUsp.unit < dapi_g->system->totalNumOfUnits; searchUsp.unit++)
  {
    for (searchUsp.slot=0; searchUsp.slot < dapi_g->unit[searchUsp.unit]->numOfSlots; searchUsp.slot++)
    {
      if ((dapi_g->unit[searchUsp.unit]->slot[searchUsp.slot]->cardPresent == L7_TRUE) &&
          (dapi_g->unit[searchUsp.unit]->slot[searchUsp.slot]->cardType    == SYSAPI_CARD_TYPE_LINE))
      {
        for (searchUsp.port=0; searchUsp.port < dapi_g->unit[searchUsp.unit]->slot[searchUsp.slot]->numOfPortsInSlot; searchUsp.port++)
        {
          if (isValidUsp (&searchUsp, dapi_g) != L7_TRUE)
          {
            continue;
          }

          hapiPortPtr = HAPI_PORT_GET(&searchUsp, dapi_g);

          rc = hapiBroadCachedStatsGet (hapiPortPtr, bcmStatId, &bcmStat);

          if (L7_BCMX_OK(rc) != L7_TRUE)
            COMPILER_64_ZERO(bcmStat);

          totalBcmStat += bcmStat;
        }
      }
    }
  }
  hapiBroadStatsConvert(&stats_p[hapiCounterId], totalBcmStat);

  osapiSemaGive(hapiBroadStatSema);
}



void hapiBroadStatPortUpdate(DAPI_USP_t *usp, DAPI_INTF_STATS_t hapiCounterId, DAPI_t *dapi_g)
{
  DAPI_PORT_t           *dapiPortPtr;
  BROAD_PORT_t          *hapiPortPtr;
  int                    rc;
  uint64                 bcmStat;
  L7_ulong64            *stats_p;
  bcm_stat_val_t         bcmStatId;

  if (isValidUsp (usp,dapi_g ) != L7_TRUE)
  {
    return;
  }

  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);
  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);


  /* Get the logical port */


  switch (hapiCounterId)
  {
  case DAPI_STATS_RECEIVED_BYTES:
    bcmStatId = snmpIfHCInOctets;
    break;

  case DAPI_STATS_RECEIVED_GOOD_OVERSIZED_FRAMES:
    bcmStatId = snmpEtherStatsInOversizePkts;
    break;

  case DAPI_STATS_RECEIVED_ERROR_OVERSIZED_FRAMES:
    bcmStatId = snmpEtherStatsJabbers;
    break;

  case DAPI_STATS_RECEIVED_GOOD_UNDERSIZED_FRAMES:
    bcmStatId = snmpEtherStatsUndersizePkts;
    break;

  case DAPI_STATS_RECEIVED_ERROR_UNDERSIZED_FRAMES:
    bcmStatId = snmpEtherStatsFragments;
    break;

  case DAPI_STATS_RECEIVED_UNICAST_FRAMES:
    bcmStatId = snmpIfHCInUcastPkts;
    break;

  case DAPI_STATS_RECEIVED_MULTICAST_FRAMES:
    bcmStatId = snmpIfHCInMulticastPkts;
    break;

  case DAPI_STATS_RECEIVED_BROADCAST_FRAMES:
    bcmStatId = snmpIfHCInBroadcastPkts;
    break;

  case DAPI_STATS_RECEIVED_ALIGN_ERROR_FRAMES:
    bcmStatId = snmpDot3StatsAlignmentErrors;
    break;

  case DAPI_STATS_RECEIVED_FCS_ERROR_FRAMES:
    bcmStatId = snmpDot3StatsFCSErrors;
    break;

  case DAPI_STATS_RECEIVED_OVERRUN_FRAMES:
    bcmStatId = snmpDot1dBasePortDelayExceededDiscards;
    break;

  case DAPI_STATS_RECEIVED_FRAME_TOO_LONG:
    bcmStatId = snmpDot3StatsFrameTooLongs;
    break;

  case DAPI_STATS_RECEIVED_UNACCEPTABLE_FRAME_TYPE_DISCARDS:
    bcmStatId = snmpDot1dPortInDiscards;
    break;

  case DAPI_STATS_TRANSMITTED_BYTES:
    bcmStatId = snmpIfHCOutOctets;
    break;

  case DAPI_STATS_TRANSMITTED_UNICAST_FRAMES:
    bcmStatId = snmpIfHCOutUcastPkts;
    break;

  case DAPI_STATS_TRANSMITTED_MULTICAST_FRAMES:
    bcmStatId = snmpIfHCOutMulticastPkts;
    break;

  case DAPI_STATS_TRANSMITTED_BROADCAST_FRAMES:
    bcmStatId = snmpIfHCOutBroadcastPckts;
    break;

  case DAPI_STATS_TRANSMITTED_OVERSIZED_FRAMES:
    bcmStatId = snmpEtherStatsOutOversizePkts;
    break;

  case DAPI_STATS_TRANSMITTED_ONE_COLLISION_FRAMES:
    bcmStatId = snmpDot3StatsSingleCollisionFrames;
    break;

  case DAPI_STATS_TRANSMITTED_MULTIPLE_COLLISION_FRAMES:
    bcmStatId = snmpDot3StatsMultipleCollisionFrames;
    break;

  case DAPI_STATS_TRANSMITTED_EXCESSIVE_COLLISION_FRAMES:
    bcmStatId = snmpDot3StatsExcessiveCollisions;
    break;

  case DAPI_STATS_TRANSMITTED_LATE_COLLISIONS:
    bcmStatId = snmpDot3StatsLateCollisions;
    break;

  case DAPI_STATS_TRANSMITTED_AND_RECEIVED_64_BYTE_FRAMES:
    bcmStatId = snmpEtherStatsPkts64Octets;
    break;

  case DAPI_STATS_TRANSMITTED_AND_RECEIVED_65_127_BYTE_FRAMES:
    bcmStatId = snmpEtherStatsPkts65to127Octets;
    break;

  case DAPI_STATS_TRANSMITTED_AND_RECEIVED_128_255_BYTE_FRAMES:
    bcmStatId = snmpEtherStatsPkts128to255Octets;
    break;

  case DAPI_STATS_TRANSMITTED_AND_RECEIVED_256_511_BYTE_FRAMES:
    bcmStatId = snmpEtherStatsPkts256to511Octets;
    break;

  case DAPI_STATS_TRANSMITTED_AND_RECEIVED_512_1023_BYTE_FRAMES:
    bcmStatId = snmpEtherStatsPkts512to1023Octets;
    break;

  case DAPI_STATS_TRANSMITTED_AND_RECEIVED_1024_1518_BYTE_FRAMES:
    bcmStatId = snmpEtherStatsPkts1024to1518Octets;
    break;

  case DAPI_STATS_TRANSMITTED_AND_RECEIVED_1519_1522_BYTE_FRAMES:
    bcmStatId = snmpBcmEtherStatsPkts1519to1522Octets;
    break;

  case DAPI_STATS_TRANSMITTED_AND_RECEIVED_1523_2047_BYTE_FRAMES:
    bcmStatId = snmpBcmEtherStatsPkts1522to2047Octets;
    break;

  case DAPI_STATS_TRANSMITTED_AND_RECEIVED_2048_4095_BYTE_FRAMES:
    bcmStatId = snmpBcmEtherStatsPkts2048to4095Octets;
    break;

  case DAPI_STATS_TRANSMITTED_AND_RECEIVED_4096_9216_BYTE_FRAMES:
    bcmStatId = snmpBcmEtherStatsPkts4095to9216Octets;
    break;

  case  DAPI_STATS_ETHER_STATS_DROP_EVENTS:
    bcmStatId = snmpEtherStatsDropEvents;
    break;

  case  DAPI_STATS_SNMPIFOUTDISCARD_FRAMES:
    bcmStatId = snmpIfOutDiscards;
    break;

  case  DAPI_STATS_SNMPIFINDISCARD_FRAMES:
    bcmStatId = snmpIfInDiscards;
    break;
  
  case DAPI_STATS_RECEIVED_64_BYTE_FRAMES:
    bcmStatId = snmpBcmReceivedPkts64Octets;
    break;

  case DAPI_STATS_RECEIVED_65_TO_127_BYTE_FRAMES:
    bcmStatId = snmpBcmReceivedPkts65to127Octets;
    break;

  case DAPI_STATS_RECEIVED_128_TO_255_BYTE_FRAMES:
    bcmStatId = snmpBcmReceivedPkts128to255Octets;
    break;

  case DAPI_STATS_RECEIVED_256_TO_511_BYTE_FRAMES:
    bcmStatId = snmpBcmReceivedPkts256to511Octets;
    break;

  case DAPI_STATS_RECEIVED_512_TO_1023_BYTE_FRAMES:
    bcmStatId = snmpBcmReceivedPkts512to1023Octets;
    break;

  case DAPI_STATS_RECEIVED_1024_TO_1518_BYTE_FRAMES:
    bcmStatId = snmpBcmReceivedPkts1024to1518Octets;
    break;

  case DAPI_STATS_TRANSMITTED_64_BYTE_FRAMES:
    bcmStatId = snmpBcmTransmittedPkts64Octets;
    break;

  case DAPI_STATS_TRANSMITTED_65_TO_127_BYTE_FRAMES:
    bcmStatId = snmpBcmTransmittedPkts65to127Octets;
    break;

  case DAPI_STATS_TRANSMITTED_128_TO_255_BYTE_FRAMES:
    bcmStatId = snmpBcmTransmittedPkts128to255Octets;
    break;

  case DAPI_STATS_TRANSMITTED_256_TO_511_BYTE_FRAMES:
    bcmStatId = snmpBcmTransmittedPkts256to511Octets;
    break;

  case DAPI_STATS_TRANSMITTED_512_TO_1023_BYTE_FRAMES:
    bcmStatId = snmpBcmTransmittedPkts512to1023Octets;
    break;

  case DAPI_STATS_TRANSMITTED_1024_TO_1518_BYTE_FRAMES:
    bcmStatId = snmpBcmTransmittedPkts1024to1518Octets;
    break;

  case DAPI_STATS_RECEIVED_PAUSE_FRAMES:
    bcmStatId = snmpDot3InPauseFrames;
    break;

  case DAPI_STATS_TRANSMITTED_PAUSE_FRAMES:
    bcmStatId = snmpDot3OutPauseFrames;
    break;

  case DAPI_STATS_IPV6_IN_RECEIVES:
    bcmStatId = snmpIpv6IfStatsInReceives;
    break;

  case DAPI_STATS_IPV6_IN_HDR_ERRS:
    bcmStatId = snmpIpv6IfStatsInHdrErrors;
    break;

  case DAPI_STATS_IPV6_IN_ADDR_ERRS:
    bcmStatId = snmpIpv6IfStatsInAddrErrors;
    break;

  case DAPI_STATS_IPV6_IN_DISCARDS:
    bcmStatId = snmpIpv6IfStatsInDiscards;
    break;

  case DAPI_STATS_IPV6_OUT_FORW_DATAGRAMS:
    bcmStatId = snmpIpv6IfStatsOutForwDatagrams;
    break;

  case DAPI_STATS_IPV6_OUT_DISCARDS:
    bcmStatId = snmpIpv6IfStatsOutDiscards;
    break;

  case DAPI_STATS_IPV6_IN_MCAST_PKTS:
    bcmStatId = snmpIpv6IfStatsInMcastPkts;
    break;

  case DAPI_STATS_IPV6_OUT_MCAST_PKTS:
    bcmStatId = snmpIpv6IfStatsOutMcastPkts;
    break;

  default:
    return;
    break;
  }

  osapiSemaTake(hapiBroadStatSema,L7_WAIT_FOREVER);

  stats_p = dapiPortPtr->modeparm.physical.stats;

  rc = hapiBroadCachedStatsGet (hapiPortPtr, bcmStatId, &bcmStat);

  if (L7_BCMX_OK(rc) != L7_TRUE)
  {
    COMPILER_64_ZERO(bcmStat);
  }

  hapiBroadStatsConvert(&stats_p[hapiCounterId], bcmStat);


  osapiSemaGive(hapiBroadStatSema);


}

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
void hapiBroadStatsConvert(L7_ulong64 *statPtr, uint64 bcmStat)
{
  COMPILER_64_TO_32_HI(statPtr->high, bcmStat);
  COMPILER_64_TO_32_LO(statPtr->low, bcmStat);
}



/*********************************************************************
**  Debug Functions
*********************************************************************/

static L7_uint32 statsCollectorLoopCount = 0;
static L7_BOOL   statsCollectorTerminate = L7_TRUE;
extern DAPI_t    *dapi_g;

/*******************************************************************
** Stats polling task.
**
** This task attempts to retrieve every available statistic from
** every port in the stack.
**
*******************************************************************/
static void hapiBroadDebugStatsCollectorTask (void)
{
 DAPI_USP_t             searchUsp;
 int i;
 L7_uint32 loop_count;

 printf("Starting Stats Collector Task. LoopCount: %d\n",
        statsCollectorLoopCount);

 loop_count = 0;
 do
 {

 for (searchUsp.unit=0; searchUsp.unit < dapi_g->system->totalNumOfUnits; searchUsp.unit++)
 {
  for (searchUsp.slot=0; searchUsp.slot < dapi_g->unit[searchUsp.unit]->numOfSlots; searchUsp.slot++)
  {
    if ((dapi_g->unit[searchUsp.unit]->slot[searchUsp.slot]->cardPresent == L7_TRUE) &&
        (dapi_g->unit[searchUsp.unit]->slot[searchUsp.slot]->cardType    == SYSAPI_CARD_TYPE_LINE))
    {
      for (searchUsp.port=0; searchUsp.port < dapi_g->unit[searchUsp.unit]->slot[searchUsp.slot]->numOfPortsInSlot; searchUsp.port++)
      {
        if (isValidUsp (&searchUsp, dapi_g) != L7_TRUE)
        {
          continue;
        }

        for (i = 0; i < DAPI_STATS_NUM_OF_INTF_ENTRIES; i++)
        {
          hapiBroadStatPortUpdate(&searchUsp, i, dapi_g);
          if (statsCollectorTerminate == L7_TRUE)
          {
            printf("Stats connection terminated after %d loops.\n", loop_count);
            return;
          }
        }

      }
    }
   }
 }
 loop_count++;

 } while ((statsCollectorLoopCount != 0) &&
          (loop_count < statsCollectorLoopCount));

 statsCollectorTerminate = L7_TRUE;

 printf("Stats connection terminated after %d loops.\n", loop_count);
}

/*******************************************************************
** Start Stats Collection
**
*******************************************************************/
L7_int32 hapiBroadDebugStatsCollectStart (L7_uint32 loop_count)
{

  if (statsCollectorTerminate == L7_FALSE)
  {
    printf("ERROR: Stats collection is already in progress.\n");
    return -1;
  }
  statsCollectorLoopCount = loop_count;
  statsCollectorTerminate = L7_FALSE;

  if (osapiTaskCreate("hapi: Stats", hapiBroadDebugStatsCollectorTask, 0, 0,
                      L7_DEFAULT_STACK_SIZE,
                      L7_DEFAULT_TASK_PRIORITY,
                      L7_DEFAULT_TASK_SLICE) == L7_ERROR)
  {
	printf("ERROR: Failed to start stats collector task\n");
	return -1;
  }


  return 0;
}

/*******************************************************************
** Stop Stats Collection
**
*******************************************************************/
L7_int32 hapiBroadDebugStatsCollectStop (void)
{
  if (statsCollectorTerminate == L7_TRUE)
  {
    printf("ERROR: Stats collection is not running.\n");
    return -1;
  }

  statsCollectorTerminate = L7_TRUE;
  return 0;
}
