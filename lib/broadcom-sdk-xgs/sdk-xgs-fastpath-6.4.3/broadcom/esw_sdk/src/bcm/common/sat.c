/*
 * $Id: sat.c,v 1.5 Broadcom SDK $
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
 * BCM layer SAT APIs
 */

#include <shared/bsl.h>
#include <bcm/error.h>
#include <bcm/pkt.h>
#include <bcm_int/control.h>
#include <bcm_int/common/sat.h>
#include <bcm_int/common/debug.h>
#include <bcm/sat.h>
#include <sal/core/alloc.h>

#include <soc/drv.h>
#include <soc/shared/mbcm_sat.h>

#define _BCM_SAT_NUM_CLKS_PER_SEC_JERICHO    720000000
#define _BCM_SAT_NUM_CLKS_PER_SEC_SABER2     130000000
#define _BCM_SAT_CMIC_TOD_MODE               0x2
#define _BCM_SAT_RATE_NUM_CLKS_CYCLE         720000
#define _BCM_SAT_PAYLOAD_OFFSET_MIN          0
#define _BCM_SAT_PAYLOAD_OFFSET_MAX          0xFFFF
#define _BCM_SAT_SEQ_NUM_OFFSET_MIN          0
#define _BCM_SAT_SEQ_NUM_OFFSET_MAX          0xFFFF
#define _BCM_SAT_TIME_STAMP_OFFSET_MIN       0
#define _BCM_SAT_TIME_STAMP_OFFSET_MAX       0xFFFF
#define _BCM_SAT_MEF_PAYLOAD_OFFSET          184
#define _BCM_SAT_MEF_SEQ_NUM_OFFSET          96
#define _BCM_SAT_MEF_TIME_STAMP_OFFSET       0
#define _BCM_SAT_Y1731_PAYLOAD_OFFSET        96
#define _BCM_SAT_Y1731_SEQ_NUM_OFFSET        0
#define _BCM_SAT_Y1731_TIME_STAMP_OFFSET     32

#define _BCM_SAT_GTF_ID_MIN                  0
#define _BCM_SAT_GTF_ID_MAX                  7

#define _BCM_SAT_GTF_OBJ_COMMON              -1
#define _BCM_SAT_GTF_OBJ_CIR                 0
#define _BCM_SAT_GTF_OBJ_EIR                 1

#define _BCM_SAT_GTF_PKT_HDR_LEN_MIN         1
#define _BCM_SAT_GTF_PKT_HDR_LEN_MAX         128
#define _BCM_SAT_GTF_PKT_LEN_PATN_INX_MIN      0
#define _BCM_SAT_GTF_PKT_LEN_PATN_INX_MAX      7
#define _BCM_SAT_GTF_SEQ_PERIOD_MIN          1
#define _BCM_SAT_GTF_SEQ_PERIOD_MAX          3
#define _BCM_SAT_GTF_STAMP_INC_STEP_MIN      1
#define _BCM_SAT_GTF_STAMP_INC_STEP_MAX      0x7f
#define _BCM_SAT_GTF_STAMP_INC_PERIOD_MIN    1
#define _BCM_SAT_GTF_STAMP_INC_PERIOD_MAX    0x3
#define _BCM_SAT_GTF_STAMP_2_bit_VALUE_MIN   0
#define _BCM_SAT_GTF_STAMP_2_bit_VALUE_MAX   0x3

#define _BCM_SAT_GTF_BW_RATE_MAX                             23000000                /* 23Gbits */
#define _BCM_SAT_GTF_BW_RATE_MIN                             1
#define _BCM_SAT_GTF_BW_BURST_MAX                            (0x7fff8000/125)        /* 0xffff*(1<<15)*8/1000 */
#define _BCM_SAT_GTF_BW_BURST_MIN                            1
#define _BCM_SAT_GTF_RATE_PATN_HIGH_TH_MAX                   23000000                /* 23Gbits */
#define _BCM_SAT_GTF_RATE_PATN_HIGH_TH_MIN                   1
#define _BCM_SAT_GTF_RATE_PATN_LOW_TH_MAX                    23000000                /* 23Gbits */
#define _BCM_SAT_GTF_RATE_PATN_LOW_TH_MIN                    1
#define _BCM_SAT_GTF_RATE_PATN_STOP_ITER_MAX                 0x3ff
#define _BCM_SAT_GTF_RATE_PATN_STOP_ITER_MIN                 0
#define _BCM_SAT_GTF_RATE_PATN_STOP_BURST_MAX                0xffffffff
#define _BCM_SAT_GTF_RATE_PATN_STOP_BURST_MIN                0
#define _BCM_SAT_GTF_RATE_PATN_STOP_INTERVAL_MAX             0xffffffff
#define _BCM_SAT_GTF_RATE_PATN_STOP_INTERVAL_MIN             0
#define _BCM_SAT_GTF_RATE_PATN_BURST_PACKET_WEIGHT_MAX       0xffffffff
#define _BCM_SAT_GTF_RATE_PATN_BURST_PACKET_WEIGHT_MIN       0
#define _BCM_SAT_GTF_RATE_PATN_INTERVAL_PACKET_WEIGHT_MAX    0xffffffff
#define _BCM_SAT_GTF_RATE_PATN_INTERVAL_PACKET_WEIGHT_MIN    0

#define _BCM_SAT_CTF_TRAP_ID_MAX_NUM       3 
#define _BCM_SAT_CTF_ID_MIN                0
#define _BCM_SAT_CTF_ID_MAX                31
#define _BCM_SAT_CTF_OAM_ID_MIN            0
#define _BCM_SAT_CTF_OAM_ID_MAX            15
#define _BCM_SAT_CTF_TRAP_ID_MIN           0
#define _BCM_SAT_CTF_TRAP_ID_MAX           3
#define _BCM_SAT_CTF_COLOR_MIN             0
#define _BCM_SAT_CTF_COLOR_MAX             3
#define _BCM_SAT_CTF_COS_MIN               0
#define _BCM_SAT_CTF_COS_MAX               7
#define _BCM_SAT_CTF_TRAP_ID_VAL_MIN       0
#define _BCM_SAT_CTF_TRAP_ID_VAL_MAX       255
#define _BCM_SAT_CTF_TRAP_ID_UNSET_VAL     0x0
#define _BCM_SAT_CTF_BINS_LIMIT_CNT_MIN    1
#define _BCM_SAT_CTF_BINS_LIMIT_CNT_MAX    9
#define _BCM_SAT_CTF_BINS_LIMIT_SELECT_MAX 8
#define _BCM_SAT_CTF_SWITCH_STATE_NUM_OF_SLOTS_MIN   0
#define _BCM_SAT_CTF_SWITCH_STATE_NUM_OF_SLOTS_MAX   31
#define _BCM_SAT_CTF_SWITCH_STATE_THRES_PER_SLOT_MIN 0
#define _BCM_SAT_CTF_SWITCH_STATE_THRES_PER_SLOT_MAX 0x1FFFFFF
#define _BCM_SAT_CTF_REPORT_SAMPLING_PER_MAX         100
#define _BCM_SAT_CTF_TRAP_DATA_MIN                   0x0
#define _BCM_SAT_CTF_TRAP_DATA_MAX                   0x3FFF
#define _BCM_SAT_CTF_TRAP_DATA_MASK_MIN              0x0
#define _BCM_SAT_CTF_TRAP_DATA_MASK_MAX              0x3FFF
#define _BCM_SAT_CTF_TRAP_DATA_ENTRY_NUM             16
#define _BCM_SAT_CTF_TRAP_DATA_SSID_MAX              0xF


typedef struct _bcm_sat_ctf_trap_data_s {
    uint16 trap_data;
    uint16 trap_mask;
    uint8  ssid;
}_bcm_sat_ctf_trap_data_t;

typedef struct _bcm_sat_data_s {
    uint32        ctf_id_bitmap;
    uint32        gtf_id_bitmap;
    sal_mutex_t   sat_mutex; /* mutex for LLM message */
    uint32        ctf_trap_id[_BCM_SAT_CTF_TRAP_ID_MAX_NUM];
    uint32        ctf_trap_bitmap;
    uint32        ctf_ssid_bitmap;
    _bcm_sat_ctf_trap_data_t trap_data[_BCM_SAT_CTF_TRAP_DATA_ENTRY_NUM];
} _bcm_sat_data_t;


_bcm_sat_data_t sat_data[BCM_UNITS_MAX];

#define SAT_NULL_CHECK(p) \
    if (p == NULL) { \
        return BCM_E_PARAM; \
    }

#define SAT_VALUE_CHECK(val, min, max) \
    if ((val) < (min) || (val) >= (max)) { \
        return BCM_E_PARAM; \
    }

#define SAT_VALUE_MAX_CHECK(val, max) \
    if ((val) >= (max)) { \
        return BCM_E_PARAM; \
    }


#define SAT_BITMAP_EXIST(bitmap, index) \
        ((bitmap) & (1U << (index)))
#define SAT_BITMAP_SET(bitmap, index)   \
        ((bitmap) |= (1U << (index)))
#define SAT_BITMAP_CLR(bitmap, index)     \
        ((bitmap) &= (~(1U << (index))))

/* GTF ID */
#define SAT_GTF_ID_EXIST(psat_data, gtf_id) \
        SAT_BITMAP_EXIST((psat_data)->gtf_id_bitmap, gtf_id)
#define SAT_GTF_ID_SET(psat_data, gtf_id)   \
        SAT_BITMAP_SET((psat_data)->gtf_id_bitmap, gtf_id)
#define SAT_GTF_ID_CLR(psat_data, gtf_id)     \
        SAT_BITMAP_CLR((psat_data)->gtf_id_bitmap, gtf_id)
#define SAT_GTF_ID_CLR_ALL(psat_data)    \
    ((psat_data)->gtf_id_bitmap = 0)
#define SAT_GTF_ID_NOT_EXIST_EXIT(psat_data, gtf_id) \
    if (!SAT_GTF_ID_EXIST(psat_data, gtf_id)) {\
        return BCM_E_NOT_FOUND; \
    }

/* CTF ID */
#define SAT_CTF_ID_EXIST(psat_data, ctf_id) \
        SAT_BITMAP_EXIST((psat_data)->ctf_id_bitmap, ctf_id)
#define SAT_CTF_ID_SET(psat_data, ctf_id)   \
        SAT_BITMAP_SET((psat_data)->ctf_id_bitmap, ctf_id)
#define SAT_CTF_ID_CLR(psat_data, ctf_id)     \
        SAT_BITMAP_CLR((psat_data)->ctf_id_bitmap, ctf_id)
#define SAT_CTF_ID_CLR_ALL(psat_data)    \
    ((psat_data)->ctf_id_bitmap = 0)
#define SAT_CTF_ID_NOT_EXIST_EXIT(psat_data, ctf_id) \
    if (!SAT_CTF_ID_EXIST(psat_data, ctf_id)) {\
        return BCM_E_NOT_FOUND; \
    }

/* CTF TRAP ID */
#define SAT_CTF_TRAP_ID_EXIST(psat_data, trap_idx) \
        SAT_BITMAP_EXIST((psat_data)->ctf_trap_bitmap, trap_idx)
#define SAT_CTF_TRAP_ID_SET(psat_data, trap_idx, trap_id)   \
        (psat_data)->ctf_trap_id[trap_idx] = trap_id; \
        SAT_BITMAP_SET((psat_data)->ctf_trap_bitmap, trap_idx)
#define SAT_CTF_TRAP_ID_CLR(psat_data, trap_idx)     \
        (psat_data)->ctf_trap_id[trap_idx] = _BCM_SAT_CTF_TRAP_ID_UNSET_VAL; \
        SAT_BITMAP_CLR((psat_data)->ctf_trap_bitmap, trap_idx)
#define SAT_CTF_TRAP_ID_CLR_ALL(psat_data)    \
    ((psat_data)->ctf_trap_bitmap = 0); \
    for (trap_idx = 0; trap_idx < _BCM_SAT_CTF_TRAP_ID_MAX_NUM; trap_idx++) { \
        (psat_data)->ctf_trap_id[trap_idx] = _BCM_SAT_CTF_TRAP_ID_UNSET_VAL; \
    }

#define _BCM_RX_SYSTEM_LOCK(unit)                                                \
          sal_mutex_take(sat_data[unit].sat_mutex, sal_mutex_FOREVER)
#define _BCM_RX_SYSTEM_UNLOCK(unit) sal_mutex_give(sat_data[unit].sat_mutex)


/* Functions */

#ifdef BCM_SAT_SUPPORT
int bcm_common_sat_init(int unit)
{
    int rv = BCM_E_UNAVAIL;

    _bcm_sat_data_t *psat_data;
    soc_sat_init_t soc_sat_init;
    uint32 freq_hz = 0;

    psat_data = &(sat_data[unit]);
    sal_memset(psat_data, 0, sizeof(_bcm_sat_data_t));

    psat_data->sat_mutex = sal_mutex_create("SAT LOCK");
    if(psat_data->sat_mutex == NULL) {
        BCM_FREE(psat_data->sat_mutex);
        return BCM_E_MEMORY;
    }

    rv = mbcm_sat_init(unit);
    if (rv != SOC_E_NONE)
    {
        LOG_BSL_ERROR(BSL_LS_SOC_INIT,
                  (BSL_META_U(unit,
                              "soc_dpp_init error in mbcm_sat_init\n")));
        return rv;
    }

    /* Time tickets */
    freq_hz = SOC_INFO(unit).frequency * 1000000;
    
    soc_sat_init.cmic_tod_mode = _BCM_SAT_CMIC_TOD_MODE;
    soc_sat_init.num_clks_sec = freq_hz;
    soc_sat_init.rate_num_clks_cycle = (freq_hz/1000)*8;    /* 1kbits per sec(125 cycles per sec) */

    _BCM_RX_SYSTEM_LOCK(unit);
	rv = MBCM_SAT_DRIVER_CALL(unit, mbcm_sat_general_cfg_init, (unit, &soc_sat_init));
	_BCM_RX_SYSTEM_UNLOCK(unit);
	
    return rv;
}

int bcm_common_sat_detach(int unit)
{
    int rv = BCM_E_UNAVAIL;
    _bcm_sat_data_t *psat_data;

    psat_data = &(sat_data[unit]);
    rv = BCM_E_NONE;
    sal_memset(psat_data, 0, sizeof(_bcm_sat_data_t));

    if (psat_data->sat_mutex != NULL) {
        sal_mutex_destroy(psat_data->sat_mutex);
		psat_data->sat_mutex = NULL;
    }

    return rv;
}
#endif /* BCM_SAT_SUPPORT */

void bcm_sat_config_t_init(bcm_sat_config_t *conf)
{
    if (NULL != conf) {
        sal_memset(conf, 0, sizeof (*conf));
    }
    return;
}

int
bcm_common_sat_config_get(
    int unit,
    bcm_sat_config_t* conf)
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_SAT_SUPPORT
    soc_sat_config_t soc_conf;

    SAT_NULL_CHECK(conf);
    sal_memset(&soc_conf, 0, sizeof(soc_sat_config_t));

    _BCM_RX_SYSTEM_LOCK(unit);
     rv = MBCM_SAT_DRIVER_CALL(unit, mbcm_sat_config_get, (unit, &soc_conf));
    _BCM_RX_SYSTEM_UNLOCK(unit); 
    conf->config_flags = soc_conf.config_flags;
    conf->timestamp_format = soc_conf.timestamp_format;
#endif /* BCM_SAT_SUPPORT */

    return rv;
}

int
bcm_common_sat_config_set(
    int unit,
    bcm_sat_config_t* conf)

{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_SAT_SUPPORT
    soc_sat_config_t soc_conf;

    SAT_NULL_CHECK(conf);
    SAT_VALUE_CHECK(conf->timestamp_format, bcmSATTimestampFormatIEEE1588v1, (bcmSATTimestampFormatNTP+1));
    sal_memset(&soc_conf, 0, sizeof(soc_sat_config_t));
    soc_conf.config_flags = conf->config_flags;
    soc_conf.timestamp_format = conf->timestamp_format;
    _BCM_RX_SYSTEM_LOCK(unit);
     rv = MBCM_SAT_DRIVER_CALL(unit, mbcm_sat_config_set, (unit, &soc_conf));
    _BCM_RX_SYSTEM_UNLOCK(unit);    
#endif /* BCM_SAT_SUPPORT */

    return rv;
}

#ifdef BCM_SAT_SUPPORT
STATIC int _bcm_common_sat_gtf_freed_id_find (
     _bcm_sat_data_t* psat_data,
     bcm_sat_gtf_t* gtf_id
    )
{
    int rv = BCM_E_NONE;
    int idx = 0;

    for (idx = _BCM_SAT_GTF_ID_MIN; idx <= _BCM_SAT_GTF_ID_MAX; idx++) {
        if (!SAT_GTF_ID_EXIST(psat_data, idx)) {
            break;
        }
    }

    if (idx == (_BCM_SAT_GTF_ID_MAX + 1)) {
        rv = BCM_E_RESOURCE;
    } else {
        *gtf_id = idx;
    }

    return rv;
}
#endif

void bcm_sat_header_user_define_offsets_t_init(bcm_sat_header_user_define_offsets_t *offsets)
{
    if (NULL != offsets) {
        sal_memset(offsets, 0, sizeof (*offsets));
    }
    return;
}

void bcm_sat_payload_t_init(bcm_sat_payload_t *payload)
{
    if (NULL != payload) {
        sal_memset(payload, 0, sizeof (*payload));
    }
    return;
}

void bcm_sat_stamp_t_init(bcm_sat_stamp_t *stamp)
{
    if (NULL != stamp) {
        sal_memset(stamp, 0, sizeof (*stamp));
    }
    return;
}

void bcm_sat_gtf_packet_config_t_init(bcm_sat_gtf_packet_config_t *pkt_cfg)
{
    if (NULL != pkt_cfg) {
        sal_memset(pkt_cfg, 0, sizeof (*pkt_cfg));
    }
    return;
}

void bcm_sat_gtf_bandwidth_t_init(bcm_sat_gtf_bandwidth_t *bw)
{
    if (NULL != bw) {
        sal_memset(bw, 0, sizeof (*bw));
    }
    return;
}

void bcm_sat_gtf_rate_pattern_t_init(bcm_sat_gtf_rate_pattern_t *rate_pattern)
{
    if (NULL != rate_pattern) {
        sal_memset(rate_pattern, 0, sizeof (*rate_pattern));
    }
    return;
}

int bcm_common_sat_gtf_create (
    int            unit,
    uint32         flags,
    bcm_sat_gtf_t *gtf_id
    )
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_SAT_SUPPORT
    _bcm_sat_data_t *psat_data;
    bcm_sat_gtf_t tmp_gtf_id = 0;
    int pkt_gen_en = 0;

    SAT_NULL_CHECK(gtf_id);
    psat_data = &(sat_data[unit]);
    if (flags & BCM_SAT_GTF_WITH_ID) {
        SAT_VALUE_CHECK(*gtf_id, _BCM_SAT_GTF_ID_MIN, (_BCM_SAT_GTF_ID_MAX+1));
        if (SAT_GTF_ID_EXIST(psat_data, *gtf_id)) {
            return BCM_E_EXISTS;
        } else {
            tmp_gtf_id = *gtf_id;
        }
    } else {
        /* Find a freed gtf ID */
        rv = _bcm_common_sat_gtf_freed_id_find(psat_data, &tmp_gtf_id);
        if (rv != BCM_E_NONE) {
            return rv;
        }
    }

    /* Enable Genrate Packet */
    pkt_gen_en = 1;
    _BCM_RX_SYSTEM_LOCK(unit);
	rv = MBCM_SAT_DRIVER_CALL(unit, mbcm_sat_gtf_packet_gen_set, (unit, tmp_gtf_id, pkt_gen_en));
    _BCM_RX_SYSTEM_UNLOCK(unit);

    *gtf_id = tmp_gtf_id;
	
    SAT_GTF_ID_SET(psat_data, tmp_gtf_id);
    
#endif/* BCM_SAT_SUPPORT */

    return rv;
}

int bcm_common_sat_gtf_destroy (
    int unit,
    bcm_sat_gtf_t gtf_id
    )
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_SAT_SUPPORT
    _bcm_sat_data_t *psat_data;
    int pkt_gen_en = 0;

    SAT_VALUE_CHECK(gtf_id, _BCM_SAT_GTF_ID_MIN, (_BCM_SAT_GTF_ID_MAX+1));
    psat_data = &(sat_data[unit]);

    SAT_GTF_ID_NOT_EXIST_EXIT(psat_data, gtf_id);

    /* Disable Generate Packet */
    pkt_gen_en = 0;
    _BCM_RX_SYSTEM_LOCK(unit);
     rv = MBCM_SAT_DRIVER_CALL(unit, mbcm_sat_gtf_packet_gen_set, (unit, gtf_id, pkt_gen_en));
    _BCM_RX_SYSTEM_UNLOCK(unit);

    SAT_GTF_ID_CLR(psat_data, gtf_id);
#endif/* BCM_SAT_SUPPORT */

    return rv;
}

int bcm_common_sat_gtf_destroy_all (
    int unit
    )
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_SAT_SUPPORT
    _bcm_sat_data_t *psat_data;
    bcm_sat_gtf_t gtf_id;
    int pkt_gen_en = 0;

    rv = BCM_E_NONE;
    psat_data = &(sat_data[unit]);
    
    _BCM_RX_SYSTEM_LOCK(unit);
    for(gtf_id = _BCM_SAT_GTF_ID_MIN; gtf_id <= _BCM_SAT_GTF_ID_MAX; gtf_id++)
    {
        if (SAT_GTF_ID_EXIST(psat_data, gtf_id)) {
            /* Disable Generate Packet */
            rv = MBCM_SAT_DRIVER_CALL(unit, mbcm_sat_gtf_packet_gen_set, (unit, gtf_id, pkt_gen_en));
        }
    }
    _BCM_RX_SYSTEM_UNLOCK(unit);

    SAT_GTF_ID_CLR_ALL(psat_data);
#endif/* BCM_SAT_SUPPORT */

    return rv;
}

int
bcm_common_sat_gtf_traverse(
    int unit,
    bcm_sat_gtf_traverse_cb cb,
    void *user_data)
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_SAT_SUPPORT
    bcm_sat_gtf_t gtf_id;
    _bcm_sat_data_t *psat_data;

    SAT_NULL_CHECK(user_data);
    psat_data = &(sat_data[unit]);

    for(gtf_id = _BCM_SAT_GTF_ID_MIN; gtf_id <= _BCM_SAT_GTF_ID_MAX; gtf_id++)
    {
        if (SAT_GTF_ID_EXIST(psat_data, gtf_id)) {
            /* Invoke user callback. */
            (*cb)(unit, gtf_id, user_data);
        }
    }

    rv = BCM_E_NONE;
#endif/* BCM_SAT_SUPPORT */

    return rv;
}

int bcm_common_sat_gtf_packet_config (
    int unit,
    bcm_sat_gtf_t gtf_id, 
    bcm_sat_gtf_packet_config_t *config
    )
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_SAT_SUPPORT
    _bcm_sat_data_t *psat_data;
    soc_sat_gtf_packet_config_t soc_pkt_cfg;
    int i;
    int priority;
    int stamp_count[bcmSatStampCount] = {0};
	uint8 pkt_data[128];

    SAT_NULL_CHECK(config);
    SAT_VALUE_CHECK(gtf_id, _BCM_SAT_GTF_ID_MIN, (_BCM_SAT_GTF_ID_MAX+1));
    psat_data = &(sat_data[unit]);
    SAT_GTF_ID_NOT_EXIST_EXIT(psat_data, gtf_id);

    SAT_VALUE_CHECK(config->sat_header_type, bcmSatHeaderUserDefined, bcmSatHeadersCount);
    SAT_VALUE_CHECK(config->header_info.pkt_data[0].len, _BCM_SAT_GTF_PKT_HDR_LEN_MIN, (_BCM_SAT_GTF_PKT_HDR_LEN_MAX+1));
    SAT_VALUE_CHECK(config->payload.payload_type, bcmSatPayloadConstant8Bytes, bcmSatPayloadsCount);
    for (priority = 0; priority < BCM_SAT_GTF_NUM_OF_PRIORITIES; priority++) {

        SAT_VALUE_CHECK(config->packet_edit[priority].pattern_length, 1, (BCM_SAT_GTF_PACKET_MAX_PATTERN_LENGTH+1));
        for (i = 0; i < BCM_SAT_GTF_PACKET_MAX_PATTERN_LENGTH; i++) {
            SAT_VALUE_MAX_CHECK(config->packet_edit[priority].packet_length_pattern[i], (_BCM_SAT_GTF_PKT_LEN_PATN_INX_MAX+1));
        }
        SAT_VALUE_MAX_CHECK(config->packet_edit[priority].number_of_stamps, (BCM_SAT_GTF_MAX_STAMPS+1));
        SAT_VALUE_CHECK(config->packet_edit[priority].number_of_ctfs, _BCM_SAT_GTF_SEQ_PERIOD_MIN, (_BCM_SAT_GTF_SEQ_PERIOD_MAX+1));

        stamp_count[bcmSatStampConstant2Bit] = stamp_count[bcmSatStampCounter8Bit] = stamp_count[bcmSatStampCounter16Bit] = 0;
        for (i = 0; i < BCM_SAT_GTF_MAX_STAMPS; i++) {
            SAT_VALUE_CHECK(config->packet_edit[priority].stamps[i].stamp_type, bcmSatStampInvalid, bcmSatStampCount);
            SAT_VALUE_CHECK(config->packet_edit[priority].stamps[i].field_type, bcmSatStampFieldUserDefined, bcmSatStampFieldsCount);

            if ((config->packet_edit[priority].stamps[i].stamp_type == bcmSatStampCounter8Bit) || 
                (config->packet_edit[priority].stamps[i].stamp_type == bcmSatStampCounter16Bit)) {
                SAT_VALUE_CHECK(config->packet_edit[priority].stamps[i].inc_step, _BCM_SAT_GTF_STAMP_INC_STEP_MIN, _BCM_SAT_GTF_STAMP_INC_STEP_MAX+1);
                SAT_VALUE_CHECK(config->packet_edit[priority].stamps[i].inc_period_packets, _BCM_SAT_GTF_STAMP_INC_PERIOD_MIN, _BCM_SAT_GTF_STAMP_INC_PERIOD_MAX+1);
            }
            else if (config->packet_edit[priority].stamps[i].stamp_type == bcmSatStampConstant2Bit) {
                SAT_VALUE_MAX_CHECK(config->packet_edit[priority].stamps[i].value, (_BCM_SAT_GTF_STAMP_2_bit_VALUE_MAX+1));
            }

            stamp_count[config->packet_edit[priority].stamps[i].stamp_type]++;
        }

        if ((stamp_count[bcmSatStampConstant2Bit] > 1) || 
            (stamp_count[bcmSatStampCounter8Bit] > 1) ||
            (stamp_count[bcmSatStampCounter16Bit] > 1)) {
            return BCM_E_PARAM;
        }
    }
    
#if defined(BCM_JERICHO_SUPPORT)
    SAT_VALUE_MAX_CHECK(config->packet_context_id, (MAX_NUM_OF_CORES+1));
#endif
    sal_memset(&soc_pkt_cfg, 0, sizeof(soc_pkt_cfg));

    /* Don't allow to configure gtf packet during packet generate enable */
    soc_pkt_cfg.sat_header_type = config->sat_header_type;
    soc_pkt_cfg.header_info.pkt_data = &(soc_pkt_cfg.header_info._pkt_data);
    soc_pkt_cfg.header_info.blk_count = 1;
	soc_pkt_cfg.header_info.pkt_data[0].data = pkt_data;
    soc_pkt_cfg.header_info.pkt_data[0].len = config->header_info.pkt_data[0].len;
    sal_memcpy(soc_pkt_cfg.header_info.pkt_data[0].data, config->header_info.pkt_data[0].data, sizeof(uint8)*config->header_info.pkt_data[0].len);    
    soc_pkt_cfg.payload.payload_type = config->payload.payload_type;
    sal_memcpy(soc_pkt_cfg.payload.payload_pattern, config->payload.payload_pattern, sizeof(uint8)*BCM_SAT_PAYLOAD_MAX_PATTERN_SIZE);    
    
    for (priority = 0; priority < BCM_SAT_GTF_NUM_OF_PRIORITIES; priority++) {
        sal_memcpy(soc_pkt_cfg.packet_edit[priority].packet_length, config->packet_edit[priority].packet_length, sizeof(uint32)*BCM_SAT_GTF_PACKET_LENGTH_NUM_OF_PATTERNS); 
        sal_memcpy(soc_pkt_cfg.packet_edit[priority].packet_length_pattern, config->packet_edit[priority].packet_length_pattern, sizeof(uint32)*BCM_SAT_GTF_PACKET_MAX_PATTERN_LENGTH); 
        soc_pkt_cfg.packet_edit[priority].pattern_length = config->packet_edit[priority].pattern_length;

        soc_pkt_cfg.packet_edit[priority].number_of_stamps = config->packet_edit[priority].number_of_stamps;
        for (i = 0; i < config->packet_edit[priority].number_of_stamps; i++) {
            soc_pkt_cfg.packet_edit[priority].stamps[i].stamp_type = config->packet_edit[priority].stamps[i].stamp_type;
            soc_pkt_cfg.packet_edit[priority].stamps[i].field_type = config->packet_edit[priority].stamps[i].field_type;
            soc_pkt_cfg.packet_edit[priority].stamps[i].inc_step = config->packet_edit[priority].stamps[i].inc_step;
            soc_pkt_cfg.packet_edit[priority].stamps[i].inc_period_packets = config->packet_edit[priority].stamps[i].inc_period_packets;
            soc_pkt_cfg.packet_edit[priority].stamps[i].value = config->packet_edit[priority].stamps[i].value;
            soc_pkt_cfg.packet_edit[priority].stamps[i].offset= config->packet_edit[priority].stamps[i].offset;
            soc_pkt_cfg.packet_edit[priority].stamps[i].stamp_type = config->packet_edit[priority].stamps[i].stamp_type;
        }
        soc_pkt_cfg.packet_edit[priority].number_of_ctfs = config->packet_edit[priority].number_of_ctfs;
        if(config->packet_edit[priority].flags & BCM_SAT_GTF_PACKET_EDIT_ADD_END_TLV) {
            soc_pkt_cfg.packet_edit[priority].flags |= SOC_SAT_GTF_PACKET_EDIT_ADD_END_TLV;
        }
        if(config->packet_edit[priority].flags & BCM_SAT_GTF_PACKET_EDIT_ADD_CRC) {
            soc_pkt_cfg.packet_edit[priority].flags |= SOC_SAT_GTF_PACKET_EDIT_ADD_CRC;
        }
    }

    soc_pkt_cfg.packet_context_id = config->packet_context_id;
    soc_pkt_cfg.offsets.payload_offset = config->offsets.payload_offset;
    soc_pkt_cfg.offsets.timestamp_offset = config->offsets.timestamp_offset;
    soc_pkt_cfg.offsets.seq_number_offset = config->offsets.seq_number_offset;
    
    _BCM_RX_SYSTEM_LOCK(unit);
    rv = MBCM_SAT_DRIVER_CALL(unit, mbcm_sat_gtf_packet_config, (unit, gtf_id, &soc_pkt_cfg));
    _BCM_RX_SYSTEM_UNLOCK(unit);
        
#endif/* BCM_SAT_SUPPORT */

    return rv;
}

int bcm_common_sat_gtf_bandwidth_set (
    int unit, 
    bcm_sat_gtf_t gtf_id,	 
    int priority,
    bcm_sat_gtf_bandwidth_t *bw
    )
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_SAT_SUPPORT
    _bcm_sat_data_t *psat_data;
    soc_sat_gtf_bandwidth_t soc_bandwidth;
    
    SAT_NULL_CHECK(bw);
    SAT_VALUE_CHECK(gtf_id, _BCM_SAT_GTF_ID_MIN, (_BCM_SAT_GTF_ID_MAX+1));
    psat_data = &(sat_data[unit]);
    SAT_GTF_ID_NOT_EXIST_EXIT(psat_data, gtf_id);

    SAT_VALUE_CHECK(priority, _BCM_SAT_GTF_OBJ_COMMON, (_BCM_SAT_GTF_OBJ_EIR+1));       
    SAT_VALUE_CHECK(bw->rate, _BCM_SAT_GTF_BW_RATE_MIN, (_BCM_SAT_GTF_BW_RATE_MAX+1));
    SAT_VALUE_CHECK(bw->max_burst, _BCM_SAT_GTF_BW_BURST_MIN, (_BCM_SAT_GTF_BW_BURST_MAX+1));
    
    soc_bandwidth.rate = bw->rate;
    soc_bandwidth.max_burst = bw->max_burst;

    _BCM_RX_SYSTEM_LOCK(unit);
     rv = MBCM_SAT_DRIVER_CALL(unit, mbcm_sat_gtf_bandwidth_set, (unit, gtf_id, priority, &soc_bandwidth));
    _BCM_RX_SYSTEM_UNLOCK(unit);
#endif/* BCM_SAT_SUPPORT */

    return rv;
}

int bcm_common_sat_gtf_bandwidth_get (
    int unit, 
    bcm_sat_gtf_t gtf_id,	 
    int priority,
    bcm_sat_gtf_bandwidth_t *bw
    )
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_SAT_SUPPORT
    _bcm_sat_data_t *psat_data;
    soc_sat_gtf_bandwidth_t soc_bandwidth;

    SAT_NULL_CHECK(bw);
    SAT_VALUE_CHECK(gtf_id, _BCM_SAT_GTF_ID_MIN, (_BCM_SAT_GTF_ID_MAX+1));
    psat_data = &(sat_data[unit]);
    SAT_GTF_ID_NOT_EXIST_EXIT(psat_data, gtf_id);

    SAT_VALUE_CHECK(priority, _BCM_SAT_GTF_OBJ_COMMON, (_BCM_SAT_GTF_OBJ_EIR+1));       
    
    _BCM_RX_SYSTEM_LOCK(unit);
     rv = MBCM_SAT_DRIVER_CALL(unit, mbcm_sat_gtf_bandwidth_get, (unit, gtf_id, priority, &soc_bandwidth));
    _BCM_RX_SYSTEM_UNLOCK(unit);

    if (BCM_SUCCESS(rv)) {
        bw->rate = soc_bandwidth.rate;
        bw->max_burst = soc_bandwidth.max_burst;
    }
#endif/* BCM_SAT_SUPPORT */

    return rv;
}

int bcm_common_sat_gtf_rate_pattern_set (
	int unit,
	bcm_sat_gtf_t gtf_id,
	int priority,
	bcm_sat_gtf_rate_pattern_t *config
    )
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_SAT_SUPPORT
    _bcm_sat_data_t *psat_data;
    soc_sat_gtf_rate_pattern_t soc_rate_pattern;

    SAT_NULL_CHECK(config);
    SAT_VALUE_CHECK(gtf_id, _BCM_SAT_GTF_ID_MIN, (_BCM_SAT_GTF_ID_MAX+1));
    psat_data = &(sat_data[unit]);
    SAT_GTF_ID_NOT_EXIST_EXIT(psat_data, gtf_id);

    SAT_VALUE_CHECK(priority, _BCM_SAT_GTF_OBJ_CIR, (_BCM_SAT_GTF_OBJ_EIR+1));       

    SAT_VALUE_CHECK(config->high_threshold, _BCM_SAT_GTF_RATE_PATN_HIGH_TH_MIN, (_BCM_SAT_GTF_RATE_PATN_HIGH_TH_MAX+1));
    SAT_VALUE_CHECK(config->low_threshold, _BCM_SAT_GTF_RATE_PATN_LOW_TH_MIN, (_BCM_SAT_GTF_RATE_PATN_LOW_TH_MAX+1));
    SAT_VALUE_MAX_CHECK(config->stop_iter_count, (_BCM_SAT_GTF_RATE_PATN_STOP_ITER_MAX+1));
    if (config->high_threshold < config->low_threshold) {
        return BCM_E_PARAM;
    }
    sal_memset(&soc_rate_pattern, 0, sizeof(soc_rate_pattern));
    
    if(config->flags & BCM_SAT_GTF_RATE_PATTERN_STOP_INTERVAL_EQ_BURST) {
        soc_rate_pattern.flags |= SOC_SAT_GTF_RATE_PATTERN_STOP_INTERVAL_EQ_BURST;
    }
    soc_rate_pattern.rate_pattern_mode = config->rate_pattern_mode;
    soc_rate_pattern.high_threshold = config->high_threshold;
    soc_rate_pattern.low_threshold = config->low_threshold;
    soc_rate_pattern.stop_iter_count = config->stop_iter_count;
    soc_rate_pattern.stop_burst_count = config->stop_burst_count;
    soc_rate_pattern.stop_interval_count = config->stop_interval_count;
    soc_rate_pattern.burst_packet_weight = config->burst_packet_weight;
    soc_rate_pattern.interval_packet_weight = config->interval_packet_weight;

    _BCM_RX_SYSTEM_LOCK(unit);
     rv = MBCM_SAT_DRIVER_CALL(unit, mbcm_sat_gtf_rate_pattern_set, (unit, gtf_id, priority, &soc_rate_pattern));
    _BCM_RX_SYSTEM_UNLOCK(unit);
#endif/* BCM_SAT_SUPPORT */

    return rv;
}

int bcm_common_sat_gtf_rate_pattern_get (
	int unit,
	bcm_sat_gtf_t gtf_id,
	int priority,
	bcm_sat_gtf_rate_pattern_t *config
    )
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_SAT_SUPPORT
    _bcm_sat_data_t *psat_data;
    soc_sat_gtf_rate_pattern_t soc_rate_pattern;

    SAT_NULL_CHECK(config);
    SAT_VALUE_CHECK(gtf_id, _BCM_SAT_GTF_ID_MIN, (_BCM_SAT_GTF_ID_MAX+1));
    psat_data = &(sat_data[unit]);
    SAT_GTF_ID_NOT_EXIST_EXIT(psat_data, gtf_id);

    SAT_VALUE_CHECK(priority, _BCM_SAT_GTF_OBJ_CIR, (_BCM_SAT_GTF_OBJ_EIR+1));       
    sal_memset(&soc_rate_pattern, 0, sizeof(soc_rate_pattern));

    _BCM_RX_SYSTEM_LOCK(unit);
     rv = MBCM_SAT_DRIVER_CALL(unit, mbcm_sat_gtf_rate_pattern_get, (unit, gtf_id, priority, &soc_rate_pattern));
    _BCM_RX_SYSTEM_UNLOCK(unit);

    if (BCM_SUCCESS(rv)) {
        config->rate_pattern_mode = soc_rate_pattern.rate_pattern_mode;
        config->high_threshold = soc_rate_pattern.high_threshold;
        config->low_threshold = soc_rate_pattern.low_threshold;
        config->stop_iter_count = soc_rate_pattern.stop_iter_count;
        config->stop_burst_count = soc_rate_pattern.stop_burst_count;
        config->stop_interval_count = soc_rate_pattern.stop_interval_count;
        config->burst_packet_weight = soc_rate_pattern.burst_packet_weight;
        config->interval_packet_weight = soc_rate_pattern.interval_packet_weight;
        if(soc_rate_pattern.flags & BCM_SAT_GTF_RATE_PATTERN_STOP_INTERVAL_EQ_BURST) {
            config->flags |= SOC_SAT_GTF_RATE_PATTERN_STOP_INTERVAL_EQ_BURST;
        }        
    }
#endif/* BCM_SAT_SUPPORT */

    return rv;
}

int bcm_common_sat_gtf_stat_get (
	int unit,
	bcm_sat_gtf_t gtf_id,
	int priority,
	uint32 flags,
	bcm_sat_gtf_stat_counter_t type,
	uint64* value
    )
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_SAT_SUPPORT
    _bcm_sat_data_t *psat_data;
    soc_sat_gtf_stat_counter_t stat_type;
    uint64 stat_cur;

    SAT_NULL_CHECK(value);
    SAT_VALUE_CHECK(gtf_id, _BCM_SAT_GTF_ID_MIN, (_BCM_SAT_GTF_ID_MAX+1));
    psat_data = &(sat_data[unit]);
    SAT_GTF_ID_NOT_EXIST_EXIT(psat_data, gtf_id);

    SAT_VALUE_CHECK(priority, _BCM_SAT_GTF_OBJ_CIR, (_BCM_SAT_GTF_OBJ_EIR+1));       
    /* parameter check */
    if ((type != bcmSatGtfStatPacketCount) || (flags != 0)) {
        return BCM_E_PARAM;
    }

    stat_type = type;
    COMPILER_64_ZERO(stat_cur);

    _BCM_RX_SYSTEM_LOCK(unit);
     rv = MBCM_SAT_DRIVER_CALL(unit, mbcm_sat_gtf_stat_get, (unit, gtf_id, priority, flags, stat_type, &stat_cur));
    _BCM_RX_SYSTEM_UNLOCK(unit);

    COMPILER_64_ZERO(*value);
    COMPILER_64_ADD_64(*value, stat_cur);
#endif/* BCM_SAT_SUPPORT */

    return rv;
}

/* CTF functions */
void bcm_sat_ctf_packet_info_t_init(bcm_sat_ctf_packet_info_t *packet_info)
{
    if (NULL != packet_info) {
        sal_memset(packet_info, 0, sizeof (*packet_info));
    }
    return;
}

void bcm_sat_ctf_identifier_t_init(bcm_sat_ctf_identifier_t *ctf_identifier)
{
    if (NULL != ctf_identifier) {
        sal_memset(ctf_identifier, 0, sizeof (*ctf_identifier));
    }
    return;
}

void bcm_sat_ctf_bin_limit_t_init(bcm_sat_ctf_bin_limit_t *bins)
{
    if (NULL != bins) {
        sal_memset(bins, 0, sizeof (*bins));
    }
    return;
}

void bcm_sat_ctf_stat_config_t_init(bcm_sat_ctf_stat_config_t *stat_cfg)
{
    if (NULL != stat_cfg) {
        sal_memset(stat_cfg, 0, sizeof (*stat_cfg));
    }
    return;
}

void bcm_sat_ctf_stat_t_init(bcm_sat_ctf_stat_t *stat)
{
    if (NULL != stat) {
        sal_memset(stat, 0, sizeof (*stat));
    }
    return;
}

void bcm_sat_ctf_availability_config_t_init(bcm_sat_ctf_availability_config_t *config)
{
    if (NULL != config) {
        sal_memset(config, 0, sizeof (*config));
    }
    return;
}

void bcm_sat_ctf_report_config_t_init(bcm_sat_ctf_report_config_t *reports)
{
    if (NULL != reports) {
        sal_memset(reports, 0, sizeof (*reports));
    }
    return;
}

#ifdef BCM_SAT_SUPPORT
STATIC int _bcm_common_sat_ctf_freed_id_find (
     _bcm_sat_data_t* psat_data,
     bcm_sat_ctf_t* ctf_id
    )
{
    int rv = BCM_E_NONE;
    int idx = 0;

    for (idx = _BCM_SAT_CTF_ID_MIN; idx <= _BCM_SAT_CTF_ID_MAX; idx++) {
        if (!SAT_CTF_ID_EXIST(psat_data, idx)) {
            break;
        }
    }

    if (idx == (_BCM_SAT_CTF_ID_MAX + 1)) {
        rv = BCM_E_RESOURCE;
    } else {
        *ctf_id = idx;
    }

    return rv;
}

STATIC int _bcm_common_sat_ctf_freed_trap_id_find (
     _bcm_sat_data_t* psat_data,
     int32* trap_idx
    )
{
    int rv = BCM_E_NONE;
    int idx = 0;

    for (idx = 0; idx < _BCM_SAT_CTF_TRAP_ID_MAX_NUM; idx++) {
        if (!SAT_CTF_TRAP_ID_EXIST(psat_data, idx)) {
            break;
        }
    }

    if (idx == _BCM_SAT_CTF_TRAP_ID_MAX_NUM) {
        rv = BCM_E_RESOURCE;
    } else {
        *trap_idx = idx;
    }

    return rv;
}

STATIC int _bcm_common_sat_trap_id_find (
     _bcm_sat_data_t* psat_data,
     uint32 trap_id,
     int32* trap_idx
    )
{
    int rv = BCM_E_NONE;
    int idx = 0;

    for (idx = 0; idx < _BCM_SAT_CTF_TRAP_ID_MAX_NUM; idx++) {
        if (trap_id == psat_data->ctf_trap_id[idx] && SAT_CTF_TRAP_ID_EXIST(psat_data, idx)) {
            break;
        }
    }

    if (idx == _BCM_SAT_CTF_TRAP_ID_MAX_NUM) {
        rv = BCM_E_NOT_FOUND;
    } else {
        *trap_idx = idx;
    }

    return rv;
}

STATIC int _bcm_common_sat_ctf_trap_entry_find (
     _bcm_sat_data_t* psat_data,
     uint32  trap_data,
     uint32  trap_mask,
     uint32  *entry_idx
    )
{
    int rv = BCM_E_NONE;
    int idx = 0;
    uint8 found = FALSE;

    /* Check the entry whether is existed */
    for (idx = 0; idx < _BCM_SAT_CTF_TRAP_DATA_ENTRY_NUM; idx++) {
        if (psat_data->trap_data[idx].trap_data == trap_data &&
            psat_data->trap_data[idx].trap_mask == trap_mask &&
            SAT_BITMAP_EXIST(psat_data->ctf_ssid_bitmap, idx)) {
            found = TRUE;
            break;
        }
    }

    if (found == TRUE) {
        *entry_idx = idx;
    } else {
        rv = BCM_E_NOT_FOUND;
    }

    return rv;
}


STATIC int _bcm_common_sat_ctf_freed_trap_entry_find (
     _bcm_sat_data_t* psat_data,
     uint32  trap_data,
     uint32  trap_mask,
     uint32  *entry_idx
    )
{
    int rv = BCM_E_NONE;
    int idx = 0;

    /* Check the entry whether is existed */
    rv = _bcm_common_sat_ctf_trap_entry_find(psat_data, trap_data, trap_mask, entry_idx);
    if (rv == BCM_E_NONE) {
        *entry_idx = idx;
        rv = BCM_E_EXISTS;
    } else {
        /* Find a freed entry */
        rv = BCM_E_NONE;
        for (idx = 0; idx < _BCM_SAT_CTF_TRAP_DATA_ENTRY_NUM; idx++) {
            if (!SAT_BITMAP_EXIST(psat_data->ctf_ssid_bitmap, idx)) {
                break;
            }
        }

        if (idx == _BCM_SAT_CTF_TRAP_DATA_ENTRY_NUM) {
            rv = BCM_E_RESOURCE;
        } else {
            *entry_idx = idx;
        }
    }

    return rv;
}
#endif

int bcm_common_sat_ctf_create (
     int unit,
     uint32 flags,
     bcm_sat_ctf_t *ctf_id
    )
{
    int rv = BCM_E_UNAVAIL;
 #ifdef BCM_SAT_SUPPORT   
    _bcm_sat_data_t *psat_data;
    bcm_sat_ctf_t tmp_ctf_id = 0;

    SAT_NULL_CHECK(ctf_id);
    rv = BCM_E_NONE;
    psat_data = &(sat_data[unit]);
    if (flags & BCM_SAT_CTF_WITH_ID) {
        /* In case return BCM_E_PARAM if ctf_id is invalid */
        SAT_VALUE_CHECK(*ctf_id, _BCM_SAT_CTF_ID_MIN, (_BCM_SAT_CTF_ID_MAX+1));
        if (SAT_CTF_ID_EXIST(psat_data, *ctf_id)) {
            return BCM_E_EXISTS;
        } else {
            tmp_ctf_id = *ctf_id;
        }
    } else {
        /* Find a freed CTF ID */
        /* In case return BCM_E_RESOURCE if haven't resource.
         */
        rv = _bcm_common_sat_ctf_freed_id_find(psat_data, &tmp_ctf_id);
        if (rv != BCM_E_NONE) {
            return rv;
        }
        *ctf_id = tmp_ctf_id;
    }
    SAT_CTF_ID_SET(psat_data, tmp_ctf_id);
#endif /* BCM_SAT_SUPPORT */

    return rv;
}

int bcm_common_sat_ctf_destroy (
    int unit,
    bcm_sat_ctf_t ctf_id
    )
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_SAT_SUPPORT
    _bcm_sat_data_t *psat_data;

    rv = BCM_E_NONE;
    SAT_VALUE_CHECK(ctf_id, _BCM_SAT_CTF_ID_MIN, (_BCM_SAT_CTF_ID_MAX+1));
    psat_data = &(sat_data[unit]);
    SAT_CTF_ID_NOT_EXIST_EXIT(psat_data, ctf_id);
    SAT_CTF_ID_CLR(psat_data, ctf_id);
#endif /* BCM_SAT_SUPPORT */

    return rv;
}

int bcm_common_sat_ctf_destroy_all (
    int unit
    )
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_SAT_SUPPORT
    _bcm_sat_data_t *psat_data;

    rv = BCM_E_NONE;
    psat_data = &(sat_data[unit]);
    SAT_CTF_ID_CLR_ALL(psat_data);
#endif/* BCM_SAT_SUPPORT */

    return rv;
}


int
bcm_common_sat_ctf_traverse(
    int unit,
    bcm_sat_ctf_traverse_cb cb,
    void *user_data)
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_SAT_SUPPORT
    bcm_sat_ctf_t ctf_id;
    _bcm_sat_data_t *psat_data;

    rv = BCM_E_NONE;
    SAT_NULL_CHECK(user_data);
    psat_data = &(sat_data[unit]);
    for(ctf_id = _BCM_SAT_CTF_ID_MIN; ctf_id <= _BCM_SAT_CTF_ID_MAX; ctf_id++)
    {
        if (SAT_CTF_ID_EXIST(psat_data, ctf_id)) {
            /* Invoke user callback. */
            (*cb)(unit, ctf_id, user_data);
        }
    }
#endif/* BCM_SAT_SUPPORT */

    return rv;
}

int bcm_common_sat_ctf_packet_config (
	int unit,
	bcm_sat_ctf_t ctf_id,
	bcm_sat_ctf_packet_info_t *packet_info
    )
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_SAT_SUPPORT
    soc_sat_ctf_packet_info_t soc_packet_info;
    _bcm_sat_data_t *psat_data;

    SAT_NULL_CHECK(packet_info);
    SAT_VALUE_CHECK(ctf_id, _BCM_SAT_CTF_ID_MIN, (_BCM_SAT_CTF_ID_MAX+1));
    psat_data = &(sat_data[unit]);
    SAT_CTF_ID_NOT_EXIST_EXIT(psat_data, ctf_id);
    SAT_VALUE_CHECK(packet_info->sat_header_type, bcmSatHeaderUserDefined, bcmSatHeadersCount);
    SAT_VALUE_CHECK(packet_info->payload.payload_type, bcmSatPayloadConstant8Bytes, bcmSatPayloadsCount);
    SAT_VALUE_CHECK(packet_info->offsets.payload_offset, _BCM_SAT_PAYLOAD_OFFSET_MIN, (_BCM_SAT_PAYLOAD_OFFSET_MAX+1));
    SAT_VALUE_CHECK(packet_info->offsets.seq_number_offset, _BCM_SAT_SEQ_NUM_OFFSET_MIN, (_BCM_SAT_SEQ_NUM_OFFSET_MAX+1));
    SAT_VALUE_CHECK(packet_info->offsets.timestamp_offset, _BCM_SAT_TIME_STAMP_OFFSET_MIN, (_BCM_SAT_TIME_STAMP_OFFSET_MAX+1));
    sal_memset(&soc_packet_info, 0, sizeof(soc_packet_info));

    if(packet_info->flags & BCM_SAT_CTF_PACKET_INFO_ADD_END_TLV) {
        soc_packet_info.flags |= SOC_SAT_CTF_PACKET_INFO_ADD_END_TLV;
    }
    if(packet_info->flags & BCM_SAT_CTF_PACKET_INFO_ADD_CRC) {
        soc_packet_info.flags |= SOC_SAT_CTF_PACKET_INFO_ADD_CRC;
    }
    soc_packet_info.sat_header_type = packet_info->sat_header_type;
    soc_packet_info.payload.payload_type = packet_info->payload.payload_type;
    sal_memcpy(soc_packet_info.payload.payload_pattern, packet_info->payload.payload_pattern, sizeof(uint8)*BCM_SAT_PAYLOAD_MAX_PATTERN_SIZE);
    if (packet_info->sat_header_type == bcmSatHeaderUserDefined) {
        soc_packet_info.offsets.payload_offset    = packet_info->offsets.payload_offset;
        soc_packet_info.offsets.seq_number_offset = packet_info->offsets.seq_number_offset;    
        soc_packet_info.offsets.timestamp_offset  = packet_info->offsets.timestamp_offset;
    }
    else if (packet_info->sat_header_type == bcmSatHeaderY1731) {
        soc_packet_info.offsets.payload_offset    = 
            (packet_info->offsets.payload_offset ? packet_info->offsets.payload_offset : _BCM_SAT_Y1731_PAYLOAD_OFFSET);
        soc_packet_info.offsets.seq_number_offset = 
            (packet_info->offsets.seq_number_offset ? packet_info->offsets.seq_number_offset : _BCM_SAT_Y1731_SEQ_NUM_OFFSET);
        soc_packet_info.offsets.timestamp_offset  = 
            (packet_info->offsets.timestamp_offset ? packet_info->offsets.timestamp_offset : _BCM_SAT_Y1731_TIME_STAMP_OFFSET);
    }
    else if (packet_info->sat_header_type == bcmSatHeaderMEF) {
        soc_packet_info.offsets.payload_offset    = 
            (packet_info->offsets.payload_offset ? packet_info->offsets.payload_offset : _BCM_SAT_MEF_PAYLOAD_OFFSET);
        soc_packet_info.offsets.seq_number_offset = 
            (packet_info->offsets.seq_number_offset ? packet_info->offsets.seq_number_offset : _BCM_SAT_MEF_SEQ_NUM_OFFSET);
        soc_packet_info.offsets.timestamp_offset  = 
            (packet_info->offsets.timestamp_offset ? packet_info->offsets.timestamp_offset : _BCM_SAT_MEF_TIME_STAMP_OFFSET);
    }
    _BCM_RX_SYSTEM_LOCK(unit);
     rv = MBCM_SAT_DRIVER_CALL(unit, mbcm_sat_ctf_packet_config, (unit, ctf_id, &soc_packet_info));
    _BCM_RX_SYSTEM_UNLOCK(unit);
#endif/* BCM_SAT_SUPPORT */

    return rv;
}

int bcm_common_sat_ctf_identifier_map (
	int unit,
	bcm_sat_ctf_identifier_t *identifier,
	bcm_sat_ctf_t ctf_id
    )
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_SAT_SUPPORT
    soc_sat_ctf_identifier_t soc_identifier;
    _bcm_sat_data_t *psat_data;

    SAT_NULL_CHECK(identifier);
    SAT_VALUE_CHECK(ctf_id, _BCM_SAT_CTF_ID_MIN, _BCM_SAT_CTF_ID_MAX);
    psat_data = &(sat_data[unit]);
    SAT_CTF_ID_NOT_EXIST_EXIT(psat_data, ctf_id);
    SAT_VALUE_MAX_CHECK(identifier->session_id, (_BCM_SAT_CTF_OAM_ID_MAX+1));
    SAT_VALUE_MAX_CHECK(identifier->trap_id, (_BCM_SAT_CTF_TRAP_ID_MAX+1));
    SAT_VALUE_MAX_CHECK(identifier->color, (_BCM_SAT_CTF_COLOR_MAX+1));
    SAT_VALUE_MAX_CHECK(identifier->tc, (_BCM_SAT_CTF_COS_MAX+1));
    sal_memset(&soc_identifier, 0, sizeof(soc_identifier));
    soc_identifier.session_id = identifier->session_id;
    soc_identifier.trap_id = identifier->trap_id;
    soc_identifier.color = identifier->color;
    soc_identifier.tc = identifier->tc;
    _BCM_RX_SYSTEM_LOCK(unit);
     rv = MBCM_SAT_DRIVER_CALL(unit, mbcm_sat_ctf_identifier_map, (unit, &soc_identifier, ctf_id));
    _BCM_RX_SYSTEM_UNLOCK(unit);
#endif/* BCM_SAT_SUPPORT */

    return rv;
}

int bcm_common_sat_ctf_identifier_unmap (
	int unit,
	bcm_sat_ctf_identifier_t *identifier
    )
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_SAT_SUPPORT
    soc_sat_ctf_identifier_t soc_identifier;

    SAT_NULL_CHECK(identifier);
    SAT_VALUE_MAX_CHECK(identifier->session_id, (_BCM_SAT_CTF_OAM_ID_MAX+1));
    SAT_VALUE_MAX_CHECK(identifier->trap_id, (_BCM_SAT_CTF_TRAP_ID_MAX+1));
    SAT_VALUE_MAX_CHECK(identifier->color, (_BCM_SAT_CTF_COLOR_MAX+1));
    SAT_VALUE_MAX_CHECK(identifier->tc, (_BCM_SAT_CTF_COS_MAX+1));
    sal_memset(&soc_identifier, 0, sizeof(soc_identifier));
    soc_identifier.session_id = identifier->session_id;
    soc_identifier.trap_id = identifier->trap_id;
    soc_identifier.color = identifier->color;
    soc_identifier.tc = identifier->tc;
    _BCM_RX_SYSTEM_LOCK(unit);
     rv = MBCM_SAT_DRIVER_CALL(unit, mbcm_sat_ctf_identifier_unmap, (unit, &soc_identifier));
    _BCM_RX_SYSTEM_UNLOCK(unit);
#endif/* BCM_SAT_SUPPORT */

    return rv;
}

int bcm_common_sat_ctf_trap_add (
	int unit, 
	uint32 trap_id
    )
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_SAT_SUPPORT
    _bcm_sat_data_t *psat_data;
    int trap_idx = 0;

    SAT_VALUE_MAX_CHECK(trap_id, (_BCM_SAT_CTF_TRAP_ID_VAL_MAX+1));
    psat_data = &(sat_data[unit]);
    rv = _bcm_common_sat_trap_id_find(psat_data, trap_id, &trap_idx);
    if (rv == BCM_E_NONE) {
        rv = BCM_E_EXISTS;
        return rv;
    }
    rv = BCM_E_NONE;
    rv = _bcm_common_sat_ctf_freed_trap_id_find(psat_data, &trap_idx);
    if (rv != BCM_E_NONE) {
        return rv;
    }

    SAT_CTF_TRAP_ID_SET(psat_data, trap_idx, trap_id);
    _BCM_RX_SYSTEM_LOCK(unit);
     rv = MBCM_SAT_DRIVER_CALL(unit, mbcm_sat_ctf_trap_set, (unit, _BCM_SAT_CTF_TRAP_ID_MAX_NUM, psat_data->ctf_trap_id));
    _BCM_RX_SYSTEM_UNLOCK(unit);    
#endif/* BCM_SAT_SUPPORT */

    return rv;
}

int bcm_common_sat_ctf_trap_remove (
	int unit, 
	uint32 trap_id
    )
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_SAT_SUPPORT
    _bcm_sat_data_t *psat_data;
    int trap_idx = 0;

    SAT_VALUE_MAX_CHECK(trap_id, (_BCM_SAT_CTF_TRAP_ID_VAL_MAX+1));
    psat_data = &(sat_data[unit]);
    rv = _bcm_common_sat_trap_id_find(psat_data, trap_id, &trap_idx);
    if (rv != BCM_E_NONE) {
        return rv;
    }
    SAT_CTF_TRAP_ID_CLR(psat_data, trap_idx);
    _BCM_RX_SYSTEM_LOCK(unit);
     rv = MBCM_SAT_DRIVER_CALL(unit, mbcm_sat_ctf_trap_set, (unit, _BCM_SAT_CTF_TRAP_ID_MAX_NUM, psat_data->ctf_trap_id));
    _BCM_RX_SYSTEM_UNLOCK(unit);    
#endif/* BCM_SAT_SUPPORT */

    return rv;
}

int bcm_common_sat_ctf_trap_remove_all
    (
	int unit
    )
{
    int rv = BCM_E_UNAVAIL;
 #ifdef BCM_SAT_SUPPORT   
    _bcm_sat_data_t *psat_data;
    int trap_idx = 0;

    psat_data = &(sat_data[unit]);
    SAT_CTF_TRAP_ID_CLR_ALL(psat_data);
    _BCM_RX_SYSTEM_LOCK(unit);
     rv = MBCM_SAT_DRIVER_CALL(unit, mbcm_sat_ctf_trap_set, (unit, _BCM_SAT_CTF_TRAP_ID_MAX_NUM, psat_data->ctf_trap_id));
    _BCM_RX_SYSTEM_UNLOCK(unit);    
#endif/* BCM_SAT_SUPPORT */

    return rv;
}

int bcm_common_sat_ctf_bin_limit_set (
	int unit, 
	int bins_count, 
 	bcm_sat_ctf_bin_limit_t* bins
    )
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_SAT_SUPPORT
    soc_sat_ctf_bin_limit_t soc_bins_limit[_BCM_SAT_CTF_BINS_LIMIT_CNT_MAX];
    int idx = 0;

    SAT_NULL_CHECK(bins);
    SAT_VALUE_CHECK(bins_count, _BCM_SAT_CTF_BINS_LIMIT_CNT_MIN, (_BCM_SAT_CTF_BINS_LIMIT_CNT_MAX+1));
    sal_memset(&soc_bins_limit, 0, sizeof(soc_bins_limit));
    for (idx = 0; idx < bins_count; idx++) {
        SAT_VALUE_MAX_CHECK(bins[idx].bin_select, _BCM_SAT_CTF_BINS_LIMIT_CNT_MAX);
        soc_bins_limit[idx].bin_select = bins[idx].bin_select;
        soc_bins_limit[idx].bin_limit = bins[idx].bin_limit;
    }
    _BCM_RX_SYSTEM_LOCK(unit);
    rv = MBCM_SAT_DRIVER_CALL(unit, mbcm_sat_ctf_bin_limit_set, (unit, bins_count, soc_bins_limit));
    _BCM_RX_SYSTEM_UNLOCK(unit);    
#endif/* BCM_SAT_SUPPORT */

    return rv;
}

int bcm_common_sat_ctf_bin_limit_get (
	int unit, 
	int max_bins_count,
	int * bins_count, 
 	bcm_sat_ctf_bin_limit_t* bins
    )
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_SAT_SUPPORT
    soc_sat_ctf_bin_limit_t soc_bins_limit[_BCM_SAT_CTF_BINS_LIMIT_CNT_MAX];
    int idx = 0;

    SAT_NULL_CHECK(bins_count);
    SAT_NULL_CHECK(bins);
    SAT_VALUE_CHECK(max_bins_count, _BCM_SAT_CTF_BINS_LIMIT_CNT_MIN, _BCM_SAT_CTF_BINS_LIMIT_CNT_MAX+1);
    sal_memset(&soc_bins_limit, 0, sizeof(soc_bins_limit));
    _BCM_RX_SYSTEM_LOCK(unit);
     rv = MBCM_SAT_DRIVER_CALL(unit, mbcm_sat_ctf_bin_limit_get, (unit, max_bins_count, bins_count, soc_bins_limit));
    _BCM_RX_SYSTEM_UNLOCK(unit); 
    for (idx = 0; idx < (*bins_count); idx++) {
        bins[idx].bin_select = soc_bins_limit[idx].bin_select;
        bins[idx].bin_limit = soc_bins_limit[idx].bin_limit;
    }
#endif/* BCM_SAT_SUPPORT */

    return rv;
}

int bcm_common_sat_ctf_stat_config_set (
	int unit,
	bcm_sat_ctf_t ctf_id,
	bcm_sat_ctf_stat_config_t *stat
    )
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_SAT_SUPPORT
    soc_sat_ctf_stat_config_t soc_sat_ctf_stat_cfg;
    _bcm_sat_data_t *psat_data;

    psat_data = &(sat_data[unit]);
    SAT_NULL_CHECK(stat);
    SAT_VALUE_CHECK(ctf_id, _BCM_SAT_CTF_ID_MIN, (_BCM_SAT_CTF_ID_MAX+1));
    SAT_CTF_ID_NOT_EXIST_EXIT(psat_data, ctf_id);
    sal_memset(&soc_sat_ctf_stat_cfg, 0, sizeof(soc_sat_ctf_stat_cfg));
    soc_sat_ctf_stat_cfg.bin_min_delay = stat->bin_min_delay;
    soc_sat_ctf_stat_cfg.bin_step = stat->bin_step;
    soc_sat_ctf_stat_cfg.use_global_bin_config = (stat->use_global_bin_config ? 1 : 0);
    soc_sat_ctf_stat_cfg.update_counters_in_unvavail_state = (stat->update_counters_in_unvavail_state ? 1 : 0);
    _BCM_RX_SYSTEM_LOCK(unit);
    rv = MBCM_SAT_DRIVER_CALL(unit, mbcm_sat_ctf_stat_config_set, (unit, ctf_id, &soc_sat_ctf_stat_cfg));
    _BCM_RX_SYSTEM_UNLOCK(unit);
#endif/* BCM_SAT_SUPPORT */

    return rv;
}

int bcm_common_sat_ctf_stat_get (
	int unit, 
	bcm_sat_ctf_t ctf_id,
	uint32 flags,
	bcm_sat_ctf_stat_t* stat
    )
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_SAT_SUPPORT
    _bcm_sat_data_t *psat_data;
	soc_sat_ctf_stat_t soc_stat;

    psat_data = &(sat_data[unit]);
    SAT_NULL_CHECK(stat);
    SAT_VALUE_CHECK(ctf_id, _BCM_SAT_CTF_ID_MIN, (_BCM_SAT_CTF_ID_MAX+1));
    SAT_CTF_ID_NOT_EXIST_EXIT(psat_data, ctf_id);
    sal_memset(&soc_stat, 0, sizeof(soc_sat_ctf_stat_t));
    _BCM_RX_SYSTEM_LOCK(unit);
     rv = MBCM_SAT_DRIVER_CALL(unit, mbcm_sat_ctf_stat_get, (unit, ctf_id, flags, &soc_stat));
    _BCM_RX_SYSTEM_UNLOCK(unit);
    sal_memcpy(stat, &soc_stat, sizeof(bcm_sat_ctf_stat_t));
#endif/* BCM_SAT_SUPPORT */

    return rv;
}

int bcm_common_sat_ctf_availability_config_set (
	int unit,
	bcm_sat_ctf_t ctf_id,
	bcm_sat_ctf_availability_config_t *config
	)
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_SAT_SUPPORT
    soc_sat_ctf_availability_config_t avail_cfg;
    _bcm_sat_data_t *psat_data;

    psat_data = &(sat_data[unit]);
    SAT_NULL_CHECK(config);
    SAT_VALUE_CHECK(ctf_id, _BCM_SAT_CTF_ID_MIN, (_BCM_SAT_CTF_ID_MAX+1));
    SAT_CTF_ID_NOT_EXIST_EXIT(psat_data, ctf_id);
    SAT_VALUE_MAX_CHECK(config->switch_state_num_of_slots,
                       (_BCM_SAT_CTF_SWITCH_STATE_NUM_OF_SLOTS_MAX+1));
    SAT_VALUE_MAX_CHECK(config->switch_state_threshold_per_slot,
                       (_BCM_SAT_CTF_SWITCH_STATE_THRES_PER_SLOT_MAX+1));
    sal_memset(&avail_cfg, 0, sizeof(avail_cfg));
    avail_cfg.switch_state_num_of_slots = config->switch_state_num_of_slots;
    avail_cfg.switch_state_threshold_per_slot = config->switch_state_threshold_per_slot;
    _BCM_RX_SYSTEM_LOCK(unit);
     rv = MBCM_SAT_DRIVER_CALL(unit, mbcm_sat_ctf_availability_config_set, (unit, ctf_id, &avail_cfg));
    _BCM_RX_SYSTEM_UNLOCK(unit);
#endif/* BCM_SAT_SUPPORT */

    return rv;
}

int bcm_common_sat_ctf_trap_data_to_session_map (
	int unit,
	uint32 trap_data, 
	uint32 trap_data_mask,
	uint32 session_id
    )
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_SAT_SUPPORT
    _bcm_sat_data_t *psat_data;
    uint32 entry_idx;

    SAT_VALUE_MAX_CHECK(trap_data, (_BCM_SAT_CTF_TRAP_DATA_MAX+1));
    SAT_VALUE_MAX_CHECK(trap_data_mask, (_BCM_SAT_CTF_TRAP_DATA_MASK_MAX+1));
    SAT_VALUE_MAX_CHECK(session_id, (_BCM_SAT_CTF_TRAP_DATA_SSID_MAX+1));

    psat_data = &(sat_data[unit]);
    rv = _bcm_common_sat_ctf_freed_trap_entry_find(psat_data, trap_data, trap_data_mask, &entry_idx);
    if (rv != BCM_E_NONE) {
        return rv;
    }
    _BCM_RX_SYSTEM_LOCK(unit);
     rv = MBCM_SAT_DRIVER_CALL(unit, mbcm_sat_ctf_trap_data_to_session_map, (unit, trap_data, trap_data_mask, entry_idx, session_id));
    _BCM_RX_SYSTEM_UNLOCK(unit);
    SAT_BITMAP_SET(psat_data->ctf_ssid_bitmap, entry_idx);
    psat_data->trap_data[entry_idx].trap_data = trap_data;
    psat_data->trap_data[entry_idx].trap_mask = trap_data_mask;
    psat_data->trap_data[entry_idx].ssid = session_id;
#endif/* BCM_SAT_SUPPORT */

    return rv;
}

int bcm_common_sat_ctf_trap_data_to_session_unmap (
	int unit,
	uint32 trap_data,
	uint32 trap_data_mask
    )
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_SAT_SUPPORT
    _bcm_sat_data_t *psat_data;
    uint32 entry_idx;

    SAT_VALUE_MAX_CHECK(trap_data, (_BCM_SAT_CTF_TRAP_DATA_MAX+1));
    SAT_VALUE_MAX_CHECK(trap_data_mask, (_BCM_SAT_CTF_TRAP_DATA_MASK_MAX+1));
    psat_data = &(sat_data[unit]);
    rv = _bcm_common_sat_ctf_trap_entry_find(psat_data, trap_data, trap_data_mask, &entry_idx);
    if (rv != BCM_E_NONE) {
        return rv;
    }
    _BCM_RX_SYSTEM_LOCK(unit);
     rv = MBCM_SAT_DRIVER_CALL(unit, mbcm_sat_ctf_trap_data_to_session_unmap, (unit, entry_idx));
    _BCM_RX_SYSTEM_UNLOCK(unit);
    SAT_BITMAP_CLR(psat_data->ctf_ssid_bitmap, entry_idx);
    sal_memset(&(psat_data->trap_data[entry_idx]), 0, sizeof(_bcm_sat_ctf_trap_data_t));
#endif/* BCM_SAT_SUPPORT */

    return rv;
}

int bcm_common_sat_ctf_reports_config_set (
	int unit,
	bcm_sat_ctf_t ctf_id,
	bcm_sat_ctf_report_config_t *reports
    )
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_SAT_SUPPORT
	soc_sat_ctf_report_config_t soc_reports;
    _bcm_sat_data_t *psat_data;

    psat_data = &(sat_data[unit]);
    SAT_NULL_CHECK(reports);
    SAT_VALUE_CHECK(ctf_id, _BCM_SAT_CTF_ID_MIN, (_BCM_SAT_CTF_ID_MAX+1));
    SAT_VALUE_MAX_CHECK(reports->report_sampling_percent, (_BCM_SAT_CTF_REPORT_SAMPLING_PER_MAX+1));
    SAT_CTF_ID_NOT_EXIST_EXIT(psat_data, ctf_id);
    sal_memset(&soc_reports, 0, sizeof(bcm_sat_ctf_report_config_t));
    soc_reports.report_sampling_percent = reports->report_sampling_percent;
    if (reports->flags & BCM_SAT_CTF_REPORT_ADD_SEQ_NUM) {
        soc_reports.flags |= SOC_SAT_CTF_REPORT_ADD_SEQ_NUM;
    }
    if (reports->flags & BCM_SAT_CTF_REPORT_ADD_DELAY) {
        soc_reports.flags |= SOC_SAT_CTF_REPORT_ADD_DELAY;
    }
    _BCM_RX_SYSTEM_LOCK(unit);
     rv = MBCM_SAT_DRIVER_CALL(unit, mbcm_sat_ctf_reports_config_set, (unit, ctf_id, &soc_reports));
    _BCM_RX_SYSTEM_UNLOCK(unit);    
#endif/* BCM_SAT_SUPPORT */

    return rv;
}

int
bcm_common_sat_event_register(
    int unit,
    bcm_sat_event_type_t event_type,
    bcm_sat_event_cb cb,
    void *user_data)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_SAT_SUPPORT

#endif/* BCM_SAT_SUPPORT */

    return rv;
}

int
bcm_common_sat_event_unregister(
    int unit,
    bcm_sat_event_type_t event_type,
    bcm_sat_event_cb cb)
{
    int rv = BCM_E_UNAVAIL;

#ifdef BCM_SAT_SUPPORT
        
#endif/* BCM_SAT_SUPPORT */

    return rv;
}


/* For india */
int
bcm_common_sat_endpoint_create(
    int unit,
    bcm_sat_endpoint_info_t *endpoint_info)
{
    int rv = BCM_E_UNAVAIL;


    return rv;
}

int
bcm_common_sat_endpoint_destroy(
    int unit,
    bcm_sat_endpoint_t endpoint,
    uint32 flags)

{
    int rv = BCM_E_UNAVAIL;


    return rv;

}

int
bcm_common_sat_endpoint_destroy_all(
    int unit,
    uint32 flags)
{
    int rv = BCM_E_UNAVAIL;


    return rv;

}


int bcm_common_sat_endpoint_get(
    int unit,
    bcm_sat_endpoint_t endpoint,
    uint32 flags,
    bcm_sat_endpoint_info_t *endpoint_info)
{
    int rv = BCM_E_UNAVAIL;


    return rv;
}


int
bcm_common_sat_endpoint_traverse(
    int unit,
    uint32 flags,
    bcm_sat_endpoint_traverse_cb cb,
    void *user_data)
{
    int rv = BCM_E_UNAVAIL;


    return rv;
}


