/*
 * $Id: saber2.h,v 1.1.2.1 Broadcom SDK $
 * $Copyright: Copyright 2015 Broadcom Corporation.
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
 * File:        saber2.h
 */

#ifndef _SOC_SABER2_H_
#define _SOC_SABER2_H_

#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/error.h>
#include <soc/regulator.h>
#include <soc/katana2.h>
#include <soc/shmoo_and28.h>
typedef struct sb2_pbmp_s {
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
}sb2_pbmp_t;


typedef struct  sb2_tdm_pos_info_s {
        uint16 total_slots;
        int16  pos[180];
} sb2_tdm_pos_info_t;

typedef struct bcm56260_tdm_info_s {
        uint8  tdm_freq;
        uint32  tdm_size;

        /* Display purpose only */
        uint8  row;
        uint8  col;
}bcm56260_tdm_info_t;

typedef enum bcmXlPhyPortMode_s {
    bcmXlPhyPortModeQuad=0,
    bcmXlPhyPortModeTri012=1,
    bcmXlPhyPortModeTri023=2,
    bcmXlPhyPortModeDual=3,
    bcmXlPhyPortModeSingle=4,
    bcmXlPhyPortModeTdmDisable=5
}bcmXlPhyPortMode_t;

typedef enum bcmXlCorePortMode_s {
    bcmXlCorePortModeQuad=0,
    bcmXlCorePortModeTri012=1,
    bcmXlCorePortModeTri023=2,
    bcmXlCorePortModeDual=3,
    bcmXlCorePortModeSingle=4,
    bcmXlCorePortModeTdmDisable=5
}bcmXlCorePortMode_t;

#define SB2_BIST_FLAGS_CONS_ADDR_8_BANKS            (0x0001)
#define SB2_BIST_FLAGS_ADDRESS_SHIFT_MODE           (0X0002)
#define SB2_BIST_FLAGS_INFINITE                     (0x0004)
#define SB2_BIST_FLAGS_ALL_ADDRESS                  (0x0008)
#define SB2_BIST_FLAGS_STOP                         (0x0010)
#define SB2_BIST_FLAGS_GET_DATA                     (0x0020)
#define SB2_BIST_FLAGS_TWO_ADDRESS_MODE             (0x0040)
#define SB2_BIST_FLAGS_BG_INTERLEAVE                (0x0080)
#define SB2_BIST_FLAGS_SINGLE_BANK_TEST             (0x0100)
#define SB2_BIST_FLAGS_MPR_STAGGER_MODE             (0x0200)  
#define SB2_BIST_FLAGS_MPR_STAGGER_INCREMENT_MODE   (0x0400)
#define SB2_BIST_FLAGS_MPR_READOUT_MODE             (0x0800)
#define SB2_BIST_FLAGS_ADDRESS_PRBS_MODE            (0x1000)
#define SB2_BIST_FLAGS_USE_RANDOM_DATA_SEED         (0x2000)    
#define SB2_BIST_FLAGS_USE_RANDOM_DBI_SEED          (0x4000) 

#define SB2_BIST_FLAGS_USE_RANDOM_SEED \
    (SB2_BIST_FLAGS_USE_RANDOM_DATA_SEED | SB2_BIST_FLAGS_USE_RANDOM_DBI_SEED)


#define SB2_BIST_NUM_PATTERNS 8
#define SB2_BIST_NUM_SEEDS 8
#define SB2_BIST_MPR_NUM_PATTERNS 4

typedef enum
{
    SB2_BIST_MPR_MODE_SERIAL = 0,
    SB2_BIST_MPR_MODE_PARALLEL = 1,
    SB2_BIST_MPR_MODE_STAGARED = 2,
    SB2_BIST_MPR_DISABLE = 3
} SB2_BIST_MPR_MODE;

typedef struct {
    SB2_BIST_MPR_MODE mpr_mode;
    uint32 mpr_readout_mpr_location;
    uint8 mpr_pattern[SB2_BIST_MPR_NUM_PATTERNS];
} sb2_bist_mpr_info;

typedef enum {
    SB2_DRAM_BIST_DATA_PATTERN_CUSTOM = 0,
    SB2_DRAM_BIST_DATA_PATTERN_RANDOM_PRBS = 1,
    SB2_DRAM_BIST_DATA_PATTERN_DIFF = 2,
    SB2_DRAM_BIST_DATA_PATTERN_ONE = 3,
    SB2_DRAM_BIST_DATA_PATTERN_ZERO = 4,
    SB2_DRAM_BIST_DATA_PATTERN_BIT_MODE = 5,
    SB2_DRAM_BIST_DATA_PATTERN_SHIFT_MODE = 6,
    SB2_DRAM_BIST_DATA_PATTERN_ADDR_MODE = 7,
    SB2_DRAM_BIST_NOF_DATA_PATTERN_MODES = 8
}SB2_DRAM_BIST_DATA_PATTERN_MODE;

typedef struct {
    uint32 write_weight;
    uint32 read_weight;
    uint32 bist_num_actions;
    uint32 bist_start_address;
    uint32 bist_end_address;
    SB2_DRAM_BIST_DATA_PATTERN_MODE pattern_mode;
    uint32 pattern[SB2_BIST_NUM_PATTERNS];
    uint32 data_seed[SB2_BIST_NUM_SEEDS];
    uint32 bist_flags;
}sb2_bist_info;

#define SB2_MEM_GRADE_080808        0x080808
#define SB2_MEM_GRADE_090909        0x090909
#define SB2_MEM_GRADE_101010        0x101010
#define SB2_MEM_GRADE_111111        0x111111
#define SB2_MEM_GRADE_121212        0x121212
#define SB2_MEM_GRADE_131313        0x131313
#define SB2_MEM_GRADE_141414        0x141414

#define SB2_MEM_ROWS_16K        16384
#define SB2_MEM_ROWS_32K        32768

#define SB2_DDR_FREQ_667        667
#define SB2_DDR_FREQ_800        800

enum sb2_mem_size_set {
    SB2_MEM_4G,
    SB2_MEM_2G,
    SB2_MEM_1G,
    SB2_MEM_COUNT
};


enum sb2_freq_set {
   SB2_FREQ_667,
   SB2_FREQ_800,
   SB2_FREQ_CNT
};

enum sb2_grade_set {
    SB2_GRADE_DEFAULT,
    SB2_GRADE_080808,
    SB2_GRADE_090909,
    SB2_GRADE_101010,
    SB2_GRADE_111111,
    SB2_GRADE_121212,
    SB2_GRADE_131313,
    SB2_GRADE_141414,
    SB2_GRADE_CNT
};

#define SOC_SB2_AND28_INTERFACES_NUM_MAX            8
#define SOC_SB2_AND28_NOF_PER_INTERFACE_BYTES       4
#define SOC_SB2_AND28_NOF_BITS_IN_BYTE              8
#define SOC_SB2_AND28_MRS_NUM_MAX                   16


typedef struct {
    int twl, twr, trc;                                          /* CONFIG0 */
    int rfifo_afull, rfifo_full;                                /* CONFIG1 */
    int trtw, twtr, tfaw, tread_enb;                            /* CONFIG2 */
    int bank_unavail_rd, bank_unavail_wr, rr_read, rr_write;    /* CONFIG3 */
    int refrate;                                                /* CONFIG4 */
    int trp_read, trp_write, trfc[3];                           /* CONFIG6 */
    int tzq;                                                    /* CONFIG7 */
    int cl, cwl, wr;                                            /* PHY_STRAP0 */
    int jedec, mhz;                                             /* PHY_STRAP1 */
    uint32 mr0, mr2;                                            /* MR0, MR2   */
} sb2_req_grade_mem_set_set_t;

typedef struct soc_sb2_and28_info_s {
    uint32 device_core_freq;
    uint32 device_dram_num_max;
    uint8 enable;
    uint32 dram_bitmap;
    soc_pbmp_t ref_clk_bitmap;
    int dram_num;
    int dram_freq;
    int data_rate_mbps;
    int ref_clk_mhz;
    uint32 dram_type;
    uint32 nof_banks;
    uint32 nof_columns;
    uint32 nof_rows;
#if 0
    soc_sb2_and28_info_dram_param_t dram_param;
    soc_sb2_and28_info_dram_internal_param_t dram_int_param;
#endif
    uint32 mr[ SOC_SB2_AND28_MRS_NUM_MAX];
#if 0
    DRC_AND28_DRAM_CLAM_SHELL_MODE
        dram_clam_shell_mode[SOC_SB2_AND28_INTERFACES_NUM_MAX];
#endif
    uint32
           dram_dq_swap[SOC_SB2_AND28_INTERFACES_NUM_MAX][SOC_SB2_AND28_NOF_PER_INTERFACE_BYTES][SOC_SB2_AND28_NOF_BITS_IN_BYTE];

    uint32 gear_down_mode;
    uint32 alert_n_period_thrs;
    uint32 abi;
    uint32 write_dbi;
    uint32 read_dbi;
    uint32 write_crc;
    uint32 read_crc;
    uint32 cmd_par_latency;
    int auto_tune;
    and28_shmoo_config_param_t shmoo_config_param[SOC_SB2_AND28_INTERFACES_NUM_MAX];
    uint8 bist_enable;
    int sim_system_mode;
}soc_sb2_and28_info_t;

extern int soc_sb2_linkphy_port_reg_blk_idx_get(
    int unit, int port, int blktype, int *block, int *index);
extern int soc_sb2_linkphy_port_blk_idx_get(
    int unit, int port, int *block, int *index);
extern int soc_sb2_linkphy_get_portid(int unit, int block, int index);

extern void _saber2_phy_addr_default(int unit, int port,
    uint16 *phy_addr, uint16 *phy_addr_int);
extern soc_error_t soc_saber2_get_mxq_phy_port_mode(
        int unit, soc_port_t port,int speed, bcmMxqPhyPortMode_t *mode);
extern soc_error_t soc_saber2_get_xl_phy_core_port_mode(
        int unit, soc_port_t port, bcmXlPhyPortMode_t *phy_mode, 
        bcmXlCorePortMode_t *core_mode);

extern int soc_sb2_iecell_port_reg_blk_idx_get(
    int unit, int port, int blktype, int *block, int *index);
extern int soc_sb2_iecell_port_blk_idx_get(
    int unit, int port, int *block, int *index);
extern int soc_sb2_iecell_get_portid(int unit, int block, int index);
extern soc_error_t soc_saber2_port_enable_set(
       int unit, soc_port_t port, int enable);
extern soc_error_t soc_saber2_get_port_block(
       int unit, soc_port_t port,uint8 *block);

extern soc_functions_t soc_saber2_drv_funs;
extern int
soc_saber2_num_cosq_init(int unit);
extern int soc_sb2_dump_default_mmu(int unit); 
extern int soc_sb2_mem_config(int unit, int dev_id);
extern int _soc_saber2_mmu_reconfigure(int unit);

extern void soc_saber2_pbmp_init(int unit, sb2_pbmp_t sb2_pbmp);
extern void soc_saber2_subport_init(int unit);
extern void soc_saber2_block_reset(int unit, uint8 block,int active_low);
extern void soc_saber2_save_tdm_pos(int unit, uint32 new_tdm_size,uint32 *new_tdm);
extern soc_error_t soc_saber2_reconfigure_tdm(int unit,uint32 new_tdm_size,uint32 *new_tdm);

extern int _soc_saber2_flexio_scache_allocate(int unit);
extern int _soc_saber2_flexio_scache_retrieve(int unit);
extern int _soc_saber2_flexio_scache_sync(int unit);

extern int soc_sb2_temperature_monitor_get(int unit,
            int temperature_max,
            soc_switch_temperature_monitor_t *temperature_array,
            int *temperature_count);

#define SB2_TDM_MAX_SIZE           284 

#define SB2_MAX_BLOCKS             7
#define SB2_MAX_PORTS_PER_BLOCK    4

#define SB2_MAX_LOGICAL_PORTS           30 
#define SB2_MAX_PHYSICAL_PORTS          28 

#define SB2_LPBK                        29 
#define SB2_CMIC                        0
#define SB2_IDLE                        SB2_MAX_LOGICAL_PORTS

#define SB2_PORT_MAC_MAX       96

#define SB2_MAX_SERVICE_POOLS           4
#define SB2_MAX_PRIORITY_GROUPS         8

#define spn_BCM5626X_CONFIG  "bcm5626x_config"

#define SB2_MIN_SUBPORT_INDEX                 30
#define SB2_MAX_SUBPORTS                      64
#define SB2_MAX_LINKPHY_SUBPORTS_PER_PORT     64

/* Configs for BCM56460,461,465, 466 */
#define BCM56460_DEVICE_ID_OFFSET_CFG 0
#define BCM56460_DEVICE_ID_DEFAULT_CFG 1
#define BCM56460_DEVICE_ID_MAX_CFG 6

/* Configs for BCM56462,467 */
#define BCM56462_DEVICE_ID_OFFSET_CFG 6
#define BCM56462_DEVICE_ID_DEFAULT_CFG 7
#define BCM56462_DEVICE_ID_MAX_CFG 8

/* Configs for BCM56463,468 */
#define BCM56463_DEVICE_ID_OFFSET_CFG 8
#define BCM56463_DEVICE_ID_DEFAULT_CFG 9
#define BCM56463_DEVICE_ID_MAX_CFG 11

/* Configs for BCM56260,261,265,266 */
#define BCM56260_DEVICE_ID_OFFSET_CFG 11
#define BCM56260_DEVICE_ID_DEFAULT_CFG 12
#define BCM56260_DEVICE_ID_MAX_CFG 13

/* Configs for BCM56262,267 */
#define BCM56262_DEVICE_ID_OFFSET_CFG 13
#define BCM56262_DEVICE_ID_DEFAULT_CFG 14
#define BCM56262_DEVICE_ID_MAX_CFG 15

/* Configs for BCM56263,268 */
#define BCM56263_DEVICE_ID_OFFSET_CFG 15
#define BCM56263_DEVICE_ID_DEFAULT_CFG 16
#define BCM56263_DEVICE_ID_MAX_CFG 17

typedef uint32 sb2_port_speeds_t[SB2_MAX_BLOCKS][SB2_MAX_PORTS_PER_BLOCK];
typedef uint32 sb2_block_ports_t[SB2_MAX_BLOCKS][SB2_MAX_PORTS_PER_BLOCK];

typedef uint32 sb2_speed_t[SB2_MAX_PHYSICAL_PORTS];
typedef uint32 sb2_port_to_block_t[SB2_MAX_PHYSICAL_PORTS];
typedef uint32 sb2_port_to_block_subports_t[SB2_MAX_PHYSICAL_PORTS];

extern sb2_block_ports_t *sb2_block_ports[SOC_MAX_NUM_DEVICES];
extern sb2_port_speeds_t *sb2_port_speeds[SOC_MAX_NUM_DEVICES];

extern uint32 sb2_current_tdm[SB2_TDM_MAX_SIZE];
extern uint32 sb2_current_tdm_size;
extern uint8  sb2_tdm_update_flag;
extern sb2_tdm_pos_info_t sb2_tdm_pos_info[SB2_MAX_BLOCKS];

#ifdef INCLUDE_AVS
extern int soc_saber2_avs_init(int unit);
#endif /* INCLUDE_AVS */
extern int soc_saber2_and28_info_config(int unit, soc_sb2_and28_info_t *drc_info,
        and28_shmoo_dram_info_t *sdi);
extern int soc_sb2_and28_dram_init_reset(int unit);
extern int soc_sb2_and28_dram_init_config(int unit);
extern int soc_sb2_and28_dram_savecfg(int unit, and28_shmoo_config_param_t *config_param);
extern int soc_sb2_and28_dram_restorecfg(int unit, and28_shmoo_config_param_t *config_param);
extern int soc_saber2_hw_reset_control_init(int unit);
extern int bb_saber2_regulator_callback(int unit, regulator_func_t func,
        regulator_type_t type, regulator_config_t *rcfg);
extern int soc_saber2_ser_init(int unit,int enable);
extern soc_error_t soc_saber2_mem_parity_control(int unit, soc_mem_t mem, int copyno, int enable);
extern soc_error_t soc_saber2_ser_mem_clear(int unit, soc_mem_t mem);

extern int soc_sb2_cosq_max_bucket_set(int unit, int port,
                        int index, uint32 level);
extern int
_soc_ddr_sb2_phy_freqtoloc(uint32 freq);
extern int soc_saber2_xl_port_speed_get(int unit, int port, int *speed);

#endif  /* !_SOC_SABER2_H_ */
