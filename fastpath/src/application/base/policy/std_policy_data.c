/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename src\policy\std_data.c
*
* @purpose Contains policy component specific global & static data     
*
* @component 
*
* @comments 
*
* @create 05/10/01
*
* @author anayar
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/



#include "l7_common.h"
#include "statsapi.h"
#include "platform_counters.h"
#include "std_policy_api.h"

mutling_id_t mutlingRxTotalDiscards[STD_POLICY_MUTLING_COUNT] =
{
  {
    cPLUS,
    L7_NULL,
    L7_PLATFORM_CTR_UNACCEPTABLE_FRAME_TYPE_DISCARDS
  },
#ifdef NOT_SUPPORTED
  /* The following counters were marked as NOT_SUPPORTED, as none of the
   * hardware platforms actually support them. Do not include the unsupported
   * counters here, else the counter always returns failure.
   * !!!! REMEMBER TO CHANGE **STD_POLICY_MUTLING_COUNT** value !!!!
   */
  { 
    cPLUS,
    L7_NULL,
    L7_PLATFORM_CTR_LOCAL_TRAFFIC_DISCARDS
  },
  { 
    cPLUS,
    L7_NULL,
    L7_PLATFORM_CTR_INGRESS_FILTER_DISCARDS
  },
  { 
    cPLUS,
    L7_NULL,
    L7_PLATFORM_CTR_INGRESS_VLAN_VIABLE_DISCARDS
  },
  { 
    cPLUS,
    L7_NULL,
    L7_PLATFORM_CTR_MULTICAST_TREE_VIABLE_DISCARDS
  },
  { 
    cPLUS,
    L7_NULL,
    L7_PLATFORM_CTR_RESERVED_ADDRESS_DISCARDS
  },
  { 
    cPLUS,
    L7_NULL,
    L7_PLATFORM_CTR_BCAST_STORM_RECOVERY_DISCARDS
  },
  { 
    cPLUS,
    L7_NULL,
    L7_PLATFORM_CTR_CFI_DISCARDS
  },
  { 
    cPLUS,
    L7_NULL,
    L7_PLATFORM_CTR_UPSTREAM_THRESHOLD_DISCARDS
  }
#endif
};

mutling_parm_list_t mutantRxTotalDiscards =
{
  sizeof( mutlingRxTotalDiscards ) / sizeof( mutling_id_t ),
  mutlingRxTotalDiscards
};

statsParm_entry_t stdPolicyMutants[STD_POLICY_MUTANT_COUNT] =
{
  {  /* counter L7_PLATFORM_CTR_RX_TOTAL_DISCARDS */
    L7_FAILURE,
    MutantCtr,
    L7_NULL,
    L7_PLATFORM_CTR_RX_TOTAL_DISCARDS,
    C32_BITS,
    L7_FALSE,
    L7_NULL,
    &mutantRxTotalDiscards
  }
};

