/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   usmdb_mib_diffserv_common.h
*
* @purpose    Common definitions for USMDB MIB support
*                  
* @component  unitmgr
*
* @comments   Used for both private and standard MIB definitions
*
* @create     03/18/2002
*
* @author     gpaussa
* @end
*
**********************************************************************/
#ifndef USMDB_MIB_DIFFSERV_COMMON_H
#define USMDB_MIB_DIFFSERV_COMMON_H


#include "l7_common.h"
#include "diffserv_exports.h"


/* Generic MIB Row Pointer construct (implementation-specific) */
typedef enum
{
  L7_USMDB_MIB_DIFFSERV_TABLE_ID_NONE = 0,

  /* Private MIB tables */
  L7_USMDB_MIB_DIFFSERV_TABLE_ID_CLASS,
  L7_USMDB_MIB_DIFFSERV_TABLE_ID_CLASS_RULE,
  L7_USMDB_MIB_DIFFSERV_TABLE_ID_POLICY,
  L7_USMDB_MIB_DIFFSERV_TABLE_ID_POLICY_INST,
  L7_USMDB_MIB_DIFFSERV_TABLE_ID_POLICY_ATTR,
  L7_USMDB_MIB_DIFFSERV_TABLE_ID_POLICY_PERF_IN,
  L7_USMDB_MIB_DIFFSERV_TABLE_ID_POLICY_PERF_OUT,
  L7_USMDB_MIB_DIFFSERV_TABLE_ID_POLICY_SERVICE,

  /* Standard MIB tables */
  L7_USMDB_MIB_DIFFSERV_TABLE_ID_DATA_PATH,
  L7_USMDB_MIB_DIFFSERV_TABLE_ID_CLFR,
  L7_USMDB_MIB_DIFFSERV_TABLE_ID_CLFR_ELEMENT,
  L7_USMDB_MIB_DIFFSERV_TABLE_ID_MULTI_FIELD_CLFR,
  L7_USMDB_MIB_DIFFSERV_TABLE_ID_AUX_MF_CLFR,         /* extensions MIB */
  L7_USMDB_MIB_DIFFSERV_TABLE_ID_METER,
  L7_USMDB_MIB_DIFFSERV_TABLE_ID_TB_PARAM,
  L7_USMDB_MIB_DIFFSERV_TABLE_ID_COLOR_AWARE,         /* extensions MIB */
  L7_USMDB_MIB_DIFFSERV_TABLE_ID_ACTION,
  L7_USMDB_MIB_DIFFSERV_TABLE_ID_MARK_COS_ACT,        /* extensions MIB */
  L7_USMDB_MIB_DIFFSERV_TABLE_ID_MARK_COS_AS_COS2_ACT,        /* extensions MIB */  
  L7_USMDB_MIB_DIFFSERV_TABLE_ID_MARK_COS2_ACT,       /* extensions MIB */
  L7_USMDB_MIB_DIFFSERV_TABLE_ID_MARK_IPDSCP_ACT,
  L7_USMDB_MIB_DIFFSERV_TABLE_ID_MARK_IPPREC_ACT,     /* extensions MIB */
  L7_USMDB_MIB_DIFFSERV_TABLE_ID_COUNT_ACT,
  L7_USMDB_MIB_DIFFSERV_TABLE_ID_ASSIGN_QUEUE,        /* extensions MIB */
  L7_USMDB_MIB_DIFFSERV_TABLE_ID_REDIRECT,            /* extensions MIB */
  L7_USMDB_MIB_DIFFSERV_TABLE_ID_MIRROR,              /* extensions MIB */
  L7_USMDB_MIB_DIFFSERV_TABLE_ID_ALG_DROP,
  L7_USMDB_MIB_DIFFSERV_TABLE_ID_RANDOM_DROP,
  L7_USMDB_MIB_DIFFSERV_TABLE_ID_QUEUE,               /* deprecated */
  L7_USMDB_MIB_DIFFSERV_TABLE_ID_SCHEDULER,
  L7_USMDB_MIB_DIFFSERV_TABLE_ID_MIN_RATE,
  L7_USMDB_MIB_DIFFSERV_TABLE_ID_MAX_RATE,

  L7_USMDB_MIB_DIFFSERV_TABLE_ID_TOTAL          /* num of enum entries */

} L7_USMDB_MIB_DIFFSERV_TABLE_ID_t;

typedef struct
{
  L7_USMDB_MIB_DIFFSERV_TABLE_ID_t  tableId;
  L7_uint32     rowIndex1;
  L7_uint32     rowIndex2;
  L7_uint32     rowIndex3;
} L7_USMDB_MIB_DIFFSERV_ROW_PTR_t;

#define L7_USMDB_MIB_DIFFSERV_ROW_PTR_UPDATE(_rowp, _tid, _ri1, _ri2, _ri3) \
  do \
  { \
    (_rowp)->tableId = (_tid); \
    (_rowp)->rowIndex1 = (_ri1); \
    (_rowp)->rowIndex2 = (_ri2); \
    (_rowp)->rowIndex3 = (_ri3); \
  } while (0) \


/*
**********************************************************************
*   MIB DEFINED ENUMERATIONS
**********************************************************************
*/

/* Enumerated values for RowStatus object */
typedef enum
{
  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NONE = 0,    /* not a valid value */

  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE,
  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_IN_SERVICE,
  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_READY,
  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_CREATE_AND_GO,
  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_CREATE_AND_WAIT,
  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_DESTROY,

  L7_USMDB_MIB_DIFFSERV_ROW_STATUS_TOTAL        /* number of enum entries */

} L7_USMDB_MIB_DIFFSERV_ROW_STATUS_t;

/* Enumerated values for StorageType object */
/* NOTE: Update DiffServ defaults in defaultconfig.h when changing this enum. */
typedef enum
{
  L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_NONE = 0,  /* not a valid value */

  L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_OTHER,
  L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_VOLATILE,
  L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_NONVOLATILE,
  L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_READONLY,
  L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_PERMANENT,

  L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_TOTAL      /* number of enum entries */

} L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_t;

/* Enumerated values for TruthValue type */
typedef enum
{
  L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_NONE = 0,   /* not a valid value */

  L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_TRUE,
  L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_FALSE,

  L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_TOTAL       /* number of enum entries */

} L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_t;

/* Enumerated values for InetAddrType object */
typedef enum
{
  L7_USMDB_MIB_DIFFSERV_INET_ADDR_TYPE_UNKNOWN = 0,
  L7_USMDB_MIB_DIFFSERV_INET_ADDR_TYPE_IPV4,
  L7_USMDB_MIB_DIFFSERV_INET_ADDR_TYPE_IPV6,

  L7_USMDB_MIB_DIFFSERV_INET_ADDR_TYPE_TOTAL    /* number of enum entries */

} L7_USMDB_MIB_DIFFSERV_INET_ADDR_TYPE_t;

/* Enumerated values for AddressFamilyNumbers object */
typedef enum
{
  L7_USMDB_MIB_DIFFSERV_INET_ADDRESS_FAMILY_NUMBERS_OTHER = 0,
  L7_USMDB_MIB_DIFFSERV_INET_ADDRESS_FAMILY_NUMBERS_IPV4,
  L7_USMDB_MIB_DIFFSERV_INET_ADDRESS_FAMILY_NUMBERS_IPV6,

  L7_USMDB_MIB_DIFFSERV_INET_ADDRESS_FAMILY_NUMBERS_TOTAL /* number of enums */

} L7_USMDB_MIB_DIFFSERV_INET_ADDRESS_FAMILY_NUMBERS_t;

/* Values for DiffServ Interface Direction type */
typedef enum
{
  L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_NONE = 0,  /* not a valid value */

  L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_IN,
  L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_OUT,

  L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_TOTAL      /* number of enum entries */

} L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t;

/* Values for DiffServ Interface Operational Status type */
typedef enum
{
  L7_USMDB_MIB_DIFFSERV_IF_OPER_STATUS_NONE = 0,  /* not a valid value */

  L7_USMDB_MIB_DIFFSERV_IF_OPER_STATUS_UP,
  L7_USMDB_MIB_DIFFSERV_IF_OPER_STATUS_DOWN,

  L7_USMDB_MIB_DIFFSERV_IF_OPER_STATUS_TOTAL    /* number of enum entries */

} L7_USMDB_MIB_DIFFSERV_IF_OPER_STATUS_t;


#endif /* USMDB_MIB_DIFFSERV_COMMON_H */
