/*
 * $Id: metrolite.h,v 1.1.2.1 Broadcom SDK $
 * $Copyright: Copyright 2016 Broadcom Corporation.
 * This program is the proprietary software of Broadcom Corporation
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software
 * and all intellectual property rights therein.  IF YOU HAVE
 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of
 * Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
 *
 * File:        metrolite.h
 */

#ifndef _SOC_METROLITE_H_
#define _SOC_METROLITE_H_

#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/error.h>
#include <soc/katana2.h>
#include <soc/saber2.h>
#include <soc/shmoo_and28.h>
#include <bcm/types.h>
#define ML_MAX_LOGICAL_PORTS           14
#define ML_MAX_PHYSICAL_PORTS          12 
#define ML_MAX_PP_PORTS                32
#define ML_LPBK                        13 
#define ML_CMIC                        0
#define ML_IDLE                        ML_MAX_LOGICAL_PORTS
#define ML_MAX_BLOCKS                  3
#define ML_MAX_PORTS_PER_BLOCK         4

#define SOC_ML_MIN_SUBPORT_INDEX          14
#define SOC_ML_MAX_SUBPORTS               16
/* Max 6 streams per linkphy port.
 * So we can support max of 6 subports per linkphy port.
 */
#define SOC_ML_MAX_LINKPHY_SUBPORTS_PER_PORT     4
#define SOC_ML_MAX_LINKPHY_STREAMS_PER_PORT      6 
#define SOC_ML_MAX_STREAMS_PER_SLICE             24

#define SOC_ML_LINKPHY_TX_DATA_BUF_START_ADDR_MAX 3392
#define SOC_ML_LINKPHY_TX_DATA_BUF_END_ADDR_MIN   31
#define SOC_ML_LINKPHY_TX_DATA_BUF_END_ADDR_MAX   3423

#define SOC_ML_LINKPHY_TX_STREAM_START_ADDR_OFFSET 0x80

#define SOC_ML_LINKPHY_BLOCK_MAX              1  
#define SOC_ML_MAX_LINKPHY_PORTS              4

#define SOC_ML_SUBPORT_PORT_MAX               (16)
#define SOC_ML_SUBPORT_GROUP_MAX              SOC_ML_SUBPORT_PORT_MAX

#define SOC_ML_SUBPORT_PP_PORT_INDEX_MIN      (14)
#define SOC_ML_SUBPORT_PP_PORT_INDEX_MAX      (\
                SOC_ML_SUBPORT_PP_PORT_INDEX_MIN + SOC_ML_SUBPORT_PORT_MAX -1)

#define ML_PORT_MAC_MAX       32

#define ML_MAX_SERVICE_POOLS            4
#define ML_MAX_PRIORITY_GROUPS          8

const static uint32 ml_port_mapping[ML_MAX_LOGICAL_PORTS][2] =
{ {0,0}, {1,15}, {2,23}, {3, 31}, {4, 39}, {5,47}, {6, 55},
  {7,63}, {8, 71}, {9, 79}, {21, 175}, {22, 183}, {23, 191}, {29,239}};

#define ML_GET_THDI_PORT(p) ml_port_mapping[p][0]
#define ML_GET_THDI_PORT_PG(p) ml_port_mapping[p][1]
#define  METROLITE_GET_REG_THDI_PORT(unit, port, reg, thdi_port)                 \
         if  (SOC_IS_METROLITE(unit) && (block == MMU_BLOCK(unit)) &&            \
              ((SOC_REG_STAGE(unit, reg) == 2) ||                                \
              (SOC_REG_STAGE(unit, reg) == 38)|                                  \
              (SOC_REG_STAGE(unit, reg) == 40))) {                               \
               thdi_port = ML_GET_THDI_PORT(port);}  
              
typedef struct ml_pbmp_s {
    soc_pbmp_t *pbmp_gport_stack;
    soc_pbmp_t *pbmp_mxq;
    soc_pbmp_t *pbmp_mxq1g;
    soc_pbmp_t *pbmp_mxq2p5g;
    soc_pbmp_t *pbmp_mxq10g;
    soc_pbmp_t *pbmp_xl;
    soc_pbmp_t *pbmp_xl1g;
    soc_pbmp_t *pbmp_xl2p5g;
    soc_pbmp_t *pbmp_xl10g;
    soc_pbmp_t *pbmp_xport_ge;
    soc_pbmp_t *pbmp_xport_xe;
    soc_pbmp_t *pbmp_valid;
    soc_pbmp_t *pbmp_pp;
    soc_pbmp_t *pbmp_linkphy;
}ml_pbmp_t;

typedef uint32 ml_port_speeds_t[ML_MAX_BLOCKS][ML_MAX_PORTS_PER_BLOCK];
typedef uint32 ml_speed_t[ML_MAX_PHYSICAL_PORTS];
typedef uint32 ml_port_to_block_t[ML_MAX_PHYSICAL_PORTS];
typedef uint32 ml_port_to_block_subports_t[ML_MAX_PHYSICAL_PORTS];
typedef uint32 ml_block_ports_t[ML_MAX_BLOCKS][ML_MAX_PORTS_PER_BLOCK];

extern int soc_ml_linkphy_port_reg_blk_idx_get(
     int unit, int port, int blktype, int *block, int *index);
extern int soc_ml_linkphy_port_blk_idx_get(
     int unit, int port, int *block, int *index);
extern int soc_ml_linkphy_get_portid(int unit, int block, int index);

extern void _ml_phy_addr_default(int unit, int port,
                                 uint16 *phy_addr, uint16 *phy_addr_int);
extern int soc_ml_mem_config(int unit, int dev_id);
extern int _soc_ml_mmu_reconfigure(int unit);
extern void soc_metrolite_pbmp_init(int unit, ml_pbmp_t ml_pbmp);
extern void soc_metrolite_subport_init(int unit);
extern int bcm_ml_flexio_pbmp_update(int unit, bcm_pbmp_t *pbmp);
extern soc_functions_t soc_metrolite_drv_funs;
extern ml_block_ports_t *ml_block_ports[SOC_MAX_NUM_DEVICES];
extern ml_port_speeds_t *ml_port_speeds[SOC_MAX_NUM_DEVICES];
extern int soc_ml_iecell_port_reg_blk_idx_get(
    int unit, int port, int blktype, int *block, int *index);
extern int soc_metrolite_num_cosq_init(int unit);
extern soc_error_t soc_metrolite_get_port_block(
       int unit, soc_port_t port,uint8 *block);
extern soc_error_t soc_metrolite_reconfigure_tdm(int unit,uint32 new_tdm_size,uint32 *new_tdm);
extern void soc_metrolite_block_reset(int unit, uint8 block,int active_low);
extern int soc_metrolite_xl_port_speed_get(int unit, int port, int *speed);

extern int _soc_ml_flexio_scache_allocate(int unit);
extern int _soc_ml_flexio_scache_retrieve(int unit);
extern int _soc_ml_flexio_scache_sync(int unit);

extern soc_error_t soc_metrolite_get_mxq_phy_port_mode(
        int unit, soc_port_t port,int speed, bcmMxqPhyPortMode_t *mode);
extern soc_error_t soc_metrolite_get_xl_phy_core_port_mode(
        int unit, soc_port_t port, bcmXlPhyPortMode_t *phy_mode,
        bcmXlCorePortMode_t *core_mode);

typedef struct bcm56270_tdm_info_s {
        uint8  tdm_freq;
        uint32  tdm_size;

        /* Display purpose only */
        uint8  row;
        uint8  col;
}bcm56270_tdm_info_t;

#define spn_BCM5627X_CONFIG  "bcm5627x_config"

/* Configs for BCM56270 */
#define BCM56270_DEVICE_ID_OFFSET_CFG 0
#define BCM56270_DEVICE_ID_DEFAULT_CFG 1
#define BCM56270_DEVICE_ID_MAX_CFG 1

/* Configs for BCM56271 */
#define BCM56271_DEVICE_ID_OFFSET_CFG 1
#define BCM56271_DEVICE_ID_DEFAULT_CFG 2
#define BCM56271_DEVICE_ID_MAX_CFG 4

/* Configs for BCM56272 */
#define BCM56272_DEVICE_ID_OFFSET_CFG 4
#define BCM56272_DEVICE_ID_DEFAULT_CFG 5
#define BCM56272_DEVICE_ID_MAX_CFG 6

typedef struct  ml_tdm_pos_info_s {
        uint16 total_slots;
        int16  pos[180];
} ml_tdm_pos_info_t;

#define ML_TDM_MAX_SIZE 204

extern uint32 ml_current_tdm[ML_TDM_MAX_SIZE];
extern uint32 ml_current_tdm_size;
extern ml_tdm_pos_info_t ml_tdm_pos_info[ML_MAX_BLOCKS];

extern uint32 ml_current_tdm[ML_TDM_MAX_SIZE];
extern uint32 ml_current_tdm_size;
extern ml_tdm_pos_info_t ml_tdm_pos_info[ML_MAX_BLOCKS];
extern void soc_ml_port_bitmap_add(int unit,soc_port_t port,int block,
                                   int speed, int new_lanes);
extern int soc_metrolite_bond_info_init(int unit);
#endif  /* !_SOC_METROLITE_H_ */
