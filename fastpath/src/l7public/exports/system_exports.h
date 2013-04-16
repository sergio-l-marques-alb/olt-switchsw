/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename system_exports.h
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

#ifndef __SYSTEM_EXPORTS_H_
#define __SYSTEM_EXPORTS_H_

/* STACKING Component Feature List */
typedef enum
{
  /* STACKING supported */
  L7_STACKING_FEATURE_SUPPORTED = 0,          /* general support statement */
  L7_FPS_FEATURE_ID,   /* Front-panel stacking support */
  L7_FPS_QOS_REPLACEMENT_FEATURE_ID,  /* Special QOS support for front panel stacking ports */
  L7_FPS_PORT_MODE_CFG_FEATURE_ID, /* Ports are configurable from Ethernet to Stack mode */
  L7_STACKING_NSF_FEATURE_ID,              /* nonstop forwarding */
  L7_STACKING_SFS_FEATURE_ID,        /* Stack Firmware Sync Feature */
  L7_STACKING_FEATURE_ID_TOTAL                /* total number of enum values */
} L7_STACKING_FEATURE_IDS_t;

/*--------------------------------------*/
/*  BOOTP/DHCP Constants                */
/*--------------------------------------*/

typedef enum
{
  L7_SYSCONFIG_MODE_NONE =1,
  L7_SYSCONFIG_MODE_BOOTP,
  L7_SYSCONFIG_MODE_DHCP,
  L7_SYSCONFIG_MODE_NOT_COMPLETE,
  L7_SYSCONFIG_MODE_COMPLETE
} L7_SYSCFG_MODE_t;


/*--------------------------------------*/
/*  MAC Address Constants               */
/*--------------------------------------*/

#define L7_MAC_ADDR_LEN                 6
#define L7_MAC_ADDR_STRING_LEN          (L7_MAC_ADDR_LEN * 3)
#define L7_VID_MAC_ADDR_LEN             (L7_MAC_ADDR_LEN+L7_MFDB_VLANID_LEN)

/*--------------------------------------*/
/*  IP Address Constants                */
/*--------------------------------------*/

#define L7_IP_ADDR_STRING_LEN           16      /* "xxx.xxx.xxx.xxx" */

typedef enum
{
  L7_SYSMAC_NONE = 0,
  L7_SYSMAC_BIA,            /* Burned-In Address */
  L7_SYSMAC_LAA             /* Locally-Administered Address */
} L7_MACADDR_TYPE_t;

/*--------------------------------------*/
/*  Baud Rates                          */
/*--------------------------------------*/

typedef enum
{
  L7_BAUDRATE_1200 =1,
  L7_BAUDRATE_2400,
  L7_BAUDRATE_4800,
  L7_BAUDRATE_9600,
  L7_BAUDRATE_19200,
  L7_BAUDRATE_38400,
  L7_BAUDRATE_57600,
  L7_BAUDRATE_115200
} L7_BAUDRATES_t;

/*
* Parity Types
*/
typedef enum
{
  L7_PARITY_EVEN,
  L7_PARITY_ODD,
  L7_PARITY_NONE
} L7_PARITY_t;

/*
* LED Color Types
*/
typedef enum
{
  L7_LED_GREEN = 1,
  L7_LED_AMBER,    /* or orange */
  L7_LED_YELLOW,
  L7_LED_RED,
  L7_LED_OFF,       /* black */
  L7_LED_BLINK_GREEN,
  L7_LED_BLINK_AMBER,
} L7_LED_COLOR_t;


/*
* Serial Port Stop Bits
*/
#define L7_STOP_BIT_1 1
#define L7_STOP_BIT_2 2


/*********************************************************************
*
* RESET TYPES
*
*********************************************************************/
typedef enum
{
  L7_WARMRESET,
  L7_COLDRESET

} L7_RESET_TYPE;

typedef enum
{
  /* The switch re-booted. This could have been caused by a power cycle 
   * or an administrative "Reload" command. */
  L7_STARTUP_POWER_ON = 0, 

  /* The administrator issued the "move-management" command */
  L7_STARTUP_COLD_ADMIN_MOVE,

  /* The administrator issued the "initiate failover" command for the 
   * stand-by manager to take over. */
  L7_STARTUP_WARM_ADMIN_MOVE, 

  /* The primary management unit restarted because of a failure, and the 
   * system executed a nonstop forwarding failover. */
  L7_STARTUP_AUTO_WARM,

  /* The system switched from the active manager to the backup manager and 
   * was unable to maintain nonstop forwarding. This is usually caused by 
   * multiple failures occurring close together or because NSF is operationally
   * disabled. */
  L7_STARTUP_AUTO_COLD

} L7_LAST_STARTUP_REASON_t;

/* Max number of characters (incl NULL term) in startup reason string */
#define L7_STARTUP_REASON_STR_LEN 32

typedef enum
{
    /* Running configuration has not yet been copied to the backup unit. */
    L7_RUN_CFG_NONE = 0,

    /* Running configuration has been copied, but it is out of date */
    L7_RUN_CFG_STALE,

    /* Copy of running configuration is in progress */
    L7_RUN_CFG_IN_PROGRESS,

    /* Running configuration on backup is the same as on mgmt unit */
    L7_RUN_CFG_CURRENT,

    /* No backup unit has been elected */
    L7_RUN_CFG_NO_BACKUP

} L7_RUN_CFG_STATUS_t;

/* Max number of characters (incl NULL term) in run config status string */
#define L7_RUN_CFG_STATUS_STR_LEN 32


/* checkpoint statistics */
typedef struct
{
  L7_uint32  numMsgs;   
  L7_uint32  numBytes;  
  L7_uint32  timestamp; 
  L7_uint32  msgRate;   
  L7_uint32  msgRateMax;
} CkptStats_t;

#define CKPT_STATS_COLL_INT  10    /* checkpoint stat collection interval (seconds) */


/*********************************************************************
*
* COMPONENT INTIALIZATION
*
*********************************************************************/

typedef enum
{

  L7_PARM_STRUCT = 1,
  L7_PARM_FILE

}L7_PARM_TYPES_t;

/*---------------------------------------*/
/*  Network Processing device Enum.      */
/*  Add other ID's by appending to       */
/*  the list, never add in the middle.   */
/*                                       */
/*  NOTE: When introducing an ID for a   */
/*  fundamentally different platform,    */
/*  be sure to add an entry to the       */
/*  L7_BASE_TECHNOLOGY_TYPES_t below.    */
/*---------------------------------------*/
/* PTin updated: new platform */
typedef enum
{
  __BROADCOM_5615_ID = 1,
  __BROADCOM_5645_ID,
  __BROADCOM_5650_ID,
  __BROADCOM_5665_ID,
  __BROADCOM_5673_ID,
  __BROADCOM_5674_ID,
  __BROADCOM_5675_ID,
  __BROADCOM_5690_ID,
  __BROADCOM_5691_ID,
  __BROADCOM_5692_ID,
  __BROADCOM_5693_ID,
  __BROADCOM_5695_ID,
  __BROADCOM_56504_ID,
  __BROADCOM_56304_ID,
  __BROADCOM_56314_ID,
  __BROADCOM_56800_ID,
  __BROADCOM_56514_ID,
  __BROADCOM_56214_ID,
  __BROADCOM_56218_ID,
  __BROADCOM_56224_ID,
  __BROADCOM_56228_ID,
  __BROADCOM_56624_ID,
  __BROADCOM_56680_ID,
  __BROADCOM_56820_ID,
  __BROADCOM_56634_ID,
  __BROADCOM_56524_ID,
  __BROADCOM_53115_ID,
  __BROADCOM_53314_ID,
  __BROADCOM_56636_ID,
  __BROADCOM_56334_ID,
  __BROADCOM_56685_ID,  /* PTin added: new switch */
  __BROADCOM_56843_ID,  /* PTin added: new switch 56843 */
  __BROADCOM_56643_ID,  /* PTin added: new switch 56643 */
  __MASTER_DRIVER_ID,
} L7_NPD_ID_t;

/* Summarization of Network Processing devices into
 * a generic base technology specification
 *
 * Note:  NPDs that operate in essentially the same
 *        manner from a software application point of
 *        view should be represented by a single entry
 *        here.
 */
typedef enum
{
  L7_BASE_TECHNOLOGY_TYPE_BROADCOM_STRATA = 1,
  L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS,
  L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS3,
  L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS4,
  L7_BASE_TECHNOLOGY_TYPE_BROADCOM_ROBO,
  L7_BASE_TECHNOLOGY_TYPE_MASTER_DRIVER,
  L7_BASE_TECHNOLOGY_TYPE_UNKNOWN,      /* unidentified technology type */
  L7_BASE_TECHNOLOGY_TYPE_TOTAL         /* total number of enum values */
} L7_BASE_TECHNOLOGY_TYPES_t;

/*  NPD subtype specification
 *
 * Note:  Some NPD's share the same BASE_TECHNOLOGY_TYPE above but
 *        have differences in features that need further granularity.
 *        e.g. ECMP is supported in the 5695, but not the 5690, but
 *        they both are L7_BASE_TECHNOLOGY_TYPE_BROADCOM_XGS.
 */

 typedef enum
{
  L7_BASE_TECHNOLOGY_SUBTYPE_NONE = 1,  /* no meaningful subtype for the technology */
  L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5665,
  L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5673,
  L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5690,
  L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5695,
  L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5630x,  /* helix     */
  L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5631x,  /* helixplus */
  L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5650x,  /* firebolt  */
  L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5651x,  /* firebolt2 */
  L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5660x,  /* easyrider */
  L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5675,
  L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_56800,  /* bradley */
  L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS_5621x,  /* raptor */
  L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS4_5668x, /* valkyrie */
  L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS4_5662x, /* triumph */
  L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS4_5682x, /* scorpion */
  L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS4_56520, /* triumph2 family */
  L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS4_5664x, /* triumph3 family */   /* PTin added: new switch BCM56643 */ /* PTin updated: new platform */
  L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_XGS4_5684x, /* PTin added: new switch: trident */
  L7_BASE_TECHNOLOGY_SUBTYPE_BROADCOM_ROBO_53115,  /* vulcan */
  L7_BASE_TECHNOLOGY_SUBTYPE_MASTERDRIVER_NETAPPL,  /* network appliance */
  L7_BASE_TECHNOLOGY_SUBTYPE_UNKNOWN,      /* unidentified technology type */
  L7_BASE_TECHNOLOGY_SUBTYPE_TOTAL         /* total number of enum values */
} L7_BASE_TECHNOLOGY_SUBTYPES_t;

/************************
 * Sysnet PDU Intercept *
 ************************/
#define L7_SYSNET_PDU_MAX_HOOKS  16  /* Max number of hooks per address family */

typedef enum
{
  L7_SYSNET_HOOK_PRECEDENCE_0 = 0,   /* Lowest value is highest precedence */
  L7_SYSNET_HOOK_PRECEDENCE_1,
  L7_SYSNET_HOOK_PRECEDENCE_2,
  L7_SYSNET_HOOK_PRECEDENCE_3,
  L7_SYSNET_HOOK_PRECEDENCE_4,
  L7_SYSNET_HOOK_PRECEDENCE_5,
  L7_SYSNET_HOOK_PRECEDENCE_6,
  L7_SYSNET_HOOK_PRECEDENCE_7,       /* Highest value is lowest precedence */
  L7_SYSNET_HOOK_PRECEDENCE_LAST     /* Number of precedence values */
} L7_SYSNET_HOOK_PRECEDENCE_t;

#define L7_MIN_BROADCAST_RATE 0
#define L7_MAX_BROADCAST_RATE 33554432

#define FACTORY_DEFAULT_REFER   11
#define FACTORY_DEFAULT_DEFINE  22

#if defined(L7_CLI_PACKAGE)
#define FACTORY_DEFAULT_DEFINE_CLI FACTORY_DEFAULT_DEFINE
#define FACTORY_DEFAULT_DEFINE_SNMP FACTORY_DEFAULT_REFER
#elif defined(L7_SNMP_PACKAGE)
#define FACTORY_DEFAULT_DEFINE_CLI FACTORY_DEFAULT_REFER
#define FACTORY_DEFAULT_DEFINE_SNMP FACTORY_DEFAULT_DEFINE
#endif

#define L7_SYSMGMT_ASSETTAG_MAX  16
#define L7_SYSMGMT_SERVICE_TAG_MAX_LENGTH 16
#define L7_SYSMGMT_CHASSIS_SERVICE_TAG_MAX_LENGTH 16
#define L7_SYSMGMT_CHASSIS_SLOT_ID_MAX_LENGTH 16

/*  Action to be taken on packets that come to the CPU with   
 *  DMAC of one of the reserved MACs which correspond to   
 *  protocols that we do not support.
 *  Ex: Packets with DMAC 01:80:c2:00:00:15 (ISIS)  
 *
 *  Default action is to flood these packets.
 *  Add an override and set it to L7_FALSE in case 
 *  these packets need to be dropped.
 */

#define L7_RESERVED_ADDRESS_FLOOD L7_TRUE

/******************** conditional Override *****************************/

#ifdef INCLUDE_SYSTEM_EXPORTS_OVERRIDES
#include "system_exports_overrides.h"
#endif

#endif /* __SYSTEM_EXPORTS_H_*/
