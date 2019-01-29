/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* Name: dtl_stats.c
*
* Purpose: This file contains the functions to transform the statistics
*          from various components's driver requests. 
*
* Component: Device Transformation Layer (DTL)
*
* Commnets: 
*
* Created by: Shekhar Kalyanam 3/14/2001 
*
*********************************************************************/
/*************************************************************
                    
*************************************************************/





/*
**********************************************************************
*                           HEADER FILES
**********************************************************************
*/

#define DTLCTRL_STATS_GLOBALS              /* Enable global space   */    
#include "dtlinclude.h"


#if DTLCTRL_COMPONENT_STATS

static L7_uint32 dtlCounters_list[] =
{ L7_PLATFORM_CTR_RX_DISCARDS,
  L7_PLATFORM_CTR_TX_DISCARDS
};

typedef struct 
{
  L7_uint32 InDiscards;
  L7_uint32 OutDiscards;


}DTL_COUNTER_t;

typedef struct 
{
	L7_uint32 platform_stat;
	DAPI_INTF_STATS_t dapi_stat;
} DTL_DAPI_COUNTER_MAP_t;


static DTL_COUNTER_t  dtlCounterValue[L7_ALL_INTERFACES];

/* Map between platform counters and DAPI counters. 
*/
#define DTL_DAPI_MAP_SIZE ((sizeof (dtlDapiMap)) / (sizeof (DTL_DAPI_COUNTER_MAP_t)))
static DTL_DAPI_COUNTER_MAP_t dtlDapiMap[] = 
{
	{L7_PLATFORM_CTR_RX_TOTAL_BYTES,  DAPI_STATS_RECEIVED_BYTES },
	{L7_PLATFORM_CTR_RX_64,           DAPI_STATS_RECEIVED_64_BYTE_FRAMES},
    {L7_PLATFORM_CTR_RX_65_127,       DAPI_STATS_RECEIVED_65_TO_127_BYTE_FRAMES},
    {L7_PLATFORM_CTR_RX_128_255,      DAPI_STATS_RECEIVED_128_TO_255_BYTE_FRAMES},
    {L7_PLATFORM_CTR_RX_256_511,      DAPI_STATS_RECEIVED_256_TO_511_BYTE_FRAMES},
    {L7_PLATFORM_CTR_RX_512_1023,     DAPI_STATS_RECEIVED_512_TO_1023_BYTE_FRAMES},
    {L7_PLATFORM_CTR_RX_1024_1518,    DAPI_STATS_RECEIVED_1024_TO_1518_BYTE_FRAMES},
    {L7_PLATFORM_CTR_RX_1519_1530,    DAPI_STATS_RECEIVED_1519_TO_1530_BYTE_FRAMES},
    {L7_PLATFORM_CTR_RX_GOOD_OVERSIZE,  DAPI_STATS_RECEIVED_GOOD_OVERSIZED_FRAMES},
    {L7_PLATFORM_CTR_RX_ERROR_OVERSIZE, DAPI_STATS_RECEIVED_ERROR_OVERSIZED_FRAMES},
    {L7_PLATFORM_CTR_RX_GOOD_UNDERSIZE, DAPI_STATS_RECEIVED_GOOD_UNDERSIZED_FRAMES},
    {L7_PLATFORM_CTR_RX_ERROR_UNDERSIZE,DAPI_STATS_RECEIVED_ERROR_UNDERSIZED_FRAMES},
    {L7_PLATFORM_CTR_RX_UCAST_FRAMES, DAPI_STATS_RECEIVED_UNICAST_FRAMES},
    {L7_PLATFORM_CTR_RX_MCAST_FRAMES, DAPI_STATS_RECEIVED_MULTICAST_FRAMES},
    {L7_PLATFORM_CTR_RX_BCAST_FRAMES, DAPI_STATS_RECEIVED_BROADCAST_FRAMES},
    {L7_PLATFORM_CTR_RX_ALIGN_ERRORS, DAPI_STATS_RECEIVED_ALIGN_ERROR_FRAMES},
    {L7_PLATFORM_CTR_RX_FCS_ERRORS,   DAPI_STATS_RECEIVED_FCS_ERROR_FRAMES},
    {L7_PLATFORM_CTR_RX_OVERRUNS,     DAPI_STATS_RECEIVED_OVERRUN_FRAMES},
    {L7_PLATFORM_CTR_RX_FRAME_TOO_LONG, DAPI_STATS_RECEIVED_FRAME_TOO_LONG},
    {L7_PLATFORM_CTR_TX_TOTAL_BYTES,  DAPI_STATS_TRANSMITTED_BYTES},
    {L7_PLATFORM_CTR_TX_64,  		DAPI_STATS_TRANSMITTED_64_BYTE_FRAMES},
    {L7_PLATFORM_CTR_TX_65_127, 	DAPI_STATS_TRANSMITTED_65_TO_127_BYTE_FRAMES},
    {L7_PLATFORM_CTR_TX_128_255,  	DAPI_STATS_TRANSMITTED_128_TO_255_BYTE_FRAMES},
    {L7_PLATFORM_CTR_TX_256_511,  	DAPI_STATS_TRANSMITTED_256_TO_511_BYTE_FRAMES},
    {L7_PLATFORM_CTR_TX_512_1023,  	DAPI_STATS_TRANSMITTED_512_TO_1023_BYTE_FRAMES},
    {L7_PLATFORM_CTR_TX_1024_1518,  DAPI_STATS_TRANSMITTED_1024_TO_1518_BYTE_FRAMES},
    {L7_PLATFORM_CTR_TX_1519_1530,  DAPI_STATS_TRANSMITTED_1519_TO_1530_BYTE_FRAMES},
    {L7_PLATFORM_CTR_TX_UCAST_FRAMES,  	DAPI_STATS_TRANSMITTED_UNICAST_FRAMES},
    {L7_PLATFORM_CTR_TX_MCAST_FRAMES,  	DAPI_STATS_TRANSMITTED_MULTICAST_FRAMES},
    {L7_PLATFORM_CTR_TX_BCAST_FRAMES,  	DAPI_STATS_TRANSMITTED_BROADCAST_FRAMES},
    {L7_PLATFORM_CTR_TX_FCS_ERRORS,  	DAPI_STATS_TRANSMITTED_FCS_ERROR_FRAMES},
    {L7_PLATFORM_CTR_TX_OVERSIZED,  	DAPI_STATS_TRANSMITTED_OVERSIZED_FRAMES},
    {L7_PLATFORM_CTR_TX_UNDERRUN_ERRORS,  	DAPI_STATS_TRANSMITTED_UNDERRUN_ERROR_FRAMES},
    {L7_PLATFORM_CTR_TX_ONE_COLLISION,  	DAPI_STATS_TRANSMITTED_ONE_COLLISION_FRAMES},
    {L7_PLATFORM_CTR_TX_MULTIPLE_COLLISION,  DAPI_STATS_TRANSMITTED_MULTIPLE_COLLISION_FRAMES},
    {L7_PLATFORM_CTR_TX_EXCESSIVE_COLLISION,  DAPI_STATS_TRANSMITTED_EXCESSIVE_COLLISION_FRAMES},
    {L7_PLATFORM_CTR_TX_LATE_COLLISION,  		DAPI_STATS_TRANSMITTED_LATE_COLLISIONS},
    {L7_PLATFORM_CTR_LOCAL_TRAFFIC_DISCARDS,  	DAPI_STATS_RECEIVED_LOCAL_TRAFFIC_DISCARDS},
    {L7_PLATFORM_CTR_RX_PAUSE_FRAMES,  		DAPI_STATS_RECEIVED_PAUSE_FRAMES},
    {L7_PLATFORM_CTR_UNACCEPTABLE_FRAME_TYPE_DISCARDS,  DAPI_STATS_RECEIVED_UNACCEPTABLE_FRAME_TYPE_DISCARDS},
    {L7_PLATFORM_CTR_INGRESS_FILTER_DISCARDS,  			DAPI_STATS_RECEIVED_INGRESS_FILTER_DISCARDS},
    {L7_PLATFORM_CTR_INGRESS_VLAN_VIABLE_DISCARDS,  	DAPI_STATS_RECEIVED_VLAN_VIABLE_DISCARDS},
    {L7_PLATFORM_CTR_MULTICAST_TREE_VIABLE_DISCARDS,  	DAPI_STATS_RECEIVED_MULTICAST_TREE_VIABLE_DISCARDS},
    {L7_PLATFORM_CTR_RESERVED_ADDRESS_DISCARDS,  		DAPI_STATS_RECEIVED_RESERVED_ADDRESS_DISCARDS},
    {L7_PLATFORM_CTR_BCAST_STORM_RECOVERY_DISCARDS,  	DAPI_STATS_RECEIVED_BROADCAST_MULTICAST_STORM_RECOVERY_DISCARDS},
    {L7_PLATFORM_CTR_CFI_DISCARDS,  				DAPI_STATS_RECEIVED_CFI_DISCARDS},
    {L7_PLATFORM_CTR_UPSTREAM_THRESHOLD_DISCARDS,  	DAPI_STATS_RECEIVED_UPSTREAM_THRESHOLD_DISCARDS},
    {L7_PLATFORM_CTR_TX_PAUSE_FRAMES,  				DAPI_STATS_TRANSMITTED_PAUSE_FRAMES},
    {L7_PLATFORM_CTR_PORT_MEMBERSHIP_DISCARDS,  	DAPI_STATS_TRANSMITTED_PORT_MEMBERSHIP_DISCARDS},
    {L7_PLATFORM_CTR_EGRESS_VLAN_VIABLE_DISCARDS,  	DAPI_STATS_TRANSMITTED_VLAN_VIABLE_DISCARDS},
    {L7_PLATFORM_CTR_RX_IP_IN_RECEIVES,  			DAPI_STATS_RECEIVED_L3_IN},
    {L7_PLATFORM_CTR_RX_IP_IN_HDR_ERRORS,  			DAPI_STATS_RECEIVED_L3_IN_HDR_ERRORS},
    {L7_PLATFORM_CTR_RX_IP_IN_ADDR_ERRORS,  		DAPI_STATS_RECEIVED_L3_ADDR_ERRORS},
    {L7_PLATFORM_CTR_RX_IP_FORW_DATAGRAMS,  		DAPI_STATS_RECEIVED_L3_FORW_DATAGRAMS},
    {L7_PLATFORM_CTR_IP_ROUTING_DISCARDS,  			DAPI_STATS_RECEIVED_L3_DISC_ROUTED_FRAMES},
    {L7_PLATFORM_CTR_RX_IP_FRAG_FAILS,  			DAPI_STATS_RECEIVED_L3_FRAG_DISCARDS},
    {L7_PLATFORM_CTR_RECEIVED_L3_MAC_MCAST_DISCARDS,  DAPI_STATS_RECEIVED_L3_MAC_MCAST_DISCARDS},
    {L7_PLATFORM_CTR_RECEIVED_L3_ARP_TO_CPU,  		DAPI_STATS_RECEIVED_L3_ARP_TO_CPU},
    {L7_PLATFORM_CTR_RECEIVED_L3_IP_TO_CPU,  		DAPI_STATS_RECEIVED_L3_IP_TO_CPU},
    {L7_PLATFORM_CTR_TX_RX_64,  		DAPI_STATS_TRANSMITTED_AND_RECEIVED_64_BYTE_FRAMES},
    {L7_PLATFORM_CTR_TX_RX_65_127, 	    DAPI_STATS_TRANSMITTED_AND_RECEIVED_65_127_BYTE_FRAMES},
    {L7_PLATFORM_CTR_TX_RX_128_255,  	DAPI_STATS_TRANSMITTED_AND_RECEIVED_128_255_BYTE_FRAMES},
    {L7_PLATFORM_CTR_TX_RX_256_511,  	DAPI_STATS_TRANSMITTED_AND_RECEIVED_256_511_BYTE_FRAMES},
    {L7_PLATFORM_CTR_TX_RX_512_1023,  	DAPI_STATS_TRANSMITTED_AND_RECEIVED_512_1023_BYTE_FRAMES},
    {L7_PLATFORM_CTR_TX_RX_1024_1518,   DAPI_STATS_TRANSMITTED_AND_RECEIVED_1024_1518_BYTE_FRAMES},
    {L7_PLATFORM_CTR_TX_RX_1519_1522,   DAPI_STATS_TRANSMITTED_AND_RECEIVED_1519_1522_BYTE_FRAMES},
    {L7_PLATFORM_CTR_TX_RX_1523_2047,   DAPI_STATS_TRANSMITTED_AND_RECEIVED_1523_2047_BYTE_FRAMES},
    {L7_PLATFORM_CTR_TX_RX_2048_4095,   DAPI_STATS_TRANSMITTED_AND_RECEIVED_2048_4095_BYTE_FRAMES},
    {L7_PLATFORM_CTR_TX_RX_4096_9216,   DAPI_STATS_TRANSMITTED_AND_RECEIVED_4096_9216_BYTE_FRAMES},
    {L7_PLATFORM_CTR_ETHER_STATS_DROP_EVENTS, DAPI_STATS_ETHER_STATS_DROP_EVENTS},
    {L7_PLATFORM_CTR_SNMPIFOUTDISCARD_FRAMES, DAPI_STATS_SNMPIFOUTDISCARD_FRAMES},
    {L7_PLATFORM_CTR_SNMPIFINDISCARD_FRAMES,  DAPI_STATS_SNMPIFINDISCARD_FRAMES}

};
/*
**********************************************************************
*                           PRIVATE FUNCTIONS PROTOTYPES 
**********************************************************************
*/


/*
**********************************************************************
*                           API FUNCTIONS 
**********************************************************************
*/
/*********************************************************************
* @purpose  Gets a specific counter
*
* @param    intIfNum            @b{(input)} Internal Interface Number
* @param    statsMgrCounterID   @b{(input)} Statistics Manager Counter ID
* @param    counterSize         @b{(input)} Size of counter
* @param    *counterValue       @b{(output)} Pointer to the value
*
* @returns  L7_SUCCESS    if success
* @returns  L7_FAILURE    if failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dtlStatsGet(L7_uint32 intIfNum, 
                    L7_uint32 statsMgrCounterID, 
                    L7_uint32 counterSize, 
                    L7_ulong64 *counterValue)
{

  L7_RC_t rc = L7_SUCCESS, rc2;
  DAPI_INTF_STATS_t dapiCounter = 0;
  L7_ulong32 vlanId;
  L7_uint32  i;
  L7_NIM_QUERY_DATA_t intfQuery;
  L7_uint32 ifType;

  intfQuery.intIfNum = intIfNum;
  intfQuery.request  = L7_NIM_QRY_RQST_STATE;

  rc2 = nimIntfQuery (&intfQuery);
  if (rc2 != L7_SUCCESS)
  {
    return L7_FAILURE;  /* Interface does not exist */
  }

  rc2=nimGetIfType(intIfNum,&ifType);
  if (rc2 != L7_SUCCESS)
  {
    return L7_FAILURE;  /* Unable to get type of interface */
  }
  /* If the interface is not CPU interface and not attached then set stats to zero.
  */
  if(ifType != L7_IANA_OTHER_CPU)
  {
    if (intfQuery.data.state != L7_INTF_ATTACHED)
    {
      counterValue->high = 0;
      counterValue->low = 0;
      return L7_SUCCESS;
    }
  }

  for (i = 0; i < DTL_DAPI_MAP_SIZE; i++) {
	  if (dtlDapiMap[i].platform_stat == statsMgrCounterID) 
	  {
		  dapiCounter = dtlDapiMap[i].dapi_stat;
		  break;
	  }
  }

  if (i == DTL_DAPI_MAP_SIZE) 
  {
	  /* check for vlan counters */
	  if ((statsMgrCounterID & L7_PLATFORM_CTR_IN_FRAMES) == L7_PLATFORM_CTR_IN_FRAMES)
	  {
		dapiCounter = DAPI_STATS_VLAN_IN_FRAMES;
	  }
	  else if ((statsMgrCounterID & L7_PLATFORM_CTR_IN_DISCARD_FRAMES) == L7_PLATFORM_CTR_IN_DISCARD_FRAMES)
	  {
		dapiCounter = DAPI_STATS_VLAN_IN_DISCARDS;
	  }
	  else if ((statsMgrCounterID & L7_PLATFORM_CTR_OUT_FRAMES) == L7_PLATFORM_CTR_OUT_FRAMES)
	  {
		dapiCounter = DAPI_STATS_VLAN_OUT_FRAMES;
	  }
	  else
		/* Unknown counter */
		return L7_ERROR;

	  vlanId = ((statsMgrCounterID & (0xfff<<6)) >> 6);

	  rc = dtlGetVlanStats(intIfNum, dapiCounter, vlanId, counterValue);

	  return rc;
  }




  if (rc == L7_SUCCESS)
  {
    rc = dtlGetStats(intIfNum,dapiCounter,counterValue);
  }

  return rc;
}



/*
**********************************************************************
*                           PRIVATE FUNCTIONS 
**********************************************************************
*/
/*********************************************************************
* @purpose  Get the driver statistics for this interface
*
* @param    intfNUM     @b{(input)} Physical or logical interface to be configured
* @param    counterID   @b{(input)} Specific counter
* @param    *val         @b{(output)} Pointer to the value
*
* @returns  L7_SUCCESS, if counters is successfully obtained
* @returns  L7_FAILURE, otherwise
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dtlGetStats(L7_uint32         intIfNum, 
                    DAPI_INTF_STATS_t counterID, 
                    L7_ulong64       *val)
{

  DAPI_INTF_MGMT_CMD_t dapiCmd;
  DAPI_USP_t ddUsp;
  nimUSP_t usp;
  L7_ulong64    value;
  L7_RC_t rval=1;
  L7_RC_t rc=L7_FAILURE;


  /* Obtain counters from driver */
  if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  ddUsp.unit = usp.unit;
  ddUsp.slot = usp.slot;
  ddUsp.port = usp.port - 1;

  dapiCmd.cmdData.statistics.getOrSet  = DAPI_CMD_GET;
  dapiCmd.cmdData.statistics.enable    = L7_TRUE;
  dapiCmd.cmdData.statistics.counterId = counterID;
  dapiCmd.cmdData.statistics.valuePtr  = &value;

  rval = dapiCtl(&ddUsp, DAPI_CMD_INTF_STATISTICS, &dapiCmd);

  if (rval == L7_SUCCESS)
  {
    /* Obtain the requested counter */
    val->low  = value.low;
    val->high = value.high;

    rc = L7_SUCCESS;
  }

  return rc;

}


/*********************************************************************
* @purpose  Get the driver vlan statistics for this interface
*
* @param    intfNUM     @b{(input)} Physical or logical interface to be configured
* @param    counterID   @b{(input)} Specific counter
* @param    vlanID      @b{(input)} vlan id
* @param    *val        @b{(output)} Pointer to the value
*
* @returns  L7_SUCCESS, if counters is successfully obtained
* @returns  L7_FAILURE, otherwise
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dtlGetVlanStats(L7_uint32         intIfNum, 
                        DAPI_INTF_STATS_t counterID, 
                        L7_ulong32 vlanID, 
                        L7_ulong64       *val)
{
  DAPI_QVLAN_MGMT_CMD_t dapiCmd; 
  DAPI_USP_t ddUsp;
  nimUSP_t usp;
  L7_ulong64 stats;
  L7_RC_t rval=1;
  L7_RC_t rc=L7_FAILURE;

  /* Obtain counters from driver */
  if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  ddUsp.unit = usp.unit;
  ddUsp.slot = usp.slot;
  ddUsp.port = usp.port - 1;

#if 0
  if (usp.slot == L7_LIF_SLOT_NUM)
  {
    dapiLifCmd.cmdData.stats.getOrSet     = DAPI_CMD_GET;
    dapiLifCmd.cmdData.stats.statsType    = counterID;
    dapiLifCmd.cmdData.stats.lifUsp       = &ddUsp;
    dapiLifCmd.cmdData.stats.value        = &stats;

    rval = dapiCtl(&ddUsp, DAPI_CMD_LIF_BM_INTF_STATS_GET, &dapiLifCmd);

  }
  else
  {
    dapiCmd.cmdData.vlanStatsGet.getOrSet     = DAPI_CMD_GET;
    dapiCmd.cmdData.vlanStatsGet.portUsp      = &ddUsp;
    dapiCmd.cmdData.vlanStatsGet.statsType    = counterID;
    dapiCmd.cmdData.vlanStatsGet.vlanID       = vlanID;
    dapiCmd.cmdData.vlanStatsGet.statsCounter = &stats;

    rval = dapiCtl(&ddUsp, DAPI_CMD_QVLAN_VLAN_STATS_GET, &dapiCmd);
  }
#else
  /* colinw: The above code has been disabled since the driver doesn't support 
             the operation */
  dapiCmd.cmdData.vlanStatsGet.getOrSet     = DAPI_CMD_GET;
  dapiCmd.cmdData.vlanStatsGet.portUsp      = &ddUsp;
  dapiCmd.cmdData.vlanStatsGet.statsType    = counterID;
  dapiCmd.cmdData.vlanStatsGet.vlanID       = vlanID;
  dapiCmd.cmdData.vlanStatsGet.statsCounter = &stats;

  rval = dapiCtl(&ddUsp, DAPI_CMD_QVLAN_VLAN_STATS_GET, &dapiCmd);
#endif

  if (rval == L7_SUCCESS)
  {
    /* Obtain the requested counter */

    val->low  = stats.low;
    val->high = stats.high;

    rc = L7_SUCCESS;
  }

  return rc;

}

/*********************************************************************
* @purpose  Registers statistics counters with Statistics Manager
*          
* @param 		none
*	
* @returns  L7_SUCCESS if success
* @returns  L7_FAILURE if failure
*
* @notes    none
*
* @end
*
*********************************************************************/
L7_RC_t dtlStatsCreate()
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 i, j, listSize = sizeof(dtlCounters_list) / sizeof(L7_uint32);
  statsParm_entry_t dtlCtr;

  dtlCtr.cSize = C32_BITS;
  dtlCtr.cType = ExternalCtr;
  dtlCtr.isResettable = L7_TRUE;
  dtlCtr.pMutlingsParmList = L7_NULL;
  dtlCtr.pSource = (L7_VOIDFUNCPTR_t) dtlStatsCounter;

  for (j=0; j < L7_ALL_INTERFACES; j++)
  {
    dtlCtr.cKey = j;
    for (i=0; (i< listSize && rc == L7_SUCCESS); i++)
    {
      dtlCtr.cId = dtlCounters_list[i];
      rc = statsCreate(1, (pStatsParm_list_t)(&dtlCtr));

    }
  }

  return rc;

}
/*********************************************************************
* @purpose  gets or resets the dtl counters
*          
* @param 		counter      @b{(input)} Pointer to storage allocated by stats
*                                    manager to hold the results
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*
*********************************************************************/
L7_RC_t dtlStatsCounter(pCounterValue_entry_t counter)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 i;

  counter->status     = L7_SUCCESS;
  counter->cSize      = C32_BITS;
  if (counter->cMode == getMode)
  {
    switch (counter->cId)
    {
    case L7_PLATFORM_CTR_RX_DISCARDS:
      counter->cValue.low = dtlCounterValue[counter->cKey].InDiscards;
      break;
    case L7_PLATFORM_CTR_TX_DISCARDS:
      counter->cValue.low = dtlCounterValue[counter->cKey].OutDiscards;
      break;
    }
  }
  else  /*cMode==setMode i.e. reset*/
  {
    /*
    switch (counter->cId)
    {
    case L7_PLATFORM_CTR_RX_DISCARDS:
      dtlCounterValue[counter->cKey].InDiscards = 0;
      break;
    case L7_PLATFORM_CTR_TX_DISCARDS:
      dtlCounterValue[counter->cKey].OutDiscards = 0;
      break;
    } */
    for (i=0;i<L7_ALL_INTERFACES; i++)
    {
      dtlCounterValue[i].InDiscards = 0;
      dtlCounterValue[i].OutDiscards = 0;

    }

  }

  return rc;
}

/*********************************************************************
* @purpose  increments dtl counters
*          
* @param    intIfNum    @b{(input)} Internal Interface Number
* @param 		counter      @b{(input)} type of counter
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*
* @end
*
*********************************************************************/
L7_RC_t dtlStatsIncrement(L7_uint32 intIfNum, L7_uint32 counter)
{
  switch (counter)
  {
  case L7_PLATFORM_CTR_RX_DISCARDS:
    dtlCounterValue[intIfNum].InDiscards++;
    break;
  case L7_PLATFORM_CTR_TX_DISCARDS:
    dtlCounterValue[intIfNum].OutDiscards++;
    break;
  default:
    break;

  }
  return L7_SUCCESS;
}



#endif /*DTLCTRL_COMPONENT_STATS*/

