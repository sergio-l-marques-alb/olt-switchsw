/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename src\application\unitmgr\usmdb\usmdb_counters.c
*
* @purpose Support for counters, including user counters
*
* @component unitmgr
*
* @comments none
*
* @create 05/12/2001
*
* @author gpaussa
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/



#include "l7_common.h"
#include "usmdb_common.h"
#include "usmdb_dot1s_api.h"
#include "usmdb_dot3ad_api.h"
#include "usmdb_nim_api.h"

#include "statsapi.h"
#include "dot3ad_api.h"
#include "default_cnfgr.h"
#include "usmdb_counters_api.h"
#include "dot1q_api.h"


/* local function prototypes */
static L7_RC_t usmDbCounterIdGet(L7_uint32 unitIndex, L7_uint32 uid, L7_uint32 intf, L7_uint32 *pCid);


/* ------------------------------------------------------
 * Definitions for all Fast Ethernet port USMDB counters
 * ------------------------------------------------------
 */

/* USMDB counter ID definitions */

typedef enum
{
  L7_CTR_PHYS_RX_TOTAL_FRAMES = (L7_USMDB_COMPONENT_ID << 16) | 0x0100,
  L7_CTR_PHYS_RX_TOTAL_ERROR_FRAMES,
  L7_CTR_PHYS_TX_TOTAL_FRAMES,
  L7_CTR_PHYS_TX_TOTAL_ERROR_FRAMES,
  L7_CTR_PHYS_TX_TOTAL_COLLISION_FRAMES,
  L7_CTR_PHYS_RX_CRC_ERRORS,
  L7_CTR_PHYS_RX_TOTAL_MAC_ERROR_FRAMES

} usmdbPhysPortUserCtrId_t;


/* Mutant counter definitions */

static mutling_id_t mutlingRxTotalFrames[] =
{
  {cPLUS, L7_NULL, L7_PLATFORM_CTR_RX_UCAST_FRAMES},
  {cPLUS, L7_NULL, L7_PLATFORM_CTR_RX_MCAST_FRAMES},
  {cPLUS, L7_NULL, L7_PLATFORM_CTR_RX_BCAST_FRAMES}
};
static mutling_parm_list_t mutantRxTotalFrames =
{
  sizeof(mutlingRxTotalFrames) / sizeof(mutling_id_t),
  mutlingRxTotalFrames
};

static mutling_id_t mutlingRxTotalErrorFrames[] =
{
  {cPLUS, L7_NULL, L7_PLATFORM_CTR_RX_ERROR_OVERSIZE},
  {cPLUS, L7_NULL, L7_PLATFORM_CTR_RX_ERROR_UNDERSIZE},
  {cPLUS, L7_NULL, L7_PLATFORM_CTR_RX_GOOD_UNDERSIZE},
  {cPLUS, L7_NULL, L7_PLATFORM_CTR_RX_ALIGN_ERRORS},
  {cPLUS, L7_NULL, L7_PLATFORM_CTR_RX_FCS_ERRORS},
  {cPLUS, L7_NULL, L7_PLATFORM_CTR_RX_OVERRUNS}
};
static mutling_parm_list_t mutantRxTotalErrorFrames =
{
  sizeof(mutlingRxTotalErrorFrames) / sizeof(mutling_id_t),
  mutlingRxTotalErrorFrames
};


static mutling_id_t mutlingTxTotalFrames[] =
{
  {cPLUS, L7_NULL, L7_PLATFORM_CTR_TX_UCAST_FRAMES},
  {cPLUS, L7_NULL, L7_PLATFORM_CTR_TX_MCAST_FRAMES},
  {cPLUS, L7_NULL, L7_PLATFORM_CTR_TX_BCAST_FRAMES}
};
static mutling_parm_list_t mutantTxTotalFrames =
{
  sizeof(mutlingTxTotalFrames) / sizeof(mutling_id_t),
  mutlingTxTotalFrames
};

static mutling_id_t mutlingTxTotalErrorFrames[] =
{
  {cPLUS, L7_NULL, L7_PLATFORM_CTR_TX_FCS_ERRORS},
  {cPLUS, L7_NULL, L7_PLATFORM_CTR_TX_UNDERRUN_ERRORS}
};
static mutling_parm_list_t mutantTxTotalErrorFrames =
{
  sizeof(mutlingTxTotalErrorFrames) / sizeof(mutling_id_t),
  mutlingTxTotalErrorFrames
};

static mutling_id_t mutlingTxTotalCollisionFrames[] =
{
  {cPLUS, L7_NULL, L7_PLATFORM_CTR_TX_ONE_COLLISION},
  {cPLUS, L7_NULL, L7_PLATFORM_CTR_TX_MULTIPLE_COLLISION},
  {cPLUS, L7_NULL, L7_PLATFORM_CTR_TX_LATE_COLLISION},
  {cPLUS, L7_NULL, L7_PLATFORM_CTR_TX_EXCESSIVE_COLLISION}
};
static mutling_parm_list_t mutantTxTotalCollisionFrames =
{
  sizeof(mutlingTxTotalCollisionFrames) / sizeof(mutling_id_t),
  mutlingTxTotalCollisionFrames
};

static mutling_id_t mutlingRxCRCErrors[] = 
{
  {cPLUS, L7_NULL, L7_PLATFORM_CTR_RX_ALIGN_ERRORS},
  {cPLUS, L7_NULL, L7_PLATFORM_CTR_RX_FCS_ERRORS}
};
static mutling_parm_list_t mutantRxCRCErrors = 
{  
  sizeof(mutlingRxCRCErrors) / sizeof(mutling_id_t),
  mutlingRxCRCErrors
};

static mutling_id_t mutlingRxTotalMacErrorFrames[] =
{
  {cPLUS, L7_NULL, L7_PLATFORM_CTR_RX_ERROR_OVERSIZE},
  {cPLUS, L7_NULL, L7_PLATFORM_CTR_RX_ERROR_UNDERSIZE},
  {cPLUS, L7_NULL, L7_PLATFORM_CTR_RX_GOOD_UNDERSIZE},
  {cPLUS, L7_NULL, L7_PLATFORM_CTR_RX_ALIGN_ERRORS},
  {cPLUS, L7_NULL, L7_PLATFORM_CTR_RX_FCS_ERRORS},
  {cPLUS, L7_NULL, L7_PLATFORM_CTR_RX_OVERRUNS}
};
static mutling_parm_list_t mutantRxTotalMacErrorFrames = 
{  
  sizeof(mutlingRxTotalMacErrorFrames) / sizeof( mutling_id_t ),
  mutlingRxTotalMacErrorFrames
};


/* USMDB Fast Ethernet port user statistics */

static statsParm_entry_t userStatsFastEthernet[] =
{
  {                                                                            /* counter L7_CTR_RX_TOTAL_FRAMES */
    L7_FAILURE,
    MutantCtr,
    L7_NULL,
    L7_CTR_PHYS_RX_TOTAL_FRAMES,
    C64_BITS,
    L7_FALSE,
    L7_NULL,
    &mutantRxTotalFrames
  },
  {                                                                            /* counter L7_CTR_RX_TOTAL_ERROR_FRAMES */
    L7_FAILURE,
    MutantCtr,
    L7_NULL,
    L7_CTR_PHYS_RX_TOTAL_ERROR_FRAMES,
    C32_BITS,
    L7_FALSE,
    L7_NULL,
    &mutantRxTotalErrorFrames
  },
  {                                                                            /* counter L7_CTR_TX_TOTAL_FRAMES */
    L7_FAILURE,
    MutantCtr,
    L7_NULL,
    L7_CTR_PHYS_TX_TOTAL_FRAMES,
    C64_BITS,
    L7_FALSE,
    L7_NULL,
    &mutantTxTotalFrames
  },
  {                                                                            /* counter L7_CTR_TX_TOTAL_ERROR_FRAMES */
    L7_FAILURE,
    MutantCtr,
    L7_NULL,
    L7_CTR_PHYS_TX_TOTAL_ERROR_FRAMES,
    C32_BITS,
    L7_FALSE,
    L7_NULL,
    &mutantTxTotalErrorFrames
  },
  {                                                                            /* counter L7_CTR_TX_TOTAL_COLLISION_FRAMES */
    L7_FAILURE,
    MutantCtr,
    L7_NULL,
    L7_CTR_PHYS_TX_TOTAL_COLLISION_FRAMES,
    C32_BITS,
    L7_FALSE,
    L7_NULL,
    &mutantTxTotalCollisionFrames
  },
  {                                                                            /* counter L7_CTR_RX_CRC_ERRORS */
    L7_FAILURE,
    MutantCtr,
    L7_NULL,
    L7_CTR_PHYS_RX_CRC_ERRORS,
    C32_BITS,
    L7_FALSE,
    L7_NULL,
    &mutantRxCRCErrors
  },
  {                                                                            /* counter L7_CTR_RX_TOTAL_MAC_ERROR_FRAMES */
    L7_FAILURE,
    MutantCtr,
    L7_NULL,
    L7_CTR_PHYS_RX_TOTAL_MAC_ERROR_FRAMES,
    C32_BITS,
    L7_FALSE,
    L7_NULL,
    &mutantRxTotalMacErrorFrames
  },
};


/* USMDB CPU port user statistics */

static statsParm_entry_t userStatsCPU[] =
{
  {                                                                            /* counter L7_CTR_RX_TOTAL_FRAMES */
    L7_FAILURE,
    MutantCtr,
    L7_NULL,
    L7_CTR_PHYS_RX_TOTAL_FRAMES,
    C32_BITS,
    L7_FALSE,
    L7_NULL,
    &mutantRxTotalFrames
  },
  {                                                                            /* counter L7_CTR_RX_TOTAL_MAC_ERROR_FRAMES */
    L7_FAILURE,
    MutantCtr,
    L7_NULL,
    L7_CTR_PHYS_RX_TOTAL_MAC_ERROR_FRAMES,
    C32_BITS,
    L7_FALSE,
    L7_NULL,
    &mutantRxTotalErrorFrames
  },
  {                                                                            /* counter L7_CTR_TX_TOTAL_FRAMES */
    L7_FAILURE,
    MutantCtr,
    L7_NULL,
    L7_CTR_PHYS_TX_TOTAL_FRAMES,
    C32_BITS,
    L7_FALSE,
    L7_NULL,
    &mutantTxTotalFrames
  },
  {                                                                            /* counter L7_CTR_TX_TOTAL_ERROR_FRAMES */
    L7_FAILURE,
    MutantCtr,
    L7_NULL,
    L7_CTR_PHYS_TX_TOTAL_ERROR_FRAMES,
    C32_BITS,
    L7_FALSE,
    L7_NULL,
    &mutantTxTotalErrorFrames
  },
};



/* --------------------------------------------
 * Definitions for all LAG port USMDB counters
 * --------------------------------------------
 */

/* USMDB counter ID definitions */

typedef enum
{
  L7_CTR_LAG_RX_TOTAL_FRAMES = (L7_USMDB_COMPONENT_ID << 16) | 0x0200,
  L7_CTR_LAG_RX_TOTAL_ERROR_FRAMES,
  L7_CTR_LAG_TX_TOTAL_FRAMES,
  L7_CTR_LAG_TX_TOTAL_ERROR_FRAMES,
  L7_CTR_LAG_TX_TOTAL_COLLISION_FRAMES,
  L7_CTR_LAG_RX_CRC_ERRORS,
  L7_CTR_LAG_RX_TOTAL_MAC_ERROR_FRAMES

} usmdbLagPortUserCtrId_t;


/* Mutant counter definitions */

static mutling_id_t mutlingLagRxTotalFrames[] =
{
  {cPLUS, L7_NULL, L7_PLATFORM_CTR_LAG_RX_UCAST_FRAMES},
  {cPLUS, L7_NULL, L7_PLATFORM_CTR_LAG_RX_MCAST_FRAMES},
  {cPLUS, L7_NULL, L7_PLATFORM_CTR_LAG_RX_BCAST_FRAMES},
};

static mutling_parm_list_t mutantLagRxTotalFrames =
{
  sizeof(mutlingLagRxTotalFrames) / sizeof(mutling_id_t),
  mutlingLagRxTotalFrames
};

static mutling_id_t mutlingLagRxTotalErrorFrames[] =
{
  {cPLUS, L7_NULL, L7_PLATFORM_CTR_LAG_RX_ERROR_OVERSIZE},
  {cPLUS, L7_NULL, L7_PLATFORM_CTR_LAG_RX_ERROR_UNDERSIZE},
  {cPLUS, L7_NULL, L7_PLATFORM_CTR_LAG_RX_GOOD_UNDERSIZE},
  {cPLUS, L7_NULL, L7_PLATFORM_CTR_LAG_RX_ALIGN_ERRORS},
  {cPLUS, L7_NULL, L7_PLATFORM_CTR_LAG_RX_FCS_ERRORS},
  {cPLUS, L7_NULL, L7_PLATFORM_CTR_LAG_RX_OVERRUNS}
};
static mutling_parm_list_t mutantLagRxTotalErrorFrames =
{
  sizeof(mutlingLagRxTotalErrorFrames) / sizeof(mutling_id_t),
  mutlingLagRxTotalErrorFrames
};

static mutling_id_t mutlingLagTxTotalFrames[] =
{
  {cPLUS, L7_NULL, L7_PLATFORM_CTR_LAG_TX_UCAST_FRAMES},
  {cPLUS, L7_NULL, L7_PLATFORM_CTR_LAG_TX_MCAST_FRAMES},
  {cPLUS, L7_NULL, L7_PLATFORM_CTR_LAG_TX_BCAST_FRAMES}
};
static mutling_parm_list_t mutantLagTxTotalFrames =
{
  sizeof(mutlingLagTxTotalFrames) / sizeof(mutling_id_t),
  mutlingLagTxTotalFrames
};

static mutling_id_t mutlingLagTxTotalErrorFrames[] =
{
  {cPLUS, L7_NULL, L7_PLATFORM_CTR_LAG_TX_FCS_ERRORS},
  {cPLUS, L7_NULL, L7_PLATFORM_CTR_LAG_TX_OVERSIZED},
  {cPLUS, L7_NULL, L7_PLATFORM_CTR_LAG_TX_UNDERRUN_ERRORS}
};
static mutling_parm_list_t mutantLagTxTotalErrorFrames =
{
  sizeof(mutlingLagTxTotalErrorFrames) / sizeof(mutling_id_t),
  mutlingLagTxTotalErrorFrames
};

static mutling_id_t mutlingLagTxTotalCollisionFrames[] =
{
  {cPLUS, L7_NULL, L7_PLATFORM_CTR_LAG_TX_ONE_COLLISION},
  {cPLUS, L7_NULL, L7_PLATFORM_CTR_LAG_TX_MULTIPLE_COLLISION},
  {cPLUS, L7_NULL, L7_PLATFORM_CTR_LAG_TX_LATE_COLLISION},
  {cPLUS, L7_NULL, L7_PLATFORM_CTR_LAG_TX_EXCESSIVE_COLLISION}
};
static mutling_parm_list_t mutantLagTxTotalCollisionFrames =
{
  sizeof(mutlingLagTxTotalCollisionFrames) / sizeof(mutling_id_t),
  mutlingLagTxTotalCollisionFrames
};

static mutling_id_t mutlingLagRxCRCErrors[] = 
{
  {cPLUS, L7_NULL, L7_PLATFORM_CTR_LAG_RX_ALIGN_ERRORS},
  {cPLUS,L7_NULL, L7_PLATFORM_CTR_LAG_RX_FCS_ERRORS}
};
static mutling_parm_list_t mutantLagRxCRCErrors = 
{  
  sizeof(mutlingLagRxCRCErrors) / sizeof(mutling_id_t),
  mutlingLagRxCRCErrors
};

static mutling_id_t mutlingLagRxTotalMacErrorFrames[] =
{
  {cPLUS, L7_NULL, L7_PLATFORM_CTR_LAG_RX_ERROR_OVERSIZE},
  {cPLUS, L7_NULL, L7_PLATFORM_CTR_LAG_RX_ERROR_UNDERSIZE},
  {cPLUS, L7_NULL, L7_PLATFORM_CTR_LAG_RX_GOOD_UNDERSIZE},
  {cPLUS, L7_NULL, L7_PLATFORM_CTR_LAG_RX_ALIGN_ERRORS},
  {cPLUS, L7_NULL, L7_PLATFORM_CTR_LAG_RX_FCS_ERRORS},
  {cPLUS, L7_NULL, L7_PLATFORM_CTR_LAG_RX_OVERRUNS}
};
static mutling_parm_list_t mutantLagRxTotalMacErrorFrames = 
{  
  sizeof(mutlingLagRxTotalMacErrorFrames) / sizeof(mutling_id_t),
  mutlingLagRxTotalMacErrorFrames
};


/* USMDB Fast Ethernet LAG user statistics */

static statsParm_entry_t userStatsLagFastEthernet[] =
{
  {                                                                            /* counter L7_CTR_RX_TOTAL_FRAMES */
    L7_FAILURE,
    MutantCtr,
    L7_NULL,
    L7_CTR_LAG_RX_TOTAL_FRAMES,
    C32_BITS,
    L7_FALSE,
    L7_NULL,
    &mutantLagRxTotalFrames
  },
  {                                                                            /* counter L7_CTR_RX_TOTAL_ERROR_FRAMES */
    L7_FAILURE,
    MutantCtr,
    L7_NULL,
    L7_CTR_LAG_RX_TOTAL_ERROR_FRAMES,
    C32_BITS,
    L7_FALSE,
    L7_NULL,
    &mutantLagRxTotalErrorFrames
  },
  {                                                                            /* counter L7_CTR_TX_TOTAL_FRAMES */
    L7_FAILURE,
    MutantCtr,
    L7_NULL,
    L7_CTR_LAG_TX_TOTAL_FRAMES,
    C32_BITS,
    L7_FALSE,
    L7_NULL,
    &mutantLagTxTotalFrames
  },
  {                                                                            /* counter L7_CTR_TX_TOTAL_ERROR_FRAMES */
    L7_FAILURE,
    MutantCtr,
    L7_NULL,
    L7_CTR_LAG_TX_TOTAL_ERROR_FRAMES,
    C32_BITS,
    L7_FALSE,
    L7_NULL,
    &mutantLagTxTotalErrorFrames
  },
  {                                                                            /* counter L7_CTR_TX_TOTAL_COLLISION_FRAMES */
    L7_FAILURE,
    MutantCtr,
    L7_NULL,
    L7_CTR_LAG_TX_TOTAL_COLLISION_FRAMES,
    C32_BITS,
    L7_FALSE,
    L7_NULL,
    &mutantLagTxTotalCollisionFrames
  },
  {                                                                            /* counter L7_CTR_RX_CRC_ERRORS */
    L7_FAILURE,
    MutantCtr,
    L7_NULL,
    L7_CTR_LAG_RX_CRC_ERRORS,
    C32_BITS,
    L7_FALSE,
    L7_NULL,
    &mutantLagRxCRCErrors
  },
  {                                                                            /* counter L7_CTR_RX_TOTAL_MAC_ERROR_FRAMES */
    L7_FAILURE,
    MutantCtr,
    L7_NULL,
    L7_CTR_LAG_RX_TOTAL_MAC_ERROR_FRAMES,
    C32_BITS,
    L7_FALSE,
    L7_NULL,
    &mutantLagRxTotalMacErrorFrames
  },
};


/* mutant counter definition for VLAN Ingress Statistics */

static mutling_id_t mutlingDot1qRxTotalNotForwardedFrames[] =
{
  {cPLUS, L7_NULL, L7_PLATFORM_CTR_LOCAL_TRAFFIC_DISCARDS},
  {cPLUS, L7_NULL, L7_PLATFORM_CTR_RX_PAUSE_FRAMES},
  {cPLUS, L7_NULL, L7_PLATFORM_CTR_UNACCEPTABLE_FRAME_TYPE_DISCARDS},
  {cPLUS, L7_NULL, L7_PLATFORM_CTR_INGRESS_FILTER_DISCARDS},
  {cPLUS, L7_NULL, L7_PLATFORM_CTR_INGRESS_VLAN_VIABLE_DISCARDS},
  {cPLUS, L7_NULL, L7_PLATFORM_CTR_MULTICAST_TREE_VIABLE_DISCARDS},
  {cPLUS, L7_NULL, L7_PLATFORM_CTR_RESERVED_ADDRESS_DISCARDS},
  {cPLUS, L7_NULL, L7_PLATFORM_CTR_BCAST_STORM_RECOVERY_DISCARDS},
  {cPLUS, L7_NULL, L7_PLATFORM_CTR_CFI_DISCARDS},
  {cPLUS, L7_NULL, L7_PLATFORM_CTR_UPSTREAM_THRESHOLD_DISCARDS}
};
static mutling_parm_list_t mutantDot1qRxTotalNotForwardedFrames =
{
  sizeof(mutlingDot1qRxTotalNotForwardedFrames) / sizeof(mutling_id_t),
  mutlingDot1qRxTotalNotForwardedFrames
};


/* USMDB Fast Ethernet Dot1q user statistics */

static statsParm_entry_t userStatsDot1qFastEthernet[] =
{
  {                                                                            /* counter L7_CTR_DOT1Q_RX_TOTAL_NOT_FORWARDED_FRAMES */
    L7_FAILURE,
    MutantCtr,
    L7_NULL,
    L7_CTR_DOT1Q_RX_TOTAL_NOT_FORWARDED_FRAMES,
    C32_BITS,
    L7_FALSE,
    L7_NULL,
    &mutantDot1qRxTotalNotForwardedFrames
  }
};



/* ----------------------------------------------------------------------------
 * The following tables are used for port-based counters (both base and user)
 * where a distinction needs to be made between a physical port and a LAG
 * port, since the intrinsic counter IDs are different.  These tables are 
 * used to translate the general USMDB counter identifier into the appropriate
 * Stats Mgr counter ID.
 *
 * NOTE: The number of entries in each table must be the same as the number
 *       of elements defined in the usmdbCounterTableIndex_t structure
 * ----------------------------------------------------------------------------
 */

/* Physical port counter ID table (entries MUST MATCH usmdbPortCounterIndex_t) */
static L7_uint32 usmdbPhysPortCtrIdTable[] = 
{
  0,                                                                           /* don't use entry 0 for a counter ID */
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
  L7_PLATFORM_CTR_SNMPIFINDISCARD_FRAMES,

  L7_CTR_PHYS_RX_TOTAL_FRAMES,
  L7_CTR_PHYS_RX_TOTAL_ERROR_FRAMES,
  L7_CTR_PHYS_TX_TOTAL_FRAMES,
  L7_CTR_PHYS_TX_TOTAL_ERROR_FRAMES,
  L7_CTR_PHYS_TX_TOTAL_COLLISION_FRAMES,
  L7_CTR_PHYS_RX_CRC_ERRORS,
  L7_CTR_PHYS_RX_TOTAL_MAC_ERROR_FRAMES

};


/* LAG port counter ID table (entries MUST MATCH usmdbPortCounterIndex_t) */
static L7_uint32 usmdbLagPortCtrIdTable[] =
{
  0,                                                                           /* don't use entry 0 for a counter ID */
  L7_PLATFORM_CTR_LAG_RX_TOTAL_BYTES,    
  L7_PLATFORM_CTR_LAG_RX_64,
  L7_PLATFORM_CTR_LAG_RX_65_127,
  L7_PLATFORM_CTR_LAG_RX_128_255,        
  L7_PLATFORM_CTR_LAG_RX_256_511,        
  L7_PLATFORM_CTR_LAG_RX_512_1023,       
  L7_PLATFORM_CTR_LAG_RX_1024_1518,      
  L7_PLATFORM_CTR_LAG_RX_1519_1530,      
  L7_PLATFORM_CTR_LAG_RX_GOOD_OVERSIZE,  
  L7_PLATFORM_CTR_LAG_RX_ERROR_OVERSIZE, 
  L7_PLATFORM_CTR_LAG_RX_GOOD_UNDERSIZE, 
  L7_PLATFORM_CTR_LAG_RX_ERROR_UNDERSIZE,
  L7_PLATFORM_CTR_LAG_RX_UCAST_FRAMES,   
  L7_PLATFORM_CTR_LAG_RX_MCAST_FRAMES,   
  L7_PLATFORM_CTR_LAG_RX_BCAST_FRAMES,   
  L7_PLATFORM_CTR_LAG_RX_ALIGN_ERRORS,   
  L7_PLATFORM_CTR_LAG_RX_FCS_ERRORS,     
  L7_PLATFORM_CTR_LAG_RX_OVERRUNS,       
  L7_PLATFORM_CTR_LAG_RX_FRAME_TOO_LONG,       

  L7_PLATFORM_CTR_LAG_TX_TOTAL_BYTES,        
  L7_PLATFORM_CTR_LAG_TX_64,                 
  L7_PLATFORM_CTR_LAG_TX_65_127,             
  L7_PLATFORM_CTR_LAG_TX_128_255,            
  L7_PLATFORM_CTR_LAG_TX_256_511,            
  L7_PLATFORM_CTR_LAG_TX_512_1023,           
  L7_PLATFORM_CTR_LAG_TX_1024_1518,          
  L7_PLATFORM_CTR_LAG_TX_1519_1530,          
  L7_PLATFORM_CTR_LAG_TX_UCAST_FRAMES,       
  L7_PLATFORM_CTR_LAG_TX_MCAST_FRAMES,       
  L7_PLATFORM_CTR_LAG_TX_BCAST_FRAMES,       
  L7_PLATFORM_CTR_LAG_TX_FCS_ERRORS,         
  L7_PLATFORM_CTR_LAG_TX_OVERSIZED,          
  L7_PLATFORM_CTR_LAG_TX_UNDERRUN_ERRORS,    
  L7_PLATFORM_CTR_LAG_TX_ONE_COLLISION,      
  L7_PLATFORM_CTR_LAG_TX_MULTIPLE_COLLISION, 
  L7_PLATFORM_CTR_LAG_TX_EXCESSIVE_COLLISION,
  L7_PLATFORM_CTR_LAG_TX_EXCESSIVE_COLLISION,


  L7_PLATFORM_CTR_LAG_TX_RX_64,
  L7_PLATFORM_CTR_LAG_TX_RX_65_127,
  L7_PLATFORM_CTR_LAG_TX_RX_128_255,
  L7_PLATFORM_CTR_LAG_TX_RX_256_511,
  L7_PLATFORM_CTR_LAG_TX_RX_512_1023,
  L7_PLATFORM_CTR_LAG_TX_RX_1024_1518,
  L7_PLATFORM_CTR_LAG_TX_RX_1519_1522,
  L7_PLATFORM_CTR_LAG_TX_RX_1523_2047,
  L7_PLATFORM_CTR_LAG_TX_RX_2048_4095,
  L7_PLATFORM_CTR_LAG_TX_RX_4096_9216,
  L7_PLATFORM_CTR_LAG_ETHER_STATS_DROP_EVENTS,
  L7_PLATFORM_CTR_LAG_SNMPIFOUTDISCARD_FRAMES,
  L7_PLATFORM_CTR_LAG_SNMPIFINDISCARD_FRAMES,

  L7_CTR_LAG_RX_TOTAL_FRAMES,
  L7_CTR_LAG_RX_TOTAL_ERROR_FRAMES,
  L7_CTR_LAG_TX_TOTAL_FRAMES,
  L7_CTR_LAG_TX_TOTAL_ERROR_FRAMES,
  L7_CTR_LAG_TX_TOTAL_COLLISION_FRAMES,
  L7_CTR_LAG_RX_CRC_ERRORS,
  L7_CTR_LAG_RX_TOTAL_MAC_ERROR_FRAMES

};


/*********************************************************************
*
* @purpose Creates the set of user statistics for the specified
*          interface category.
*
* @param unitIndex                Unit Index
* @param usmdbUserStatsCat_t cat  USMDB user statistics category specifier
* @param L7_uint32 intf           internal interface number 
*
* @returns L7_SUCCESS, if counter set was successfully created
* @returns L7_FAILURE, if function failed
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbUserStatsCreate(L7_uint32 unitIndex, usmdbUserStatsCat_t cat, L7_uint32 intf)
{
  L7_uint32 userListSize, i, j;
  pStatsParm_list_t pUserStatsParmList, pTmpS;
  pMutling_id_t pTmpM;                                                     

  switch (cat)
  {
  case L7_USMDB_USER_STAT_CPU:
    userListSize = sizeof(userStatsCPU) / sizeof(statsParm_entry_t);
    pUserStatsParmList = userStatsCPU;
    break;

  case L7_USMDB_USER_STAT_FAST_ENET:
    userListSize = sizeof(userStatsFastEthernet) / sizeof(statsParm_entry_t);
    pUserStatsParmList = userStatsFastEthernet;
    break;

  case L7_USMDB_USER_STAT_LAG:
    userListSize = sizeof(userStatsLagFastEthernet) / sizeof(statsParm_entry_t);
    pUserStatsParmList = userStatsLagFastEthernet;
    break;   

  case L7_USMDB_USER_STAT_DOT1Q:
    userListSize = sizeof(userStatsDot1qFastEthernet) / sizeof(statsParm_entry_t);
    pUserStatsParmList = userStatsDot1qFastEthernet;
    break;   

  default:
    return L7_FAILURE;
  }

  pTmpS = pUserStatsParmList;

  for (i = 0; i < userListSize; i++)
  {
    pTmpS->cKey = intf;

    if (pTmpS->pMutlingsParmList != L7_NULL)
    {
      pTmpM = pTmpS->pMutlingsParmList->pMutlingParms;                                                     

      for (j = 0; j < pTmpS->pMutlingsParmList->listSize; j++)
      {
        pTmpM->cKey = intf;                                     
        pTmpM++;                                                    
      }                                                              
    }
    pTmpS++;                     
  }                               

  return statsCreate(userListSize, pUserStatsParmList);
}

/*********************************************************************
*
* @purpose Deletes the set of user statistics for the specified
*          interface category.
*
* @param unitIndex                Unit Index
* @param usmdbUserStatsCat_t cat  USMDB user statistics category specifier
* @param L7_uint32 intf           internal interface number 
*
* @returns L7_SUCCESS, if counter set was successfully created
* @returns L7_FAILURE, if function failed
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbUserStatsDelete(L7_uint32 unitIndex, usmdbUserStatsCat_t cat, L7_uint32 intf)
{
  L7_uint32 userListSize, i, j;
  pStatsParm_list_t pUserStatsParmList, pTmpS;
  pMutling_id_t pTmpM;                                                     

  switch (cat)
  {
  case L7_USMDB_USER_STAT_CPU:
    userListSize = sizeof(userStatsCPU) / sizeof(statsParm_entry_t);
    pUserStatsParmList = userStatsCPU;
    break;

  case L7_USMDB_USER_STAT_FAST_ENET:
    userListSize = sizeof(userStatsFastEthernet) / sizeof(statsParm_entry_t);
    pUserStatsParmList = userStatsFastEthernet;
    break;

  case L7_USMDB_USER_STAT_LAG:
    userListSize = sizeof(userStatsLagFastEthernet) / sizeof(statsParm_entry_t);
    pUserStatsParmList = userStatsLagFastEthernet;
    break;   

  case L7_USMDB_USER_STAT_DOT1Q:
    userListSize = sizeof(userStatsDot1qFastEthernet) / sizeof(statsParm_entry_t);
    pUserStatsParmList = userStatsDot1qFastEthernet;
    break;   

  default:
    return L7_FAILURE;
  }

  pTmpS = pUserStatsParmList;

  for (i = 0; i < userListSize; i++)
  {
    pTmpS->cKey = intf;

    if (pTmpS->pMutlingsParmList != L7_NULL)
    {
      pTmpM = pTmpS->pMutlingsParmList->pMutlingParms;                                                     

      for (j = 0; j < pTmpS->pMutlingsParmList->listSize; j++)
      {
        pTmpM->cKey = intf;                                     
        pTmpM++;                                                    
      }                                                              
    }
    pTmpS++;                     
  }                               

  return (statsDelete(userListSize, pUserStatsParmList));
}

/*********************************************************************
*
* @purpose Get a single statistic.
*
* @param L7_uint32 unitIndex Unit Index
* @param L7_uint32 uid   USMDB counter identifier for this statistic
* @param L7_uint32 intf  internal interface number for per-interface
*                        statistic
* @param L7_uint32 *val  pointer to output location
*                        (@b{Output:} returned value of statistic)
*
* @returns L7_SUCCESS, if counter was successfully retrieved
* @returns L7_ERROR, if requested counter does not exist 
* @returns L7_FAILURE, if function failed (output not valid)
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbStatGet(L7_uint32 UnitIndex, L7_uint32 uid, L7_uint32 intf, L7_uint32 *val)
{

  L7_RC_t rc;
  counterValue_entry_t CVE;
  pCounterValue_list_t pCVL = &CVE;
  L7_uint32 cid;

  if ((rc = usmDbCounterIdGet(UnitIndex, uid, intf, &cid)) != L7_SUCCESS)
    return rc;

  /* Unsupported counters have cid equal to 0.
  */
  if (cid == 0) 
  {
	  return L7_ERROR;
  }

  pCVL -> cId   = cid;
  pCVL -> raw   = L7_FALSE;
  pCVL -> cKey  = intf;
  pCVL -> cValue.low   = 0;
  pCVL -> cValue.high  = 0;

  rc = statsGet(1, pCVL);
  *val = pCVL -> cValue.low;
  return rc;
}   


/*********************************************************************
*
* @purpose Reset a single statistic.
*
* @param    unitIndex   Unit Index
* @param L7_uint32 uid   USMDB counter identifier for this statistic
* @param L7_uint32 intf  internal interface number for per-interface
*                        statistic
*
* @returns L7_SUCCESS, if counter was successfully reset
* @returns L7_ERROR, if requested counter does not exist or not resettable
* @returns L7_FAILURE, if function failed
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbStatReset(L7_uint32 unitIndex, L7_uint32 uid, L7_uint32 intf)
{
  L7_RC_t rc;
  counterValue_entry_t CVE;
  pCounterValue_list_t pCVL = &CVE;
  L7_uint32 cid;

  if ((rc = usmDbCounterIdGet(unitIndex, uid, intf, &cid)) != L7_SUCCESS)
    return rc;

  pCVL -> cId   = cid;
  pCVL -> raw   = L7_FALSE;
  pCVL -> cKey  = intf;

  rc = statsReset(1, pCVL);


  return rc;
}


/*********************************************************************
*
* @purpose Reset all statistics on the specified interface.
*
* @param L7_uint32 unitIndex Unit Index
* @param L7_uint32 intf  internal interface number for per-interface
*                        statistic
*
* @returns L7_SUCCESS, if counters were successfully reset
* @returns L7_FAILURE, if function failed
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbIntfStatReset(L7_uint32 unitIndex, L7_uint32 intf)
{
  counterValue_entry_t CVE;
  pCounterValue_list_t pCVL = &CVE;
  L7_uint32 i;
  L7_uint32 interface;

  if (intf >= FD_CNFGR_NIM_MIN_LAG_INTF_NUM  &&  intf <= FD_CNFGR_NIM_MAX_LAG_INTF_NUM)
  {
    /* Reset all the usmdb counters for this lag interface */
    for (i = L7_CTR_RX_TOTAL_BYTES; i < L7_USMDB_NUM_OF_CTRS; i++)
    {
      pCVL->cId   = usmdbLagPortCtrIdTable[i];
      pCVL->raw   = L7_FALSE;
      pCVL->cKey  = intf;

      statsReset(1, pCVL);
    }
  }

  else
  {
    /* Reset all the usmdb counters for this interface */
    for (i = L7_CTR_RX_TOTAL_BYTES; i < L7_USMDB_NUM_OF_CTRS; i++)
    {
      pCVL->cId   = usmdbPhysPortCtrIdTable[i];
      pCVL->raw   = L7_FALSE;
      pCVL->cKey  = intf;

      statsReset(1, pCVL);
    }

  }
  /* Reset LACPDU counters for the interface */
  usmDbDot3adPortStatsClear(unitIndex, intf);

  /* Reset all system base counters for this interface */
  usmDbStatReset(unitIndex, L7_CTR_RX_GVRP_PDU_COUNT, intf);
  usmDbStatReset(unitIndex, L7_CTR_TX_GVRP_PDU_COUNT, intf);
  usmDbStatReset(unitIndex, L7_CTR_GVRP_FAILED_REGISTRATIONS, intf);
  usmDbStatReset(unitIndex, L7_CTR_RX_GMRP_PDU_COUNT, intf);
  usmDbStatReset(unitIndex, L7_CTR_TX_GMRP_PDU_COUNT, intf);
  usmDbStatReset(unitIndex, L7_CTR_GMRP_FAILED_REGISTRATIONS, intf);
  usmDbStatReset(unitIndex, L7_CTR_RX_BPDU_COUNT, intf);
  usmDbStatReset(unitIndex, L7_CTR_TX_BPDU_COUNT, intf);

	  usmDbDot1sPortStatsClear(unitIndex, intf);
  
  /* Reset all vlan statistics for this interface */
  usmDbStatReset(unitIndex, L7_CTR_DOT1Q_LOCAL_TRAFFIC_DISCARDS, intf);
  usmDbStatReset(unitIndex, L7_CTR_DOT1Q_RX_PAUSE_FRAMES, intf); 
  usmDbStatReset(unitIndex, L7_CTR_DOT1Q_UNACCEPTABLE_FRAME_TYPE_DISCARDS, intf);
  usmDbStatReset(unitIndex, L7_CTR_DOT1Q_INGRESS_FILTER_DISCARDS, intf);
  usmDbStatReset(unitIndex, L7_CTR_DOT1Q_INGRESS_VLAN_VIABLE_DISCARDS, intf);
  usmDbStatReset(unitIndex, L7_CTR_DOT1Q_MULTICAST_TREE_VIABLE_DISCARDS, intf);
  usmDbStatReset(unitIndex, L7_CTR_DOT1Q_RESERVED_ADDRESS_DISCARDS, intf);
  usmDbStatReset(unitIndex, L7_CTR_DOT1Q_BCAST_STORM_RECOVERY_DISCARDS, intf);
  usmDbStatReset(unitIndex, L7_CTR_DOT1Q_CFI_DISCARDS, intf);
  usmDbStatReset(unitIndex, L7_CTR_DOT1Q_UPSTREAM_THRESHOLD_DISCARDS, intf);
  usmDbStatReset(unitIndex, L7_CTR_DOT1Q_TX_PAUSE_FRAMES, intf);
  usmDbStatReset(unitIndex, L7_CTR_DOT1Q_PORT_MEMBERSHIP_DISCARDS, intf);
  usmDbStatReset(unitIndex, L7_CTR_DOT1Q_EGRESS_VLAN_VIABLE_DISCARDS, intf);

  /* Reset all IP Stats for the CPU interface*/
  usmDbStatReset(unitIndex, L7_CTR_RX_IP_IN_RECEIVES        , intf);
  usmDbStatReset(unitIndex, L7_CTR_RX_IP_IN_HDR_ERRORS      , intf);
  usmDbStatReset(unitIndex, L7_CTR_RX_IP_IN_ADDR_ERRORS     , intf);
  usmDbStatReset(unitIndex, L7_CTR_RX_IP_FORW_DATAGRAMS     , intf);
  usmDbStatReset(unitIndex, L7_CTR_RX_IP_IN_UNKNOWN_PROTOS  , intf);
  usmDbStatReset(unitIndex, L7_CTR_RX_IP_IN_DISCARDS        , intf);
  usmDbStatReset(unitIndex, L7_CTR_RX_IP_IN_DELIVERS        , intf);
  usmDbStatReset(unitIndex, L7_CTR_RX_IP_REASM_REQDS        , intf);
  usmDbStatReset(unitIndex, L7_CTR_RX_IP_REASM_OKS          , intf);
  usmDbStatReset(unitIndex, L7_CTR_RX_IP_REASM_FAILS        , intf);
  usmDbStatReset(unitIndex, L7_CTR_RX_IP_FRAG_OKS           , intf);
  usmDbStatReset(unitIndex, L7_CTR_RX_IP_FRAG_FAILS         , intf);
  usmDbStatReset(unitIndex, L7_CTR_RX_IP_FRAG_CREATES       , intf);
  usmDbStatReset(unitIndex, L7_CTR_RX_ICMP_IN_MSGS          , intf);
  usmDbStatReset(unitIndex, L7_CTR_RX_ICMP_IN_ERRORS        , intf);
  usmDbStatReset(unitIndex, L7_CTR_RX_ICMP_IN_DEST_UNREACHS , intf);
  usmDbStatReset(unitIndex, L7_CTR_RX_ICMP_IN_TIME_EXCDS    , intf);
  usmDbStatReset(unitIndex, L7_CTR_RX_ICMP_IN_PARM_PROBS    , intf);
  usmDbStatReset(unitIndex, L7_CTR_RX_ICMP_IN_SRC_QUENCHS   , intf);
  usmDbStatReset(unitIndex, L7_CTR_RX_ICMP_IN_REDIRECTS     , intf);
  usmDbStatReset(unitIndex, L7_CTR_RX_ICMP_IN_ECHOS         , intf);
  usmDbStatReset(unitIndex, L7_CTR_RX_ICMP_IN_ECHO_REPS     , intf);
  usmDbStatReset(unitIndex, L7_CTR_RX_ICMP_IN_TIMESTAMPS    , intf);
  usmDbStatReset(unitIndex, L7_CTR_RX_ICMP_IN_TIMESTAMP_REPS, intf);
  usmDbStatReset(unitIndex, L7_CTR_RX_ICMP_IN_ADDR_MASKS    , intf);
  usmDbStatReset(unitIndex, L7_CTR_RX_ICMP_IN_ADDR_MASK_REPS, intf);

  usmDbStatReset(unitIndex, L7_CTR_TX_IP_OUT_REQUESTS        , intf);
  usmDbStatReset(unitIndex, L7_CTR_TX_IP_OUT_DISCARDS        , intf);
  usmDbStatReset(unitIndex, L7_CTR_TX_IP_OUT_NO_ROUTES       , intf);
  usmDbStatReset(unitIndex, L7_CTR_TX_ICMP_OUT_MSGS          , intf);
  usmDbStatReset(unitIndex, L7_CTR_TX_ICMP_OUT_ERRORS        , intf);
  usmDbStatReset(unitIndex, L7_CTR_TX_ICMP_OUT_DEST_UNREACHS , intf);
  usmDbStatReset(unitIndex, L7_CTR_TX_ICMP_OUT_TIME_EXCDS    , intf);
  usmDbStatReset(unitIndex, L7_CTR_TX_ICMP_OUT_PARM_PROBS    , intf);
  usmDbStatReset(unitIndex, L7_CTR_TX_ICMP_OUT_SRC_QUENCHS   , intf);
  usmDbStatReset(unitIndex, L7_CTR_TX_ICMP_OUT_REDIRECTS     , intf);
  usmDbStatReset(unitIndex, L7_CTR_TX_ICMP_OUT_ECHOS         , intf);
  usmDbStatReset(unitIndex, L7_CTR_TX_ICMP_OUT_ECHO_REPS     , intf);
  usmDbStatReset(unitIndex, L7_CTR_TX_ICMP_OUT_TIMESTAMPS    , intf);
  usmDbStatReset(unitIndex, L7_CTR_TX_ICMP_OUT_TIMESTAMP_REPS, intf);
  usmDbStatReset(unitIndex, L7_CTR_TX_ICMP_OUT_ADDR_MASKS    , intf);
  usmDbStatReset(unitIndex, L7_CTR_TX_ICMP_OUT_ADDR_MASK_REPS, intf);

  if(usmDbMgtSwitchintIfNumGet(unitIndex, &interface) == L7_SUCCESS)
  {
    if (interface == intf)
    {
      fdbMaxEntriesReset();
      dot1qVlanResetStats();
    }
  }

  usmDbStatTimeReset(unitIndex, intf);

  return L7_SUCCESS;
}   


/*********************************************************************
*
* @purpose Reset a single interface's statistic reset time.
*
* @param L7_uint32 uid   USMDB counter identifier for this statistic
* @param L7_uint32 intf  internal interface number for per-interface
*                        statistic
*
* @returns L7_SUCCESS, if counter was successfully reset
* @returns L7_ERROR, if requested counter does not exist or not resettable
* @returns L7_FAILURE, if function failed
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbStatTimeReset(L7_uint32 unitIndex, L7_uint32 intf)
{
  return(nimClearCounters(intf));
}


/*********************************************************************
*
* @purpose Get a single 64-bit statistic.
*
* @param L7_uint32 UnitIndex Unit Index
* @param L7_uint32 uid    USMDB counter identifier for this statistic
* @param L7_uint32 intf   internal interface number for per-interface
*                         statistic
* @param L7_uint32 *high  pointer to output location for high-order 32-bits
*                         (@b{Output:} returned value of statistic high)
* @param L7_uint32 *low   pointer to output location for low-order 32-bits
*                         (@b{Output:} returned value of statistic low)
*
* @returns L7_SUCCESS, if counter was successfully reset
* @returns L7_ERROR, if requested counter does not exist
* @returns L7_FAILURE, if function failed (output not valid)
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbStatGet64(L7_uint32 UnitIndex, L7_uint32 uid, L7_uint32 intf, L7_uint32 *high, L7_uint32 *low)
{
  L7_RC_t rc;
  counterValue_entry_t CVE;
  pCounterValue_list_t pCVL = &CVE;
  L7_uint32 cid;

  if ((rc = usmDbCounterIdGet(UnitIndex, uid, intf, &cid)) != L7_SUCCESS)
    return rc;

  /* Unsupported counters have cid equal to 0.
  */
  if (cid == 0) 
  {
	  return L7_ERROR;
  }

  pCVL -> cId   = cid;
  pCVL -> raw   = L7_FALSE;
  pCVL -> cKey  = intf;

  rc = statsGet(1, pCVL);
  *high = pCVL -> cValue.high;
  *low = pCVL -> cValue.low;
  return rc;
}   


/*********************************************************************
*
* @purpose Get a counter ID from the appropriate translation table,
*          where necessary.
*
* @param L7_uint32 unitIndex Unit Index
* @param L7_uint32 uid   USMDB counter identifier for this statistic
* @param L7_uint32 intf  internal interface number for per-interface
*                        statistic
* @param L7_uint32 *val  pointer to output location
*                        (@b{Output:} counter ID of requested statistic)
*
* @returns L7_SUCCESS, if counter ID was successfully determined
* @returns L7_ERROR, if requested counter ID does not exist 
* @returns L7_FAILURE, if function failed (output not valid)
*
* @notes For the purpose of this function, the CPU port is treated the
*        same as a physical port type. 
*
* @end
*
*********************************************************************/
static L7_RC_t usmDbCounterIdGet(L7_uint32 unitIndex, L7_uint32 uid, L7_uint32 intf, L7_uint32 *pCid)
{
  L7_uint32 cid;

  /* a counter ID of 0 is invalid */
  if (uid == L7_USMDB_RESERVED)
    return L7_ERROR;

  /* if the counter id is one of the special USMDB counter index values,
   * translate it to a real counter ID by using the appropriate table...
   */
  if (uid < L7_USMDB_NUM_OF_CTRS)
  {
    if (dot3adIsLag(intf) == L7_TRUE)
      cid = usmdbLagPortCtrIdTable[uid];                                       /* LAG port type */

    /* use "else if(...)" for other port types in the future */

    else
      cid = usmdbPhysPortCtrIdTable[uid];                                      /* physical port type */
  }

  /* ...otherwise use specified counter ID as is */
  else
    cid = uid;

  *pCid = cid;
  return L7_SUCCESS;
}   


