/*
 * $Id: asf.c,v 1.1 Broadcom SDK $
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
 * File:      asf.c
 * Purpose:   ASF feature support for Tomahawk SKUs
 * Requires:  soc/tomahawk.h
 */

#include <soc/tomahawk.h>

#if defined(BCM_TOMAHAWK_SUPPORT) && !defined(BCM_TH_ASF_EXCLUDE)

#include <appl/diag/system.h>
#include <soc/drv.h>
#include <soc/ll.h>
#include <soc/property.h>
#include <shared/bsl.h>


#define _SOC_TH_ASF_QUERY   0xfe
#define _SOC_TH_ASF_RETRV   0xff

#define _SOC_TH_CT_CLASS_TO_SPEED_MAP(ct_class) \
        _soc_th_asf_cfg_tbl[(ct_class)].speed

#define MIN(x, y) (((x) < (y)) ? (x) : (y))

#define _SOC_TH_PORT_ASF_XMIT_START_COUNT_INIT \
        soc_th_port_asf_xmit_start_count_set

#define _SOC_TH_PORT_ASF_REINIT \
        soc_th_port_asf_init

#define _SOC_TH_ASF_SPEED_CLASS_VALIDATE(class, floor) \
        if (!(((class) >= (floor)) && ((class) <= 12)))   \
            return SOC_E_PARAM;

#define _SOC_TH_ASF_PORT_VALIDATE(unit, port) \
        if (IS_CPU_PORT(((unit)), (port)) || IS_LB_PORT(((unit)), (port)) || \
            SOC_PBMP_MEMBER(PBMP_MANAGEMENT(unit), (port))) \
            return SOC_E_UNAVAIL;

/* EP credit accumulation */
typedef struct _soc_th_ep_credit_acc_s {
    uint8 line_rate;       /* Line-rate profile */
    uint8 oversub;         /* General profile */
} _soc_th_ep_credit_acc_t;

/* MMU EP Credit Thresholds */
typedef struct _soc_th_mmu_ep_credit_s {
    uint8 base_cells;      /* Baseline cells */
    uint8 pfc_op_cells;    /* PFC Optimized cells */
} _soc_th_mmu_ep_credit_t;

/* EP Credits Outstanding */
typedef struct _soc_th_egr_mmu_cell_credit_s {
    uint8 base_cells;      /* Baseline cells */
    uint8 pfc_op_cells;    /* PFC Optimized cells */
} _soc_th_egr_mmu_cell_credit_t;

/* Transmit Start Count */
typedef struct _soc_th_egr_xmit_start_count_s {
    uint8 saf;             /* Store-And-Forward */
    uint8 oversub_2_1;     /* 2:1 Oversub */
    uint8 oversub_3_2;     /* 3:2 Oversub */
    uint8 line_rate;       /* Line-rate */
} _soc_th_egr_xmit_start_count_t;

/* ASF core config */
typedef struct _soc_th_asf_core_cfg_s {
    int                              speed;
    uint8                            min_sp;                   /* Min. Src. Port Speed */
    uint8                            max_sp;                   /* Max. Src. Port Speed */
    uint8                            min_src_port_xmit_cnt;    /* MMU prebuffer */
    _soc_th_ep_credit_acc_t          ep_credit_acc;            /* EP Credit Accumulation */
    uint8                            encap_acc;                /* Encap Accumulation */
    uint8                            mmu_prebuf_depth;         /* Extra MMU Prebuffer depth */
    uint8                            fast_to_slow_acc;         /* Fast to slow CT Accumulation */
    _soc_th_mmu_ep_credit_t          mmu_ep_credit;
    _soc_th_egr_mmu_cell_credit_t    egr_mmu_credit;
    _soc_th_egr_xmit_start_count_t   xmit_cnt_l2_latency;      /* L2 Latency Mode */
    _soc_th_egr_xmit_start_count_t   xmit_cnt_l3_full_latency; /* L3 & Full Latency Modes */
    /* NOTE: DON'T ALTER FIELD ORDER */
} _soc_th_asf_core_cfg_t;

/* ASF ctrl */
typedef struct _soc_th_asf_ctrl_s {
    uint8                     init;
    pbmp_t                    asf_ports;
    pbmp_t                    pause_restore;
    pbmp_t                    asf_ss;
    pbmp_t                    asf_sf;
    pbmp_t                    asf_fs;
    soc_th_asf_mem_profile_e  asf_mem_profile;
} _soc_th_asf_ctrl_t;

#ifdef BCM_WARM_BOOT_SUPPORT
/* ASF Warmboot */
typedef struct _soc_th_asf_wb_s {
    int unit;
    _soc_th_asf_ctrl_t asf_ctrl;
} _soc_th_asf_wb_t;
#endif

/* ASF Core Config Table  */
static const _soc_th_asf_core_cfg_t
_soc_th_asf_cfg_tbl[] = {
    {    -1,  0,  0,  0, {0,  0}, 0,  0,  0, { 0,  0}, { 0,   0}, {18,  0,  0,  0}, {18,  0,  0,  0}},   /* SAF     */
    { 10000,  1, 10,  0, {2,  8}, 6,  0, 38, { 9,  6}, {13,  11}, {18, 51, 42, 30}, {18, 60, 48, 27}},   /* 10GE    */
    { 11000,  1, 10,  6, {2,  8}, 6, 10, 38, { 9,  6}, {13,  11}, {18, 51, 42, 30}, {18, 60, 48, 27}},   /* HG[11]  */
    { 20000,  3, 10,  0, {2,  8}, 7,  0, 29, {13,  7}, {18,  16}, {18, 63, 48, 30}, {18, 54, 42, 21}},   /* 20GE    */
    { 21000,  3, 10,  7, {2,  8}, 7, 11, 29, {13,  7}, {18,  16}, {18, 63, 48, 30}, {18, 54, 42, 21}},   /* HG[21]  */
    { 25000,  5, 10,  0, {3,  8}, 6,  0, 25, {12,  7}, {16,  15}, {18, 60, 48, 27}, {18, 51, 36, 18}},   /* 25GE    */
    { 27000,  5, 10,  7, {3,  8}, 6, 11, 25, {12,  7}, {16,  15}, {18, 60, 48, 27}, {18, 51, 36, 18}},   /* HG[27]  */
    { 40000,  7, 12,  0, {3, 12}, 8,  0, 29, {18,  7}, {25,  19}, {18, 57, 45, 24}, {18, 42, 30, 12}},   /* 40GE    */
    { 42000,  7, 12,  7, {3, 12}, 8, 11, 29, {18,  7}, {25,  19}, {18, 57, 45, 24}, {18, 42, 30, 12}},   /* HG[42]  */
    { 50000,  7, 12, 20, {1, 12}, 8, 31, 25, {20, 17}, {27,  23}, {18, 54, 42, 21}, {18, 33, 27, 12}},   /* 50GE    */
    { 53000,  7, 12,  7, {1, 12}, 8, 11, 25, {20, 17}, {27,  23}, {18, 54, 42, 21}, {18, 33, 27, 12}},   /* HG[53]  */
    {100000, 11, 12,  0, {3,  3}, 8,  0,  3, {33, 27}, {44,  36}, {18, 48, 36, 18}, {18, 33, 27, 15}},   /* 100GE   */
    {106000, 11, 12,  6, {3,  3}, 8, 10,  3, {33, 27}, {44,  36}, {18, 48, 36, 18}, {18, 33, 27, 15}}    /* HG[106] */
};

/* ASF Control */
static _soc_th_asf_ctrl_t*
_soc_th_asf_ctrl[SOC_MAX_NUM_DEVICES];


/*
 * Cut-through class encoding:
 *   0 - SAF
 *   1 - 10GE
 *   2 - HG[11]
 *   3 - 20GE
 *   4 - 21[HG}
 *   5 - 25GE
 *   6 - HG[27]
 *   7 - 40GE
 *   8 - HG[42]
 *   9 - 50GE
 *   10 - HG[53]
 *   11 - 100GE
 *   12 - HG[106]
 */
STATIC int
_soc_th_speed_to_ct_class_map(int speed)
{
    int ct_class;

    for (ct_class = 0; ct_class <= 12; ct_class++) {
        if (_soc_th_asf_cfg_tbl[ct_class].speed == speed) {
            return ct_class;
        }
    }

    return SOC_E_PARAM;
}

/******************************************************************************
 * Name: _soc_th_port_asf_class_get                                           *
 * Description:                                                               *
 *     Get Cut-through class configured for the specified port                *
 * IN params:                                                                 *
 *     - Unit                                                                 *
 *     - Logical Port                                                         *
 *     - Port Speed                                                           *
 * OUT params:                                                                *
 *     - CT class                                                             *
 * Returns:                                                                   *
 *     - SOC_E_NONE on success                                                *
 *     - SOC_E_PARAM on invalid parameter                                     *
 ******************************************************************************/
STATIC int
_soc_th_port_asf_class_get(
    int             unit,
    soc_port_t      port,
    OUT int* const  class)
{
    egr_ip_cut_thru_class_entry_t entry;

    if (!class) {
        return SOC_E_PARAM;
    }

    sal_memset(&entry, 0, sizeof(egr_ip_cut_thru_class_entry_t));
    SOC_IF_ERROR_RETURN
        (READ_EGR_IP_CUT_THRU_CLASSm(unit, MEM_BLOCK_ALL, port, &entry));
    *class = soc_mem_field32_get(unit, EGR_IP_CUT_THRU_CLASSm, &entry,
                                 CUT_THRU_CLASSf);

    return SOC_E_NONE;
}

/******************************************************************************
 * Name: _soc_th_port_asf_class_init                                          *
 * Description:                                                               *
 *     Initialize Cut-through class for the specified port                    *
 * IN params:                                                                 *
 *     - Unit                                                                 *
 *     - Logical Port                                                         *
 *     - CT class                                                             *
 * Returns:                                                                   *
 *     - SOC_E_NONE on success                                                *
 *     - SOC_E_PARAM on invalid parameter                                     *
 ******************************************************************************/
STATIC int
_soc_th_port_asf_class_init(
    int          unit,
    soc_port_t   port,
    int          ct_class)
{
    egr_ip_cut_thru_class_entry_t entry;

    sal_memset(&entry, 0, sizeof(egr_ip_cut_thru_class_entry_t));
    soc_mem_field32_set(unit, EGR_IP_CUT_THRU_CLASSm, &entry,
                        CUT_THRU_CLASSf, ct_class);
    SOC_IF_ERROR_RETURN
        (WRITE_EGR_IP_CUT_THRU_CLASSm(unit, MEM_BLOCK_ALL, port, &entry));

    return SOC_E_NONE;
}

/******************************************************************************
 * Name: _soc_th_port_asf_xmit_start_count_get                                *
 * Description:                                                               *
 *     Query or retrieve the XMIT Start Count for the specified source class  *
 *     of the port depending on the magic number passed in xmit_cnt           *
 * IN params:                                                                 *
 *     - Unit                                                                 *
 *     - Logical Port                                                         *
 *     - Source Port's Class                                                  *
 *     - Destination Port's CT Class                                          *
 *     - ASF Mode                                                             *
 * IN/OUT params:                                                             *
 *     - xmit_cnt : IN:  carries magic number for query / retrieve            *
 *     - xmit_cnt : OUT: contains a valid XMIT START COUNT for the            *
 *       source port class, on success                                        *
 * Returns:                                                                   *
 *     - SOC_E_NONE on success                                                *
 *     - SOC_E_PARAM on invalid parameter                                     *
 *     - SOC_E_INTERNAL on init/soc errors                                    *
 ******************************************************************************/
STATIC int
_soc_th_port_asf_xmit_start_count_get(
    int                      unit,
    soc_port_t               port,
    int                      sc,
    int                      dc,
    soc_th_asf_mode_e        mode,
    IN_OUT uint8* const      xmit_cnt)
{
    soc_info_t                  *si;
    egr_xmit_start_count_entry_t entry;
    int                          ovs_ratio, port_idx;
    soc_pbmp_t                   th_ports, line_rate_ports, ovs_3_2_ports;
    soc_port_t                   th_port;
    int                          latency = SOC_SWITCH_BYPASS_MODE_NONE;
    int                          speed;
    int                          class;
    int                          pipe;
    soc_mem_t                    config_mem = INVALIDm;

    _SOC_TH_UNIT_VALIDATE(unit);

    if (!(si = &SOC_INFO(unit))) {
        return SOC_E_INTERNAL;
    }

    pipe = si->port_pipe[port];
    config_mem = SOC_MEM_UNIQUE_ACC(unit, EGR_XMIT_START_COUNTm)[pipe];

    if (_SOC_TH_ASF_MODE_CFG_UPDATE == mode) { /* translate to valid mode */
        /* reverse map speed class to speed, to surmount speed deficit */
        SOC_IF_ERROR_RETURN(_soc_th_port_asf_class_get(unit, port, &class));
        _SOC_TH_ASF_SPEED_CLASS_VALIDATE(class, 0);
        speed = _SOC_TH_CT_CLASS_TO_SPEED_MAP(class);
        SOC_IF_ERROR_RETURN(soc_th_port_asf_mode_get(unit, port, speed, &mode));
    }
    if (!xmit_cnt ||
        !((mode >= _SOC_TH_ASF_MODE_SAF) &&
          (mode <= _SOC_TH_ASF_MODE_FAST_TO_SLOW))) {
        return SOC_E_PARAM;
    }
    _SOC_TH_ASF_SPEED_CLASS_VALIDATE(sc, 0);
    _SOC_TH_ASF_SPEED_CLASS_VALIDATE(dc, 0);

    if (_SOC_TH_ASF_RETRV == *xmit_cnt) {
        *xmit_cnt = 0;
        port_idx = ((port % 34) * 16) + sc;

        sal_memset(&entry, 0, sizeof(egr_xmit_start_count_entry_t));
        SOC_IF_ERROR_RETURN(soc_mem_read(unit, config_mem, MEM_BLOCK_ALL,
                                                     port_idx, &entry));
        *xmit_cnt = soc_mem_field32_get(unit, config_mem,
                                        &entry, THRESHOLDf);
    } else if (_SOC_TH_ASF_QUERY == *xmit_cnt) {
        *xmit_cnt = 0;
        SOC_PBMP_CLEAR(th_ports);
        SOC_PBMP_ASSIGN(th_ports, PBMP_E_ALL(unit));
        SOC_PBMP_OR(th_ports, PBMP_HG_ALL(unit));
        SOC_PBMP_REMOVE(th_ports, PBMP_MANAGEMENT(unit));
        SOC_PBMP_CLEAR(line_rate_ports);
        SOC_PBMP_CLEAR(ovs_3_2_ports);

        PBMP_ITER(th_ports, th_port) {
            if (SOC_PBMP_MEMBER(si->oversub_pbm, th_port)) {
                SOC_IF_ERROR_RETURN(
                    soc_th_port_oversub_ratio_get(unit, th_port, &ovs_ratio));
                if (ovs_ratio <= 1500) { /* oversub ratio <= 3:2 */
                    SOC_PBMP_PORT_ADD(ovs_3_2_ports, th_port);
                }
            }
            else { /* Line-rate */
                SOC_PBMP_PORT_ADD(line_rate_ports, th_port);
            }
        }

#ifndef BCM_TH_LATENCY_EXCLUDE
        /* Get current switch latency mode & set xmit_cnt accordingly */
        SOC_IF_ERROR_RETURN(soc_th_latency_get(unit, &latency));
#endif

        if ((_SOC_TH_ASF_MODE_SAF == mode) || (sc < dc)) {
            *xmit_cnt = (SOC_SWITCH_BYPASS_MODE_LOW == latency) ?
            _soc_th_asf_cfg_tbl[sc].xmit_cnt_l2_latency.saf:
            _soc_th_asf_cfg_tbl[sc].xmit_cnt_l3_full_latency.saf;
        } else if (SOC_PBMP_EQ(th_ports, line_rate_ports)) {
            *xmit_cnt = (SOC_SWITCH_BYPASS_MODE_LOW == latency) ?
            _soc_th_asf_cfg_tbl[sc].xmit_cnt_l2_latency.line_rate:
            _soc_th_asf_cfg_tbl[sc].xmit_cnt_l3_full_latency.line_rate;
        } else if (SOC_PBMP_EQ(th_ports, ovs_3_2_ports)) {
            *xmit_cnt = (SOC_SWITCH_BYPASS_MODE_LOW == latency) ?
            _soc_th_asf_cfg_tbl[sc].xmit_cnt_l2_latency.oversub_3_2:
            _soc_th_asf_cfg_tbl[sc].xmit_cnt_l3_full_latency.oversub_3_2;
        } else {
            *xmit_cnt = (SOC_SWITCH_BYPASS_MODE_LOW == latency) ?
            _soc_th_asf_cfg_tbl[sc].xmit_cnt_l2_latency.oversub_2_1:
            _soc_th_asf_cfg_tbl[sc].xmit_cnt_l3_full_latency.oversub_2_1;
        }
    }

    return SOC_E_NONE;
}

/******************************************************************************
 * Name: _soc_th_port_asf_speed_limits_get                                    *
 * Description:                                                               *
 *     Query or retrieve min & max source port speeds for the specified port  *
 *     depending on the magic number passed in min_sp &                       *
 *     max_sp                                                                 *
 * IN params:                                                                 *
 *     - Unit                                                                 *
 *     - Logical Port                                                         *
 *     - Port Speed                                                           *
 *     - ASF Mode                                                             *
 * IN/OUT params:                                                             *
 *     - min_sp : IN:  carries magic number for query / retrieve              *
 *     - min_sp : OUT: contains a valid min source port speed                 *
 *       on success                                                           *
 *     - max_sp : IN:  carries magic number for query / retrieve              *
 *     - max_sp : OUT: contains a valid max source port speed                 *
 *       on success                                                           *
 * Returns:                                                                   *
 *     - SOC_E_NONE on success                                                *
 *     - SOC_E_PARAM on invalid parameter                                     *
 *     - SOC_E_INTERNAL on init/soc errors                                    *
 ******************************************************************************/
STATIC int
_soc_th_port_asf_speed_limits_get(
    int                   unit,
    soc_port_t            port,
    int                   port_speed,
    soc_th_asf_mode_e     mode,
    IN_OUT uint8* const   min_sp,
    IN_OUT uint8* const   max_sp)
{
    uint32 rval;
    int    speed_encoding;

    _SOC_TH_UNIT_VALIDATE(unit);
    _SOC_TH_ASF_PORT_VALIDATE(unit, port);
    if (!min_sp || !max_sp) {
        return SOC_E_PARAM;
    }

    if ((_SOC_TH_ASF_RETRV == *min_sp) &&
        (_SOC_TH_ASF_RETRV == *max_sp)) {
        *min_sp = *max_sp = 0;
        if (!_soc_th_asf_ctrl[unit]->init) {
            return SOC_E_INTERNAL;
        }

        SOC_IF_ERROR_RETURN(READ_ASF_EPORT_CFGr(unit, port, &rval));

        /* MIN_SRC_PORT_SPEED */
        *min_sp = soc_reg_field_get(unit, ASF_EPORT_CFGr, rval,
                                    MIN_SRC_PORT_SPEEDf);
        /* MAX_SRC_PORT_SPEED */
        *max_sp = soc_reg_field_get(unit, ASF_EPORT_CFGr, rval,
                                    MAX_SRC_PORT_SPEEDf);
    } else if ((_SOC_TH_ASF_QUERY == *min_sp) &&
               (_SOC_TH_ASF_QUERY == *max_sp)) {
        *min_sp = *max_sp = 0;
        speed_encoding = _soc_th_speed_to_ct_class_map(port_speed);
        _SOC_TH_ASF_SPEED_CLASS_VALIDATE(speed_encoding, 1);

        switch(mode) {
            case _SOC_TH_ASF_MODE_SAF:
                return SOC_E_NONE;

            case _SOC_TH_ASF_MODE_SAME_SPEED:
                *min_sp = *max_sp = speed_encoding;
                break;

            case _SOC_TH_ASF_MODE_SLOW_TO_FAST:
                *min_sp = _soc_th_asf_cfg_tbl[speed_encoding].min_sp;
                *max_sp = speed_encoding;
                break;

            case _SOC_TH_ASF_MODE_FAST_TO_SLOW:
                if (_SOC_TH_ASF_MEM_PROFILE_SIMILAR ==
                        _soc_th_asf_ctrl[unit]->asf_mem_profile) {
                    if (speed_encoding % 2) { /* IEEE */
                        /* HG counterpart */
                        *max_sp = speed_encoding + 1;
                    } else { /* HG */
                        *max_sp = speed_encoding;
                    }
                } else if (_SOC_TH_ASF_MEM_PROFILE_EXTREME ==
                               _soc_th_asf_ctrl[unit]->asf_mem_profile) {
                    *max_sp = _soc_th_asf_cfg_tbl[speed_encoding].max_sp;
                }
                *min_sp = speed_encoding;
                break;

            default:
                return SOC_E_PARAM;
        }
    }

    return SOC_E_NONE;
}

/******************************************************************************
 * Name: _soc_th_port_asf_mmu_prebuf_get                                      *
 * Description:                                                               *
 *     Query or retrieve MMU Prebuffer (Min Source Port XMIT Count) for the   *
 *     specified port depending on the magic number passed in mmu_prebuf      *
 * IN params:                                                                 *
 *     - Unit                                                                 *
 *     - Logical Port                                                         *
 *     - Port Speed                                                           *
 *     - ASF Mode                                                             *
 * IN/OUT params:                                                             *
 *     - mmu_prebuf : IN:  carries magic number for query / retrieve          *
 *     - mmu_prebuf : OUT: contains a valid Min Source Port XMIT Count on     *
 *       success                                                              *
 * Returns:                                                                   *
 *     - SOC_E_NONE on success                                                *
 *     - SOC_E_PARAM on invalid parameter                                     *
 *     - SOC_E_INTERNAL on init/soc errors                                    *
 ******************************************************************************/
STATIC int
_soc_th_port_asf_mmu_prebuf_get(
    int                   unit,
    soc_port_t            port,
    int                   port_speed,
    int                   min_sp,
    IN_OUT uint8* const   mmu_prebuf)
{
    uint32 rval;
    int    speed_encoding;

    _SOC_TH_UNIT_VALIDATE(unit);
    _SOC_TH_ASF_PORT_VALIDATE(unit, port);
    if (!mmu_prebuf) {
        return SOC_E_PARAM;
    }

    if (_SOC_TH_ASF_RETRV == *mmu_prebuf) {
        *mmu_prebuf = 0;

        if (!_soc_th_asf_ctrl[unit]->init) {
            return SOC_E_INTERNAL;
        }

        SOC_IF_ERROR_RETURN(READ_ASF_EPORT_CFGr(unit, port, &rval));
        *mmu_prebuf = soc_reg_field_get(unit, ASF_EPORT_CFGr, rval,
                                        MIN_SRC_PORT_XMIT_CNTf);
    } else if (_SOC_TH_ASF_QUERY == *mmu_prebuf) {
        *mmu_prebuf = 0;
        speed_encoding = _soc_th_speed_to_ct_class_map(port_speed);
        _SOC_TH_ASF_SPEED_CLASS_VALIDATE(speed_encoding, 1);

        /* special cases : 50GE & HG[53] */
        if ((9 == speed_encoding) || (10 == speed_encoding)) {
            if ((7 == min_sp) || (8 == min_sp)) {  /* 40GE or HG[42] */
                *mmu_prebuf = 20;
            } else if (9 == min_sp) {   /* HG[50] */
                *mmu_prebuf = 7;
            }
        } else {
            *mmu_prebuf =
                _soc_th_asf_cfg_tbl[speed_encoding].min_src_port_xmit_cnt;
        }
    }

    return SOC_E_NONE;
}

/******************************************************************************
 * Name: _soc_th_port_asf_fifo_threshold_get                                  *
 * Description:                                                               *
 *     Query or retrieve FIFO Threshold for the specified port depending on   *
 *     the magic number passed in fifo_threshold                              *
 * IN params:                                                                 *
 *     - Unit                                                                 *
 *     - Logical Port                                                         *
 *     - Port Speed                                                           *
 *     - ASF Mode                                                             *
 *     - Min Source Port Speed                                                *
 * IN/OUT params:                                                             *
 *     - fifo_threshold : IN:  carries magic number for query / retrieve      *
 *     - fifo_threshold : OUT: contains a valid FIFO Threshold on success     *
 * Returns:                                                                   *
 *     - SOC_E_NONE on success                                                *
 *     - SOC_E_PARAM on invalid parameter                                     *
 *     - SOC_E_INTERNAL on init/soc errors                                    *
 ******************************************************************************/
STATIC int
_soc_th_port_asf_fifo_threshold_get(
    int                   unit,
    soc_port_t            port,
    int                   port_speed,
    soc_th_asf_mode_e     mode,
    int                   min_sp,
    IN_OUT uint8* const   fifo_threshold)
{
    uint32 rval;
    int    ret, speed_encoding;
    uint8  mmu_prebuf = 0, oversub;

    _SOC_TH_UNIT_VALIDATE(unit);
    _SOC_TH_ASF_PORT_VALIDATE(unit, port);
    if (!fifo_threshold) {
        return SOC_E_PARAM;
    }

    if (_SOC_TH_ASF_RETRV == *fifo_threshold) {
        *fifo_threshold = 0;

        if (!_soc_th_asf_ctrl[unit]->init) {
            return SOC_E_INTERNAL;
        }

        SOC_IF_ERROR_RETURN(READ_ASF_EPORT_CFGr(unit, port, &rval));
        *fifo_threshold = soc_reg_field_get(unit, ASF_EPORT_CFGr, rval,
                                            FIFO_THRESHOLDf);
    } else if (_SOC_TH_ASF_QUERY == *fifo_threshold) {
        *fifo_threshold = 0;
        speed_encoding = _soc_th_speed_to_ct_class_map(port_speed);
        _SOC_TH_ASF_SPEED_CLASS_VALIDATE(speed_encoding, 1);

        oversub = (SOC_PBMP_MEMBER(SOC_INFO(unit).oversub_pbm, port) ? 1 : 0);

        switch(mode) {
            case _SOC_TH_ASF_MODE_SAME_SPEED:
                /* passthru */
            case _SOC_TH_ASF_MODE_FAST_TO_SLOW:
                if (oversub) {
                    *fifo_threshold = 10;
                } else {
                    *fifo_threshold = 3;
                }
                break;

            case _SOC_TH_ASF_MODE_SLOW_TO_FAST:
                if (oversub) {
                    *fifo_threshold = 10;
                } else {
                    *fifo_threshold = 3;
                }

                mmu_prebuf = _SOC_TH_ASF_QUERY;
                ret = _soc_th_port_asf_mmu_prebuf_get(unit, port, port_speed,
                                                      min_sp, &mmu_prebuf);
                if (SOC_E_NONE != ret) {
                    return ret;
                }

                *fifo_threshold += mmu_prebuf;
                break;

            default:
                return SOC_E_PARAM;
        }
    }

    return SOC_E_NONE;
}

/******************************************************************************
 * Name: _soc_th_port_asf_fifo_depth_get                                      *
 * Description:                                                               *
 *     Query or retrieve FIFO Depth for the specified port depending on the   *
 *     magic number passed in buf_sz                                          *
 * IN params:                                                                 *
 *     - Unit                                                                 *
 *     - Logical Port                                                         *
 *     - Port Speed                                                           *
 *     - ASF Mode                                                             *
 *     - Min Src Port Speed                                                   *
 * IN/OUT params:                                                             *
 *     - fifo_depth : IN:  carries magic number for query/retrieve            *
 *     - fifo_depth : OUT: contains a valid FIFO Depth on success             *
 * Returns:                                                                   *
 *     - SOC_E_NONE on success                                                *
 *     - SOC_E_PARAM on invalid parameter                                     *
 *     - SOC_E_INTERNAL on init/soc errors                                    *
 ******************************************************************************/
STATIC int
_soc_th_port_asf_fifo_depth_get(
    int                   unit,
    soc_port_t            port,
    int                   port_speed,
    soc_th_asf_mode_e     mode,
    int                   min_sp,
    IN_OUT uint8* const   fifo_depth)
{
    uint32 rval;
    int    speed_encoding;
    uint8  oversub, mmu_prebuf_depth = 0;
    uint8  same_speed_ct_depth = 0, same_speed_ct_threshold = 0;

    _SOC_TH_UNIT_VALIDATE(unit);
    _SOC_TH_ASF_PORT_VALIDATE(unit, port);
    if (!fifo_depth) {
        return SOC_E_PARAM;
    }

    if (_SOC_TH_ASF_RETRV == *fifo_depth) {
        if (!_soc_th_asf_ctrl[unit]->init) {
            return SOC_E_INTERNAL;
        }

        *fifo_depth = 0;
        SOC_IF_ERROR_RETURN(READ_ASF_EPORT_CFGr(unit, port, &rval));
        *fifo_depth = soc_reg_field_get(unit, ASF_EPORT_CFGr, rval,
                                        FIFO_DEPTHf);
    } else if (_SOC_TH_ASF_QUERY == *fifo_depth) {
        *fifo_depth = 0;
        speed_encoding = _soc_th_speed_to_ct_class_map(port_speed);
        _SOC_TH_ASF_SPEED_CLASS_VALIDATE(speed_encoding, 1);

        oversub = (SOC_PBMP_MEMBER(SOC_INFO(unit).oversub_pbm, port) ? 1 : 0);

        switch(mode) {
            case _SOC_TH_ASF_MODE_SAME_SPEED:
                if (oversub) {
                    *fifo_depth =
                    (_soc_th_asf_cfg_tbl[speed_encoding].ep_credit_acc.oversub +
                     _soc_th_asf_cfg_tbl[speed_encoding].encap_acc + 17);
                } else {
                    *fifo_depth =
                    (_soc_th_asf_cfg_tbl[speed_encoding].ep_credit_acc.line_rate + 6);
                }
                break;

            case _SOC_TH_ASF_MODE_SLOW_TO_FAST:
                if (oversub) {
                    *fifo_depth =
                    (_soc_th_asf_cfg_tbl[speed_encoding].ep_credit_acc.oversub +
                     _soc_th_asf_cfg_tbl[speed_encoding].encap_acc + 17);
                } else {
                    *fifo_depth =
                    (_soc_th_asf_cfg_tbl[speed_encoding].ep_credit_acc.line_rate + 6);
                }

                /* Extra MMU prebuffer depth */
                if ((9 == speed_encoding) || (10 == speed_encoding)) { /* special cases : 50GE & HG[53] */
                    if ((7 == min_sp) || (8 == min_sp)) {  /* 40GE or HG[42] */
                        mmu_prebuf_depth = 31;
                    } else if (9 == min_sp) {   /* HG[50] */
                        mmu_prebuf_depth = 11;
                    }
                } else {
                    mmu_prebuf_depth = _soc_th_asf_cfg_tbl[speed_encoding].mmu_prebuf_depth;
                }

                *fifo_depth += mmu_prebuf_depth;
                break;

            case _SOC_TH_ASF_MODE_FAST_TO_SLOW:
                if (oversub) {
                    same_speed_ct_depth =
                    (_soc_th_asf_cfg_tbl[speed_encoding].ep_credit_acc.oversub +
                     _soc_th_asf_cfg_tbl[speed_encoding].encap_acc + 17);

                    same_speed_ct_threshold = 10;
                } else {
                    same_speed_ct_depth =
                    (_soc_th_asf_cfg_tbl[speed_encoding].ep_credit_acc.line_rate + 6);

                    same_speed_ct_threshold = 3;
                }

                *fifo_depth =
                 MIN((same_speed_ct_depth + _soc_th_asf_cfg_tbl[speed_encoding].fast_to_slow_acc),
                     (same_speed_ct_threshold + 46) );
                break;

            default:
                return SOC_E_PARAM;
        }
    }

    return SOC_E_NONE;
}

/******************************************************************************
 * Name: _soc_th_asf_obm_ca_fifo_thresh_set                                   *
 * Description:                                                               *
 *     Configure Cell Assembly CT FIFO threshold for all OBM and Pipe         *
 *     instances.                                                             *
 * IN params:                                                                 *
 *     - Unit                                                                 *
 *     - Logical Port                                                         *
 *     - Cell Assembly CT Threshold                                           *
 * Returns:                                                                   *
 *     - SOC_E_NONE on success                                                *
 *     - SOC_E_* on internal errors                                           *
 ******************************************************************************/
STATIC int
_soc_th_asf_obm_ca_fifo_thresh_set(
    int     unit,
    soc_port_t    port,
    uint8   ca_thresh)
{
    int  obm, pipe, clport;
    int port_block_base, phy_port, lane;
    uint32 rval;
    soc_info_t *si = &SOC_INFO(unit);
    uint32 thresh_field[4] = { THRESHOLD0f, THRESHOLD1f,
           THRESHOLD2f, THRESHOLD3f };
    uint32 port_ct_sel_field[4] = { PORT0_CT_SELf, PORT1_CT_SELf,
           PORT2_CT_SELf, PORT3_CT_SELf };
    uint32 idb_obm_ca_ct_ctrl_reg[8][4] = {
           {IDB_OBM0_CA_CT_CONTROL_PIPE0r, IDB_OBM0_CA_CT_CONTROL_PIPE1r,
            IDB_OBM0_CA_CT_CONTROL_PIPE2r, IDB_OBM0_CA_CT_CONTROL_PIPE3r},
           {IDB_OBM1_CA_CT_CONTROL_PIPE0r, IDB_OBM1_CA_CT_CONTROL_PIPE1r,
            IDB_OBM1_CA_CT_CONTROL_PIPE2r, IDB_OBM1_CA_CT_CONTROL_PIPE3r},
           {IDB_OBM2_CA_CT_CONTROL_PIPE0r, IDB_OBM2_CA_CT_CONTROL_PIPE1r,
            IDB_OBM2_CA_CT_CONTROL_PIPE2r, IDB_OBM2_CA_CT_CONTROL_PIPE3r},
           {IDB_OBM3_CA_CT_CONTROL_PIPE0r, IDB_OBM3_CA_CT_CONTROL_PIPE1r,
            IDB_OBM3_CA_CT_CONTROL_PIPE2r, IDB_OBM3_CA_CT_CONTROL_PIPE3r},
           {IDB_OBM4_CA_CT_CONTROL_PIPE0r, IDB_OBM4_CA_CT_CONTROL_PIPE1r,
            IDB_OBM4_CA_CT_CONTROL_PIPE2r, IDB_OBM4_CA_CT_CONTROL_PIPE3r},
           {IDB_OBM5_CA_CT_CONTROL_PIPE0r, IDB_OBM5_CA_CT_CONTROL_PIPE1r,
            IDB_OBM5_CA_CT_CONTROL_PIPE2r, IDB_OBM5_CA_CT_CONTROL_PIPE3r},
           {IDB_OBM6_CA_CT_CONTROL_PIPE0r, IDB_OBM6_CA_CT_CONTROL_PIPE1r,
            IDB_OBM6_CA_CT_CONTROL_PIPE2r, IDB_OBM6_CA_CT_CONTROL_PIPE3r},
           {IDB_OBM7_CA_CT_CONTROL_PIPE0r, IDB_OBM7_CA_CT_CONTROL_PIPE1r,
            IDB_OBM7_CA_CT_CONTROL_PIPE2r, IDB_OBM7_CA_CT_CONTROL_PIPE3r}};

    _SOC_TH_UNIT_VALIDATE(unit);

    /* Get lane, pipe & obm */
    phy_port = si->port_l2p_mapping[port];
    port_block_base = PORT_BLOCK_BASE_PORT(port);
    lane = phy_port - port_block_base;
    pipe = si->port_pipe[port];
    clport = si->port_serdes[port];
    /* obm number is reversed (mirrored) in odd pipe */
    obm = pipe & 1 ? 7 - (clport & 0x7) : clport & 0x7;

    SOC_IF_ERROR_RETURN(
        soc_reg32_get(unit, idb_obm_ca_ct_ctrl_reg[obm][pipe],
                      REG_PORT_ANY, 0, &rval));

    soc_reg_field_set(unit, idb_obm_ca_ct_ctrl_reg[obm][pipe], &rval,
              port_ct_sel_field[lane], lane);
    soc_reg_field_set(unit, idb_obm_ca_ct_ctrl_reg[obm][pipe], &rval,
              thresh_field[lane], ca_thresh);

    SOC_IF_ERROR_RETURN(
        soc_reg32_set(unit, idb_obm_ca_ct_ctrl_reg[obm][pipe],
                              REG_PORT_ANY, 0, rval));

    return SOC_E_NONE;
}

/******************************************************************************
 * Name: _soc_th_asf_obm_bubble_mop_set                                       *
 * Description:                                                               *
 *     Configure Bubble MOP for all OBM and Pipe Instances                    *
 * IN params:                                                                 *
 *     - Unit                                                                 *
 *     - Logical Port                                                         *
 *     - Bubble MOP Flag                                                      *
 * Returns:                                                                   *
 *     - SOC_E_NONE on success                                                *
 *     - SOC_E_* on internal errors                                           *
 ******************************************************************************/
STATIC int
_soc_th_asf_obm_bubble_mop_set(
    int     unit,
    soc_port_t    port,
    uint8   bubble_mop_disable)
{
    int  obm, pipe, clport;
    int port_block_base, phy_port, lane;
    uint32 rval;
    soc_info_t *si = &SOC_INFO(unit);
    uint32 port_field[4] = {
            PORT0_BUBBLE_MOP_DISABLEf, PORT1_BUBBLE_MOP_DISABLEf,
            PORT2_BUBBLE_MOP_DISABLEf, PORT3_BUBBLE_MOP_DISABLEf };
    uint32 idb_obm_ctrl_reg[8][4] = {
           {IDB_OBM0_CONTROL_PIPE0r, IDB_OBM0_CONTROL_PIPE1r,
            IDB_OBM0_CONTROL_PIPE2r, IDB_OBM0_CONTROL_PIPE3r},
           {IDB_OBM1_CONTROL_PIPE0r, IDB_OBM1_CONTROL_PIPE1r,
            IDB_OBM1_CONTROL_PIPE2r, IDB_OBM1_CONTROL_PIPE3r},
           {IDB_OBM2_CONTROL_PIPE0r, IDB_OBM2_CONTROL_PIPE1r,
            IDB_OBM2_CONTROL_PIPE2r, IDB_OBM2_CONTROL_PIPE3r},
           {IDB_OBM3_CONTROL_PIPE0r, IDB_OBM3_CONTROL_PIPE1r,
            IDB_OBM3_CONTROL_PIPE2r, IDB_OBM3_CONTROL_PIPE3r},
           {IDB_OBM4_CONTROL_PIPE0r, IDB_OBM4_CONTROL_PIPE1r,
            IDB_OBM4_CONTROL_PIPE2r, IDB_OBM4_CONTROL_PIPE3r},
           {IDB_OBM5_CONTROL_PIPE0r, IDB_OBM5_CONTROL_PIPE1r,
            IDB_OBM5_CONTROL_PIPE2r, IDB_OBM5_CONTROL_PIPE3r},
           {IDB_OBM6_CONTROL_PIPE0r, IDB_OBM6_CONTROL_PIPE1r,
            IDB_OBM6_CONTROL_PIPE2r, IDB_OBM6_CONTROL_PIPE3r},
           {IDB_OBM7_CONTROL_PIPE0r, IDB_OBM7_CONTROL_PIPE1r,
            IDB_OBM7_CONTROL_PIPE2r, IDB_OBM7_CONTROL_PIPE3r}};

    _SOC_TH_UNIT_VALIDATE(unit);

    /* Get lane, pipe & obm */
    phy_port = si->port_l2p_mapping[port];
    port_block_base = PORT_BLOCK_BASE_PORT(port);
    lane = phy_port - port_block_base;
    pipe = si->port_pipe[port];
    clport = si->port_serdes[port];
    /* obm number is reversed (mirrored) in odd pipe */
    obm = pipe & 1 ? 7 - (clport & 0x7) : clport & 0x7;

    /* BubbleMOP - At present only L3 & Full Latency Mode supported */
    SOC_IF_ERROR_RETURN(
        soc_reg32_get(unit, idb_obm_ctrl_reg[obm][pipe],
                      REG_PORT_ANY, 0, &rval));

    soc_reg_field_set(unit, idb_obm_ctrl_reg[obm][pipe], &rval,
                      port_field[lane], bubble_mop_disable);

    SOC_IF_ERROR_RETURN(
        soc_reg32_set(unit, idb_obm_ctrl_reg[obm][pipe], REG_PORT_ANY,
                      0, rval));

    return SOC_E_NONE;
}

/*************************************
 * Init Start Routine                *
 *    - Allocates ASF Ctrl structure *
 *      for the given unit           *
 *************************************/
int
soc_th_asf_init_start(int unit)
{
    soc_th_asf_mem_profile_e asf_mem_profile;

    _SOC_TH_UNIT_VALIDATE(unit);

    asf_mem_profile = soc_property_get(unit, spn_ASF_MEM_PROFILE,
                                       _SOC_TH_ASF_MEM_PROFILE_EXTREME);
    if (!(_soc_th_asf_ctrl[unit] =
              sal_alloc(sizeof(_soc_th_asf_ctrl_t), "TH ASF Ctrl Area"))) {
        return SOC_E_MEMORY;
    }

    if ((asf_mem_profile >= 0) && (asf_mem_profile < 3)) {
        _soc_th_asf_ctrl[unit]->asf_mem_profile = asf_mem_profile;
    } else {
        _soc_th_asf_ctrl[unit]->asf_mem_profile = 0;
    }

    return SOC_E_NONE;
}

/************************
 * Init Complete Marker *
 ************************/
int
soc_th_asf_init_done(int unit)
{
    _SOC_TH_UNIT_VALIDATE(unit);

    if (_soc_th_asf_ctrl[unit]) {
        _soc_th_asf_ctrl[unit]->init = 1;
        return SOC_E_NONE;
    } else {
        return SOC_E_INTERNAL;
    }
}

/******************************************************************************
 * Name: soc_th_port_asf_init                                                 *
 * Description:                                                               *
 *     Initialize ASF primitives for the specified port                       *
 * IN params:                                                                 *
 *     - Unit                                                                 *
 *     - Logical Port                                                         *
 *     - Port Speed                                                           *
 *     - ASF Mode                                                             *
 * Returns:                                                                   *
 *     - SOC_E_NONE on success                                                *
 *     - SOC_E_PARAM on invalid parameter                                     *
 *     - SOC_E_INIT on init/soc errors                                        *
 ******************************************************************************/
int
soc_th_port_asf_init(
    int                 unit,
    soc_port_t          port,
    int                 speed,
    soc_th_asf_mode_e   mode)
{
    int rv, speed_encoding;

    _SOC_TH_UNIT_VALIDATE(unit);
    if (SOC_E_PARAM == (speed_encoding = _soc_th_speed_to_ct_class_map(speed))) {
        /* port on a speed unsupported by CT - will resort to SAF */
        speed_encoding = 0;
    }

    /* init CT class */
    if (SOC_E_NONE !=
        (rv = _soc_th_port_asf_class_init(unit, port, speed_encoding))) {
        return SOC_E_INIT;
    }

    /* init Egress XMIT Start Count */
    if (SOC_E_NONE !=
        (rv = _SOC_TH_PORT_ASF_XMIT_START_COUNT_INIT(unit, port,
                                                     speed, mode, 0))) {
        return SOC_E_INIT;
    }

    return SOC_E_NONE;
}

/************************
 * Init Start Routine   *
 ************************/
int
soc_th_asf_deinit(int unit)
{
    _SOC_TH_UNIT_VALIDATE(unit);

    if (_soc_th_asf_ctrl[unit]) {
        sal_free(_soc_th_asf_ctrl[unit]);
    }

    return SOC_E_NONE;
}

/******************************************************************************
 * Name: _soc_th_port_asf_xmit_start_count_set                                *
 * Description:                                                               *
 *     Initialize XMIT START COUNT memory for all the source class for the    *
 *     specified port                                                         *
 * IN params:                                                                 *
 *     - Unit                                                                 *
 *     - Logical Port                                                         *
 *     - Port Speed                                                           *
 *     - ASF Mode                                                             *
 *     - Extra cells (if any, to be added to xmit start count)                *
 * Returns:                                                                   *
 *     - SOC_E_NONE on success                                                *
 *     - SOC_E_PARAM on invalid parameter                                     *
 ******************************************************************************/
int
soc_th_port_asf_xmit_start_count_set(
    int                  unit,
    soc_port_t           port,
    int                  port_speed,
    soc_th_asf_mode_e    mode,
    uint8                extra_cells)
{
    egr_xmit_start_count_entry_t entry;
    soc_info_t *si = &SOC_INFO(unit);
    uint8                        xmit_cnt = 0;
    int                          rv, src_class, dst_class = 0, port_idx;
    int                          pipe;
    soc_mem_t                    config_mem = INVALIDm;

    if (!((mode >= _SOC_TH_ASF_MODE_SAF) &&
          (mode <= _SOC_TH_ASF_MODE_CFG_UPDATE))) {
        return SOC_E_PARAM;
    }

    if (_SOC_TH_ASF_MODE_CFG_UPDATE != mode) {
        if (SOC_E_PARAM == (dst_class = _soc_th_speed_to_ct_class_map(port_speed))) {
            /* port on a speed unsupported by CT - will resort to SAF */
            dst_class = 0;
        }
    }

    sal_memset(&entry, 0, sizeof(egr_xmit_start_count_entry_t));

    pipe = si->port_pipe[port];
    config_mem = SOC_MEM_UNIQUE_ACC(unit, EGR_XMIT_START_COUNTm)[pipe];

    for (src_class = 0; src_class < 13; src_class++) {
        if (_SOC_TH_ASF_MODE_CFG_UPDATE == mode) {
            xmit_cnt = _SOC_TH_ASF_RETRV;
        } else {
            xmit_cnt = _SOC_TH_ASF_QUERY;
        }

        rv = _soc_th_port_asf_xmit_start_count_get(unit, port, src_class,
                                                   dst_class, mode, &xmit_cnt);
        if (SOC_FAILURE(rv)) {
            if (SOC_E_UNAVAIL == rv) {
                xmit_cnt = 18;
            } else {
                return rv;
            }
        }

        xmit_cnt += extra_cells;

        port_idx = ((port % 34) * 16) + src_class;

        soc_mem_field32_set(unit, config_mem, &entry,
                            THRESHOLDf, xmit_cnt);
        SOC_IF_ERROR_RETURN(soc_mem_write(unit, config_mem, MEM_BLOCK_ALL,
                                                     port_idx, &entry));
    }

    return SOC_E_NONE;
}

/******************************************************************************
 * Name: soc_th_port_asf_mode_get                                             *
 * Description:                                                               *
 *     Retrieve the ASF/SAF mode configured on the specified port             *
 * IN params:                                                                 *
 *     - Unit                                                                 *
 *     - Logical Port                                                         *
 *     - Port Speed                                                           *
 * OUT params:                                                                *
 *     - mode : OUT: contains the ASF/SAF mode confiugred on the specified    *
 *       port on success                                                      *
 * Returns:                                                                   *
 *     - SOC_E_NONE on success                                                *
 *     - SOC_E_PARAM on invalid parameter                                     *
 *     - SOC_E_INTERNAL on init/soc errors                                    *
 ******************************************************************************/
int
soc_th_port_asf_mode_get(
    int                            unit,
    soc_port_t                     port,
    int                            port_speed,
    OUT soc_th_asf_mode_e* const   mode)
{
    uint32 rval;
    int    speed_encoding;
    uint8  max_sp, min_sp, enable;

    _SOC_TH_UNIT_VALIDATE(unit);
    _SOC_TH_ASF_PORT_VALIDATE(unit, port);

    SOC_IF_ERROR_RETURN(READ_ASF_EPORT_CFGr(unit, port, &rval));
    enable = soc_reg_field_get(unit, ASF_EPORT_CFGr, rval, ENABLEf);
    if (!enable) {
        *mode = _SOC_TH_ASF_MODE_SAF;
        return SOC_E_NONE;
    }

    if (!_soc_th_asf_ctrl[unit] || !_soc_th_asf_ctrl[unit]->init) {
        return SOC_E_INTERNAL;
    }

    max_sp = soc_reg_field_get(unit, ASF_EPORT_CFGr, rval, MAX_SRC_PORT_SPEEDf);
    min_sp = soc_reg_field_get(unit, ASF_EPORT_CFGr, rval, MIN_SRC_PORT_SPEEDf);

    speed_encoding = _soc_th_speed_to_ct_class_map(port_speed);
    _SOC_TH_ASF_SPEED_CLASS_VALIDATE(speed_encoding, 1);

    if (max_sp && min_sp) {
        if (max_sp == min_sp) {
            if (SOC_PBMP_MEMBER(_soc_th_asf_ctrl[unit]->asf_sf, port)) {
                *mode = _SOC_TH_ASF_MODE_SLOW_TO_FAST;
            } else if (SOC_PBMP_MEMBER(_soc_th_asf_ctrl[unit]->asf_fs, port)) {
                *mode = _SOC_TH_ASF_MODE_FAST_TO_SLOW;
            } else {
                *mode = _SOC_TH_ASF_MODE_SAME_SPEED;
            }
        } else if (max_sp == speed_encoding) {
            *mode = _SOC_TH_ASF_MODE_SLOW_TO_FAST;
        }
        else if (min_sp == speed_encoding) {
            *mode = _SOC_TH_ASF_MODE_FAST_TO_SLOW;
        } else {
            *mode = _SOC_TH_ASF_MODE_UNSUPPORTED;
            return SOC_E_UNAVAIL;
        }
    } else {
        *mode = _SOC_TH_ASF_MODE_UNSUPPORTED;
        return SOC_E_UNAVAIL;
    }

    return SOC_E_NONE;
}

/******************************************************************************
 * Name: soc_th_port_asf_mode_set                                             *
 * Description:                                                               *
 *     Configure specified ASF/SAF modes on the specified port                *
 * IN params:                                                                 *
 *     - Unit                                                                 *
 *     - Logical Port                                                         *
 *     - Port Speed                                                           *
 *     - ASF Mode                                                             *
 * OUT params:                                                                *
 *     - None                                                                 *
 * Returns:                                                                   *
 *     - SOC_E_NONE on success                                                *
 *     - SOC_E_PARAM on invalid parameter                                     *
 *     - SOC_E_INTERNAL on init/soc errors                                    *
 ******************************************************************************/
int
soc_th_port_asf_mode_set(
    int                 unit,
    soc_port_t          port,
    int                 port_speed,
    soc_th_asf_mode_e   mode)
{
    int           ret, speed_encoding = 0, pause_enable = 0, txp = 0, rxp = 0;
    uint8         min_sp = 0, max_sp = 0;
    uint8         fifo_threshold = 0, fifo_depth = 0, mmu_prebuf = 0;
    uint8         oversub = 0, ca_thresh = 0;
    uint8         bubble_mop_disable = 0;
    uint32        rval;
    soc_info_t   *si = NULL;
    mac_driver_t *macd;
    int           latency = SOC_SWITCH_BYPASS_MODE_NONE;

    _SOC_TH_UNIT_VALIDATE(unit);
    speed_encoding = _soc_th_speed_to_ct_class_map(port_speed);
    _SOC_TH_ASF_SPEED_CLASS_VALIDATE(speed_encoding, 1);

    if (_SOC_TH_ASF_MODE_CFG_UPDATE == mode) {
        if (!_soc_th_asf_ctrl[unit]) {
            return SOC_E_NONE;
        }
    } else {
        _SOC_TH_ASF_PORT_VALIDATE(unit, port);
        if (!_soc_th_asf_ctrl[unit] || !_soc_th_asf_ctrl[unit]->init ||
            !(si = &SOC_INFO(unit))) {
            return SOC_E_INTERNAL;
        }
        if (!_soc_th_asf_ctrl[unit]->asf_mem_profile) {
            return SOC_E_UNAVAIL;
        }

        /* min. and max. src port speeds */
        min_sp = max_sp = _SOC_TH_ASF_QUERY;
        SOC_IF_ERROR_RETURN(
            _soc_th_port_asf_speed_limits_get(unit, port, port_speed, mode,
                                              &min_sp, &max_sp));
    }

    /* mode specific configurations */
    switch(mode) {
        case _SOC_TH_ASF_MODE_SAF:
            break;

        case _SOC_TH_ASF_MODE_SAME_SPEED:
            /* verify mode specific conformance */
            if (min_sp != max_sp) {
                return SOC_E_PARAM;
            }
            break;

        case _SOC_TH_ASF_MODE_SLOW_TO_FAST:
            /* verify mode specific conformance */
            if ( !((min_sp >=
                    _soc_th_asf_cfg_tbl[speed_encoding].min_sp) &&
                   (max_sp == speed_encoding)) ) {
                return SOC_E_PARAM;
            }
            break;

        case _SOC_TH_ASF_MODE_FAST_TO_SLOW:
            /* verify mode specific conformance */
            if ( !((max_sp <=
                    _soc_th_asf_cfg_tbl[speed_encoding].max_sp) &&
                   (min_sp == speed_encoding)) ) {
                return SOC_E_PARAM;
            }
            break;

        case _SOC_TH_ASF_MODE_CFG_UPDATE:
            SOC_IF_ERROR_RETURN(
                soc_th_port_asf_mode_get(unit, port, port_speed, &mode));
            SOC_IF_ERROR_RETURN(
                _SOC_TH_PORT_ASF_REINIT(unit, port, port_speed, mode));

            /* disable CT first */
            SOC_IF_ERROR_RETURN(READ_ASF_EPORT_CFGr(unit, port, &rval));
            soc_reg_field_set(unit, ASF_EPORT_CFGr, &rval,
                              FIFO_THRESHOLDf, 0);
            SOC_IF_ERROR_RETURN(WRITE_ASF_EPORT_CFGr(unit, port, rval));
            sal_usleep(1);
            soc_reg_field_set(unit, ASF_EPORT_CFGr, &rval, ENABLEf, 0);
            SOC_IF_ERROR_RETURN(WRITE_ASF_EPORT_CFGr(unit, port, rval));

            /* update new settings & re-enable */
            ret = soc_th_port_asf_mode_set(unit, port, port_speed, mode);
            return ret;
            break;

        default:
            return SOC_E_PARAM;
    }

    SOC_IF_ERROR_RETURN(soc_mac_probe(unit, port, &macd));

    /* ASF_IPORT_CFG */
    SOC_IF_ERROR_RETURN(READ_ASF_IPORT_CFGr(unit, port, &rval));
    soc_reg_field_set(unit, ASF_IPORT_CFGr, &rval, ASF_PORT_SPEEDf,
                      speed_encoding);
    SOC_IF_ERROR_RETURN(WRITE_ASF_IPORT_CFGr(unit, port, rval));

    if (_SOC_TH_ASF_MODE_SAF != mode) {
        /* Oversub */
        oversub =  (SOC_PBMP_MEMBER(si->oversub_pbm, port) ? 1 : 0);

        /* FIFO threshold */
        fifo_threshold = _SOC_TH_ASF_QUERY;
        SOC_IF_ERROR_RETURN(
            _soc_th_port_asf_fifo_threshold_get(unit, port, port_speed, mode,
                                                min_sp, &fifo_threshold));

        /* FIFO depth */
        fifo_depth = _SOC_TH_ASF_QUERY;
        SOC_IF_ERROR_RETURN(
            _soc_th_port_asf_fifo_depth_get(unit, port, port_speed, mode,
                                            min_sp, &fifo_depth));

        /* MMU Prebuffer */
        mmu_prebuf = _SOC_TH_ASF_QUERY;
        SOC_IF_ERROR_RETURN(
            _soc_th_port_asf_mmu_prebuf_get(unit, port, port_speed,
                                            min_sp, &mmu_prebuf));

        /* Disable BubbleMOP for low latency mode */
#ifndef BCM_TH_LATENCY_EXCLUDE
        SOC_IF_ERROR_RETURN(soc_th_latency_get(unit, &latency));
#endif
        if ((SOC_SWITCH_BYPASS_MODE_LOW == latency) &&
            ((1 == speed_encoding) || (2 == speed_encoding))) { /* special cases: 10G & HG[11] */
            bubble_mop_disable = 1;
        } else {
            bubble_mop_disable = 0;
        }

        pause_enable = 0;
        ca_thresh = 4;

        soc_th_port_asf_params_set(unit, port, port_speed, mode,
            bubble_mop_disable, ca_thresh, speed_encoding);

        /* PAUSE */
        SOC_IF_ERROR_RETURN(MAC_PAUSE_GET(macd, unit, port, &txp, &rxp));
        if (rxp) {
            SOC_PBMP_PORT_ADD(_soc_th_asf_ctrl[unit]->pause_restore, port);
        }
        rxp = pause_enable;
        SOC_IF_ERROR_RETURN(MAC_PAUSE_SET(macd, unit, port, txp, rxp));

        /* drain the port CT FIFO */
        SOC_IF_ERROR_RETURN(READ_ASF_EPORT_CFGr(unit, port, &rval));
        soc_reg_field_set(unit, ASF_EPORT_CFGr, &rval, ENABLEf, 1);
        soc_reg_field_set(unit, ASF_EPORT_CFGr, &rval, FIFO_THRESHOLDf, 0);
        SOC_IF_ERROR_RETURN(WRITE_ASF_EPORT_CFGr(unit, port, rval));

        sal_usleep(1);

        /* enable cut-through */
        soc_reg_field_set(unit, ASF_EPORT_CFGr, &rval, OVERSUBf, oversub);
        soc_reg_field_set(unit, ASF_EPORT_CFGr, &rval,
                          MAX_SRC_PORT_SPEEDf, max_sp);
        soc_reg_field_set(unit, ASF_EPORT_CFGr, &rval,
                          MIN_SRC_PORT_SPEEDf, min_sp);
        soc_reg_field_set(unit, ASF_EPORT_CFGr, &rval,
                          MIN_SRC_PORT_XMIT_CNTf, mmu_prebuf);
        soc_reg_field_set(unit, ASF_EPORT_CFGr, &rval,
                          FIFO_THRESHOLDf, fifo_threshold);
        soc_reg_field_set(unit, ASF_EPORT_CFGr, &rval, FIFO_DEPTHf,
                          fifo_depth);
        SOC_IF_ERROR_RETURN(WRITE_ASF_EPORT_CFGr(unit, port, rval));

        /* bookkeeping */
        if (SOC_PBMP_MEMBER(_soc_th_asf_ctrl[unit]->asf_ss, port)) {
            SOC_PBMP_PORT_REMOVE(_soc_th_asf_ctrl[unit]->asf_ss, port);
        } else if (SOC_PBMP_MEMBER(_soc_th_asf_ctrl[unit]->asf_sf, port)) {
            SOC_PBMP_PORT_REMOVE(_soc_th_asf_ctrl[unit]->asf_sf, port);
        } else if (SOC_PBMP_MEMBER(_soc_th_asf_ctrl[unit]->asf_fs, port)) {
            SOC_PBMP_PORT_REMOVE(_soc_th_asf_ctrl[unit]->asf_fs, port);
        }
        SOC_PBMP_PORT_ADD(_soc_th_asf_ctrl[unit]->asf_ports, port);
        if (_SOC_TH_ASF_MODE_SAME_SPEED == mode) {
            SOC_PBMP_PORT_ADD(_soc_th_asf_ctrl[unit]->asf_ss, port);
        } else if (_SOC_TH_ASF_MODE_SLOW_TO_FAST== mode) {
            SOC_PBMP_PORT_ADD(_soc_th_asf_ctrl[unit]->asf_sf, port);
        } else if (_SOC_TH_ASF_MODE_FAST_TO_SLOW== mode) {
            SOC_PBMP_PORT_ADD(_soc_th_asf_ctrl[unit]->asf_fs, port);
        }
    } else {   /* Store and Forward */
        fifo_depth = 0x40;
        fifo_threshold = 0x20;
        mmu_prebuf = 0;
        min_sp = 0;
        max_sp = 0;
        oversub = 0;
        pause_enable = 1;
        ca_thresh = 4;
        bubble_mop_disable = 0;

        soc_th_port_asf_params_set(unit, port, port_speed, mode,
            bubble_mop_disable, ca_thresh, speed_encoding);

        /* ASF_EPORT_CFG */
        SOC_IF_ERROR_RETURN(READ_ASF_EPORT_CFGr(unit, port, &rval));
        soc_reg_field_set(unit, ASF_EPORT_CFGr, &rval, FIFO_THRESHOLDf, 0);
        SOC_IF_ERROR_RETURN(WRITE_ASF_EPORT_CFGr(unit, port, rval));

        sal_usleep(1);

        soc_reg_field_set(unit, ASF_EPORT_CFGr, &rval, ENABLEf, 0);
        SOC_IF_ERROR_RETURN(WRITE_ASF_EPORT_CFGr(unit, port, rval));

        /* update SAF configs */
        SOC_IF_ERROR_RETURN(READ_ASF_EPORT_CFGr(unit, port, &rval));
        soc_reg_field_set(unit, ASF_EPORT_CFGr, &rval, OVERSUBf, oversub);
        soc_reg_field_set(unit, ASF_EPORT_CFGr, &rval,
                          MAX_SRC_PORT_SPEEDf, max_sp);
        soc_reg_field_set(unit, ASF_EPORT_CFGr, &rval,
                          MIN_SRC_PORT_SPEEDf, min_sp);
        soc_reg_field_set(unit, ASF_EPORT_CFGr, &rval,
                          MIN_SRC_PORT_XMIT_CNTf, mmu_prebuf);
        soc_reg_field_set(unit, ASF_EPORT_CFGr, &rval,
                          FIFO_THRESHOLDf, fifo_threshold);
        soc_reg_field_set(unit, ASF_EPORT_CFGr, &rval, FIFO_DEPTHf,
                          fifo_depth);
        SOC_IF_ERROR_RETURN(WRITE_ASF_EPORT_CFGr(unit, port, rval));

        /* restore pause */
        if (SOC_PBMP_MEMBER(_soc_th_asf_ctrl[unit]->pause_restore, port)) {
            SOC_IF_ERROR_RETURN(MAC_PAUSE_GET(macd, unit, port, &txp, &rxp));
            rxp = 1;
            SOC_IF_ERROR_RETURN(MAC_PAUSE_SET(macd, unit, port, txp, rxp));
            SOC_PBMP_PORT_REMOVE(_soc_th_asf_ctrl[unit]->pause_restore, port);
        }

        /* bookkeeping */
        SOC_PBMP_PORT_REMOVE(_soc_th_asf_ctrl[unit]->asf_ports, port);
        if (SOC_PBMP_MEMBER(_soc_th_asf_ctrl[unit]->asf_ss, port)) {
            SOC_PBMP_PORT_REMOVE(_soc_th_asf_ctrl[unit]->asf_ss, port);
        } else if (SOC_PBMP_MEMBER(_soc_th_asf_ctrl[unit]->asf_sf, port)) {
            SOC_PBMP_PORT_REMOVE(_soc_th_asf_ctrl[unit]->asf_sf, port);
        } else if (SOC_PBMP_MEMBER(_soc_th_asf_ctrl[unit]->asf_fs, port)) {
            SOC_PBMP_PORT_REMOVE(_soc_th_asf_ctrl[unit]->asf_fs, port);
        }
    }

    return SOC_E_NONE;
}

/******************************************************************************
 * Name: soc_th_port_asf_params_set                                           *
 * Description:                                                               *
 *     Configure ASF/SAF mode params on the specified port                    *
 * IN params:                                                                 *
 *     - Unit                                                                 *
 *     - Logical Port                                                         *
 *     - Port Speed                                                           *
 *     - ASF Mode                                                             *
 *     - Bubble MOP disable flag                                              *
 *     - CA Threshold                                                         *
 *     - Speed Encoding                                                       *
 * OUT params:                                                                *
 *     - None                                                                 *
 * Returns:                                                                   *
 *     - SOC_E_NONE on success                                                *
 *     - SOC_E_PARAM on invalid parameter                                     *
 *     - SOC_E_INTERNAL on init/soc errors                                    *
 ******************************************************************************/
int
soc_th_port_asf_params_set(
    int                 unit,
    soc_port_t          port,
    int                 port_speed,
    soc_th_asf_mode_e   mode,
    uint8   bubble_mop_disable,
    uint8   ca_thresh,
    int     speed_encoding)
{
    uint8         mmu_ep_credits = 0, egr_mmu_cell_credits = 0;
    uint32        rval;
    soc_info_t   *si = NULL;

    if (!(si = &SOC_INFO(unit))) {
        return SOC_E_INTERNAL;
    }

    /* EDB Prebuffer */
    SOC_IF_ERROR_RETURN(
        soc_th_port_asf_xmit_start_count_set(unit, port, port_speed,
                                                 mode, 0));
    /* OBM Cell Assembly FIFO threshold */
    SOC_IF_ERROR_RETURN(
        _soc_th_asf_obm_ca_fifo_thresh_set(unit, port, ca_thresh));

    /* BubbleMOP - At present only L3 & Full Latency Mode supported */
    SOC_IF_ERROR_RETURN(
        _soc_th_asf_obm_bubble_mop_set(unit, port, bubble_mop_disable));

    /* MMU EP Credit Threshold */
    if (CCLK_FREQ_850MHZ == si->frequency) {
        mmu_ep_credits =
        _soc_th_asf_cfg_tbl[speed_encoding].mmu_ep_credit.pfc_op_cells;
    } else {
        mmu_ep_credits =
        _soc_th_asf_cfg_tbl[speed_encoding].mmu_ep_credit.base_cells;
    }
    /* ASF_CREDIT_THRESH_HI */
    SOC_IF_ERROR_RETURN(READ_ASF_CREDIT_THRESH_HIr(unit, port, &rval));
    soc_reg_field_set(unit, ASF_CREDIT_THRESH_HIr, &rval, THRESHf,
                      mmu_ep_credits);
    SOC_IF_ERROR_RETURN(WRITE_ASF_CREDIT_THRESH_HIr(unit, port, rval));

    /* EP credits outstanding */
    if (CCLK_FREQ_850MHZ == si->frequency) {
        egr_mmu_cell_credits =
        _soc_th_asf_cfg_tbl[speed_encoding].egr_mmu_credit.pfc_op_cells;
    } else {
        egr_mmu_cell_credits =
        _soc_th_asf_cfg_tbl[speed_encoding].egr_mmu_credit.base_cells;
    }
    /* EGR_MMU_CELL_CREDIT */
    SOC_IF_ERROR_RETURN(READ_EGR_MMU_CELL_CREDITr(unit, port, &rval));
    soc_reg_field_set(unit, EGR_MMU_CELL_CREDITr, &rval, CREDITf,
                      egr_mmu_cell_credits);
    SOC_IF_ERROR_RETURN(WRITE_EGR_MMU_CELL_CREDITr(unit, port, rval));

    return SOC_E_NONE;
}

/******************************************************************************
 * Name: soc_th_port_asf_mmu_cell_credit_to_port_speed                            *
 * Description:                                                               *
 *     Retrieve port speed based on mmu_cell_credit configured.               *
 *     Helper function for MAC driver speed get.                              *
 * IN params:                                                                 *
 *     - Unit                                                                 *
 *     - Logical Port                                                         *
 *     - Port Speed                                                           *
 *     - MMU Cell credit                                                      *
 * OUT params:                                                                *
 *     - Port speed                                                           *
 * Returns:                                                                   *
 *     - SOC_E_XXX on success                                                 *
 ******************************************************************************/
int
soc_th_port_asf_mmu_cell_credit_to_speed(
    int        unit,
    soc_port_t port,
    uint8      mmu_cell_credit,
    OUT int    *port_speed)
{
    int i, freq;
    uint8 cell_credit;
    soc_info_t   *si = NULL;

    _SOC_TH_UNIT_VALIDATE(unit);

    if (NULL == port_speed) {
        return SOC_E_PARAM;
    }

    if (!(si = &SOC_INFO(unit))) {
        return SOC_E_INTERNAL;
    }

    freq = si->frequency;
    for (i = 0; i < (COUNTOF(_soc_th_asf_cfg_tbl) - 1); i++) {
        cell_credit = (CCLK_FREQ_850MHZ == freq) ?
                      _soc_th_asf_cfg_tbl[i].egr_mmu_credit.pfc_op_cells :
                      _soc_th_asf_cfg_tbl[i].egr_mmu_credit.base_cells;
        if (mmu_cell_credit == cell_credit) {
            if (IS_HG_PORT(unit, port)) {
                *port_speed = _soc_th_asf_cfg_tbl[i + 1].speed;
            } else {
                *port_speed = _soc_th_asf_cfg_tbl[i].speed;
            }
            return SOC_E_NONE;
        }
    }

    return SOC_E_NOT_FOUND;
}

/************************
 * ASF Warmboot Support *
 ************************/
#ifdef BCM_WARM_BOOT_SUPPORT
int
soc_th_asf_wb_memsz_get(
    int unit,
    OUT uint32* const mem_sz)
{
    _SOC_TH_UNIT_VALIDATE(unit);
    if (!mem_sz) {
        return SOC_E_PARAM;
    }
    *mem_sz = 0;

    if (!SOC_WARM_BOOT(unit)) {
        if (!_soc_th_asf_ctrl[unit] || !_soc_th_asf_ctrl[unit]->asf_mem_profile) {
            return SOC_E_UNAVAIL;
        }
        if (!_soc_th_asf_ctrl[unit]->init) {
            return SOC_E_INTERNAL;
        }
    }

    *mem_sz = sizeof(_soc_th_asf_wb_t);

    return SOC_E_NONE;
}

int
soc_th_asf_wb_sync(
    int    unit,
    IN_OUT uint8* const wb_data)
{
    _soc_th_asf_wb_t *wbd;

    _SOC_TH_UNIT_VALIDATE(unit);
    if (!_soc_th_asf_ctrl[unit] || !_soc_th_asf_ctrl[unit]->asf_mem_profile) {
        return SOC_E_UNAVAIL;
    }
    if (!_soc_th_asf_ctrl[unit]->init) {
        return SOC_E_INTERNAL;
    }
    if (!wb_data) {
        return SOC_E_PARAM;
    }

    wbd = (_soc_th_asf_wb_t *) wb_data;
    wbd->unit = unit;
    sal_memcpy(&wbd->asf_ctrl, _soc_th_asf_ctrl[unit],
               sizeof(_soc_th_asf_ctrl_t));

    return SOC_E_NONE;
}

int
soc_th_asf_wb_recover(
    int unit,
    uint8* const wb_data)
{
    _soc_th_asf_wb_t *wbd;

    _SOC_TH_UNIT_VALIDATE(unit);
    if (!wb_data) {
        return SOC_E_PARAM;
    }

    wbd = (_soc_th_asf_wb_t *) wb_data;
    if (wbd->unit == unit) {
        if (!(_soc_th_asf_ctrl[unit] =
                   sal_alloc(sizeof(_soc_th_asf_ctrl_t), "TH ASF Ctrl Area"))) {
            return SOC_E_MEMORY;
        }

        sal_memcpy(_soc_th_asf_ctrl[unit], &wbd->asf_ctrl,
                   sizeof(_soc_th_asf_ctrl_t));
    }

    return SOC_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

/**********************
 * ASF Debug Support  *
 **********************/
int
soc_th_asf_pbmp_get(int unit)
{
    char pfmt[SOC_PBMP_FMT_LEN];

    _SOC_TH_UNIT_VALIDATE(unit);
    if (!_soc_th_asf_ctrl[unit]->init) {
        return SOC_E_INTERNAL;
    }

    LOG_CLI(("ASF PBMP: %s\n",
             SOC_PBMP_FMT(_soc_th_asf_ctrl[unit]->asf_ports, pfmt)));

    return SOC_E_NONE;
}

int
soc_th_port_asf_show(
    int        unit,
    soc_port_t port,
    int        port_speed)
{
    int ret;
    char mode_str[4][15] = {"SAF", "Same speed  ", "Slow to Fast",
                             "Fast to Slow"};
    char speed_str[13][10] = {"SAF", "10G", "HG[11]", "20G", "HG[21]", "25G",
                               "HG[27]", "40G", "HG[42]", "50G", "HG[53]",
                               "100G", "HG[106]"};
    soc_th_asf_mode_e mode = _SOC_TH_ASF_RETRV;
    uint8 min_speed, max_speed;

    min_speed = max_speed = _SOC_TH_ASF_RETRV;
    ret = soc_th_port_asf_mode_get(unit, port, port_speed, &mode);
    if (SOC_E_UNAVAIL == ret) {
        mode = _SOC_TH_ASF_MODE_SAF;
    } else if (SOC_E_NONE != ret) {
        return ret;
    }

    LOG_CLI(("%-5s     %-11s      ", SOC_PORT_NAME(unit, port), mode_str[mode]));

    if (_SOC_TH_ASF_MODE_SAF == mode) {
        LOG_CLI(("    .. NA ..\n"));
    } else if (_SOC_TH_ASF_MODE_FAST_TO_SLOW == mode) {
        ret = _soc_th_port_asf_speed_limits_get(unit, port, port_speed, mode,
                                                &min_speed, &max_speed);
        LOG_CLI(("%s / %s\n", speed_str[max_speed], speed_str[min_speed]));
    } else {
        ret = _soc_th_port_asf_speed_limits_get(unit, port, port_speed, mode,
                                                &min_speed, &max_speed);
        LOG_CLI(("%s / %s\n", speed_str[min_speed], speed_str[max_speed]));
    }

    return SOC_E_NONE;
}

int soc_th_asf_config_dump(int unit)
{
    char asf_profile_str[3][25] = {"No cut-thru support",
                                   "Similar speed cut-thru",
                                   "Extreme speed cut-thru"};

    _SOC_TH_UNIT_VALIDATE(unit);
    if (!_soc_th_asf_ctrl[unit] || !_soc_th_asf_ctrl[unit]->init) {
        return SOC_E_INTERNAL;
    }

    LOG_CLI(("ASF Profile: %s\n\n",
             asf_profile_str[_soc_th_asf_ctrl[unit]->asf_mem_profile]));

    return SOC_E_NONE;
}

int
soc_th_port_asf_config_dump(
    int        unit,
    soc_port_t port,
    int        port_speed)
{
    int    txp, rxp, ct_class;
    uint8  src_class, dst_class;
    uint8  min_sp, max_sp;
    uint8  fifo_threshold, fifo_depth, mmu_prebuf, xmit_start_cnt[13];
    uint32 rval, mmu_ep_credits, egr_mmu_cell_credits;
    mac_driver_t *macd;
    soc_th_asf_mode_e mode = _SOC_TH_ASF_RETRV;

    _SOC_TH_UNIT_VALIDATE(unit);

    min_sp = max_sp = fifo_threshold = _SOC_TH_ASF_RETRV;
    fifo_depth = mmu_prebuf = _SOC_TH_ASF_RETRV;

    if (!_soc_th_asf_ctrl[unit] || !_soc_th_asf_ctrl[unit]->init) {
        return SOC_E_INTERNAL;
    }

    SOC_IF_ERROR_RETURN(
        soc_th_port_asf_mode_get(unit, port, port_speed, &mode));

    SOC_IF_ERROR_RETURN(
        _soc_th_port_asf_class_get(unit, port, &ct_class));

    SOC_IF_ERROR_RETURN(
        _soc_th_port_asf_speed_limits_get(unit, port, port_speed, mode,
                                          &min_sp, &max_sp));

    SOC_IF_ERROR_RETURN(
        _soc_th_port_asf_fifo_threshold_get(unit, port, port_speed, mode,
                                            min_sp, &fifo_threshold));

    SOC_IF_ERROR_RETURN(
        _soc_th_port_asf_fifo_depth_get(unit, port, port_speed, mode,
                                        min_sp, &fifo_depth));

    SOC_IF_ERROR_RETURN(
        _soc_th_port_asf_mmu_prebuf_get(unit, port, port_speed,
                                        min_sp, &mmu_prebuf));

    dst_class = _soc_th_speed_to_ct_class_map(port_speed);
    for (src_class = 0; src_class <= 12; src_class++) {
        xmit_start_cnt[src_class] = _SOC_TH_ASF_RETRV;
        SOC_IF_ERROR_RETURN(
            _soc_th_port_asf_xmit_start_count_get(unit, port, src_class,
                                                  dst_class, mode,
                                                  &xmit_start_cnt[src_class]));
    }

    LOG_CLI(("%-5s %-3d %-3d %-6d %-6d %-3d %-3d   ", SOC_PORT_NAME(unit, port),
             mode, ct_class, _SOC_TH_CT_CLASS_TO_SPEED_MAP(min_sp),
            _SOC_TH_CT_CLASS_TO_SPEED_MAP(max_sp), fifo_threshold,
            fifo_depth));
    for (src_class = 0; src_class <= 12; src_class++) {
        LOG_CLI(("%-3d ", xmit_start_cnt[src_class]));
    }

    SOC_IF_ERROR_RETURN(READ_ASF_CREDIT_THRESH_HIr(unit, port, &rval));
    mmu_ep_credits = soc_reg_field_get(unit, ASF_CREDIT_THRESH_HIr,
                                       rval, THRESHf);

    SOC_IF_ERROR_RETURN(READ_EGR_MMU_CELL_CREDITr(unit, port, &rval));
    egr_mmu_cell_credits = soc_reg_field_get(unit, EGR_MMU_CELL_CREDITr,
                                             rval, CREDITf);
    SOC_IF_ERROR_RETURN(soc_mac_probe(unit, port, &macd));
    SOC_IF_ERROR_RETURN(MAC_PAUSE_GET(macd, unit, port, &txp, &rxp));

    LOG_CLI((" %-3d %-3d %-3d %-3d\n", mmu_prebuf, mmu_ep_credits,
             egr_mmu_cell_credits, rxp));

    return SOC_E_NONE;
}

#endif /* BCM_TOMAHAWK_SUPPORT */

