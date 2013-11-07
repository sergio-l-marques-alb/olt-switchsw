/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   l7_usl_common.h
*
* @purpose    Common defines
*
* @component  HAPI
*
* @comments   none
*
* @create     11/19/2003
*
* @author     mbaucom
* @end
*
**********************************************************************/
#ifndef L7_USL_COMMON_H
#define L7_USL_COMMON_H

#include "l7_common.h"
#include "log.h"
#include "osapi.h"
#include "l7_usl_bcm_debug.h"

/* List of USL DB's. Each USL Db must be assigned to one of the USL_DB_GROUP_t
** in usl_db_group_mapping_set routine.
*/

typedef enum
{
  USL_FIRST_DB_ID = 1,

  /* General system configuration */
  USL_SYSTEM_DB_ID = USL_FIRST_DB_ID,

  /* Port configuration */
  USL_PORT_DB_ID,

  USL_L2_UCAST_DB_ID,
  USL_L2_MCAST_DB_ID,
  USL_L2_VLAN_DB_ID,
  USL_L2_STG_DB_ID,
  USL_L2_TRUNK_DB_ID,
  USL_L2_VLAN_IPSUBNET_DB_ID,
  USL_L2_VLAN_MAC_DB_ID,
  USL_L2_DVLAN_DB_ID,
  USL_L2_PROTECTED_GRP_DB_ID,

  USL_POLICY_DB_ID,

  USL_L3_HOST_DB_ID,
  USL_L3_LPM_DB_ID,
  USL_L3_INTF_DB_ID,
  USL_L3_EGR_NHOP_DB_ID,
  USL_L3_MPATH_EGR_NHOP_DB_ID,
  USL_L3_TUNNEL_INITIATOR_DB_ID,
  USL_L3_TUNNEL_TERMINATOR_DB_ID,

  USL_IPMC_ROUTE_DB_ID,
  USL_IPMC_PORT_DB_ID,

  USL_WLAN_PORT_DB_ID,
  USL_WLAN_VLAN_DB_ID,

  USL_METRO_VLAN_XLATE_DB_ID,
  USL_METRO_VLAN_XLATE_EGRESS_DB_ID,
  USL_METRO_VLAN_CROSSCONNECT_DB_ID,

  /* Always last */
  USL_LAST_DB_ID

} USL_DB_ID_t;


/* USL Db Groups:
** All the Db's within a Group are reconciled together during Warm Start.
** Do not change the order of the Groups listed below as they are reconciled
** using the table order.
*/

typedef enum
{
  USL_FIRST_DB_GROUP = 1,

  /* All the Db's not in L3 and IPMC group */
  USL_GENERIC_DB_GROUP = USL_FIRST_DB_GROUP,

  /* All the L3 Db's */
  USL_L3_DB_GROUP,

  /* All the IPMC Db's */
  USL_IPMC_DB_GROUP,


  /* Always last */
  USL_LAST_DB_GROUP

} USL_DB_GROUP_t;

/* USL Database types */
typedef enum
{
  USL_OPERATIONAL_DB = 0x1,
  USL_SHADOW_DB = 0x2,

  /* OPER Db is Current Db all the time except warm start.
  ** SHADOW Db becomes Current Db during warm start.
  */
  USL_CURRENT_DB = 0x4,

  USL_LAST_DB

} USL_DB_TYPE_t;

#define USL_OPERATIONAL_DB_NAME "Operational Db"
#define USL_SHADOW_DB_NAME      "Shadow Db"

#ifdef L7_STACKING_PACKAGE
#define L7_MOD_MAX  32
#else
#define L7_MOD_MAX  5
#endif

#define USL_DB_EXACT_ELEM 0
#define USL_DB_NEXT_ELEM 1

/* Generic USL Cmds */
typedef enum
{
  USL_CMD_ADD = 1,
  USL_CMD_REMOVE,
  USL_CMD_SET,

} USL_CMD_t;

/* USL States */
typedef enum
{
  USL_INIT_STATE = 1,
  USL_COLD_START_STATE,
  USL_WARM_START_STATE,
  USL_OPERATIONAL_STATE,

  USL_LAST_STATE
} USL_STATE_t;


/* Synchronization types:
   COLD: Synchronize a unit that is in default state. Done
         when a new unit joins a stack or non-NSF manager failover.

   WARM: Synchronize a unit that is actively forwarding traffic.
         Done after a NSF management failover.
*/

typedef enum
{
  USL_COLD_SYNC = 1,
  USL_WARM_SYNC,

  USL_INVALID_SYNC

} USL_SYNC_TYPE_t;


/* The messages below are handled by usl worker task.
** The usl worker task processes periodic messages that
** require long processing times.
*/
typedef enum
{
  USL_WORKER_TASK_FIRST_MSG = 0,
  USL_MAC_ADDR_LEARN,
  USL_MAC_ADDR_AGE,
  USL_MAC_TABLE_FLUSH,
  USL_MAC_TABLE_UNIT_FLUSH,
  USL_MAC_TABLE_FORCE_RESYNC,
  USL_MAC_TABLE_TRUNK_FLUSH,
  USL_MAC_TABLE_WLAN_PORT_FLUSH,
  USL_MAC_TABLE_PORT_FLUSH,
  USL_MAC_TABLE_FLUSH_ALL,
  USL_MAC_TABLE_SYNC_APPLICATION,
  USL_MAC_TABLE_VLAN_FLUSH,
  USL_MAC_TABLE_MAC_FLUSH,
  USL_MAC_TABLE_AGE_TIME,
  USL_MAC_TABLE_DATAPLANE_CLEANUP,

  USL_IPMC_INUSE_TIMER_POP,
  USL_IPMC_INUSE,
  USL_HOST_INUSE_TIMER_POP,
  USL_HOST_INUSE,

  USL_WORKER_TASK_LAST_MSG_TYPE
} uslWorkerTaskMsgTypes_t;

/* USL Control task message types */
typedef enum
{
  USL_CONTROL_TASK_FIRST_MSG = USL_WORKER_TASK_LAST_MSG_TYPE + 1,
  USL_SYNCED_UNITS_LIST,
  USL_DRIVER_STARTUP_MSG,
  USL_PORTDB_UPDATE,
  USL_COLD_SYNC_START,
  USL_COLD_SYNC_FINISH,
  USL_WARM_SYNC_START,
  USL_WARM_SYNC_FINISH,
  USL_SYNC_SYSTEM_TABLE,
  USL_SYNC_VLAN_TABLE,
  USL_SYNC_STG_TABLE,
  USL_SYNC_TRUNK_TABLE,
  USL_SYNC_L2UCAST_STATIC_TABLE,
  USL_SYNC_L2MC_TABLE,
  USL_SYNC_L3_INTF_TABLE,
  USL_SYNC_L3_HOST_TABLE,
  USL_SYNC_L3_ROUTE_TABLE,
  USL_SYNC_IPMC_TABLE,
  USL_SYNC_IPMC_PORT_TABLE,
  USL_SYNC_POLICY_TABLE_START,
  USL_SYNC_POLICY_TABLE_RULE,
  USL_SYNC_POLICY_TABLE,
  USL_SYNC_DVLAN_DATA,
  USL_SYNC_L3_TUNNEL_INITIATOR_TABLE,
  USL_SYNC_L3_TUNNEL_TERMINATOR_TABLE,
  USL_SYNC_VLANIPSUBNET_TABLE,
  USL_SYNC_VLANMAC_TABLE,
  USL_SYNC_WLAN_VLAN_TABLE,
  USL_SYNC_WLAN_PORT_TABLE,
  USL_SYNC_PROTECTED_GROUP_TABLE,
  USL_SYNC_L3_EGR_NHOP_TABLE,
  USL_SYNC_L3_MULTIPATH_EGR_TABLE,
  USL_SYNC_PORT_TABLE,
  USL_SYNC_PORT_VLAN_COMMIT,
  USL_SYNC_VLAN_XLATE,
  USL_SYNC_VLAN_XLATE_EGRESS,
  USL_SYNC_VLAN_CROSS_CONNECT,

  /* Always last */
  USL_CONTROL_TASK_LAST_MSG,
} uslControlMsgTypes_t;


/* USL MSG FORMAT */
#define USL_MSG_TYPE_OFFSET (0)
#define USL_MSG_ELEM_OFFSET (USL_MSG_TYPE_OFFSET + sizeof(L7_uint32))
#define USL_MSG_UNIT_OFFSET (USL_MSG_ELEM_OFFSET + sizeof(L7_uint32))
#define USL_MSG_ID_OFFSET   (USL_MSG_UNIT_OFFSET + sizeof(L7_uint32))
#define USL_MSG_DATA_OFFSET (USL_MSG_ID_OFFSET + sizeof(L7_uint32))
#define USL_MSG_SIZE_HDR    (USL_MSG_DATA_OFFSET)

/* Flush message size.
*/
#define USL_FLUSH_MSG_SIZE   (USL_MSG_SIZE_HDR + ((sizeof (L7_uint32)) * 2))


#define USL_DRIVER_STARTUP_MSG_TIMEOUT 2000
#define USL_SYNC_STATUS_TIMEOUT (20000) /* in milliseconds */

/* Response code from the sync */
typedef struct
{
  L7_uint32           status;
  L7_uint32           data;
} uslSyncMsgStatus_t;


/* Other utility defines */

#define USL_INFO_LOG  ((L7_uint32)(1 << 0))
#define USL_BCM_E_LOG ((L7_uint32)(1 << 1))
#define USL_E_LOG     ((L7_uint32)(1 << 2))

#define USL_LOG_MSG(type_,format, args...)  {\
        L7_uchar8 usl_log_buf[LOG_MSG_MAX_MSG_SIZE];\
    L7_uint32 allowed_types;\
    allowed_types = usl_allowed_log_types_get();\
        if (osapiSnprintf (usl_log_buf, LOG_MSG_MAX_MSG_SIZE, format, ##args) > (LOG_MSG_MAX_MSG_SIZE - 1))\
          usl_log_buf[LOG_MSG_MAX_MSG_SIZE-1] = (L7_uchar8)0x00;\
      if (allowed_types & type_)\
      {\
        usl_log_msg_fmt(__FILE__,__LINE__,usl_log_buf,L7_FALSE);\
        L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_DRIVER_COMPONENT_ID, usl_log_buf);\
      }\
    }

#define USL_LOG_ERROR(format, args...) {\
        L7_uchar8 usl_log_buf[LOG_MSG_MAX_MSG_SIZE];\
    L7_uint32 allowed_types;\
    allowed_types = usl_allowed_log_types_get();\
        if (osapiSnprintf (usl_log_buf, LOG_MSG_MAX_MSG_SIZE, format, ##args) > (LOG_MSG_MAX_MSG_SIZE - 1))\
          usl_log_buf[LOG_MSG_MAX_MSG_SIZE-1] = (L7_uchar8)0x00;\
      usl_log_msg_fmt(__FILE__,__LINE__,usl_log_buf,L7_TRUE);\
      if (allowed_types & USL_INFO_LOG) usl_trace_show(0,"INF",0); \
    LOG_ERROR(0xbadd); \
    }


#ifdef __mips__

#define L7_UINT32_GET(x) ({ unsigned char *c = (unsigned char *)x; \
   ((c[0]<<24) & 0xff000000) + ((c[1]<<16) & 0x00ff0000) + ((c[2]<<8) & 0x0000ff00) + c[3]; })
#define L7_UINT32_SET(x,v) do { \
   ((L7_uchar8 *)(x))[0] = ((v)>>24)&0xff; \
   ((L7_uchar8 *)(x))[1] = ((v)>>16)&0xff; \
   ((L7_uchar8 *)(x))[2] = ((v)>>8)&0xff; \
   ((L7_uchar8 *)(x))[3] = (v)&0xff; \
} while(0)

#else

#define L7_UINT32_GET(x) *(L7_uint32 *)(x)
#define L7_UINT32_SET(x,v) *(L7_uint32 *)(x) = (v)

#endif

#define L7_UINT32_PACK(_buf, _var) \
  do { \
    *_buf++ = ((_var) >> 24) & 0xff; \
    *_buf++ = ((_var) >> 16) & 0xff; \
    *_buf++ = ((_var) >> 8) & 0xff; \
    *_buf++ = (_var) & 0xff; \
  } while (0)

#define L7_UINT32_UNPACK(_buf, _var) \
  do { \
    _var  = *_buf++ << 24; \
    _var |= *_buf++ << 16; \
    _var |= *_buf++ << 8; \
    _var |= *_buf++; \
  } while (0)

#define L7_UINT16_PACK(_buf, _var) \
  do { \
    *_buf++ = ((_var) >> 8) & 0xff; \
    *_buf++ = (_var) & 0xff; \
  } while (0)

#define L7_UINT16_UNPACK(_buf, _var) \
  do { \
    _var  = *_buf++ << 8; \
    _var |= *_buf++; \
  } while (0)

#define L7_MAC_ADDRESS_PACK(_buf, _var) \
  do { \
    memcpy(_buf, _var, 6); \
    _buf += 6; \
  } while (0)


#define USL_DEFAULT_AGE_TIME 300
#define USL_DEFAULT_DVLAN_ETHERTYPE 0x8100

#define USL_INVALID_DB_INDEX (-1)

typedef struct
{
  /* pointer to Db Element */
  void *dbElem;

  /* Index of the Db element. Applicable for Db's implemented
  ** using Arrays.
  */
  L7_int32 elemIndex;

} uslDbElemInfo_t;

/* List of Db functions to be registered for using the general sync routines */
typedef struct
{
  /* Get the size of a db element */
  L7_uint32 (*get_size_of_db_elem) (void);

  /* search element in the Db  */
  L7_int32    (*get_db_elem) (USL_DB_TYPE_t dbType, L7_uint32 flags,
                              uslDbElemInfo_t searchInfo,
                              uslDbElemInfo_t *elemInfo);

  /* Delete a element from the specified db  */
  L7_int32  (*delete_elem_from_db) (USL_DB_TYPE_t   dbType,
                                    uslDbElemInfo_t elemInfo);

  /* Print the contents of a vlan db element in specified buffer */
  void      (*print_db_elem) (void *dbElem, L7_uchar8 *buffer,
                              L7_uint32 size);

  /* Compare the shadowElem and operElem.
  ** If they are different then update the Oper Db and Hw
  ** with shadowElem.
  */
  L7_int32  (*update_usl_bcm)(void *shadowElem, void *operElem);


  /* Create the element in Usl Bcm layer  */
  L7_int32  (*create_usl_bcm)(void *dbElem);

  /* Delete the given shadow db element from Usl Bcm */
  L7_int32  (*delete_usl_bcm)(void *dbElem);

  /* Optional routines. If these are not provided then
  ** the default routines specified below are used.
  ** Register these routines if any db member elem
  ** is dynamic, i.e has pointer member elements.
  */

  /* Allocate memory for a db element.
  ** If this routine is not specified then sync
  ** code uses osapiMalloc using size of db elem
  */
  void * (*alloc_db_elem)(void);

  /* Free memory for a db element.
  ** If this routine is not specified then sync
  ** code uses osapiFree.
  */
  void  (*free_db_elem)(void *elem);

  /* Copy the contents of src elem to dest elem.
  ** If this routine is not specified then memcpy
  ** is used
  */
  void  (*copy_db_elem) (void *dest, void *src);

  /* Pack the element in the buffer. Returns the modified
  ** pointer to buffer.
  ** If this routine is not specified then memcpy is used.
  */
  void * (*pack_db_elem) (void *buffer, void *elem);

  /* Unpack the buffer into element. Returns the modified
  ** buffer ptr.
  ** If this routine is not specified then memcpy is used.
  */
  void * (*unpack_db_elem) (void *bufffer, void *elem);

} uslDbSyncFuncs_t;

#endif
