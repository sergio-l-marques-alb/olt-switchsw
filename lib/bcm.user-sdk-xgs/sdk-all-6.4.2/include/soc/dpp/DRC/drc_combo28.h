/*
 * $Id: drc_combo28.h,v 1.1.2.12 Broadcom SDK $
 * $Copyright: Copyright 2012 Broadcom Corporation.
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
 * This file contains DPP DRC main structure and routine declarations for the Dram operation using PHY Combo28.
 *
 */
#ifndef _SOC_DPP_DRC_COMBO28_H
#define _SOC_DPP_DRC_COMBO28_H

/* SOC DPP includes */
#include <soc/dpp/dpp_config_defs.h>

/* SOC SAND includes */
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h> /* Mainly for SOC_SAND_NOF_BITS_IN_BYTE*/

/* SOC DPP TMC includes */
#include <soc/dpp/TMC/tmc_api_dram.h>

/* Combo28 Shmoo Header */
#include <soc/shmoo_combo28.h>


/* 
 * TMC (TM Common) reuse
 */

#define DRC_COMBO28_DRAM_CLAM_SHELL_MODE_DISABLED          SOC_TMC_DRAM_CLAM_SHELL_MODE_DISABLED
#define DRC_COMBO28_DRAM_CLAM_SHELL_MODE_DRAM_0            SOC_TMC_DRAM_CLAM_SHELL_MODE_DRAM_0
#define DRC_COMBO28_DRAM_CLAM_SHELL_MODE_DRAM_1            SOC_TMC_DRAM_CLAM_SHELL_MODE_DRAM_1
typedef SOC_TMC_DRAM_CLAM_SHELL_MODE                       DRC_COMBO28_DRAM_CLAM_SHELL_MODE;

/* 
 * Defines
 */

/* General Dram defines */
#define SOC_DPP_DRC_COMBO28_MRS_NUM_MAX (16)

#define SOC_DPP_DRC_COMBO28_NOF_BITS_IN_BYTE (SOC_SAND_NOF_BITS_IN_BYTE) /* Should be 8*/
#define SOC_DPP_DRC_COMBO28_NOF_PER_INTERFACE_BITS (32)
#define SOC_DPP_DRC_COMBO28_NOF_PER_INTERFACE_BYTES (SOC_DPP_DRC_COMBO28_NOF_PER_INTERFACE_BITS / SOC_DPP_DRC_COMBO28_NOF_BITS_IN_BYTE) /* should be 4 */

/* Dram Params General define */
#define SOC_DPP_DRC_COMBO28_VAL_IS_IN_CLOCKS_BIT (31)

/* DQ Swap Array values */
#define SOC_DPP_DRC_COMBO28_DQ_SWAP_VAL_DEFAULT_NO_SWAP (0xff)

/* Dram type define */
#define SOC_DPP_DRC_COMBO28_FLD_VAL_DRAM_TYPE_DDR4 0
#define SOC_DPP_DRC_COMBO28_FLD_VAL_DRAM_TYPE_GDDR5 1

/* AP place define */
#define SOC_DPP_DRC_COMBO28_FLD_VAL_DRC_AP_BIT_10 2
#define SOC_DPP_DRC_COMBO28_FLD_VAL_DRC_AP_BIT_8 0

/* 
 * ENUM
 */

/* 
 * Structures
 */

typedef struct soc_dpp_drc_combo28_info_dram_param_s {

    /*
     *  Row Refresh Cycle.
     *  Auto refresh command period. The minimal period between the refresh command and the next active command (tRFC).
     *  By default this period is stated in terms of picoseconds.
     *  To state it in terms of number of clocks add "c" suffix to the value (e.g. 1000 is 1000 ps, and 1000c is 1000 clocks).
     */
    uint32 t_rfc;

    /*
     *  Refresh Cycle.
     *  Period between the active to the active/auto refresh commands (tRC).
     *  By default this period is stated in terms of picoseconds.
     *  To state it in terms of number of clocks add "c" suffix to the value (e.g. 1000 is 1000 ps, and 1000c is 1000 clocks).
     */
    uint32 t_rc;

    /*
     *  Row address to Column address Delay.
     *  The minimal period needed between RAS and CAS. It is the time required between row activation and read access to the column of the given memory block (tRcdRd).
     *  By default this period is stated in terms of picoseconds.
     *  To state it in terms of number of clocks add "c" suffix to the value (e.g. 1000 is 1000 ps, and 1000c is 1000 clocks).
     */
    uint32 t_rcd_rd;
    /*
     *  Row address to Column address Delay.
     *  The minimal period needed between RAS and CAS. It is the time required between row activation and write access to the column of the given memory block (tRcdWr).
     *  By default this period is stated in terms of picoseconds.
     *  To state it in terms of number of clocks add "c" suffix to the value (e.g. 1000 is 1000 ps, and 1000c is 1000 clocks).
     */
    uint32 t_rcd_wr;

    /*
     *  RAS To RAS delay (Same).
     *  ACTIVATE to ACTIVATE Command delay to same bank group (tRRD_L).
     *  By default this period is stated in terms of picoseconds.
     *  To state it in terms of number of clocks add "c" suffix to the value (e.g. 1000 is 1000 ps, and 1000c is 1000 clocks).
     */
    uint32 t_rrd_l;

    /*
     *  RAS To RAS delay (Diff).
     *  ACTIVATE to ACTIVATE Command delay to different bank group (tRRD_S).
     *  By default this period is stated in terms of picoseconds.
     *  To state it in terms of number of clocks add "c" suffix to the value (e.g. 1000 is 1000 ps, and 1000c is 1000 clocks).
     */
    uint32 t_rrd_s;

    /*
     *  Row Address Strobe.
     *  The minimal period needed to access a certain row of data in RAM between the data request and the precharge command (tRAS).
     *  By default this period is stated in terms of picoseconds.
     *  To state it in terms of number of clocks add "c" suffix to the value (e.g. 1000 is 1000 ps, and 1000c is 1000 clocks).
     */
    uint32 t_ras;

    /*
     *  Row Precharge.
     *  The minimal period between pre-charge action of a certain Row and the next consecutive action to the same bank/row (tRP).
     *  By default this period is stated in terms of picoseconds.
     *  To state it in terms of number of clocks add "c" suffix to the value (e.g. 1000 is 1000 ps, and 1000c is 1000 clocks).
     */
    uint32 t_rp;

    /*
     *  Write Recovery Time.
     *  Specifies the period that must elapse after the completion of a valid write operation, before a pre-charge command can be issued (tWR)
     *  By default this period is stated in terms of picoseconds.
     *  To state it in terms of number of clocks add "c" suffix to the value (e.g. 1000 is 1000 ps, and 1000c is 1000 clocks).
     */
    uint32 t_wr;

    /*
     *  Four Active Window.
     *  No more than four banks may be activated in a rolling window (tFAW).
     *  By default this period is stated in terms of picoseconds.
     *  To state it in terms of number of clocks add "c" suffix to the value (e.g. 1000 is 1000 ps, and 1000c is 1000 clocks).
     */
    uint32 t_faw;

    /*
     *  Thirty two bank activate window.
     *  No more than 32 banks may be activated in a rolling t32AW window. (t32AW).
     *  By default this period is stated in terms of picoseconds.
     *  To state it in terms of number of clocks add "c" suffix to the value (e.g. 1000 is 1000 ps, and 1000c is 1000 clocks).
     */
    uint32 t_32aw;

    /*
     *  READ to PRECHARGE command delay same bank with bank groups enabled (tRTP_L)
     *  By default this period is stated in terms of picoseconds.
     *  To state it in terms of number of clocks add "c" suffix to the value (e.g. 1000 is 1000 ps, and 1000c is 1000 clocks).
     */
    uint32 t_rtp_l;

    /*
     *  READ to PRECHARGE command delay different bank with bank groups enabled (tRTP_S)
     *  By default this period is stated in terms of picoseconds.
     *  To state it in terms of number of clocks add "c" suffix to the value (e.g. 1000 is 1000 ps, and 1000c is 1000 clocks).
     */
    uint32 t_rtp_s;

    /*
     *  Write To Read Delay (Same).
     *  The minimal period that must elapse between the last valid write operation and the next read command to the same internal bank of the DDR device (tWTR_L).
     *  By default this period is stated in terms of picoseconds.
     *  To state it in terms of number of clocks add "c" suffix to the value (e.g. 1000 is 1000 ps, and 1000c is 1000 clocks).
     */
    uint32 t_wtr_l;

    /*
     *  Write To Read Delay (Diff).
     *  The minimal period that must elapse between the last valid write operation and the next read command to different internal bank of the DDR device (tWTR_S).
     *  By default this period is stated in terms of picoseconds.
     *  To state it in terms of number of clocks add "c" suffix to the value (e.g. 1000 is 1000 ps, and 1000c is 1000 clocks).
     */
    uint32 t_wtr_s;

    /*
     *  RD/WR bank A to RD/WR bank B command delay same bank group (tCCD_L).
     *  By default this period is stated in terms of picoseconds.
     *  To state it in terms of number of clocks add "c" suffix to the value (e.g. 1000 is 1000 ps, and 1000c is 1000 clocks).
     */
    uint32 t_ccd_l;

    /*
     *  RD/WR bank A to RD/WR bank B command delay different bank group (tCCD_S).
     *  By default this period is stated in terms of picoseconds.
     *  To state it in terms of number of clocks add "c" suffix to the value (e.g. 1000 is 1000 ps, and 1000c is 1000 clocks).
     */
    uint32 t_ccd_s;

    /*
     *  Normal operation Short calibration time (tZQCS).
     *  By default this period is stated in terms of picoseconds.
     *  To state it in terms of number of clocks add "c" suffix to the value (e.g. 1000 is 1000 ps, and 1000c is 1000 clocks).
     */
    uint32 t_zqcs;

    /*
     *  Average periodic refresh interval (tREFI).
     *  By default this period is stated in terms of picoseconds.
     *  To state it in terms of number of clocks add "c" suffix to the value (e.g. 1000 is 1000 ps, and 1000c is 1000 clocks).
     */
    uint32 t_ref;

    /*
     *  CRC error to ALERT_n latency (tCRC_ALERT).
     *  By default this period is stated in terms of picoseconds.
     *  To state it in terms of number of clocks add "c" suffix to the value (e.g. 1000 is 1000 ps, and 1000c is 1000 clocks).
     */
    uint32 t_crc_alert;

    /*
     *  Number of clocks to wait after reset (tRST).
     *  By default this period is stated in terms of picoseconds.
     *  To state it in terms of number of clocks add "c" suffix to the value (e.g. 1000 is 1000 ps, and 1000c is 1000 clocks).
     */
    uint32 t_rst;

    /* 
     * Additive latency add to user (tAL). 
     * By default this period is stated in terms of picoseconds.
     * To state it in terms of number of clocks add "c" suffix to the value (e.g. 1000 is 1000 ps, and 1000c is 1000 clocks). 
     */
    uint32 t_al;

    /* 
     * CRC Read Latency (tCRCRL).
     * By default this period is stated in terms of picoseconds.
     * To state it in terms of number of clocks add "c" suffix to the value (e.g. 1000 is 1000 ps, and 1000c is 1000 clocks). 
     */
    uint32 t_crc_rd_latency;

    /* 
     * CRC Write Latency (tCRCWL).
     * By default this period is stated in terms of picoseconds.
     * To state it in terms of number of clocks add "c" suffix to the value (e.g. 1000 is 1000 ps, and 1000c is 1000 clocks). 
     */
    uint32 t_crc_wr_latency;

    /*
     *  Column Address Strobe latency (tCL).
     *  The period (clocks) between READ command and valid read data presented on the data out pins of the dram.
     */
    uint32 c_cas_latency;

    /*
     *  The period (clocks) between write command and write data set on the dram data in pins (tCWL)
     */
    uint32 c_wr_latency;

    /* 
     *  Wait period (Clocks) between commands during INIT sequence. Range: 0-0x3ff
     *  replace the following Dram Parameters: DDRtMRD, DDRtMOD, DDRtXPR, DDRtATH, DDRtATS, DDRtODTON. 
     */
    uint32 init_wait_period;

} soc_dpp_drc_combo28_info_dram_param_t;


typedef struct soc_dpp_drc_combo28_info_dram_internal_param_s {

    /* DDRtCK in ps */
    uint32 t_ck;

    /* Write lattency. In clocks. */
    uint32 c_wl;

    /* Average periodic refresh interval (tREFI). In clocks. */
    uint32 c_refi;

    /* Layout-specific timing correction values poked into them to allow for the different trace lengths to different data bus pins of the RAM modules.  */
    uint32 c_pcb;

    /* Row Refresh Cycle (tRFC). In clocks for DRC field */
    uint32 c_rfc_f;

    /* Row Address Strobe (tRAS). In clocks. */
    uint32 c_ras;

    /* Row Address Strobe (tRAS). In clocks for MR. */
    uint32 c_mr_ras;

    /* Row Precharge (tRP). In clocks. */
    uint32 c_rp;

    /* Write Recovery Time (tWR). In clocks. */
    uint32 c_wr;

    /* Write Recovery Time (tWR). In clocks for MR. */
    uint32 c_mr_wr;

    /* READ to PRECHARGE - same (tRTP_L). In clocks. */
    uint32 c_rtp_l;

    /* Write To Read Delay - same (tWTR_L). In clocks. */
    uint32 c_wtr_l;

    /* Write To Read Delay - diff (tWTR_S). In clocks. */
    uint32 c_wtr_s;

    /* D/WR to RD/WR - same (tCCD_L). In clocks. */
    uint32 c_ccd_l;

    /* D/WR to RD/WR - diff (tCCD_S). In clocks. */
    uint32 c_ccd_s;

    /* Additive latency add to user (tAL). In clocks. */
    uint32 c_al;

    /* CRC Read Latency (tCRCRL). In clocks. */
    uint32 c_crc_rd_latency;

    /* CRC Write Latency (tCRCWRL). In clocks. */
    uint32 c_crc_wr_latency;

    /* The number of drc clocks in a full data burst */
    uint32 bl;

    /* DQS preamble mode of 1CK */
    uint32 dqs_prem_1_ck;

    /* write recovery opcode */
    uint32 wr_recovery_op;

    /* burst chop enable */
    uint32 burst_chop_en;

} soc_dpp_drc_combo28_info_dram_internal_param_t;


typedef struct soc_dpp_drc_combo28_info_s {

    /* 
     * Device (which uses the DRC) parameters 
     */ 

    /* Device Core freq */
    uint32 device_core_freq;

    /* Enable/disable DRAM configuration */
    uint8 enable;

    /* Dram bitmap */
    soc_pbmp_t dram_bitmap;

    /* Number of Drams bitmap */
    int dram_num;

    /* Dram freq */
    int dram_freq;

    /* Data Rate in MBPS (= Dram Freq * 2 + 1) */
    int data_rate_mbps;

    /* Dram ref CLK in MHZ */
    int ref_clk_mhz;

    /* DRAM type. */
    uint32 dram_type;

    /* Number of Banks. */
    uint32 nof_banks;

    /* Number of DRAM columns. Range: 256/512/1024/2048/4096/8192 */
    uint32 nof_columns;

    /* Number of DRAM rows. Range: 8192/16384 */
    uint32 nof_rows;
    
    /* Dram configuration mode */
    soc_dpp_drc_combo28_info_dram_param_t dram_param;

    /* Dram configuration mode */
    soc_dpp_drc_combo28_info_dram_internal_param_t dram_int_param;

    /* MR registers */
    uint32 mr[ SOC_DPP_DRC_COMBO28_MRS_NUM_MAX];

    /* Dram clam shell mode */
    DRC_COMBO28_DRAM_CLAM_SHELL_MODE dram_clam_shell_mode[SOC_DPP_DEFS_MAX(HW_DRAM_INTERFACES_MAX)];

    /* Dram clam shell mode. dram_dq_swap[DRC][Byte][Bit] = Map to bit */
    uint32 dram_dq_swap[SOC_DPP_DEFS_MAX(HW_DRAM_INTERFACES_MAX)][SOC_DPP_DRC_COMBO28_NOF_PER_INTERFACE_BYTES][SOC_DPP_DRC_COMBO28_NOF_BITS_IN_BYTE]; 

    /* 
     *  Features
     */
     
    /* Gear down mode */
    uint32 gear_down_mode;

    /* Address bus inversion */
    uint32 abi;

    /* Data bus inversion on write direction */
    uint32 write_dbi;

    /* Data bus inversion on read direction */
    uint32 read_dbi;

    /* Write crc */
    uint32 write_crc;
    
    /* Read crc */
    uint32 read_crc;

    /* Command parity latency */
    uint32 cmd_par_latency;

    /* 
     * Dram Tuning (Calibration/Shmoo).
     *   if 2 then Use Dram saved config Parameters, if no Parameters perform Shmoo on init. Default option.                                                               ..
     *   if 1 then Perform Shmoo on init.
     *   if 0 then Use Dram saved config Parameters, if no Parameters do nothing.
     */
    int auto_tune;

    combo28_shmoo_config_param_t shmoo_config_param[SOC_DPP_DEFS_MAX(HW_DRAM_INTERFACES_MAX)];

    /* Enable/disable Dram BIST on initialization */
    uint8 bist_enable;

    /* 
     * Emulation/Simulatore/PCID system mode 
     * if set to 0 than Perfom Normal operation (Default) 
     */
    int sim_system_mode;

} soc_dpp_drc_combo28_info_t;

/*
 * Utility functions 
 */
 
/* CalmShell Bits swap function  */
int soc_dpp_drc_combo28_util_calm_shell_bits_swap(int unit, uint32 in_buf, uint32 *out_buf);

/* DRAM CPU command functions */
int soc_dpp_drc_combo28_dram_cpu_command(int unit, int drc_ndx, uint32 ras_n, uint32 cas_n, uint32 we_n, uint32 act_n, uint32 bank, uint32 address);
int soc_dpp_drc_combo28_shmoo_load_mrs(int unit, int drc_ndx, uint32 mr_ndx, uint32 mrs_opcode);

/*
 * Configuration Functions
 */
int soc_dpp_drc_combo28_info_config_default(int unit, soc_dpp_drc_combo28_info_t *drc_info);
int soc_dpp_drc_combo28_info_config(int unit, soc_dpp_drc_combo28_info_t *drc_info);
int soc_dpp_drc_combo28_shmoo_cfg_get(int unit, int drc_ndx, combo28_shmoo_config_param_t* shmoo_config_param);
int soc_dpp_drc_combo28_shmoo_cfg_set(int unit, int drc_ndx, combo28_shmoo_config_param_t* shmoo_config_param);
int soc_dpp_drc_combo28_enable_wr_crc(int unit, int drc_ndx, int enable);

/*
 * PHY access Functions
 */
int soc_dpp_drc_combo28_phy_reg_write(int unit, int drc_ndx, uint32 addr, uint32 data);
int soc_dpp_drc_combo28_phy_reg_read(int unit, int drc_ndx, uint32 addr, uint32 *data);
int soc_dpp_drc_combo28_phy_reg_modify(int unit, int drc_ndx, uint32 addr, uint32 data, uint32 mask);



#endif /* !_SOC_DPP_DRC_COMBO28_H  */
