/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   diffserv_stdmib.h
*
* @purpose    DiffServ component standard MIB structures
*
* @component  DiffServ
*
* @comments   none
*
* @create     04/12/2002
*
* @author     vbhaskar
* @end
*
**********************************************************************/
#ifndef INCLUDE_DIFFSERV_STDMIB_H
#define INCLUDE_DIFFSERV_STDMIB_H

#include "l7_common.h"
#include "defaultconfig.h"
#include "diffserv_cfg_limit.h"
#include "usmdb_mib_diffserv_api.h"

/* external references */
extern L7_uint32 clfrIdNextFree;                /* next available clfr id     */
extern L7_uint32 multiFieldClfrIdNextFree;      /* next available multi field */
                                                /*   clfr id                  */
extern L7_uint32 auxMFClfrIdNextFree;           /* next available auxiliary MF*/
                                                /*   clfr id                  */   
extern L7_uint32 meterIdNextFree;               /* next available meter id    */
extern L7_uint32 tbParamIdNextFree;             /* next available token bucket*/
                                                /*   parameter id             */
extern L7_uint32 actionIdNextFree;              /* next available action id   */   
extern L7_uint32 countActIdNextFree;            /* next available count act id*/   
extern L7_uint32 assignQueueIdNextFree;         /* next available assign queue id*/   
extern L7_uint32 redirectIdNextFree;            /* next available redirect id */   
extern L7_uint32 mirrorIdNextFree;              /* next available mirror id */   
extern L7_uint32 algDropIdNextFree;             /* next available alg drop id */
extern L7_uint32 randomDropIdNextFree;          /* next available random drop id*/
extern L7_uint32 qIdNextFree;                   /* next available queue id    */  
extern L7_uint32 schedulerIdNextFree;           /* next available scheduler id*/
extern L7_uint32 minRateIdNextFree;             /* next available min rate id */
extern L7_uint32 maxRateIdNextFree;             /* next available max rate id */

/* AVL tree control fields */
typedef struct
{
  L7_uint32         entryMaxNum;                /* maximum number of elements */
  L7_uint32         entrySize;                  /* size of each element       */
  L7_uint32         entryKeySize;               /* size of element search key */
  avlTreeTables_t   *pTreeHeap;                 /* tree heap ptr              */
  void              *pDataHeap;                 /* data heap ptr              */
} dsStdMibAvlCtrl_t;

/* Enum for Standard MIB Table IDs */
typedef enum
{
  DSSTDMIB_TABLE_ID_NONE = 0,

  DSSTDMIB_TABLE_ID_DATA_PATH,
  DSSTDMIB_TABLE_ID_CLFR,
  DSSTDMIB_TABLE_ID_CLFR_ELEMENT,
  DSSTDMIB_TABLE_ID_MULTI_FIELD_CLFR,
  DSSTDMIB_TABLE_ID_AUX_MF_CLFR,
  DSSTDMIB_TABLE_ID_METER,
  DSSTDMIB_TABLE_ID_TB_PARAM,
  DSSTDMIB_TABLE_ID_COLOR_AWARE,
  DSSTDMIB_TABLE_ID_ACTION,
  DSSTDMIB_TABLE_ID_MARK_COS_ACT,
  DSSTDMIB_TABLE_ID_MARK_COS_AS_COS2_ACT,
  DSSTDMIB_TABLE_ID_MARK_COS2_ACT,
  DSSTDMIB_TABLE_ID_MARK_IPDSCP_ACT,
  DSSTDMIB_TABLE_ID_MARK_IPPREC_ACT,
  DSSTDMIB_TABLE_ID_COUNT_ACT,
  DSSTDMIB_TABLE_ID_ASSIGN_QUEUE,
  DSSTDMIB_TABLE_ID_REDIRECT,
  DSSTDMIB_TABLE_ID_MIRROR,
  DSSTDMIB_TABLE_ID_ALG_DROP,
/*  DSSTDMIB_TABLE_ID_RANDOM_DROP,     deprecated */
/*  DSSTDMIB_TABLE_ID_QUEUE,           deprecated */
/*  DSSTDMIB_TABLE_ID_SCHEDULER,       deprecated */
/*  DSSTDMIB_TABLE_ID_MIN_RATE,        deprecated */
/*  DSSTDMIB_TABLE_ID_MAX_RATE,        deprecated */

  DSSTDMIB_TABLE_ID_TOTAL                       /* number of enum entries */

} dsStdMibTableId_t;

/* Enumerated values for RowStatus object */
typedef enum
{
  L7_DIFFSERV_ROW_STATUS_NONE = 0,    /* not a valid value */

  L7_DIFFSERV_ROW_STATUS_ACTIVE,
  L7_DIFFSERV_ROW_STATUS_NOT_IN_SERVICE,
  L7_DIFFSERV_ROW_STATUS_DESTROY,

  L7_DIFFSERV_ROW_STATUS_TOTAL        /* number of enum entries */

} L7_DIFFSERV_ROW_STATUS_t;

typedef enum
{
  DSSTDMIB_ALG_DROPPER_TYPE_NONE = 0,
  DSSTDMIB_ALG_DROPPER_TYPE_TAIL,
  DSSTDMIB_ALG_DROPPER_TYPE_RANDOM,
  DSSTDMIB_ALG_DROPPER_TYPE_ALWAYS
}DSSTDMIB_ALG_DROPPER_TYPE_t;

typedef enum
{
  DSSTDMIB_ACTION_TYPE_NONE = 0,
  DSSTDMIB_ACTION_TYPE_POLICE_SIMPLE,
  DSSTDMIB_ACTION_TYPE_POLICE_SINGLERATE,
  DSSTDMIB_ACTION_TYPE_POLICE_TWORATE,
  DSSTDMIB_ACTION_TYPE_SHAPE_AVERAGE,
  DSSTDMIB_ACTION_TYPE_SHAPE_PEAK
}DSSTDMIB_ACTION_TYPE_t;

typedef enum
{
  DSSTDMIB_RATE_TYPE_NONE = 0,
  DSSTDMIB_RATE_TYPE_COMMITTED,
  DSSTDMIB_RATE_TYPE_EXCESS,
  DSSTDMIB_RATE_TYPE_PEAK
}DSSTDMIB_RATE_TYPE_t;

typedef enum
{
  DSSTDMIB_ATTR_TYPE_NONE = 0,
  DSSTDMIB_ATTR_TYPE_BANDWIDTH,
  DSSTDMIB_ATTR_TYPE_EXPEDITE
}DSSTDMIB_ATTR_TYPE_t;

typedef enum
{
  DSSTDMIB_COUNT_TYPE_NONE = 0,
  DSSTDMIB_COUNT_TYPE_IN_OFFERED,
  DSSTDMIB_COUNT_TYPE_OUT_OFFERED,
}DSSTDMIB_COUNT_TYPE_t;

typedef enum
{
  DSSTDMIB_MARKER_TYPE_NONE = 0,
  DSSTDMIB_MARKER_TYPE_COS,
  DSSTDMIB_MARKER_TYPE_COS2,
  DSSTDMIB_MARKER_TYPE_IPDSCP,
  DSSTDMIB_MARKER_TYPE_IPPREC
}DSSTDMIB_MARKER_TYPE_t;

typedef enum
{
  DSSTDMIB_SHAPER_TYPE_NONE = 0,
  DSSTDMIB_SHAPER_TYPE_AVERAGE,
  DSSTDMIB_SHAPER_TYPE_PEAK
}DSSTDMIB_SHAPER_TYPE_t;

/* general typedefs */
typedef L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_t    dsStdMibStorageType_t;
typedef L7_DIFFSERV_ROW_STATUS_t                dsStdMibRowStatus_t;
typedef L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_t     dsStdMibTruthValue_t;
typedef L7_USMDB_MIB_DIFFSERV_INET_ADDR_TYPE_t  dsStdMibInetAddrType_t;

typedef struct
{
  dsStdMibTableId_t tableId;
  L7_uint32         rowIndex1;
  L7_uint32         rowIndex2;
  L7_uint32         rowIndex3;
} dsStdMibRowPtr_t;

/* index maximum definitions */
#define DSSTDMIB_DATAPATH_INTF_INDEX_MAX          (L7_MAX_INTERFACE_COUNT-1)                
#define DSSTDMIB_DATAPATH_DIR_INDEX_MAX           2                                         
#define DSSTDMIB_CLFR_INDEX_MAX                   L7_DIFFSERV_CLFR_LIM                      
#define DSSTDMIB_CLFR_ELEMENT_INDEX_MAX           L7_DIFFSERV_CLFR_ELEMENT_LIM              
#define DSSTDMIB_MULTI_FIELD_CLFR_INDEX_MAX       L7_DIFFSERV_MULTI_FIELD_CLFR_LIM          
#define DSSTDMIB_AUX_MF_CLFR_INDEX_MAX            L7_DIFFSERV_AUX_MF_CLFR_LIM               
#define DSSTDMIB_METER_INDEX_MAX                  L7_DIFFSERV_METER_LIM                     
#define DSSTDMIB_TBPARAM_INDEX_MAX                L7_DIFFSERV_TBPARAM_LIM                   
#define DSSTDMIB_ACTION_INDEX_MAX                 L7_DIFFSERV_ACTION_LIM                    
#define DSSTDMIB_COS_MARK_ACT_INDEX_MAX           (L7_DIFFSERV_COS_MARK_ACT_LIM-1)          
#define DSSTDMIB_COS2_MARK_ACT_INDEX_MAX          (L7_DIFFSERV_COS2_MARK_ACT_LIM-1)      
#define DSSTDMIB_DSCP_MARK_ACT_INDEX_MAX          (L7_DIFFSERV_DSCP_MARK_ACT_LIM-1)         
#define DSSTDMIB_IPPREC_MARK_ACT_INDEX_MAX        (L7_DIFFSERV_IPPREC_MARK_ACT_LIM-1)       
#define DSSTDMIB_COUNT_ACT_INDEX_MAX              L7_DIFFSERV_COUNT_ACT_LIM                 
#define DSSTDMIB_ASSIGN_QUEUE_INDEX_MAX           L7_DIFFSERV_ASSIGN_QUEUE_LIM              
#define DSSTDMIB_REDIRECT_INDEX_MAX               L7_DIFFSERV_REDIRECT_LIM                  
#define DSSTDMIB_MIRROR_INDEX_MAX                 L7_DIFFSERV_MIRROR_LIM                  
#define DSSTDMIB_ALG_DROP_INDEX_MAX               L7_DIFFSERV_ALG_DROP_LIM                  
#define DSSTDMIB_RANDOM_DROP_INDEX_MAX            L7_DIFFSERV_RANDOM_DROP_LIM               
#define DSSTDMIB_SCHEDULER_INDEX_MAX              L7_DIFFSERV_SCHEDULER_LIM                 
#define DSSTDMIB_MINRATE_INDEX_MAX                L7_DIFFSERV_MINRATE_LIM                   
#define DSSTDMIB_MAXRATE_INDEX_MAX                L7_DIFFSERV_MAXRATE_LIM                   
#define DSSTDMIB_MAXRATE_LEVEL_MAX                L7_USMDB_MIB_DIFFSERV_MAXRATE_LEVEL_TOTAL 

/* derived table size maximums */
#define DSSTDMIB_DATAPATH_TABLE_SIZE_MAX          L7_DIFFSERV_DATAPATH_LIM
#define DSSTDMIB_CLFR_TABLE_SIZE_MAX              DSSTDMIB_CLFR_INDEX_MAX               
#define DSSTDMIB_CLFR_ELEMENT_TABLE_SIZE_MAX      DSSTDMIB_CLFR_ELEMENT_INDEX_MAX       
#define DSSTDMIB_MULTI_FIELD_CLFR_TABLE_SIZE_MAX  DSSTDMIB_MULTI_FIELD_CLFR_INDEX_MAX   
#define DSSTDMIB_AUX_MF_CLFR_TABLE_SIZE_MAX       DSSTDMIB_AUX_MF_CLFR_INDEX_MAX        
#define DSSTDMIB_METER_TABLE_SIZE_MAX             DSSTDMIB_METER_INDEX_MAX              
#define DSSTDMIB_TBPARAM_TABLE_SIZE_MAX           DSSTDMIB_TBPARAM_INDEX_MAX            
#define DSSTDMIB_ACTION_TABLE_SIZE_MAX            DSSTDMIB_ACTION_INDEX_MAX             
#define DSSTDMIB_COS_MARK_ACT_TABLE_SIZE_MAX      L7_DIFFSERV_COS_MARK_ACT_LIM          
#define DSSTDMIB_COS2_MARK_ACT_TABLE_SIZE_MAX     L7_DIFFSERV_COS2_MARK_ACT_LIM        
#define DSSTDMIB_DSCP_MARK_ACT_TABLE_SIZE_MAX     L7_DIFFSERV_DSCP_MARK_ACT_LIM         
#define DSSTDMIB_IPPREC_MARK_ACT_TABLE_SIZE_MAX   L7_DIFFSERV_IPPREC_MARK_ACT_LIM       
#define DSSTDMIB_COUNT_ACT_TABLE_SIZE_MAX         DSSTDMIB_COUNT_ACT_INDEX_MAX          
#define DSSTDMIB_ASSIGN_QUEUE_TABLE_SIZE_MAX      DSSTDMIB_ASSIGN_QUEUE_INDEX_MAX       
#define DSSTDMIB_REDIRECT_TABLE_SIZE_MAX          DSSTDMIB_REDIRECT_INDEX_MAX           
#define DSSTDMIB_MIRROR_TABLE_SIZE_MAX            DSSTDMIB_MIRROR_INDEX_MAX           
#define DSSTDMIB_ALG_DROP_TABLE_SIZE_MAX          DSSTDMIB_ALG_DROP_INDEX_MAX           


/* index parm range checking macros */
#define DSSTDMIB_DATAPATH_INTF_INDEX_OUT_OF_RANGE(_idx) \
          ((L7_BOOL)(((_idx) == 0) || ((_idx) > (L7_uint32)DSSTDMIB_DATAPATH_INTF_INDEX_MAX)))
#define DSSTDMIB_DATAPATH_DIR_INDEX_OUT_OF_RANGE(_idx) \
          ((L7_BOOL)(((_idx) == 0) || ((_idx) > (L7_uint32)DSSTDMIB_DATAPATH_DIR_INDEX_MAX)))
#define DSSTDMIB_CLFR_INDEX_OUT_OF_RANGE(_idx) \
          ((L7_BOOL)(((_idx) == 0) || ((_idx) > (L7_uint32)DSSTDMIB_CLFR_INDEX_MAX)))
#define DSSTDMIB_CLFR_ELEMENT_INDEX_OUT_OF_RANGE(_idx) \
          ((L7_BOOL)(((_idx) == 0) || ((_idx) > (L7_uint32)DSSTDMIB_CLFR_ELEMENT_INDEX_MAX)))
#define DSSTDMIB_MULTI_FIELD_CLFR_INDEX_OUT_OF_RANGE(_idx) \
          ((L7_BOOL)(((_idx) == 0) || ((_idx) > (L7_uint32)DSSTDMIB_MULTI_FIELD_CLFR_INDEX_MAX)))
#define DSSTDMIB_AUX_MF_CLFR_INDEX_OUT_OF_RANGE(_idx) \
          ((L7_BOOL)(((_idx) == 0) || ((_idx) > (L7_uint32)DSSTDMIB_AUX_MF_CLFR_INDEX_MAX)))
#define DSSTDMIB_METER_INDEX_OUT_OF_RANGE(_idx) \
          ((L7_BOOL)(((_idx) == 0) || ((_idx) > (L7_uint32)DSSTDMIB_METER_INDEX_MAX)))
#define DSSTDMIB_TBPARAM_INDEX_OUT_OF_RANGE(_idx) \
          ((L7_BOOL)(((_idx) == 0) || ((_idx) > (L7_uint32)DSSTDMIB_TBPARAM_INDEX_MAX)))
#define DSSTDMIB_ACTION_INDEX_OUT_OF_RANGE(_idx) \
          ((L7_BOOL)(((_idx) == 0) || ((_idx) > (L7_uint32)DSSTDMIB_ACTION_INDEX_MAX)))
#define DSSTDMIB_COS_MARK_ACT_INDEX_OUT_OF_RANGE(_idx) \
          ((L7_BOOL)((_idx) > (L7_uint32)DSSTDMIB_COS_MARK_ACT_INDEX_MAX))
#define DSSTDMIB_COS2_MARK_ACT_INDEX_OUT_OF_RANGE(_idx) \
          ((L7_BOOL)((_idx) > (L7_uint32)DSSTDMIB_COS2_MARK_ACT_INDEX_MAX))
#define DSSTDMIB_DSCP_MARK_ACT_INDEX_OUT_OF_RANGE(_idx) \
          ((L7_BOOL)((_idx) > (L7_uint32)DSSTDMIB_DSCP_MARK_ACT_INDEX_MAX))
#define DSSTDMIB_IPPREC_MARK_ACT_INDEX_OUT_OF_RANGE(_idx) \
          ((L7_BOOL)((_idx) > (L7_uint32)DSSTDMIB_IPPREC_MARK_ACT_INDEX_MAX))
#define DSSTDMIB_COUNT_ACT_INDEX_OUT_OF_RANGE(_idx) \
          ((L7_BOOL)(((_idx) == 0) || ((_idx) > (L7_uint32)DSSTDMIB_COUNT_ACT_INDEX_MAX)))
#define DSSTDMIB_ASSIGN_QUEUE_INDEX_OUT_OF_RANGE(_idx) \
          ((L7_BOOL)(((_idx) == 0) || ((_idx) > (L7_uint32)DSSTDMIB_ASSIGN_QUEUE_INDEX_MAX)))
#define DSSTDMIB_REDIRECT_INDEX_OUT_OF_RANGE(_idx) \
          ((L7_BOOL)(((_idx) == 0) || ((_idx) > (L7_uint32)DSSTDMIB_REDIRECT_INDEX_MAX)))
#define DSSTDMIB_MIRROR_INDEX_OUT_OF_RANGE(_idx) \
          ((L7_BOOL)(((_idx) == 0) || ((_idx) > (L7_uint32)DSSTDMIB_MIRROR_INDEX_MAX)))
#define DSSTDMIB_ALG_DROP_INDEX_OUT_OF_RANGE(_idx) \
          ((L7_BOOL)(((_idx) == 0) || ((_idx) > (L7_uint32)DSSTDMIB_ALG_DROP_INDEX_MAX)))

/* Default values */
#define DSSTDMIB_DEFAULT_ROWPTR         dsStdMibZeroDotZero
#define DSSTDMIB_DEFAULT_STORAGE_TYPE   3 /* non-volatile(3) */
#define DSSTDMIB_DEFAULT_IP_MASK        0
#define DSSTDMIB_DEFAULT_PROTOCOL_NUM   255
#define DSSTDMIB_DEFAULT_PORT_MIN       0
#define DSSTDMIB_DEFAULT_PORT_MAX       65535 
#define DSSTDMIB_DEFAULT_COS            (-1)
#define DSSTDMIB_DEFAULT_ETYPE_VAL      0       /* 0 means inactive */
#define DSSTDMIB_DEFAULT_TOS_MASK       0
#define DSSTDMIB_DEFAULT_MAC_MASK       0
#define DSSTDMIB_DEFAULT_VID_MIN        0
#define DSSTDMIB_DEFAULT_VID_MAX        4095
#define DSSTDMIB_DEFAULT_DSCP_VAL       (-1)
#define DSSTDMIB_DEFAULT_QTHRESHOLD     16384   /* need to get from HW somehow*/
#define DSSTDMIB_DEFAULT_COLOR_LEVEL    L7_USMDB_MIB_DIFFSERV_COLOR_LEVEL_NONE
#define DSSTDMIB_DEFAULT_COLOR_MODE     L7_USMDB_MIB_DIFFSERV_COLOR_MODE_BLIND
#define DSSTDMIB_DEFAULT_COLOR_VALUE    0

/* internal definitions */
#define DSSTDMIB_ZERO_DOT_ZERO          dsStdMibZeroDotZero
#define DSSTDMIB_PATH_NEXT_UPDATE       L7_USMDB_MIB_DIFFSERV_ROW_PTR_UPDATE
#define DSSTDMIB_ROW_PTR_SET            L7_USMDB_MIB_DIFFSERV_ROW_PTR_UPDATE

#define DSSTDMIB_ROW_PTR_IS_SAME(_rp1, _rp2) \
  ((memcmp((_rp1), (_rp2), sizeof(L7_USMDB_MIB_DIFFSERV_ROW_PTR_t)) == 0) \
    ? L7_TRUE : L7_FALSE)
#define DSSTDMIB_ROW_PTR_IS_NULL(_rp1) \
  ((memcmp((_rp1), &dsStdMibZeroDotZero, sizeof(L7_USMDB_MIB_DIFFSERV_ROW_PTR_t)) == 0) \
    ? L7_TRUE : L7_FALSE)
  

#define DSSTDMIB_PRVTMIB_CTR_ID_NONE    0
#define DSSTDMIB_ID_TRACE_INIT_VAL      ((L7_uint32)0xFFFFFFFF)

#define DSSTDMIB_SHAPE_AVERAGE          1
#define DSSTDMIB_SHAPE_PEAK             2

#define DSSTDMIB_1K                     1024

#define DSSTDMIB_TB_PARAM_INTERVAL_MIN          1     /* microseconds */

#define DSSTDMIB_CLFR_ELEM_PRECEDENCE_ALLEXCL   1
#define DSSTDMIB_CLFR_ELEM_PRECEDENCE_ALL       2
#define DSSTDMIB_CLFR_ELEM_PRECEDENCE_ANY       3


/* Enums for Set Test OIDs */
                                      
/* Enumerated values for Data Path set-test object ids */
typedef enum
{
  L7_DIFFSERV_DATA_PATH_SETTEST_OID_START = 1,
  L7_DIFFSERV_DATA_PATH_SETTEST_OID_STORAGE,
  L7_DIFFSERV_DATA_PATH_SETTEST_OID_ROW_STATUS

} L7_DIFFSERV_DATA_PATH_SETTEST_OID_TYPE_t;

/* Enumerated values for Classifier set-test object ids */
typedef enum
{
  L7_DIFFSERV_CLFR_SETTEST_OID_STORAGE = 1,
  L7_DIFFSERV_CLFR_SETTEST_OID_ROW_STATUS

} L7_DIFFSERV_CLFR_SETTEST_OID_TYPE_t;

/* Enumerated values for Classifier Element set-test object ids */
typedef enum
{
  L7_DIFFSERV_CLFR_ELEMENT_SETTEST_OID_PRECEDENCE = 1,
  L7_DIFFSERV_CLFR_ELEMENT_SETTEST_OID_NEXT,
  L7_DIFFSERV_CLFR_ELEMENT_SETTEST_OID_SPECIFIC,
  L7_DIFFSERV_CLFR_ELEMENT_SETTEST_OID_STORAGE,
  L7_DIFFSERV_CLFR_ELEMENT_SETTEST_OID_ROW_STATUS

} L7_DIFFSERV_CLFR_ELEMENT_SETTEST_OID_TYPE_t;

/* Enumerated values for IP Multi-field Classification set-test object ids */
typedef enum
{
  L7_DIFFSERV_MULTI_FIELD_CLFR_SETTEST_OID_ADDR_TYPE = 1,
  L7_DIFFSERV_MULTI_FIELD_CLFR_SETTEST_OID_DST_ADDR,
  L7_DIFFSERV_MULTI_FIELD_CLFR_SETTEST_OID_DST_PREFIX_LENGTH,
  L7_DIFFSERV_MULTI_FIELD_CLFR_SETTEST_OID_SRC_ADDR,
  L7_DIFFSERV_MULTI_FIELD_CLFR_SETTEST_OID_SRC_PREFIX_LENGTH,
  L7_DIFFSERV_MULTI_FIELD_CLFR_SETTEST_OID_DSCP,
  L7_DIFFSERV_MULTI_FIELD_CLFR_SETTEST_OID_FLOW_ID,
  L7_DIFFSERV_MULTI_FIELD_CLFR_SETTEST_OID_PROTOCOL,
  L7_DIFFSERV_MULTI_FIELD_CLFR_SETTEST_OID_DSTL4PORT_MIN,
  L7_DIFFSERV_MULTI_FIELD_CLFR_SETTEST_OID_DSTL4PORT_MAX,
  L7_DIFFSERV_MULTI_FIELD_CLFR_SETTEST_OID_SRCL4PORT_MIN,
  L7_DIFFSERV_MULTI_FIELD_CLFR_SETTEST_OID_SRCL4PORT_MAX,
  L7_DIFFSERV_MULTI_FIELD_CLFR_SETTEST_OID_STORAGE,
  L7_DIFFSERV_MULTI_FIELD_CLFR_SETTEST_OID_ROW_STATUS

} L7_DIFFSERV_MULTI_FIELD_CLFR_SETTEST_OID_TYPE_t;

/* Enumerated values for Meter set-test object ids */
typedef enum
{
  L7_DIFFSERV_METER_SETTEST_OID_SUCCEED_NEXT = 1,
  L7_DIFFSERV_METER_SETTEST_OID_FAIL_NEXT,
  L7_DIFFSERV_METER_SETTEST_OID_SPECIFIC,
  L7_DIFFSERV_METER_SETTEST_OID_STORAGE,
  L7_DIFFSERV_METER_SETTEST_OID_ROW_STATUS

} L7_DIFFSERV_METER_SETTEST_OID_TYPE_t;


/* Enumerated values for Token Bucket Parameter set-test object ids */
typedef enum
{
  L7_DIFFSERV_TB_PARAM_SETTEST_OID_TYPE = 1,
  L7_DIFFSERV_TB_PARAM_SETTEST_OID_RATE,
  L7_DIFFSERV_TB_PARAM_SETTEST_OID_BURST_SIZE,
  L7_DIFFSERV_TB_PARAM_SETTEST_OID_INTERVAL,
  L7_DIFFSERV_TB_PARAM_SETTEST_OID_STORAGE,
  L7_DIFFSERV_TB_PARAM_SETTEST_OID_ROW_STATUS

} L7_DIFFSERV_TB_PARAM_SETTEST_OID_TYPE_t;

/* Enumerated values for Action set-test object ids */
typedef enum
{
  L7_DIFFSERV_ACTION_SETTEST_OID_INTERFACE = 1,
  L7_DIFFSERV_ACTION_SETTEST_OID_NEXT,
  L7_DIFFSERV_ACTION_SETTEST_OID_SPECIFIC,
  L7_DIFFSERV_ACTION_SETTEST_OID_STORAGE,
  L7_DIFFSERV_ACTION_SETTEST_OID_ROW_STATUS

} L7_DIFFSERV_ACTION_SETTEST_OID_TYPE_t;

/* Enumerated values for Count Action set-test object ids */
typedef enum
{
  L7_DIFFSERV_COUNT_ACT_SETTEST_OID_OCTETS = 1,
  L7_DIFFSERV_COUNT_ACT_SETTEST_OID_PKTS,
  L7_DIFFSERV_COUNT_ACT_SETTEST_OID_STORAGE,
  L7_DIFFSERV_COUNT_ACT_SETTEST_OID_ROW_STATUS

} L7_DIFFSERV_COUNT_ACT_SETTEST_OID_TYPE_t;

/* Enumerated values for Assign Queue set-test object ids */
typedef enum
{
  L7_DIFFSERV_ASSIGN_QUEUE_SETTEST_OID_QNUM = 1,
  L7_DIFFSERV_ASSIGN_QUEUE_SETTEST_OID_STORAGE,
  L7_DIFFSERV_ASSIGN_QUEUE_SETTEST_OID_ROW_STATUS

} L7_DIFFSERV_ASSIGN_QUEUE_SETTEST_OID_TYPE_t;

/* Enumerated values for Color Aware set-test object ids */
typedef enum
{
  L7_DIFFSERV_COLOR_AWARE_SETTEST_OID_LEVEL = 1,
  L7_DIFFSERV_COLOR_AWARE_SETTEST_OID_MODE,
  L7_DIFFSERV_COLOR_AWARE_SETTEST_OID_VALUE

} L7_DIFFSERV_COLOR_AWARE_SETTEST_OID_TYPE_t;

/* Enumerated values for Redirect set-test object ids */
typedef enum
{
  L7_DIFFSERV_REDIRECT_SETTEST_OID_INTF = 1,
  L7_DIFFSERV_REDIRECT_SETTEST_OID_STORAGE,
  L7_DIFFSERV_REDIRECT_SETTEST_OID_ROW_STATUS

} L7_DIFFSERV_REDIRECT_SETTEST_OID_TYPE_t;

/* Enumerated values for Mirror set-test object ids */
typedef enum
{
  L7_DIFFSERV_MIRROR_SETTEST_OID_INTF = 1,
  L7_DIFFSERV_MIRROR_SETTEST_OID_STORAGE,
  L7_DIFFSERV_MIRROR_SETTEST_OID_ROW_STATUS

} L7_DIFFSERV_MIRROR_SETTEST_OID_TYPE_t;

/* Enumerated values for Algorithmic Drop set-test object ids */
typedef enum
{
  L7_DIFFSERV_ALG_DROP_SETTEST_OID_TYPE = 1,
  L7_DIFFSERV_ALG_DROP_SETTEST_OID_NEXT,
  L7_DIFFSERV_ALG_DROP_SETTEST_OID_QMEASURE,
  L7_DIFFSERV_ALG_DROP_SETTEST_OID_QTHRESHOLD,
  L7_DIFFSERV_ALG_DROP_SETTEST_OID_SPECIFIC,
  L7_DIFFSERV_ALG_DROP_SETTEST_OID_STORAGE,
  L7_DIFFSERV_ALG_DROP_SETTEST_OID_ROW_STATUS

} L7_DIFFSERV_ALG_DROP_SETTEST_OID_TYPE_t;

/* Enumerated values for Random Drop set-test object ids */
typedef enum
{
  L7_DIFFSERV_RANDOMDROP_SETTEST_OID_MINTHRESH_BYTES = 1,
  L7_DIFFSERV_RANDOMDROP_SETTEST_OID_MINTHRESH_PKTS,
  L7_DIFFSERV_RANDOMDROP_SETTEST_OID_MAXTHRESH_BYTES,
  L7_DIFFSERV_RANDOMDROP_SETTEST_OID_MAXTHRESH_PKTS,
  L7_DIFFSERV_RANDOMDROP_SETTEST_OID_PROBMAX,
  L7_DIFFSERV_RANDOMDROP_SETTEST_OID_WEIGHT,
  L7_DIFFSERV_RANDOMDROP_SETTEST_OID_SAMPLINGRATE,
  L7_DIFFSERV_RANDOMDROP_SETTEST_OID_STORAGE,
  L7_DIFFSERV_RANDOMDROP_SETTEST_OID_ROW_STATUS

} L7_DIFFSERV_RANDOMDROP_SETTEST_OID_TYPE_t;

/* Enumerated values for Queue set-test object ids */
typedef enum
{
  L7_DIFFSERV_Q_SETTEST_OID_NEXT = 1,
  L7_DIFFSERV_Q_SETTEST_OID_MINRATE,
  L7_DIFFSERV_Q_SETTEST_OID_MAXRATE,
  L7_DIFFSERV_Q_SETTEST_OID_STORAGE,
  L7_DIFFSERV_Q_SETTEST_OID_ROW_STATUS

} L7_DIFFSERV_Q_SETTEST_OID_TYPE_t;

/* Enumerated values for Scheduler set-test object ids */
typedef enum
{
  L7_DIFFSERV_SCHEDULER_SETTEST_OID_NEXT = 1,
  L7_DIFFSERV_SCHEDULER_SETTEST_OID_METHOD,
  L7_DIFFSERV_SCHEDULER_SETTEST_OID_MINRATE,
  L7_DIFFSERV_SCHEDULER_SETTEST_OID_MAXRATE,
  L7_DIFFSERV_SCHEDULER_SETTEST_OID_STORAGE,
  L7_DIFFSERV_SCHEDULER_SETTEST_OID_ROW_STATUS

} L7_DIFFSERV_SCHEDULER_SETTEST_OID_TYPE_t;

/* Enumerated values for Minimum Rate Parameter set-test object ids */
typedef enum
{
  L7_DIFFSERV_MINRATE_SETTEST_OID_PRIORITY = 1,
  L7_DIFFSERV_MINRATE_SETTEST_OID_ABSOLUTE,
  L7_DIFFSERV_MINRATE_SETTEST_OID_RELATIVE,
  L7_DIFFSERV_MINRATE_SETTEST_OID_STORAGE,
  L7_DIFFSERV_MINRATE_SETTEST_OID_ROW_STATUS

} L7_DIFFSERV_MINRATE_SETTEST_OID_TYPE_t;

/* Enumerated values for Maximum Rate Parameter set-test object ids */
typedef enum
{
  L7_DIFFSERV_MAXRATE_SETTEST_OID_ABSOLUTE = 1,
  L7_DIFFSERV_MAXRATE_SETTEST_OID_RELATIVE,
  L7_DIFFSERV_MAXRATE_SETTEST_OID_THRESHOLD,
  L7_DIFFSERV_MAXRATE_SETTEST_OID_STORAGE,
  L7_DIFFSERV_MAXRATE_SETTEST_OID_ROW_STATUS

} L7_DIFFSERV_MAXRATE_SETTEST_OID_TYPE_t;


/****************************************
 *
 * Data Path Table
 *
 ****************************************
 */

typedef L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t    dsStdMibDataPathIfDir_t;

/* MIB entry definition */
typedef struct 
{
  dsStdMibDataPathIfDir_t ifDirection;          /* intf direction             */
  dsStdMibRowPtr_t        start;                /* first data path element    */
  dsStdMibStorageType_t   storage;              /* type of config storage     */
  dsStdMibRowStatus_t     status;               /* status of this row         */

}dsStdMibDataPathEntry_t;

/* Key structure */
typedef struct
{
  L7_uint32               ifIndex;              /* intf index                 */ 
  dsStdMibDataPathIfDir_t ifDirection;          /* intf direction             */

}dsStdMibDataPathKey_t;

/* MIB controls */
typedef struct
{ 
  dsStdMibDataPathKey_t   key;                  /* AVL KEYS                   */

  dsStdMibTableId_t       tableId;              /* Table Id                   */

  /* MIB fields */
  dsStdMibDataPathEntry_t mib;                  /* MIB object definitions     */
  
  /* NOTE:  The AVL utility REQUIRES a next ptr as the LAST element */
  void                    *next;                /* (RESERVED FOR AVL USE)     */

}dsStdMibDataPathEntryCtrl_t;


/****************************************
 *
 * Classifier Table
 *
 ****************************************
 */

/* MIB entry definition */
typedef struct
{
  L7_uint32               id;                   /* classifier ID              */
  dsStdMibStorageType_t   storage;              /* type of config storage     */
  dsStdMibRowStatus_t     status;               /* status of this row         */

} dsStdMibClfrEntry_t;

/* Key structure */
typedef struct
{
  L7_uint32               clfrId;               /* classifier ID              */

}dsStdMibClfrKey_t;

/* MIB controls */
typedef struct
{
  dsStdMibClfrKey_t       key;                  /* AVL KEY                    */ 

  dsStdMibTableId_t       tableId;              /* Table Id                   */
  L7_uint32               precedenceAllExcl;    /* 'all' exclude rule prec.   */ 
  L7_uint32               precedenceAll;        /* 'all' non-excl rule prec.  */ 
  L7_uint32               precedenceAny;        /* 'any' rule prec.           */ 

  /* MIB fields */
  dsStdMibClfrEntry_t     mib;                  /* MIB object definitions     */

  /* NOTE:  The AVL utility REQUIRES a next ptr as the LAST element */
  void                    *next;                /* (RESERVED FOR AVL USE)     */

} dsStdMibClfrEntryCtrl_t;


/****************************************
 *
 * Classifier Element Table
 *
 ****************************************
 */

/* MIB entry definition */
typedef struct
{
  L7_uint32                   id;               /* classifier element id      */
  L7_uint32                   precedence;       /* element precedence         */
  dsStdMibRowPtr_t            next;             /* next data path element     */
  dsStdMibRowPtr_t            specific;         /* specific entry in another tbl*/
  dsStdMibStorageType_t       storage;          /* type of config storage     */
  dsStdMibRowStatus_t         status;           /* status of this row         */

} dsStdMibClfrElementEntry_t;

/* Key structure */
typedef struct 
{
  L7_uint32                   clfrId;           /* clfr Id                    */
  L7_uint32                   clfrElemId;       /* clfr element Id            */

}dsStdMibClfrElementKey_t;

/* MIB controls */
typedef struct
{
  dsStdMibClfrElementKey_t    key;              /* AVL KEY                    */
  
  dsStdMibTableId_t           tableId;          /* Table Id                   */

  /* MIB fields */
  dsStdMibClfrElementEntry_t  mib;              /* MIB object definitions     */

  /* NOTE:  The AVL utility REQUIRES a next ptr as the LAST element */
  void                        *next;            /* (RESERVED FOR AVL USE)     */

} dsStdMibClfrElementEntryCtrl_t;


/****************************************
 *
 * IP Multi Field Classifier Table
 *
 ****************************************
 */

/* MIB entry definition */
typedef struct
{
  L7_uint32               id;                   /* multi field clfr Id        */
  dsStdMibInetAddrType_t  addrType;             /* address type               */
  L7_uint32               dstAddr;              /* destination address        */
  L7_uint32               dstPrefixLen;         /* destination prefix length  */
  L7_uint32               srcAddr;              /* source address             */
  L7_uint32               srcPrefixLen;         /* source prefix length       */
  L7_int32                dscp;                 /* DSCP value (-1=any)        */
  L7_uint32               flowId;               /* flow identifier            */
  L7_uint32               protocol;             /* IP protocol number(255=all)*/
  L7_uint32               dstL4PortMin;         /* destination port minimum   */
  L7_uint32               dstL4PortMax;         /* destination port maximum   */
  L7_uint32               srcL4PortMin;         /* source port minimum        */
  L7_uint32               srcL4PortMax;         /* source port maximum        */
  dsStdMibStorageType_t   storage;              /* type of config storage     */
  dsStdMibRowStatus_t     status;               /* status of this row         */

} dsStdMibMultiFieldClfrEntry_t;

/* Key Structure */
typedef struct
{
  L7_uint32               multiFieldClfrId;     /* multi field clfr Id        */

} dsStdMibMultiFieldClfrKey_t;

/* MIB controls */
typedef struct
{
  dsStdMibMultiFieldClfrKey_t   key;            /* AVL KEY                    */

  dsStdMibTableId_t             tableId;        /* Table Id                   */

  /* MIB fields */
  dsStdMibMultiFieldClfrEntry_t mib;            /* MIB object definitions     */

  /* NOTE:  The AVL utility REQUIRES a next ptr as the LAST element */
  void                          *next;          /* (RESERVED FOR AVL USE)     */

} dsStdMibMultiFieldClfrEntryCtrl_t;


/*****************************************
 *
 * Auxiliary Multi Field Classifier Table
 *
 *****************************************
 */

/* MIB entry definition */
typedef struct
{
  L7_uint32                 id;                 /* Aux multi-field clfr id    */
  L7_uint32                 dstAddr;            /* destination address        */
  L7_uint32                 dstMask;            /* destination mask           */
  L7_uint32                 srcAddr;            /* source address             */
  L7_uint32                 srcMask;            /* source mask                */
  L7_uint32                 protocol;           /* IP protocol number(255=all)*/
  L7_uint32                 dstL4PortMin;       /* destination port minimum   */
  L7_uint32                 dstL4PortMax;       /* destination port maximum   */
  L7_uint32                 srcL4PortMin;       /* source port minimum        */
  L7_uint32                 srcL4PortMax;       /* source port maximum        */
  L7_int32                  cos;                /* class of service (-1=all)  */
  L7_int32                  cos2;               /* secondary COS (-1=all)     */
  L7_uint32                 etypeVal1;          /* Ethertype value1(0=inactive)*/
  L7_uint32                 etypeVal2;          /* Ethertype value2(0=inactive)*/
  L7_in6_addr_t             ipv6DstAddr;        /* IPv6 destination address    */
  L7_uint32                 ipv6DstPlen;        /* IPv6 dest prefix length    */
  L7_uint32                 ipv6FlowLabel;      /* IPv6 Flow Label            */
  L7_in6_addr_t             ipv6SrcAddr;        /* IPv6 source address        */
  L7_uint32                 ipv6SrcPlen;        /* IPv6 src prefix length     */
  L7_uchar8                 tos;                /* IP TOS bits value          */
  L7_uchar8                 tosMask;            /* IP TOS bits mask value     */
  L7_uchar8                 rsvd1[2];           /* (rsvd -- for alignment)    */
  L7_uchar8                 dstMac[L7_MAC_ADDR_LEN];  /* MAC address          */
  L7_uchar8                 dstMacMask[L7_MAC_ADDR_LEN];  /* MAC address mask */
  L7_uchar8                 srcMac[L7_MAC_ADDR_LEN];  /* MAC address          */
  L7_uchar8                 srcMacMask[L7_MAC_ADDR_LEN];  /* MAC address mask */
  L7_uint32                 vlanIdMin;          /* VLAN id minimum            */
  L7_uint32                 vlanIdMax;          /* VLAN id maximum            */
  L7_uint32                 vlanId2Min;         /* secondary VLAN id minimum  */
  L7_uint32                 vlanId2Max;         /* secondary VLAN id maximum  */
  dsStdMibStorageType_t     storage;            /* type of config storage     */
  dsStdMibRowStatus_t       status;             /* status of this row         */

}dsStdMibAuxMFClfrEntry_t;

/* Key Structure */
typedef struct
{
  L7_uint32                 auxMFClfrId;        /* Aux multi field clfr Id    */

} dsStdMibAuxMFClfrKey_t;

/* MIB controls */
typedef struct
{
  dsStdMibAuxMFClfrKey_t    key;                /* AVL KEY                    */

  dsStdMibTableId_t         tableId;            /* Table Id                   */

  /* MIB fields */
  dsStdMibAuxMFClfrEntry_t  mib;                /* MIB object definitions     */

  /* reuse control fields */
  L7_uint32                 classIndex;         /* class idx from prvt MIB    */
  L7_uint32                 classRuleIndex;     /* class rule idx from prvt MIB*/

  /* NOTE:  The AVL utility REQUIRES a next ptr as the LAST element */
  void                      *next;              /* (RESERVED FOR AVL USE)     */

} dsStdMibAuxMFClfrEntryCtrl_t;

/* the following structure is used to reuse common Aux MF Clfr entries */
typedef struct
{
  L7_uint32                 auxMFClfrId;        /* Aux Id used for this class */
  L7_uint32                 refCount;           /* Aux Id reference count     */
} dsStdMibAuxMFClfrReuse_t;


/****************************************
 *
 * Meter Table
 * Color Aware Table (augments Meter Table)
 *
 ****************************************
 */

typedef L7_USMDB_MIB_DIFFSERV_COLOR_LEVEL_t     dsStdMibColorLevel_t;
typedef L7_USMDB_MIB_DIFFSERV_COLOR_MODE_t      dsStdMibColorMode_t;

/* MIB entry definition */
typedef struct
{
  L7_uint32                   id;               /* meter Id                   */
  dsStdMibRowPtr_t            succeedNext;      /* next data path element, if */
                                                /* traffic conforms           */
  dsStdMibRowPtr_t            failNext;         /* next data path element, if */
                                                /* traffic does not conform   */
  dsStdMibRowPtr_t            specific;         /* specific data path element */
  dsStdMibStorageType_t       storage;          /* type of config storage     */
  dsStdMibRowStatus_t         status;           /* status of this row         */

} dsStdMibMeterEntry_t;

/* MIB entry definition */
typedef struct
{
  dsStdMibColorLevel_t        level;            /* entry type (conform,exceed)*/
  dsStdMibColorMode_t         mode;             /* color mode (blind, aware..)*/
  L7_uint32                   value;            /* color-aware field value    */

} dsStdMibColorAwareEntry_t;

/* Key structure */
typedef struct
{
  L7_uint32                   meterId;          /* meter Id                   */

} dsStdMibMeterKey_t;

/* MIB controls */
typedef struct
{
  dsStdMibMeterKey_t          key;              /* AVL KEY */

  dsStdMibTableId_t           tableId;          /* Table Id                   */

  /* MIB fields */
  dsStdMibMeterEntry_t        mib;              /* MIB object definitions     */
  dsStdMibColorAwareEntry_t   mibColor;         /* MIB color aware definitions*/
  
  /* NOTE:  The AVL utility REQUIRES a next ptr as the LAST element */
  void                        *next;            /* (RESERVED FOR AVL USE)     */

} dsStdMibMeterEntryCtrl_t;


/****************************************
 *
 * Token Bucket Parameter Table
 *
 ****************************************
 */

typedef L7_USMDB_MIB_DIFFSERV_TB_METER_TYPE_t  dsStdMibTBMeterType_t;

/* MIB entry definition */
typedef struct
{
  L7_uint32               id;                   /* TB param  Id               */
  dsStdMibTBMeterType_t   type;                 /* meter type                 */
  L7_uint32               rate;                 /* rate in kbps               */
  L7_uint32               burstSize;            /* max num bytes in tx burst  */
  L7_uint32               interval;             /* time intvl used with the TB*/
  dsStdMibStorageType_t   storage;              /* type of config storage     */
  dsStdMibRowStatus_t     status;               /* status of this row         */

} dsStdMibTBParamEntry_t;

/* Key Structure */
typedef struct
{
  L7_uint32               tbParamId;            /* meter Id                   */

} dsStdMibTBParamKey_t;

/* MIB controls */
typedef struct
{
  dsStdMibTBParamKey_t    key;                  /* AVL KEY                    */

  dsStdMibTableId_t       tableId;              /* Table Id                   */

  /* MIB fields */
  dsStdMibTBParamEntry_t  mib;                  /* MIB object definitions     */

  /* NOTE:  The AVL utility REQUIRES a next ptr as the LAST element */
  void                    *next;                /* (RESERVED FOR AVL USE)     */

} dsStdMibTBParamEntryCtrl_t;


/****************************************
 *
 * Action Table
 *
 ****************************************
 */

/* MIB entry definition */
typedef struct
{
  L7_uint32               id;                   /* action Id                  */
  L7_uint32               interface;            /* ifindex this action occurs */
  dsStdMibRowPtr_t        next;                 /* next data path element     */
  dsStdMibRowPtr_t        specific;             /* specific data path element */
  dsStdMibStorageType_t   storage;              /* type of config storage     */
  dsStdMibRowStatus_t     status;               /* status of this row         */

} dsStdMibActionEntry_t;

/* Key Structure */
typedef struct
{
  L7_uint32               actionId;             /* action Id                  */

} dsStdMibActionKey_t;

/* MIB controls */
typedef struct
{
  dsStdMibActionKey_t     key;                  /* AVL KEY                    */

  dsStdMibTableId_t       tableId;              /* Table Id                   */

  /* MIB fields */
  dsStdMibActionEntry_t   mib;                  /* MIB object definitions     */

  /* NOTE:  The AVL utility REQUIRES a next ptr as the LAST element */
  void                    *next;                /* (RESERVED FOR AVL USE)     */

} dsStdMibActionEntryCtrl_t;


/****************************************
 *
 *  Count Action Table
 *
 ****************************************
 */

/* private MIB Policy Performance In/Out object get function type */
typedef L7_RC_t (*fpPerfObjectGet_t)(L7_uint32 policyIndex, 
                                     L7_uint32 policyInstIndex, 
                                     L7_uint32 intIfNum, 
                                     L7_DIFFSERV_POLICY_PERF_OUT_TABLE_OBJECT_t oid,
                                     void *pValue);

/* MIB entry definition */
typedef struct
{
  L7_uint32                 id;                 /* count action id            */
  L7_ulong64                octets;             /* num of octets at action    */
                                                /* data path element          */
  L7_ulong64                pkts;               /* num of pkts at action      */
                                                /* data path element          */
  dsStdMibStorageType_t     storage;            /* type of config storage     */
  dsStdMibRowStatus_t       status;             /* status of this row         */

} dsStdMibCountActEntry_t;

/* Key structure */
typedef struct
{
  L7_uint32                 countActId;         /* action Id                  */

} dsStdMibCountActKey_t;

/* Counter source info (to retrieve updated count value from private MIB) */
typedef struct
{
  DSSTDMIB_COUNT_TYPE_t     countType;          /* std MIB counter type       */
  fpPerfObjectGet_t         fpPerfObjectGet;    /* prvt MIB counter obj get fn*/
  L7_uint32                 policyIndex;        /* prvt MIB policy index      */
  L7_uint32                 policyInstIndex;    /* prvt MIB policy inst index */
  L7_uint32                 intIfNum;           /* prvt MIB internal intf num */
  L7_uint32                 octetsCtrId;        /* prvt MIB octets counter ID */
  L7_uint32                 pktsCtrId;          /* prvt MIB packets counter ID*/

} dsStdMibCountActCtrSource_t;

/* MIB controls */
typedef struct
{
  dsStdMibCountActKey_t     key;                /* AVL KEY                    */
  
  dsStdMibTableId_t         tableId;            /* Table Id                   */
  dsStdMibCountActCtrSource_t ctrSource;        /* counter source info        */

  /* MIB fields */
  dsStdMibCountActEntry_t   mib;                /* MIB object definitions     */

  /* NOTE:  The AVL utility REQUIRES a next ptr as the LAST element */
  void                      *next;              /* (RESERVED FOR AVL USE)     */

} dsStdMibCountActEntryCtrl_t;


/****************************************
 *
 * Assign Queue Table
 *
 ****************************************
 */

/* MIB entry definition */
typedef struct
{
  L7_uint32                   id;               /* assign queue id (row)      */
  L7_uint32                   qNum;             /* queue num flow is assigned */
  dsStdMibStorageType_t       storage;          /* type of config storage     */
  dsStdMibRowStatus_t         status;           /* status of this row         */

} dsStdMibAssignQueueEntry_t;

/* Key Structure */
typedef struct
{
  L7_uint32                   assignId;         /* assign queue id (row)      */

} dsStdMibAssignQueueKey_t;

/* MIB controls */
typedef struct
{
  dsStdMibAssignQueueKey_t    key;              /* AVL KEY                    */

  dsStdMibTableId_t           tableId;          /* Table Id                   */

  /* MIB fields */
  dsStdMibAssignQueueEntry_t  mib;              /* MIB object definitions     */

  /* NOTE:  The AVL utility REQUIRES a next ptr as the LAST element */
  void                        *next;            /* (RESERVED FOR AVL USE)     */

} dsStdMibAssignQueueEntryCtrl_t;


/****************************************
 *
 * Redirect Table
 *
 ****************************************
 */

/* MIB entry definition */
typedef struct
{
  L7_uint32                   id;               /* redirect id                */
  L7_uint32                   intIfNum;         /* interface to fwd traffic   */
  dsStdMibStorageType_t       storage;          /* type of config storage     */
  dsStdMibRowStatus_t         status;           /* status of this row         */

} dsStdMibRedirectEntry_t;

/* Key Structure */
typedef struct
{
  L7_uint32                   redirectId;       /* redirect Id                */

} dsStdMibRedirectKey_t;

/* MIB controls */
typedef struct
{
  dsStdMibRedirectKey_t       key;              /* AVL KEY                    */

  dsStdMibTableId_t           tableId;          /* Table Id                   */

  /* MIB fields */
  dsStdMibRedirectEntry_t     mib;              /* MIB object definitions     */

  /* NOTE:  The AVL utility REQUIRES a next ptr as the LAST element */
  void                        *next;            /* (RESERVED FOR AVL USE)     */

} dsStdMibRedirectEntryCtrl_t;


/****************************************
 *
 * Mirror Table
 *
 ****************************************
 */

/* MIB entry definition */
typedef struct
{
  L7_uint32                   id;               /* mirror id                  */
  L7_uint32                   intIfNum;         /* interface to fwd traffic   */
  dsStdMibStorageType_t       storage;          /* type of config storage     */
  dsStdMibRowStatus_t         status;           /* status of this row         */

} dsStdMibMirrorEntry_t;

/* Key Structure */
typedef struct
{
  L7_uint32                   mirrorId;         /* mirror Id                  */

} dsStdMibMirrorKey_t;

/* MIB controls */
typedef struct
{
  dsStdMibMirrorKey_t         key;              /* AVL KEY                    */

  dsStdMibTableId_t           tableId;          /* Table Id                   */

  /* MIB fields */
  dsStdMibMirrorEntry_t       mib;              /* MIB object definitions     */

  /* NOTE:  The AVL utility REQUIRES a next ptr as the LAST element */
  void                        *next;            /* (RESERVED FOR AVL USE)     */

} dsStdMibMirrorEntryCtrl_t;


/****************************************
 *
 * Algorithmic Drop Table
 *
 ****************************************
 */

typedef L7_USMDB_MIB_DIFFSERV_ALG_DROP_TYPE_t dsStdMibAlgDropType_t;

/* MIB entry definition */
typedef struct
{
  L7_uint32               id;                   /* algorithmic drop Id        */
  dsStdMibAlgDropType_t   type;                 /* type of algorithm          */
  dsStdMibRowPtr_t        next;                 /* next data path element     */
  dsStdMibRowPtr_t        qMeasure;             /* pointer to an entry in the */
                                                /*  diffServQTable            */
  L7_uint32               qThreshold;           /* threshold on the depth     */
  dsStdMibRowPtr_t        specific;             /* pointer to further drop alg*/
  L7_ulong64              octets;               /* num of octets that have been*/ 
                                                /*  deterministically dropped  */
  L7_ulong64              pkts;                 /* num of pkts that have been */
                                                /*  deterministically dropped */
  L7_ulong64              randomDropOctets;     /* num of octets that have been*/
                                                /*  randomly dropped           */
  L7_ulong64              randomDropPkts;       /* num of pkts that have been */
                                                /*  randomly dropped          */
  dsStdMibStorageType_t   storage;              /* type of config storage     */
  dsStdMibRowStatus_t     status;               /* status of this row         */

} dsStdMibAlgDropEntry_t;

/* Key structure*/
typedef struct
{
  L7_uint32               algDropId;            /* alg drop id                */

} dsStdMibAlgDropKey_t;

/* Counter source info (to retrieve updated count value from private MIB) */
typedef struct
{
  L7_uint32               policyIndex;          /* prvt MIB policy index      */
  L7_uint32               policyInstIndex;      /* prvt MIB policy inst index */
  L7_uint32               policyAttrIndex;      /* prvt MIB policy attr index */
  L7_uint32               intIfNum;             /* prvt MIB internal intf num */

} dsStdMibAlgDropCtrSource_t;

/* MIB controls */
typedef struct
{
  dsStdMibAlgDropKey_t    key;                  /* AVL Key                    */

  dsStdMibTableId_t       tableId;              /* Table Id                   */
  dsStdMibAlgDropCtrSource_t  ctrSource;        /* counter source info        */

  /* MIB fields */
  dsStdMibAlgDropEntry_t  mib;                  /* MIB object definitions     */

  /* NOTE:  The AVL utility REQUIRES a next ptr as the LAST element */
  void                    *next;                /* (RESERVED FOR AVL USE)     */

} dsStdMibAlgDropEntryCtrl_t;



/* external references */
extern avlTree_t dsStdMibAvlTree[];
extern void      *dsStdMibSemId;   
extern dsStdMibRowPtr_t dsStdMibZeroDotZero;

/* Function prototypes */
/* diffserv_stdmib.c */
L7_RC_t dsStdMibAvlCreate(void);
L7_RC_t dsStdMibAvlTreeCreate(avlTree_t *pTree, dsStdMibAvlCtrl_t *pCtrl);
void    dsStdMibAvlDelete(void);
void    dsStdMibAvlTreeDelete(avlTree_t *pTree, dsStdMibAvlCtrl_t *pCtrl);
void    dsStdMibAvlPurge(void);
void    dsStdMibGroupInit(void);
L7_RC_t dsStdMibInBoundDataPathCreate(L7_uint32 intIfNum,
                                      L7_uint32 clfrId);
L7_RC_t dsStdMibPathInstanceCreate(L7_uint32 policyIndex, 
                                          L7_uint32 policyInstIndex,
                                          L7_uint32 intIfNum,
                                          L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection,
                                          L7_uint32 clfrId,
                                          dsStdMibRowPtr_t *pPathNext);
L7_RC_t dsStdMibMarkerAttrExists(L7_uint32 policyIndex, 
                                 L7_uint32 policyInstIndex,
                                 L7_uint32 *pPolicyAttrIndex);
L7_RC_t dsStdMibPolicerAttrExists(L7_uint32 policyIndex, 
                                  L7_uint32 policyInstIndex,
                                  L7_uint32 *pPolicyAttrIndex);
L7_RC_t dsStdMibPathAttrExists(L7_uint32 policyIndex, 
                               L7_uint32 policyInstIndex,
                               dsmibPolicyAttrType_t policyAttrType,
                               L7_uint32 *pPolicyAttrIndex);
L7_RC_t dsStdMibRedirectElemCreate(L7_uint32 policyIndex, 
                                   L7_uint32 policyInstIndex,
                                   L7_uint32 policyAttrIndex,
                                   L7_uint32 intIfNum,
                                   dsStdMibRowPtr_t *pPathNext);
L7_RC_t dsStdMibMirrorElemCreate(L7_uint32 policyIndex, 
                                 L7_uint32 policyInstIndex,
                                 L7_uint32 policyAttrIndex,
                                 L7_uint32 intIfNum,
                                 dsStdMibRowPtr_t *pPathNext);
L7_RC_t dsStdMibAssignQueueElemCreate(L7_uint32 policyIndex, 
                                      L7_uint32 policyInstIndex,
                                      L7_uint32 policyAttrIndex,
                                      L7_uint32 intIfNum,
                                      dsStdMibRowPtr_t *pPathNext);
L7_RC_t dsStdMibInDropElemCreate(L7_uint32 policyIndex, 
                                 L7_uint32 policyInstIndex,
                                 L7_uint32 policyAttrIndex,
                                 L7_uint32 intIfNum,
                                 dsStdMibRowPtr_t *pPathNext);
L7_RC_t dsStdMibMarkerElemCreate(L7_uint32 policyIndex, 
                                 L7_uint32 policyInstIndex,
                                 L7_uint32 policyAttrIndex,
                                 L7_uint32 intIfNum,
                                 dsStdMibRowPtr_t *pPathNext);
L7_RC_t dsStdMibPolicerElemCreate(L7_uint32 policyIndex, 
                                  L7_uint32 policyInstIndex,
                                  L7_uint32 policyAttrIndex,
                                  L7_uint32 intIfNum,
                                  dsStdMibRowPtr_t *pPathNext);
L7_RC_t dsStdMibPoliceNextElementCreate(L7_uint32 policyIndex, 
                                        L7_uint32 policyInstIndex, 
                                        L7_uint32 policyAttrIndex,
                                        L7_uint32 intIfNum,
                                        L7_DIFFSERV_POLICY_ATTR_TABLE_OBJECT_t actionOid,
                                        L7_DIFFSERV_POLICY_ATTR_TABLE_OBJECT_t valueOid,
                                        dsStdMibRowPtr_t *pMeterNext);
L7_RC_t dsStdMibCounterElemCreate(L7_uint32 policyIndex, L7_uint32 policyInstIndex, 
                                  L7_uint32 intIfNum, 
                                  DSSTDMIB_COUNT_TYPE_t countType, 
                                  dsStdMibRowPtr_t *pPathNext);
L7_RC_t dsStdMibClfrElemCreate(L7_uint32 policyIndex, L7_uint32 policyInstIndex,
                               L7_uint32 clfrId, dsStdMibRowPtr_t *pPathNext);
L7_RC_t dsStdMibClfrElementsCreate(L7_uint32 classIndex,
                                   L7_uint32 clfrId,
                                   dsStdMibRowPtr_t *pPathNext);
L7_RC_t dsStdMibClfrElementsBuild(dsStdMibAuxMFClfrEntryCtrl_t *pAuxInfo,
                                  dsmibClassType_t classType, 
                                  dsmibTruthValue_t excludeFlag,
                                  L7_uint32 clfrId,
                                  dsStdMibRowPtr_t *pPathNext);
L7_RC_t dsStdMibDataPathElemCreate(L7_uint32 ifIndex, 
                                   L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection, 
                                   dsStdMibRowPtr_t *pStart);
L7_RC_t dsStdMibDataPathElemDelete(L7_uint32 intIfNum, 
                                   L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection);
L7_RC_t dsStdMibDataPathDelete(L7_uint32 intIfNum,
                               L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection,
                               dsStdMibRowPtr_t *pPathNext);
L7_RC_t dsStdMibDataPathInstanceDelete(dsStdMibRowPtr_t *pPathNext);
L7_RC_t dsStdMibPrimitiveElemDelete(dsStdMibRowPtr_t *pRow);
dsStdMibDataPathEntryCtrl_t *dsStdMibDataPathRowFind(L7_uint32 intIfNum,                            
                              L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection);
L7_USMDB_MIB_DIFFSERV_ROW_PTR_t dsStdMibRowPtrXlateToExt(dsStdMibRowPtr_t stdRowPtr);

/* diffserv_clfr.c */
void    dsStdMibClfrInit(void);
void    dsStdMibClfrIdNext(L7_uint32 *pClfrId);
void    dsStdMibClfrIdNextUpdate(void);
dsStdMibClfrEntryCtrl_t *dsStdMibClfrRowFind(L7_uint32 clfrId);
L7_RC_t diffServClfrCreate(dsStdMibRowPtr_t *pPathNext);
L7_RC_t diffServClfrDelete(dsStdMibRowPtr_t *pPathNext);

void    dsStdMibClfrElemInit(void);
void    dsStdMibClfrElemIdNext(L7_uint32 *pClfrElemId);
void    dsStdMibClfrElemIdNextUpdate();
dsStdMibClfrElementEntryCtrl_t *dsStdMibClfrElemRowFind(L7_uint32 clfrId, 
                                                        L7_uint32 clfrElemId);
L7_RC_t diffServClfrElemCreate(L7_uint32 clfrId, 
                               dsStdMibRowPtr_t *pSpecific, 
                               L7_uint32 precedence,
                               dsStdMibRowPtr_t  *pPathNext);
L7_RC_t diffServClfrElemDelete(dsStdMibRowPtr_t  *pPathNext);

void    dsStdMibMFClfrInit(void);
void    dsStdMibMFClfrIdNext(L7_uint32 *pMfClfrId);
void    dsStdMibMFClfrIdNextUpdate(void);

void    dsStdMibAuxMFClfrInit(void);
void    dsStdMibAuxMFClfrIdNext(L7_uint32 *pAuxMFClfrId);
void    dsStdMibAuxMFClfrIdNextUpdate(void);
dsStdMibAuxMFClfrEntryCtrl_t *dsStdMibAuxMFClfrRowFind(L7_uint32 auxMFClfrId);
L7_RC_t diffServAuxMFClfrCreate(dsStdMibAuxMFClfrEntryCtrl_t *pAuxMFClfrRow,
                                L7_uint32 *pAuxMFClfrId);
L7_RC_t diffServAuxMFClfrDelete(L7_uint32 auxMFClfrId);

L7_RC_t dsStdMibAuxMFClfrObjectSet(L7_uint32 classIndex, 
                                   L7_uint32 classRuleIndex,
                                   dsStdMibAuxMFClfrEntryCtrl_t *pAuxMFClfrRow);
void    dsStdMibAuxMFClfSetToDefaults(dsStdMibAuxMFClfrEntryCtrl_t *pAuxMFClfrRow);

/* diffserv_meter_api.c */
void    dsStdMibMeterInit(void);
void    dsStdMibMeterIdNext(L7_uint32 *pMeterId);
void    dsStdMibMeterIdNextUpdate(void);
dsStdMibMeterEntryCtrl_t *dsStdMibMeterRowFind(L7_uint32 meterId);
L7_RC_t diffServMeterCreate(dsStdMibRowPtr_t  *pSucceedNext,
                            dsStdMibRowPtr_t  *pFailNext,
                            dsStdMibRowPtr_t  *pSpecific,
                            dsStdMibRowPtr_t  *pPathNext);
L7_RC_t diffServMeterDelete(dsStdMibRowPtr_t *pPathNext);

void    dsStdMibTBParamInit(void);
void    dsStdMibTBParamIdNext(L7_uint32 *pTbParamId);
void    dsStdMibTBParamIdNextUpdate(void);
dsStdMibTBParamEntryCtrl_t *dsStdMibTBParamRowFind(L7_uint32 tbParamId);
L7_RC_t diffServTBParamCreate(L7_uint32 policyIndex, L7_uint32 policyInstIndex, 
                              L7_uint32 policyAttrIndex,
                              DSSTDMIB_ACTION_TYPE_t actionType,
                              DSSTDMIB_RATE_TYPE_t rateType,
                              dsStdMibRowPtr_t *pSpecific);
L7_RC_t diffServTBParamDelete(L7_uint32 tbParamId);
void    dsStdMibColorAwareInit(void);
dsStdMibMeterEntryCtrl_t *dsStdMibColorAwareRowFind(L7_uint32 meterId);
L7_RC_t diffServColorAwareCreate(L7_uint32 policyIndex, 
                                 L7_uint32 policyInstIndex, 
                                 L7_uint32 policyAttrIndex,
                                 DSSTDMIB_ACTION_TYPE_t policingType,
                                 dsStdMibColorLevel_t colorLevel,
                                 L7_uint32 meterId);
L7_RC_t diffServColorAwareDelete(L7_uint32 meterId);

/* diffserv_action_api.c */
void    dsStdMibActionInit(void);
void    dsStdMibActionIdNext(L7_uint32 *pActionId);
void    dsStdMibActionIdNextUpdate(void);
dsStdMibActionEntryCtrl_t *dsStdMibActionRowFind(L7_uint32 actionId);
L7_RC_t diffServActionCreate(L7_uint32        intIfNum, 
                             dsStdMibRowPtr_t *pSpecific, 
                             dsStdMibRowPtr_t *pPathNext);
L7_RC_t diffServActionDelete(dsStdMibRowPtr_t *pPathNext);
void    dsStdMibCountActInit(void);
void    dsStdMibCountActIdNext(L7_uint32 *pCountActId);
void    dsStdMibCountActIdNextUpdate(void);
dsStdMibCountActEntryCtrl_t *dsStdMibCountActRowFind(L7_uint32 countActId);
L7_RC_t diffServCountActCreate(L7_uint32 policyIndex, 
                               L7_uint32 policyInstIndex, 
                               L7_uint32 intIfNum,
                               DSSTDMIB_COUNT_TYPE_t countType,
                               L7_uint32 *pCountActId);
L7_RC_t diffServCountActDelete(L7_uint32 countActId);
void    dsStdMibAssignQueueInit(void);
void    dsStdMibAssignQueueIdNext(L7_uint32 *pAssignId);
void dsStdMibAssignQueueIdNextUpdate(void);
dsStdMibAssignQueueEntryCtrl_t *dsStdMibAssignQueueRowFind(L7_uint32 assignId);
L7_RC_t diffServAssignQueueCreate(L7_uint32 policyIndex, 
                                  L7_uint32 policyInstIndex, 
                                  L7_uint32 policyAttrIndex, 
                                  L7_uint32 *pAssignId);
L7_RC_t diffServAssignQueueDelete(L7_uint32 assignId);
void    dsStdMibRedirectInit(void);
void    dsStdMibRedirectIdNext(L7_uint32 *pRedirectId);
void dsStdMibRedirectIdNextUpdate(void);
dsStdMibRedirectEntryCtrl_t *dsStdMibRedirectRowFind(L7_uint32 redirectId);
L7_RC_t diffServRedirectCreate(L7_uint32 policyIndex, 
                               L7_uint32 policyInstIndex, 
                               L7_uint32 policyAttrIndex, 
                               L7_uint32 *pRedirectId);
L7_RC_t diffServRedirectDelete(L7_uint32 redirectId);
void    dsStdMibMirrorInit(void);
void    dsStdMibMirrorIdNext(L7_uint32 *pMirrorId);
void dsStdMibMirrorIdNextUpdate(void);
dsStdMibMirrorEntryCtrl_t *dsStdMibMirrorRowFind(L7_uint32 mirrorId);
L7_RC_t diffServMirrorCreate(L7_uint32 policyIndex, 
                             L7_uint32 policyInstIndex, 
                             L7_uint32 policyAttrIndex, 
                             L7_uint32 *pMirrorId);
L7_RC_t diffServMirrorDelete(L7_uint32 mirrorId);

/* diffserv_dropper_api.c */
void    dsStdMibAlgDropInit(void);
void    dsStdMibAlgDropIdNext(L7_uint32 *pAlgDropId);
void    dsStdMibAlgDropIdNextUpdate(void);
dsStdMibAlgDropEntryCtrl_t *dsStdMibAlgDropRowFind(L7_uint32 algDropId);
L7_RC_t diffServAlgDropCreate(L7_uint32 policyIndex, 
                              L7_uint32 policyInstIndex,
                              L7_uint32 policyAttrIndex,
                              L7_uint32 intIfNum,
                              dsStdMibAlgDropType_t type,
                              dsStdMibRowPtr_t *pQMeasure,
                              dsStdMibRowPtr_t *pSpecific,
                              dsStdMibRowPtr_t *pPathNext);
L7_RC_t diffServAlgDropDelete(dsStdMibRowPtr_t *pPathNext);

/* The following are the Standard MIB debug 'show' function prototypes */
void dsStdMibAvlTableSizeShow(void);
void dsStdMibDataPathTableShow(void);
void dsStdMibClfrTableShow(void);
void dsStdMibClfrElementTableShow(void);
void dsStdMibAuxMFClfrTableShow(void);
void dsStdMibAuxReuseMatrixShow(void);
void dsStdMibMeterTableShow(void);
void dsStdMibTBParamTableShow(void);
void dsStdMibColorAwareTableShow(void);
void dsStdMibActionTableShow(void);
void dsStdMibCountActTableShow(void);
void dsStdMibAssignQueueTableShow(void);
void dsStdMibRedirectTableShow(void);
void dsStdMibMirrorTableShow(void);
void dsStdMibAlgDropTableShow(void);
void dsStdMibAvlCtrlShow(void);
void dsmibStdShowAll(void);

#endif /* INCLUDE_DIFFSERV_STDMIB_H */

