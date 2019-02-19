/*
 * $Id: wb_db_cosq.h,v 1.14 Broadcom SDK $
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
 * File:        wb_db_cosq.h
 * Purpose:     WarmBoot - Level 2 support (COSQ Module)
 */

#ifndef _BCM_DPP_WB_DB_COSQ_H_
#define _BCM_DPP_WB_DB_COSQ_H_


/*
 * WarmBoot persistence storage  data structures. To be generic data structure declaration
 * is seperate from s/w data structures. This provides for the following generic case
 *   - Not all data fields of s/w data structures need to be stored in peristence storage
 *   - Persistence storage (for optimization) could pack the variuos fields.
 *     NOTE: Currently no packing of instance is done in a byte. This is because the
 *           the storage footprint is not large. If requirements change this optimization
 *           can be done.
 * Warmboot framework also maintains additional memory footprint. s/w footprint and warmboot
 * footprint are different for the reasons outlined above. Ideally warmbot framework should
 * not have such a large memory footprint. This optimization should be considered if memory
 * footprint needs to be reduced.
 *  
 * In general persistance data structures declaration is common across different generations.
 * This general approach is taken where ever the foot print impact is not large on devices
 * that have less scale/instances.
 *
 * h/w and tools dependency.
 * Persistence storage is accessed by only one system. Thus endian dependencies are not there.
 * s/w data structures are declared without the "pack' attribute. Warmboot persistence
 * data structures are declared with 'pack" attribute to reduce footprint. This requires
 * that loading and storing data to those locations be done accounting different h/w (CPUs).
 * Appropriate macros are declared in this file to handle loading and storing data (uint16/uint32)
 * at non-native boundaries.
 * Tools can change across releases and thus structures will not be declared with bitwise
 * definitions.
 *
 * Exception to endian dependencies
 * It is possible that for debugging purposes the persistance
 * storage my be loaded on an evaluation system / simulator. That will bring in endian
 * dependiencies. This will require the persistence storage format to indicate the endianness
 * of data stored and compare it with current system endianness. Alternative is to have macros
 * (compile time options). This debug feature is a very low priority and not currently implemented.
 *
 * Currently there is a single handle for COSQ module.
 * In future (to handle multiple versions) if required seperate handle for each data
 * structure can be allocated. This is facilated by "_sequence_" field of
 * "SOC_SCACHE_HANDLE_SET()" macro.
 */


#include <bcm_int/dpp/cosq.h>

#if defined(BCM_WARM_BOOT_SUPPORT)

#define BCM_DPP_WB_COSQ_VERSION_1_0            SOC_SCACHE_VERSION(1,0)
#define BCM_DPP_WB_COSQ_CURRENT_VERSION        BCM_DPP_WB_COSQ_VERSION_1_0


#define BCM_DPP_WB_COSQ_INFO(unit)             (_dpp_wb_cosq_info_p[unit])
#define BCM_DPP_WB_COSQ_INFO_INIT(unit)        ( (_dpp_wb_cosq_info_p[unit] != NULL) &&          \
                                                 (_dpp_wb_cosq_info_p[unit]->init_done == TRUE) )

/*
 * Following are based on maximum resources.
 * Declared as macros if later an optimization on number of resources is made via SOC properties
 */
#define BCM_DPP_WB_COSQ_MAX_VOQ_GROUPS(unit)                                          \
                        ((DPP_DEVICE_PETRA_COSQ_MAX_QUEUE(unit) + 1) / 4)
#define BCM_DPP_WB_COSQ_MAX_CONN_GROUPS(unit)                                         \
                        ((DPP_DEVICE_PETRA_COSQ_MAX_FLOW(unit) + 1) / 4)
#define BCM_DPP_WB_COSQ_MAX_SE(unit)                                                  \
                        (SOC_DPP_CONFIG(unit)->tm.max_ses)
#define BCM_DPP_WB_COSQ_MAX_FLOW(unit)                                                \
                        (DPP_DEVICE_PETRA_COSQ_MAX_FLOW(unit) + 1)

#define BCM_DPP_WB_DEV_COSQ_DIRTY_BIT_SET(unit)                                       \
                        SOC_CONTROL_LOCK(unit);                                       \
                        SOC_CONTROL(unit)->scache_dirty = 1;                          \
                        BCM_DPP_WB_COSQ_INFO(unit)->is_dirty = 1;                     \
                        SOC_CONTROL_UNLOCK(unit);

#define BCM_DPP_WB_COSQ_DIRTY_BIT_CLEAR(unit)                                         \
                        SOC_CONTROL_LOCK(unit);                                       \
                        BCM_DPP_WB_COSQ_INFO(unit)->is_dirty = 0;                     \
                        SOC_CONTROL_UNLOCK(unit);

#define BCM_DPP_WB_COSQ_IS_DIRTY(unit)                                                \
                        (BCM_DPP_WB_COSQ_INFO(unit)->is_dirty == 1)



#define BCM_DPP_WB_LOAD16(ptr)                                                        \
               _bcm_dpp_wb_load16(ptr)
#define BCM_DPP_WB_LOAD32(ptr)                                                        \
               _bcm_dpp_wb_load32(ptr)

#define BCM_DPP_WB_STORE16(value, ptr)                                                \
               _bcm_dpp_wb_store16(value, ptr);
#define BCM_DPP_WB_STORE32(value, ptr)                                                \
               _bcm_dpp_wb_store32(value, ptr);



/*
 * runtime information
 */
typedef struct bcm_dpp_wb_cosq_info_s {
    int                      init_done;
    bcm_dpp_cosq_config_t   *cosq_config;

    int                      is_dirty;

    uint16                   version;
    uint8                   *scache_ptr;
    int                      size;

    uint32                   max_voqs;
    uint32                   voq_off;

    uint32                   max_conns;
    uint32                   conn_off;

    uint32                   max_ses;
    uint32                   se_off;

    uint32                   max_flows;
    uint32                   flow_off;

    uint32                   max_e2e_fc_data;
    uint32                   e2e_fc_data_off;

    uint32                   max_class_data;
    uint32                   class_data_off;

    uint32                   max_ingr_rate_class_data;
    uint32                   ingr_rate_class_data_off;

    uint32                   max_vsq_rate_class_data;
    uint32                   vsq_rate_class_data_off;

    uint32                   max_ingr_discount_class_data;
    uint32                   ingr_discount_class_data_off;


    uint32                   ipf_data_off;

    uint32                   gbl_data_off;
    
    uint32                   rx_pfc_map_info_off;
} bcm_dpp_wb_cosq_info_t;

/*
 * VoQ data structure
 * NOTE
 *   - if required optimization of packing multiple instances in a byte can be done.
 */
typedef struct bcm_dpp_wb_cosq_voq_config_s {
    uint8    num_cos;
}  __ATTRIBUTE_PACKED__ bcm_dpp_wb_cosq_voq_config_t;

/*
 * Connector data structure
 * NOTE
 *   - if required optimization of packing multiple instances in a byte can be done.
 */
typedef struct bcm_dpp_wb_cosq_connector_config_s { /* Connector Group */
    uint8    num_cos;
#ifdef BCM_DPP_COSQ_CONN_NO_MODID_OPTIMIZATION
    uint8    src_modid;
#endif /* BCM_DPP_COSQ_CONN_NO_MODID_OPTIMIZATION */
} __ATTRIBUTE_PACKED__ bcm_dpp_wb_cosq_connector_config_t;

/*
 * se data structure
 * NOTE
 *   - If required optimization to pack in 3 bytes can be done.
 *   - Another optimzation is to have different data structures for different generation
 *     of devices
 */
typedef struct bcm_dpp_wb_cosq_se_config_s { /* Scheduler */
    uint32   ref_cnt;
} __ATTRIBUTE_PACKED__ bcm_dpp_wb_cosq_se_config_t;

/*
 * flow data structure
 */
typedef struct bcm_dpp_wb_cosq_flow_config_s { /* flows - Connectors/SEs */
    int16    weight;
             /* 0 => implies no weight, */
             /* +ve value => normal meaning */
             /* -ve value => attach sub-point */
    int16     mode; /* mode saved from the sched_set of the SE */
} __ATTRIBUTE_PACKED__ bcm_dpp_wb_cosq_flow_config_t;

/*
 * E2E Flow Control Template
 * NOTE: As these resources are limited no packing is currently done
 */
typedef struct bcm_dpp_wb_cosq_e2e_fc_data_s {
    uint32      ref_cnt;
} bcm_dpp_wb_cosq_e2e_fc_data_t;

/*
 * Class Template
 * NOTE: As these resources are limited no packing is currently done
 */
typedef struct bcm_dpp_wb_cosq_class_data_s {
    uint32      ref_cnt;
} bcm_dpp_wb_cosq_class_data_t;


typedef struct bcm_dpp_wb_cosq_egr_queue_map_data_s {
    uint32                 ref_cnt;
                               /* reference count */
    SOC_TMC_EGR_Q_PRIORITY     queue_mapping[DPP_DEVICE_COSQ_EGR_NOF_Q_PRIO_MAPPING_TYPES][DPP_DEVICE_COSQ_ING_NOF_TC][DPP_DEVICE_COSQ_ING_NOF_DP];
} bcm_dpp_wb_cosq_egr_queue_map_data_t;

/*
 * Ingress Rate Class template
 * NOTE: As these resources are limited no packing is currently done
 */
typedef struct bcm_dpp_wb_cosq_ingr_rate_class_data_s {
    /* reference count */
    uint32                 ref_cnt;
    /* settings for queue WRED*/
    bcm_dpp_cosq_ingress_rate_class_info_t ingress_rate_class_info;

} bcm_dpp_wb_cosq_ingr_rate_class_data_t;

/*
 * VSQ Rate Class template
 * NOTE: As these resources are limited no packing is currently done
 */
typedef struct bcm_dpp_wb_cosq_vsq_rate_class_data_s {
    uint32                     ref_cnt;
                               /* reference count */
    uint8                      wred_enable[DPP_DEVICE_COSQ_VSQ_NOF_CATEGORY_MAX][DPP_DEVICE_COSQ_ING_NOF_DP];
                                   /* WRED Settings Enable WRED algorithm test */
    uint32                     wred_min_avrg_th[DPP_DEVICE_COSQ_VSQ_NOF_CATEGORY_MAX][DPP_DEVICE_COSQ_ING_NOF_DP];
                                   /* WRED Min queue average size */
    uint32                     wred_max_avrg_th[DPP_DEVICE_COSQ_VSQ_NOF_CATEGORY_MAX][DPP_DEVICE_COSQ_ING_NOF_DP];
                                   /* WRED Max queue average size */
    uint8                      wred_ignore_packet_size[DPP_DEVICE_COSQ_VSQ_NOF_CATEGORY_MAX][DPP_DEVICE_COSQ_ING_NOF_DP];
                                   /* WRED ignore packet size */
    uint32                     wred_max_probability[DPP_DEVICE_COSQ_VSQ_NOF_CATEGORY_MAX][DPP_DEVICE_COSQ_ING_NOF_DP];
                                   /* WRED Max probability */
    uint32                     queue_max_size[DPP_DEVICE_COSQ_VSQ_NOF_CATEGORY_MAX][DPP_DEVICE_COSQ_ING_NOF_DP];
                                   /* Tail drop settings. Maximal queue size per DP */
    uint32                     queue_max_size_bds[DPP_DEVICE_COSQ_VSQ_NOF_CATEGORY_MAX][DPP_DEVICE_COSQ_ING_NOF_DP];
                                   /* Maximal queue size in BDs threshold */
    SOC_TMC_THRESH_WITH_HYST_INFO  fc_queue_size[DPP_DEVICE_COSQ_VSQ_NOF_CATEGORY_MAX];
    SOC_TMC_THRESH_WITH_HYST_INFO  fc_bd_size[DPP_DEVICE_COSQ_VSQ_NOF_CATEGORY_MAX];
                                   /* FC settings */
} bcm_dpp_wb_cosq_vsq_rate_class_data_t;

/*
 * Ingress Discount Class Template
 * NOTE: As these resources are limited no packing is currently done
 */
typedef struct bcm_dpp_wb_cosq_ingr_discount_class_data_s {
    uint32                     ref_cnt;
                                   /* reference count */
    SOC_TMC_ITM_CR_DISCOUNT_INFO   discount_class;
} bcm_dpp_wb_cosq_ingr_discount_class_data_t;


/*
 * TC Mapping Template
 * NOTE: As these resources are limited no packing is currently done
 */
typedef struct bcm_dpp_wb_cosq_tc_map_data_s {
    uint32                 ref_cnt;
                               /* reference count */
    SOC_TMC_ITM_TC_MAPPING     tc_map;
} bcm_dpp_wb_cosq_tc_map_data_t;



typedef struct bcm_dpp_wb_cosq_ipf_data_s {
    SOC_TMC_SCH_FLOW_IPF_CONFIG_MODE ipf_mode; /* inverse/propotional WFQ indepenedent per flow */
} bcm_dpp_wb_cosq_ipf_data_t;

/*
 * Miscellaneous Global Configuration
 * NOTE: As these resources are limited no packing is currently done
 */
typedef struct bcm_dpp_wb_cosq_gbl_data_s {
    int32                  vsq_category_mode; /* vsq category mode */

    int32                  ucast_qid_start;   /* unicast QID start */
    int32                  ucast_qid_end;     /* unicast QID end */
                                              /* NOTE: do not change the order of this field */
} bcm_dpp_wb_cosq_gbl_data_t;



/*
 * Functions
 */
extern int
_bcm_dpp_wb_cosq_state_init(int unit, bcm_dpp_cosq_config_t *cosq_config);

extern int
_bcm_dpp_wb_cosq_state_deinit(int unit);

extern int
_bcm_dpp_wb_cosq_sync(int unit);

extern int
_bcm_dpp_wb_cosq_update_voq_state(int unit, int base_qid);

extern int
_bcm_dpp_wb_cosq_update_conn_all_state(int unit, int base_conn, int is_non_contiguous, int is_composite, int src_modid);

extern int
_bcm_dpp_wb_cosq_update_se_all_state(int unit, int se_id, int is_composite, int is_dual);

extern int
_bcm_dpp_wb_cosq_update_se_state(int unit, int se_id, uint32 ref_cnt);

extern int
_bcm_dpp_wb_cosq_update_flow_state(int unit, int flow_id);


extern int
_bcm_dpp_wb_cosq_update_e2e_fc_data(int unit, int flags, int id1, int id2);

extern int
_bcm_dpp_wb_cosq_update_class_data(int unit, int flags, int id1, int id2);

/* extern int */
/* _bcm_dpp_wb_cosq_update_egr_thres_data(int unit, int flags, int id1, int id2); */

extern int
_bcm_dpp_wb_cosq_update_ingr_rate_class_data(int unit, int flags, int id1, int id2);

extern int
_bcm_dpp_wb_cosq_update_vsq_rate_class_data(int unit, int flags, int id1, int id2);

extern int
_bcm_dpp_wb_cosq_update_ingr_discount_class_data(int unit, int flags, int id1, int id2);

/* extern int */
/* _bcm_dpp_wb_cosq_update_egr_discount_class_data(int unit, int flags, int id1, int id2); */

extern int
_bcm_dpp_wb_cosq_update_tc_map_data(int unit, int flags, int id1, int id2);


extern int
_bcm_dpp_wb_cosq_update_vsq_data(int unit, int category_mode);

extern int
_bcm_dpp_wb_cosq_update_ucast_data(int unit, int qid_start, int qid_end);

extern int
_bcm_dpp_wb_cosq_update_ipf_mode(int unit, SOC_TMC_SCH_FLOW_IPF_CONFIG_MODE ipf_mode);

extern int
_bcm_dpp_wb_cosq_update_pfc_rx_type_data(int unit, bcm_port_t port);

#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * Routines to make code Independent of Host processor alignment constaints
 */
extern uint16
_bcm_dpp_wb_load16(uint8 *ptr);

extern uint32
_bcm_dpp_wb_load32(uint8 *ptr);

extern void
_bcm_dpp_wb_store16(uint16 value, uint8 *ptr);

extern void
_bcm_dpp_wb_store32(uint32 value, uint8 *ptr);

#endif /* _BCM_DPP_WB_DB_COSQ_H_ */
