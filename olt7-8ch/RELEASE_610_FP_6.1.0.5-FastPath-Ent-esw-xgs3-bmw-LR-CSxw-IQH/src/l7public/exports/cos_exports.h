/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename cos_exports.h
*
* @purpose  Defines constants and feature definitions that are shared by Management and the application
*
* @component
*
* @comments
*
*
* @Notes
*
* @created 03/26/2008
*
* @author akulkarn
* @end
*
**********************************************************************/

#ifndef __COS_EXPORTS_H_
#define __COS_EXPORTS_H_

#include "defaultconfig.h"

/* NOTE:  Defining separate upper limits for the number of interfaces,
 *        number of queues per interface, and number of drop precedence
 *        levels to be used in the config file data structures.  This 
 *        allows the config structure array size to remain constant 
 *        regardless of which port types are supported (e.g. physical,
 *        LAG), the number of active COS queues (e.g. one less for a 
 *        stacking build), and the number of supported drop precedence
 *        levels for the device.  This increases the likelihood of being
 *        able to use a saved config.
 */

/* NOTE: L7_COS_INTF_MAX_COUNT is defined in default_cnfgr.h */

#define L7_COS_INTF_QUEUE_MAX_COUNT     ((L7_uint32) \
                                         max(L7_MAX_CFG_QUEUES_PER_PORT, \
                                             FD_QOS_COS_MAX_QUEUES_PER_INTF))

#define L7_COS_INTF_DROP_PREC_MAX_COUNT ((L7_uint32) \
                                         max(L7_MAX_CFG_DROP_PREC_LEVELS, \
                                             FD_QOS_COS_MAX_DROP_PREC_PER_QUEUE))

/* QOS COS Component Feature List
 *
 * NOTE: Remember to update the feature string list in cos_util.c whenever
 *       a change is made to this list
 */
typedef enum
{
  /* COS support */
  L7_COS_FEATURE_SUPPORTED = 0,             /* general support statement */
  L7_COS_LAG_INTF_SUPPORT_FEATURE_ID,       /* COS config supported on LAG intf*/
  /* mapping tables */
  L7_COS_MAP_IPPREC_FEATURE_ID,             /* IP Precedence mapping table support */
  L7_COS_MAP_IPPREC_PER_INTF_FEATURE_ID,    /* IP Precedence mapping per intf */
  L7_COS_MAP_IPDSCP_FEATURE_ID,             /* IP DSCP mapping table support */
  L7_COS_MAP_IPDSCP_PER_INTF_FEATURE_ID,    /* IP DSCP mapping table per intf */
  L7_COS_MAP_TRUST_MODE_FEATURE_ID,         /* trust mode support */
  L7_COS_MAP_TRUST_MODE_PER_INTF_FEATURE_ID, /* trust mode per-intf cfg */
  /* queue configuration */
  L7_COS_QUEUE_CFG_PER_INTF_FEATURE_ID,     /* queue config per intf */
  L7_COS_QUEUE_DROP_CFG_PER_INTF_FEATURE_ID,  /* drop parms config per intf */
  L7_COS_QUEUE_INTF_SHAPING_FEATURE_ID,     /* traffic shaping on entire intf */
  L7_COS_QUEUE_MIN_BW_FEATURE_ID,           /* minimum bandwidth cfg-able */
  L7_COS_QUEUE_MAX_BW_FEATURE_ID,           /* maximum bandwidth cfg-able */
  L7_COS_QUEUE_SCHED_STRICT_ONLY_FEATURE_ID, /* instead of WRED */
  L7_COS_QUEUE_MGMT_INTF_ONLY_FEATURE_ID,   /* instead of per-queue mgmt */
  L7_COS_QUEUE_TDROP_THRESH_FEATURE_ID,     /* if taildrop thresh is cfg-able */
  L7_COS_QUEUE_WRED_SUPPORT_FEATURE_ID,     /* if WRED supported by device */
  L7_COS_QUEUE_WRED_MAX_THRESH_FEATURE_ID,  /* if WRED max thresh is cfg-able */
  L7_COS_QUEUE_WRED_DECAY_EXP_SYSTEM_ONLY_FEATURE_ID, /* instead of per-intf */
  L7_COS_QUEUE_BUCKET_FEATURE_ID,           /* bucket feature support */

 /* Queue Scheduler Configuration */
  L7_COS_QUEUE_SCHED_TYPE_GLOBAL_FEATURE_ID, /* all interfaces */
  L7_COS_QUEUE_SCHED_TYPE_PER_INTF_FEATURE_ID, /* per interface */
  L7_COS_QUEUE_SCHED_TYPE_PER_COS_QUEUE_GLOBAL_ONLY_FEATURE_ID, /* not per cos queue only global */

  L7_COS_FEATURE_ID_TOTAL                   /* total number of enum values */
} L7_COS_FEATURE_IDS_t;

/*--------------------------------------*/
/*  COS Constants and Types             */
/*--------------------------------------*/
#define L7_QOS_COS_MAP_NUM_IPPREC               8     /* indexed 0 to 7 */
#define L7_QOS_COS_MAP_NUM_IPDSCP               64    /* indexed 0 to 63 */

#define L7_QOS_COS_MAP_IPPREC_MIN               0     /* IP precedence range */
#define L7_QOS_COS_MAP_IPPREC_MAX               (L7_QOS_COS_MAP_NUM_IPPREC - 1)

#define L7_QOS_COS_MAP_IPDSCP_MIN               0     /* IP DSCP range */
#define L7_QOS_COS_MAP_IPDSCP_MAX               (L7_QOS_COS_MAP_NUM_IPDSCP - 1)

/* the following is defined same as for dot1p */
#define L7_QOS_COS_MAP_TRAFFIC_CLASS_MIN        0     /* traffic class range */
#define L7_QOS_COS_MAP_TRAFFIC_CLASS_MAX        (L7_DOT1P_NUM_TRAFFIC_CLASSES - 1)

#define L7_QOS_COS_INTF_SHAPING_RATE_UNITS      L7_RATE_UNIT_PERCENT     /* intf shaping rate expressed in percent of link speed or kbps */
#define L7_QOS_COS_INTF_SHAPING_RATE_MIN        0     /* intf shaping rate range */
#define L7_QOS_COS_INTF_SHAPING_RATE_MAX        100
#define L7_QOS_COS_INTF_SHAPING_STEP_SIZE       1    /* bandwidth intf shaping rate increment for percentage */
#define L7_QOS_COS_INTF_SHAPING_RATE_KBPS_MIN   64   /* intf shaping rate range */
#define L7_QOS_COS_INTF_SHAPING_RATE_KBPS_MAX   4294967295U

#define L7_QOS_COS_INTF_WRED_DECAY_EXP_MIN      0     /* WRED decay exp range */
#define L7_QOS_COS_INTF_WRED_DECAY_EXP_MAX      15

#define L7_QOS_COS_QUEUE_MIN_BANDWIDTH_MIN      0     /* min bandwidth range */
#define L7_QOS_COS_QUEUE_MIN_BANDWIDTH_MAX      100000000UL    // PTin modified

#define L7_QOS_COS_QUEUE_MAX_BANDWIDTH_MIN      0     /* max bandwidth range */
#define L7_QOS_COS_QUEUE_MAX_BANDWIDTH_MAX      100000000UL    // PTin modified

#define L7_QOS_COS_QUEUE_BANDWIDTH_STEP_SIZE       1     /* bandwidth percent increment */

#define L7_QOS_COS_QUEUE_TDROP_THRESH_MIN       0     /* tail drop thresh range */
#define L7_QOS_COS_QUEUE_TDROP_THRESH_MAX       100

#define L7_QOS_COS_QUEUE_WRED_MIN_THRESH_MIN    0     /* WRED min thresh range */
#define L7_QOS_COS_QUEUE_WRED_MIN_THRESH_MAX    100

#define L7_QOS_COS_QUEUE_WRED_MAX_THRESH_MIN    0     /* WRED max thresh range */
#define L7_QOS_COS_QUEUE_WRED_MAX_THRESH_MAX    100

#define L7_QOS_COS_QUEUE_WRED_DROP_PROB_MIN 0         /* WRED drop prob range */
#define L7_QOS_COS_QUEUE_WRED_DROP_PROB_MAX 100

/* QOS COS mapping interface trust mode */
typedef enum
{
  L7_QOS_COS_MAP_INTF_MODE_UNTRUSTED = 1,   /* do not trust any pkt markings */
  L7_QOS_COS_MAP_INTF_MODE_TRUST_DOT1P,     /* trust pkt Dot1p (802.1p) value */
  L7_QOS_COS_MAP_INTF_MODE_TRUST_IPPREC,    /* trust pkt IP Precedence value */
  L7_QOS_COS_MAP_INTF_MODE_TRUST_IPDSCP     /* trust pkt IP DSCP value */

} L7_QOS_COS_MAP_INTF_MODE_t;

/* QOS COS queue scheduler types */
typedef enum
{
  L7_QOS_COS_QUEUE_SCHED_TYPE_STRICT = 1,   /* strict priority */
  L7_QOS_COS_QUEUE_SCHED_TYPE_WEIGHTED      /* WRR/WFQ */

} L7_QOS_COS_QUEUE_SCHED_TYPE_t;

/* QOS COS queue management types */
typedef enum
{
  L7_QOS_COS_QUEUE_MGMT_TYPE_RESERVED = 0,  /* (rsvd for application) */
  L7_QOS_COS_QUEUE_MGMT_TYPE_TAILDROP,      /* tail drop */
  L7_QOS_COS_QUEUE_MGMT_TYPE_WRED,          /* weighted RED */
  L7_QOS_COS_QUEUE_MGMT_TYPE_UNCHANGED      /* don't change the config on this queue */

} L7_QOS_COS_QUEUE_MGMT_TYPE_t;

/* COS mapping tables and trust mode */
typedef struct
{
  L7_uchar8           ipPrecMapTable[L7_QOS_COS_MAP_NUM_IPPREC];
  L7_uchar8           ipDscpMapTable[L7_QOS_COS_MAP_NUM_IPDSCP];
  L7_uchar8           intfTrustMode;        /* COS mapping trust mode */
  L7_uchar8           rsvd1[3];             /* (reserved -- for alignment) */
} L7_cosMapCfg_t;

/* COS interface configuration parameters */
typedef struct
{
  L7_uint32           intfShapingRate;      /* 0-100% (0=unlimited) */
  L7_uchar8           queueMgmtTypePerIntf; /* per-intf: tail drop vs. WRED */
  L7_uchar8           wredDecayExponent;    /* WRED averaging constant (1-15) */
  L7_uchar8           rsvd1[2];             /* (reserved -- for alignment) */
} L7_cosIntfCfg_t;

/* COS drop precedence config parms per queue */
typedef struct
{
  L7_uchar8             tdropThresh;        /* 0-100% of queue len*/
  L7_uchar8             wredMinThresh;      /* 0-100% of queue len*/
  L7_uchar8             wredMaxThresh;      /* 0-100% of queue len*/
  L7_uchar8             wredDropProb;       /* 0-100% */
} L7_cosDropPrecCfg_t;

/* COS queue configuration parameters */
typedef struct
{
  L7_uchar8             queueId;        /* (for reference) */
  L7_ulong32            minBwPercent;   /* 0-100% (ignored for SP) */   /* PTin modified: L7_uchar8 */
  L7_ulong32            maxBwPercent;   /* 0-100% (0=unlimited) */      /* PTin modified: L7_uchar8 */
  L7_uchar8             schedulerType;  /* strict vs. weighted */
  L7_uchar8             queueMgmtType;  /* per-queue: tail drop vs. WRED */
  L7_uchar8             rsvd1[3];       /* (reserved -- for alignment) */
  L7_cosDropPrecCfg_t   dropPrec[L7_COS_INTF_DROP_PREC_MAX_COUNT+1]; /* DP parms, +1 for non-TCP WRED */
} L7_cosQueueCfg_t;

/* COS configurable parameters */
typedef struct
{
  L7_cosMapCfg_t          mapping;      /* mapping table config */
  L7_cosIntfCfg_t         intf;         /* other interface config */
  L7_cosQueueCfg_t        queue[L7_COS_INTF_QUEUE_MAX_COUNT];

} L7_cosCfgParms_t;

/******************** conditional Override *****************************/

#ifdef INCLUDE_COS_EXPORTS_OVERRIDES
#include "cos_exports_overrides.h"
#endif

#endif /* __COS_EXPORTS_H_*/
