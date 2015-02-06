//*****************************************************************************
// FW_SHM
//*****************************************************************************

#ifndef __FW_SHM_H__
#define __FW_SHM_H__

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "dtl_ptin.h"

//*****************************************************************************

#if (PTIN_BOARD_IS_STANDALONE)

#define FW_SHM_KEY      9889

#define SSM_N_SLOTS         1
#define SSM_N_INTFS         18
#define SSM_N_INTFS_IN_USE  PTIN_SYSTEM_N_PORTS

/* Counter state bits
Bit 8: Rx Dropped packets
Bit 7: Rx Undersized packets 
Bit 6: Rx Oversized packets 
Bit 5: Rx Fragments 
Bit 4: Rx Jabbers 
Bit 3: CRC errors 
Bit 2: Tx Collisions 
Bit 1: Tx activity 
Bit 0: Rx activity
*/
typedef struct {
  L7_uint32     admin;          // Estado administrativo: 0=Down , 1=Up
  L7_uint32     link;		    // Bit 0 -> Link state; Bit 1 -> RX traffic; Bit 2 -> TX traffic
  L7_uint32     port_state;     // Uso futuro (exemplo: indicacao de actividade/trafego)

 #ifdef SYNC_SSM_IS_SUPPORTED
  L7_uint32     ssm_rx;         /* bit 16 -> enable; bits 7:0 -> ssm code */
  L7_uint32     ssm_tx;         /* bit 16 -> enable; bits 7:0 -> ssm code */
 #endif
} t_eth_status;

typedef struct {
  t_eth_status  intf[SSM_N_INTFS];
 #ifdef SYNC_SSM_IS_SUPPORTED
  L7_uchar8     SyncE_Recovery_clock[2];
 #endif
} t_fw_shm;

#elif (defined(SYNC_SSM_IS_SUPPORTED) && PTIN_BOARD_IS_MATRIX)

#define FW_SHM_KEY          9890

#define SSM_N_SLOTS         20
#define SSM_N_INTFS         8
#define SSM_N_INTFS_IN_USE  PTIN_SYS_INTFS_PER_SLOT_MAX

typedef struct {
  struct
  {
    L7_uint32   ssm_rx;         /* bit 16 -> enable; bits 7:0 -> ssm code */
    L7_uint32   ssm_tx;         /* bit 16 -> enable; bits 7:0 -> ssm code */
    L7_uint32   link;
  } intf[SSM_N_SLOTS][SSM_N_INTFS];
} t_fw_shm;

#elif (defined(SYNC_SSM_IS_SUPPORTED) && (PTIN_BOARD_IS_LINECARD))

#define FW_SHM_KEY          9889

#define SSM_N_SLOTS         1                         /* Always 1 slot for linecards */
#define SSM_N_INTFS         PTIN_SYSTEM_MAX_N_PORTS   /* 64 ports used for array dim */
#define SSM_N_INTFS_IN_USE  PTIN_SYSTEM_N_PORTS       /* Ports actually used for the related board */

typedef struct {
  L7_uint32     admin;                          // Estado administrativo: 0=Down , 1=Up
  L7_uint32     link;                           // Bit 0 -> Link state; Bit 1 -> RX traffic; Bit 2 -> TX traffic
  L7_uint32     port_state;                     // Uso futuro (exemplo: indicacao de actividade/trafego)

  L7_uint32     ssm_rx;                         /* bit 16 -> enable; bits 7:0 -> ssm code */
  L7_uint32     ssm_tx;                         /* bit 16 -> enable; bits 7:0 -> ssm code */
} t_eth_status;

typedef struct {
  t_eth_status  intf[PTIN_SYSTEM_MAX_N_PORTS];
  L7_uchar8     SyncE_Recovery_clock[2];        /* Main and secondary ports */
} t_fw_shm;

#else

#define FW_SHM_KEY      9889

typedef struct {
  L7_uint32 dummy;
} t_fw_shm;

#endif

//*****************************************************************************

#ifndef __FW_SHM_C__
#define _VAR_ extern
#else
#define _VAR_
#endif

#ifdef __cplusplus
#define _EXTERN_ extern "C"
#else
#define _EXTERN_ extern
#endif

//*****************************************************************************

#if ( defined(SYNC_SSM_IS_SUPPORTED) || PTIN_BOARD_IS_STANDALONE )
_VAR_ t_fw_shm  fw_shm;
_VAR_ t_fw_shm  *pfw_shm;
#endif

//*****************************************************************************

_EXTERN_ int fw_shm_open( void );
_EXTERN_ int fw_shm_close( void );

//*****************************************************************************

#undef _VAR_
#undef _EXTERN_

//*****************************************************************************

#endif
