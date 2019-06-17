/**
 * \file egq_ofp_rates.h
 * 
 * Internal DNX procedures related to ofp rates 
 * 
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */

#ifndef EGQ_OFP_RATES_H_INCLUDED
/** { */
#define EGQ_OFP_RATES_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif
/*
 * This file was ported from:
 *   arad_ofp_rates.h
 *   jer_ofp_rates.h
 */
#include <bcm/types.h>
#include <bcm_int/dnx/cosq/cosq.h>
#include <bcm_int/dnx/cosq/egress/egr_queuing.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_egr_queuing.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>

/*
 * Defines
 * {
 */

#define DNX_OFP_RATES_NOF_CALS_IN_DUAL_MODE    2

/**
 * Number of clocks to traverse a single calendar slot in the EGQ
 */
#define DNX_EGQ_CAL_SLOT_TRAVERSE_IN_CLOCKS    (1)
/**
 * Calender maximum length for first 2 channelize arbiters
 */
#define DNX_OFP_RATES_CAL_LEN_EGQ_MAX          (DNX_DATA_MAX_EGR_QUEUING_PARAMS_CALENDAR_SIZE)
/**
 * EGQ bandwidth The unit is 1/256th byte chanelized arbiters
 */
#define DNX_EGQ_UNITS_VAL_IN_BITS_CHAN_ARB     (dnx_data_egr_queuing.params.cal_res_get(unit))
/**
 * EGQ bandwidth The unit is 1/128th byte for QPAIR and TCG
 */
#define DNX_EGQ_UNITS_VAL_IN_BITS_QPAIR_TCG    (dnx_data_egr_queuing.params.cal_res_get(unit))
/**
 * Maximal number of bytes per burst. This value is converted,
 * by multiplying by '', before loading into HW.
 */
#define DNX_EGQ_MAX_BURST_IN_BYTES                     0x4000

#define DNX_OFP_RATES_CAL_LEN_SCH_MAX                  0x0400

#define DNX_OFP_RATES_CAL_LEN_EGQ_PORT_PRIO_MAX        0x0200
#define DNX_OFP_RATES_CAL_LEN_EGQ_TCG_MAX              0x0200

#define DNX_OFP_RATES_CALCAL_LEN_EGQ_MAX        (dnx_data_egr_queuing.params.cal_cal_len_get(unit))     /* 0x0100 */
#define DNX_OFP_RATES_CAL_LEN_SCH_MAX                  0x0400
/**
 * This definition will probably not be needed after porting is done.
 */
#define DNX_EGQ_NIF_SCM_TBL_ENTRY_SIZE                 3
#define DNX_EGQ_PMC_TBL_ENTRY_SIZE                     3
#define DNX_EGQ_DWM_TBL_ENTRY_SIZE                     1
#define DNX_EGQ_ERPP_PER_PORT_TBL_ENTRY_SIZE           5
/**
 * Have a specific offset for TCG ID
 */
#define DNX_OFP_RATES_TCG_ID_GET(ps,tcg_ndx) \
  (ps*DNX_NOF_TCGS_IN_PS + tcg_ndx)
/**
 *  Setting  this value as maximal burst will result in no burst limitation
 */
#define DNX_OFP_RATES_BURST_LIMIT_MAX                  (0xFFFF)
#define DNX_OFP_RATES_BURST_DEFAULT                    (0x4000 * dnx_data_egr_queuing.params.cal_burst_res_get(unit))
#define DNX_OFP_RATES_BURST_FC_Q_LIMIT_MAX             (0x3FFF)
#define DNX_OFP_RATES_BURST_EMPTY_Q_LIMIT_MAX          (0x3FFF)
/*
 *  The coefficient to convert 1Kilo-bit-per-second to bit-per-second (e.g.).
 */
#define DNX_RATE_1K                                    1000
/*
 * Maximal interface rate, in Mega-bits per second.
 * This is the upper boundary, it can be lower
 *  depending on the credit size
 */
#define DNX_IF_MAX_RATE_MBPS_DNX(unit)                (DNX_RATE_1K * dnx_data_egr_queuing.params.max_gbps_rate_egq_get(unit))
/**
 * Maximal interface rate, in Kilo-bits per second.
 * This is the upper boundary, it can be lower
 *  depending on the credit size
 */
#define DNX_IF_MAX_RATE_KBPS(unit)                (DNX_RATE_1K * DNX_IF_MAX_RATE_MBPS_DNX(unit))
/*
 *  Setting  this value as maximal burst will result in no burst limitation
 */
#define DNX_INVALID_CALENDAR      0xff
/*
 * }
 */
/*
 * Typedefs
 * {
 */
typedef struct
{
    uint32 port_cr_to_add;
    uint32 ofp_index;
} dnx_egq_scm_tbl_data_t;
/**
 * A single entry of OFP calendar - EGQ. 
 * Consists of: base_q_pair and credit. 
 */
typedef struct
{
    uint32 base_q_pair;
    uint32 credit;
} dnx_ofp_egq_rates_cal_entry_t;
typedef struct
{
    uint32 interface_select;
} dnx_egq_ccm_tbl_data_t;
typedef struct
{
    uint32 port_max_credit;
} dnx_egq_pmc_tbl_data_t;
typedef enum
{
  /**
   *  Calendar Set A - scheduler and EGQ.
   */
    DNX_OFP_RATES_CAL_SET_A = 0,
  /**
   *  Calendar Set B - scheduler and EGQ.
   */
    DNX_OFP_RATES_CAL_SET_B,
  /**
   *  Total number of calendar sets.
   */
    DNX_OFP_NOF_RATES_CAL_SETS
} dnx_ofp_rates_cal_set_e;

typedef enum
{
    DNX_OFP_RATES_EGQ_CHAN_ARB_FIELD_RATE = 0,
    DNX_OFP_RATES_EGQ_CHAN_ARB_FIELD_MAX_BURST,
    DNX_OFP_RATES_EGQ_CHAN_ARB_FIELD_CAL_LEN,
    DNX_OFP_RATES_EGQ_CHAN_ARB_NOF_FIELD_TYPE
} dnx_ofp_rates_egq_chan_arb_field_type_e;

/*
 * A single entry of OFP calendar - SCH. 
 * Consists of: base_port_tc. 
 */
typedef uint32 dnx_ofp_sch_rates_cal_entry_t;

typedef enum
{
    FIRST_DNX_OFP_RATES_EGQ_CAL_TYPE = 0,
    /*
     * Represents Channelize arbiter calendar
     */
    DNX_OFP_RATES_EGQ_CAL_CHAN_ARB = FIRST_DNX_OFP_RATES_EGQ_CAL_TYPE,
    /*
     * Represents TCG calendar
     */
    DNX_OFP_RATES_EGQ_CAL_TCG,
    /*
     * Represents Port Priority calendar
     */
    DNX_OFP_RATES_EGQ_CAL_PORT_PRIORITY,
    /*
     * Number of calendar types
     */
    NUM_DNX_OFP_RATES_EGQ_CAL_TYPE
} dnx_ofp_rates_egq_cal_type_e;

typedef struct
{
    /**
     *  Calendar type.   
     */
    dnx_ofp_rates_egq_cal_type_e cal_type;
    /**
     *  More information:
     *  currently only channelize arbiter id in case of Channelize calendar
     */
    uint32 chan_arb_id;
    /**
     *  More information:
     *  relevant for CHAN_ARB and determines the slot_id in which the calendar is started
     */
    uint32 arb_slot_id;
} dnx_ofp_rates_cal_info_t;
/*
 *  EGQ calendar
 */
typedef struct
{
    dnx_ofp_egq_rates_cal_entry_t slots[DNX_OFP_RATES_CAL_LEN_EGQ_MAX];
} dnx_ofp_rates_cal_egq_t;
/*
 *  Scheduler calendar.
 *  Note: credit is always '1' for scheduler calendar.
 */
typedef struct
{
    dnx_ofp_sch_rates_cal_entry_t slots[DNX_OFP_RATES_CAL_LEN_SCH_MAX];
} dnx_ofp_rates_cal_sch_t;
typedef enum
{
    DNX_OFP_RATES_GENERIC_FUNC_STATE_TCG_SCH_RATE = 0,
    DNX_OFP_RATES_GENERIC_FUNC_STATE_PTC_SCH_RATE = 1,
    DNX_OFP_RATES_GENERIC_FUNC_STATE_TCG_SCH_BURST = 2,
    DNX_OFP_RATES_GENERIC_FUNC_STATE_PTC_SCH_BURST = 3,
    DNX_OFP_RATES_GENERIC_FUNC_STATE_TCG_EGQ_RATE = 4,
    DNX_OFP_RATES_GENERIC_FUNC_STATE_PTC_EGQ_RATE = 5,
    DNX_OFP_RATES_GENERIC_FUNC_STATE_TCG_EGQ_BURST = 6,
    DNX_OFP_RATES_GENERIC_FUNC_STATE_PTC_EGQ_BURST = 7,
    DNX_OFP_RATES_GENERIC_FUNC_STATE_PTC_EGQ_EMPTY_Q_BURST = 8,
    DNX_OFP_RATES_GENERIC_FUNC_STATE_PTC_EGQ_FC_Q_BURST = 9
} dnx_ofp_rates_generic_func_state_e;
typedef enum
{
    FIRST_DNX_OFP_RATES_EGQ_CHAN_ARB_ID = 0,
    DNX_OFP_RATES_EGQ_CHAN_ARB_00 = FIRST_DNX_OFP_RATES_EGQ_CHAN_ARB_ID,
    DNX_OFP_RATES_EGQ_CHAN_ARB_01,
    DNX_OFP_RATES_EGQ_CHAN_ARB_02,
    DNX_OFP_RATES_EGQ_CHAN_ARB_03,
    DNX_OFP_RATES_EGQ_CHAN_ARB_04,
    DNX_OFP_RATES_EGQ_CHAN_ARB_05,
    DNX_OFP_RATES_EGQ_CHAN_ARB_06,
    DNX_OFP_RATES_EGQ_CHAN_ARB_07,
    DNX_OFP_RATES_EGQ_CHAN_ARB_CPU,
    DNX_OFP_RATES_EGQ_CHAN_ARB_08 = DNX_OFP_RATES_EGQ_CHAN_ARB_CPU,
    DNX_OFP_RATES_EGQ_CHAN_ARB_RCY,
    DNX_OFP_RATES_EGQ_CHAN_ARB_09 = DNX_OFP_RATES_EGQ_CHAN_ARB_RCY,
    DNX_OFP_RATES_EGQ_CHAN_ARB_NON_CHAN,
    DNX_OFP_RATES_EGQ_NOF_CHAN_ARB,
    /*
     * Number of calendar types
     */
    NUM_DNX_OFP_RATES_EGQ_CHAN_ARB_ID,
    DNX_OFP_RATES_EGQ_CHAN_ARB_INVALID = DNX_INVALID_CALENDAR
} dnx_ofp_rates_egq_chan_arb_id_e;
/*
 * }
 */
/*
 * Procedure headers
 * {
 */
/*
 * JER2 calender algorithm {
 */
/**
 * transform if_id+nif_type to nif_idx for api commands
 *  if_id : 0-31 
 *  nif_type : ILKN-0,ILKN-1, RXAUI-0 ...
 *  nif_idx:0-1000 (api "knows" only nif_idx values. 
*/
int dnx_ofp_rates_fixed_len_cal_build(
    int unit,
    uint32 *port_nof_slots,
    uint32 nof_ports,
    uint32 calendar_len,
    uint32 max_calendar_len,
    uint32 is_fqp_pqp,
    uint32 *calendar);
/**
 * \brief
 *   Translate from kilobit-per-second
 *   to shaper internal representation:
 *   units of 1/256 Bytes per clock
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] rate_kbps -
 *   Rate in kilo BPS.
 * \param [in] calendar_slots - the number of slots dedicated for the shaper
 * \param [in] calendar_size - the total size of calendar (in slots)
 * \param [out] rate_internal -
 *   Pointer to uint32. This procedure loads pointed memory
 *   by the internal representation of 'rate_kbps'.
 * \return
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e
 * \remark
 *   None.
 * \see
 *   * dnx_ofp_rates_egq_shaper_rate_from_internal
 */
int dnx_ofp_rates_egq_shaper_rate_to_internal(
    int unit,
    uint32 rate_kbps,
    uint32 calendar_slots,
    uint32 calendar_size,
    uint32 *rate_internal);
/**
 * \brief
 *   Translate from kilobit-per-second
 *   to shaper internal representation:
 *   units of 1/256 Bytes per clock
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] rate_internal -
 *   Internal representation of 'rate'.
 * \param [in] calendar_slots - the number of slots dedicated for the shaper
 * \param [in] calendar_size - the total size of calendar (in slots)
 * \param [out] rate_kbps -
 *   Pointer to uint32. This procedure loads pointed memory
 *   by the standard kbps rate as converted from 'rate_internal'.
 * \return
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e
 * \remark
 *   None.
 * \see
 *   * dnx_ofp_rates_egq_shaper_rate_to_internal
 */
int dnx_ofp_rates_egq_shaper_rate_from_internal(
    int unit,
    uint32 rate_internal,
    uint32 calendar_slots,
    uint32 calendar_size,
    uint32 *rate_kbps);
/*
 * }
 */

/*
 * this function is enable/disable design read from otm and interface credit table.
 * This function will enable/disable shapers accordingly in order to not cause any traffic lost.
 * however, disabling shaper will effect all the ports in the system
 */
int dnx_ofp_rates_if_port_read_enable_set(
    int unit,
    int core,
    uint32 read_en,
    uint32 *org_if_read_en,
    uint32 *org_port_read_en);
int dnx_ofp_rates_init(
    int unit);

int dnx_ofp_rates_deinit(
    int unit);

int dnx_ofp_rates_interface_internal_rate_set(
    int unit,
    int core,
    uint32 egr_if_id,
    uint32 internal_rate);

int dnx_ofp_rates_calcal_config(
    int unit,
    int core);

/*
 * Function to copy the CalCal length and entries from the active to inactive calendar before switching them.
 * This is done in case of setting BW for a channalized local port, because in this case re-calculation of 
 * the CalCal and interface shapers is not needed.
 */
int dnx_ofp_rates_calcal_copy(
    int unit,
    int core);

int dnx_ofp_rates_egq_single_port_rate_hw_set(
    int unit,
    bcm_core_t core);

/*
 * Function to re-alculate and set the CalCal and all OTM shapers.
 * The parameter if_recalc is TRUE if it is needed to recalculate calcal and interface shapers.
 * Else copy calcal to the active calendar and recalculate only port shapers.
 */
int dnx_ofp_rates_egq_otm_shapers_set(
    int unit,
    int core,
    uint8 if_recalc);

int dnx_ofp_rates_egq_interface_shaper_get(
    int unit,
    int core,
    uint32 tm_port,
    uint32 *if_shaper_rate);

int dnx_ofp_rates_interface_internal_rate_get(
    int unit,
    int core,
    uint32 egr_if_id,
    uint32 *internal_rate);

int dnx_ofp_rates_egq_single_port_rate_sw_set(
    int unit,
    int core,
    uint32 tm_port,
    uint32 rate);

int dnx_ofp_rates_port2chan_cal_get(
    int unit,
    int core,
    uint32 tm_port,
    uint32 *calendar);

int dnx_ofp_rates_egress_shaper_cal_write(
    int unit,
    int core,
    dnx_ofp_rates_cal_info_t * cal_info,
    dnx_ofp_rates_cal_set_e cal2set,
    dnx_ofp_rates_egq_chan_arb_field_type_e field_type,
    uint32 data);

int dnx_ofp_rates_egress_shaper_cal_read(
    int unit,
    int core,
    dnx_ofp_rates_cal_info_t * cal_info,
    dnx_ofp_rates_cal_set_e cal2set,
    dnx_ofp_rates_egq_chan_arb_field_type_e field_type,
    uint32 *data);
int dnx_ofp_rates_max_credit_empty_port_set(
    int unit,
    int arg);
int dnx_ofp_rates_max_credit_empty_port_get(
    int unit,
    int *arg);

int dnx_ofp_rates_egq_single_port_rate_hw_get(
    int unit,
    int core,
    uint32 tm_port,
    uint32 *rate);
int dnx_ofp_rates_egq_single_port_rate_sw_get(
    int unit,
    int core,
    uint32 tm_port,
    uint32 *rate);
/*
 * PTC max burst setting
 */
int dnx_ofp_rates_egq_port_priority_max_burst_set(
    int unit,
    int core,
    uint32 tm_port,
    uint32 priority_ndx,
    uint32 max_burst);
int dnx_ofp_rates_egq_port_priority_max_burst_get(
    int unit,
    int core,
    uint32 tm_port,
    uint32 priority_ndx,
    uint32 *max_burst);
/*
 * TCG max burst setting
 */
int dnx_ofp_rates_egq_tcg_max_burst_set(
    int unit,
    int core,
    uint32 tm_port,
    uint32 tcg_ndx,
    uint32 max_burst);
int dnx_ofp_rates_egq_tcg_max_burst_get(
    int unit,
    int core,
    uint32 tm_port,
    uint32 tcg_ndx,
    uint32 *max_burst);
/*
 * Single port burst setting
 */
int dnx_ofp_rates_single_port_max_burst_set(
    int unit,
    int core,
    uint32 tm_port,
    uint32 max_burst);
int dnx_ofp_rates_single_port_max_burst_get(
    int unit,
    int core,
    uint32 tm_port,
    uint32 *max_burst);
int dnx_ofp_rates_port_priority_max_burst_for_empty_queues_set(
    int unit,
    uint32 max_burst_empty_queues);
int dnx_ofp_rates_port_priority_max_burst_for_fc_queues_set(
    int unit,
    uint32 max_burst_fc_queues);
int dnx_ofp_rates_port_priority_max_burst_for_fc_queues_get(
    int unit,
    uint32 *max_burst_fc_queues);
int dnx_ofp_rates_port_priority_max_burst_for_empty_queues_get(
    int unit,
    uint32 *max_burst_empty_queues);
int dnx_ofp_rates_egq_tcg_rate_sw_set(
    int unit,
    int core,
    uint32 tm_port,
    dnx_tcg_ndx_t tcg_ndx,
    uint32 tcg_rate);
int dnx_ofp_rates_egq_port_priority_rate_sw_set(
    int unit,
    int core,
    uint32 tm_port,
    uint32 prio_ndx,
    uint32 ptc_rate);
int dnx_ofp_rates_egq_tcg_rate_hw_get(
    int unit,
    int core,
    dnx_fap_port_id_t tm_port,
    dnx_tcg_ndx_t tcg_ndx,
    uint32 *tcg_rate);
int dnx_ofp_rates_egq_port_priority_rate_hw_get(
    int unit,
    int core,
    uint32 tm_port,
    dnx_tcg_ndx_t ptc_ndx,
    uint32 *ptc_rate);
int dnx_ofp_rates_egq_tcg_rate_hw_set(
    int unit);
int dnx_ofp_rates_egq_port_priority_rate_hw_set(
    int unit);

/* } */
/* } */
#endif
/* EGQ_OFP_RATES_H_INCLUDED */
