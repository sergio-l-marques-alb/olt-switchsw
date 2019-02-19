/*
 * $Id: cmu.h,v 1.8.6.1 Broadcom SDK $
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
 * File:    cmu.h
 * Purpose: Caladan3 CMU defines
 * Requires:
 */

#ifndef _SBX_CALADN3_CMU_H_
#define _SBX_CALADN3_CMU_H_

#define SOC_SBX_CALADAN3_CMU_NUM_SEGMENT                 (32)
#define SOC_SBX_CALADAN3_CMU_NUM_OCM_PORT                (2)

#define SOC_SBX_CALADAN3_CMU_RING_THREAD_DEFAULT_PRI     (60)

#define _SOC_CALADAN3_CMU_CMICM_CMC     (0)
#define _SOC_CALADAN3_CMU_CMICM_CH      (0)

typedef enum soc_sbx_caladan3_cmu_segment_type_e_s {
    SOC_SBX_CALADAN3_CMU_SEGMENT_TYPE_TURBO_64B = 0,
    SOC_SBX_CALADAN3_CMU_SEGMENT_TYPE_TURBO_32B,
    SOC_SBX_CALADAN3_CMU_SEGMENT_TYPE_SIMPLE_64B,
    SOC_SBX_CALADAN3_CMU_SEGMENT_TYPE_SIMPLE_32B,
    SOC_SBX_CALADAN3_CMU_SEGMENT_TYPE_UNUSED,
    SOC_SBX_CALADAN3_CMU_SEGMENT_TYPE_RANGE,
    /* leave as last */
    SOC_SBX_CALADAN3_CMU_SEGMENT_TYPE_MAX
} soc_sbx_caladan3_cmu_segment_type_e_t;

#define SOC_SBX_CALADAN3_CMU_SEGMENT_TYPE_VALID(type) \
    ((((type)>=SOC_SBX_CALADAN3_CMU_SEGMENT_TYPE_TURBO_64B)  &&		\
      ((type)<=SOC_SBX_CALADAN3_CMU_SEGMENT_TYPE_SIMPLE_32B)) ||	\
     ((type) ==SOC_SBX_CALADAN3_CMU_SEGMENT_TYPE_RANGE))

typedef struct soc_sbx_caladan3_cmu_segment_config_s {
    int8     bEnabled;
    int8     bBGEjectEnabled;
    int8     bSTACEEnabled;
    int8     nSegmentType;
    int8     nSegmentPort;
    int32    nSegmentOcmBase;
    int32    nSegmentOcmSize;
    int32    nSegmentLimit;
    int64   *pSegmentPciBase;
} soc_sbx_caladan3_cmu_segment_config_t;

typedef struct soc_sbx_caladan3_cmu_ring_config_s {
    int32           nRingEntries;
    int32           nRingThresh;
    int32          *pRingPciBase;
    int32          *pRingPciRead;
    /* ring processing thread control */
    int             thread_pri;
    char            name[16];
    uint32          flags;
    sal_thread_t    pid;
    VOL sal_usecs_t interval;
    sal_sem_t       trigger;
} soc_sbx_caladan3_cmu_ring_config_t;

typedef struct soc_sbx_caladan3_cmu_config_s {
    soc_sbx_caladan3_cmu_segment_config_t *segments;
    soc_sbx_caladan3_cmu_ring_config_t ring;
    sal_mutex_t nFlushLock;
    sal_mutex_t nRingLock;
    uint32      uBGEjectRate;     /* in HZ */
    uint32      uManualEjectRate; /* in HZ */
    uint32      uLFSRseed;        /* Seed */
    uint8       bDualEjection;    /* dual ejection */
    volatile uint8      bFlushing;
    uint32      uCMUWatermark;
    uint8       bDriverInit;
    /* size of OCM memory in bytes
     * Only thing requires config by user, 
     */
    uint32      uCMUOcmSize[SOC_SBX_CALADAN3_CMU_NUM_OCM_PORT];
} soc_sbx_caladan3_cmu_config_t;


#define FLUSH_LOCK(unit)   sal_mutex_take(SOC_SBX_CFG_CALADAN3(unit)->cmu_cfg.nFlushLock, sal_mutex_FOREVER)
#define FLUSH_UNLOCK(unit) sal_mutex_give(SOC_SBX_CFG_CALADAN3(unit)->cmu_cfg.nFlushLock);

extern int
soc_sbx_caladan3_cmu_hw_init(int unit);

extern int
soc_sbx_caladan3_cmu_driver_init(int unit);

extern int
soc_sbx_caladan3_cmu_driver_uninit(int unit);

extern int
soc_sbx_caladan3_cmu_ocm_memory_size_set(int unit,
					 int32 port,
					 uint32 size);

extern int
soc_sbx_caladan3_cmu_segment_set(int unit,
				 int segment,
				 soc_sbx_caladan3_cmu_segment_config_t *config);

extern int
soc_sbx_caladan3_cmu_counter_group_register(int unit,
					    uint32 *segment,
					    uint32 num_counters,
					    uint32 port,
					    soc_sbx_caladan3_cmu_segment_type_e_t type);

extern int
soc_sbx_caladan3_cmu_counter_group_unregister(int unit,
					      uint32 segment);

extern int
soc_sbx_caladan3_cmu_counter_read(int unit,
				  uint32 segment,
				  uint32 start,
				  uint32 size,
				  uint64 *data,
				  uint8  sync,
				  uint8  clear);

extern int
soc_sbx_caladan3_cmu_segment_clear(int unit,
				   uint32 segment);

extern int
soc_sbx_caladan3_cmu_ring_process_thread_start(int unit,
					       uint32 flags,
					       sal_usecs_t interval);

extern int
soc_sbx_caladan3_cmu_ring_process_thread_stop(int unit);

extern int
soc_sbx_caladan3_cmu_segment_verify(int unit);

extern int
soc_sbx_caladan3_cmu_segment_background_flush_enable(int unit,
						     uint32 segment,
						     uint8 enable);

extern int
soc_sbx_caladan3_cmu_segment_background_flush_status(int unit,
						     uint32 *segment,
						     uint32 *counter);

extern int 
soc_sbx_caladan3_cmu_segment_flush_all(int unit,
                                       uint32 segment);
extern int 
soc_sbx_caladan3_cmu_segment_manual_flush(int unit,
					  uint32 segment,
					  uint32 start,
					  uint32 size);

extern int
soc_sbx_caladan3_cmu_segment_manual_flush_status(int unit,
						 uint32 *counter);

extern int soc_sbx_caladan3_cmu_init_final(int unit);
#endif /* _SBX_CALADN3_CMU_H_ */
