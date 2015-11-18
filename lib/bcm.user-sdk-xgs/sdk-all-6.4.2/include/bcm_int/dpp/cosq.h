/*
 * $Id: cosq.h,v 1.55 Broadcom SDK $
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
 * File:        cosq.h
 * Purpose:     COSQ internal definitions to the BCM library.
 */

#ifndef   _BCM_INT_DPP_COSQ_H_
#define   _BCM_INT_DPP_COSQ_H_

#include <shared/gport.h>

#include <soc/dpp/drv.h>
#include <soc/dpp/dpp_config_defs.h>

#include <soc/dpp/TMC/tmc_api_egr_queuing.h>
#include <soc/dpp/TMC/tmc_api_general.h>
#include <soc/dpp/TMC/tmc_api_end2end_scheduler.h>

#include <bcm/debug.h>
#include <bcm/fabric.h>


/*        Currently this is not done and thus it is stored in persistent storage.  */
/*        The following definition should be removed once the above is implemented */
/*        This will help in retreiving the sttae from h/w.                         */
#define BCM_DPP_COSQ_CONN_NO_MODID_OPTIMIZATION     1

/*
 * reserved queue flows
 */
#define DPP_DEVICE_COSQ_QUEUE_RESERVED_FLOW_START           SHR_DEVICE_QUEUE_RESERVED_FLOW_START
#define DPP_DEVICE_COSQ_QUEUE_RESERVED_TOTAL_FLOWS          16

#define DPP_DEVICE_PETRA_COSQ_OCB_16BYTES_RESOLUTION       16

#define DPP_DEVICE_COSQ_QUEUE_REGION_UNICAST                3
#define DPP_DEVICE_COSQ_QUEUE_REGION_MULTICAST              4
#define DPP_DEVICE_COSQ_QUEUE_REGION_ISQ                    5

/* Start of Flow regions that are not simple */
#define DPP_DEVICE_PETRA_COSQ_CONFIG_FLOW_REGION_START        25 /* 1 based */
#define DPP_DEVICE_PETRA_COSQ_FLOW_REGION_FIXED_TYPE0_END     24 /* 1 based */
#define DPP_DEVICE_PETRA_COSQ_CONFIG_FLOW_REGIONS             32
#define DPP_DEVICE_PETRA_COSQ_TOTAL_FLOW_REGIONS              56

#define DPP_DEVICE_ARAD_COSQ_CONFIG_FLOW_REGION_START         65 /* 1 based */
#define DPP_DEVICE_ARAD_COSQ_FLOW_REGION_FIXED_TYPE0_END      64 /* 1 based */
#define DPP_DEVICE_ARAD_COSQ_CONFIG_FLOW_REGIONS              64
#define DPP_DEVICE_ARAD_COSQ_TOTAL_FLOW_REGIONS               128

#define DPP_DEVICE_COSQ_CONFIG_FLOW_REGION_START(unit)          (SOC_IS_PETRAB(unit) ? DPP_DEVICE_PETRA_COSQ_CONFIG_FLOW_REGION_START:DPP_DEVICE_ARAD_COSQ_CONFIG_FLOW_REGION_START) /*1 base*/
#define DPP_DEVICE_COSQ_CONFIG_FLOW_REGIONS(unit)               (SOC_IS_PETRAB(unit) ? DPP_DEVICE_PETRA_COSQ_CONFIG_FLOW_REGIONS:DPP_DEVICE_ARAD_COSQ_CONFIG_FLOW_REGIONS) 
#define DPP_DEVICE_COSQ_CONFIG_FLOW_REGIONS_END(unit)           (DPP_DEVICE_COSQ_CONFIG_FLOW_REGION_START(unit) + DPP_DEVICE_COSQ_CONFIG_FLOW_REGIONS(unit) - 1) /*1 base*/ 

#define DPP_DEVICE_COSQ_TOTAL_FLOW_REGIONS(unit)                (SOC_IS_PETRAB(unit) ? DPP_DEVICE_PETRA_COSQ_TOTAL_FLOW_REGIONS:DPP_DEVICE_ARAD_COSQ_TOTAL_FLOW_REGIONS)
#define DPP_DEVICE_COSQ_FLOW_REGION_FIXED_TYPE0_END(unit)       (SOC_IS_PETRAB(unit) ? DPP_DEVICE_PETRA_COSQ_FLOW_REGION_FIXED_TYPE0_END:DPP_DEVICE_ARAD_COSQ_FLOW_REGION_FIXED_TYPE0_END )

#define DPP_DEVICE_COSQ_MAX_REGION_VAL                      127

#define DPP_DEVICE_PETRA_COSQ_CONFIG_QUEUE_REGIONS          32
#define DPP_DEVICE_ARAD_COSQ_CONFIG_QUEUE_REGIONS           96
#define DPP_DEVICE_COSQ_CONFIG_QUEUE_REGIONS(unit)          ( ( SOC_IS_PETRAB(unit) || SOC_IS_ARDON(unit) ) ? DPP_DEVICE_PETRA_COSQ_CONFIG_QUEUE_REGIONS:DPP_DEVICE_ARAD_COSQ_CONFIG_QUEUE_REGIONS)
#define DPP_DEVICE_PETRA_COSQ_MAX_QUEUE(unit)               (SOC_DPP_DEFS_GET(unit, max_queues)-1)
#define DPP_DEVICE_PETRA_COSQ_MAX_FLOW(unit)                (SOC_DPP_CONFIG(unit)->tm.max_connectors-1)



#define DPP_DEVICE_PETRA_COSQ_QUEUE_QUARTET_DIVISOR         4
#define DPP_DEVICE_INGR_QUEUE_PRI_TEMPLATE_MAX              3  /* hi/lo ingress priority queue template ndx */
#define DPP_DEVICE_INGR_QUEUE_PRI_TEMPLATE_REGION_SIZE      64 /* hi/lo queue region size 64 queues */

#define DPP_DEVICE_COSQ_HR_MASK            (BCM_COSQ_GPORT_SCHEDULER_HR_DUAL_WFQ |               \
                                            BCM_COSQ_GPORT_SCHEDULER_HR_ENHANCED |               \
                                            BCM_COSQ_GPORT_SCHEDULER_HR_SINGLE_WFQ)

#define DPP_DEVICE_COSQ_CL_MASK            (BCM_COSQ_GPORT_SCHEDULER_CLASS_MODE1_4SP |           \
                                            BCM_COSQ_GPORT_SCHEDULER_CLASS_MODE2_3SP_WFQ |       \
                                            BCM_COSQ_GPORT_SCHEDULER_CLASS_MODE3_WFQ_2SP |       \
                                            BCM_COSQ_GPORT_SCHEDULER_CLASS_MODE4_2SP_WFQ |       \
                                            BCM_COSQ_GPORT_SCHEDULER_CLASS_MODE5_1SP_WFQ)

#define DPP_DEVICE_COSQ_CL_ENHANCED_MASK   (BCM_COSQ_GPORT_SCHEDULER_CLASS_ENHANCED_LOWPRI_FQ |  \
                                            BCM_COSQ_GPORT_SCHEDULER_CLASS_ENHANCED_HIGHPRI_FQ)

#define DPP_DEVICE_COSQ_CL_WFQ_MASK        (BCM_COSQ_GPORT_SCHEDULER_CLASS_WFQ_MODE_DISCREET |   \
                                            BCM_COSQ_GPORT_SCHEDULER_CLASS_WFQ_MODE_CLASS |      \
                                            BCM_COSQ_GPORT_SCHEDULER_CLASS_WFQ_MODE_INDEPENDENT| \
                                            BCM_COSQ_GPORT_SCHEDULER_CLASS_WFQ_MODE_INDEPENDENT_PROPORTIONAL)

#define DPP_DEVICE_COSQ_DUAL_SHAPER_MASK   (BCM_COSQ_GPORT_SCHEDULER_DUAL_SHAPER)

#define DPP_DEVICE_COSQ_FQ_MASK            (BCM_COSQ_GPORT_SCHEDULER_FQ)

#define DPP_DEVICE_COSQ_MAX_COS                             8
#define DPP_DEVICE_COSQ_COS_ALLOC_SZ                        4

/* currently used for SE resources */
#define DPP_DEVICE_COSQ_RESOURCE_ALLOC_NREF_VALUE           1

/* number of profiles handled for egress queue mapping */
#define DPP_DEVICE_COSQ_EGRESS_QUEUE_MAP_NOF_PROFILES   (4)
/* number of profiles handled for egress thresholds & fcs  */

#define  DPP_DEVICE_COSQ_EGR_NOF_Q_PRIO(unit)           (SOC_DPP_CONFIG(unit)->tm.max_egr_q_prio)
#define  DPP_DEVICE_COSQ_EGR_NOF_Q_PRIO_MAX             (SOC_TMC_EGR_NOF_Q_PRIO_ARAD)
#define  DPP_DEVICE_COSQ_EGR_NOF_DP                     (4)
#define  DPP_DEVICE_COSQ_EGR_NOF_TC                     (8)
#define  DPP_DEVICE_COSQ_EGR_NOF_Q_PRIO_MAPPING_TYPES   (SOC_TMC_EGR_NOF_Q_PRIO_MAPPING_TYPES)
#define  DPP_DEVICE_COSQ_ING_NOF_TC                     (8)
#define  DPP_DEVICE_COSQ_ING_NOF_DP                     (4)


#define DPP_DEVICE_COSQ_VSQ_NOF_CATEGORY(unit)          (SOC_DPP_CONFIG(unit)->tm.nof_vsq_category)
#define DPP_DEVICE_COSQ_VSQ_NOF_CATEGORY_MAX            (SOC_TMC_ITM_VSQ_GROUP_LAST_ARAD)

#define DPP_DEVICE_COSQ_QUEUE_NOF_CATEGORY              (4)

#define DPP_DEVICE_COSQ_CL_SE_ID_MIN_ARAD           SOC_TMC_CL_SE_ID_MIN_ARAD
#define DPP_DEVICE_COSQ_CL_SE_ID_MAX_ARAD           SOC_TMC_CL_SE_ID_MAX_ARAD
#define DPP_DEVICE_COSQ_FQ_SE_ID_MIN_ARAD           SOC_TMC_FQ_SE_ID_MIN_ARAD
#define DPP_DEVICE_COSQ_FQ_SE_ID_MAX_ARAD           SOC_TMC_FQ_SE_ID_MAX_ARAD
#define DPP_DEVICE_COSQ_HR_SE_ID_MIN_ARAD           SOC_TMC_HR_SE_ID_MIN_ARAD                                           
#define DPP_DEVICE_COSQ_HR_SE_ID_MAX_ARAD           SOC_TMC_HR_SE_ID_MAX_ARAD                                           
#define DPP_DEVICE_COSQ_SCH_FLOW_ID_INVALID_ARAD    SOC_TMC_SCH_FLOW_ID_INVALID_ARAD

#define DPP_DEVICE_COSQ_NBR_QUEUES_IN_QUARTET           4
#define DPP_DEVICE_COSQ_NBR_QUEUES_IN_REGION            1024
#define BCM_DEVICE_PETRA_COSQ_QUEUE_QUAD_GET(qid)       (qid/DPP_DEVICE_COSQ_NBR_QUEUES_IN_QUARTET)
#define BCM_DEVICE_PETRA_COSQ_QUEUE_REGION_GET(qid)     (qid/DPP_DEVICE_COSQ_NBR_QUEUES_IN_REGION)
#define BCM_DEVICE_PETRA_COSQ_FLOW_REGION_GET(flow_id)  (flow_id/DPP_DEVICE_COSQ_NBR_QUEUES_IN_REGION)

#define BCM_DEVICE_PETRA_COSQ_DWEIGHT_MAX_INDEX         4

/* start of interface range in gport port definition */
#define BCM_DPP_PORT_INTERFACE_START                  SOC_DPP_PORT_INTERFACE_START
#define BCM_DPP_PORT_INTERFACE_END                    SOC_DPP_PORT_INTERFACE_END
#define BCM_DPP_PORT_INTERNAL_OLP(core)               SOC_DPP_PORT_INTERNAL_OLP(core)
#define BCM_DPP_PORT_INTERNAL_ERP(core)               SOC_DPP_PORT_INTERNAL_ERP(core)
#define BCM_DPP_PORT_INTERNAL_OAMP(core)              SOC_DPP_PORT_INTERNAL_OAMP(core)

#define BCM_DPP_PORT_IS_INTERFACE(port) \
  (((port) >= BCM_DPP_PORT_INTERFACE_START) && ((port) <= BCM_DPP_PORT_INTERFACE_END))


/* default egr tc/dp map profile */
#define DPP_COSQ_PB_EGR_QUEUE_DEFAULT_PROFILE_MAP       (0)

#define DPP_COSQ_EG_DISCOUNT_DEFAULT_VALUE          (0)

#define BCM_INT_DPP_COSQ_GPORT_COMPOSITE_SF2_TYPE_GET(type, gport_sf2) \
{ \
    if (_SHR_COSQ_GPORT_IS_COMPOSITE_SF2(gport_sf2)) { \
      if ((_SHR_COSQ_GPORT_RESERVED_TYPE_GET(gport_sf2)) == _SHR_COSQ_GPORT_RESERVED_VOQ_CONNECTOR) { \
          (type) = BCM_COSQ_GPORT_TYPE_VOQ_CONNECTOR; \
      } else { \
          (type) = BCM_GPORT_TYPE_SCHEDULER; \
      } \
    } \
}

#define BCM_DPP_SCHEDULER_ONLY_CORE_SET(gport, core) _SHR_GPORT_SCHEDULER_ONLY_CORE_SET(gport, core)
/*
 * FMQ Definitions
 */
#define DPP_DEVICE_FMQ_CLASS_PORTS                  4
#define DPP_DEVICE_COSQ_FMQ_NON_ENHANCED_QID_MIN    0
#define DPP_DEVICE_COSQ_FMQ_NON_ENHANCED_QID_MAX    3


/* Scheduler attributes per device */
#define DPP_COSQ_SCH_MAX_FLOW_ID(unit) (SOC_DPP_CONFIG(unit)->tm.max_connectors-1)
#define DPP_COSQ_SCH_MAX_PORT_ID(unit) ((SOC_IS_PETRAB(unit) ? SOC_TMC_SCH_MAX_PORT_ID_PETRA : SOC_TMC_SCH_MAX_PORT_ID_ARAD))
#define DPP_COSQ_SCH_PORT_ID_INVALID(unit) (SOC_DPP_CONFIG(unit)->tm.invalid_port_id_num)
#define DPP_COSQ_SCH_SE_ID_INVALID(unit) (SOC_DPP_CONFIG(unit)->tm.invalid_se_id_num)
#define DPP_COSQ_SCH_FLOW_ID_INVALID(unit) (SOC_DPP_CONFIG(unit)->tm.invalid_voq_connector_id_num)

/* Cosq gport fabric pipes WFQ whight check */
#define DPP_COSQ_GPORT_FABRIC_PIPE_WFQ_WHIGHT_MAX       (127)
#define DPP_COSQ_GPORT_FABRIC_PIPE_WFQ_WHIGHT_IS_VALID(weight)\
        ((weight > 0) && (weight <= DPP_COSQ_GPORT_FABRIC_PIPE_WFQ_WHIGHT_MAX))

/* iterate over all cores */
#define BCM_DPP_CORES_ITER(core_id, index) SOC_DPP_CORES_ITER(core_id, index)

typedef enum dpp_device_cosq_region_type_e
{
    DPP_DEVICE_COSQ_REGION_TYPE0 = 0,/* connector flows */
    DPP_DEVICE_COSQ_REGION_TYPE1 = 1,/* {connector, SE} flows */
    DPP_DEVICE_COSQ_REGION_TYPE2 = 2, /* {connector, SE} flows not odd_even*/
    DPP_DEVICE_COSQ_REGION_TYPE3 = 3, /* {connector, SE} flows not odd_even - HR */
    DPP_DEVICE_COSQ_REGION_TYPE_LAST
} dpp_device_cosq_region_type_t;


typedef enum bcm_dpp_cl_scheduler_profile_e {
    bcm_dpp_cosq_cl_cid_mode3_ind,
    bcm_dpp_cosq_cl_cid_mode3_ind_ehp_fq,
    bcm_dpp_cosq_cl_cid_mode3_ind_elp_fq,
    bcm_dpp_cosq_cl_cid_mode4_ind,
    bcm_dpp_cosq_cl_cid_mode4_ind_ehp_fq,
    bcm_dpp_cosq_cl_cid_mode4_ind_elp_fq,
    bcm_dpp_cosq_cl_cid_mode5_ind,
    bcm_dpp_cosq_cl_cid_mode5_ind_ehp_fq,
    bcm_dpp_cosq_cl_cid_mode5_ind_elp_fq,

    bcm_dpp_cosq_cl_cid_mode1,
    bcm_dpp_cosq_cl_cid_mode1_ehp_fq,
    bcm_dpp_cosq_cl_cid_mode1_elp_fq,

    
    /* template management                                                         */
    bcm_dpp_cosq_cl_cid_mode3_des,
    bcm_dpp_cosq_cl_cid_mode3_des_ehp_fq,
    bcm_dpp_cosq_cl_cid_mode3_des_elp_fq,
    bcm_dpp_cosq_cl_cid_mode4_des,
    bcm_dpp_cosq_cl_cid_mode4_des_ehp_fq,
    bcm_dpp_cosq_cl_cid_mode4_des_elp_fq,
    bcm_dpp_cosq_cl_cid_mode5_des,
    bcm_dpp_cosq_cl_cid_mode5_des_ehp_fq,
    bcm_dpp_cosq_cl_cid_mode5_des_elp_fq,
    bcm_dpp_cosq_cl_cid_mode2_des,
    bcm_dpp_cosq_cl_cid_mode2_des_ehp_fq,
    bcm_dpp_cosq_cl_cid_mode2_des_elp_fq,
    bcm_dpp_cosq_cl_cid_mode3_cls,
    bcm_dpp_cosq_cl_cid_mode3_cls_ehp_fq,
    bcm_dpp_cosq_cl_cid_mode3_cls_elp_fq,
    bcm_dpp_cosq_cl_cid_mode4_cls,
    bcm_dpp_cosq_cl_cid_mode4_cls_ehp_fq,
    bcm_dpp_cosq_cl_cid_mode4_cls_elp_fq,
    bcm_dpp_cosq_cl_cid_mode5_cls,
    bcm_dpp_cosq_cl_cid_mode5_cls_ehp_fq,
    bcm_dpp_cosq_cl_cid_mode5_cls_elp_fq,
    bcm_dpp_cosq_cl_cid_mode2_cls,
    bcm_dpp_cosq_cl_cid_mode2_cls_ehp_fq,
    bcm_dpp_cosq_cl_cid_mode2_cls_elp_fq,

    bcm_dpp_cosq_cl_cid_last /* last one. This and following one's are managed profiles. Ref note */
} bcm_dpp_cosq_cl_scheduler_profiles_t;

#define DPP_DEVICE_COSQ_CL_CLASS_TABLE_INIT(class_tbl, cid, cl_mode, emode, weight_type, weights) \
    class_tbl->class_types[cid].id = cid;                                                     \
    class_tbl->class_types[cid].mode = cl_mode;                                               \
    class_tbl->class_types[cid].enhanced_mode = emode;                                        \
    class_tbl->class_types[cid].weight_mode = weight_type;                                    \
    if ( (weight_type == SOC_TMC_SCH_CL_WEIGHTS_MODE_INDEPENDENT_PER_FLOW) ||                     \
         (weight_type == SOC_TMC_SCH_CL_CLASS_WEIGHTS_MODE_LAST) ) {                              \
        class_tbl->class_types[cid].weight[0] = 0;                                            \
        class_tbl->class_types[cid].weight[1] = 0;                                            \
        class_tbl->class_types[cid].weight[2] = 0;                                            \
        class_tbl->class_types[cid].weight[3] = 0;                                            \
    }                                                                                         \
    else {                                                                                    \
        class_tbl->class_types[cid].weight[0] = *(weights + 0);                               \
        class_tbl->class_types[cid].weight[1] = *(weights + 1);                               \
        class_tbl->class_types[cid].weight[2] = *(weights + 2);                               \
        class_tbl->class_types[cid].weight[3] = *(weights + 3);                               \
    }

#define DPP_DEVICE_COSQ_CLASS_INFO_INIT(info, class_type, mode_weight, weight_max, dclass_max) \
    info->type = class_type;                                                                   \
    info->weight_mode = mode_weight;                                                           \
    info->max_weight = weight_max;                                                             \
    info->max_discrete_class = dclass_max;

#define DPP_DEVICE_COSQ_CL_CLASS_DATA_INIT(class_data, cl_mode, emode, weight_type, weights)   \
    class_data.mode = cl_mode;                                                                 \
    class_data.enhanced_mode = emode;                                                          \
    class_data.weight_mode = weight_type;                                                      \
    class_data.weights[0] = weights[0];                                                        \
    class_data.weights[1] = weights[1];                                                        \
    class_data.weights[2] = weights[2];                                                        \
    class_data.weights[3] = weights[3];

typedef enum bcm_dpp_cosq_gport_type_info_e {
    dpp_device_cosq_gport_child_element = 0x1,
    dpp_device_cosq_gport_parent_element = 0x2,
    dpp_device_cosq_gport_sched_nocirpir_ds_element = 0x4,
    dpp_device_cosq_gport_connector_element = 0x8
} bcm_dpp_cosq_gport_type_info_t;

typedef enum bcm_dpp_cosq_hdlist_type_e {
    dpp_cosq_hdlist_voq_group = 0,        /* queue group data structures */
    dpp_cosq_hdlist_connector_group,      /* connector group data structure */
    dpp_cosq_hdlist_se,                   /* se data structures */
    dpp_cosq_hdlist_flow,                 /* each flow data structure (applicable to connector/SE resources */
    dpp_cosq_hdlist_invalid
} bcm_dpp_cosq_hdlist_type_t;

/*
 * Data structures
 */

/* enumerations for "flag" parameter in resource data structures */

typedef struct bcm_dpp_cosq_gport_info_s { 
    int                 is_connector;       /* connector */
    int                 is_se;              /* se */
    int                 is_composite;       /* valid for connector and SE */
    int                 is_non_contiguous;  /* valid if is_connector=FALSE */
    int                 is_dual;            /* dual configuation */
    int                 is_sf2;             /* sf2 request */
    int                 flow_id;            /* flow_id corresonding to the gport */
    int                 se_id;              /* valid if is_connector=FALSE */
    int                 actual_flow_id;
    SOC_TMC_SCH_FLOW     *flow_config;
    SOC_TMC_SCH_SE_INFO  *se_config;
} bcm_dpp_cosq_gport_info_t;

typedef struct bcm_dpp_cosq_sched_class_info_s {
    int                 mode;
    int                 enhanced_mode;
    int                 weight_mode;
    int                 weights[SOC_TMC_SCH_MAX_NOF_DISCRETE_WEIGHT_VALS];
} bcm_dpp_cosq_sched_class_data_t;

typedef struct bcm_dpp_cosq_class_info_s {
    int                 type;               /* type */
    int                 weight_mode;        /* weight mode */
    int                 max_weight;         /* max weights if weight mode is independent */
    int                 max_discrete_class; /* max discrete classes if weight mode is discrete */
} bcm_dpp_cosq_class_info_t;

typedef struct {

   bcm_gport_t base_queue;
   int         num_cos;

}bcm_dpp_gport_state;


/*
 * Data structures for various resources (voq, connector, se, flows)
 * data structure fields kept to minimum. State learnt from h/w where possible
 */
/* voq group data structure */
typedef struct bcm_dpp_cosq_voq_config_s { /* Queue group */
    uint8    num_cos;
    uint8    flags;
    uint32   ref_cnt;
} bcm_dpp_cosq_voq_config_t;

/* connector group data structure */
typedef struct bcm_dpp_cosq_connector_config_s { /* Connector Group */
    uint8    num_cos;
    uint8    flags;
#ifdef BCM_DPP_COSQ_CONN_NO_MODID_OPTIMIZATION
    uint8    src_modid;
#endif /* BCM_DPP_COSQ_CONN_NO_MODID_OPTIMIZATION */
    uint32   ref_cnt;
} bcm_dpp_cosq_connector_config_t;

/* se data structure */
typedef struct bcm_dpp_cosq_se_config_s { /* Scheduler */
    uint32   ref_cnt; /* reference DPP_DEVICE_COSQ_RESOURCE_ALLOC_NREF_VALUE definition */
} bcm_dpp_cosq_se_config_t;

/* flow data structure */
typedef struct bcm_dpp_cosq_flow_config_s { /* flows - Connectors/SEs */
    int     weight;
             /* 0 => implies no weight, */
             /* +ve value => normal meaning */
             /* -ve value => attach sub-point */
    int     mode; /* mode saved from the sched_set of the SE */
} bcm_dpp_cosq_flow_config_t;

typedef struct bcm_dpp_cosq_blk_info_s {
    int                               start_element;
    int                               end_element;
    int                               count;           /* redundant information */
    int                               used;            /* used entries, free when none used */
    void                             *data;
    struct bcm_dpp_cosq_blk_info_s   *next_blk;
} bcm_dpp_cosq_blk_info_t;

typedef struct bcm_dpp_cosq_list_hd_s {
    bcm_dpp_cosq_blk_info_t          *blk;
    int                               num_elements_per_blk;
    int                               element_size;
    int                               index_divisor;
    bcm_dpp_cosq_hdlist_type_t        type;
} bcm_dpp_cosq_list_hd_t;

typedef struct bcm_dpp_cosq_rx_cal_s {
    int valid;
    int intf;
    int cal_mode_ndx;
} bcm_dpp_cosq_rx_cal_t;

typedef struct bcm_dpp_cosq_tx_cal_s {
    int valid;
    int intf;
    int cal_mode_ndx;
} bcm_dpp_cosq_tx_cal_t;

typedef struct bcm_dpp_cosq_cal_s {
    bcm_dpp_cosq_rx_cal_t  rx;
    bcm_dpp_cosq_tx_cal_t  tx;
} bcm_dpp_cosq_cal_t;

typedef enum bcm_dpp_cosq_pfc_rx_type_e {
    _BCM_DPP_COSQ_PFC_RX_NONE = 0x0,
    _BCM_DPP_COSQ_PFC_RX_DEFAULT = 0x1,
    _BCM_DPP_COSQ_PFC_RX_PORT_OVER = 0x2,
    _BCM_DPP_COSQ_PFC_RX_INTF_COSQ = 0x3,
    _BCM_DPP_COSQ_PFC_RX_LAST = 0x4
} bcm_dpp_cosq_pfc_rx_type_t;

typedef struct bcm_dpp_cosq_config_s {
    bcm_gport_t                       fmq_class_ports[DPP_DEVICE_FMQ_CLASS_PORTS];
    bcm_gport_t                       isq_port;
    int                               hr_fc_default_template;
    int                               hr_fc_default_template_data;
    int                               default_se;
    int                               default_se_flow;
    int                               default_se_type;
    int                               default_se_class;

    uint32                            voq_flags; /* e.g. sequencing state */
    bcm_dpp_cosq_voq_config_t         voq_config[DPP_DEVICE_COSQ_MAX_COS];

    uint32                            connector_flags; /* e.g. sequencing state */
    bcm_dpp_cosq_connector_config_t   connector_config[DPP_DEVICE_COSQ_MAX_COS];

    uint32                            se_flags; /* e.g. sequencing state */
    bcm_dpp_cosq_se_config_t          se_config;

    bcm_dpp_cosq_list_hd_t            voq_hd;
    bcm_dpp_cosq_list_hd_t            connector_hd;
    bcm_dpp_cosq_list_hd_t            se_hd;
    bcm_dpp_cosq_list_hd_t            flow_hd;

    bcm_dpp_cosq_cal_t                inband_cal[SOC_DPP_MAX_INTERLAKEN_PORTS];
    bcm_dpp_cosq_cal_t                oob_cal[SOC_DPP_MAX_OOB_PORTS];
    bcm_fabric_vsq_category_mode_t    vsq_category_mode;        

    int                               ucast_qid_start;
    int                               ucast_qid_end;

    bcm_dpp_cosq_pfc_rx_type_t        rx_pfc_map_info[SOC_MAX_NUM_PORTS];
} bcm_dpp_cosq_config_t;

typedef struct bcm_dpp_cosq_egress_thresh_key_info_s {

    /* Soc_petra B*/

    /* Schedule (UC) words consumed per priority - Soc_petra-B*/
    uint32                          sch_drop_words_consumed[DPP_DEVICE_COSQ_EGR_NOF_Q_PRIO_MAX];
    /* Unschedule (MC) words consumed per {priority,dp} - Soc_petra-B*/
    uint32                          unsch_drop_words_consumed[DPP_DEVICE_COSQ_EGR_NOF_Q_PRIO_MAX][DPP_DEVICE_COSQ_EGR_NOF_DP];
    /* Schedule (UC) pkts consumed per priority - Soc_petra-B */
    uint32                          sch_drop_pkts_consumed[DPP_DEVICE_COSQ_EGR_NOF_Q_PRIO_MAX];
    /* Unschedule (MC) pkts consumed per {priority,dp} - Soc_petra-B*/
    uint32                          unsch_drop_pkts_consumed[DPP_DEVICE_COSQ_EGR_NOF_Q_PRIO_MAX][DPP_DEVICE_COSQ_EGR_NOF_DP];
    /* Schedule (UC) flow control words consumed per priority - Soc_petra-B*/
    uint32                          sch_fc_words_consumed[DPP_DEVICE_COSQ_EGR_NOF_Q_PRIO_MAX];

    /* ARAD */

    /* Schedule (UC) packet descriptors - ARAD */
    uint32                          sch_drop_packet_descriptors;
    /* Schedule (UC) packet descriptors per tc - ARAD */
    uint32                          sch_drop_packet_descriptors_tc[DPP_DEVICE_COSQ_EGR_NOF_TC];
    /* Unschedule (MC) packet descriptors per - ARAD */
    uint32                          unsch_drop_packet_descriptors;
    /* Unschedule (MC) packet descriptors per {tc,dp} - ARAD*/
    uint32                          unsch_drop_packet_descriptors_tc_dp[DPP_DEVICE_COSQ_EGR_NOF_TC][DPP_DEVICE_COSQ_EGR_NOF_DP];
    /* Schedule (UC) data buffers - ARAD*/
    uint32                          sch_drop_data_buffers;
    /* Schedule (UC) data buffers per tc - ARAD*/
    uint32                          sch_drop_data_buffers_tc[DPP_DEVICE_COSQ_EGR_NOF_TC];
    /* Unschedule (MC) data buffers - ARAD*/
    uint32                          unsch_drop_data_buffers;
    /* Unschedule (MC) data buffers per {tc,dp} - ARAD*/
    uint32                          unsch_drop_data_buffers_tc_dp[DPP_DEVICE_COSQ_EGR_NOF_TC][DPP_DEVICE_COSQ_EGR_NOF_DP];
    /* Schedule (UC) flow control packet descriptors - ARAD*/
    uint32                          sch_fc_packet_descriptors;
    /* Schedule (UC) flow control packet descriptors per tc - ARAD*/
    uint32                          sch_fc_packet_descriptors_tc[DPP_DEVICE_COSQ_EGR_NOF_TC];
    /* Schedule (UC) flow control data buffers - ARAD*/
    uint32                          sch_fc_data_buffers;
    /* Schedule (UC) flow control data buffers per tc - ARAD*/
    uint32                          sch_fc_data_buffers_tc[DPP_DEVICE_COSQ_EGR_NOF_TC];
    /* Unschedule (MC) data buffers available per tc - ARAD*/
    uint32                          unsch_drop_packet_descriptors_available_tc[DPP_DEVICE_COSQ_EGR_NOF_TC];
    uint32                          port_fc_packet_descriptors;
    uint32                          port_fc_data_buffers;
} bcm_dpp_cosq_egress_thresh_key_info_t;

typedef struct bcm_dpp_cosq_egress_interface_unicast_thresh_key_info_s {

    /* ARAD */

    /* flow control words consumed */
    uint32                          fc_words_consumed;
    /* flow control packet descriptors - ARAD */
    uint32                          fc_packet_descriptors;
} bcm_dpp_cosq_egress_interface_unicast_thresh_key_info_t;

typedef struct bcm_dpp_cosq_egress_interface_multicast_thresh_key_info_s {

    /* ARAD */

    /* flow control packet descriptors - ARAD */
    uint32                          fc_packet_descriptors;
} bcm_dpp_cosq_egress_interface_multicast_thresh_key_info_t;

typedef struct bcm_dpp_cosq_egress_queue_mapping_info_s {
    /* Schedule (UC) words consumed per priority */
    SOC_TMC_EGR_Q_PRIORITY                 queue_mapping[DPP_DEVICE_COSQ_EGR_NOF_Q_PRIO_MAPPING_TYPES][DPP_DEVICE_COSQ_ING_NOF_TC][DPP_DEVICE_COSQ_ING_NOF_DP];
} bcm_dpp_cosq_egress_queue_mapping_info_t;

typedef struct bcm_dpp_cosq_ingress_rate_class_info_s {
    /* Maximal queue size per DP */
    uint32                        queue_max_size[DPP_DEVICE_COSQ_ING_NOF_DP];    
    /* Maximal queue size per DP in Buffer Descriptors */
    uint32                        queue_max_size_bds[DPP_DEVICE_COSQ_ING_NOF_DP];    
    /* queue size Alpha per DP in Buffer Descriptors */
    int8                          queue_alpha_bds[DPP_DEVICE_COSQ_ING_NOF_DP];    
    /* Guaranteed queue size */
    uint32                        queue_min_size;    
    /* Guaranteed queue size in Buffer Descriptors */
    uint32                        queue_min_size_bds;    
    /* WRED Settings */
    /* Enable WRED algorithm test */
    uint8                         wred_enable[DPP_DEVICE_COSQ_ING_NOF_DP];
    /* WRED Min queue average size */
    uint32                        wred_min_avrg_th[DPP_DEVICE_COSQ_ING_NOF_DP];
    /* WRED Max queue average size */
    uint32                        wred_max_avrg_th[DPP_DEVICE_COSQ_ING_NOF_DP];
    /* WRED ignore packet size */
    uint8                         wred_ignore_packet_size[DPP_DEVICE_COSQ_ING_NOF_DP];
    /* WRED Max probability */
    uint32                        wred_max_probability[DPP_DEVICE_COSQ_ING_NOF_DP];
    /* WRED exp. weight */
    uint32                        wred_exp_wq;
    /* OCB */
    SOC_TMC_MGMT_OCB_VOQ_INFO     ocb_info[DPP_DEVICE_COSQ_ING_NOF_TC][DPP_DEVICE_COSQ_QUEUE_NOF_CATEGORY];
    /* ECN max queue sizes (mark congestion above these sizes) */
    uint32                        ecn_queue_max_size;          /* size in bytes */
    uint32                        ecn_queue_max_size_bds;      /* size in BDs/buffers */
    /* ECN WRED settings */
    uint8                         ecn_wred_enable;             /* Enable WRED for marking congestion */
    uint32                        ecn_wred_min_avrg_th;        /* ECN WRED Min queue average size */
    uint32                        ecn_wred_max_avrg_th;        /* ECN WRED Max queue average size */
    uint8                         ecn_wred_ignore_packet_size; /* ECN WRED ignore packet size */
    uint32                        ecn_wred_max_probability;    /* ECN WRED Max probability */

    /* System RED */
    SOC_TMC_ITM_SYS_RED_QT_INFO sys_red_qt;
    SOC_TMC_ITM_SYS_RED_QT_DP_INFO sys_red_qt_dp[DPP_DEVICE_COSQ_ING_NOF_DP];
} bcm_dpp_cosq_ingress_rate_class_info_t;

/* System RED drop probability info */
typedef struct bcm_dpp_cosq_ingress_system_red_dp_pr_info_s {
    uint32 sys_red_dp_pr_val;
} bcm_dpp_cosq_ingress_system_red_dp_pr_info_t;

typedef struct bcm_dpp_cosq_vsq_rate_class_info_s {
    /* WRED Settings */
    /* Enable WRED algorithm test */
    uint8                         wred_enable[DPP_DEVICE_COSQ_VSQ_NOF_CATEGORY_MAX][DPP_DEVICE_COSQ_ING_NOF_DP];
    /* WRED Min queue average size */
    uint32                          wred_min_avrg_th[DPP_DEVICE_COSQ_VSQ_NOF_CATEGORY_MAX][DPP_DEVICE_COSQ_ING_NOF_DP];
    /* WRED Max queue average size */
    uint32                          wred_max_avrg_th[DPP_DEVICE_COSQ_VSQ_NOF_CATEGORY_MAX][DPP_DEVICE_COSQ_ING_NOF_DP];
    /* WRED ignore packet size */
    uint8                         wred_ignore_packet_size[DPP_DEVICE_COSQ_VSQ_NOF_CATEGORY_MAX][DPP_DEVICE_COSQ_ING_NOF_DP];
    /* WRED Max probability */
    uint32                          wred_max_probability[DPP_DEVICE_COSQ_VSQ_NOF_CATEGORY_MAX][DPP_DEVICE_COSQ_ING_NOF_DP];    
    /* Tail drop settings */
    /* Maximal queue size per DP */
    uint32                          queue_max_size[DPP_DEVICE_COSQ_VSQ_NOF_CATEGORY_MAX][DPP_DEVICE_COSQ_ING_NOF_DP];    
    /* Maximal queue size in BDs threshold */
    uint32                          queue_max_size_bds[DPP_DEVICE_COSQ_VSQ_NOF_CATEGORY_MAX][DPP_DEVICE_COSQ_ING_NOF_DP];    
    /* FC settings */
    SOC_TMC_THRESH_WITH_HYST_INFO         fc_queue_size[DPP_DEVICE_COSQ_VSQ_NOF_CATEGORY_MAX];
    SOC_TMC_THRESH_WITH_HYST_INFO         fc_bd_size[DPP_DEVICE_COSQ_VSQ_NOF_CATEGORY_MAX];
} bcm_dpp_cosq_vsq_rate_class_info_t;

/*
 * VSQ valid types
 */
typedef enum {
  /*
   * VSQ General type
   */
  BCM_DPP_COSQ_VSQ_TYPE_GLOBAL = 0,
  /*
   * VSQ Category
   */
  BCM_DPP_COSQ_VSQ_TYPE_CTGRY = 1,
  /*
   * VSQ CTCC
   */
  BCM_DPP_COSQ_VSQ_TYPE_CTCC = 2,
  /*
   * VSQ CTTC
   */
  BCM_DPP_COSQ_VSQ_TYPE_CTTC = 3,
  /*
   * VSQ PP
   */
  BCM_DPP_COSQ_VSQ_TYPE_PP = 4,
  /*
   * VSQ PFC
   */
  BCM_DPP_COSQ_VSQ_TYPE_PFC = 5,
  /*
   * VSQ LLFC
   */
  BCM_DPP_COSQ_VSQ_TYPE_LLFC = 6,
  /*
   * Last entry
   */
  BCM_DPP_COSQ_VSQ_NOF_TYPES
} bcm_dpp_cosq_vsq_type_t;



extern bcm_dpp_cosq_config_t _dpp_cosq_config[BCM_MAX_NUM_UNITS];

/*
 * External functions
 */
int bcm_petra_cosq_fmq_hr_allocate(int unit, int nbr_ports, int *ports);
int bcm_petra_cosq_fmq_hr_deallocate(int unit, int nbr_ports, int *ports);
int bcm_petra_cosq_fmq_vsq_category_mode_set(int unit, bcm_fabric_vsq_category_mode_t vsq_category_mode);
int bcm_petra_cosq_fmq_vsq_category_mode_get(int unit, bcm_fabric_vsq_category_mode_t *vsq_category_mode);

int bcm_petra_cosq_isq_hr_allocate(int unit, int *ports);
int bcm_petra_cosq_isq_hr_deallocate(int unit, int *ports);

extern int
_bcm_petra_cosq_gport_vsq_gport_get(int unit,
                                    bcm_gport_t vsq_gport,
                                    bcm_dpp_cosq_vsq_type_t *vsq_group_type,
                                    int *vsq_index);
#ifdef BCM_ARAD_SUPPORT
int
_bcm_petra_cosq_voq_rate_class_default_set(int unit,
                                           int rate_class,
                                           bcm_dpp_cosq_ingress_rate_class_info_t *rate_class_info);
#endif /*BCM_ARAD_SUPPORT*/
extern uint8 *
_bcm_petra_cosq_element_alloc(int unit, bcm_dpp_cosq_list_hd_t *hd, int element_index);

extern uint8 *
_bcm_petra_cosq_element_get(int unit, bcm_dpp_cosq_list_hd_t *hd, int element_index);

extern int
_bcm_petra_cosq_qid_type_get(int unit, int base_qid, int *gport,
                             int *is_muticast, int *is_unicast, int *is_isq, int *queue_type);

/*
 * Static Functions (Resource allocation)
 */
int
_bcm_petra_flow_region_config(int unit, int region_start, int region_end,
                                              int is_interdigitated, int is_oddEven);

int
_bcm_petra_queue_region_config(int unit, int region_start, int region_end,
                                                                  int is_interdigitated);

int
_bcm_petra_flow_region_config_get(int unit, int flow_region, uint8 *p_is_interdigitated, int *p_is_oddEven);

int
_bcm_petra_queue_region_config_get(int unit, int queue_region, uint8 *is_interdigitated);
/*
 * Static Functions (Resource allocation) 
 * End 
 */
int
_bcm_petra_flow_id_get(int unit, int base_flow_id, int cos, int is_non_contiguous, int is_composite, int is_remove, int *flow);

int
_bcm_petra_composite_flow_id_get(int unit, int base_flow_id, int cos, int is_non_contiguous, int is_composite, int *flow);

int
_bcm_petra_se_dual_flow_id_get(int unit, int flow_id, SOC_TMC_SCH_SE_TYPE type, int is_odd_even, int *flow);

int
_bcm_petra_se_composite_flow_id_get(int unit, int flow_id, SOC_TMC_SCH_SE_TYPE type, int is_odd_even, int *flow);

int
_bcm_petra_cosq_egress_thresh_map_get(int unit, int port, uint32 *profile);

int
_bcm_petra_cosq_ucast_qid_range_set(int unit, int qid_start, int qid_end);

int
_bcm_petra_cosq_ucast_qid_range_get(int unit, int *qid_start, int *qid_end);

int
_bcm_petra_cosq_is_se_reserved(int unit, int se_id, int *is_reserved);

int
_bcm_petra_cosq_is_conn_reserved(int unit, int conn_id, int *is_reserved);

int
bcm_petra_cosq_resource_gport_port_get(int unit, bcm_gport_t gport, bcm_port_t *port);

int
_bcm_petra_cosq_is_queue_reserved(int unit, int queue_id, int *is_reserved);

int
_bcm_cosq_before_traffic_validation(int unit);

int
_bcm_petra_cosq_gport_egress_queue_mapping_hw_get(int unit,
                                                  int profile,
                                                  int core,
                                                  bcm_dpp_cosq_egress_queue_mapping_info_t* mapping_info);
int
_bcm_petra_cosq_gport_egress_map_init(int unit, 
                                      bcm_gport_t port);

int
_bcm_petra_cosq_egress_thresh_entry_get(int unit,
                                        int profile,
                                        bcm_dpp_cosq_egress_thresh_key_info_t* thresh_info);

int
_bcm_petra_cosq_fc_port_threshold_init(int unit, soc_port_t port, int *mc_reserved_pds);

/* dyamic port */
int 
_bcm_petra_cosq_hr_handle(int unit, int port, int enable);

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
extern int
_bcm_dpp_cosq_sw_dump(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */

#endif /* _BCM_INT_DPP_COSQ_H_ */
