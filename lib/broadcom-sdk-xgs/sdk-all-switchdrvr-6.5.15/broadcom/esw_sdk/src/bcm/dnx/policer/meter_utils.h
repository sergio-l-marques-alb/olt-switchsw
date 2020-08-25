 /** meter_generic.h
 * 
 *  DNX METER UTILS H FILE. (INTERNAL METER FILE)
 * 
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */

#ifndef _DNX_METER_UTILS_INCLUDED__
/*
 * { 
 */
#define _DNX_METER_UTILS_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
 * DEFINEs/MACROs
 * {
 */

#define DNX_METER_UTILS_DEFAULT_PROFILE_ID (0)

#define DNX_METER_UTILS_INVALID_IR_REV_EXP (0xFFFFFFFF)
#define DNX_METER_UTILS_MAX_RATE_UNLIMITED (0xFFFFFFFF)
/** const value to devide in the rate formula. */
/** rate[kbps]=CIR_MANT * ((CLK_FREQ_MHZ*(10^6)/2)/(2**(CIR_REV_EXP))) *8* (2^CIR_MANT_EXP))/ (10^3) */
#define DNX_METER_UTILS_MHZ_TO_KBITS_PER_SEC (250)
#define DNX_METER_UTILS_KBITS_TO_BYTES (125) /** const value used in the burst calculation. (1000/8)*/
#define DNX_METER_UTILS_PACKETS_TO_BYTES_FACTOR (64)

#define DNX_METER_UTILS_PROFILE_IR_MAX_UNLIMITED (0xFFFFFFFF)

/* cir, max_cir (if sharing), eir, max_eir
   and if global_sharing: max+min rates of 3 other meters of 4 hierarchical_meters */
#define DNX_METER_UTILS_PROFILE_MAX_RATES_FOR_CALC (6)
#define DNX_METER_UTILS_NOF_METER_CASCADE_MODE (4) /** number of meters to consider in cascade modes (MEF mode) */

#define DNX_METER_UTILS_BURST_MANT_MIN_VAL (1)
#define DNX_METER_UTILS_RATE_MANT_MIN_VAL (1)
#define DNX_METER_UTILS_RATE_EXP_MIN_VAL (0)
#define DNX_METER_UTILS_BURST_EXP_MIN_VAL (0)
#define DNX_METER_UTILS_BURST_MANT_CONST_VAL (0)
#define DNX_METER_UTILS_BURST_MULTI_CONST_VAL (1)
#define DNX_METER_UTILS_RATE_MANT_CONST_VAL (0)

/*
 * }
 */

/*
 * structures
 * {
 */

typedef struct
{
    uint32 color_aware;
    uint32 coupling_flag;
    uint32 sharing_flag;
    uint32 cir_mantissa;
    uint32 cir_mantissa_exponent;
    uint32 reset_cir;
    uint32 cbs_mantissa_64;
    uint32 cbs_exponent;
    uint32 eir_mantissa;
    uint32 eir_mantissa_exponent;
    uint32 max_eir_mantissa;
    uint32 max_eir_mantissa_exponent;
    uint32 reset_eir;
    uint32 ebs_mantissa_64;
    uint32 ebs_exponent;
    uint32 packet_mode;
} meter_utils_profile_table_data;

 /**
 * \brief
 *      Verify that the bucket size is not smaller than the amount of credits it suppose to get in each credits chunk 
 * chunk  [credits] = mant1*2^exp1 + mant2*2^exp2
 * bucket [credits] = (bucket_mant+64)*2^bucket_exp 
 *  if bucket < chunk: bucket=chunk, else: do nothing
 *
 * \param [in] unit -unit id
 * \param [in] rate1_man -rate 1 mantissa
 * \param [in] rate1_exp - rate 1 exponent
 * \param [in] rate2_man -rate 2 mantissa
 * \param [in] rate2_exp -rate 2 exponent
 * \param [out] bucket_man -bucket mantissa 
 * \param [out] bucket_exp - bucket exponent
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
/** see .h file */
shr_error_e dnx_meter_utils_bucket_size_adjust(
    int unit,
    uint32 rate1_man,
    uint32 rate1_exp,
    uint32 rate2_man,
    uint32 rate2_exp,
    uint32 *bucket_man,
    uint32 *bucket_exp);

/**
 * \brief
 *      Break down the burst size to mantissa and exponent.
 * \param [in] unit -unit id
 * \param [in] burst -burst size required
 * \param [in] is_large_bucket_mode -is large bucket mode
 * \param [out] mnt -calculation of mantissa value
 * \param [out] exp -calculation of exp value 
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
shr_error_e dnx_meter_utils_profile_burst_to_exp_mnt(
    int unit,
    uint32 burst,
    int is_large_bucket_mode,
    uint32 *mnt,
    uint32 *exp);

/**
 * \brief
 * Break down the rate to mantissa, exponent and resolution.
 * Used for global Meter Profiles and may be used for single meters.
 * \param [in] unit -unit id
 * \param [in] rate -input rate
 * \param [out] rev_exp -
 * \param [out] mnt -
 * \param [out] exp -
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
shr_error_e dnx_meter_utils_profile_rate_to_res_exp_mnt(
    int unit,
    uint32 rate,
    uint32 *rev_exp,
    uint32 *mnt,
    uint32 *exp);

/**
 * \brief
 * get back the min rate and max rate, per rev exponent
 * \param [in] unit -unit id
 * \param [in] rev_exp -reverse exponent
 * \param [out] min_rate -min rate calculated
 * \param [out] max_rate -max rate calculated
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
shr_error_e dnx_meter_utils_min_max_rate_per_rev_exp_get(
    int unit,
    uint32 rev_exp,
    uint32 *min_rate,
    uint32 *max_rate);

/**
 * \brief
 *      calculate the max and min rate and burst size of the unit and hold it in global structure
 * \param [in] unit -unit id
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   function assume all rates and all burst has the same max values. (ingress/egress/global and cir/eir)
 * \see
 *   NONE
 */
shr_error_e dnx_utils_limits_profile_info_init(
    int unit);

/**
 * \brief
 * First calculate the highest shared rev_exp as described in arad_pp_mtr_ir_val_to_max_rev_exp. 
 * Afterwards look for a lower rev_exp, as long as the accuracy of the rates are unaffected by it. 
 * \param [in] unit -unit id
 * \param [in] rates -array of rates to consider when calc the rev_exp
 * \param [in] nof_rates -size of rates array
 * \param [out] rev_exp -calculated reverse exponent
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 * \remark
 *   NONE
 * \see
 *   NONE
 */
shr_error_e dnx_meter_utils_rev_exp_optimized_for_bucket_rate(
    int unit,
    uint32 *rates,
    uint32 nof_rates,
    uint32 *rev_exp);

/**
* \brief
*      wrapper function for getting the rate from mantissa, exponent and reverse exponent.
*      the rate is rounded to closest value.
* \param [in] unit -unit id
* \param [in] mnt -mantissa
* \param [in] exp -exponent
* \param [in] rev_exp -reverse exponent
* \param [out] rate -calculated rate
* \return
* shr_error_e
* \remark
*   NONE
* \see
*   NONE
*/
shr_error_e dnx_meter_utils_rate_from_mnt_exp_rev_exp_get(
    int unit,
    uint32 mnt,
    uint32 exp,
    uint32 rev_exp,
    uint32 *rate);

/**
* \brief
*   function return if meter bucket profile is in large bucket mode.
* \param [in] unit        - unit id
* \param [in] cbs - green bucket size
* \param [in] ebs - yellow bucket size
* \param [out] is_large_bucket_mode - TRUE is working in large bucket mode, otherwise FALSE.
* \return
*   shr_error_e - Error Type
* \remark
* 
* \see
*   * None
*/
shr_error_e meter_utils_large_bucket_mode_get(
    int unit,
    uint32 cbs,
    uint32 ebs,
    uint32 *is_large_bucket_mode);

#endif/*_DNX_METER_UTILS_INCLUDED__*/
