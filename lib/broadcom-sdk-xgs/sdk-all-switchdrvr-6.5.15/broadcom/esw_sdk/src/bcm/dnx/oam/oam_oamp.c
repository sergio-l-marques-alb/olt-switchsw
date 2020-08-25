/** \file oam_oamp.c
 * $Id$
 *
 * OAMP procedures for DNX.
 *
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_OAM

 /*
  * Include files.
  * {
  */
#include <src/bcm/dnx/oam/oam_oamp.h>
#include <src/bcm/dnx/oam/oamp_pe.h>
#include <src/bcm/dnx/oam/oam_oamp_crc.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_oam.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/mpls/mpls.h>
#include <bcm_int/dnx/bfd/bfd.h>
#include <include/bcm/vlan.h>
#include <include/bcm_int/dnx/switch/switch_tpid.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_oam.h>
#include <bcm_int/dnx/algo/oamp/algo_oamp.h>
#include <soc/dnx/swstate/auto_generated/access/algo_oam_access.h>
#include <soc/dnx/swstate/auto_generated/access/algo_oamp_access.h>
#include <include/bcm/switch.h>
#include <include/bcm_int/dnx_dispatch.h>
#include <include/soc/dnx/intr/auto_generated/jer2/jer2_intr.h>

/*
 * }
 */

/*
 * DEFINEs
 * {
 */

/** Value for 5 byte FHEI */
#define OAM_PUNT_FHEI_SIZE 2

#define MPLS_LABEL_GAL_TTL 64
#define MPLS_LABEL_GAL_EXP 0
#define MPLS_LABEL_GAL_BOS 1

#define BFD_PWE_DEFAULT_CHANNEL 0x0007
#define BFD_MPLS_TP_CC_GACH_DEFAULT_CHANNEL 0x0022

#define Y1731_MPLS_TP_GACH_DEFAULT_CHANNEL 0x8902

#define Y1731_PWE_GACH_DEFAULT_CHANNEL 0x0027

#define DNX_OAM_ETH_SA_MAC_LSB_SIZE_IN_BITS     (15)
#define DNX_OAM_ETH_SA_MAC_SECOND_BYTE_MASK UTILEX_BITS_MASK(DNX_OAM_ETH_SA_MAC_LSB_SIZE_IN_BITS - SAL_UINT8_NOF_BITS - 1,0)

/**
 *  For self contained OAM endpoints, the LM/DM data is
 *  contained in these types of entries
 */
#define IS_LM_DM_TYPE(x) ((x == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_LM_DB) ||\
                          (x == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_LM_STAT) ||\
                          (x == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_DM_STAT) ||\
                          (x == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_DM_STAT_ONE_WAY))

/** Masks for TCAM trap table */
#define DEFAULT_MEP_TYPE_MASK 0
#define DEFAULT_GACH_SEL_MASK SAL_UPTO_BIT(1)
#define DEFAULT_GACH_CHAN_MASK SAL_UPTO_BIT(16)
#define DEFAULT_TRAP_CODE_MASK 0

/** Number of entries in TCAM trap table */
#define NOF_CLS_TRAP_TABLE_ENTRIES SAL_UPTO_BIT(5)

/** Set the RDI generation method in OAMP according to  application RDI requirements provided */
#define DNX_OAM_SET_RDI_GEN_METHOD_FIELD_ON_MEP_PROFILE(rdi_method, rx_disable, scanner_disable) \
               rdi_method = ( (rx_disable)==0) <<1 | ((scanner_disable)==0)

/** Get the application RDI requirements from the RDI generation method in OAMP */
#define DNX_OAM_GET_RDI_GEN_METHOD_FIELD_FROM_MEP_PROFILE(rdi_method, is_rdi_from_rx, is_rdi_from_scanner) \
    do {\
        is_rdi_from_rx = ((rdi_method & 0x2)==0x2);\
        is_rdi_from_scanner = (rdi_method & 0x1);\
    } while (0)

/** CCM_CNT is 19 bits in Jericho_2 */
#define DNX_OAM_OAMP_ETH1731_MEP_PROFILE_MAX_CCM_CNT 0x7ffff

/** Get oam_ts_fromat for OAMP response TCAM data, from opcode_or_channel*/
#define DNX_OAM_RSP_TCAM_OPCODE_TO_OAM_TS_FORMAT(opcode_or_channel) \
    (((opcode_or_channel)==DBAL_DEFINE_OAM_OPCODE_LMM) || ((opcode_or_channel)==DBAL_DEFINE_OAM_OPCODE_SLM))? DBAL_ENUM_FVAL_OAM_SUB_TYPE_LOSS_MEASUREMENT : \
    ((opcode_or_channel)==DBAL_DEFINE_OAM_OPCODE_DMM)? DBAL_ENUM_FVAL_OAM_SUB_TYPE_DELAY_MEASUREMENT_1588: \
    DBAL_ENUM_FVAL_OAM_SUB_TYPE_NULL

/** Get OAMP pending event interrupt by device type.(currently only Jericho_2 */
#define _DNX_OAM_GET_OAMP_PENDING_EVENT_BY_DEVICE(unit) JER2_INT_OAMP_PENDING_EVENT

/**
 *  Structure used as a parameter for
 *  dnx_oamp_mep_db_find_or_add_lm_dm_entry, the function that
 *  searches the LM/DM entries associated with an OAM endpoint
 *  for a certain type of entry
 */
typedef struct
{
    /**
     *  Entry ("OAM_ID" format) to which the CCM entry's flex LM/DM field is
     *  currently pointing
     */
    uint32 first_entry;

    /**
     *  Entry ("OAM_ID" format) specified by the user to which the flex LM/DM field
     *  should point. If the first entry field points to a legitimate
     *  LM/DM entry, this field is ignored.
     */
    uint32 param_entry;

     /**
      *  Entry number ("OAM_ID" format) where entry data should be
      *  written
      */
    uint32 resulting_entry;

     /**
      * boolean that indicates whether an existing entry was found,
      * or not.
      */
    uint8 update_entry;

     /**
      *  boolean that indicates whether this MEP already had an
      *  entry marked "last entry," which should now be unmarked.
      */
    uint8 clear_last_entry;

     /**
      *  Type of entry that is being sought (one of four possible
      *  types.)
      */
    dbal_enum_value_field_oamp_mep_type_e search_type;

} dnx_oam_oamp_lm_dm_search_t;
/*
 * }
 */

/*
 * MACROs
 * {
 */

/*
 * }
 */

/*
 * Global and Static
 * {
 */

/*
 * }
 */

/**
* \brief
*  This function calculate mep_id_shift and RX/TX statistics enable per profile.
* \par DIRECT INPUT:
*   \param [in] unit  -
*   Relevant unit.
*   \param [in] flags  -
*   Endpoint flags2 parameter, used to enable RX/TX statistics and per opcode count.
*   \param [out] mep_profile_data  -
*   hold information for mep_profile.
* \par INDIRECT INPUT:
*   * none
* \par DIRECT OUTPUT:
*   shr_error_param - Return relevant error.
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
static void
dnx_oam_oamp_statistics_enable(
    int unit,
    uint32 flags,
    dnx_oam_mep_profile_t * mep_profile_data)
{

    if (_SHR_IS_FLAG_SET(flags, BCM_OAM_ENDPOINT_FLAGS2_TX_STATISTICS))
    {
        mep_profile_data->opcode_tx_statistics_enable = TRUE;
    }

    if (_SHR_IS_FLAG_SET(flags, BCM_OAM_ENDPOINT_FLAGS2_RX_STATISTICS))
    {
        mep_profile_data->opcode_rx_statistics_enable = TRUE;
    }

    mep_profile_data->mep_id_shift = 0;

    if (_SHR_IS_FLAG_SET(flags, BCM_OAM_ENDPOINT_FLAGS2_PER_OPCODE_STATISTICS))
    {
        mep_profile_data->opcode_mask = TRUE;
        mep_profile_data->mep_id_shift = 3;
    }

}

/**
 * \brief
 * This function get the rmep index and return result type of OAMP_RMEP_DB entry
 * Result Types are:
 *     1. OAM_SELF_CONTAINED
 *     2. BFD_SELF_CONTAINED
 *     3. OAM_OFFLOADED
 *     4. BFD_OFFLOADED
 */
static dbal_enum_value_result_type_oamp_rmep_db_e
dnx_oam_oamp_rmep_type_get(
    int unit,
    uint32 rmep_idx,
    uint32 is_oam)
{
    uint32 self_contained_threshold;

    self_contained_threshold = dnx_data_oam.oamp.oamp_rmep_full_entry_threshold_get(unit);

    if (rmep_idx < self_contained_threshold)
    {
        return is_oam ? DBAL_RESULT_TYPE_OAMP_RMEP_DB_OAM_OFFLOADED : DBAL_RESULT_TYPE_OAMP_RMEP_DB_BFD_OFFLOADED;
    }
    else
    {
        return is_oam ? DBAL_RESULT_TYPE_OAMP_RMEP_DB_OAM_SELF_CONTAINED :
            DBAL_RESULT_TYPE_OAMP_RMEP_DB_BFD_SELF_CONTAINED;
    }
}

/**
 * \brief
 * This function translate period(ccm/bfd interval) to lifetime and lifetime units.
 * Period should be in microseconds.
 */
void
dnx_oam_oamp_period_to_lifetime(
    int unit,
    uint32 period_us,
    uint32 *lifetime,
    uint32 *lifetime_units)
{
    uint32 device_nof_clocks_us;
    uint32 rmep_scan_nof_clocks;
    uint32 rmep_scan_us;

    /** Number of device clocks in 1us */
    device_nof_clocks_us = dnx_data_device.general.core_clock_khz_get(unit) / 1000;
    /** Number of clocks required for full rmep scanning */
    rmep_scan_nof_clocks =
        (2 * dnx_data_oam.oamp.rmep_max_self_contained_get(unit)) * dnx_data_oam.oamp.rmep_sys_clocks_get(unit);
    /** Time required to scan rmep in microseconds */
    rmep_scan_us = rmep_scan_nof_clocks / device_nof_clocks_us;

    /** calculate lifetime and lifetime units */
    if ((period_us / rmep_scan_us) <= 0x3FF)
    {
        *lifetime_units = DBAL_ENUM_FVAL_LIFETIME_UNITS_SCAN_PERIOD_1;
        *lifetime = ((period_us / rmep_scan_us) < 1) ? 1 : (period_us / rmep_scan_us);
    }
    else if (((period_us / 100) / rmep_scan_us) <= 0x3FF)
    {
        *lifetime_units = DBAL_ENUM_FVAL_LIFETIME_UNITS_SCAN_PERIOD_100;
        *lifetime = (((period_us / 100) / rmep_scan_us) < 1) ? 1 : ((period_us / 100) / rmep_scan_us);
    }
    else
    {
        *lifetime_units = DBAL_ENUM_FVAL_LIFETIME_UNITS_SCAN_PERIOD_10000;
        *lifetime =
            (((((period_us / 1000))) / (rmep_scan_us * 10)) < 1) ? 1 : ((((period_us / 1000))) / (rmep_scan_us * 10));
    }
}

/**
 * \brief
 * This function calculate period(ccm/bfd interval) according lifetime and lifetime units.
 * Period will be in microseconds.
 */
static void
dnx_oam_oamp_lifetime_to_period(
    int unit,
    uint32 lifetime,
    uint32 lifetime_units,
    uint32 *period_us)
{
    uint32 device_nof_clocks_us;
    uint32 rmep_scan_nof_clocks;
    uint32 rmep_scan_us;

     /** Number of device clocks in 1us */
    device_nof_clocks_us = dnx_data_device.general.core_clock_khz_get(unit) / 1000;
     /** Number of clocks required for full rmep scanning */
    rmep_scan_nof_clocks =
        (2 * dnx_data_oam.oamp.rmep_max_self_contained_get(unit)) * dnx_data_oam.oamp.rmep_sys_clocks_get(unit);
     /** Time required to scan rmep in microseconds */
    rmep_scan_us = rmep_scan_nof_clocks / device_nof_clocks_us;

     /** Period calculation */
    if (lifetime_units == DBAL_ENUM_FVAL_LIFETIME_UNITS_SCAN_PERIOD_1)
    {
        *period_us = rmep_scan_us * lifetime;
    }
    else
    {
        if (lifetime_units == DBAL_ENUM_FVAL_LIFETIME_UNITS_SCAN_PERIOD_100)
        {
            *period_us = rmep_scan_us * lifetime * 100;
        }
        else
        {
            *period_us = rmep_scan_us * lifetime * 10000;
        }
    }

}

/**
 * \brief
 * This function calculate internal ccm interval value according requested period.
 */
static void
dnx_oam_oamp_ccm_period_to_ccm_interval(
    int ccm_period,
    dbal_enum_value_field_ccm_interval_e * ccm_interval)
{
    switch (ccm_period)
    {
        case BCM_OAM_ENDPOINT_CCM_PERIOD_DISABLED:
            *ccm_interval = DBAL_ENUM_FVAL_CCM_INTERVAL_NO_TX;
            break;
        case BCM_OAM_ENDPOINT_CCM_PERIOD_3MS:
            *ccm_interval = DBAL_ENUM_FVAL_CCM_INTERVAL_3MS;
            break;
        case BCM_OAM_ENDPOINT_CCM_PERIOD_10MS:
            *ccm_interval = DBAL_ENUM_FVAL_CCM_INTERVAL_10MS;
            break;
        case BCM_OAM_ENDPOINT_CCM_PERIOD_100MS:
            *ccm_interval = DBAL_ENUM_FVAL_CCM_INTERVAL_100MS;
            break;
        case BCM_OAM_ENDPOINT_CCM_PERIOD_1S:
            *ccm_interval = DBAL_ENUM_FVAL_CCM_INTERVAL_1S;
            break;
        case BCM_OAM_ENDPOINT_CCM_PERIOD_10S:
            *ccm_interval = DBAL_ENUM_FVAL_CCM_INTERVAL_10S;
            break;
        case BCM_OAM_ENDPOINT_CCM_PERIOD_1M:
            *ccm_interval = DBAL_ENUM_FVAL_CCM_INTERVAL_1M;
            break;
        case BCM_OAM_ENDPOINT_CCM_PERIOD_10M:
            *ccm_interval = DBAL_ENUM_FVAL_CCM_INTERVAL_10M;
            break;
    }
}

/**
 * \brief
 * This function calculate period ccm period value according requested interval.
 */
void
dnx_oam_oamp_ccm_interval_to_ccm_period(
    dbal_enum_value_field_ccm_interval_e ccm_interval,
    int *ccm_period)
{
    switch (ccm_interval)
    {
        case DBAL_ENUM_FVAL_CCM_INTERVAL_NO_TX:
            *ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_DISABLED;
            break;
        case DBAL_ENUM_FVAL_CCM_INTERVAL_3MS:
            *ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_3MS;
            break;
        case DBAL_ENUM_FVAL_CCM_INTERVAL_10MS:
            *ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_10MS;
            break;
        case DBAL_ENUM_FVAL_CCM_INTERVAL_100MS:
            *ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_100MS;
            break;
        case DBAL_ENUM_FVAL_CCM_INTERVAL_1S:
            *ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_1S;
            break;
        case DBAL_ENUM_FVAL_CCM_INTERVAL_10S:
            *ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_10S;
            break;
        case DBAL_ENUM_FVAL_CCM_INTERVAL_1M:
            *ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_1M;
            break;
        case DBAL_ENUM_FVAL_CCM_INTERVAL_10M:
            *ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_10M;
            break;
        default:
            *ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_DISABLED;
            break;
    }
}

/*
 * See h file for description
 */
shr_error_e
dnx_oam_oamp_rmep_index_db_add(
    int unit,
    uint16 oam_id,
    uint16 mep_id,
    uint32 rmep_idx)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_RMEP_INDEX_DB, &entry_handle_id));

    /** Setting key fields: oam-id and mep-id */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_ID, oam_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MEP_ID, mep_id);

    /** Setting result(rmep_idx) */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RMEP_INDEX, INST_SINGLE, rmep_idx);

    /** Commit table values */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See h file for description
 */
shr_error_e
dnx_oam_oamp_rmep_index_db_get(
    int unit,
    uint16 oam_id,
    uint16 mep_id,
    uint32 *rmep_idx)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_RMEP_INDEX_DB, &entry_handle_id));

    /** Setting key fields: oam-id and mep-id */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_ID, oam_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MEP_ID, mep_id);

    /** Setting pointers value to receive the fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_RMEP_INDEX, INST_SINGLE, rmep_idx);

    /** Receive the rmep_idx */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See h file for description
 */
shr_error_e
dnx_oam_oamp_rmep_index_db_remove(
    int unit,
    uint16 oam_id,
    uint16 mep_id)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_RMEP_INDEX_DB, &entry_handle_id));

    /** Setting key fields: oam-id and mep-id */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_ID, oam_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MEP_ID, mep_id);

    /** Clear the entry */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See h file for description
 */
shr_error_e
dnx_oam_oamp_rmep_add(
    int unit,
    uint32 rmep_idx,
    uint32 is_oam,
    uint32 is_update,
    dnx_oam_oamp_rmep_db_entry_t * rmep_entry)
{
    dbal_tables_e rmep_table;
    dbal_entry_action_flags_e commit_type;
    uint32 entry_handle_id;
    dbal_enum_value_result_type_oamp_rmep_db_e rmep_type;
    uint32 lifetime;
    uint32 lifetime_units;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    commit_type = DBAL_COMMIT;
    /** RMEP type calculating */
    rmep_type = dnx_oam_oamp_rmep_type_get(unit, rmep_idx, is_oam);

    /** If RMEP is ofloaded it's mean that static part resides in MDB(DBAL_TABLE_OAMP_RMEP_STATIC_DATA_DB) */
    if ((rmep_type == DBAL_RESULT_TYPE_OAMP_RMEP_DB_OAM_OFFLOADED) ||
        (rmep_type == DBAL_RESULT_TYPE_OAMP_RMEP_DB_BFD_OFFLOADED))
    {
        rmep_table = DBAL_TABLE_OAMP_RMEP_STATIC_DATA_DB;
    }
    else
    {
        rmep_table = DBAL_TABLE_OAMP_RMEP_DB;
    }

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, rmep_table, &entry_handle_id));

    /** Setting key rmep index */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RMEP_INDEX, rmep_idx);

    /** Setting result type */
    if (DBAL_TABLE_OAMP_RMEP_DB == rmep_table)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, rmep_type);
    }

    /** Calculate lifetime and lifetime unis according period */
    dnx_oam_oamp_period_to_lifetime(unit, rmep_entry->period, &lifetime, &lifetime_units);

    /** set lifetime and lifetime units */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LIFETIME, INST_SINGLE, lifetime);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LIFETIME_UNITS, INST_SINGLE, lifetime_units);

    /** set loss of continuty threshold */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LOC_CLEAR_LIMIT, INST_SINGLE,
                                 rmep_entry->loc_clear_threshold);
    /** Loss of continuty enable in case threshold is bigger than 0 */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LOC_CLEAR_ENABLE, INST_SINGLE,
                                 (rmep_entry->loc_clear_threshold == 0) ? 0 : 1);

    /** set punt profile */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PUNT_PROFILE, INST_SINGLE, rmep_entry->punt_profile);
    /** set punt oam_id */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_ID, INST_SINGLE, rmep_entry->oam_id);
    /** set punt good profile */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PUNT_GOOD_PROFILE, INST_SINGLE,
                                 rmep_entry->punt_good_profile);

    /** If rmep entry is offloaded add static part to MDB(DBAL_TABLE_OAMP_RMEP_STATIC_DATA_DB) */
    if ((rmep_type == DBAL_RESULT_TYPE_OAMP_RMEP_DB_OAM_OFFLOADED) ||
        (rmep_type == DBAL_RESULT_TYPE_OAMP_RMEP_DB_BFD_OFFLOADED))
    {
        /** This is an MDB entry */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_FORCE));

        /** clear handle for dynamic part */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_OAMP_RMEP_DB, entry_handle_id));

        /** Setting key rmep_idx */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RMEP_INDEX, rmep_idx);

        /** Set the result type */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, rmep_type);
    }

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAST_LIFETIME_STAMP, INST_SINGLE, 0);
    /** if rmep entry updated,doesn't update VALID bit */
    if (!is_update)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LAST_LIFETIME_STAMP_VALID, INST_SINGLE, 0);
    }

    /** set rmep state according protocol(OAM/BFD) */
    if ((rmep_type == DBAL_RESULT_TYPE_OAMP_RMEP_DB_OAM_SELF_CONTAINED) ||
        (rmep_type == DBAL_RESULT_TYPE_OAMP_RMEP_DB_OAM_OFFLOADED))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ETH_INTERFACE_STATE, INST_SINGLE,
                                     rmep_entry->rmep_state.eth_state.interface_status);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ETH_PORT_STATE, INST_SINGLE,
                                     rmep_entry->rmep_state.eth_state.port_status);
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BFD_FLAGS_PROFILE, INST_SINGLE,
                                     rmep_entry->rmep_state.bfd_state.flags_profile);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BFD_STATE, INST_SINGLE,
                                     rmep_entry->rmep_state.bfd_state.state);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BFD_DIAG_PROFILE, INST_SINGLE,
                                     rmep_entry->rmep_state.bfd_state.diag_profile);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DETECT_MULT, INST_SINGLE,
                                     rmep_entry->rmep_state.bfd_state.detect_multiplier);
    }

    /** add/update rmep entry */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, commit_type));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See h file for description
 */
shr_error_e
dnx_oam_oamp_rmep_get(
    int unit,
    uint32 rmep_idx,
    uint32 is_oam,
    dnx_oam_oamp_rmep_db_entry_t * rmep_entry)
{
    dbal_tables_e rmep_table;
    uint32 entry_handle_id;
    uint32 rmep_type;
    uint32 lifetime;
    uint32 lifetime_units;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** RMEP type calculating */
    rmep_type = dnx_oam_oamp_rmep_type_get(unit, rmep_idx, is_oam);

    /** If RMEP is ofloaded it's mean that static part resides in MDB(DBAL_TABLE_OAMP_RMEP_STATIC_DATA_DB) */
    if ((rmep_type == DBAL_RESULT_TYPE_OAMP_RMEP_DB_OAM_OFFLOADED) ||
        (rmep_type == DBAL_RESULT_TYPE_OAMP_RMEP_DB_BFD_OFFLOADED))
    {
        rmep_table = DBAL_TABLE_OAMP_RMEP_STATIC_DATA_DB;
    }
    else
    {
        rmep_table = DBAL_TABLE_OAMP_RMEP_DB;
    }

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, rmep_table, &entry_handle_id));

    /** Setting key rmep index */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RMEP_INDEX, rmep_idx);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    /** get lifetime and lifetime unit used for period calculation */
    dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_LIFETIME, INST_SINGLE, &lifetime);
    dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_LIFETIME_UNITS, INST_SINGLE, &lifetime_units);

    /** get loss of continuty threshold */
    dbal_entry_handle_value_field8_get(unit, entry_handle_id, DBAL_FIELD_LOC_CLEAR_LIMIT, INST_SINGLE,
                                       &rmep_entry->loc_clear_threshold);

    /** Get punt profile */
    dbal_entry_handle_value_field8_get(unit, entry_handle_id, DBAL_FIELD_PUNT_PROFILE, INST_SINGLE,
                                       &rmep_entry->punt_profile);
    dbal_entry_handle_value_field16_get(unit, entry_handle_id, DBAL_FIELD_OAM_ID, INST_SINGLE, &rmep_entry->oam_id);
    /** Get punt good profile */
    dbal_entry_handle_value_field8_get(unit, entry_handle_id, DBAL_FIELD_PUNT_GOOD_PROFILE, INST_SINGLE,
                                       &rmep_entry->punt_good_profile);

    /** If rmep is offloaded submit MDB(DBAL_TABLE_OAMP_RMEP_STATIC_DATA_DB) and clear handle */
    if ((rmep_type == DBAL_RESULT_TYPE_OAMP_RMEP_DB_OAM_OFFLOADED) ||
        (rmep_type == DBAL_RESULT_TYPE_OAMP_RMEP_DB_BFD_OFFLOADED))
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_OAMP_RMEP_DB, entry_handle_id));

        /** Setting key  rmep_idx */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RMEP_INDEX, rmep_idx);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    }

    /** get rmep state according protocol(OAM/BFD */
    if ((rmep_type == DBAL_RESULT_TYPE_OAMP_RMEP_DB_OAM_SELF_CONTAINED) ||
        (rmep_type == DBAL_RESULT_TYPE_OAMP_RMEP_DB_OAM_OFFLOADED))
    {
        dbal_entry_handle_value_field8_get(unit, entry_handle_id, DBAL_FIELD_ETH_INTERFACE_STATE, INST_SINGLE,
                                           &rmep_entry->rmep_state.eth_state.interface_status);
        dbal_entry_handle_value_field8_get(unit, entry_handle_id, DBAL_FIELD_ETH_PORT_STATE, INST_SINGLE,
                                           &rmep_entry->rmep_state.eth_state.port_status);
    }
    else
    {
        dbal_entry_handle_value_field8_get(unit, entry_handle_id, DBAL_FIELD_BFD_FLAGS_PROFILE, INST_SINGLE,
                                           &rmep_entry->rmep_state.bfd_state.flags_profile);
        dbal_entry_handle_value_field8_get(unit, entry_handle_id, DBAL_FIELD_BFD_STATE, INST_SINGLE,
                                           &rmep_entry->rmep_state.bfd_state.state);
        dbal_entry_handle_value_field8_get(unit, entry_handle_id, DBAL_FIELD_BFD_DIAG_PROFILE, INST_SINGLE,
                                           &rmep_entry->rmep_state.bfd_state.diag_profile);
        dbal_entry_handle_value_field16_get(unit, entry_handle_id, DBAL_FIELD_DETECT_MULT, INST_SINGLE,
                                            &rmep_entry->rmep_state.bfd_state.detect_multiplier);
    }

    /** get rdi recived */
    dbal_entry_handle_value_field8_get(unit, entry_handle_id, DBAL_FIELD_RDI_RECEIVED, INST_SINGLE,
                                       &rmep_entry->rdi_received);
    /** get loss of continuty */
    dbal_entry_handle_value_field8_get(unit, entry_handle_id, DBAL_FIELD_LOC, INST_SINGLE, &rmep_entry->loc);

    /** Calculating period(ccm/bfd interval) */
    dnx_oam_oamp_lifetime_to_period(unit, lifetime, lifetime_units, &rmep_entry->period);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See h file for description
 */
shr_error_e
dnx_oam_oamp_rmep_remove(
    int unit,
    uint32 rmep_idx,
    uint32 is_oam)
{
    dbal_tables_e rmep_table;
    uint32 entry_handle_id;
    uint32 rmep_type;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** RMEP type calculating */
    rmep_type = dnx_oam_oamp_rmep_type_get(unit, rmep_idx, is_oam);

    /** If RMEP is ofloaded it's mean that static part resides in MDB(DBAL_TABLE_OAMP_RMEP_STATIC_DATA_DB) */
    if ((rmep_type == DBAL_RESULT_TYPE_OAMP_RMEP_DB_OAM_OFFLOADED) ||
        (rmep_type == DBAL_RESULT_TYPE_OAMP_RMEP_DB_BFD_OFFLOADED))
    {
        rmep_table = DBAL_TABLE_OAMP_RMEP_STATIC_DATA_DB;
    }
    else
    {
        rmep_table = DBAL_TABLE_OAMP_RMEP_DB;
    }

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, rmep_table, &entry_handle_id));

    /** Setting key rmep index */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RMEP_INDEX, rmep_idx);

    /** If RMEP is offloaded remove entry from MDB DBAL_TABLE_OAMP_RMEP_STATIC_DATA_DB */
    if ((rmep_type == DBAL_RESULT_TYPE_OAMP_RMEP_DB_OAM_OFFLOADED) ||
        (rmep_type == DBAL_RESULT_TYPE_OAMP_RMEP_DB_BFD_OFFLOADED))
    {
        /** Clear the entry */
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

        /** Prepare handler for dynamic part */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_OAMP_RMEP_DB, entry_handle_id));

        /** Setting rmep_idx */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RMEP_INDEX, rmep_idx);
    }

    /** Clear RMEP from OAMP_RMEP_DB */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function gets OAMP punt profile from HW.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] profile_id - Punt profile index.
 * \param [out] punt_profile_data - Punt profile data read from HW
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   This function get the memory under PUNT_PROFILE_TBL DBAL table.
 * \see
 *   * None
 */
shr_error_e
dnx_oam_oamp_punt_profile_tbl_get(
    int unit,
    int profile_id,
    dnx_oam_oamp_punt_event_profile_t * punt_profile_data)
{
    uint32 entry_handle_id;
    uint32 punt_rate;
    uint32 punt_enable;
    uint32 rx_state_update_en;
    uint32 profile_scan_state_update_en;
    uint32 mep_rdi_update_loc_en;
    uint32 mep_rdi_update_loc_clear_en;
    uint32 mep_rdi_update_rx_en;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAM_PUNT_PROFILE_TBL, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PUNT_PROFILE, profile_id);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PROFILE_PUNT_RATE, INST_SINGLE, &(punt_rate));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PROFILE_PUNT_ENABLE, INST_SINGLE, &(punt_enable));
    dbal_value_field32_request(unit, entry_handle_id,
                               DBAL_FIELD_PROFILE_RX_STATE_UPDATE_EN, INST_SINGLE, &(rx_state_update_en));
    dbal_value_field32_request(unit, entry_handle_id,
                               DBAL_FIELD_PROFILE_SCAN_STATE_UPDATE_EN, INST_SINGLE, &(profile_scan_state_update_en));
    dbal_value_field32_request(unit, entry_handle_id,
                               DBAL_FIELD_PROFILE_MEP_RDI_UPDATE_LOC_EN, INST_SINGLE, &(mep_rdi_update_loc_en));
    dbal_value_field32_request(unit, entry_handle_id,
                               DBAL_FIELD_PROFILE_MEP_RDI_UPDATE_LOC_CLEAR_EN, INST_SINGLE,
                               &(mep_rdi_update_loc_clear_en));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PROFILE_MEP_RDI_UPDATE_RX_EN, INST_SINGLE,
                               &(mep_rdi_update_rx_en));
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    punt_profile_data->punt_rate = punt_rate;
    punt_profile_data->punt_enable = punt_enable;
    punt_profile_data->rx_state_update_en = rx_state_update_en;
    punt_profile_data->profile_scan_state_update_en = profile_scan_state_update_en;
    punt_profile_data->mep_rdi_update_loc_en = mep_rdi_update_loc_en;
    punt_profile_data->mep_rdi_update_loc_clear_en = mep_rdi_update_loc_clear_en;
    punt_profile_data->mep_rdi_update_rx_en = mep_rdi_update_rx_en;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function writes OAMP punt profile into HW.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] profile_id - Punt profile index.
 * \param [in] punt_profile_data - Punt profile data to be written.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   This function set the memory under PUNT_PROFILE_TBL DBAL table.
 * \see
 *   * None
 */
shr_error_e
dnx_oam_oamp_punt_profile_tbl_set(
    int unit,
    int profile_id,
    const dnx_oam_oamp_punt_event_profile_t * punt_profile_data)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAM_PUNT_PROFILE_TBL, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PUNT_PROFILE, profile_id);

    dbal_entry_value_field32_set(unit, entry_handle_id,
                                 DBAL_FIELD_PROFILE_PUNT_RATE, INST_SINGLE, punt_profile_data->punt_rate);
    dbal_entry_value_field32_set(unit, entry_handle_id,
                                 DBAL_FIELD_PROFILE_PUNT_ENABLE, INST_SINGLE, punt_profile_data->punt_enable);
    dbal_entry_value_field32_set(unit, entry_handle_id,
                                 DBAL_FIELD_PROFILE_RX_STATE_UPDATE_EN, INST_SINGLE,
                                 punt_profile_data->rx_state_update_en);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROFILE_SCAN_STATE_UPDATE_EN, INST_SINGLE,
                                 punt_profile_data->profile_scan_state_update_en);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROFILE_MEP_RDI_UPDATE_LOC_EN, INST_SINGLE,
                                 punt_profile_data->mep_rdi_update_loc_en);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROFILE_MEP_RDI_UPDATE_LOC_CLEAR_EN, INST_SINGLE,
                                 punt_profile_data->mep_rdi_update_loc_clear_en);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PROFILE_MEP_RDI_UPDATE_RX_EN, INST_SINGLE,
                                 punt_profile_data->mep_rdi_update_rx_en);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function sets the OAMP_RSP_TCAM each index
 *          with key, data, mask and validity bit
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] oam_opcode_or_channel - OAM opcode(is_y_1731=0)
 *             or channel(is_y_1731=1) for which response need to be set
 *             { Y.1731 ? 0 (8b),OpCode (8b) : channel (16b) }
 * \param [in] is_y_1731 - is OAM opcode(1) or channel(0)
 *        this MEP type.
 * \param [in] index - Index in TCAM to be written
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_oamp_opcode_response_set(
    int unit,
    uint16 oam_opcode_or_channel,
    uint8 is_y_1731,
    uint8 index)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_OPCODE_RESPONSE_TABLE, &entry_handle_id));

    /** set access ID (index) */
    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, index));

    /*
     * TCAM key programming
     *
     *  The Key to the RSP TCAM is composed of Y-1731 (1b), { Y.1731 ? 0 (8b),OpCode (8b) : channel (16b) }
     *   Macro builds the TCAM key according to the above encoding.
     */

    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_OAM_OPCODE_CHANNEL, oam_opcode_or_channel, 0);
    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_IS_Y1731, is_y_1731, 0);

    /*
     * TCAM result programming
     *
     * The Data in the RSP_TCAM is composed of  Generate-rsp(1), OAM-TS-format (2),  opcode_or_channel_type (16)
     * For OAM-TS format use macro DNX_OAM_RSP_TCAM_OPCODE_TO_OAM_TS_FORMAT
     */

    /*
     * The response message's opcode is always one below of that of the original packet.
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_RSP_OPCODE_CHANNEL, INST_SINGLE,
                                 (oam_opcode_or_channel - 1));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_TS_FORMAT, INST_SINGLE,
                                 DNX_OAM_RSP_TCAM_OPCODE_TO_OAM_TS_FORMAT(oam_opcode_or_channel));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GENERATE_RSP, INST_SINGLE, 1);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESERVED, INST_SINGLE, 0);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, 1);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function initializes OAMP_RSP_TCAM used to generate
 *          response packets for OAM opcodes.
 *
 * \param [in] unit - Number of hardware unit used.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * This function is part of the OAM initialization process,
 *     and is called from dnx_oam_init.
 */
static shr_error_e
dnx_oam_oamp_init_response_table(
    int unit)
{
    uint8 index = 0;
    SHR_FUNC_INIT_VARS(unit);

    /** Response TCAM is prgrammed for LBM, LTM and EXM Y_1731 PDU types */
    SHR_IF_ERR_EXIT(dnx_oam_oamp_opcode_response_set(unit, DBAL_DEFINE_OAM_OPCODE_LBM, 1, index));
    index++;

    SHR_IF_ERR_EXIT(dnx_oam_oamp_opcode_response_set(unit, DBAL_DEFINE_OAM_OPCODE_LTM, 1, index));
    index++;

    SHR_IF_ERR_EXIT(dnx_oam_oamp_opcode_response_set(unit, DBAL_DEFINE_OAM_OPCODE_EXM, 1, index));
    index++;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function finalize the initialization of OAMP registers and
            enable scan and transmit.
 *
 * \param [in] unit - Number of hardware unit used.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   This function set the registers that grouped together under OAMP_INIT_OAMP_INIT_GENERAL_CFG DBAL table.
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_oamp_init_general_configuration(
    int unit)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_INIT_GENERAL_CFG, &entry_handle_id));
    /** Enable DMR/LMR respose */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DMR_LMR_RESPONSE_ENABLE, INST_SINGLE, 1);
    /** Enable DMR/LMR respose */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DMR_LMR_RESPONSE_DROP_ENABLE, INST_SINGLE, 1);
    /** Disable punting of good packets */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GOOD_PACKET_PUNT_ENABLE, INST_SINGLE, 0);

    /** Set Report Mode to Normal Mode */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_REPORT_MODE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_REPORT_MODE_NORMAL);
    /** Set number of RMEP's entires to be scanned*/
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NUM_OF_RMEPS, INST_SINGLE,
                                 dnx_data_oam.oamp.oamp_nof_scan_rmep_db_entries_get(unit));
    /** Set system clock */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NUM_CLOCKS_SEC, INST_SINGLE,
                                 dnx_data_device.general.core_clock_khz_get(unit) * 1000);

    

    /** Set Opaque for PTCH header value */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PRT_QUALIFIER, INST_SINGLE,
                                 DBAL_ENUM_FVAL_PRT_QUALIFIER_OAMP_INJECTION);

    /** Enable interrupt messaging for LOC_SET, LOC_CLEAR, RDI_SET, RDI_CLEAR events */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LOC_SET_EVENT_ENABLE, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LOC_CLEAR_EVENT_ENABLE, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RDI_SET_EVENT_ENABLE, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RDI_CLEAR_EVENT_ENABLE, INST_SINGLE, 1);

    /** Update entry */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** Clear handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_OAMP_INIT_GENERAL_CFG, entry_handle_id));

    /** Should be last: Enable Transmit and scanner  machine */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TRANSMIT_ENABLE, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SCANNER_ENABLE, INST_SINGLE, 1);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See h file for description
 */
shr_error_e
dnx_oam_oamp_init_cpu_port_configuration(
    int unit)
{
    bcm_gport_t oamp_gport[2];
    uint32 entry_handle_id;
    int count;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get the OAMP port */
    sal_memset(oamp_gport, 0, sizeof(oamp_gport));
    SHR_IF_ERR_EXIT(bcm_port_internal_get(unit, BCM_PORT_INTERNAL_OAMP, 2, oamp_gport, &count));

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_INIT_CPU_PORT_CFG, &entry_handle_id));

    /** Initialize Traffic Class and Drop Precedence for packets send to CPU*/
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CPU_PORT_TC, INST_SINGLE, 7);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CPU_PORT_DP, INST_SINGLE, 0);

    /** Initialize OAMP Port */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PTCH_PP_SSP, INST_SINGLE,
                                 BCM_GPORT_LOCAL_GET(oamp_gport[0]));

    /** Initialize Opaque */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PRT_QUALIFIER, INST_SINGLE,
                                 DBAL_ENUM_FVAL_PRT_QUALIFIER_OAMP_INJECTION);

    /** Set FHEI size */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PUNT_PPH_FHEI_SIZE, INST_SINGLE, OAM_PUNT_FHEI_SIZE);

    /**Submit entry */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function configures RMEP_DB:
 *            1. Get rmep_db threshold soc property and
 *               configure RMEP_DB
 *            2. initialize RMEP access table.
 *
 * \param [in] unit - Number of hardware unit used.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * Registers and memories initialized in this function:
 *   The following register is initialized in this function:
 *       a. OAMP_R_MEP_DB_FULL_ENTRY_THRESHOLD
 *
 *   The following memory is initialized in this function:
 *       b. OAMP_INIT_RMEP_ACCESS_ENABLE_CFG table(OAMP_RMEP_INDEX_ACC)\n
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_oamp_init_rmep_cfg(
    int unit)
{
    uint32 entry_handle_id;
    int rmep_db_threshold;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get soc property */
    rmep_db_threshold = dnx_data_oam.oamp.oamp_rmep_full_entry_threshold_get(unit);
    if ((rmep_db_threshold & 0x1) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "RMEP DB Threshold value should be aligned to full entry. (align 2)");
    }
    /** Take handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_INIT_GENERAL_CFG, &entry_handle_id));
    /** Set rmep threshold */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAMP_RMEP_DB_FULL_ENTRY_THERSHOLD, INST_SINGLE,
                                 rmep_db_threshold);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** Take handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_INIT_RMEP_ACCESS_ENABLE_CFG, &entry_handle_id));
    /** Set enbale */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LOOKUP_ENABLE, INST_SINGLE, 1);

    /** Set CCM Opcode */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_OPCODE, DBAL_DEFINE_OAM_OPCODE_CCM);
    /** Enable to all Ethernet OAM CCM packet access to RMEP Indes table */
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_OAMP_MEP_TYPE,
                                     DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM, DBAL_ENUM_FVAL_OAMP_MEP_TYPE_Y1731_PWE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** Enable for all OPCODES for BFDoIPv4 and BFDoMPLS access to RMEP Index table */
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_OAM_OPCODE, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_OAMP_MEP_TYPE,
                                     DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_1HOP, DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_PWE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_EXIT();
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_oam_oamp_init_opcode_configuration(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function initialize OAMP_INIT_Y1731_GENERAL_CFG table.
 *
 * \param [in] unit - Number of hardware unit used.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * Registers and memories initialized in this function:
 *
 *   The following memory is initialized in this function:
 *       b. OAMP_INIT_Y1731_GENERAL_CFG table\n
 * \see
 *   * None
 *
 */
static shr_error_e
dnx_oam_oamp_init_y1731_cfg(
    int unit)
{
    uint32 entry_handle_id;
    uint32 bos;
    uint32 mpls_label;
    uint32 exp;
    uint32 ttl;
    uint32 mpls = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Take handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_INIT_Y1731_GENERAL_CFG, &entry_handle_id));

    /** Set MPLS header for Y1731oMPLSTPGAL */
    mpls_label = MPLS_LABEL_GAL;
    ttl = MPLS_LABEL_GAL_TTL;
    exp = MPLS_LABEL_GAL_EXP;
    bos = MPLS_LABEL_GAL_BOS;
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode(unit, DBAL_FIELD_Y1731_MPLSTP_GAL, DBAL_FIELD_BOS, &bos, &mpls));
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                    (unit, DBAL_FIELD_Y1731_MPLSTP_GAL, DBAL_FIELD_MPLS_LABEL, &mpls_label, &mpls));
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode(unit, DBAL_FIELD_Y1731_MPLSTP_GAL, DBAL_FIELD_EXP, &exp, &mpls));
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode(unit, DBAL_FIELD_Y1731_MPLSTP_GAL, DBAL_FIELD_TTL, &ttl, &mpls));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_Y1731_MPLSTP_GAL, INST_SINGLE, mpls);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Write default values of BFD PWE CW(0x0007) and BFD GACH CC(0x0022) into indexes 0 and 1 of OAMP_BFD_PWE_CW
     * registers
     */
    SHR_IF_ERR_EXIT(dnx_oam_bfd_pwe_channel_value_set(unit, 0, BFD_PWE_DEFAULT_CHANNEL));
    SHR_IF_ERR_EXIT(dnx_oam_bfd_pwe_channel_value_set(unit, 1, BFD_MPLS_TP_CC_GACH_DEFAULT_CHANNEL));

    /*
     * Write default values of MPLS TP GACH (0x8902) into OAMP_Y_1731O_MPLSTP_GACH register
     */
    SHR_IF_ERR_EXIT(dnx_oam_y1731_mpls_tp_channel_value_set(unit, Y1731_MPLS_TP_GACH_DEFAULT_CHANNEL));

    /*
     * Write default values of PWE GACH (0x0027) into OAMP_Y_1731O_PWE_GACH register
     */
    SHR_IF_ERR_EXIT(dnx_oam_y1731_pwe_channel_value_set(unit, Y1731_PWE_GACH_DEFAULT_CHANNEL));

    SHR_EXIT();
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_oam_oamp_init_mep_db_cfg(
    int unit)
{
    int mep_db_threshold;
    int mep_db_threshold_lim;
    int max_num_of_entries_in_bank;
    int num_of_short_entry_banks;
    int total_num_of_mep_db_banks;
    int num_of_used_mep_db_banks;
    int num_of_clocks_us;
    int ms_multiplier;
    int active_stages;
    int idle_stages;
    int i;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get mep_db_threshold soc property */
    mep_db_threshold = dnx_data_oam.oamp.oamp_mep_full_entry_threshold_get(unit);
    mep_db_threshold_lim = dnx_data_oam.oamp.oamp_mep_db_threshold_version_limitation_get(unit);
    /** Max number of entries per bank*/
    max_num_of_entries_in_bank = dnx_data_oam.oamp.nof_mep_db_entries_per_bank_get(unit) *
        dnx_data_oam.oamp.mep_db_nof_sub_entries_get(unit);

    if (!mep_db_threshold_lim && (mep_db_threshold % max_num_of_entries_in_bank) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "MEP DB Threshold value should be multiple of 2048*4");
    }

    if (mep_db_threshold_lim && ((mep_db_threshold != 0) &&
                                 (mep_db_threshold != dnx_data_oam.oamp.max_nof_endpoint_id_get(unit))))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "MEP DB Threshold should be 0 or 64K.");
    }

    /** Take handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_INIT_GENERAL_CFG, &entry_handle_id));
    /** Set enbale */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAMP_MEP_DB_FULL_ENTRY_THERSHOLD, INST_SINGLE,
                                 (mep_db_threshold >> 2));

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** Take handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_INIT_MEP_DB_BANK_CFG, &entry_handle_id));

    total_num_of_mep_db_banks = dnx_data_oam.oamp.nof_mep_db_entry_banks_get(unit);
    num_of_used_mep_db_banks = dnx_data_oam.oamp.nof_mep_db_entry_banks_for_endpoints_get(unit);

    num_of_short_entry_banks = mep_db_threshold / max_num_of_entries_in_bank;

    num_of_clocks_us = dnx_data_device.general.core_clock_khz_get(unit) / 1000;

    for (i = 0; i < total_num_of_mep_db_banks; i++)
    {
        if (i < num_of_used_mep_db_banks)
        {
            active_stages = 1;
            idle_stages = 0;
            if (i < num_of_short_entry_banks)
            {
                ms_multiplier = 3330;
            }
            else
            {
                ms_multiplier = 1670;
            }
        }
        else
        {
            active_stages = 0;
            idle_stages = 1;
            ms_multiplier = 1670;
        }

        /** Set bank */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MEP_DB_BANK, i);

        /** Set number of active stages */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NUM_ACTIVE_STAGES, INST_SINGLE, active_stages);

        /** Set number of idle stages */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NUM_IDLE_STAGES, INST_SINGLE, idle_stages);

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NUM_CLOCKS_SCAN, INST_SINGLE,
                                     num_of_clocks_us * ms_multiplier);

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_oam_oamp_init_trap_map_cfg(
    int unit)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Take handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_INIT_MAP_OAM_ID, &entry_handle_id));

    dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_TRAP_ID,
                               DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_ETH_OAM);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LOOKUP_ENABLE, INST_SINGLE, TRUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_TRAP_ID,
                               DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_Y1731_O_MPLSTP);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LOOKUP_ENABLE, INST_SINGLE, TRUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_TRAP_ID,
                               DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_Y1731_O_PWE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LOOKUP_ENABLE, INST_SINGLE, TRUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_TRAP_ID,
                               DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_BFD_O_IPV4);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LOOKUP_ENABLE, INST_SINGLE, TRUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_TRAP_ID,
                               DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_BFD_O_IPV6);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LOOKUP_ENABLE, INST_SINGLE, TRUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_TRAP_ID,
                               DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_BFD_O_MPLS);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LOOKUP_ENABLE, INST_SINGLE, TRUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_TRAP_ID,
                               DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_BFD_O_PWE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LOOKUP_ENABLE, INST_SINGLE, TRUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_TRAP_ID,
                               DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_OAM_LEVEL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LOOKUP_ENABLE, INST_SINGLE, TRUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_TRAP_ID,
                               DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_OAM_PASSIVE);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LOOKUP_ENABLE, INST_SINGLE, TRUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_TRAP_ID,
                               DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_NON_ACC_OAM_BFD);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LOOKUP_ENABLE, INST_SINGLE, TRUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * Argument verification for dnx_oam_oamp_rx_trap_codes_set
 */
static shr_error_e
dnx_oam_oamp_rx_trap_codes_set_verify(
    int unit,
    dbal_enum_value_field_oamp_mep_type_e mep_type,
    uint32 trap_code)
{
    SHR_FUNC_INIT_VARS(unit);

    if (mep_type >= DBAL_NOF_ENUM_OAMP_MEP_TYPE_VALUES)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: Illegal MEP type.  Value provided is %d, maximum is %d",
                     mep_type, DBAL_NOF_ENUM_OAMP_MEP_TYPE_VALUES - 1);
    }

    if (trap_code >= DBAL_NOF_ENUM_INGRESS_TRAP_ID_VALUES)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: Illegal trap code.  Value provided is %d, maximum is %d",
                     trap_code, DBAL_NOF_ENUM_INGRESS_TRAP_ID_VALUES - 1);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function sets a single OAM or BFD
 *          trap.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] mep_type - Type of OAM/BFD MEP which
 *        the received packet is targeting.
 * \param [in] trap_code - Trap code to associate with
 *        this MEP type.
 * \param [in] index - index associated with the entry
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_oamp_rx_trap_codes_set(
    int unit,
    dbal_enum_value_field_oamp_mep_type_e mep_type,
    uint32 trap_code,
    uint32 index)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_oam_oamp_rx_trap_codes_set_verify(unit, mep_type, trap_code));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MEP_TYPE_TRAP_CODE_TCAM, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INDEX, index);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_KEY_MEP_TYPE, INST_SINGLE, mep_type);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_KEY_OPCODE_GACH_SELECT, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_KEY_OPCODE_GACH_CHANNEL, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_KEY_RECEIVED_TRAP_CODE, INST_SINGLE, trap_code);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_KEY_MEP_TYPE_MASK, INST_SINGLE,
                                 DEFAULT_MEP_TYPE_MASK);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_KEY_OPCODE_GACH_SELECT_MASK, INST_SINGLE,
                                 DEFAULT_GACH_SEL_MASK);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_KEY_OPCODE_GACH_CHANNEL_MASK, INST_SINGLE,
                                 DEFAULT_GACH_CHAN_MASK);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_KEY_RECEIVED_TRAP_CODE_MASK, INST_SINGLE,
                                 DEFAULT_TRAP_CODE_MASK);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_VALID, INST_SINGLE, TRUE);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function initializes the OAM and BFD
 *          traps.
 *
 * \param [in] unit - Number of hardware unit used.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * This function is part of the OAM initialization process,
 *     and is called from dnx_oam_init.
 */
static shr_error_e
dnx_oam_oamp_init_rx_trap_codes(
    int unit)
{
    uint32 index = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_oam_oamp_rx_trap_codes_set(unit, DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM,
                                                   DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_ETH_OAM, index++));
    SHR_IF_ERR_EXIT(dnx_oam_oamp_rx_trap_codes_set(unit, DBAL_ENUM_FVAL_OAMP_MEP_TYPE_Y1731_MPLSTP,
                                                   DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_Y1731_O_MPLSTP, index++));
    SHR_IF_ERR_EXIT(dnx_oam_oamp_rx_trap_codes_set(unit, DBAL_ENUM_FVAL_OAMP_MEP_TYPE_Y1731_PWE,
                                                   DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_Y1731_O_PWE, index++));
    SHR_IF_ERR_EXIT(dnx_oam_oamp_rx_trap_codes_set(unit, DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_1HOP,
                                                   DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_BFD_O_IPV4, index++));
    SHR_IF_ERR_EXIT(dnx_oam_oamp_rx_trap_codes_set(unit, DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_MHOP,
                                                   DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_BFD_O_IPV4, index++));
    SHR_IF_ERR_EXIT(dnx_oam_oamp_rx_trap_codes_set(unit, DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_1HOP,
                                                   DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_BFD_O_IPV6, index++));
    SHR_IF_ERR_EXIT(dnx_oam_oamp_rx_trap_codes_set(unit, DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_IPV4_MHOP,
                                                   DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_BFD_O_IPV6, index++));
    SHR_IF_ERR_EXIT(dnx_oam_oamp_rx_trap_codes_set(unit, DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_MPLS,
                                                   DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_BFD_O_MPLS, index++));
    SHR_IF_ERR_EXIT(dnx_oam_oamp_rx_trap_codes_set(unit, DBAL_ENUM_FVAL_OAMP_MEP_TYPE_BFD_PWE,
                                                   DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_BFD_O_PWE, index));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function initializes the OAMP interript
 *          pending event mask.
 *
 * \param [in] unit - Number of hardware unit used.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * This function is part of the OAM initialization process,
 *     and is called from dnx_oam_init.
 */
shr_error_e
dnx_oam_oamp_init_interrupt(
    int unit)
{
    bcm_switch_event_control_t event_control;
    bcm_switch_event_t switch_event = BCM_SWITCH_EVENT_DEVICE_INTERRUPT;
    uint32 enable = 1;

    SHR_FUNC_INIT_VARS(unit);

    /** Force unmask the interrupt first and then set mask  to zero */
    event_control.event_id = _DNX_OAM_GET_OAMP_PENDING_EVENT_BY_DEVICE(unit);
    event_control.action = bcmSwitchEventForceUnmask;
    event_control.index = 0;

    SHR_IF_ERR_EXIT(bcm_dnx_switch_event_control_set(unit, switch_event, event_control, enable));

    event_control.action = bcmSwitchEventMask;

    SHR_IF_ERR_EXIT(bcm_dnx_switch_event_control_set(unit, switch_event, event_control, !enable));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function copies a bit sequence.
 *
 * \param [out] dst_buffer - Pointer to destination.
 * \param [in] dst_offset - Offset from dst_buffer to
 *        start writing, in bits.  For 0, starts from LSB
 *        of the byte to which dst_buffer is pointing.
 * \param [in] src_buffer - Pointer to source.
 *        this MEP type.
 * \param [in] src_offset - Offset from src_buffer to
 *        start reading, in bits.  For 0, starts from LSB
 *        of the byte to which src_buffer is pointing.
 * \param [in] nof_bits_to_copy - Number of bits to copy
 *
 * \return
 *   None
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static void
dnx_oamp_copy_bit_array(
    uint8 *dst_buffer,
    uint32 dst_offset,
    const uint8 *src_buffer,
    uint32 src_offset,
    uint32 nof_bits_to_copy)
{
    int index, bit_val, src_byte_offset, src_bit_offset, src_bit_in_byte_offset;
    int opposite_offset, dst_byte_offset, dst_bit_in_byte_offset;
    src_bit_offset = src_offset;
    for (index = 0; index < nof_bits_to_copy; index++, src_bit_offset++)
    {
        /** In the source, start with the MSB of the first byte */
        src_byte_offset = src_bit_offset >> UTILEX_BIT_BYTE_SHIFT;
        src_bit_in_byte_offset = UTILEX_NOF_BITS_IN_BYTE - 1 -
            UTILEX_GET_BITS_RANGE(src_bit_offset, UTILEX_BIT_BYTE_SHIFT - 1, 0);
        bit_val = UTILEX_GET_BIT(src_buffer[src_byte_offset], src_bit_in_byte_offset);

        /** In the destination, start with the MSB of the last byte */
        opposite_offset = dst_offset + nof_bits_to_copy - 1 - index;
        dst_byte_offset = opposite_offset >> UTILEX_BIT_BYTE_SHIFT;
        dst_bit_in_byte_offset = UTILEX_GET_BITS_RANGE(opposite_offset, UTILEX_BIT_BYTE_SHIFT - 1, 0);

        UTILEX_SET_BIT(dst_buffer[dst_byte_offset], bit_val, dst_bit_in_byte_offset);
    }
}

/**
 * \brief - This function initializes the OAMP TCAM and
 *          mask table for calculating CRC per mep_pe_profile
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] data_length - the length of the data given to the crc calculation
 * \param [in] start_index - the offset of the data from the beginning of the PDU
 * \param [in] mep_pe_profile - MEP PE profile
 * \param [in] crc_mask_index - the entry index of the table OAMP_FLEX_CRC_MASK
 * \param [in] opcode_gach_channel - opcode or gach channel
 * \param [in] opcode_gach_channel_mask
 *
 *
 * \return
 *   None
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_oamp_init_crc_validations(
    int unit,
    uint32 data_length,
    uint32 start_index,
    dbal_enum_value_field_mep_pe_profile_sw_e mep_pe_profile,
    uint8 crc_mask_index,
    uint32 opcode_gach_channel,
    uint32 opcode_gach_channel_mask)
{
    uint8 *value_array8 = NULL, *net_mask = NULL;
    uint8 opcode_gach_select_mask = 0;
    uint32 entry_handle_id, bytes_needed_for_masking, entry_access_id = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** How many bytes do we need if a bit masks a byte? */
    bytes_needed_for_masking = UTILEX_TRANSLATE_BITS_TO_BYTES(data_length);

    SHR_ALLOC_SET_ZERO(value_array8, DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES, "Array for over 32-bit dbal fields",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_ALLOC(net_mask, bytes_needed_for_masking, "Mask with the number of bits needed",
              "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    sal_memset(net_mask, 0xFF, bytes_needed_for_masking);

    /** Configure CRC calculation mask */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_FLEX_CRC_MASK, &entry_handle_id));

    /** Only one mask is needed */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CRC_MASK_INDEX, crc_mask_index);

    /** For this type of mask, 1 means "included" and 0 means "ignored." */

    /** THe CRC is calculated over bytes 10-57 of the PDU, so the first 8 are not included */
    dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_PER_BIT_MASK, INST_SINGLE, value_array8);

    /**
     * The per-byte mask is treated as a 120 bit long number.  The MSB masks byte 10 of the PDU,  (not 9?)
     * and the LSB masks byte 127.  DBAL writes 8-bit arrays from LSB to MSB, so here bits 70-117
     * need to be set.
     */
    dnx_oamp_copy_bit_array(value_array8, 120 - (start_index - 64) / 8 - data_length, net_mask, 0, data_length);
    dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_PER_BYTE_MASK, INST_SINGLE, value_array8);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** Configure CRC calculation TCAMs */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_CLS_FLEX_CRC_TCAM, &entry_handle_id));

    entry_access_id = mep_pe_profile;
    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, entry_access_id));

    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_GACH_CHANNEL,
                                      opcode_gach_channel, opcode_gach_channel_mask);
    if (mep_pe_profile == DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_BFD_IPV6)
    {
        opcode_gach_select_mask = 1;
    }
    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_GACH_SELECT, DNX_OAM_OAMP_OPCODE_SELECT,
                                      opcode_gach_select_mask);
    dbal_entry_key_field32_masked_set(unit, entry_handle_id, DBAL_FIELD_MEP_PE_PROFILE, mep_pe_profile, 0       /* mask 
                                                                                                                 */ );

    /** Use the mask in table entry crc_mask_index and CRC1 in the extra data header */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DAT, INST_SINGLE, crc_mask_index);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, TRUE);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    SHR_FREE(net_mask);
    SHR_FREE(value_array8);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * See prototype definition for function description
 */
shr_error_e
dnx_oam_oamp_init(
    int unit)
{
    int rv = _SHR_E_NONE;
    uint8 crc_mask_index = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** Initialize trap codes */
    SHR_IF_ERR_EXIT(dnx_oam_oamp_init_rx_trap_codes(unit));

    /** Initializing opcode configuration,currently used for AIS*/
    rv = dnx_oam_oamp_init_opcode_configuration(unit);
    SHR_IF_ERR_EXIT(rv);

    /** RMEP Index table initialization */
    rv = dnx_oam_oamp_init_rmep_cfg(unit);
    SHR_IF_ERR_EXIT(rv);

    /** Ethernet  OAM initialization */
    rv = dnx_oam_oamp_init_y1731_cfg(unit);
    SHR_IF_ERR_EXIT(rv);

    /** Configuration of MEP DB banks */
    rv = dnx_oam_oamp_init_mep_db_cfg(unit);
    SHR_IF_ERR_EXIT(rv);

    /** Configuration of enable traps */
    rv = dnx_oam_oamp_init_trap_map_cfg(unit);
    SHR_IF_ERR_EXIT(rv);

    /** Initialization of OAMP response table*/
    rv = dnx_oam_oamp_init_response_table(unit);
    SHR_IF_ERR_EXIT(rv);

    crc_mask_index++;
    /** Initialize 48B MAID CRC */
    SHR_IF_ERR_EXIT(dnx_oam_oamp_init_crc_validations(unit,
                                                      BCM_OAM_GROUP_NAME_LENGTH,
                                                      DNX_OAMP_OAM_GROUP_MAID_OFFSET,
                                                      DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_MAID_48,
                                                      crc_mask_index, DNX_OAM_OAMP_OPCODE_CCM_CCM_LM, 0));

    crc_mask_index++;
    /** Initialize BFD over IPv6 CRC */
    
    SHR_IF_ERR_EXIT(dnx_oam_oamp_init_crc_validations(unit,
                                                      sizeof(bcm_ip6_t),
                                                      (DNX_BFD_BFD_HEADER_SIZE + 2) * UTILEX_NOF_BITS_IN_BYTE,
                                                      DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_BFD_IPV6,
                                                      crc_mask_index, 0, 0xffff));

    /** Initialization of OAMP interrupt*/
    rv = dnx_oam_oamp_init_interrupt(unit);
    SHR_IF_ERR_EXIT(rv);

    /** Final Initialization of OAMP  settings */
    rv = dnx_oam_oamp_init_general_configuration(unit);
    SHR_IF_ERR_EXIT(rv);

    SHR_IF_ERR_EXIT(dnx_oamp_pe_module_init(unit));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition for function description
 */
shr_error_e
dnx_oam_oamp_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function reads a 48B MAID group's
 *          calculated CRC from the SW state table.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] group - ID of group.
 * \param [out] crc - Resulting value..
 *
 * \return
 *   None
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
oam_oamp_get_group_crc(
    int unit,
    bcm_oam_group_t group,
    uint16 *crc)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAM_GROUP_INFO_SW, &entry_handle_id));

    /** Setting key field */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_GROUP_ID, group);

    /** Setting value field */
    dbal_value_field16_request(unit, entry_handle_id, DBAL_FIELD_RX_CRC, INST_SINGLE, crc);

    /** Accessing the table */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oamp_mdb_extra_data_set(
    int unit,
    uint16 oam_id,
    uint8 is_offloaded,
    uint8 is_q_entry,
    uint16 calculated_crc,
    int opcode,
    uint32 nof_data_bits,
    uint8 *data,
    uint8 is_update)
{
    dnx_oam_oamp_mep_db_args_t *oamp_params = NULL;
    int pl_index;
    int extra_data_len = 1;
    int extra_data_hdr_seg_len;
    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC(oamp_params, sizeof(dnx_oam_oamp_mep_db_args_t), "Param struct for creating single entries",
              "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    sal_memset(oamp_params, 0, sizeof(dnx_oam_oamp_mep_db_args_t));
    UTILEX_SET_BIT(oamp_params->flags, is_offloaded, OAMP_MEP_LM_DM_OFFLOADED);
    UTILEX_SET_BIT(oamp_params->flags, is_q_entry, OAMP_MEP_Q_ENTRY);
    oamp_params->mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_EXT_DATA_HDR;
    oamp_params->exclusive.extra_data.crc_val1 = calculated_crc;
    oamp_params->exclusive.extra_data.opcode_bmp = opcode;

    if (nof_data_bits > DNX_OAM_MDB_EXTRA_DATA_HDR_SEG_LEN)
    {
        extra_data_len +=
            (nof_data_bits - DNX_OAM_MDB_EXTRA_DATA_HDR_SEG_LEN - 1) / DNX_OAM_MDB_EXTRA_DATA_PLD_SEG_LEN + 1;
        extra_data_hdr_seg_len = DNX_OAM_MDB_EXTRA_DATA_HDR_SEG_LEN;
    }
    else
    {
        /** Here, the entire buffer will not be filled, so clear it first */
        sal_memset(oamp_params->exclusive.extra_data.data_segment, 0,
                   sizeof(oamp_params->exclusive.extra_data.data_segment));
        extra_data_hdr_seg_len = nof_data_bits;
    }
    dnx_oamp_copy_bit_array(oamp_params->exclusive.extra_data.data_segment, 0, data, 0, extra_data_hdr_seg_len);

    if (is_update)
    {
        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
    }

    /** Add header entry to MDB   */
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_add(unit, oam_id, oamp_params));

    oamp_params->mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_EXT_DATA_PLD;

    /** Add payload entries to MDB   */
    for (pl_index = 1; pl_index < extra_data_len; pl_index++)
    {
        int bits_left = 0, bits_to_copy = 0, start_from = 0;
        sal_memset(oamp_params, 0, DATA_SEG_LEN);

        /**
         * For offloaded/short entry endpoints, extra_data_len is
         * not in use, so we use it as segment index
         */
        oamp_params->exclusive.extra_data.extra_data_len = MDB_EXTRA_DATA_PLD1_ACCESS + pl_index - 1;

        bits_left =
            nof_data_bits - DNX_OAM_MDB_EXTRA_DATA_HDR_SEG_LEN - DNX_OAM_MDB_EXTRA_DATA_PLD_SEG_LEN * (pl_index - 1);
        if (bits_left < DNX_OAM_MDB_EXTRA_DATA_PLD_SEG_LEN)
        {
            bits_to_copy = bits_left;
            start_from = DNX_OAM_MDB_EXTRA_DATA_PLD_SEG_LEN - bits_left;
            /** Here, the entire buffer will not be filled, so clear it first */
            sal_memset(oamp_params->exclusive.extra_data.data_segment, 0,
                       sizeof(oamp_params->exclusive.extra_data.data_segment));
        }
        else
        {
            bits_to_copy = DNX_OAM_MDB_EXTRA_DATA_PLD_SEG_LEN;
        }
        dnx_oamp_copy_bit_array(oamp_params->exclusive.extra_data.data_segment, start_from,
                                data,
                                DNX_OAM_MDB_EXTRA_DATA_HDR_SEG_LEN + (pl_index -
                                                                      1) * DNX_OAM_MDB_EXTRA_DATA_PLD_SEG_LEN,
                                bits_to_copy);
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_add(unit, oam_id, oamp_params));
    }

exit:
    SHR_FREE(oamp_params);
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_oamp_mdb_extra_data_get(
    int unit,
    uint16 oam_id,
    uint32 extra_data_length,
    uint8 *data)
{
    dnx_oam_oamp_mep_db_args_t *oamp_params = NULL;
    uint32 nof_payload_access = 0;
    int ii = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(oamp_params, sizeof(dnx_oam_oamp_mep_db_args_t), "Parameter structure for single entry",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    /** Get  MDB entry */
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_get(unit, oam_id, MDB_EXTRA_DATA_HDR_ACCESS, oamp_params));

    dnx_oamp_copy_bit_array(data, 0, oamp_params->exclusive.extra_data.data_segment, 0,
                            DNX_OAM_MDB_EXTRA_DATA_HDR_SEG_LEN);

    if (extra_data_length > DNX_OAM_MDB_EXTRA_DATA_HDR_SEG_LEN)
    {
        nof_payload_access =
            ((extra_data_length - DNX_OAM_MDB_EXTRA_DATA_HDR_SEG_LEN) / (DNX_OAM_MDB_EXTRA_DATA_PLD_SEG_LEN + 1)) + 1;
    }

    for (ii = 0; ii < nof_payload_access; nof_payload_access++)
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_get
                        (unit, oam_id, MDB_EXTRA_DATA_PLD1_ACCESS + nof_payload_access, oamp_params));

        dnx_oamp_copy_bit_array(data,
                                DNX_OAM_MDB_EXTRA_DATA_HDR_SEG_LEN +
                                (nof_payload_access * DNX_OAM_MDB_EXTRA_DATA_PLD_SEG_LEN),
                                oamp_params->exclusive.extra_data.data_segment, 0, DNX_OAM_MDB_EXTRA_DATA_PLD_SEG_LEN);
    }

exit:
    SHR_FREE(oamp_params);
    SHR_FUNC_EXIT;
}

/**
 * \brief - this function build additional data for Egress Down-MEP injection quarter and offloaded entries
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] out_lif - Out LIF
 * \param [in] vsi - vsi
 * \param [out] extra_data_header - additional data
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */

static shr_error_e
dnx_oam_oamp_downmep_egress_injection_additional_data_set(
    int unit,
    uint32 out_lif,
    uint32 vsi,
    uint8 *extra_data_header)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(extra_data_header, _SHR_E_PARAM, "entry_values");


    extra_data_header[1] = (out_lif >> 16) & 0xff;
    extra_data_header[2] = (out_lif >> 8) & 0xff;
    extra_data_header[3] = out_lif & 0xff;

    extra_data_header[6] = ((vsi << 1) >> 16) & 0xff;
    extra_data_header[7] = ((vsi << 1) >> 8) & 0xff;
    extra_data_header[8] = (vsi << 1) & 0xff;
    extra_data_header[15] = 0x3;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - this function get outLif and vsi from extra data
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] extra_data_header - additional data
 * \param [out] out_lif - Out LIF
 * \param [out] vsi - vsi
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */

static shr_error_e
dnx_oam_oamp_downmep_egress_injection_additional_data_get(
    int unit,
    const uint8 *extra_data_header,
    uint32 *out_lif,
    uint32 *vsi)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(extra_data_header, _SHR_E_PARAM, "entry_values");


    *out_lif = (uint32) (extra_data_header[1] << 16) |
        (uint32) (extra_data_header[2] << 8) | (uint32) extra_data_header[3];

    *vsi = (uint32) ((extra_data_header[6] >> 1) << 16) |
        (uint32) ((extra_data_header[7] >> 1) << 8) | ((extra_data_header[6] << 7) & 0x80) |
        (uint32) ((extra_data_header[8] >> 1) | ((extra_data_header[7] << 7) & 0x80));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition in oam_oamp.h for function description
 */
shr_error_e
dnx_oamp_mep_db_ccm_endpoint_set(
    int unit,
    uint16 oam_id,
    dnx_oam_oamp_ccm_endpoint_t * entry_values)
{
    dnx_oam_oamp_mep_db_args_t *oamp_params = NULL;
    bcm_oam_group_info_t *group_info = NULL;
    uint32 extra_data_nof_bits = 0;
    uint8 *extra_header_data = NULL;
    uint32 index = 0;
    uint8 offloaded, q_entry;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(entry_values, _SHR_E_PARAM, "entry_values");
    SHR_ALLOC_SET_ZERO(oamp_params, sizeof(dnx_oam_oamp_mep_db_args_t), "Param struct for creating single entries",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    offloaded = _SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_CCM_MEP_LM_DM_OFFLOADED);
    q_entry = _SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_CCM_MEP_Q_ENTRY);

    /** Set default values to new entries, but not existing entries */
    if (_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_CCM_MEP_UPDATE))
    {
        if (q_entry != 1)
        {
            /** These values are not relevant for short entries */
            if (offloaded == 1)
            {
                /** Get oam_lmm_da_profile from second MDB entry */
                SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_get(unit, oam_id, MDB_PART_2_ACCESS, oamp_params));
            }

            /**
             * Get flex_lm_dm_ptr, and for self-contained endpoints
             * oam_lmm_da_profile as well from first MEP DB entry
             */
            SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, oam_id, oamp_params));
        }
        else
        {
            /** Short entry.  Get LMM_DA_LSB_PROFILE from MDB entry */
            SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_get(unit, oam_id, MDB_Q_ENTRY_ACCESS, oamp_params));
        }

        /** Leave only these flags; clear everything else */
        oamp_params->flags &= (OAMP_MEP_LM_ENABLE | OAMP_MEP_DM_ENABLE |
                               OAMP_MEP_LM_STAT_ENABLE | OAMP_MEP_DM_STAT_ENABLE);
    }
    else
    {
        oamp_params->exclusive.oam_only.oam_lmm_da_profile = 0;
        oamp_params->flex_lm_dm_ptr = 0;
        oamp_params->flags = 0;
    }

    /** Convert the parameters    */
    oamp_params->mep_type = entry_values->mep_type;

    UTILEX_SET_BIT(oamp_params->flags, _SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_CCM_MEP_UPDATE),
                   OAMP_MEP_UPDATE);
    UTILEX_SET_BIT(oamp_params->flags, _SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_CCM_MEP_LM_DM_OFFLOADED),
                   OAMP_MEP_LM_DM_OFFLOADED);
    UTILEX_SET_BIT(oamp_params->flags, _SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_CCM_MEP_Q_ENTRY),
                   OAMP_MEP_Q_ENTRY);
    UTILEX_SET_BIT(oamp_params->flags, _SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_CCM_MEP_IS_UPMEP),
                   OAMP_MEP_IS_UPMEP);
    UTILEX_SET_BIT(oamp_params->flags, _SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_CCM_MEP_PORT_TLV_EN),
                   OAMP_MEP_PORT_TLV_EN);
    UTILEX_SET_BIT(oamp_params->flags, _SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_CCM_MEP_PORT_TLV_VAL),
                   OAMP_MEP_PORT_TLV_VAL);
    UTILEX_SET_BIT(oamp_params->flags, _SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_CCM_MEP_RDI_FROM_PACKET),
                   OAMP_MEP_RDI_FROM_PACKET);
    UTILEX_SET_BIT(oamp_params->flags, _SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_CCM_MEP_RDI_FROM_SCANNER),
                   OAMP_MEP_RDI_FROM_SCANNER);

    if (_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_CCM_MEP_DESTINATION_IS_FEC))
    {
        oamp_params->fec_id_or_glob_out_lif = entry_values->fec_id_or_glob_out_lif.glob_out_lif;
        UTILEX_SET_BIT(oamp_params->fec_id_or_glob_out_lif, TRUE, FIELD_IS_FEC_ID_FLAG);
    }
    else
    {
        oamp_params->fec_id_or_glob_out_lif = entry_values->fec_id_or_glob_out_lif.glob_out_lif;
    }

    oamp_params->mpls_pwe.label = entry_values->label;

    /**
     * For offloaded and quarter entries vsi will be stored in counter pointer,that not in use for
     * Down-MEP Egress Injection. ucode will zero this field.
     * For self-contained entries whole system header will be part of additional data.
     */
    if ((offloaded != 1) && (q_entry != 1)
        && (_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_CCM_MEP_DOWN_MEP_EGRESS_INJECTION)))
    {
        oamp_params->counter_ptr = entry_values->vsi << 2;
        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_EGRESS_INJECTION);
    }
    else
    {
        oamp_params->counter_ptr = entry_values->counter_ptr;
    }
    oamp_params->exclusive.oam_only.oam_mep_id = entry_values->mep_id;
    oamp_params->dest_sys_port_agr = entry_values->dest_sys_port_agr;
    oamp_params->extra_data_ptr = entry_values->extra_data_header;
    oamp_params->exclusive.oam_only.oam_maid = entry_values->maid;
    oamp_params->exclusive.oam_only.oam_inner_vid = entry_values->inner_vid;
    oamp_params->exclusive.oam_only.oam_outer_vid = entry_values->outer_vid;
    oamp_params->exclusive.oam_only.oam_ccm_interval = entry_values->ccm_interval;
    oamp_params->exclusive.oam_only.oam_sa_mac_msb_profile = entry_values->sa_gen_msb_profile;
    oamp_params->exclusive.oam_only.oam_sa_mac_lsb = entry_values->sa_gen_lsb;

    oamp_params->exclusive.oam_only.oam_icc_index = entry_values->icc_index;
    oamp_params->exclusive.oam_only.oam_mdl = entry_values->mdl;
    oamp_params->exclusive.oam_only.oam_nof_vlan_tags = entry_values->nof_vlan_tags;
    oamp_params->exclusive.oam_only.oam_inner_tpid_index = entry_values->inner_tpid_index;
    oamp_params->exclusive.oam_only.oam_inner_dei_pcp = entry_values->inner_pcp_dei;
    oamp_params->exclusive.oam_only.oam_outer_tpid_index = entry_values->outer_tpid_index;
    oamp_params->exclusive.oam_only.oam_outer_dei_pcp = entry_values->outer_pcp_dei;
    oamp_params->exclusive.oam_only.oam_interface_status_tlv_code = entry_values->interface_status_tlv_code;

    /** PTCH profile is not used. Always use value 0 */
    oamp_params->exclusive.oam_only.oam_prt_qualifier_profile = 0;

    oamp_params->itmh_tc_dp_profile = entry_values->itmh_tc_dp_profile;
    oamp_params->mep_profile = entry_values->mep_profile;
    oamp_params->mep_pe_profile = entry_values->mep_pe_profile;
    if (_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_CCM_MEP_IS_UPMEP))
    {
        oamp_params->unified_port_access.ccm_eth_up_mep_port.pp_port =
            entry_values->unified_port_access.ccm_eth_up_mep_port.pp_port;
        oamp_params->unified_port_access.ccm_eth_up_mep_port.port_core =
            entry_values->unified_port_access.ccm_eth_up_mep_port.port_core;
    }
    else
    {
        oamp_params->unified_port_access.port_profile = entry_values->unified_port_access.port_profile;
    }
    oamp_params->mpls_pwe.push_profile = entry_values->push_profile;
    oamp_params->counter_interface = entry_values->counter_interface;
    oamp_params->crps_core_select = entry_values->crps_core_select;

    /** Is this endpoint in offloaded or 1/4 entry format?   */
    if ((offloaded == 1) || (q_entry == 1))
    {
        /** Both formats have at least one MDB entry.  Add MDB entry  */
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_add(unit, oam_id, oamp_params));
    }

    /** Offloaded format has another MDB entry   */
    if (offloaded == 1)
    {
        /** Add second (smaller) MDB entry   */
        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_Q_ENTRY);
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_add(unit, oam_id, oamp_params));
        UTILEX_SET_BIT(oamp_params->flags, FALSE, OAMP_MEP_Q_ENTRY);
    }

    /** Add MEP DB entry   */
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_add(unit, oam_id, oamp_params));

    /**
     * 48B MAID and Egress Down-Mep Injection for offloaded or 1/4 means 4 extra MDB entries
     */
    SHR_ALLOC_SET_ZERO(extra_header_data, DNX_OAM_OAMP_EXTRA_DATA_MAX_SIZE,
                       "data allocated for extra header", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    if (((offloaded == 1) || (q_entry == 1)) &&
        (_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_CCM_MEP_48B_MAID)
         || _SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_CCM_MEP_DOWN_MEP_EGRESS_INJECTION))
        && !_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_CCM_MEP_UPDATE))
    {
        uint16 calculated_crc = 0;
        SHR_ALLOC_SET_ZERO(group_info, sizeof(bcm_oam_group_info_t), "Group info struct", "%s%s%s\r\n", EMPTY, EMPTY,
                           EMPTY);

        if (_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_CCM_MEP_DOWN_MEP_EGRESS_INJECTION))
        {
            SHR_IF_ERR_EXIT(dnx_oam_oamp_downmep_egress_injection_additional_data_set(unit,
                                                                                      entry_values->fec_id_or_glob_out_lif.glob_out_lif,
                                                                                      entry_values->vsi,
                                                                                      extra_header_data));
        }

        /**
         * Since group name index is not in use, it gets the meaning of
         * "group index"
         */

        if (_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_CCM_MEP_48B_MAID))
        {
            SHR_IF_ERR_EXIT(dnx_oam_group_info_get(unit, entry_values->extra_data_header, group_info));

            SHR_IF_ERR_EXIT(oam_oamp_get_group_crc(unit, entry_values->extra_data_header, &calculated_crc));

            index = DNX_OAM_OAMP_ITMH_EXT_SIZE + DNX_OAM_OAMP_PPH_SIZE;
            sal_memcpy(extra_header_data + index, group_info->name, BCM_OAM_GROUP_NAME_LENGTH);

            extra_data_nof_bits = DNX_OAM_OAMP_ITMH_EXT_SIZE_BITS +
                DNX_OAM_OAMP_PPH_SIZE_BITS + DNX_OAM_OAMP_NOF_MAID_BITS;
        }
        else
        {
            extra_data_nof_bits = DNX_OAM_OAMP_ITMH_EXT_SIZE_BITS + DNX_OAM_OAMP_PPH_SIZE_BITS;
            calculated_crc = 0;
        }

        SHR_IF_ERR_EXIT(dnx_oamp_mdb_extra_data_set
                        (unit, oam_id, offloaded, q_entry, calculated_crc, DBAL_DEFINE_OAM_OPCODE_CCM,
                         extra_data_nof_bits, extra_header_data, FALSE));
    }

exit:
    SHR_FREE(extra_header_data);
    SHR_FREE(group_info);
    SHR_FREE(oamp_params);
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition in oam_oamp.h for function description
 */
shr_error_e
dnx_oamp_mep_db_ccm_endpoint_get(
    int unit,
    uint16 oam_id,
    uint8 is_down_mep_egress_injection,
    dnx_oam_oamp_ccm_endpoint_t * entry_values)
{
    dnx_oam_oamp_mep_db_args_t *oamp_params = NULL;
    uint32 vsi;
    uint32 out_lif;
    uint8 *extra_header_data = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(entry_values, _SHR_E_PARAM, "entry_values");
    SHR_ALLOC_SET_ZERO(oamp_params, sizeof(dnx_oam_oamp_mep_db_args_t), "Param struct for creating single entries",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_ALLOC_SET_ZERO(extra_header_data, OAM_OAMP_EXTRA_DATA_MAX_SIZE, "extra data for Down MEP Egress Injection",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    /** Read MEP DB entry   */
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, oam_id, oamp_params));

    /** Is this a 1/4 entry?   */
    if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_Q_ENTRY))
    {
        /** 1/4 MEP DB entry.  Get MDB entry   */
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_get(unit, oam_id, MDB_Q_ENTRY_ACCESS, oamp_params));
    }

    /** Is an LM/DM offloaded entry?   */
    if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_LM_DM_OFFLOADED))
    {
        /** LM/DM offloaded MEP DB entry.  Get MDB entries   */
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_get(unit, oam_id, MDB_PART_1_ACCESS, oamp_params));
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_get(unit, oam_id, MDB_PART_2_ACCESS, oamp_params));
    }

    if (((UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_Q_ENTRY)) ||
         (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_LM_DM_OFFLOADED))) && is_down_mep_egress_injection)
    {
        if (_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_CCM_MEP_DOWN_MEP_EGRESS_INJECTION))
        {
             /** OutLIf and VSI are resides in first 16 bytes, there are no reason to read the rest of data(maid48 if exist)*/
            SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_get(unit, oam_id, MDB_EXTRA_DATA_HDR_ACCESS, oamp_params));

            /*
             * Get first 63 bits from Header of Extra data 
             */
            dnx_oamp_copy_bit_array(extra_header_data, 0,
                                    oamp_params->exclusive.extra_data.data_segment, 0,
                                    DNX_OAM_MDB_EXTRA_DATA_HDR_SEG_LEN);

            SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_get(unit, oam_id, MDB_EXTRA_DATA_PLD1_ACCESS, oamp_params));

            /*
             * Get next 109 bits from Payload of Extra data 
             */
            dnx_oamp_copy_bit_array(extra_header_data, DNX_OAM_MDB_EXTRA_DATA_HDR_SEG_LEN,
                                    oamp_params->exclusive.extra_data.data_segment, 0,
                                    DNX_OAM_MDB_EXTRA_DATA_PLD_SEG_LEN);

            SHR_IF_ERR_EXIT(dnx_oam_oamp_downmep_egress_injection_additional_data_get(unit,
                                                                                      extra_header_data,
                                                                                      &out_lif, &vsi));
            entry_values->fec_id_or_glob_out_lif.glob_out_lif = out_lif;
            entry_values->vsi = vsi;

        }

    }
    /** Convert parameters   */
    entry_values->mep_type = oamp_params->mep_type;
    entry_values->flags = 0;

    if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_LM_DM_OFFLOADED))
    {
        entry_values->flags |= DNX_OAMP_OAM_CCM_MEP_LM_DM_OFFLOADED;
    }

    if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_Q_ENTRY))
    {
        entry_values->flags |= DNX_OAMP_OAM_CCM_MEP_Q_ENTRY;
    }

    if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_IS_UPMEP))
    {
        entry_values->flags |= DNX_OAMP_OAM_CCM_MEP_IS_UPMEP;
        /** This field is treated differently for up-MEP */
        entry_values->unified_port_access.ccm_eth_up_mep_port.pp_port =
            oamp_params->unified_port_access.ccm_eth_up_mep_port.pp_port;
        entry_values->unified_port_access.ccm_eth_up_mep_port.port_core =
            oamp_params->unified_port_access.ccm_eth_up_mep_port.port_core;
    }
    else
    {
        entry_values->unified_port_access.port_profile = oamp_params->unified_port_access.port_profile;
    }

    if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_RDI_FROM_SCANNER))
    {
        entry_values->flags |= DNX_OAMP_OAM_CCM_MEP_RDI_FROM_SCANNER;
    }

    if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_RDI_FROM_PACKET))
    {
        entry_values->flags |= DNX_OAMP_OAM_CCM_MEP_RDI_FROM_PACKET;
    }

    if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_PORT_TLV_EN))
    {
        entry_values->flags |= DNX_OAMP_OAM_CCM_MEP_PORT_TLV_EN;
    }

    if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_PORT_TLV_VAL))
    {
        entry_values->flags |= DNX_OAMP_OAM_CCM_MEP_PORT_TLV_VAL;
    }

    if (oamp_params->mep_type != DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM)
    {
        /** This field is not in use for ethernet CCM endpoints */
        if (UTILEX_GET_BIT(oamp_params->fec_id_or_glob_out_lif, FIELD_IS_FEC_ID_FLAG) != 0)
        {
            entry_values->flags |= DNX_OAMP_OAM_CCM_MEP_DESTINATION_IS_FEC;
            entry_values->fec_id_or_glob_out_lif.fec_id = oamp_params->fec_id_or_glob_out_lif;
                        /** Clear this bit - it's not part of the field */
            UTILEX_SET_BIT(entry_values->fec_id_or_glob_out_lif.fec_id, 0, FIELD_IS_FEC_ID_FLAG);
        }
        else
        {
            entry_values->fec_id_or_glob_out_lif.glob_out_lif = oamp_params->fec_id_or_glob_out_lif;
        }
    }

    entry_values->label = oamp_params->mpls_pwe.label;
    entry_values->counter_ptr = oamp_params->counter_ptr;
    entry_values->mep_id = oamp_params->exclusive.oam_only.oam_mep_id;
    entry_values->dest_sys_port_agr = oamp_params->dest_sys_port_agr;
    entry_values->extra_data_header = oamp_params->extra_data_ptr;
    entry_values->flex_lm_dm_entry = oamp_params->flex_lm_dm_ptr;
    entry_values->maid = oamp_params->exclusive.oam_only.oam_maid;
    entry_values->inner_vid = oamp_params->exclusive.oam_only.oam_inner_vid;
    entry_values->outer_vid = oamp_params->exclusive.oam_only.oam_outer_vid;
    entry_values->ccm_interval = oamp_params->exclusive.oam_only.oam_ccm_interval;

    entry_values->sa_gen_lsb = oamp_params->exclusive.oam_only.oam_sa_mac_lsb;
    entry_values->sa_gen_msb_profile = oamp_params->exclusive.oam_only.oam_sa_mac_msb_profile;

    entry_values->icc_index = oamp_params->exclusive.oam_only.oam_icc_index;
    entry_values->mdl = oamp_params->exclusive.oam_only.oam_mdl;
    entry_values->nof_vlan_tags = oamp_params->exclusive.oam_only.oam_nof_vlan_tags;
    entry_values->inner_tpid_index = oamp_params->exclusive.oam_only.oam_inner_tpid_index;
    entry_values->inner_pcp_dei = oamp_params->exclusive.oam_only.oam_inner_dei_pcp;
    entry_values->outer_tpid_index = oamp_params->exclusive.oam_only.oam_outer_tpid_index;
    entry_values->outer_pcp_dei = oamp_params->exclusive.oam_only.oam_outer_dei_pcp;
    entry_values->interface_status_tlv_code = oamp_params->exclusive.oam_only.oam_interface_status_tlv_code;
    entry_values->itmh_tc_dp_profile = oamp_params->itmh_tc_dp_profile;
    entry_values->mep_profile = oamp_params->mep_profile;
    entry_values->mep_pe_profile = oamp_params->mep_pe_profile;
    entry_values->push_profile = oamp_params->mpls_pwe.push_profile;
    entry_values->counter_interface = oamp_params->counter_interface;
    entry_values->crps_core_select = oamp_params->crps_core_select;

exit:
    SHR_FREE(extra_header_data);
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition in oam_oamp.h for function description
 */
shr_error_e
dnx_oamp_mep_db_ccm_endpoint_clear(
    int unit,
    uint16 oam_id,
    uint8 extra_data_length)
{
    dnx_oam_oamp_mep_db_args_t *oamp_params = NULL;
    int access_index;
    uint32 nof_extra_data_access;
    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC(oamp_params, sizeof(dnx_oam_oamp_mep_db_args_t),
              "Endpoint data read to find MDB entries before deleting", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, oam_id, oamp_params));

    if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_Q_ENTRY) == 1)
    {
        /** 1/4 MEP DB entry.  Delete MDB entry   */
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_delete(unit, oam_id, MDB_Q_ENTRY_ACCESS));
    }
    else
    {
        if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_LM_DM_OFFLOADED) == 1)
        {
                /** LM/DM OAMP MEP DB entry.  Delete MDB entries   */
            SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_delete(unit, oam_id, MDB_PART_1_ACCESS));
            SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_delete(unit, oam_id, MDB_PART_2_ACCESS));
        }
    }

    if (extra_data_length != 0)
    {
        nof_extra_data_access = 1;

        if ((extra_data_length - DNX_OAM_MDB_EXTRA_DATA_HDR_SEG_LEN) > 63)
        {
            nof_extra_data_access =
                (extra_data_length - DNX_OAM_MDB_EXTRA_DATA_HDR_SEG_LEN) / (DNX_OAM_MDB_EXTRA_DATA_PLD_SEG_LEN + 1) + 2;
        }

        for (access_index = 0; access_index < nof_extra_data_access; access_index++)
        {
            SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_delete(unit, oam_id, access_index + MDB_EXTRA_DATA_HDR_ACCESS));
        }
    }

    /** Clear MEP DB entry   */
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_delete(unit, oam_id));

exit:
    SHR_FREE(oamp_params);
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function modifies the MEP DB and MDB entries of
 *        an offloaded LM/DM accelerated OAM endpoint for the
 *        purpose of activating or modifying the LM
 *        functionality.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] oam_id - ID of entry to add or modify. For full
 *        entry, this must be a multiple of 4.
 * \param [in] entry_values - Structure containing the fields to
 *        write to the target entry.
 * \param [in] oamp_params - A pointer to an allocated structure
 *        that will be used to access the individual MEP DB and
 *        MDB entries.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_oamp_mep_db_lm_offloaded_endpoint_set(
    int unit,
    uint16 oam_id,
    const dnx_oam_oamp_endpoint_lm_params_t * entry_values,
    dnx_oam_oamp_mep_db_args_t * oamp_params)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Update first MDB entry */
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_get(unit, oam_id, MDB_PART_1_ACCESS, oamp_params));
    UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
    oamp_params->mep_profile = entry_values->mep_profile;
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_add(unit, oam_id, oamp_params));

    /** Update second MDB entry   */
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_get(unit, oam_id, MDB_PART_2_ACCESS, oamp_params));
    UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
    UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_Q_ENTRY);
    oamp_params->exclusive.oam_only.oam_lmm_da_profile = entry_values->lmm_dmm_da_profile;
    oamp_params->mep_profile = entry_values->mep_profile;
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_add(unit, oam_id, oamp_params));

    /** For LM/DM entries, LM statistics are always measured */
    if (!_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_LM_MEP_ADD_STAT_ENTRY))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: for LM/DM offloaded MEPs, LM statistics are "
                     "automatically calculated, ADD_STAT_ENTRY flag must be set.\n");
    }

    /** If a second MEP DB entry is needed, create it   */
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, oam_id, oamp_params));
    if (oamp_params->flex_lm_dm_ptr == 0)
    {
        if (entry_values->flex_lm_entry == 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error: the second MEP DB entry has not been created "
                         "yet for this LM/DM offloaded MEP, so the flex_lm_entry must be "
                         "set to the target entry, which cannot be 0.");
        }

        sal_memset(oamp_params, 0, sizeof(dnx_oam_oamp_mep_db_args_t));
        /**
         *  MEP DB entries that have the format "LM/DM offloaded part 2"
         *  have both the flags "offloaded" and "1/4 entry" set.  The MEP
         *  type is not necessary - set to 0x1F automatically.
         */
        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_LM_DM_OFFLOADED);
        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_Q_ENTRY);
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_add(unit, entry_values->flex_lm_entry, oamp_params));
    }

    /** Update the first MEP DB entry   */
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, oam_id, oamp_params));
    UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
    UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_LM_STAT_ENABLE);
    UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_LM_ENABLE);
    if (_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_LM_MEP_ADD_STAT_ENTRY))
    {
        /**
         *  For LM/DM offloaded endpoints, the mandator PART 1 entry can
         *  point to the optional PART 2 entry.
         */
        oamp_params->flex_lm_dm_ptr = OAM_ID_TO_MEP_DB_ENTRY(entry_values->flex_lm_entry);
    }
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_add(unit, oam_id, oamp_params));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function searches the LM/DM entires associated
 *        with a self contained OAM endpoint for a specific type
 *        of the possible four types.  If an entry is found,
 *        that entry number is returned.  If not, the number of
 *        the entry in the next bank after the current last
 *        entry is returned.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] oamp_params - A pointer to an allocated structure
 *        that will be used to access the individual MEP DB and
 *        MDB entries.
 * \param [in,out] search_params - a pointer to a structure
 *        containing input fields specifying the search and
 *        output fields where the results will be written.
 * \param [in] is_find_only - 0 for find&add, 1 for find only
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_oamp_mep_db_find_or_add_lm_dm_entry(
    int unit,
    dnx_oam_oamp_mep_db_args_t * oamp_params,
    dnx_oam_oamp_lm_dm_search_t * search_params,
    int is_find_only)
{
    uint32 lm_dm_entry, next_bank_offset;
    SHR_FUNC_INIT_VARS(unit);

    /**
     *  The difference in OAM_ID between two OAMP MEP DB entries
     *  in the same in-bank offset in consecutive banks - used to
     *  split data between multiple entries for extra data an
     *  LM/DM for self-contained OAM endpoints
     */
    next_bank_offset = MEP_DB_ENTRY_TO_OAM_ID(dnx_data_oam.oamp.nof_mep_db_entries_per_bank_get(unit));

    if (!IS_LM_DM_TYPE(search_params->search_type))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: %d is not an LM type nor a DM TYPE.\n", search_params->search_type);
    }

    search_params->update_entry = FALSE;
    search_params->clear_last_entry = TRUE;
    lm_dm_entry = search_params->first_entry;

    if (lm_dm_entry != 0)
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, lm_dm_entry, oamp_params));
    }

    if ((lm_dm_entry == 0) || !IS_LM_DM_TYPE(oamp_params->mep_type))
    {
        /** The flex_lm_dm field is invalid - no search needed   */
        search_params->resulting_entry = search_params->param_entry;
        search_params->clear_last_entry = FALSE;
    }
    else
    {
        while (TRUE)
        {
            if (oamp_params->mep_type == search_params->search_type)
            {
                search_params->resulting_entry = lm_dm_entry;
                search_params->update_entry = TRUE;
                search_params->clear_last_entry = FALSE;
                break;
            }
            lm_dm_entry += next_bank_offset;
            if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_LAST_ENTRY))
            {
                if (is_find_only == LM_DM_FIND_ONLY)
                {
                    search_params->resulting_entry = lm_dm_entry - next_bank_offset;
                }
                else
                {
                    search_params->resulting_entry = lm_dm_entry;
                }
                break;
            }
            SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, lm_dm_entry, oamp_params));
            if (!IS_LM_DM_TYPE(oamp_params->mep_type))
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error: an entry of type %d found before last entry.\n",
                             oamp_params->mep_type);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function creates or modifies an LM_DB type
 *        entry for a self-contained accelerated OAM endpoint.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] oam_id - ID of entry to add or modify. For full
 *        entry, this must be a multiple of 4.
 * \param [in] entry_values - Structure containing the fields to
 *        write to the target entry.
 * \param [in] oamp_params - A pointer to an allocated structure
 *        that will be used to access the individual MEP DB and
 *        MDB entries.
 * \param [in] update_ptr - indicates whether FLEX_LM_DM_PTR
 *        needs to be set in MEP DB CCM entry.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_oamp_mep_db_self_contained_endpoint_lm_db_set(
    int unit,
    uint16 oam_id,
    dnx_oam_oamp_endpoint_lm_params_t * entry_values,
    dnx_oam_oamp_mep_db_args_t * oamp_params,
    uint8 *update_ptr)
{
    uint16 flex_lm_dm_ptr = 0;
    dnx_oam_oamp_lm_dm_search_t search_params;
    uint32 lm_db_entry, next_bank_offset;
    SHR_FUNC_INIT_VARS(unit);

    /**
     *  The difference in OAM_ID between two OAMP MEP DB entries
     *  in the same in-bank offset in consecutive banks - used to
     *  split data between multiple entries for extra data an
     *  LM/DM for self-contained OAM endpoints
     */
    next_bank_offset = MEP_DB_ENTRY_TO_OAM_ID(dnx_data_oam.oamp.nof_mep_db_entries_per_bank_get(unit));

    /** Save the current pointer to first LM/DM entry */
    flex_lm_dm_ptr = oamp_params->flex_lm_dm_ptr;

    search_params.first_entry = MEP_DB_ENTRY_TO_OAM_ID(flex_lm_dm_ptr);
    search_params.param_entry = entry_values->flex_lm_entry;
    search_params.search_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_LM_DB;
    SHR_IF_ERR_EXIT(dnx_oamp_mep_db_find_or_add_lm_dm_entry(unit, oamp_params, &search_params, LM_DM_FIND_AND_ADD));
    lm_db_entry = search_params.resulting_entry;

    if (search_params.update_entry)
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, lm_db_entry, oamp_params));
        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
    }
    else
    {
        sal_memset(oamp_params, 0, sizeof(dnx_oam_oamp_mep_db_args_t));
        if (flex_lm_dm_ptr == 0)
        {
            /**
             *  DM entry does not exist. New LM_DB entry was added.
             *  Set flex_lm_dm_ptr to point to it
             */
            oamp_params->flex_lm_dm_ptr = OAM_ID_TO_MEP_DB_ENTRY(lm_db_entry);
        }
        else
        {
            /**
             *  DM entry already exists and is first.
             *  Restore the flex_lm_dm_ptr to point to it
             */
            oamp_params->flex_lm_dm_ptr = flex_lm_dm_ptr;
        }

        /**
         *  If this is not an existing entry, it must be the last
         *  entry
         */
        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_LAST_ENTRY);
        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_COPY_LM_STATISTICS);
    }
    oamp_params->mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_LM_DB;
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_add(unit, lm_db_entry, oamp_params));

    if (search_params.clear_last_entry)
    {
        /**
         *  There's a new LM/DM entry in the next bank - this entry
         *  should not be marked "last entry"
         */
        lm_db_entry -= next_bank_offset;
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, lm_db_entry, oamp_params));
        UTILEX_SET_BIT(oamp_params->flags, FALSE, OAMP_MEP_LAST_ENTRY);
        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_add(unit, lm_db_entry, oamp_params));
    }

    /**
     *  If the flex_lm_dm_ptr field was not pointing to an LM/DM
     *  entry, use the parameter provided.
     */
    *update_ptr = !search_params.update_entry && !search_params.clear_last_entry;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function creates or modifies an LM_STAT type
 *        entry for a self-contained accelerated OAM endpoint.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] oam_id - ID of entry to add or modify. For full
 *        entry, this must be a multiple of 4.
 * \param [in] entry_values - Structure containing the fields to
 *        write to the target entry.
 * \param [in] oamp_params - A pointer to an allocated structure
 *        that will be used to access the individual MEP DB and
 *        MDB entries.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_oamp_mep_db_self_contained_endpoint_lm_stat_set(
    int unit,
    uint16 oam_id,
    dnx_oam_oamp_endpoint_lm_params_t * entry_values,
    dnx_oam_oamp_mep_db_args_t * oamp_params)
{
    dnx_oam_oamp_lm_dm_search_t search_params;
    uint32 lm_stat_entry, next_bank_offset;
    SHR_FUNC_INIT_VARS(unit);

    /**
     *  The difference in OAM_ID between two OAMP MEP DB entries
     *  in the same in-bank offset in consecutive banks - used to
     *  split data between multiple entries for extra data an
     *  LM/DM for self-contained OAM endpoints
     */
    next_bank_offset = MEP_DB_ENTRY_TO_OAM_ID(dnx_data_oam.oamp.nof_mep_db_entries_per_bank_get(unit));

    /** LM_DB entry must already exist - don't overwrite it!   */
    search_params.first_entry = MEP_DB_ENTRY_TO_OAM_ID(oamp_params->flex_lm_dm_ptr);
    search_params.param_entry = entry_values->flex_lm_entry;
    search_params.search_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_LM_STAT;
    SHR_IF_ERR_EXIT(dnx_oamp_mep_db_find_or_add_lm_dm_entry(unit, oamp_params, &search_params, LM_DM_FIND_AND_ADD));
    lm_stat_entry = search_params.resulting_entry;

    if (search_params.update_entry)
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, lm_stat_entry, oamp_params));
        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
    }
    else
    {
        sal_memset(oamp_params, 0, sizeof(dnx_oam_oamp_mep_db_args_t));

        /**
         *  If this is not an existing entry, it must be the last
         *  entry
         */
        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_LAST_ENTRY);
        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_COPY_LM_STATISTICS);
    }

    oamp_params->mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_LM_STAT;
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_add(unit, lm_stat_entry, oamp_params));

    if (search_params.clear_last_entry)
    {
        /**
         *  There's a new LM/DM entry in the next bank - this entry
         *  should not be marked "last entry"
         */
        lm_stat_entry -= next_bank_offset;
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, lm_stat_entry, oamp_params));
        UTILEX_SET_BIT(oamp_params->flags, FALSE, OAMP_MEP_LAST_ENTRY);
        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_add(unit, lm_stat_entry, oamp_params));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition in oam_oamp.h for function description
 */
shr_error_e
dnx_oamp_mep_db_lm_endpoint_set(
    int unit,
    uint16 oam_id,
    dnx_oam_oamp_endpoint_lm_params_t * entry_values)
{
    dnx_oam_oamp_mep_db_args_t *oamp_params = NULL;
    uint8 update_ptr = FALSE;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(entry_values, _SHR_E_PARAM, "entry_values");
    SHR_ALLOC(oamp_params, sizeof(dnx_oam_oamp_mep_db_args_t), "Param struct for creating single entries",
              "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    /** First, read the CCM entry   */
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, oam_id, oamp_params));

    /** Update flag is needed to avoid redundant allocation */
    UTILEX_SET_BIT(oamp_params->flags,
                   _SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_LM_MEP_UPDATE), OAMP_MEP_UPDATE);

    /** Is it an offloaded LM/DM entry?   */
    if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_LM_DM_OFFLOADED))
    {
        SHR_IF_ERR_EXIT(dnx_oamp_mep_db_lm_offloaded_endpoint_set(unit, oam_id, entry_values, oamp_params));
    }
    else
    {
        /** Not LM/DM offloaded   */
        if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_Q_ENTRY))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error: this action is not allowed for 1/4 entry endpoints.\n");
        }
        else
        {
            /** Self contained entry  */

            /** Set LM_DB entry   */
            SHR_IF_ERR_EXIT(dnx_oamp_mep_db_self_contained_endpoint_lm_db_set(unit, oam_id, entry_values,
                                                                              oamp_params, &update_ptr));

            if (_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_LM_MEP_ADD_STAT_ENTRY))
            {
                /** Set LM_Stat entry if requested   */

                /** Set the entry   */
                SHR_IF_ERR_EXIT(dnx_oamp_mep_db_self_contained_endpoint_lm_stat_set
                                (unit, oam_id, entry_values, oamp_params));
            }

            /** Set CCM entry   */
            if (update_ptr)
            {
                SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, oam_id, oamp_params));
                oamp_params->flex_lm_dm_ptr = OAM_ID_TO_MEP_DB_ENTRY(entry_values->flex_lm_entry);
                oamp_params->exclusive.oam_only.oam_lmm_da_profile = entry_values->lmm_dmm_da_profile;
                oamp_params->mep_profile = entry_values->mep_profile;
                UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
                SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_add(unit, oam_id, oamp_params));
            }
        }
    }

exit:
    SHR_FREE(oamp_params);
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function reads the LM data for an LM/DM
 *        offloaded OAM endpoint.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] oam_id - ID of entry to add or modify. For LM/DM
 *        offloaded entry, this must be a multiple of 4.
 * \param [out] entry_values - Struct to which retrieved data
 *        will be written.
 * \param [in] oamp_params - Allocated struct used to call
 *        low-level function and read data.
 * \param [out] lm_exists - Whether LM is enabled or not.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_oamp_mep_db_lm_offloaded_endpoint_get(
    int unit,
    uint16 oam_id,
    dnx_oam_oamp_endpoint_lm_params_t * entry_values,
    dnx_oam_oamp_mep_db_args_t * oamp_params,
    uint8 *lm_exists)
{
    uint32 second_hl_entry;
    SHR_FUNC_INIT_VARS(unit);

    /** Read the first MEP DB entry   */
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, oam_id, oamp_params));

    if (!UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_LM_ENABLE) ||
        !UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_LM_STAT_ENABLE))
    {
        *lm_exists = 0;
        SHR_EXIT();
    }

    *lm_exists = 1;
    entry_values->lm_my_tx = oamp_params->exclusive.lm_dm.lm_my_tx;
    entry_values->lm_my_rx = oamp_params->exclusive.lm_dm.lm_my_tx;
    entry_values->lm_peer_tx = oamp_params->exclusive.lm_dm.lm_peer_tx;
    entry_values->lm_peer_rx = oamp_params->exclusive.lm_dm.lm_peer_rx;
    entry_values->last_lm_far = oamp_params->exclusive.lm_dm.last_lm_far;
    entry_values->last_lm_near = oamp_params->exclusive.lm_dm.last_lm_near;
    entry_values->max_lm_far = oamp_params->exclusive.lm_dm.max_lm_far;

    if (oamp_params->flex_lm_dm_ptr != 0)
    {
        second_hl_entry = MEP_DB_ENTRY_TO_OAM_ID(oamp_params->flex_lm_dm_ptr);
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, second_hl_entry, oamp_params));
        if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_LM_DM_OFFLOADED) &&
            UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_Q_ENTRY))
        {
            entry_values->acc_lm_far = oamp_params->exclusive.lm_dm.acc_lm_far;
            entry_values->acc_lm_near = oamp_params->exclusive.lm_dm.acc_lm_near;
            entry_values->max_lm_near = oamp_params->exclusive.lm_dm.max_lm_near;
        }
    }

    /** Read the first MDB entry */
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_get(unit, oam_id, MDB_PART_1_ACCESS, oamp_params));
    entry_values->mep_profile = oamp_params->mep_profile;

    /** Read the second MDB entry */
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_get(unit, oam_id, MDB_PART_2_ACCESS, oamp_params));

    entry_values->lmm_dmm_da_profile = oamp_params->exclusive.oam_only.oam_lmm_da_profile;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function finds the LM_DB entry associated with
 *        a self-contained OAM endpoint and reads its data.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] oam_id - ID of entry to add or modify. For a self
 *        contained entry, this must be a multiple of 4.
 * \param [out] entry_values - Struct to which retrieved data
 *        will be written.
 * \param [in] oamp_params - Allocated struct used to call
 *        low-level function and read data.
 * \param [out] lm_exists - Whether LM is enabled or not
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_oamp_mep_db_self_contained_endpoint_lm_db_get(
    int unit,
    uint16 oam_id,
    dnx_oam_oamp_endpoint_lm_params_t * entry_values,
    dnx_oam_oamp_mep_db_args_t * oamp_params,
    uint8 *lm_exists)
{
    dnx_oam_oamp_lm_dm_search_t search_params;
    uint32 lm_db_entry;
    SHR_FUNC_INIT_VARS(unit);

    /** Find LM_DB entry   */
    search_params.first_entry = MEP_DB_ENTRY_TO_OAM_ID(oamp_params->flex_lm_dm_ptr);
    search_params.param_entry = MEP_DB_ENTRY_TO_OAM_ID(oamp_params->flex_lm_dm_ptr);
    search_params.search_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_LM_DB;
    SHR_IF_ERR_EXIT(dnx_oamp_mep_db_find_or_add_lm_dm_entry(unit, oamp_params, &search_params, LM_DM_FIND_ONLY));
    lm_db_entry = search_params.resulting_entry;

    if (!search_params.update_entry)
    {
        *lm_exists = 0;
        SHR_EXIT();
    }
    *lm_exists = 1;
    entry_values->lm_db_entry_idx = lm_db_entry;

    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, lm_db_entry, oamp_params));

    entry_values->lm_my_tx = oamp_params->exclusive.lm_dm.lm_my_tx;
    entry_values->lm_my_rx = oamp_params->exclusive.lm_dm.lm_my_tx;
    entry_values->lm_peer_tx = oamp_params->exclusive.lm_dm.lm_peer_tx;
    entry_values->lm_peer_rx = oamp_params->exclusive.lm_dm.lm_peer_rx;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function finds the LM_STAT entry associated
 *        with a self-contained OAM endpoint and reads its data.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] oam_id - ID of entry to add or modify. For a self
 *        contained entry, this must be a multiple of 4.
 * \param [out] entry_values - Struct to which retrieved data
 *        will be written.
 * \param [in] oamp_params - Allocated struct used to call
 *        low-level function and read data.
 * \param [out] lm_stat_exists - Whether LM stat is enabled or not.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_oamp_mep_db_self_contained_endpoint_lm_stat_get(
    int unit,
    uint16 oam_id,
    dnx_oam_oamp_endpoint_lm_params_t * entry_values,
    dnx_oam_oamp_mep_db_args_t * oamp_params,
    uint8 *lm_stat_exists)
{
    dnx_oam_oamp_lm_dm_search_t search_params;
    uint32 lm_stat_entry;
    SHR_FUNC_INIT_VARS(unit);

    /** Find LM_DB entry   */
    search_params.first_entry = MEP_DB_ENTRY_TO_OAM_ID(oamp_params->flex_lm_dm_ptr);
    search_params.param_entry = MEP_DB_ENTRY_TO_OAM_ID(oamp_params->flex_lm_dm_ptr);
    search_params.search_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_LM_STAT;
    SHR_IF_ERR_EXIT(dnx_oamp_mep_db_find_or_add_lm_dm_entry(unit, oamp_params, &search_params, LM_DM_FIND_ONLY));
    lm_stat_entry = search_params.resulting_entry;

    *lm_stat_exists = 0;
    if (search_params.update_entry)
    {
        *lm_stat_exists = 1;
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, lm_stat_entry, oamp_params));
        entry_values->lm_stat_entry_idx = lm_stat_entry;

        entry_values->acc_lm_far = oamp_params->exclusive.lm_dm.acc_lm_far;
        entry_values->acc_lm_near = oamp_params->exclusive.lm_dm.acc_lm_near;
        entry_values->last_lm_far = oamp_params->exclusive.lm_dm.last_lm_far;
        entry_values->last_lm_near = oamp_params->exclusive.lm_dm.last_lm_near;
        entry_values->max_lm_far = oamp_params->exclusive.lm_dm.max_lm_far;
        entry_values->max_lm_near = oamp_params->exclusive.lm_dm.max_lm_near;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition in oam_oamp.h for function description
 */
shr_error_e
dnx_oamp_mep_db_lm_endpoint_get(
    int unit,
    uint16 oam_id,
    dnx_oam_oamp_endpoint_lm_params_t * entry_values,
    uint8 *lm_exists,
    uint8 *lm_stat_exists)
{
    dnx_oam_oamp_mep_db_args_t *oamp_params = NULL;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(entry_values, _SHR_E_PARAM, "entry_values");
    SHR_ALLOC_SET_ZERO(oamp_params, sizeof(dnx_oam_oamp_mep_db_args_t), "Param struct for reading single entries",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    /** First, read the CCM entry   */
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, oam_id, oamp_params));

    entry_values->lmm_dmm_da_profile = oamp_params->exclusive.oam_only.oam_lmm_da_profile;
    entry_values->flex_lm_entry = MEP_DB_ENTRY_TO_OAM_ID(oamp_params->flex_lm_dm_ptr);
    entry_values->mep_profile = oamp_params->mep_profile;

    /** Is it an offloaded LM/DM entry?   */
    if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_LM_DM_OFFLOADED))
    {
        SHR_IF_ERR_EXIT(dnx_oamp_mep_db_lm_offloaded_endpoint_get(unit, oam_id, entry_values, oamp_params, lm_exists));
    }
    else
    {
        /** Not LM/DM offloaded   */
        if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_Q_ENTRY))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error: this action is not allowed for 1/4 entry endpoints.\n");
        }
        else
        {
            /** Self contained entry  */

            /** Read the LM_DB entry   */
            SHR_IF_ERR_EXIT(dnx_oamp_mep_db_self_contained_endpoint_lm_db_get
                            (unit, oam_id, entry_values, oamp_params, lm_exists));

            /** Read the LM_STAT entry   */
            SHR_IF_ERR_EXIT(dnx_oamp_mep_db_self_contained_endpoint_lm_stat_get(unit, oam_id, entry_values,
                                                                                oamp_params, lm_stat_exists));
        }
    }

exit:
    SHR_FREE(oamp_params);
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition in oam_oamp.h for function description
 */
shr_error_e
dnx_oamp_mep_db_lm_endpoint_clear(
    int unit,
    uint16 oam_id)
{
    dnx_oam_oamp_lm_dm_search_t search_params;
    dnx_oam_oamp_mep_db_args_t *oamp_params = NULL;
    uint32 dm_stat_entry, flex_lm_dm_entry, second_mep_db_entry, next_bank_offset;
    uint8 dm_entry_found;
    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC(oamp_params, sizeof(dnx_oam_oamp_mep_db_args_t), "Param struct for reading single entries",
              "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    /**
     *  The difference in OAM_ID between two OAMP MEP DB entries
     *  in the same in-bank offset in consecutive banks - used to
     *  split data between multiple entries for extra data and
     *  LM/DM for self-contained OAM endpoints
     */
    next_bank_offset = MEP_DB_ENTRY_TO_OAM_ID(dnx_data_oam.oamp.nof_mep_db_entries_per_bank_get(unit));

    /** First, read the CCM entry   */
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, oam_id, oamp_params));

    /** Is it an offloaded LM/DM entry?   */
    if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_LM_DM_OFFLOADED))
    {
        /** Disable LM and reset all LM statistics  */
        UTILEX_SET_BIT(oamp_params->flags, FALSE, OAMP_MEP_LM_STAT_ENABLE);
        UTILEX_SET_BIT(oamp_params->flags, FALSE, OAMP_MEP_LM_ENABLE);
        UTILEX_SET_BIT(oamp_params->flags, FALSE, OAMP_MEP_RESET_LM_STATISTICS);
        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_add(unit, oam_id, oamp_params));

        /** If second entry exists, reset statistics there as well */
        if (oamp_params->flex_lm_dm_ptr != 0)
        {
            second_mep_db_entry = MEP_DB_ENTRY_TO_OAM_ID(oamp_params->flex_lm_dm_ptr);
            if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_DM_ENABLE))
            {
                        /** DM exists - Reset LM statistics */
                SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, second_mep_db_entry, oamp_params));
                if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_LM_DM_OFFLOADED)
                    && UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_Q_ENTRY))
                {
                   /**
                    * Both the LM/DM offloaded and the Q_ENTRY flags are set.  That means
                    * this entry is this MEP's offloaded_2 type entry, so the additional
                    * statistics that it holds must also be reset.
                    */
                    UTILEX_SET_BIT(oamp_params->flags, FALSE, OAMP_MEP_RESET_LM_STATISTICS);
                    UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
                    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_add(unit, second_mep_db_entry, oamp_params));
                }
            }
            else
            {
                        /** No DM - clear pointer and delete second entry */
                oamp_params->flex_lm_dm_ptr = 0;
                SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_add(unit, oam_id, oamp_params));
                SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_delete(unit, second_mep_db_entry));
            }
        }
    }
    else
    {
        /** Not LM/DM offloaded   */
        if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_Q_ENTRY))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error: this action is not allowed for 1/4 entry endpoints.\n");
        }
        else
        {
            /** Self contained entry  */
            flex_lm_dm_entry = MEP_DB_ENTRY_TO_OAM_ID(oamp_params->flex_lm_dm_ptr);

            /** Is there a one-way statistics DM entry?   */
            search_params.first_entry = flex_lm_dm_entry;
            search_params.param_entry = flex_lm_dm_entry;
            search_params.search_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_DM_STAT_ONE_WAY;
            SHR_IF_ERR_EXIT(dnx_oamp_mep_db_find_or_add_lm_dm_entry
                            (unit, oamp_params, &search_params, LM_DM_FIND_ONLY));
            dm_stat_entry = search_params.resulting_entry;

            if (!search_params.update_entry)
            {
                /** No.  Is there two-way statistics DM entry?   */
                search_params.first_entry = flex_lm_dm_entry;
                search_params.param_entry = flex_lm_dm_entry;
                search_params.search_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_DM_STAT;
                SHR_IF_ERR_EXIT(dnx_oamp_mep_db_find_or_add_lm_dm_entry
                                (unit, oamp_params, &search_params, LM_DM_FIND_ONLY));
                dm_stat_entry = search_params.resulting_entry;
            }

            dm_entry_found = search_params.update_entry;

            /** Is there an LM_STAT entry?  */
            search_params.first_entry = flex_lm_dm_entry;
            search_params.param_entry = flex_lm_dm_entry;
            search_params.search_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_LM_STAT;
            SHR_IF_ERR_EXIT(dnx_oamp_mep_db_find_or_add_lm_dm_entry
                            (unit, oamp_params, &search_params, LM_DM_FIND_ONLY));

            if (dm_entry_found)
            {
                /**
                 *  DM entry found.  This entry should be at index
                 *  flex_lm_dm_entry
                 */
                SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, dm_stat_entry, oamp_params));
                UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_LAST_ENTRY);
                UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
                UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_COPY_DM_STATISTICS);
                SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_add(unit, flex_lm_dm_entry, oamp_params));
                if (flex_lm_dm_entry != dm_stat_entry)
                {
                    /** If the DM entry is not the first, delete the old entry */
                    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_delete(unit, dm_stat_entry));
                }
                else
                {
                    /**
                     *  The DM entry is first. Delete the LM_DB entry in next bank.
                     *  Note: LM_DB entry ID is already released
                     */
                    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_delete(unit, dm_stat_entry + next_bank_offset));
                }
            }
            else
            {
                /** DM entry not found.  Clear the pointer and delete it.   */
                sal_memset(oamp_params, 0, sizeof(dnx_oam_oamp_mep_db_args_t));
                SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, oam_id, oamp_params));
                oamp_params->flex_lm_dm_ptr = 0;
                UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
                SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_add(unit, oam_id, oamp_params));
                SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_delete(unit, flex_lm_dm_entry));
            }

            if (search_params.update_entry)
            {
                /** LM_STAT entry was found earlier,  Delete it */
                SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_delete(unit, search_params.resulting_entry));
            }
        }
    }

exit:
    SHR_FREE(oamp_params);
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function modifies the MEP DB and MDB entries of
 *        an offloaded LM/DM accelerated OAM endpoint for the
 *        purpose of activating or modifying the DM
 *        functionality.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] oam_id - ID of entry to add or modify. For full
 *        entry, this must be a multiple of 4.
 * \param [in] entry_values - Structure containing the fields to
 *        write to the target entry.
 * \param [in] oamp_params - A pointer to an allocated structure
 *        that will be used to access the individual MEP DB and
 *        MDB entries.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_oamp_mep_db_dm_offloaded_endpoint_set(
    int unit,
    uint16 oam_id,
    dnx_oam_oamp_endpoint_dm_params_t * entry_values,
    dnx_oam_oamp_mep_db_args_t * oamp_params)
{
    uint32 second_mep_db_entry;
    uint8 set_part2_ptr = FALSE;
    SHR_FUNC_INIT_VARS(unit);

    /** For LM/DM offloaded MEPs LM statistics are mandatory */
    if (!_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_DM_MEP_MEASURE_TWO_WAY))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: for LM/DM offloaded MEPs, DM measurement can"
                     "only be two-way, so two way flag must be set.\n");
    }
    /** Update first MDB entry */
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_get(unit, oam_id, MDB_PART_1_ACCESS, oamp_params));
    UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
    oamp_params->mep_profile = entry_values->mep_profile;
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_add(unit, oam_id, oamp_params));

    /** Update second MDB entry   */
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_get(unit, oam_id, MDB_PART_2_ACCESS, oamp_params));
    UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
    UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_Q_ENTRY);
    oamp_params->exclusive.oam_only.oam_lmm_da_profile = entry_values->lmm_dmm_da_profile;
    oamp_params->mep_profile = entry_values->mep_profile;
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_add(unit, oam_id, oamp_params));

    /** If a second MEP DB entry does not exist yet, create it   */

    /** Read the entry to which PART_2_PTR is pointing */
    second_mep_db_entry = MEP_DB_ENTRY_TO_OAM_ID(oamp_params->flex_lm_dm_ptr);
    if (second_mep_db_entry != 0)
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, second_mep_db_entry, oamp_params));
    }
    if ((second_mep_db_entry == 0) ||
        !UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_LM_DM_OFFLOADED) ||
        !UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_Q_ENTRY) ||
        (oamp_params->mep_type != DBAL_ENUM_FVAL_OAMP_MEP_TYPE_OFFLOADED_SECOND_ENTRY))
    {
                /** a second entry does not exist, so create it */
        if (entry_values->flex_dm_entry == 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error: the second MEP DB entry has not been created "
                         "yet for this LM/DM offloaded MEP, so the flex_dm_entry must be"
                         "set to the target entry, which cannot be 0.");
        }

        set_part2_ptr = TRUE;
        sal_memset(oamp_params, 0, sizeof(dnx_oam_oamp_mep_db_args_t));
        oamp_params->mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_OFFLOADED_SECOND_ENTRY;
                /**
         *  This combination of flags indicates an OAMP MEP DB entry of
         *  type LM/DM offloaded part 2
         */
        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_LM_DM_OFFLOADED);
        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_Q_ENTRY);
        /** Reset DM values */
        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_RESET_DM_STATISTICS);
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_add(unit, entry_values->flex_dm_entry, oamp_params));
    }

    /** Update the first MEP DB entry   */
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, oam_id, oamp_params));
    UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
    UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_DM_STAT_ENABLE);
    UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_DM_ENABLE);
    if (set_part2_ptr)
    {
        /**
         *  For LM/DM offloaded endpoints, the mandatory PART 1 entry can
         *  point to the optional PART 2 entry.
         */
        oamp_params->flex_lm_dm_ptr = OAM_ID_TO_MEP_DB_ENTRY(entry_values->flex_dm_entry);
    }

    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_add(unit, oam_id, oamp_params));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition in oam_oamp.h for function description
 */
shr_error_e
dnx_oamp_mep_db_dm_endpoint_set(
    int unit,
    uint16 oam_id,
    dnx_oam_oamp_endpoint_dm_params_t * entry_values)
{
    dnx_oam_oamp_lm_dm_search_t search_params;
    dnx_oam_oamp_mep_db_args_t *oamp_params = NULL;
    uint32 dm_stat_entry, next_bank_offset;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(entry_values, _SHR_E_PARAM, "entry_values");
    SHR_ALLOC(oamp_params, sizeof(dnx_oam_oamp_mep_db_args_t), "Param struct for creating single entries",
              "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    /**
     *  The difference in OAM_ID between two OAMP MEP DB entries
     *  in the same in-bank offset in consecutive banks - used to
     *  split data between multiple entries for extra data an
     *  LM/DM for self-contained OAM endpoints
     */
    next_bank_offset = MEP_DB_ENTRY_TO_OAM_ID(dnx_data_oam.oamp.nof_mep_db_entries_per_bank_get(unit));

    /** First, read the CCM entry   */
    dnx_oam_oamp_mep_db_get(unit, oam_id, oamp_params);

    /** Is it an offloaded LM/DM entry?   */
    if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_LM_DM_OFFLOADED))
    {
        SHR_IF_ERR_EXIT(dnx_oamp_mep_db_dm_offloaded_endpoint_set(unit, oam_id, entry_values, oamp_params));
    }
    else
    {
        /** Not LM/DM offloaded   */
        if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_Q_ENTRY))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error: this action is not allowed for 1/4 entry endpoints.\n");
        }
        else
        {
            /** Self contained entry  */

            /** Validate flags */
            if (_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_DM_MEP_MEASURE_ONE_WAY) ==
                _SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_DM_MEP_MEASURE_TWO_WAY))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Error: Choose either one-way or two-way measurement.\n");
            }

            /** Search for two-way DM stat entry */
            search_params.first_entry = MEP_DB_ENTRY_TO_OAM_ID(oamp_params->flex_lm_dm_ptr);
            search_params.param_entry = entry_values->flex_dm_entry;
            search_params.search_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_DM_STAT;
            SHR_IF_ERR_EXIT(dnx_oamp_mep_db_find_or_add_lm_dm_entry
                            (unit, oamp_params, &search_params, LM_DM_FIND_AND_ADD));
            dm_stat_entry = search_params.resulting_entry;

            if (!search_params.update_entry)
            {
                /** Two way DM stat entry not found.  Search for one-way */
                search_params.search_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_DM_STAT_ONE_WAY;
                SHR_IF_ERR_EXIT(dnx_oamp_mep_db_find_or_add_lm_dm_entry
                                (unit, oamp_params, &search_params, LM_DM_FIND_AND_ADD));
                dm_stat_entry = search_params.resulting_entry;
            }

            /** Write to the resulting entry */
            if (search_params.update_entry)
            {
                SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, dm_stat_entry, oamp_params));
                UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
            }
            else
            {
                sal_memset(oamp_params, 0, sizeof(dnx_oam_oamp_mep_db_args_t));

                /**
                 *  If this is not an existing entry, it must be the last
                 *  entry
                 */
                UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_LAST_ENTRY);

                /** Also, initialize the DM values */
                UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_RESET_DM_STATISTICS);
            }
            if (_SHR_IS_FLAG_SET(entry_values->flags, DNX_OAMP_OAM_DM_MEP_MEASURE_TWO_WAY))
            {
                oamp_params->mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_DM_STAT;
            }
            else
            {
                oamp_params->mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_DM_STAT_ONE_WAY;
            }
            SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_add(unit, dm_stat_entry, oamp_params));

            if (search_params.clear_last_entry)
            {
                 /**
                 *  There's a new LM/DM entry in the next bank - this entry
                 *  should not be marked "last entry"
                 */
                dm_stat_entry -= next_bank_offset;
                SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, dm_stat_entry, oamp_params));
                UTILEX_SET_BIT(oamp_params->flags, FALSE, OAMP_MEP_LAST_ENTRY);
                UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
                SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_add(unit, dm_stat_entry, oamp_params));
            }

            /** Set CCM entry   */
            if (!search_params.update_entry && !search_params.clear_last_entry)
            {
                SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, oam_id, oamp_params));
                oamp_params->flex_lm_dm_ptr = OAM_ID_TO_MEP_DB_ENTRY(entry_values->flex_dm_entry);
                oamp_params->exclusive.oam_only.oam_lmm_da_profile = entry_values->lmm_dmm_da_profile;
                oamp_params->mep_profile = entry_values->mep_profile;
                UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
                SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_add(unit, oam_id, oamp_params));
            }
        }
    }

exit:
    SHR_FREE(oamp_params);
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function reads the DM data for an LM/DM
 *        offloaded OAM endpoint.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] oam_id - ID of entry to add or modify. For LM/DM
 *        offloaded entry, this must be a multiple of 4.
 * \param [out] entry_values - Struct to which retrieved data
 *        will be written.
 * \param [in] oamp_params - Allocated struct used to call
 *        low-level function and read data.
 * \param [out] is_found - Whether dm entry exists or not.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_oamp_mep_db_dm_offloaded_endpoint_get(
    int unit,
    uint16 oam_id,
    dnx_oam_oamp_endpoint_dm_params_t * entry_values,
    dnx_oam_oamp_mep_db_args_t * oamp_params,
    uint8 *is_found)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Read the first MEP DB entry   */
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, oam_id, oamp_params));

    if (!UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_DM_ENABLE) ||
        !UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_DM_STAT_ENABLE))
    {
        *is_found = 0;
        SHR_EXIT();
    }

    *is_found = 1;

    /** Read the second MEP DB entry if it is valid   */
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, MEP_DB_ENTRY_TO_OAM_ID(oamp_params->flex_lm_dm_ptr), oamp_params));

    if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_LM_DM_OFFLOADED) &&
        UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_Q_ENTRY))
    {
        sal_memcpy(entry_values->last_delay, oamp_params->exclusive.lm_dm.last_delay, TWO_WAY_DELAY_SIZE);
        sal_memcpy(entry_values->max_delay, oamp_params->exclusive.lm_dm.max_delay, TWO_WAY_DELAY_SIZE);
        sal_memcpy(entry_values->min_delay, oamp_params->exclusive.lm_dm.min_delay, TWO_WAY_DELAY_SIZE);
        entry_values->flags = DNX_OAMP_OAM_DM_MEP_MEASURE_TWO_WAY;
        entry_values->flex_dm_entry = MEP_DB_ENTRY_TO_OAM_ID(oamp_params->flex_lm_dm_ptr);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error!  This endpoint has no DM entry.\n");
    }

    /** Read the first MDB entry */
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_get(unit, oam_id, MDB_PART_1_ACCESS, oamp_params));
    entry_values->mep_profile = oamp_params->mep_profile;

    /** Read the second MDB entry */
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mdb_get(unit, oam_id, MDB_PART_2_ACCESS, oamp_params));

    entry_values->lmm_dmm_da_profile = oamp_params->exclusive.oam_only.oam_lmm_da_profile;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function finds the DM_STAT entry (one-way or two-way)
 *        associated with a self-contained OAM endpoint and reads its
 *        data.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] oam_id - ID of entry to add or modify. For a self
 *        contained entry, this must be a multiple of 4.
 * \param [out] entry_values - Struct to which retrieved data
 *        will be written.
 * \param [in] oamp_params - Allocated struct used to call
 *        low-level function and read data.
 * \param [out] is_found - Whether dm entry exists or not.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_oamp_mep_db_self_contained_endpoint_dm_stat_get(
    int unit,
    uint16 oam_id,
    dnx_oam_oamp_endpoint_dm_params_t * entry_values,
    dnx_oam_oamp_mep_db_args_t * oamp_params,
    uint8 *is_found)
{
    dnx_oam_oamp_lm_dm_search_t search_params;
    uint32 dm_stat_entry;
    SHR_FUNC_INIT_VARS(unit);

    entry_values->flex_dm_entry = MEP_DB_ENTRY_TO_OAM_ID(oamp_params->flex_lm_dm_ptr);

    /** Find LM_DB entry   */
    search_params.first_entry = MEP_DB_ENTRY_TO_OAM_ID(oamp_params->flex_lm_dm_ptr);
    search_params.param_entry = MEP_DB_ENTRY_TO_OAM_ID(oamp_params->flex_lm_dm_ptr);
    search_params.search_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_DM_STAT;
    SHR_IF_ERR_EXIT(dnx_oamp_mep_db_find_or_add_lm_dm_entry(unit, oamp_params, &search_params, LM_DM_FIND_AND_ADD));
    dm_stat_entry = search_params.resulting_entry;

    *is_found = 0;
    if (search_params.update_entry)
    {
        /** two-way DM stat entry found */
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, dm_stat_entry, oamp_params));
        *is_found = 1;
        entry_values->dm_stat_entry_idx = dm_stat_entry;
        sal_memcpy(entry_values->last_delay, oamp_params->exclusive.lm_dm.last_delay, TWO_WAY_DELAY_SIZE);
        sal_memcpy(entry_values->max_delay, oamp_params->exclusive.lm_dm.max_delay, TWO_WAY_DELAY_SIZE);
        sal_memcpy(entry_values->min_delay, oamp_params->exclusive.lm_dm.min_delay, TWO_WAY_DELAY_SIZE);
        entry_values->flags = DNX_OAMP_OAM_DM_MEP_MEASURE_TWO_WAY;
    }
    else
    {
        search_params.search_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_DM_STAT_ONE_WAY;
        SHR_IF_ERR_EXIT(dnx_oamp_mep_db_find_or_add_lm_dm_entry(unit, oamp_params, &search_params, LM_DM_FIND_ONLY));
        dm_stat_entry = search_params.resulting_entry;
        if (search_params.update_entry)
        {
            *is_found = 1;
            /** one-way DM stat entry found */
            SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, dm_stat_entry, oamp_params));
            entry_values->dm_stat_entry_idx = dm_stat_entry;
            sal_memcpy(entry_values->last_delay_dmm, oamp_params->exclusive.lm_dm.last_delay_dmm, ONE_WAY_DELAY_SIZE);
            sal_memcpy(entry_values->last_delay_dmr, oamp_params->exclusive.lm_dm.last_delay_dmr, ONE_WAY_DELAY_SIZE);
            sal_memcpy(entry_values->max_delay_dmm, oamp_params->exclusive.lm_dm.max_delay_dmm, ONE_WAY_DELAY_SIZE);
            sal_memcpy(entry_values->max_delay_dmr, oamp_params->exclusive.lm_dm.max_delay_dmr, ONE_WAY_DELAY_SIZE);
            sal_memcpy(entry_values->min_delay_dmm, oamp_params->exclusive.lm_dm.min_delay_dmm, ONE_WAY_DELAY_SIZE);
            sal_memcpy(entry_values->min_delay_dmr, oamp_params->exclusive.lm_dm.min_delay_dmr, ONE_WAY_DELAY_SIZE);
            entry_values->flags = DNX_OAMP_OAM_DM_MEP_MEASURE_ONE_WAY;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition in oam_oamp.h for function description
 */
shr_error_e
dnx_oamp_mep_db_dm_endpoint_get(
    int unit,
    uint16 oam_id,
    dnx_oam_oamp_endpoint_dm_params_t * entry_values,
    uint8 *is_found)
{
    dnx_oam_oamp_mep_db_args_t *oamp_params = NULL;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(entry_values, _SHR_E_PARAM, "entry_values");
    SHR_ALLOC(oamp_params, sizeof(dnx_oam_oamp_mep_db_args_t), "Param struct for reading single entries",
              "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    /** First, read the CCM entry   */
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, oam_id, oamp_params));
    entry_values->mep_profile = oamp_params->mep_profile;

    /** Is it an offloaded LM/DM entry?   */
    if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_LM_DM_OFFLOADED))
    {
        SHR_IF_ERR_EXIT(dnx_oamp_mep_db_dm_offloaded_endpoint_get(unit, oam_id, entry_values, oamp_params, is_found));
    }
    else
    {
        /** Not LM/DM offloaded   */
        if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_Q_ENTRY))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error: this action is not allowed for 1/4 entry endpoints.\n");
        }
        else
        {
            /** Self contained entry  */

            /** Read the DM_STAT (one-way or two-way) entry   */
            SHR_IF_ERR_EXIT(dnx_oamp_mep_db_self_contained_endpoint_dm_stat_get(unit, oam_id, entry_values,
                                                                                oamp_params, is_found));
        }
    }

exit:
    SHR_FREE(oamp_params);
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition in oam_oamp.h for function description
 */
shr_error_e
dnx_oamp_mep_db_dm_endpoint_clear(
    int unit,
    uint16 oam_id)
{
    dnx_oam_oamp_lm_dm_search_t search_params;
    dnx_oam_oamp_mep_db_args_t *oamp_params = NULL;
    uint32 lm_db_entry, lm_stat_entry, flex_lm_dm_entry, next_bank_offset;
    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC(oamp_params, sizeof(dnx_oam_oamp_mep_db_args_t), "Param struct for reading single entries",
              "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    /**
     *  The difference in OAM_ID between two OAMP MEP DB entries
     *  in the same in-bank offset in consecutive banks - used to
     *  split data between multiple entries for extra data an
     *  LM/DM for self-contained OAM endpoints
     */
    next_bank_offset = MEP_DB_ENTRY_TO_OAM_ID(dnx_data_oam.oamp.nof_mep_db_entries_per_bank_get(unit));

    /** First, read the CCM entry   */
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, oam_id, oamp_params));

    /** Is it an offloaded LM/DM entry?   */
    if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_LM_DM_OFFLOADED))
    {
        /** Disable DM   */
        UTILEX_SET_BIT(oamp_params->flags, FALSE, OAMP_MEP_DM_STAT_ENABLE);
        UTILEX_SET_BIT(oamp_params->flags, FALSE, OAMP_MEP_DM_ENABLE);
        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_add(unit, oam_id, oamp_params));

        /** If second entry exists, reset DM statistics there */
        if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_LM_ENABLE | OAMP_MEP_LM_STAT_ENABLE))
        {
            if (oamp_params->flex_lm_dm_ptr != 0)
            {
                SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get
                                (unit, MEP_DB_ENTRY_TO_OAM_ID(oamp_params->flex_lm_dm_ptr), oamp_params));
            }

            if ((oamp_params->flex_lm_dm_ptr != 0) &&
                UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_LM_DM_OFFLOADED) &&
                UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_Q_ENTRY))
            {
                UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_RESET_DM_STATISTICS);
                UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
                SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_add
                                (unit, MEP_DB_ENTRY_TO_OAM_ID(oamp_params->flex_lm_dm_ptr), oamp_params));
            }
        }
        else
        {
            if (oamp_params->flex_lm_dm_ptr != 0)
            {
                        /** No LM - delete second entry and clear pointer */
                SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_delete(unit, MEP_DB_ENTRY_TO_OAM_ID(oamp_params->flex_lm_dm_ptr)));
                oamp_params->flex_lm_dm_ptr = 0;
                SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_add(unit, oam_id, oamp_params));
            }
        }
    }
    else
    {
        /** Not LM/DM offloaded   */
        if (UTILEX_GET_BIT(oamp_params->flags, OAMP_MEP_Q_ENTRY))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error: this action is not allowed for 1/4 entry endpoints.\n");
        }
        else
        {
            /** Self contained entry  */
            flex_lm_dm_entry = MEP_DB_ENTRY_TO_OAM_ID(oamp_params->flex_lm_dm_ptr);

            /** Is there an LM_DB entry?   */
            search_params.first_entry = flex_lm_dm_entry;
            search_params.param_entry = flex_lm_dm_entry;
            search_params.search_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_LM_DB;
            SHR_IF_ERR_EXIT(dnx_oamp_mep_db_find_or_add_lm_dm_entry
                            (unit, oamp_params, &search_params, LM_DM_FIND_ONLY));
            lm_db_entry = search_params.resulting_entry;

            if (search_params.update_entry)
            {
                /** Yes.  Is there an LM stat entry?   */
                search_params.first_entry = flex_lm_dm_entry;
                search_params.param_entry = flex_lm_dm_entry;
                search_params.search_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_LM_STAT;
                SHR_IF_ERR_EXIT(dnx_oamp_mep_db_find_or_add_lm_dm_entry
                                (unit, oamp_params, &search_params, LM_DM_FIND_ONLY));
                lm_stat_entry = search_params.resulting_entry;

                if (search_params.update_entry)
                {
                    /** Yes */
                    if (flex_lm_dm_entry == lm_stat_entry)
                    {
                        /**
                         * If LM stat entry is first,
                         * Put LM_DB entry second, and last
                         */
                        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, lm_db_entry, oamp_params));
                        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_LAST_ENTRY);
                        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_COPY_LM_STATISTICS);
                        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
                        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_add
                                        (unit, flex_lm_dm_entry + next_bank_offset, oamp_params));
                    }
                    else
                    {
                        /** Put LM DB entry first */
                        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, lm_db_entry, oamp_params));
                        UTILEX_SET_BIT(oamp_params->flags, FALSE, OAMP_MEP_LAST_ENTRY);
                        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_COPY_LM_STATISTICS);
                        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
                        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_add(unit, flex_lm_dm_entry, oamp_params));

                        /** Put LM stat entry second and last */
                        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, lm_stat_entry, oamp_params));
                        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_LAST_ENTRY);
                        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_COPY_LM_STATISTICS);
                        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
                        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_add
                                        (unit, flex_lm_dm_entry + next_bank_offset, oamp_params));

                        if (flex_lm_dm_entry != lm_db_entry)
                        {
                            /** If the LM_DB entry is not the first, delete the old LM_STAT entry */
                            SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_delete(unit, lm_stat_entry));
                        }
                        else
                        {
                            /**
                             *  The LM_DB entry is first, followed by the LM_STAT entry. Delete the DM entry.
                             *  Note: DM entry ID is already released
                             */
                            SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_delete(unit, lm_stat_entry + next_bank_offset));
                        }
                    }
                }
                else
                {
                    /** No. LM DB entry should be at index flex_lm_dm_entry. */
                    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, lm_db_entry, oamp_params));
                    UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_LAST_ENTRY);
                    UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_COPY_LM_STATISTICS);
                    UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
                    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_add(unit, flex_lm_dm_entry, oamp_params));

                    if (flex_lm_dm_entry != lm_db_entry)
                    {
                        /** If the LM_DB entry is not the first, delete the old entry */
                        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_delete(unit, lm_db_entry));
                    }
                    else
                    {
                        /**
                         *  The LM_DB entry is first. Delete the DM entry.
                         *  Note: DM entry ID is already released
                         */
                        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_delete(unit, lm_db_entry + next_bank_offset));
                    }
                }
            }
            else
            {
                /** No.  If entry at flex_lm_dm_entry is an LM/DM entry, delete it */
                if (flex_lm_dm_entry != 0)
                {
                    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, flex_lm_dm_entry, oamp_params));
                    if (IS_LM_DM_TYPE(oamp_params->mep_type))
                    {
                        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_delete(unit, flex_lm_dm_entry));
                    }

                    /** Clear pointer in the CCM entry */
                    sal_memset(oamp_params, 0, sizeof(dnx_oam_oamp_mep_db_args_t));
                    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, oam_id, oamp_params));
                    UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
                    oamp_params->flex_lm_dm_ptr = 0;
                    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_add(unit, oam_id, oamp_params));
                }
            }
        }
    }

exit:
    SHR_FREE(oamp_params);
    SHR_FUNC_EXIT;
}

/*
 * See h file for description
 */
shr_error_e
dnx_oam_oamp_local_port_to_system_port_set(
    int unit,
    uint16 pp_port_profile,
    uint32 system_port)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_LOCAL_PORT_2_SYSTEM_PORT, &entry_handle_id));

        /** Setting key field */
    dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_LOCAL_PORT_2_SYSTEM_PORT_PROFILE, pp_port_profile);

    /** Setting result(system port) */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SYSTEM_PORT, INST_SINGLE, system_port);

    /** Commit table values */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See h file for description
 */
shr_error_e
dnx_oam_oamp_local_port_to_system_port_get(
    int unit,
    uint16 pp_port_profile,
    uint32 *system_port)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_LOCAL_PORT_2_SYSTEM_PORT, &entry_handle_id));

        /** Setting key field: profile */
    dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_LOCAL_PORT_2_SYSTEM_PORT_PROFILE, pp_port_profile);

    /** Setting pointers value to receive the system_port fields */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SYSTEM_PORT, INST_SINGLE, system_port);

    /** Receive the system_port */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See h file for description
 */
shr_error_e
dnx_oam_oamp_mep_profile_set(
    int unit,
    uint8 profile,
    const dnx_oam_mep_profile_t * mep_profile)
{
    uint32 entry_handle_id;
    dbal_enum_value_field_ccm_interval_e dmm_interval, lmm_interval, opcode_0_interval, opcode_1_interval;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MEP_PROFILE, &entry_handle_id));

    /** Setting profile */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MEP_PROFILE_ID, profile);

    /** Setting result(MEP profile parameters) */
    dnx_oam_oamp_ccm_period_to_ccm_interval(mep_profile->dmm_rate, &dmm_interval);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_DMM_RATE, INST_SINGLE, dmm_interval);
    dnx_oam_oamp_ccm_period_to_ccm_interval(mep_profile->lmm_rate, &lmm_interval);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_LMM_RATE, INST_SINGLE, lmm_interval);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_IS_DUAL_ENDED_LM, INST_SINGLE,
                                mep_profile->piggy_back_lm);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_IS_SLM, INST_SINGLE, mep_profile->slm_lm);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_IS_REPORT_MODE_LM, INST_SINGLE,
                                mep_profile->report_mode_lm);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_IS_REPORT_MODE_DM, INST_SINGLE,
                                mep_profile->report_mode_dm);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_RDI_GEN_METHOD, INST_SINGLE,
                                mep_profile->rdi_gen_method);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_DMM_OFFSET, INST_SINGLE, mep_profile->dmm_offset);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_DMR_OFFSET, INST_SINGLE, mep_profile->dmr_offset);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_LMM_OFFSET, INST_SINGLE, mep_profile->lmm_offset);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_LMR_OFFSET, INST_SINGLE, mep_profile->lmr_offset);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_BIT_MAP, INST_SINGLE,
                                mep_profile->opcode_bit_map);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_DM_MEASUREMENT_TYPE, INST_SINGLE,
                                mep_profile->dm_measurement_type);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CCM_COUNT, INST_SINGLE, mep_profile->ccm_count);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DMM_COUNT, INST_SINGLE, mep_profile->dmm_count);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LMM_COUNT, INST_SINGLE, mep_profile->lmm_count);
    dnx_oam_oamp_ccm_period_to_ccm_interval(mep_profile->opcode_0_rate, &opcode_0_interval);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_X_RATE, 0, opcode_0_interval);
    dnx_oam_oamp_ccm_period_to_ccm_interval(mep_profile->opcode_1_rate, &opcode_1_interval);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_X_RATE, 1, opcode_1_interval);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_X_COUNT, 0, mep_profile->opcode_0_count);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_X_COUNT, 1, mep_profile->opcode_1_count);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DISABLE_MAID_CHECK, INST_SINGLE,
                                 mep_profile->maid_check_dis);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DISABLE_SRC_IP_CHECK, INST_SINGLE,
                                 mep_profile->src_ip_check_dis);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DISABLE_YOUR_DISCR_CHECK, INST_SINGLE,
                                 mep_profile->your_disc_check_dis);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_TX_COUNTER_ENABLE, INST_SINGLE,
                                mep_profile->opcode_tx_statistics_enable);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_RX_COUNTER_ENABLE, INST_SINGLE,
                                mep_profile->opcode_rx_statistics_enable);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_MSK, INST_SINGLE, mep_profile->opcode_mask);
    dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_MEP_ID_SHIFT, INST_SINGLE, mep_profile->mep_id_shift);

    /** Commit table values */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See h file for description
 */
shr_error_e
dnx_oam_oamp_mep_profile_get(
    int unit,
    uint8 profile,
    dnx_oam_mep_profile_t * mep_profile)
{
    uint32 entry_handle_id;
    dbal_enum_value_field_ccm_interval_e dmm_interval, lmm_interval, opcode_0_interval, opcode_1_interval;
    /*
     * For compiler to not complain about incompatible pointer type
     */
    int dmm_rate_temp, lmm_rate_temp, opcode_0_rate_temp, opcode_1_rate_temp;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MEP_PROFILE, &entry_handle_id));

    /** Setting key field: profile */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MEP_PROFILE_ID, profile);

    /** Setting pointers value to receive the MEP profile fields */
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_DMM_RATE, INST_SINGLE, (uint8 *) &dmm_interval);
    dnx_oam_oamp_ccm_interval_to_ccm_period(dmm_interval, &(dmm_rate_temp));
    mep_profile->dmm_rate = dmm_rate_temp;
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_LMM_RATE, INST_SINGLE, (uint8 *) &lmm_interval);
    dnx_oam_oamp_ccm_interval_to_ccm_period(lmm_interval, &(lmm_rate_temp));
    mep_profile->lmm_rate = lmm_rate_temp;
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_IS_DUAL_ENDED_LM, INST_SINGLE,
                              &mep_profile->piggy_back_lm);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_IS_SLM, INST_SINGLE, &mep_profile->slm_lm);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_IS_REPORT_MODE_LM, INST_SINGLE,
                              &mep_profile->report_mode_lm);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_IS_REPORT_MODE_DM, INST_SINGLE,
                              &mep_profile->report_mode_dm);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_RDI_GEN_METHOD, INST_SINGLE,
                              &mep_profile->rdi_gen_method);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_DMM_OFFSET, INST_SINGLE, &mep_profile->dmm_offset);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_DMR_OFFSET, INST_SINGLE, &mep_profile->dmr_offset);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_LMM_OFFSET, INST_SINGLE, &mep_profile->lmm_offset);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_LMR_OFFSET, INST_SINGLE, &mep_profile->lmr_offset);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_OPCODE_BIT_MAP, INST_SINGLE,
                              &mep_profile->opcode_bit_map);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_DM_MEASUREMENT_TYPE, INST_SINGLE,
                              &mep_profile->dm_measurement_type);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CCM_COUNT, INST_SINGLE, &mep_profile->ccm_count);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DMM_COUNT, INST_SINGLE, &mep_profile->dmm_count);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LMM_COUNT, INST_SINGLE, &mep_profile->lmm_count);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_OPCODE_X_RATE, 0, (uint8 *) &opcode_0_interval);
    dnx_oam_oamp_ccm_interval_to_ccm_period(opcode_0_interval, &(opcode_0_rate_temp));
    mep_profile->opcode_0_rate = opcode_0_rate_temp;
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_OPCODE_X_RATE, 1, (uint8 *) &opcode_1_interval);
    dnx_oam_oamp_ccm_interval_to_ccm_period(opcode_1_interval, &(opcode_1_rate_temp));
    mep_profile->opcode_1_rate = opcode_1_rate_temp;
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OPCODE_X_COUNT, 0, &mep_profile->opcode_0_count);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OPCODE_X_COUNT, 1, &mep_profile->opcode_1_count);

    /** Get the MEP profile values */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See h file for description
 */
shr_error_e
dnx_oam_oamp_itmh_priority_profile_set(
    int unit,
    uint8 profile,
    uint8 tc,
    uint8 dp)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_ITMH_PRIORITY_PROFILE, &entry_handle_id));

    /** Setting profile */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ITMH_TC_DP_PROFILE, profile);

    /** Setting result(tc/dp) */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TC, INST_SINGLE, tc);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DP, INST_SINGLE, dp);

    /** Commit table values */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See h file for description
 */
shr_error_e
dnx_oam_oamp_itmh_priority_profile_get(
    int unit,
    uint8 profile,
    uint8 *tc,
    uint8 *dp)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_ITMH_PRIORITY_PROFILE, &entry_handle_id));

    /** Setting key field: profile */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ITMH_TC_DP_PROFILE, profile);

    /** Setting pointers value to receive the tc/dp fields */
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_TC, INST_SINGLE, tc);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_DP, INST_SINGLE, dp);

    /** Receive the tc/dp */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See h file for description
 */
shr_error_e
dnx_oam_oamp_mpls_pwe_exp_ttl_profile_set(
    int unit,
    uint8 profile,
    uint8 ttl,
    uint8 exp)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MPLS_PWE_EXP_TTL_PROFILE, &entry_handle_id));

    /** Setting profile */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MPLS_PUSH_PROFILE, profile);

    /** Setting result(exp/ttl) */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TTL, INST_SINGLE, ttl);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EXP, INST_SINGLE, exp);

    /** Commit table values */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See h file for description
 */
shr_error_e
dnx_oam_oamp_mpls_pwe_exp_ttl_profile_get(
    int unit,
    uint8 profile,
    uint8 *exp,
    uint8 *ttl)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MPLS_PWE_EXP_TTL_PROFILE, &entry_handle_id));

    /** Setting key field: profile */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MPLS_PUSH_PROFILE, profile);

    /** Setting pointers value to receive the ttl/exp fields */
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_TTL, INST_SINGLE, ttl);
    dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_EXP, INST_SINGLE, exp);

    /** Receive the ttl/exp */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See h file for description
 */
shr_error_e
dnx_oam_oamp_tpid_profile_set(
    int unit,
    uint8 profile,
    uint16 tpid)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_OAM_TPID_PROFILE, &entry_handle_id));

    /** Setting profile */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TPID_INDEX, profile);

    /** Setting tpid */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TPID, INST_SINGLE, tpid);

    /** Commit table values */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See h file for description
 */
shr_error_e
dnx_oam_oamp_tpid_profile_get(
    int unit,
    uint8 profile,
    uint16 *tpid)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_OAM_TPID_PROFILE, &entry_handle_id));

    /** Setting key field: profile */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TPID_INDEX, profile);

    /** Setting pointers value to receive the tpid fields */
    dbal_value_field16_request(unit, entry_handle_id, DBAL_FIELD_TPID, INST_SINGLE, tpid);

    /** Receive the tpid */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See h file for description
 */
shr_error_e
dnx_oam_oamp_sa_mac_msb_profile_set(
    int unit,
    uint8 profile,
    bcm_mac_t sa_mac_addr_msb)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_OAM_SA_MAC_MSB_PROFILE, &entry_handle_id));

    /** Setting profile */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SA_GEN_PROFILE, profile);

    /** Setting result 33 bits of mac */
    dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_MAC_MSB, INST_SINGLE, sa_mac_addr_msb);

    /** Commit table values */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See h file for description
 */
shr_error_e
dnx_oam_oamp_sa_mac_msb_profile_get(
    int unit,
    uint8 profile,
    bcm_mac_t sa_mac_addr_msb)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_OAM_SA_MAC_MSB_PROFILE, &entry_handle_id));

    /** Setting key field: profile */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SA_GEN_PROFILE, profile);

    /** Setting pointers value to receive the SA MAC fields */
    dbal_value_field_arr8_request(unit, entry_handle_id, DBAL_FIELD_MAC_MSB, INST_SINGLE, sa_mac_addr_msb);

    /** Receive the SA MAC */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See h file for description
 */
shr_error_e
dnx_oam_oamp_profile_hw_set(
    int unit,
    oam_oamp_profile_hw_data_t * mep_hw_profiles_write_data)
{

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Check if profile for ITMH priority should be set/updated */
    if (mep_hw_profiles_write_data->flags & DNX_OAM_OAMP_PROFILE_ITMH_PRIORITY_FLAG)
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_itmh_priority_profile_set(unit,
                                                               mep_hw_profiles_write_data->itmh_profile,
                                                               mep_hw_profiles_write_data->itmh_priority.tc,
                                                               mep_hw_profiles_write_data->itmh_priority.dp));
    }

    /** Check if MEP profile should be set/updated */
    if (mep_hw_profiles_write_data->flags & DNX_OAM_OAMP_PROFILE_MEP_PROFILE_FLAG)
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_profile_set(unit,
                                                     mep_hw_profiles_write_data->mep_profile_index,
                                                     &mep_hw_profiles_write_data->mep_profile));
    }

    /** Check if profile for Outer TPID should be set/updated */
    if (mep_hw_profiles_write_data->flags & DNX_OAM_OAMP_PROFILE_OUTER_TPID_FLAG)
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_tpid_profile_set(unit,
                                                      mep_hw_profiles_write_data->outer_tpid_index,
                                                      mep_hw_profiles_write_data->outer_tpid));
    }

    /** Check if profile for Inner TPID should be set/updated */
    if (mep_hw_profiles_write_data->flags & DNX_OAM_OAMP_PROFILE_INNER_TPID_FLAG)
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_tpid_profile_set(unit,
                                                      mep_hw_profiles_write_data->inner_tpid_index,
                                                      mep_hw_profiles_write_data->inner_tpid));
    }

    /** Check if profile for Source MAC(MSB) should be set/updated */
    if (mep_hw_profiles_write_data->flags & DNX_OAM_OAMP_PROFILE_SA_MAC_MSB_FLAG)
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_sa_mac_msb_profile_set(unit,
                                                            mep_hw_profiles_write_data->sa_mac_msb_profile,
                                                            mep_hw_profiles_write_data->sa_mac_msb));
    }

    /** Check if profile for exp/ttl should be set/updated */
    if (mep_hw_profiles_write_data->flags & DNX_OAM_OAMP_PROFILE_EXP_TTL_FLAG)
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mpls_pwe_exp_ttl_profile_set(unit,
                                                                  mep_hw_profiles_write_data->mpls_pwe_exp_ttl_profile,
                                                                  mep_hw_profiles_write_data->ttl_exp.ttl,
                                                                  mep_hw_profiles_write_data->ttl_exp.exp));
    }

    /** Check if profile for system port should be set/updated */
    if (mep_hw_profiles_write_data->flags & DNX_OAM_OAMP_PROFILE_LOCL_PORT_TO_SYSTEM_PORT_FLAG)
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_local_port_to_system_port_set(unit,
                                                                   mep_hw_profiles_write_data->pp_port_profile,
                                                                   mep_hw_profiles_write_data->system_port));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See h file for description
 */
shr_error_e
dnx_oam_oamp_endpoint_bcm_mep_type_to_mep_db_mep_type(
    int unit,
    const bcm_oam_endpoint_type_t bcm_mep_type,
    dbal_enum_value_field_oamp_mep_type_e * mep_db_mep_type)
{

    SHR_FUNC_INIT_VARS(unit);

    switch (bcm_mep_type)
    {
        case bcmOAMEndpointTypeEthernet:
            *mep_db_mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM;
            break;
        case bcmOAMEndpointTypeBHHMPLS:
            *mep_db_mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_Y1731_MPLSTP;
            break;
        case bcmOAMEndpointTypeBHHPwe:
            *mep_db_mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_Y1731_PWE;
            break;
        case bcmOAMEndpointTypeBHHPweGAL:
            *mep_db_mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_Y1731_MPLSTP;
            break;
        case bcmOAMEndpointTypeBhhSection:
            *mep_db_mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_Y1731_PWE;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Endpoint create error: MEP type %d is not supported.\n", bcm_mep_type);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function will update ICC index and MA ID
 *          details of endpoint structure based on OAM group info.
 * \param [in] unit - Number of hardware unit used.
 * \param [in] group_name - pointer to  group name.
 * \param [out] mep_db_entry -
 *  Pointer to OAM endpoint structure that need to be updated with group info.
 *  Updated fields are:
 *               icc_index
 *               maid

 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_update_accelarated_endpoint_info_with_group_config(
    int unit,
    const uint8 *group_name,
    dnx_oam_oamp_ccm_endpoint_t * mep_db_entry)
{
    int i;
    int icc_index;
    dnx_oam_group_icc_profile_data_t icc_profile_data;
    dnx_oam_ma_name_type_e group_name_type = BCM_DNX_OAM_GROUP_NAME_TO_NAME_TYPE(group_name);

    SHR_FUNC_INIT_VARS(unit);

    if (group_name_type == DNX_OAMP_OAM_MA_NAME_TYPE_SHORT)
    {
        mep_db_entry->icc_index = DBAL_DEFINE_SHORT_ICC_INDEX_SHORT_FORMAT;
        mep_db_entry->maid =
            group_name[DNX_OAMP_OAM_GROUP_ICC_DATA_OFFSET + 1] + (group_name[DNX_OAMP_OAM_GROUP_ICC_DATA_OFFSET] << 8);
    }
    else if (group_name_type == DNX_OAMP_OAM_MA_NAME_TYPE_ICC)
    {
        for (i = 0; i < DNX_OAMP_OAM_GROUP_ICC_DATA_LENGTH; i++)
        {
            icc_profile_data[i] =
                group_name[DNX_OAMP_OAM_GROUP_ICC_DATA_OFFSET + (DNX_OAMP_OAM_GROUP_ICC_DATA_LENGTH - 1) - i];
        }

        SHR_IF_ERR_EXIT(algo_oam_db.
                        oam_group_icc_profile.profile_get(unit, _SHR_CORE_ALL, icc_profile_data, &icc_index));

        mep_db_entry->icc_index = (uint8) icc_index;

        mep_db_entry->maid =
            group_name[DNX_OAMP_OAM_GROUP_LONG_MA_NAME_DATA_OFFSET + 1] +
            (group_name[DNX_OAMP_OAM_GROUP_LONG_MA_NAME_DATA_OFFSET] << 8);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error:   Error in group name");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set endpoint info of ehernet oam according mep_db
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] mep_db_entry - mep_db
 * \param [out] endpoint_info - return filled endpoint info for accelerated endpoints according mep_db
 *  Updated fields are:
 *               outer_tpid, vlan, pkt_pri,
 *               inner_tpid, inner_vlan, inner_pkt_pri,
 *               src_mac_address
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_oamp_ccm_endpoint_eth_oam_get(
    int unit,
    const dnx_oam_oamp_ccm_endpoint_t * mep_db_entry,
    bcm_oam_endpoint_info_t * endpoint_info)
{
    bcm_mac_t sa_mac_addr_msb;

    SHR_FUNC_INIT_VARS(unit);

    /** Get vlan */
    endpoint_info->outer_tpid = 0;
    endpoint_info->vlan = 0;
    endpoint_info->pkt_pri = 0;
    endpoint_info->inner_tpid = 0;
    endpoint_info->inner_vlan = 0;
    endpoint_info->inner_pkt_pri = 0;

    if (mep_db_entry->nof_vlan_tags == 1)
    {
        endpoint_info->vlan = mep_db_entry->inner_vid;
        endpoint_info->pkt_pri = mep_db_entry->inner_pcp_dei;
        SHR_IF_ERR_EXIT(dnx_oam_oamp_tpid_profile_get(unit,
                                                      mep_db_entry->inner_tpid_index, &endpoint_info->outer_tpid));
    }

    if (mep_db_entry->nof_vlan_tags == 2)
    {
        endpoint_info->inner_vlan = mep_db_entry->inner_vid;
        endpoint_info->inner_pkt_pri = mep_db_entry->inner_pcp_dei;
        SHR_IF_ERR_EXIT(dnx_oam_oamp_tpid_profile_get(unit,
                                                      mep_db_entry->inner_tpid_index, &endpoint_info->inner_tpid));

        endpoint_info->vlan = mep_db_entry->outer_vid;
        endpoint_info->pkt_pri = mep_db_entry->outer_pcp_dei;
        SHR_IF_ERR_EXIT(dnx_oam_oamp_tpid_profile_get(unit,
                                                      mep_db_entry->outer_tpid_index, &endpoint_info->outer_tpid));
    }

    /** Seting Source MAC */
    SHR_IF_ERR_EXIT(dnx_oam_oamp_sa_mac_msb_profile_get(unit, mep_db_entry->sa_gen_msb_profile, sa_mac_addr_msb));
    sal_memcpy(endpoint_info->src_mac_address, sa_mac_addr_msb, DNX_OAM_MAC_SIZE);

    endpoint_info->src_mac_address[5] = (uint8) mep_db_entry->sa_gen_lsb;
    endpoint_info->src_mac_address[4] = (endpoint_info->src_mac_address[4] & DNX_OAM_ETH_SA_MAC_SECOND_BYTE_MASK)
        | (uint8) ((mep_db_entry->sa_gen_lsb >> SAL_UINT8_NOF_BITS) & DNX_OAM_ETH_SA_MAC_SECOND_BYTE_MASK);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set endpoint info of mpls oam according mep_db
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] mep_db_entry - mep_db
 * \param [out] endpoint_info - return filled endpoint info for accelerated endpoints according mep_db
 *  Updated fields are:
 *               type,
 *               egress_label.label, egress_label.ttl, egress_label.exp
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_oamp_ccm_endpoint_mpls_oam_get(
    int unit,
    const dnx_oam_oamp_ccm_endpoint_t * mep_db_entry,
    bcm_oam_endpoint_info_t * endpoint_info)
{
    SHR_FUNC_INIT_VARS(unit);

    if (mep_db_entry->label == MPLS_LABEL_GAL)
    {
        endpoint_info->type = bcmOAMEndpointTypeBhhSection;
    }
    else
    {
        endpoint_info->egress_label.label = mep_db_entry->label;
    }

    SHR_IF_ERR_EXIT(dnx_oam_oamp_mpls_pwe_exp_ttl_profile_get(unit,
                                                              mep_db_entry->push_profile,
                                                              &endpoint_info->egress_label.exp,
                                                              &endpoint_info->egress_label.ttl));

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Initialize mep db parameters that relevant to Eth OAM only
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint_info - info structure of the requested MEP
 * \param [out] mep_db_entry - return filled oam eth fields of mep_db entry according endpoint_info
 *  Updated fields are:
 *               outer_tpid,inner_tpid
 *               outer_tpid_index,inner_tpid_index
 *               outer_vid,inner_vid
 *               outer_pcp_dei,inner_pcp_dei
 *               nof_vlan_tags
 *               sa_gen_lsb
 *               sa_gen_msb_profile
 * \param [out] mep_hw_profiles_write_data - return new profile for outer_tpid,inner_tpid and sa mac msb if allocated
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_oamp_ccm_endpoint_eth_oam_init(
    int unit,
    const bcm_oam_endpoint_info_t * endpoint_info,
    dnx_oam_oamp_ccm_endpoint_t * mep_db_entry,
    oam_oamp_profile_hw_data_t * mep_hw_profiles_write_data)
{
    bcm_mac_t sa_mac;
    int tpid_index;
    int profile_id;
    uint16 tpid;
    uint8 first_reference;

    SHR_FUNC_INIT_VARS(unit);

    /** Sign if ETH OAM is upmep */
    mep_db_entry->flags |= DNX_OAM_DISSECT_IS_ENDPOINT_UPMEP(endpoint_info) ? DNX_OAMP_OAM_CCM_MEP_IS_UPMEP : 0;

    /** Single tag - outer tag field is used */
    if (endpoint_info->outer_tpid == 0)
    {
        if (endpoint_info->inner_tpid != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Endpoint create error: Inner tpid can not be configured without an outer tpid.\n");
        }
        mep_db_entry->nof_vlan_tags = 0;
    }
    else
    {
        tpid = endpoint_info->outer_tpid;
        /** Check if TPID is configured in system */
        SHR_IF_ERR_EXIT(dnx_switch_tpid_index_get(unit, endpoint_info->outer_tpid, &tpid_index));
        if (tpid_index == BCM_DNX_SWITCH_TPID_INDEX_INVALID)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Endpoint create error: TPID %d not configured.\n", endpoint_info->inner_tpid);
        }

        SHR_IF_ERR_EXIT(algo_oam_db.eth_oam_tpid_profile.allocate_single
                        (unit, _SHR_CORE_ALL, 0, &tpid, NULL, &profile_id, &first_reference));

        mep_db_entry->outer_tpid_index = (uint8) profile_id;
        mep_db_entry->outer_vid = endpoint_info->vlan;
        mep_db_entry->outer_pcp_dei = endpoint_info->pkt_pri;
        if (first_reference)
        {
            /** Sign that new profile is allocated */
            mep_hw_profiles_write_data->flags |= DNX_OAM_OAMP_PROFILE_OUTER_TPID_FLAG;

            /** Set profile parameters */
            mep_hw_profiles_write_data->outer_tpid_index = profile_id;
            mep_hw_profiles_write_data->outer_tpid = endpoint_info->outer_tpid;
        }

        if (endpoint_info->inner_tpid != 0)
        {
            tpid = endpoint_info->inner_tpid;
            /** Check if TPID is configured in system */
            SHR_IF_ERR_EXIT(dnx_switch_tpid_index_get(unit, endpoint_info->inner_tpid, &tpid_index));
            if (tpid_index == BCM_DNX_SWITCH_TPID_INDEX_INVALID)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Endpoint create error: TPID %d not configured.\n",
                             endpoint_info->inner_tpid);
            }

            SHR_IF_ERR_EXIT(algo_oam_db.eth_oam_tpid_profile.allocate_single
                            (unit, _SHR_CORE_ALL, 0, &tpid, NULL, &profile_id, &first_reference));

            mep_db_entry->inner_tpid_index = (uint8) profile_id;
            mep_db_entry->inner_vid = endpoint_info->inner_vlan;
            mep_db_entry->inner_pcp_dei = endpoint_info->inner_pkt_pri;
            if (first_reference)
            {
                /** Sign that new profile is allocated */
                mep_hw_profiles_write_data->flags |= DNX_OAM_OAMP_PROFILE_INNER_TPID_FLAG;

                /** Set profile parameters */
                mep_hw_profiles_write_data->inner_tpid_index = profile_id;
                mep_hw_profiles_write_data->inner_tpid = endpoint_info->inner_tpid;
            }

            mep_db_entry->nof_vlan_tags = 2;
        }
        else
        {
            mep_db_entry->nof_vlan_tags = 1;

            /** For case with one VLAN tag , HW take this values from inner_tpid/vid/pcp fields */
            mep_db_entry->inner_tpid_index = mep_db_entry->outer_tpid_index;
            mep_db_entry->inner_vid = mep_db_entry->outer_vid;
            mep_db_entry->inner_pcp_dei = mep_db_entry->outer_pcp_dei;

            mep_db_entry->outer_tpid_index = 0;
            mep_db_entry->outer_vid = 0;
            mep_db_entry->outer_pcp_dei = 0;

        }
    }
    /** Set 15 lsb bits of mac */
    mep_db_entry->sa_gen_lsb = endpoint_info->src_mac_address[5] |
        ((endpoint_info->src_mac_address[4] & DNX_OAM_ETH_SA_MAC_SECOND_BYTE_MASK) << SAL_UINT8_NOF_BITS);

    sal_memcpy(sa_mac, endpoint_info->src_mac_address, DNX_OAM_MAC_SIZE);

    /** Clear LSB bits from the prefix */
    sa_mac[5] = 0;
    sa_mac[4] &= ~DNX_OAM_ETH_SA_MAC_SECOND_BYTE_MASK;

    /** Allocate profile for msb(33 bits) */
    SHR_IF_ERR_EXIT(algo_oam_db.oam_sa_mac_msb_profile_template.allocate_single
                    (unit, _SHR_CORE_ALL, 0, sa_mac, NULL, &profile_id, &first_reference));

    /** Check if profile is new */
    if (first_reference)
    {
        /** Sign that new profile is allocated */
        mep_hw_profiles_write_data->flags |= DNX_OAM_OAMP_PROFILE_SA_MAC_MSB_FLAG;

        /** Set profile parameters */
        mep_hw_profiles_write_data->sa_mac_msb_profile = profile_id;
        sal_memcpy(mep_hw_profiles_write_data->sa_mac_msb, sa_mac, DNX_OAM_MAC_SIZE);
    }
    mep_db_entry->sa_gen_msb_profile = profile_id;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Initialize mep db parameters that relevant to MPLS OAM only
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint_info - info structure of the requested MEP
 * \param [out] mep_db_entry - return filled oam mpls fields of mep_db entry according endpoint_info
 *  Updated fields are:
 *               push_profile
 *               label
 * \param [out] mep_hw_profiles_write_data - return new profile for TTL and EXP if allocated
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_oamp_ccm_endpoint_mpls_oam_init(
    int unit,
    const bcm_oam_endpoint_info_t * endpoint_info,
    dnx_oam_oamp_ccm_endpoint_t * mep_db_entry,
    oam_oamp_profile_hw_data_t * mep_hw_profiles_write_data)
{
    int profile_id;
    uint8 first_reference;
    dnx_oam_ttl_exp_profile_t mpls_profile;

    SHR_FUNC_INIT_VARS(unit);

    mep_db_entry->label = (endpoint_info->type == bcmOAMEndpointTypeBhhSection) ?
        MPLS_LABEL_GAL : endpoint_info->egress_label.label;

    mpls_profile.ttl = endpoint_info->egress_label.ttl;
    mpls_profile.exp = endpoint_info->egress_label.exp;

    SHR_IF_ERR_EXIT(algo_oam_db.oam_mpls_pwe_exp_ttl_profile.allocate_single
                    (unit, _SHR_CORE_ALL, 0, &mpls_profile, NULL, &profile_id, &first_reference));

    /** Check if profile is new */
    if (first_reference)
    {
        /** Sign that new profile is allocated */
        mep_hw_profiles_write_data->flags |= DNX_OAM_OAMP_PROFILE_EXP_TTL_FLAG;

        /** Set profile parameters */
        mep_hw_profiles_write_data->mpls_pwe_exp_ttl_profile = profile_id;
        mep_hw_profiles_write_data->ttl_exp.exp = mpls_profile.exp;
        mep_hw_profiles_write_data->ttl_exp.ttl = mpls_profile.ttl;
    }

    mep_db_entry->push_profile = (uint8) profile_id;

exit:
    SHR_FUNC_EXIT;

}

/*
 * See h file for description
 */
shr_error_e
dnx_oam_local_accelerated_endpoint_fill_id(
    int unit,
    bcm_oam_endpoint_info_t * endpoint_info,
    const uint8 *group_name)
{
    dnx_oam_ma_name_type_e group_name_type = BCM_DNX_OAM_GROUP_NAME_TO_NAME_TYPE(group_name);
    uint32 flags;
    dnx_oamp_mep_db_memory_type_t memory_type;
    SHR_FUNC_INIT_VARS(unit);

    /** For endpoint allocation 'with id' memory requirements are irrelevant. */
    if (endpoint_info->flags & BCM_OAM_ENDPOINT_WITH_ID)
    {
        flags = SW_STATE_ALGO_RES_ALLOCATE_WITH_ID;
    }
    else
    {

        flags = 0;
       /** Endpoints are always in banks 0-7 of MEP_DB */
        memory_type.is_extra_pool = FALSE;

       /** Type of requested entry */
        memory_type.is_full = (endpoint_info->endpoint_memory_type != bcmOamEndpointMemoryTypeShortEntry);

       /** Setting the type of memory that should be allocated.*/
        memory_type.is_2byte_maid = (group_name_type == DNX_OAMP_OAM_MA_NAME_TYPE_SHORT);
    }

   /** Allocate endpoint_id */
    SHR_IF_ERR_EXIT(dnx_algo_oamp_mep_id_alloc(unit, flags, &memory_type, (uint32 *) (&endpoint_info->id)));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - set up and allocate local-port-to-system-port
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint_info - see definition of struct for
 *        description of fields
 * \param [out] mep_hw_write_data - Pointer to struct where data
 *        for writing to HW tables is placed.  This data is
 *        relevant to fields in the OAMP MEP DB.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_system_port_profile_sw_update(
    int unit,
    const bcm_oam_endpoint_info_t * endpoint_info,
    oam_oamp_profile_hw_data_t * mep_hw_write_data)
{
    shr_error_e rv;
    dnx_algo_gpm_gport_phy_info_t *tx_gport_data = NULL;
    int system_port_profile_index;
    uint8 write_hw;
    uint16 system_port;
    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(tx_gport_data, sizeof(dnx_algo_gpm_gport_phy_info_t),
                       "Structure for reading data about tx_gport", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get(unit, endpoint_info->tx_gport,
                                                    DNX_ALGO_GPM_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT, tx_gport_data));

    system_port = tx_gport_data->sys_port;
    rv = algo_oamp_db.oamp_pp_port_2_sys_port.allocate_single
        (unit, _SHR_CORE_ALL, 0, (uint32 *) &system_port, NULL, &system_port_profile_index, &write_hw);

    if (rv == _SHR_E_RESOURCE)
    {
        /** Resource error means no match - display error message */
        SHR_ERR_EXIT(_SHR_E_RESOURCE, "Error: No free system port profile entries available.\n");
    }
    else
    {
        /** All other errors handled generically */
        SHR_IF_ERR_EXIT(rv);
    }

    mep_hw_write_data->pp_port_profile = system_port_profile_index;

    if (write_hw)
    {
        /** Save data for writing to HW table at the end   */
        mep_hw_write_data->system_port = tx_gport_data->sys_port;
        mep_hw_write_data->flags |= DNX_OAM_OAMP_PROFILE_LOCL_PORT_TO_SYSTEM_PORT_FLAG;
    }

exit:
    SHR_FREE(tx_gport_data);
    SHR_FUNC_EXIT;
}

/*
 * See h file for description
 */
shr_error_e
dnx_oam_local_acc_endpoint_get(
    int unit,
    bcm_oam_endpoint_info_t * endpoint_info)
{

    dnx_oam_oamp_ccm_endpoint_t *mep_db_entry = NULL;
    dnx_oam_itmh_priority_t itmh_priority;
    int counter;
    uint32 system_port;
    dnx_oam_mep_profile_t mep_profile;
    uint8 rdi_from_rx = 0, rdi_from_scannner = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC(mep_db_entry, sizeof(dnx_oam_oamp_ccm_endpoint_t),
              "Endpoint data read ", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(dnx_oamp_mep_db_ccm_endpoint_get(unit, (uint16) endpoint_info->id,
                                                     DNX_OAM_DISSECT_IS_DOWN_MEP_EGRESS_INJECTION(endpoint_info),
                                                     mep_db_entry));

    endpoint_info->endpoint_memory_type = ((_SHR_IS_FLAG_SET(mep_db_entry->flags, DNX_OAMP_OAM_CCM_MEP_LM_DM_OFFLOADED))
                                           ? bcmOamEndpointMemoryTypeLmDmOffloadedEntry
                                           : (((_SHR_IS_FLAG_SET(mep_db_entry->flags, DNX_OAMP_OAM_CCM_MEP_Q_ENTRY)) ?
                                               bcmOamEndpointMemoryTypeShortEntry :
                                               bcmOamEndpointMemoryTypeSelfContained)));

    if (!_SHR_IS_FLAG_SET(mep_db_entry->flags, DNX_OAMP_OAM_CCM_MEP_IS_UPMEP))
    {
        SHR_IF_ERR_EXIT(algo_oam_db.oam_itmh_priority_profile.profile_data_get
                        (unit, _SHR_CORE_ALL, mep_db_entry->itmh_tc_dp_profile, &counter, &itmh_priority));
        endpoint_info->int_pri = (itmh_priority.tc << 2) | itmh_priority.dp;

        if (mep_db_entry->mep_type != DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM)
        {
            if (_SHR_IS_FLAG_SET(mep_db_entry->flags, DNX_OAMP_OAM_CCM_MEP_DESTINATION_IS_FEC))
            {
                BCM_L3_ITF_SET(endpoint_info->intf_id, BCM_L3_ITF_TYPE_FEC,
                               mep_db_entry->fec_id_or_glob_out_lif.fec_id);
                endpoint_info->tx_gport = BCM_GPORT_INVALID;
            }
            else
            {
                BCM_L3_ITF_SET(endpoint_info->intf_id, BCM_L3_ITF_TYPE_LIF,
                               mep_db_entry->fec_id_or_glob_out_lif.glob_out_lif);
                BCM_GPORT_SYSTEM_PORT_ID_SET(endpoint_info->tx_gport, mep_db_entry->dest_sys_port_agr);
            }
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_oam_oamp_local_port_to_system_port_get(unit,
                                                                       mep_db_entry->unified_port_access.port_profile,
                                                                       &system_port));
            BCM_GPORT_SYSTEM_PORT_ID_SET(endpoint_info->tx_gport, system_port);
        }

    }

    counter = 0;
    sal_memset(&mep_profile, 0, sizeof(dnx_oam_mep_profile_t));
    SHR_IF_ERR_EXIT(algo_oam_db.oam_mep_profile.profile_data_get
                    (unit, _SHR_CORE_ALL, mep_db_entry->mep_profile, &counter, &mep_profile));

    DNX_OAM_GET_RDI_GEN_METHOD_FIELD_FROM_MEP_PROFILE(mep_profile.rdi_gen_method, rdi_from_rx, rdi_from_scannner);
    /*
     * By default both RX and Scanner processing is used to generate RDI
     */
    if (!(rdi_from_rx && rdi_from_scannner))
    {
        endpoint_info->flags2 |= rdi_from_rx ? 0 : BCM_OAM_ENDPOINT_FLAGS2_RDI_FROM_RX_DISABLE;
        endpoint_info->flags2 |= rdi_from_scannner ? 0 : BCM_OAM_ENDPOINT_FLAGS2_RDI_FROM_LOC_DISABLE;
    }

    if (mep_db_entry->mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM)
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_ccm_endpoint_eth_oam_get(unit, mep_db_entry, endpoint_info));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_ccm_endpoint_mpls_oam_get(unit, mep_db_entry, endpoint_info));
    }

    /** Setting MEP-ID */
    endpoint_info->name = mep_db_entry->mep_id;

    /** Set port state */
    if (mep_db_entry->flags & DNX_OAMP_OAM_CCM_MEP_PORT_TLV_EN)
    {
        endpoint_info->port_state =
            (mep_db_entry->flags & DNX_OAMP_OAM_CCM_MEP_PORT_TLV_VAL) ? BCM_OAM_PORT_TLV_UP : BCM_OAM_PORT_TLV_BLOCKED;
    }
    else
    {
        endpoint_info->port_state = 0;
    }

    /** Set interface state */
    endpoint_info->interface_state = mep_db_entry->interface_status_tlv_code;

    if ((mep_db_entry->ccm_interval > 0) && (mep_profile.ccm_count == DNX_OAM_OAMP_ETH1731_MEP_PROFILE_MAX_CCM_CNT))
    {
        /*
         * To enable CCM Rx without Tx, when CCM period of endpoint gets updated from non-zero to zero value, ccm_count 
         * is set to max value (0x7ffff) to stop the transmission. OAMP_MEP_DB.ccm_interval remains unchanged which
         * allows processing of CCM packets by the OAMP 
         */
        endpoint_info->ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_DISABLED;
    }
    else
    {
        /** Calculate CCM period */
        dnx_oam_oamp_ccm_interval_to_ccm_period(mep_db_entry->ccm_interval, &endpoint_info->ccm_period);
    }

exit:
    SHR_FREE(mep_db_entry);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Release profiles(sw) that were used for mpls oam and sign mpls oam profiles that not in use anymore
 *
 * \param [in] unit  - Relevant unit.
 * \param [in] mep_db_entry - mep_db entry according endpoint_info
 * \param [out] mep_hw_profiles_write_data - sign profiles that should be removed(data cleared) in hw
 * \retval
 *   shr_error - Error indication
 * \remark
 *   None
 * \see
 *   None
 */

static shr_error_e
dnx_oam_oamp_ccm_endpoint_mpls_profiles_free(
    int unit,
    const dnx_oam_oamp_ccm_endpoint_t * mep_db_entry,
    oam_oamp_profile_hw_data_t * mep_hw_profiles_write_data)
{
    uint8 last_reference;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_oam_db.oam_mpls_pwe_exp_ttl_profile.free_single
                    (unit, _SHR_CORE_ALL, (int) mep_db_entry->push_profile, &last_reference));

    if (last_reference)
    {
        /** Sign that new profile is allocated */
        mep_hw_profiles_write_data->flags |= DNX_OAM_OAMP_PROFILE_EXP_TTL_FLAG;

        /** Set profile parameters */
        mep_hw_profiles_write_data->mpls_pwe_exp_ttl_profile = mep_db_entry->push_profile;
        mep_hw_profiles_write_data->ttl_exp.exp = 0;
        mep_hw_profiles_write_data->ttl_exp.ttl = 0;
    }

exit:
    SHR_FUNC_EXIT;

}
/**
 * \brief - Release profiles(sw) that were used for ethernet oam and sign ethernet oam profiles that not in use anymore
 *
 * \param [in] unit  - Relevant unit.
 * \param [in] mep_db_entry - mep_db entry according endpoint_info
 * \param [out] mep_hw_profiles_write_data - sign profiles that should be removed(data cleared) in hw
 * \retval
 *   shr_error - Error indication
 * \remark
 *   None
 * \see
 *   None
 */

static shr_error_e
dnx_oam_oamp_ccm_endpoint_eth_oam_profiles_free(
    int unit,
    const dnx_oam_oamp_ccm_endpoint_t * mep_db_entry,
    oam_oamp_profile_hw_data_t * mep_hw_profiles_write_data)
{
    uint8 last_reference;
    bcm_mac_t sa_mac;

    SHR_FUNC_INIT_VARS(unit);

    if (!_SHR_IS_FLAG_SET(mep_db_entry->flags, DNX_OAMP_OAM_CCM_MEP_IS_UPMEP))
    {
        SHR_IF_ERR_EXIT(algo_oamp_db.oamp_pp_port_2_sys_port.free_single
                        (unit, _SHR_CORE_ALL, mep_db_entry->unified_port_access.port_profile, &last_reference));

        if (last_reference)
        {
            mep_hw_profiles_write_data->flags |= DNX_OAM_OAMP_PROFILE_LOCL_PORT_TO_SYSTEM_PORT_FLAG;
            mep_hw_profiles_write_data->pp_port_profile = mep_db_entry->unified_port_access.port_profile;
            mep_hw_profiles_write_data->system_port = 0;
        }
    }
    if (mep_db_entry->nof_vlan_tags != 0)
    {
        SHR_IF_ERR_EXIT(algo_oam_db.eth_oam_tpid_profile.free_single
                        (unit, _SHR_CORE_ALL, (int) mep_db_entry->inner_tpid_index, &last_reference));

        if (last_reference)
        {
            mep_hw_profiles_write_data->flags |= DNX_OAM_OAMP_PROFILE_INNER_TPID_FLAG;

            /** Set profile parameters */
            mep_hw_profiles_write_data->inner_tpid_index = mep_db_entry->inner_tpid_index;
            mep_hw_profiles_write_data->inner_tpid = 0;

        }
        if (mep_db_entry->nof_vlan_tags == 2)
        {

            SHR_IF_ERR_EXIT(algo_oam_db.eth_oam_tpid_profile.free_single
                            (unit, _SHR_CORE_ALL, (int) mep_db_entry->outer_tpid_index, &last_reference));
            if (last_reference)
            {
                mep_hw_profiles_write_data->flags |= DNX_OAM_OAMP_PROFILE_OUTER_TPID_FLAG;

                /** Set profile parameters */
                mep_hw_profiles_write_data->outer_tpid_index = mep_db_entry->outer_tpid_index;
                mep_hw_profiles_write_data->outer_tpid = 0;
            }
        }

        SHR_IF_ERR_EXIT(algo_oam_db.oam_sa_mac_msb_profile_template.free_single
                        (unit, _SHR_CORE_ALL, (int) mep_db_entry->sa_gen_msb_profile, &last_reference));

        if (last_reference)
        {
            sal_memset(sa_mac, 0, sizeof(bcm_mac_t));

            /** Sign that profile should be reset */
            mep_hw_profiles_write_data->flags |= DNX_OAM_OAMP_PROFILE_SA_MAC_MSB_FLAG;

            /** Set profile parameters */
            mep_hw_profiles_write_data->sa_mac_msb_profile = mep_db_entry->sa_gen_msb_profile;
            sal_memcpy(mep_hw_profiles_write_data->sa_mac_msb, sa_mac, DNX_OAM_MAC_SIZE);
        }
    }

exit:
    SHR_FUNC_EXIT;
}
/*
 * See h file for description
 */
shr_error_e
dnx_oam_oamp_ccm_endpoint_mep_db_profiles_free(
    int unit,
    const dnx_oam_oamp_ccm_endpoint_t * mep_db_entry,
    oam_oamp_profile_hw_data_t * mep_hw_profiles_write_data)
{
    uint8 last_reference;

    SHR_FUNC_INIT_VARS(unit);

    if (!_SHR_IS_FLAG_SET(mep_db_entry->flags, DNX_OAMP_OAM_CCM_MEP_IS_UPMEP))
    {

        SHR_IF_ERR_EXIT(algo_oam_db.oam_itmh_priority_profile.free_single
                        (unit, _SHR_CORE_ALL, (int) mep_db_entry->itmh_tc_dp_profile, &last_reference));

        if (last_reference)
        {
            mep_hw_profiles_write_data->flags |= DNX_OAM_OAMP_PROFILE_ITMH_PRIORITY_FLAG;

            mep_hw_profiles_write_data->itmh_profile = mep_db_entry->itmh_tc_dp_profile;
            mep_hw_profiles_write_data->itmh_priority.dp = 0;
            mep_hw_profiles_write_data->itmh_priority.tc = 0;
        }
    }

    SHR_IF_ERR_EXIT(algo_oam_db.oam_mep_profile.free_single
                    (unit, _SHR_CORE_ALL, (int) mep_db_entry->mep_profile, &last_reference));

    if (last_reference)
    {
        mep_hw_profiles_write_data->flags |= DNX_OAM_OAMP_PROFILE_MEP_PROFILE_FLAG;
        mep_hw_profiles_write_data->mep_profile_index = mep_db_entry->mep_profile;
        sal_memset(&mep_hw_profiles_write_data->mep_profile, 0, sizeof(dnx_oam_mep_profile_t));
    }

    if (mep_db_entry->mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM)
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_ccm_endpoint_eth_oam_profiles_free
                        (unit, mep_db_entry, mep_hw_profiles_write_data));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_ccm_endpoint_mpls_profiles_free(unit, mep_db_entry, mep_hw_profiles_write_data));
    }

exit:
    SHR_FUNC_EXIT;
}
/*
 * See h file for description
 */
shr_error_e
dnx_oam_oamp_ccm_endpoint_mep_db_create(
    int unit,
    const bcm_oam_endpoint_info_t * endpoint_info,
    const bcm_oam_group_info_t * group_info,
    dnx_oam_oamp_ccm_endpoint_t * mep_db_entry,
    oam_oamp_profile_hw_data_t * mep_hw_profiles_write_data,
    oam_oamp_profile_hw_data_t * mep_hw_profiles_delete_data)
{

    int prev_ccm_period = 0;
    uint8 ccm_rx_without_tx = 0;
    int profile_id;
    uint8 first_reference;
    dnx_oam_itmh_priority_t itmh_priority;
    dnx_algo_gpm_gport_phy_info_t *tx_gport_data = NULL;
    bcm_vlan_port_t vlan_port;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    dnx_oam_mep_profile_t mep_profile;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Initialize the whole 'bcm_vlan_port_t vlan_port' structure since it is both
     * input and output to bcm_vlan_port_find() below.
     */
    bcm_vlan_port_t_init(&vlan_port);
    SHR_ALLOC_SET_ZERO(tx_gport_data, sizeof(dnx_algo_gpm_gport_phy_info_t),
                       "Structure for reading data about tx_gport", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    if (_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_OAM_ENDPOINT_REPLACE))
    {
        /**
         * An existing endpoint is being modified; delete
         * current profiles
         */

        /** Get mep_db entry */
        SHR_IF_ERR_EXIT(dnx_oamp_mep_db_ccm_endpoint_get(unit, (uint16) endpoint_info->id,
                                                         DNX_OAM_DISSECT_IS_DOWN_MEP_EGRESS_INJECTION(endpoint_info),
                                                         mep_db_entry));

        /** Free allocated profiles(sw) and sign profiles that should be updated(cleared) in hw */
        SHR_IF_ERR_EXIT(dnx_oam_oamp_ccm_endpoint_mep_db_profiles_free
                        (unit, mep_db_entry, mep_hw_profiles_delete_data));

        /** Necessary for allocation and MDB tables */
        mep_db_entry->flags = DNX_OAMP_OAM_CCM_MEP_UPDATE;
    }

   /** Set entry type */
    mep_db_entry->flags |= ((endpoint_info->endpoint_memory_type == bcmOamEndpointMemoryTypeLmDmOffloadedEntry) ?
                            DNX_OAMP_OAM_CCM_MEP_LM_DM_OFFLOADED :
                            ((endpoint_info->endpoint_memory_type == bcmOamEndpointMemoryTypeShortEntry) ?
                             DNX_OAMP_OAM_CCM_MEP_Q_ENTRY : 0));

    mep_db_entry->flags |= (endpoint_info->flags & BCM_OAM_ENDPOINT_RDI_TX) ? DNX_OAMP_OAM_CCM_MEP_RDI_FROM_PACKET : 0;

    /** 48B MAID flag */
    if (_SHR_IS_FLAG_SET(group_info->flags, BCM_OAM_GROUP_FLEXIBLE_MAID_48_BYTE))
    {
        mep_db_entry->flags |= DNX_OAMP_OAM_CCM_MEP_48B_MAID;
    }

    /** Setting Down-MEP Egress flag */
    if (_SHR_IS_FLAG_SET(endpoint_info->flags2, BCM_OAM_ENDPOINT_FLAGS2_EGRESS_INJECTION_DOWN))
    {
        mep_db_entry->flags |= DNX_OAMP_OAM_CCM_MEP_DOWN_MEP_EGRESS_INJECTION;
        mep_db_entry->vsi = endpoint_info->vpn;
    }

   /** Map bcm ep type to mep db type */
    SHR_IF_ERR_EXIT(dnx_oam_oamp_endpoint_bcm_mep_type_to_mep_db_mep_type
                    (unit, endpoint_info->type, &mep_db_entry->mep_type));

    /**
     * System headers
     */
    if (!DNX_OAM_DISSECT_IS_ENDPOINT_UPMEP(endpoint_info))
    {
        /** ITMH attributes profile allocate */
        /** int_pri: ((COS & 0x7) << 2) + (DP & 0x3))*/
        itmh_priority.tc = (endpoint_info->int_pri & 0x1F) >> 2;
        itmh_priority.dp = (endpoint_info->int_pri & 0x3);
        /** Allocate profile for tc/dp */
        SHR_IF_ERR_EXIT(algo_oam_db.oam_itmh_priority_profile.allocate_single
                        (unit, _SHR_CORE_ALL, 0, &itmh_priority, NULL, &profile_id, &first_reference));
        mep_db_entry->itmh_tc_dp_profile = profile_id;
        if (first_reference)
        {
            mep_hw_profiles_write_data->flags |= DNX_OAM_OAMP_PROFILE_ITMH_PRIORITY_FLAG;

            mep_hw_profiles_write_data->itmh_profile = profile_id;
            mep_hw_profiles_write_data->itmh_priority.dp = itmh_priority.dp;
            mep_hw_profiles_write_data->itmh_priority.tc = itmh_priority.tc;
        }

        if (mep_db_entry->mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM)
        {
            /** TX gport*/
            SHR_IF_ERR_EXIT(dnx_oam_system_port_profile_sw_update(unit, endpoint_info, mep_hw_profiles_write_data));
            mep_db_entry->unified_port_access.port_profile = mep_hw_profiles_write_data->pp_port_profile;
        }
        else
        {
            /** Internal header */
            /** Is the provided interface a FEC ID? */
            if (BCM_L3_ITF_TYPE_IS_FEC(endpoint_info->intf_id))
            {
                mep_db_entry->flags |= DNX_OAMP_OAM_CCM_MEP_DESTINATION_IS_FEC;
                mep_db_entry->fec_id_or_glob_out_lif.fec_id = BCM_L3_ITF_VAL_GET(endpoint_info->intf_id);
                mep_db_entry->dest_sys_port_agr = BCM_GPORT_INVALID;
            }
            else
            {
                /** Not a valid FEC ID.  Is it a global out-LIF */
                if (BCM_L3_ITF_TYPE_IS_LIF(endpoint_info->intf_id))
                {
                    mep_db_entry->fec_id_or_glob_out_lif.glob_out_lif = BCM_L3_ITF_VAL_GET(endpoint_info->intf_id);
                    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get(unit, endpoint_info->tx_gport,
                                                                    DNX_ALGO_GPM_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT |
                                                                    DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY,
                                                                    tx_gport_data));

                    mep_db_entry->dest_sys_port_agr = tx_gport_data->sys_port;
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Error: provided interface is not a valid FEC nor an out-LIF.\n");
                }
            }
            /*
             * Put MEP's outlif to ITMH's oulitf, then lookup classifier to get correct count (For pseudo PWE, it's
             * real LSP type)
             */
            if ((dnx_data_oam.property.oam_injected_over_lsp_cnt_get(unit) == 1)
                && (endpoint_info->type == bcmOAMEndpointTypeBHHMPLS)
                && (endpoint_info->mpls_out_gport == endpoint_info->intf_id))
            {
                /*
                 * pseudo PWE, it's real LSP type
                 */
                mep_db_entry->mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_Y1731_PWE;

                if (endpoint_info->mpls_out_gport != BCM_GPORT_INVALID)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                                    (unit, endpoint_info->mpls_out_gport, DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF,
                                     &gport_hw_resources));
                    mep_db_entry->fec_id_or_glob_out_lif.glob_out_lif = gport_hw_resources.global_out_lif;
                }
            }

        }
    }
    else
    {
        /** Retrieves VLAN Port information according to a given gport */
        vlan_port.vlan_port_id = endpoint_info->gport;
        SHR_IF_ERR_EXIT(bcm_vlan_port_find(unit, &vlan_port));

        /** Get Port + Core */
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get(unit, vlan_port.port,
                                                        DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY,
                                                        &gport_info));

        /**
         * Fill mep_db_entry with port and core.
         * Since OAMP only inject to one pp port, take first port and core.
         */
        mep_db_entry->unified_port_access.ccm_eth_up_mep_port.pp_port = gport_info.internal_port_pp_info.pp_port[0];
        mep_db_entry->unified_port_access.ccm_eth_up_mep_port.port_core = gport_info.internal_port_pp_info.core_id[0];

    }

    /*
     * Set counter base and if 
     *  
     * In case of Downmep lm/dm with priority except egress_injection case, 
     * counter_pointer will be re-calculated during loss_add/delay_add. 
     *  
     * In case of upmep tx, counter is taken from classifier. So only for downmep 
     * except egress_injection case we need to set counter_ptr and counter_if 
     *  
     */
    if ((!DNX_OAM_DISSECT_IS_ENDPOINT_UPMEP(endpoint_info))
        && (!_SHR_IS_FLAG_SET(endpoint_info->flags2, BCM_OAM_ENDPOINT_FLAGS2_EGRESS_INJECTION_DOWN)))
    {
        mep_db_entry->counter_interface = endpoint_info->lm_counter_if;
        mep_db_entry->counter_ptr = endpoint_info->lm_counter_base_id;
    }

    /**
     * Network headers
     */

    if (mep_db_entry->mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM)
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_ccm_endpoint_eth_oam_init
                        (unit, endpoint_info, mep_db_entry, mep_hw_profiles_write_data));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_ccm_endpoint_mpls_oam_init
                        (unit, endpoint_info, mep_db_entry, mep_hw_profiles_write_data));
    }

    if (_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_OAM_ENDPOINT_REPLACE))
    {
        /** Calculate old CCM period */
        dnx_oam_oamp_ccm_interval_to_ccm_period(mep_db_entry->ccm_interval, &prev_ccm_period);

        if (endpoint_info->ccm_period == BCM_OAM_ENDPOINT_CCM_PERIOD_DISABLED
            && prev_ccm_period != BCM_OAM_ENDPOINT_CCM_PERIOD_DISABLED)
        {
            ccm_rx_without_tx = 1;
        }
    }

    if (!ccm_rx_without_tx)
    {
        /** CCM interval calculating */
        dnx_oam_oamp_ccm_period_to_ccm_interval(endpoint_info->ccm_period, &mep_db_entry->ccm_interval);
    }

    /** Allocate profile for MEP Profile related parameters */
    sal_memset(&mep_profile, 0, sizeof(dnx_oam_mep_profile_t));
    /*
     * By default RDI is taken from both scanner and RX i.e (scanner| RX)
     */
    DNX_OAM_SET_RDI_GEN_METHOD_FIELD_ON_MEP_PROFILE(mep_profile.rdi_gen_method,
                                                    (endpoint_info->flags2 &
                                                     BCM_OAM_ENDPOINT_FLAGS2_RDI_FROM_RX_DISABLE),
                                                    (endpoint_info->flags2 &
                                                     BCM_OAM_ENDPOINT_FLAGS2_RDI_FROM_LOC_DISABLE));
    /** Setting opcode bit_map to 0xff which does not have effect until opcode rate is configured
     *  Ideally this is a redundant parameter, better to remove from HW to get rid of redundancy
     */
    mep_profile.opcode_bit_map = 0xff;
    /*
     * For 48B MAID, MAID verification is disabled and CRC verification is enabled
     */
    if (_SHR_IS_FLAG_SET(group_info->flags, BCM_OAM_GROUP_FLEXIBLE_MAID_48_BYTE))
    {
        mep_profile.maid_check_dis = TRUE;
    }
    /*
     * Setting LMM/LMR/DMM/DMR offsets here since it could be required for OAMP reply to LMM/DMM.
     */
    DNX_OAM_SET_LMM_OFFSET(mep_db_entry->mep_type, mep_db_entry->nof_vlan_tags, 0, 0, mep_profile.lmm_offset);
    DNX_OAM_SET_LMR_OFFSET(mep_db_entry->mep_type, mep_db_entry->nof_vlan_tags, 0, 0, mep_profile.lmr_offset);
    DNX_OAM_SET_DMM_OFFSET(mep_db_entry->mep_type, mep_db_entry->nof_vlan_tags, mep_profile.dmm_offset);
    DNX_OAM_SET_DMR_OFFSET(mep_db_entry->mep_type, mep_db_entry->nof_vlan_tags, mep_profile.dmr_offset);
    /*
     * Setting phase count for CCM.
     */
    SHR_IF_ERR_EXIT(dnx_oam_oamp_scan_count_calc(unit,
                                                 endpoint_info->id,
                                                 (endpoint_info->endpoint_memory_type ==
                                                  bcmOamEndpointMemoryTypeShortEntry), mep_db_entry->ccm_interval,
                                                 ccm_rx_without_tx, OAMP_MEP_TX_OPCODE_CCM_BFD, &mep_profile));

    SHR_IF_ERR_EXIT(algo_oam_db.oam_mep_profile.allocate_single
                    (unit, _SHR_CORE_ALL, 0, &mep_profile, NULL, &profile_id, &first_reference));
    if (first_reference)
    {
        mep_hw_profiles_write_data->flags |= DNX_OAM_OAMP_PROFILE_MEP_PROFILE_FLAG;

        mep_hw_profiles_write_data->mep_profile_index = profile_id;
        mep_hw_profiles_write_data->mep_profile.rdi_gen_method = mep_profile.rdi_gen_method;
        mep_hw_profiles_write_data->mep_profile.opcode_bit_map = mep_profile.opcode_bit_map;
        mep_hw_profiles_write_data->mep_profile.lmm_offset = mep_profile.lmm_offset;
        mep_hw_profiles_write_data->mep_profile.lmr_offset = mep_profile.lmr_offset;
        mep_hw_profiles_write_data->mep_profile.dmm_offset = mep_profile.dmm_offset;
        mep_hw_profiles_write_data->mep_profile.dmr_offset = mep_profile.dmr_offset;
        mep_hw_profiles_write_data->mep_profile.maid_check_dis = mep_profile.maid_check_dis;
        mep_hw_profiles_write_data->mep_profile.ccm_count = mep_profile.ccm_count;
    }

    mep_db_entry->mep_profile = profile_id;

    /**
     * OAM PDU
     */

    /** Setting MDL */
    mep_db_entry->mdl = endpoint_info->level;
    /** Setting MEP-ID */
    mep_db_entry->mep_id = endpoint_info->name;

    if (!_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_OAM_ENDPOINT_REPLACE) &&
        !_SHR_IS_FLAG_SET(group_info->flags, BCM_OAM_GROUP_FLEXIBLE_MAID_48_BYTE))
    {
        /** Fill icc_index and maid fields*/
        SHR_IF_ERR_EXIT(dnx_oam_update_accelarated_endpoint_info_with_group_config
                        (unit, group_info->name, mep_db_entry));
    }

    /** Update Port State */
    if (endpoint_info->port_state != 0)
    {
        /** Set Port State enable */
        mep_db_entry->flags |= DNX_OAMP_OAM_CCM_MEP_PORT_TLV_EN;
        /** Set Port state:
         * '0' - Port Blocked
         * '1' - Port is ready to pass data
         */
        mep_db_entry->flags |=
            ((endpoint_info->port_state) == BCM_OAM_PORT_TLV_UP) ? DNX_OAMP_OAM_CCM_MEP_PORT_TLV_VAL : 0;
    }

    /** Set interface state */
    mep_db_entry->interface_status_tlv_code = endpoint_info->interface_state;

    /** 48B MAID */
    if (_SHR_IS_FLAG_SET(group_info->flags, BCM_OAM_GROUP_FLEXIBLE_MAID_48_BYTE))
    {
        mep_db_entry->mep_pe_profile = DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_MAID_48;

        if (endpoint_info->endpoint_memory_type == bcmOamEndpointMemoryTypeSelfContained)
        {
            /** The extra_data_header field points to a MEP DB entry, not an OAM ID */
            mep_db_entry->extra_data_header = OAM_ID_TO_MEP_DB_ENTRY(group_info->group_name_index);

            if (_SHR_IS_FLAG_SET(endpoint_info->flags2, BCM_OAM_ENDPOINT_FLAGS2_EGRESS_INJECTION_DOWN))
            {
                mep_db_entry->mep_pe_profile = DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DOWN_MEP_INJ_SELF_CONTAINED_MAID_48;
            }
        }
        else
        {
            mep_db_entry->extra_data_header = group_info->id;

            
        }
    }
    else
    {
        if (endpoint_info->endpoint_memory_type == bcmOamEndpointMemoryTypeSelfContained)
        {
            if (_SHR_IS_FLAG_SET(endpoint_info->flags2, BCM_OAM_ENDPOINT_FLAGS2_EGRESS_INJECTION_DOWN))
            {
                mep_db_entry->mep_pe_profile = DBAL_ENUM_FVAL_MEP_PE_PROFILE_SW_DOWN_MEP_INJ_SELF_CONTAINED;
            }
        }
    }

    
    /** OAMP Statistics*/
    if ((_SHR_IS_FLAG_SET(endpoint_info->flags2, BCM_OAM_ENDPOINT_FLAGS2_TX_STATISTICS))
        || (_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_OAM_ENDPOINT_FLAGS2_RX_STATISTICS)))
    {
        dnx_oam_oamp_statistics_enable(unit, endpoint_info->flags2, &mep_hw_profiles_write_data->mep_profile);
    }
exit:
    SHR_FREE(tx_gport_data);
    SHR_FUNC_EXIT;
}

/*
 * See h file for description
 */
shr_error_e
dnx_oam_oamp_mac_da_msb_profile_set(
    int unit,
    uint8 profile,
    bcm_mac_t msb)
{
    uint32 entry_handle_id;
    uint32 nof_da_mac_msb_profiles;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    nof_da_mac_msb_profiles = dnx_data_oam.oamp.oam_nof_da_mac_msb_profiles_get(unit);

    if (profile >= nof_da_mac_msb_profiles)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid profile index");
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MEP_DA_MSB, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LMM_DA_MSB_PROFILE, profile);

    dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_DA_MSB, INST_SINGLE, msb);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See h file for description
 */
shr_error_e
dnx_oam_oamp_mac_da_msb_profile_get(
    int unit,
    uint8 profile,
    bcm_mac_t msb)
{
    uint32 entry_handle_id;
    uint32 nof_da_mac_msb_profiles;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    nof_da_mac_msb_profiles = dnx_data_oam.oamp.oam_nof_da_mac_msb_profiles_get(unit);

    if (profile >= nof_da_mac_msb_profiles)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid profile index");
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MEP_DA_MSB, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LMM_DA_MSB_PROFILE, profile);

    dbal_value_field_arr8_request(unit, entry_handle_id, DBAL_FIELD_DA_MSB, INST_SINGLE, msb);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See h file for description
 */
shr_error_e
dnx_oam_oamp_mac_da_lsb_profile_set(
    int unit,
    uint8 profile,
    bcm_mac_t lsb)
{
    uint32 entry_handle_id;
    uint32 nof_da_mac_lsb_profiles;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    nof_da_mac_lsb_profiles = dnx_data_oam.oamp.oam_nof_da_mac_lsb_profiles_get(unit);
    if (profile >= nof_da_mac_lsb_profiles)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid profile index");
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MEP_DA_LSB_TABLE, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LMM_DA_LSB_PROFILE, profile);

    dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_DA_LSB, INST_SINGLE, lsb);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See h file for description
 */
shr_error_e
dnx_oam_oamp_mac_da_lsb_profile_get(
    int unit,
    uint8 profile,
    bcm_mac_t lsb)
{
    uint32 entry_handle_id;
    uint32 nof_da_mac_lsb_profiles;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    nof_da_mac_lsb_profiles = dnx_data_oam.oamp.oam_nof_da_mac_lsb_profiles_get(unit);

    if (profile >= nof_da_mac_lsb_profiles)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid profile index");
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MEP_DA_LSB_TABLE, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LMM_DA_LSB_PROFILE, profile);

    dbal_value_field_arr8_request(unit, entry_handle_id, DBAL_FIELD_DA_LSB, INST_SINGLE, lsb);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition in oam_oamp.h for function description
 */
shr_error_e
dnx_oam_oamp_short_entry_type_check_mep_db(
    int unit,
    uint16 oam_id,
    dbal_enum_value_field_oamp_mep_type_e req_mep_type,
    int *sub_index,
    uint8 *is_leader_alloc,
    uint8 *is_leader_type_match)
{
    dnx_oam_oamp_mep_db_args_t *leader_info = NULL;
    uint32 leader_endpoint_id;
    int nof_mep_db_short_entries;
    SHR_FUNC_INIT_VARS(unit);

    *is_leader_type_match = TRUE;

    nof_mep_db_short_entries = dnx_data_oam.oamp.mep_db_nof_sub_entries_get(unit);

    *sub_index = (oam_id % nof_mep_db_short_entries);
    if (*sub_index != 0)
    {
        /**
         * All short entries in the same full entry
         * must have the same MEP-type.  MEP type is
         * always read from sub-index 0, so that entry
         * must be added first.
         */
        SHR_ALLOC_SET_ZERO(leader_info, sizeof(dnx_oam_oamp_mep_db_args_t),
                           "Short entry read for MEP-type comparison", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

        leader_endpoint_id = oam_id - *sub_index;
        SHR_IF_ERR_EXIT(dnx_algo_oamp_mep_id_is_allocated(unit, leader_endpoint_id, is_leader_alloc));
        if (*is_leader_alloc == TRUE)
        {
            SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, leader_endpoint_id, leader_info));
            if (req_mep_type == leader_info->mep_type)
            {
                *is_leader_type_match = TRUE;
            }
            else
            {
                *is_leader_type_match = FALSE;
            }
        }
    }

exit:
    SHR_FREE(leader_info);
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition in oam_oamp.h for function description
 */
shr_error_e
dnx_oam_oamp_short_entry_type_verify(
    int unit,
    uint16 oam_id,
    const bcm_oam_endpoint_info_t * endpoint_info)
{
    int sub_index;
    dbal_enum_value_field_oamp_mep_type_e req_mep_type;
    uint8 is_leader_alloc, is_leader_type_match;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_oam_oamp_endpoint_bcm_mep_type_to_mep_db_mep_type(unit, endpoint_info->type, &req_mep_type));

    SHR_IF_ERR_EXIT(dnx_oam_oamp_short_entry_type_check_mep_db
                    (unit, oam_id, req_mep_type, &sub_index, &is_leader_alloc, &is_leader_type_match));

    if (sub_index != 0)
    {
        if (is_leader_alloc == FALSE)
        {
            /** Entry not found */
            SHR_ERR_EXIT(_SHR_E_PARAM, "Short entries with a non-zero sub-index can only "
                         "be added after the zero sub-index of the same full entry was "
                         "added, so you must first add entry 0x%08X before you can add entry "
                         "0x%08X", endpoint_info->id - sub_index, endpoint_info->id);
        }

        if (is_leader_type_match == FALSE)
        {
            /** MEP type is not the same */
            SHR_ERR_EXIT(_SHR_E_PARAM, "Short entries with a non-zero sub-index must have "
                         "the same MEP-type as the zero sub-index of the same full entry, "
                         "so entry 0x%08X must have the same MEP type as entry 0x%08X",
                         endpoint_info->id, endpoint_info->id - sub_index);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See h file for description
 */
shr_error_e
dnx_oamp_mep_db_extra_data_get(
    int unit,
    int index,
    uint8 *data)
{
    uint32 entry_id, next_bank_offset;
    int i;
    uint32 entry_handle_id, nof_entries;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    uint8 *tmp_data = NULL;

    SHR_ALLOC_SET_ZERO(tmp_data, DNX_OAM_MEP_DB_NOF_BITS_IN_HEADER_ENTRY / UTILEX_NOF_BITS_IN_BYTE,
                       "Array for over 32-bit dbal fields", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    next_bank_offset = MEP_DB_ENTRY_TO_OAM_ID(dnx_data_oam.oamp.nof_mep_db_entries_per_bank_get(unit));

    /** Create extra data header entry */
    entry_id = MEP_DB_ENTRY_TO_OAM_ID(index);

    /** Get table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MEP_DB, &entry_handle_id));

    /** Set key */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAMP_ENTRY_ID, entry_id);

    /** Get all fields   */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    /** get field values   */
    dbal_entry_handle_value_field_arr8_get(unit, entry_handle_id, DBAL_FIELD_EXTRA_DATA_SEGMENT, INST_SINGLE, tmp_data);

    dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_EXT_DATA_LENGTH, INST_SINGLE, &nof_entries);

    DBAL_HANDLE_FREE(unit, entry_handle_id);

    dnx_oamp_copy_bit_array(data, 0, tmp_data, 0, DNX_OAM_MEP_DB_NOF_BITS_IN_HEADER_ENTRY);

    /** get the extra data payload entries */

    for (i = 1; i != nof_entries; ++i)
    {
        int bits_to_copy = DNX_OAM_MEP_DB_NOF_BITS_IN_PAYLOAD_ENTRY, start_from = 0;

        /** get extra data header entry */
        entry_id += next_bank_offset;

        /** Get table */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MEP_DB, &entry_handle_id));

        /** Set key */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAMP_ENTRY_ID, entry_id);

        /** Get all fields   */
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

        /** get field values   */
        dbal_entry_handle_value_field_arr8_get(unit, entry_handle_id, DBAL_FIELD_EXTRA_DATA_SEGMENT, INST_SINGLE,
                                               tmp_data);

        DBAL_HANDLE_FREE(unit, entry_handle_id);

        dnx_oamp_copy_bit_array(data,
                                DNX_OAM_MEP_DB_NOF_BITS_IN_HEADER_ENTRY +
                                DNX_OAM_MEP_DB_NOF_BITS_IN_PAYLOAD_ENTRY * (i - 1), tmp_data, start_from, bits_to_copy);
    }

exit:
    SHR_FREE(tmp_data);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See h file for description
 */
shr_error_e
dnx_oamp_lm_with_priority_session_map_get(
    int unit,
    int oam_id,
    uint8 priority,
    uint32 *session_oam_id)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_LM_SESSION_DB, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_ID, oam_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY, priority);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MEP_ID, 0);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TEST_ID, 0);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OAM_ID, INST_SINGLE, session_oam_id);
    SHR_IF_ERR_EXIT_NO_MSG(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See h file for description
 */
shr_error_e
dnx_oamp_lm_with_priority_session_map_clear(
    int unit,
    int oam_id,
    uint8 priority)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_LM_SESSION_DB, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_ID, oam_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY, priority);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MEP_ID, 0);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TEST_ID, 0);

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See h file for description
 */
shr_error_e
dnx_oamp_mep_db_extra_data_set(
    int unit,
    int index,
    uint32 nof_data_bits,
    uint8 *data,
    int opcode,
    uint16 calculated_crc,
    uint8 is_update)
{
    dnx_oam_oamp_mep_db_args_t *oamp_params = NULL;
    dnx_oamp_mep_db_memory_type_t memory_type;
    uint32 entry_id, next_bank_offset, max_nof_endpoint_id;
    int i;
    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(oamp_params, sizeof(dnx_oam_oamp_mep_db_args_t), "Param struct for creating single entries",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    next_bank_offset = MEP_DB_ENTRY_TO_OAM_ID(dnx_data_oam.oamp.nof_mep_db_entries_per_bank_get(unit));
    max_nof_endpoint_id = dnx_data_oam.oamp.max_nof_endpoint_id_get(unit);

    /** Create extra data header entry */
    entry_id = MEP_DB_ENTRY_TO_OAM_ID(index);

    /** Allocate this entry */

    if (!is_update)
    {
        memory_type.is_2byte_maid = FALSE;
        memory_type.is_extra_pool = (entry_id >= max_nof_endpoint_id);
        memory_type.is_full = TRUE;
        SHR_IF_ERR_EXIT(dnx_algo_oamp_mep_id_alloc(unit, SW_STATE_ALGO_RES_ALLOCATE_WITH_ID, &memory_type, &entry_id));
    }

    if (is_update)
    {
        UTILEX_SET_BIT(oamp_params->flags, TRUE, OAMP_MEP_UPDATE);
    }
    oamp_params->mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_EXT_DATA_HDR;
    oamp_params->exclusive.extra_data.extra_data_len =
        (nof_data_bits - 1) / DNX_OAM_MEP_DB_NOF_BITS_IN_HEADER_ENTRY + 1;
    oamp_params->exclusive.extra_data.opcode_bmp = opcode;
    oamp_params->exclusive.extra_data.crc_val1 = calculated_crc;
    oamp_params->exclusive.extra_data.crc_val2 = 0;
    dnx_oamp_copy_bit_array(oamp_params->exclusive.extra_data.data_segment, 0, data, 0,
                            DNX_OAM_MEP_DB_NOF_BITS_IN_HEADER_ENTRY);
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_add(unit, entry_id, oamp_params));

    oamp_params->mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_EXT_DATA_PLD;

    /** Create the extra data payload entries */

    for (i = 1; i != oamp_params->exclusive.extra_data.extra_data_len; ++i)
    {
        int bits_left = 0, bits_to_copy = 0, start_from = 0;
        entry_id += next_bank_offset;
        /** Allocate the data entries */
        if (!is_update)
        {
            memory_type.is_extra_pool = (entry_id >= max_nof_endpoint_id);
            SHR_IF_ERR_EXIT(dnx_algo_oamp_mep_id_alloc
                            (unit, SW_STATE_ALGO_RES_ALLOCATE_WITH_ID, &memory_type, &entry_id));
        }
        bits_left =
            nof_data_bits - DNX_OAM_MEP_DB_NOF_BITS_IN_HEADER_ENTRY - DNX_OAM_MEP_DB_NOF_BITS_IN_PAYLOAD_ENTRY * (i -
                                                                                                                  1);
        if (bits_left < DNX_OAM_MEP_DB_NOF_BITS_IN_PAYLOAD_ENTRY)
        {
            bits_to_copy = bits_left;
            start_from = DNX_OAM_MEP_DB_NOF_BITS_IN_PAYLOAD_ENTRY - bits_left;
            /** Here, the entire buffer will not be filled, so clear it first */
            sal_memset(oamp_params->exclusive.extra_data.data_segment, 0,
                       sizeof(oamp_params->exclusive.extra_data.data_segment));
        }
        else
        {
            bits_to_copy = DNX_OAM_MEP_DB_NOF_BITS_IN_PAYLOAD_ENTRY;
        }
        dnx_oamp_copy_bit_array(oamp_params->exclusive.extra_data.data_segment,
                                start_from, data,
                                DNX_OAM_MEP_DB_NOF_BITS_IN_HEADER_ENTRY +
                                DNX_OAM_MEP_DB_NOF_BITS_IN_PAYLOAD_ENTRY * (i - 1), bits_to_copy);
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_add(unit, entry_id, oamp_params));

    }

exit:
    SHR_FREE(oamp_params);
    SHR_FUNC_EXIT;
}

/*
 * See h file for description
 */
shr_error_e
dnx_oamp_mep_db_48b_maid_set(
    int unit,
    bcm_oam_group_info_t * group_info,
    uint16 calculated_crc)
{
    uint8 *extra_header_data = NULL;
    uint32 index;
    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(extra_header_data, DNX_OAM_OAMP_EXTRA_DATA_MAX_SIZE,
                       "data allocated for extra header", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    index = DNX_OAM_OAMP_ITMH_EXT_SIZE + DNX_OAM_OAMP_PPH_SIZE;
    sal_memcpy(extra_header_data + index, group_info->name, BCM_OAM_GROUP_NAME_LENGTH);

    SHR_IF_ERR_EXIT(dnx_oamp_mep_db_extra_data_set
                    (unit, OAM_ID_TO_MEP_DB_ENTRY(group_info->group_name_index),
                     DNX_OAM_OAMP_ITMH_EXT_SIZE_BITS + DNX_OAM_OAMP_PPH_SIZE_BITS + DNX_OAM_OAMP_NOF_MAID_BITS,
                     extra_header_data, DBAL_DEFINE_OAM_OPCODE_CCM, calculated_crc, FALSE));
exit:
    SHR_FREE(extra_header_data);
    SHR_FUNC_EXIT;
}

/*
 * See h file for description
 */
shr_error_e
dnx_oamp_mep_db_48b_maid_clear(
    int unit,
    uint32 header_entry_index)
{
    uint32 next_bank_offset, current_entry;
    SHR_FUNC_INIT_VARS(unit);

    next_bank_offset = MEP_DB_ENTRY_TO_OAM_ID(dnx_data_oam.oamp.nof_mep_db_entries_per_bank_get(unit));
    current_entry = header_entry_index;

        /** Delete header entry */
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_delete(unit, current_entry));

    /** Release the OAMP_MEP_DB entry id */
    SHR_IF_ERR_EXIT(dnx_algo_oamp_mep_id_dealloc(unit, current_entry));

    /** Delete first payload entry */
    current_entry += next_bank_offset;
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_delete(unit, current_entry));

    /** Release the OAMP_MEP_DB entry id */
    SHR_IF_ERR_EXIT(dnx_algo_oamp_mep_id_dealloc(unit, current_entry));

    /** Delete second payload entry */
    current_entry += next_bank_offset;
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_delete(unit, current_entry));

    /** Release the OAMP_MEP_DB entry id */
    SHR_IF_ERR_EXIT(dnx_algo_oamp_mep_id_dealloc(unit, current_entry));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * This function calculates the number of scan counts required
 * for a given interval in a scan count of 6
 * scan counts.
 * 1 scan interval of MEP DB = 1.66 ms.
 */
static shr_error_e
dnx_oam_oamp_scan_count_req_in_6_count_cycle(
    int unit,
    dbal_enum_value_field_ccm_interval_e new_rate,
    uint32 *scan_count)
{
    SHR_FUNC_INIT_VARS(unit);

    if (new_rate == DBAL_ENUM_FVAL_CCM_INTERVAL_NO_TX)
    {
        /*
         * No TX. No scan required
         */
        *scan_count = 0;
    }
    else if (new_rate == DBAL_ENUM_FVAL_CCM_INTERVAL_3MS)
    {
        /*
         * Requires to be scanned every alternate cycle. 3.3ms/1.66ms = 2 scans for 1 packet (i.e) half of the entire
         * cycle number of counts. 6 * 1/2 = 3 scans.
         */
        *scan_count = 3;
    }
    else if (new_rate >= DBAL_ENUM_FVAL_CCM_INTERVAL_10MS && new_rate < DBAL_NOF_ENUM_CCM_INTERVAL_VALUES)
    {
        /*
         * If interval is 10ms or more, we just need max of one scan count since 10ms/1.66 ms = 6 scans for 1 packet.
         * (i.e) 1/6 of the entire cycle number of counts. 6 * 1/6 = 1 scan. For 100ms, 6 * 1/60 = 1 scan and so on.
         */
        *scan_count = 1;
    }
    else
    {
        /*
         * Invalid rate
         */
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid rate passed %d", new_rate);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * This function gives the phase count for an opcode
 * given the mep_profile_data.
 *
 */
static shr_error_e
dnx_oam_oamp_get_existing_phase_cnt_for_opcode(
    int unit,
    dnx_oam_oamp_tx_opcode opcode,
    const dnx_oam_mep_profile_t * mep_profile_data,
    uint32 *phase_cnt)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (opcode)
    {
        case OAMP_MEP_TX_OPCODE_CCM_BFD:
            *phase_cnt = mep_profile_data->ccm_count;
            break;
        case OAMP_MEP_TX_OPCODE_LMM:
            *phase_cnt = mep_profile_data->lmm_count;
            break;
        case OAMP_MEP_TX_OPCODE_DMM:
            *phase_cnt = mep_profile_data->dmm_count;
            break;
        case OAMP_MEP_TX_OPCODE_OP0:
            *phase_cnt = mep_profile_data->opcode_0_count;
            break;
        case OAMP_MEP_TX_OPCODE_OP1:
            *phase_cnt = mep_profile_data->opcode_1_count;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid opcode passed %d", opcode);
            break;
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * This function gives the rate interval in dbal_enum_value_field_ccm_interval_e
 * format given the mep_profile_data (which contains the rates in ccm_period format).
 *
 * NOTE: since ccm_rate is not available in mep_profile_data, we are taking it as
 * a separate param.
 */
static dbal_enum_value_field_ccm_interval_e
dnx_oam_oamp_get_transmit_rate_for_opcode(
    dnx_oam_oamp_tx_opcode opcode,
    dbal_enum_value_field_ccm_interval_e ccm_rate,
    const dnx_oam_mep_profile_t * mep_profile_data)
{
    dbal_enum_value_field_ccm_interval_e ccm_interval = DBAL_ENUM_FVAL_CCM_INTERVAL_NO_TX;

    switch (opcode)
    {
        case OAMP_MEP_TX_OPCODE_CCM_BFD:
            /*
             * Its already given in correct format
             */
            ccm_interval = ccm_rate;
            break;
        case OAMP_MEP_TX_OPCODE_LMM:
            dnx_oam_oamp_ccm_period_to_ccm_interval(mep_profile_data->lmm_rate, &ccm_interval);
            break;
        case OAMP_MEP_TX_OPCODE_DMM:
            dnx_oam_oamp_ccm_period_to_ccm_interval(mep_profile_data->dmm_rate, &ccm_interval);
            break;
        case OAMP_MEP_TX_OPCODE_OP0:
            dnx_oam_oamp_ccm_period_to_ccm_interval(mep_profile_data->opcode_0_rate, &ccm_interval);
            break;
        case OAMP_MEP_TX_OPCODE_OP1:
            dnx_oam_oamp_ccm_period_to_ccm_interval(mep_profile_data->opcode_1_rate, &ccm_interval);
            break;
        default:
            /*
             * Ideally should not happen. Anyways, we will just return NO_TX.
             */
            ccm_interval = DBAL_ENUM_FVAL_CCM_INTERVAL_NO_TX;
            break;
    }
    return ccm_interval;
}

/**
 * \brief
 * This function verifies whether we can fit in all the opcode packets
 * within the scan cycle of 6 scan counts based on their scan count requirements
 * got from their transmit rates.
 */
static shr_error_e
dnx_oam_oamp_scan_count_verify_max_per_mep(
    int unit,
    dnx_oam_oamp_tx_opcode opcode,
    dbal_enum_value_field_ccm_interval_e ccm_rate,
    dnx_oam_mep_profile_t * mep_profile_data)
{
    dnx_oam_oamp_tx_opcode opcode_i;
    uint32 total_scan_count = 0, scan_count;
    dbal_enum_value_field_ccm_interval_e rate;
    SHR_FUNC_INIT_VARS(unit);

    for (opcode_i = OAMP_MEP_TX_OPCODE_CCM_BFD; opcode_i < OAMP_MEP_TX_OPCODE_MAX_SUPPORTED; opcode_i++)
    {
        rate = dnx_oam_oamp_get_transmit_rate_for_opcode(opcode_i, ccm_rate, mep_profile_data);
        scan_count = 0;
        SHR_IF_ERR_EXIT(dnx_oam_oamp_scan_count_req_in_6_count_cycle(unit, rate, &scan_count));
        total_scan_count += scan_count;
    }

    if (total_scan_count > 6)
    {
        SHR_ERR_EXIT(_SHR_E_RESOURCE, "Unsupported number of packets requested to be transmitted from a single MEP");
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * This function verifies if the transmit rates of the opcodes
 * are supported and whether we can fit in all the opcode packets
 * within the scan cycle based on their scan count requirements
 * got from their transmit rates.
 */
static shr_error_e
dnx_oam_oamp_scan_count_calc_verify(
    int unit,
    uint8 is_short_entry_type,
    dbal_enum_value_field_ccm_interval_e ccm_rate,
    dnx_oam_oamp_tx_opcode opcode,
    dnx_oam_mep_profile_t * mep_profile_data)
{
    dnx_oam_oamp_tx_opcode other_opcode;
    dbal_enum_value_field_ccm_interval_e opcode_rate;
    dbal_enum_value_field_ccm_interval_e other_opcode_rate;
    SHR_FUNC_INIT_VARS(unit);

    if (is_short_entry_type)
    {
        if (opcode == OAMP_MEP_TX_OPCODE_CCM_BFD)
        {
            /*
             * Below verifications are not required for short entries
             */
            SHR_EXIT();
        }
        else
        {
            /*
             * Cannot really happen. Throw error
             */
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Short entry endpoints support only CCM %d", opcode);
        }
    }

    /*
     * 1) Currently we do not support a case where both opcode0 and opcode 1 both has non-zero rates. Check for that
     * case. 2) Also we support only 1s or above rates for these opcodes. Check that as well.
     */
    if (opcode == OAMP_MEP_TX_OPCODE_OP0 || opcode == OAMP_MEP_TX_OPCODE_OP1)
    {
        if (opcode == OAMP_MEP_TX_OPCODE_OP0)
        {
            other_opcode = OAMP_MEP_TX_OPCODE_OP1;
        }
        else
        {
            other_opcode = OAMP_MEP_TX_OPCODE_OP0;
        }

        opcode_rate =
            dnx_oam_oamp_get_transmit_rate_for_opcode(opcode, 0 /* dont-care for CCM rate */ , mep_profile_data);

        other_opcode_rate =
            dnx_oam_oamp_get_transmit_rate_for_opcode(other_opcode, 0 /* dont-care for CCM rate */ , mep_profile_data);

        if ((opcode_rate != DBAL_ENUM_FVAL_CCM_INTERVAL_NO_TX)
            && (other_opcode_rate != DBAL_ENUM_FVAL_CCM_INTERVAL_NO_TX))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "We do not support case of both opcode 0 and 1 rate being non-zero %d %d",
                         opcode_rate, other_opcode_rate);
        }

        if ((opcode_rate != DBAL_ENUM_FVAL_CCM_INTERVAL_NO_TX) && (opcode_rate < DBAL_ENUM_FVAL_CCM_INTERVAL_1S))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "We do not support case of opcode 0 or 1 rate being less than 1s %d", opcode_rate);
        }
    }

    SHR_IF_ERR_EXIT(dnx_oam_oamp_scan_count_verify_max_per_mep(unit, opcode, ccm_rate, mep_profile_data));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * This function calculates the phase counts for Opcode CCM
 * and updates it in the mep_profile_data.
 *
 * CCM/BFD can take phase counts 0 or 1
 */
static void
dnx_oam_oamp_scan_count_calc_ccm_bfd(
    int unit,
    bcm_oam_endpoint_t endpoint_id,
    uint8 is_short_entry_type,
    uint8 ccm_rx_without_tx,
    dnx_oam_mep_profile_t * mep_profile_data)
{
    if (ccm_rx_without_tx)
    {
        /*
         * When CCM period gets updated from non-zero to zero value, set ccm_cnt to 0x7ffff to stop the transmission of 
         * CCM packets. OAMP_MEP_DB.ccm_interval remains unchaged which allows processing of CCM packets by the OAMP 
         */
        mep_profile_data->ccm_count = DNX_OAM_OAMP_ETH1731_MEP_PROFILE_MAX_CCM_CNT;
    }
    else
    {
        if (is_short_entry_type)
        {
            /*
             * Short entry banks deal with only CCMs and their
             * scan rate is set to 3.3ms. So always set
             * phase count to 0
             */
            mep_profile_data->ccm_count = 0;
        }
        else
        {
            /*
             * The phase count has to be 1 or 0 based on
             * endpoint_id's 3rd bit from LSB (since each long entry
             * contains 4 short entries).
             */
            mep_profile_data->ccm_count = (endpoint_id >> 2) & 1;
        }
    }
}

/**
 * \brief
 * This function calculates the phase counts for Opcodes LMM and DMM
 * and updates it in the mep_profile_data.
 *
 * LMM/DMM can take phase counts 0/1 or 2/3 depending on CCM_CNT
 * 0 or 1 is used for opcodes with quicker intervals.
 * 2 or 3 is used for opcodes with slower intervals.
 *
 * Assumptions:
 * 1) ccm_count is already configured correctly,
 *    since CCM is configured before LMM/DMM.
 */
static shr_error_e
dnx_oam_oamp_scan_count_calc_lm_dm(
    int unit,
    dnx_oam_oamp_tx_opcode opcode,
    dnx_oam_mep_profile_t * mep_profile_data)
{
    dbal_enum_value_field_ccm_interval_e my_rate = 0, other_rate = 0;
    dnx_oam_oamp_tx_opcode other_opcode;
    uint32 my_cnt, other_cnt;
    uint8 ccm_transmit_phase_is_odd = 0;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Check if CCM occupies even or odd slot
     */
    ccm_transmit_phase_is_odd = (mep_profile_data->ccm_count == 0) ? 0 : 1;

    if (opcode == OAMP_MEP_TX_OPCODE_LMM)
    {
        other_opcode = OAMP_MEP_TX_OPCODE_DMM;
    }
    else if (opcode == OAMP_MEP_TX_OPCODE_DMM)
    {
        other_opcode = OAMP_MEP_TX_OPCODE_LMM;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected opcode in %s function - %d", __FUNCTION__, opcode);
    }

    /*
     * First take back up of my values (LMM/DMM) and other values (DMM/LMM)
     */
    my_rate = dnx_oam_oamp_get_transmit_rate_for_opcode(opcode, 0 /* dont-care about ccm rate */ , mep_profile_data);
    other_rate = dnx_oam_oamp_get_transmit_rate_for_opcode(other_opcode,
                                                           0 /* dont-care about ccm rate */ , mep_profile_data);
    SHR_IF_ERR_EXIT(dnx_oam_oamp_get_existing_phase_cnt_for_opcode(unit, opcode, mep_profile_data, &my_cnt));
    SHR_IF_ERR_EXIT(dnx_oam_oamp_get_existing_phase_cnt_for_opcode(unit, other_opcode, mep_profile_data, &other_cnt));

    if (my_rate == DBAL_ENUM_FVAL_CCM_INTERVAL_3MS)
    {
        /*
         * If we are running at 3.3ms, we need half of the scan cycles. So you need to take phase 0 or 1 depending on
         * what CCM took.
         */
        my_cnt = (ccm_transmit_phase_is_odd) ? 0 : 1;
        if (other_rate)
        {
            /*
             * If we have other rate non-zero, then that should mean CCM is NOT running at 3.3ms, since there cannot be
             * more than 2 opcodes running at 3.3ms. Since CCM is NOT running at 3.3ms, use CCM transmit phase's
             * oddness. Example: let us say, CCM = 10ms, DM = 3.3ms, LM = 10ms and let us say DM is getting configured
             * in this case, and assume CCM is taking phase 0, then it would be taking series 0,6,12,18 etc for 10ms
             * rate.... So we need to configure DM with 1 since DM has to take half of the cycles (1,3,5,7,... so on) So
             * we need to configure LM with phase 2 to take the series 2,8,14,20 etc... So if CCM is odd, we need to
             * take odd for other guy.  if CCM is even, we need to take even for other guy.
             */
            other_cnt = (ccm_transmit_phase_is_odd) ? 3 : 2;
        }
    }
    else
    {
        /*
         * Just take the standard allocation if you are running at 10ms or above. The quicker of 2 rates will take
         * phase 0 or 1 depending on CCM_CNT. The slower of 2 rates will take phase 2 or 3 depending on CCM_CNT. It
         * doesnt really matter whether CCM is running at 3.3ms or not in this case. NOTE: In
         * dbal_enum_value_field_ccm_interval_e enum, the smaller value means quicker, except obviously for 0.
         */
        if (my_rate)    /* non-zero rate */
        {
            if ((other_rate) && (my_rate < other_rate))
            {
                /*
                 * My rate quicker than other rate. I get 0 or 1. Other gets 2 or 3
                 */
                my_cnt = (ccm_transmit_phase_is_odd) ? 0 : 1;
                other_cnt = (ccm_transmit_phase_is_odd) ? 2 : 3;
            }
            else if ((other_rate) && (my_rate >= other_rate))
            {
                /*
                 * My rate slower (or equal) than other rate. I get 2 or 3. Other gets 0 or 1.
                 */
                my_cnt = (ccm_transmit_phase_is_odd) ? 2 : 3;
                other_cnt = (ccm_transmit_phase_is_odd) ? 0 : 1;
            }
            else
            {
                /*
                 * Means other rate is 0. In which case I just take 0 or 1 and other gets 2 or 3.
                 */
                my_cnt = (ccm_transmit_phase_is_odd) ? 0 : 1;
                other_cnt = (ccm_transmit_phase_is_odd) ? 2 : 3;
            }
        }
        /*
         * If my rate is zero, then it does not matter what phase count I hold. I need not change other phase count
         * also.
         */
    }

    /*
     * Update the actual cnts with newly calculated values
     */
    if (opcode == OAMP_MEP_TX_OPCODE_LMM)
    {
        mep_profile_data->lmm_count = my_cnt;
        mep_profile_data->dmm_count = other_cnt;
    }
    else if (opcode == OAMP_MEP_TX_OPCODE_DMM)
    {
        mep_profile_data->dmm_count = my_cnt;
        mep_profile_data->lmm_count = other_cnt;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * This function calculates the phase counts for Opcodes OPCODE_0 and OPCODE_1
 * and updates it in the mep_profile_data.
 *
 * Assumptions:
 * 1) opcode 0 rate and 1 rate
 * will not be non-zero together.
 * 2) Opcode 0 and 1 rate are generally 1s and above.
 * So they will take 4 or 5 depending on CCM_CNT.
 */
static shr_error_e
dnx_oam_oamp_scan_count_calc_opcode_x(
    int unit,
    dnx_oam_oamp_tx_opcode opcode,
    dnx_oam_mep_profile_t * mep_profile_data)
{
    uint8 ccm_transmit_phase_is_odd = 0;
    dbal_enum_value_field_ccm_interval_e tx_rate;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Check if CCM occupies even or odd slot
     */
    ccm_transmit_phase_is_odd = (mep_profile_data->ccm_count == 0) ? 0 : 1;

    if ((opcode != OAMP_MEP_TX_OPCODE_OP0) && (opcode != OAMP_MEP_TX_OPCODE_OP1))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected opcode in %s function - %d", __FUNCTION__, opcode);
    }

    tx_rate = dnx_oam_oamp_get_transmit_rate_for_opcode(opcode, 0 /* dont-care about ccm rate */ , mep_profile_data);

    /*
     * Just take the standard allocation 4 or 5 depending on CCM_CNT
     */
    if (tx_rate)        /* non-zero rate */
    {
        if (opcode == OAMP_MEP_TX_OPCODE_OP0)
        {
            mep_profile_data->opcode_0_count = (ccm_transmit_phase_is_odd) ? 4 : 5;
        }
        else
        {
            mep_profile_data->opcode_1_count = (ccm_transmit_phase_is_odd) ? 4 : 5;
        }
    }
    /*
     * If my rate is zero, then it does not matter what phase count I hold.
     */

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * This function calculates the phase counts for given opcode in the opcode parameter
 * and updates it in the mep_profile_data. It also changes the other opcodes' phase counts
 * if necessary.
 */
shr_error_e
dnx_oam_oamp_scan_count_calc(
    int unit,
    bcm_oam_endpoint_t endpoint_id,
    uint8 is_short_entry_type,
    dbal_enum_value_field_ccm_interval_e ccm_rate,
    uint8 ccm_rx_without_tx,
    dnx_oam_oamp_tx_opcode opcode,
    dnx_oam_mep_profile_t * mep_profile_data)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Verify
     */
    SHR_IF_ERR_EXIT(dnx_oam_oamp_scan_count_calc_verify(unit, is_short_entry_type, ccm_rate, opcode, mep_profile_data));

    switch (opcode)
    {
        case OAMP_MEP_TX_OPCODE_CCM_BFD:
            dnx_oam_oamp_scan_count_calc_ccm_bfd(unit, endpoint_id, is_short_entry_type, ccm_rx_without_tx,
                                                 mep_profile_data);
            break;
        case OAMP_MEP_TX_OPCODE_LMM:
        case OAMP_MEP_TX_OPCODE_DMM:
            SHR_IF_ERR_EXIT(dnx_oam_oamp_scan_count_calc_lm_dm(unit, opcode, mep_profile_data));
            break;
        case OAMP_MEP_TX_OPCODE_OP0:
        case OAMP_MEP_TX_OPCODE_OP1:
            SHR_IF_ERR_EXIT(dnx_oam_oamp_scan_count_calc_opcode_x(unit, opcode, mep_profile_data));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected opcode in %s function - %d", __FUNCTION__, opcode);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}
