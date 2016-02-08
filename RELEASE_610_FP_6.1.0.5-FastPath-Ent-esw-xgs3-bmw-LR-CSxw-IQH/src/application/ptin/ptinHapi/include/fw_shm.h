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

#define FW_SHM_KEY      9889

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
  L7_uint32     link;		//
  L7_uint32     counter_state;  // Uso futuro (exemplo: indicacao de actividade/trafego)
} t_eth_status;

typedef struct {
  t_eth_status  intf[L7_SYSTEM_N_PORTS];
} t_fw_shm;

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

_VAR_ t_fw_shm   *pfw_shm;

//*****************************************************************************

_EXTERN_ int fw_shm_open( void );
_EXTERN_ int fw_shm_close( void );

//*****************************************************************************

#undef _VAR_
#undef _EXTERN_

//*****************************************************************************

#endif
