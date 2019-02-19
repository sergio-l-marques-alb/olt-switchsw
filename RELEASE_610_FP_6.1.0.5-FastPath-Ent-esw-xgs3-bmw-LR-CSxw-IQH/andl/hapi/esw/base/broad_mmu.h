/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2008
*
**********************************************************************
*
* @filename  broad_mmu.h
*
* @purpose   
*
* @component hapi
*
* @comments
*
* @create    6/25/08
*
* @author    colinw
*
* @end
*
**********************************************************************/

#ifndef INCLUDE_BROAD_MMU_H
#define INCLUDE_BROAD_MMU_H

#include "l7_common.h"
#include "broad_common.h"

#ifdef BCM_TRIUMPH_SUPPORT

#define TR_MMU_NUM_PG   8
#define TR_MMU_NUM_COS  8

/* Standard Ethernet MTU, 1536 bytes (1 cell = 128 bytes) */
#define TR_MMU_ETH_FRAME_CELLS  12

/* Jumbo Frame MTU, 9216 (1 cell = 128 bytes) */
#define TR_MMU_JUMBO_FRAME_CELLS  72

/* MAX Frame MTU, 16384 (1 cell = 128 bytes) */
#define TR_MMU_MAX_FRAME_CELLS  128

#define TR_MMU_IN_PORT_MIN_CELLS       72
#define TR_MMU_IN_PORT_MIN_PKTS        1

/* 
 * Need enough headroom for worst case round trip delay 3*Jumbo = 3 * 72 = 216 
 */
#define TR_MMU_PG_HDRM_LIMIT_CELLS     216
#define TR_MMU_PG_HDRM_LIMIT_PKTS      0x40 
/* We need more packet headroom for HiGig ports */
#define TR_MMU_HG_PG_HDRM_LIMIT_PKTS   0x60

/* (ports * min_cell) = 53 * 12 = 636 not accounting for Jumbo here */
#define TR_MMU_GLOBAL_HDRM_LIMIT_CELLS 636 

#define TR_MMU_PG_RESET_OFFSET_CELLS   24
#define TR_MMU_PG_RESET_OFFSET_PKTS    1
/* Need more hysteresis for HiGig ports */
#define TR_MMU_HG_PG_RESET_OFFSET_CELLS 72
#define TR_MMU_HG_PG_RESET_OFFSET_PKTS 24

#define TR_MMU_OUT_PORT_MIN_CELLS      12
#define TR_MMU_OUT_PORT_MIN_PKTS       1
#define TR_MMU_OUT_RESET_OFFSET_CELLS  24
#define TR_MMU_OUT_RESET_OFFSET_PKTS   2
#define TR_MMU_SOP_POLICY              0
#define TR_MMU_MOP_POLICY              7

/* PORT_SHARED_LIMIT_CELL values */
/* 7 / 1 = dynamic, alpha = 8 */
#define TR_MMU_PORT_SHARED_LIMIT_CELL     7
#define TR_MMU_PORT_SHARED_DYNAMIC_CELL   1
#define TR_MMU_PORT_SHARED_LIMIT_PKT     7
#define TR_MMU_PORT_SHARED_DYNAMIC_PKT   1

/* PORT OP_QUEUE_CONFIG values */
#define TR_MMU_OUT_QUEUE_CELL_ALPHA_IDX   7
#define TR_MMU_OUT_QUEUE_PKT_ALPHA_IDX    7

/* CPU OP_QUEUE_CONFIG_  values */
#define TR_MMU_OUT_QUEUE_CELL_CPU_MIN_CELL        72 
#define TR_MMU_OUT_QUEUE_PKT_CPU_MIN_PKT         24 
#define TR_MMU_OUT_QUEUE_CELL_CPU_LIMIT_ENABLE    1
#define TR_MMU_OUT_QUEUE_CELL_CPU_LIMIT_DYNAMIC   0 
#define TR_MMU_OUT_QUEUE_PKT_CPU_LIMIT_ENABLE    1
#define TR_MMU_OUT_QUEUE_PKT_CPU_LIMIT_DYNAMIC   0 

/* OP_PORT_CONFIG values */
#define TR_MMU_OUT_PORT_CPU_CELL_SHARED_LIMIT   572
#define TR_MMU_OUT_PORT_CPU_CELL_SHARED_ENABLE  1
#define TR_MMU_OUT_PORT_CPU_PKT_SHARED_LIMIT    24
#define TR_MMU_OUT_PORT_CPU_PKT_SHARED_ENABLE   1

#define TR_COS_PRI_XON_ENABLE   0x7F

typedef struct {
  int cos;
  int pg;
} pg_cosmap_t ;

typedef struct {
  int pg;
  int min_cell;
  int min_pkt;
  int floor_cell;
  int reset_offset_cell;
  int reset_offset_pkt;
  int hdrm_cell;
  int hdrm_pkt;
  int thresh;
} pg_vals_t;

/**********************************************************
 * @purpose  Modify MMU for Priority Group Usage on Triumph
 *
 * @param    unit     - bcm unit number
 * @param    cosmap   - the cosmap to priority group mapping
 * @param    num_cos  - the number of elements in the cosmap
 * @param    pg_vals  - the priority group settings
 * @param    num_pg_vals - the number of elements in the pg_vals
 *
 * @returns  
 *
 * @comments Resources reserved for the Priority Group must be
 *           accounted for in the shared pools
 *
 * @end
 *
 **********************************************************/
int hapiBroadPgInit(int unit, pg_cosmap_t *cosmap,int num_cos, 
                    pg_vals_t *pg_vals,int num_pg_vals);

#endif /* TRIUMPH ONLY */

#ifdef BCM_SCORPION_SUPPORT
/* Configure all COS queues by default */
#ifndef SCORPION_MMU_NUM_COS
#define SCORPION_MMU_NUM_COS     8
#endif

/* Standard Ethernet MTU (1 cell = 128 bytes) */
#ifndef SCORPION_MMU_MTU_CELLS
#define SCORPION_MMU_MTU_CELLS   12
#endif

/* The following number of cells is based on packet simulations */
#define SCORPION_MMU_PORT_MIN    72

/* Configure CPU port by default */
#ifndef SCORPION_MMU_CONFIG_CPU
#define SCORPION_MMU_CONFIG_CPU  1
#endif

#ifndef SCORPION_CPU_RESERVED_CELLS
#define SCORPION_CPU_RESERVED_CELLS (SCORPION_MMU_PORT_MIN * 2)
#endif

#if SCORPION_MMU_CONFIG_CPU == 1
#define PBMP_MMU_ITER           PBMP_ALL_ITER
#else
#define PBMP_MMU_ITER           PBMP_GX_ITER
#endif

#define SCORPION_PG_MAX         8

#define SCORPION_RESET_GRAN_SHIFT    4

/* Alternative disabling method b/c PORT_LIMIT_ENABLE bit is broken */
#define SCORPION_OP_SHARED_LIMIT_DISABLE        (0x3fff - SCORPION_CPU_RESERVED_CELLS)

/* Setting the ALPHA value lower than front panel ports */
/* using static cells for CPU */
#define SCORPION_OP_QCFG_ALPHA_CPU    144

/* HDRM should be equal to max frame * 3 */
#define SCORPION_PG_HDRM_LIMIT    216

#define SCORPION_OP_QMIN_CELLS    12
#define SCORPION_OP_QCFG_ALPHA    7

#define SCORPION_TOTAL_CELLS  (16 * 1024)

#define SCORPION_PORT_SHARED_ALPHA  7
#endif

/* prototype for the custom function to set the mmu pause settings */
typedef int (*hapiBroadMmuPauseCb_f )(int unit, int mode);

/* prototype for the custom function to set the mmu phy settings on link change */
typedef int (*hapiBroadMmuPhyLinkCb_f )(int unit, soc_port_t port, bcm_port_info_t *info);

/*********************************************************************
* @purpose  Register a callback to perform MMU operations on Link change
*
* @param    unit - bcm unit number
* @param    port - the port
* @param    info - link information
*
* @returns  none
*
* @comments 
*
* @end
*********************************************************************/
void hapiBroadMmuPhyLinkCbSet(hapiBroadMmuPhyLinkCb_f func);

/**********************************************
 * @Purpose Set the custom MMU pause function instead of the default
 * 
 * @param   func  - the function to be registered
 * 
 * @comments  The registered function will be invoked whenever pause is
 *            enabled/disabled.  It should return one of the values from 
 *            bcm_error_t.  Typically BCM_E_NONE or BCM_E_FAIL.  The registered
 *            function should only modify registers that can be modified with
 *            traffic running.  
 * 
 * @returns none
 **********************************************/
void hapiBroadMmuPauseCbSet(hapiBroadMmuPauseCb_f func);

/*********************************************************************
*
* @purpose Make any MMU related mods when pause is set
* 
* @param  unit  - the bcm unit number
* @param  mode  - 0 to disable or 1 to enable pause
*
* @returns Broadcom driver defined
*
* @end
*
*********************************************************************/
int hapiBroadMmuPauseSet(int unit,int mode);

/*********************************************************************
* @purpose  Modify MMU for HELIX
*
* @param    unit - bcm unit number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t hapiBroadHelixMmuConfigModify(L7_uint32 unit);

/*********************************************************************
* @purpose  Modify the MMU on link callback.  Some chips may have MMU workarounds
*
* @param    unit - bcm unit number
* @param    port - the port
* @param    info - link information
*
* @returns  none
*
* @comments 
*
* @end
*********************************************************************/
void hapiBroadMmuLinkscanCb(int unit, soc_port_t port, bcm_port_info_t *info);

#if L7_FEAT_CUSTOM_MMU
/* If this feature is defined then the following function must exist in a
   customer specific file. */
extern int hapiBroadMmuCustomConfigure(L7_uint32 unit);
#define HAPI_BROAD_MMU_CUSTOM_CONFIGURE(unit) (hapiBroadMmuCustomConfigure(unit))
#else
#define HAPI_BROAD_MMU_CUSTOM_CONFIGURE(unit) (BCM_E_UNAVAIL)
#endif

#endif
