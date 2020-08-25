/*
 * $Id: alloc_mngr.h,v 1.119 Broadcom SDK $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File:        alloc_mngr.h
 * Purpose:     Resource allocation manager for DNX_SAND chips.
 *
 */

#ifndef  INCLUDE_DNX_ALLOC_MNGR_H
#define  INCLUDE_DNX_ALLOC_MNGR_H

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (Jer2) family only!"
#endif

#include <soc/dnx/legacy/TMC/tmc_api_tdm.h>
#include <soc/dnx/legacy/drv.h>
#include <bcm_int/dnx/cosq/cosq.h>

#include <bcm_int/dnx/legacy/utils.h>
#include <bcm/rx.h>
#include <shared/swstate/sw_state_resmgr.h>

#include <bcm_int/dnx/cosq/cosq.h>

#define BCM_DNX_AM_TEMPLATE_FLAG_CHECK      (1)

/* Egress thresh */
#define _DNX_AM_TEMPLATE_COSQ_EGR_THRESH_COUNT(unit) (16)

/* CRPS compensation profiles size */
#define _DNX_CRPS_COMPENSATION_PROFILES_SIZE (16)
#define _DNX_STAT_INTERFACE_COMPENSATION_PROFILES_SIZE (16)

typedef struct
{
    uint32 cir;
    uint32 eir;
    uint32 max_cir;
    uint32 max_eir;
    uint32 cbs;
    uint32 ebs;
    uint32 flags;
    uint32 mode;
    uint32 entropy_id;
    uint32 ir_rev_exp;          /* relevant only for MEF mode (cascade) */
} bcm_dnx_am_meter_entry_t;

typedef struct _dnx_res_pool_cosq_s
{
    int dnx_res_pool_cosq_connector_cont;
    int dnx_res_pool_cosq_connector_non_cont_region_type_1;
    int dnx_res_pool_cosq_connector_non_cont_region_type_2;
    int dnx_res_pool_cosq_se_cl_fq_region_type_1;
    int dnx_res_pool_cosq_se_cl_fq_region_type_2;
    int dnx_res_pool_cosq_se_cl_hr;
    int dnx_res_pool_cosq_fq_connector_region_type_2_sync;
    int dnx_res_pool_cosq_hr_connector_region_type_2_sync;
    int dnx_res_pool_cosq_e2e_end;
    int dnx_res_pool_cosq_queue_unicast_cont;
    int dnx_res_pool_cosq_queue_unicast_non_cont;
    int dnx_res_pool_cosq_queue_multicast_cont;
    int dnx_res_pool_cosq_queue_multicast_non_cont;
    int dnx_res_pool_cosq_queue_isq_cont;
    int dnx_res_pool_cosq_queue_isq_non_cont;
    int dnx_res_pool_cosq_queue_dynamic_start;
    int dnx_res_pool_cosq_queue_dynamic_end;
} _dnx_res_pool_cosq_t;

typedef struct bcm_dnx_am_cosq_quad_allocation_s
{
    int flow1;
    int flow2;
    int flow3;
    int flow4;
} bcm_dnx_am_cosq_quad_allocation_t;

#ifdef FIXME_DNX_LEGACY /** SOC_DNX_DEFS not supported */
typedef struct
{
    uint8 fec_bank_groups[SOC_DNX_DEFS_MAX(NOF_FEC_BANKS)];
    PARSER_HINT_ARR int *resource_to_pool_map;
    PARSER_HINT_ARR int *template_to_pool_map;
} bcm_dnx_alloc_mngr_info_t;
#endif

int bcm_dnx_am_attach(
    int unit);

int bcm_dnx_am_clear(
    int unit);

int bcm_dnx_am_detach(
    int unit);

int bcm_dnx_am_mc_dealloc(
    int unit,
    SOC_DNX_MULT_ID mc_id,
    uint8 is_egress);

int bcm_dnx_am_mc_is_alloced(
    int unit,
    SOC_DNX_MULT_ID mc_id,
    uint8 is_egress);

/* Cosq Egress Thresh - Start */
  /*
   * Port level drop & fc thresholds type
   * pointed by tm-port,
   * used by bcm_petra_cosq_threshold_set
   */
int dnx_am_template_egress_thresh_create(
    int unit,
    int default_profile,
    bcm_dnx_cosq_egress_thresh_key_info_t * data);

int _bcm_dnx_am_template_cosq_egr_thresh_data_get(
    int unit,
    int core,
    int port,
    bcm_dnx_cosq_egress_thresh_key_info_t * data);
int _bcm_dnx_am_template_cosq_egr_thresh_exchange(
    int unit,
    int core,
    int port,
    bcm_dnx_cosq_egress_thresh_key_info_t * data,
    int *old_template,
    int *is_last,
    int *template,
    int *is_allocated);
int _bcm_dnx_am_template_cosq_egr_thresh_ref_get(
    int unit,
    bcm_core_t core,
    int template,
    uint32 *ref_count);

int _bcm_dnx_am_template_cosq_egr_thresh_tdata_get(
    int unit,
    bcm_core_t core,
    int template,
    bcm_dnx_cosq_egress_thresh_key_info_t * data);

/* Cosq Egress Thresh - End */

/* VSQ PG TC mapping profile - Start */
#define DNX_ALGO_TEMPLATE_VSQ_PG_TC_MAPPING "TEMPLATE_VSQ_PG_TC_MAPPING"
int dnx_am_template_vsq_pg_tc_mapping_profile_create(
    int unit,
    int default_profile,
    uint32 pg_tc_bitmap);
int _bcm_dnx_am_template_vsq_pg_tc_mapping_profile_data_get(
    int unit,
    bcm_core_t core_id,
    int pg_tc_profile,
    uint32 *pg_tc_bitmap);
int _bcm_dnx_am_template_vsq_pg_tc_mapping_exchange(
    int unit,
    bcm_core_t core_id,
    uint32 pg_tc_bitmap,
    int old_pg_tc_profile,
    int *is_last,
    int *new_pg_tc_profile,
    int *is_allocated);
void dnx_algo_vsq_pg_tc_mapping_profile_print_cb(
    int unit,
    const void *data,
    dnx_algo_template_print_t * print_cb_data);                                                                         /** print callback - not yet implemented */
/* VSQ PG TC mapping profile - End*/

/* Egress port discount class - Start */
int _bcm_dnx_am_template_egress_port_discount_cls_free(
    int unit,
    int core,
    uint32 tm_port,
    int *is_last);
int _bcm_dnx_am_template_egress_port_discount_cls_init(
    int unit,
    int core,
    uint32 tm_port,
    const SOC_DNX_PORT_EGR_HDR_CR_DISCOUNT_INFO * data_discount_cls);
int _bcm_dnx_am_template_egress_port_discount_cls_data_get(
    int unit,
    int core,
    uint32 tm_port,
    SOC_DNX_PORT_EGR_HDR_CR_DISCOUNT_INFO * data_discount_cls);
int _bcm_dnx_am_template_egress_port_discount_cls_exchange(
    int unit,
    int core,
    uint32 tm_port,
    SOC_DNX_PORT_EGR_HDR_CR_DISCOUNT_INFO * data_discount_cls,
    int *old_discount_cls,
    int *is_last,
    int *new_discount_cls,
    int *is_allocated);
/* int _bcm_dnx_am_template_egress_port_discount_cls_ref_get(int unit, int template, uint32 *ref_count); */
int _bcm_dnx_am_template_egress_port_discount_cls_per_header_type_ref_get(
    int unit,
    int core,
    int template,
    SOC_DNX_PORT_HEADER_TYPE header_type,
    uint32 *ref_count);
/* int _bcm_dnx_am_template_egress_port_discount_cls_allocate_group(int unit, uint32 flags, SOC_DNX_PORT_EGR_HDR_CR_DISCOUNT_INFO *data_discount_cls, int ref_count, int *is_allocated, int *template); */
/* int _bcm_dnx_am_template_egress_port_discount_cls_tdata_get(int unit, int template, SOC_DNX_PORT_EGR_HDR_CR_DISCOUNT_INFO *data_discount_cls); */
int _bcm_dnx_am_template_egress_port_discount_cls_per_header_type_tdata_get(
    int unit,
    int core,
    int template,
    SOC_DNX_PORT_HEADER_TYPE header_type,
    SOC_DNX_PORT_EGR_HDR_CR_DISCOUNT_INFO * data_discount_cls);

/* Egress port discount class - End */

/* Fabric TDM direct routing - Start */
int dnx_am_template_fabric_tdm_link_ptr_create(
    int unit,
    int template_init_id,
    SOC_DNX_TDM_DIRECT_ROUTING_INFO * routing_info);
int _bcm_dnx_am_template_fabric_tdm_link_ptr_data_get(
    int unit,
    int port,
    SOC_DNX_TDM_DIRECT_ROUTING_INFO * routing_info);
int _bcm_dnx_am_template_fabric_tdm_link_ptr_exchange(
    int unit,
    int port,
    SOC_DNX_TDM_DIRECT_ROUTING_INFO * routing_info,
    int *old_link_ptr,
    int *is_last,
    int *new_link_ptr,
    int *is_allocated);
/* Fabric TDM direct routing - End */

/* credit adjust size scheduler final delta, relevant only for Jericho */
int _bcm_dnx_am_template_scheduler_adjust_size_final_delta_mapping_init(
    int unit,
    int template_init_id,
    int *final_delta);
int _bcm_dnx_am_template_scheduler_adjust_size_final_delta_mapping_data_get(
    int unit,
    int core,
    int delta,
    int *final_delta);
int _bcm_dnx_am_template_scheduler_adjust_size_final_delta_mapping_exchange(
    int unit,
    int core,
    int delta,
    int *final_delta,
    int *old_profile,
    int *is_last,
    int *new_profile,
    int *is_allocated);
/* Resource manager flags */

#define BCM_DNX_AM_FLAG_ALLOC_WITH_ID               (SW_STATE_RES_ALLOC_WITH_ID)        /* 0x00000001 */
/* if exist then reuse, can be set only if with id is present */
#define BCM_DNX_AM_FLAG_ALLOC_REPLACE               (SW_STATE_RES_ALLOC_REPLACE)        /* 0x00000004 */
#define BCM_DNX_AM_FLAG_ALLOC_EG_DECOUPLED_EVEN     (0x00000004)
#define BCM_DNX_AM_FLAG_ALLOC_INGRESS               (0x00000002)
#define BCM_DNX_AM_FLAG_ALLOC_EGRESS                (0x00000004)

#define BCM_DNX_AM_FLAG_ALLOC_RESERVE               (0x00010000)
#define SOC_DNX_OAM_LOCAL_PORT_2_SYSTEM_PORT_RESERVED_VALUE (-1)

/* relevant for IP tunnel allocation if present then allocate tunnel for IPv6 otherwise allocate for IPv4 */
#define BCM_DNX_AM_FLAG_ALLOC_IPV6                  (0x00020000)

/* Template manager falgs */
#define BCM_DNX_AM_TEMPLATE_FLAG_ALLOC_WITH_ID      (DNX_SHR_TEMPLATE_MANAGE_SET_WITH_ID)       /* 0x00000001 */

/*
 * Template name for TC mapping
 */
#define DNX_ALGO_TEMPLATE_EGR_TC_DP_MAPPING         "Egr TC DP mapping"
#define DNX_ALGO_TEMPLATE_COSQ_PORT_HR_FLOW_CONTROL "TEMPLATE_COSQ_PORT_HR_FLOW_CONTROL"
#define DNX_ALGO_TEMPLATE_COSQ_SCHED_CLASS "TEMPLATE_COSQ_SCHED_CLASS"
#define DNX_ALGO_TEMPLATE_FABRIC_TDM_LINK_PTR "TEMPLATE_FABRIC_TDM_LINK_PTR"

/*
 * \brief
 *   Print functions that needed for template manager.
 *   Not yet implemented.
 */
void dnx_algo_egr_tc_dp_mapping_print_cb(
    int unit,
    const void *data,
    dnx_algo_template_print_t * print_cb_data);
void dnx_algo_queue_discount_cls_print_cb(
    int unit,
    const void *data,
    dnx_algo_template_print_t * print_cb_data);
void dnx_algo_fabric_tdm_link_ptr_print_cb(
    int unit,
    const void *data,
    dnx_algo_template_print_t * print_cb_data);

/* TM */
/*
 *  Following macros provide declaration of various resource calls.
 */
#define _MGR_ALLOC(_type) \
extern int \
bcm_dnx_##_type##_alloc(int unit, uint32 flags, int count, int *elem);
#define _MGR_ALLOC_ALIGN(_type) \
extern int \
bcm_dnx_##_type##_alloc_align(int unit, \
                              uint32 flags, \
                              int align, \
                              int offset, \
                              int count, \
                              int *elem);
#define _MGR_FREE(_type) \
extern int \
bcm_dnx_##_type##_free(int unit, int count, int elem);
#define _MGR_CHECK(_type) \
extern int \
bcm_dnx_##_type##_check(int unit, int count, int elem);
#define _MGR_ALLOC_GROUP(_type) \
extern int \
bcm_dnx_##_type##_alloc_group(int unit, \
                              uint32 grp_flags, \
                              int grp_size, \
                              int *grp_done, \
                              const uint32 *elem_flags, \
                              const int *elem_count, \
                              int *elem);
#define _MGR_ALLOC_ALIGN_GROUP(_type) \
extern int \
bcm_dnx_##_type##_alloc_align_group(int unit, \
                                    uint32 grp_flags, \
                                    int grp_size, \
                                    int *grp_done, \
                                    const uint32 *elem_flags, \
                                    const int *elem_align, \
                                    const int *elem_offset, \
                                    const int *elem_count, \
                                    int *elem);
#define _MGR_FREE_GROUP(_type) \
extern int \
bcm_dnx_##_type##_free_group(int unit, \
                             uint32 grp_flags, \
                             int grp_size, \
                             int *grp_done, \
                             const int *elem_count, \
                             const int *elem);
#define _MGR_CHECK_GROUP(_type) \
extern int \
bcm_dnx_##_type##_check_group(int unit, \
                              uint32 grp_flags, \
                              int grp_size, \
                              int *grp_done, \
                              const int *elem_count, \
                              const int *elem, \
                              int *status);
#define _MGR_ALL_ACTIONS(_type) \
_MGR_ALLOC(_type) \
_MGR_ALLOC_ALIGN(_type) \
_MGR_ALLOC_GROUP(_type) \
_MGR_ALLOC_ALIGN_GROUP(_type) \
_MGR_FREE(_type) \
_MGR_FREE_GROUP(_type) \
_MGR_CHECK(_type) \
_MGR_CHECK_GROUP(_type)
/*
 *  NOTE: must update the below list whenever changing resource types.
 */
/*
 *  Don't export the declaration macros.
 */
#undef _MGR_ALLOC
#undef _MGR_ALLOC_ALIGN
#undef _MGR_FREE
#undef _MGR_CHECK
#undef _MGR_ALLOC_GROUP
#undef _MGR_ALLOC_ALIGN_GROUP
#undef _MGR_FREE_GROUP
#undef _MGR_CHECK_GROUP
#undef _MGR_ALL_ACTIONS

#endif /* INCLUDE_DNX_ALLOC_MNGR_H */
