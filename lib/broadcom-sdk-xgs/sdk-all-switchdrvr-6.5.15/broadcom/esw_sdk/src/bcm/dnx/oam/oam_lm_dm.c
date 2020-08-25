
/** \file oam_lm_dm.c
 * $Id$
 *
 * OAM LM DM procedures for DNX.
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
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_oam.h>

#include <bcm/oam.h>

#include <bcm_int/dnx_dispatch.h>
#include <bcm_int/dnx/algo/oam/algo_oam.h>
#include <bcm_int/dnx/algo/oamp/algo_oamp.h>

#include <src/bcm/dnx/oam/oam_internal.h>
#include <src/bcm/dnx/oam/oam_oamp.h>
#include <soc/dnx/swstate/auto_generated/access/algo_oam_access.h>
#include <soc/dnx/dnx_err_recovery_manager.h>

/*
 * Check if OAMP_MEP_DB.MEPTYPE is of type MPLS TP or PWE.
 */
#define DNX_OAM_MEP_TYPE_IS_MPLS_TP_OR_PWE(mep_type) (mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_Y1731_MPLSTP ||\
                                                      mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_Y1731_PWE)

/*
 * Given the DM MEP_DB entry info of type dnx_oam_oamp_endpoint_dm_params_t
 * and info whether it is an offloaded or self contained endpoint, 
 * get the DM_STAT/OFFLOADED entry part 2 index.
 */
#define DNX_OAM_GET_DELAY_ENTRY_CONFIGURED(offloaded,dm_mep_db_entry,delay_entry_ptr)\
do\
{\
    if (offloaded) {\
        /* For offloaded, flex_lm_dm_ptr gives the part2_ptr.\
         */\
        delay_entry_ptr = dm_mep_db_entry->flex_dm_entry;\
    } else {\
        /* For selfcontained, it is given by dm_stat_entry_idx */\
        delay_entry_ptr = dm_mep_db_entry->dm_stat_entry_idx;\
    }\
}while(0)

/*
 * Given the LM MEP_DB entry info of type dnx_oam_oamp_endpoint_lm_params_t
 * and info whether it is an offloaded or self contained endpoint, 
 * get the LM_DB/OFFLOADED entry part2 index.
 */
#define DNX_OAM_GET_LOSS_ENTRY_CONFIGURED(offloaded,lm_mep_db_entry,lm_db_entry_ptr)\
do\
{\
    if (offloaded) {\
        /* For offloaded, flex_lm_dm_ptr gives the part2_ptr.\
         */\
        lm_db_entry_ptr = lm_mep_db_entry->flex_lm_entry;\
    } else {\
        /* For selfcontained, it is given by lm_db_entry_idx */\
        lm_db_entry_ptr = lm_mep_db_entry->lm_db_entry_idx;\
    }\
}while(0)

/*
 * Given the LM MEP_DB entry info of type dnx_oam_oamp_endpoint_lm_params_t
 * for a self contained endpoint, get the LM_STAT entry part2 index.
 */
#define DNX_OAM_GET_LOSS_STAT_ENTRY_CONFIGURED(lm_mep_db_entry,lm_stat_entry_ptr)\
do\
{\
    /* For selfcontained, it is given by lm_stat_entry_idx */\
    lm_stat_entry_ptr = lm_mep_db_entry->lm_stat_entry_idx;\
}while(0)

/* Next index is separated by number of entries in a bank. */
#define DNX_OAM_GET_NEXT_LM_DM_CHAIN_INDEX(entry_id) (entry_id + \
        dnx_data_oam.oamp.mep_db_nof_sub_entries_get(unit) * dnx_data_oam.oamp.nof_mep_db_entries_per_bank_get(unit))

/** Flags used to indicate if oam oamp profiles should be updated */
#define DNX_OAM_LM_DM_MAC_DA_MSB_PROFILE             (1 << 0)
#define DNX_OAM_LM_DM_MAC_DA_LSB_PROFILE             (1 << 1)
#define DNX_OAM_LM_DM_OAMP_MEP_PROFILE               (1 << 2)

/* HW write data for loss/delay configuration */
typedef struct _dnx_oam_loss_delay_oamp_hw_write_data_s
{
    /*
     * Flags indicating which profiles should be updated in HW 
     */
    uint32 flags;

    /*
     * The OUI profile 
     */
    uint8 oamp_da_msb_prof;
    /*
     * The OUI (MSB) value 
     */
    bcm_mac_t oamp_mac_da_msb;

    /*
     * The NIC profile 
     */
    uint8 oamp_da_lsb_prof;
    /*
     * The NIC (LSB) value 
     */
    bcm_mac_t oamp_mac_da_lsb;

    /*
     * The MEP profile 
     */
    uint8 oamp_mep_profile;
    /*
     * The MEP profile data 
     */
    dnx_oam_mep_profile_t mep_profile_data;

    /*
     * CCM MEP DB index 
     */
    uint16 ccm_oam_id;
    /*
     * Additional statistics enabled ? 
     */
    uint8 stat_enable;
    /*
     * Is it offloaded endpoint ? 
     */
    uint8 offloaded;
    /*
     * Part 2 entry in case of offloaded lm_db/dm_stat entry in case of self_contained 
     */
    uint16 flex_lm_dm_entry;
    /*
     * Update mep db 
     */
    uint8 update_mep_db;

    /*
     * 1DM indication 
     */
    uint8 is_1dm;
} dnx_oam_loss_delay_oamp_hw_write_data_t;

/**
 * \brief - Verify the MEP DB index (Loss_ID/Delay_ID) 
 *          in the loss/delay information structure.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] loss_delay_ptr - Pointer to the structure,
 *                 containing information about the loss/delay entry.
 * \param [in] ccm_mep_db_entry - Pointer to the CCM MEP DB structure
 * \param [in] lm_0_dm_1 - Indicate caller LM = 0, DM = 1
 * \param [in] other_exists - Indicates other entry exists.
 *   
 * \return
 *   shr_error_e 
 *   
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_lm_dm_add_mep_db_entry_verify(
    int unit,
    const void *loss_delay_ptr,
    const dnx_oam_oamp_ccm_endpoint_t * ccm_mep_db_entry,
    uint8 lm_0_dm_1,
    uint8 other_exists)
{
    uint32 flags = 0, with_id_flag = 0;
    uint32 mep_db_idx = 0;
    bcm_oam_loss_t *loss_ptr = NULL;
    bcm_oam_delay_t *delay_ptr = NULL;
    char entry_type_str[6] = { 0 };
    char other_entry_type_str[6] = { 0 };
    uint8 update = 0, offloaded = 0;
    uint32 mep_db_threshold = 0;
    dnx_oam_oamp_mep_db_args_t mep_db_entry;
    dbal_enum_value_field_oamp_mep_type_e entry_type, exp_entry_type;
    SHR_FUNC_INIT_VARS(unit);

    offloaded = (ccm_mep_db_entry->flags & DNX_OAMP_OAM_CCM_MEP_LM_DM_OFFLOADED) ? 1 : 0;

    if (!lm_0_dm_1)
    {
        /*
         * Loss case 
         */
        loss_ptr = (bcm_oam_loss_t *) loss_delay_ptr;
        flags = loss_ptr->flags;
        with_id_flag = BCM_OAM_LOSS_WITH_ID;
        mep_db_idx = loss_ptr->loss_id;
        sal_strncpy(entry_type_str, "loss", 4);
        sal_strncpy(other_entry_type_str, "delay", 5);
        update = (loss_ptr->flags & BCM_OAM_LOSS_UPDATE) ? 1 : 0;
        /*
         * Expected entry type to check in case of update 
         */
        if (offloaded)
        {
            exp_entry_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_OFFLOADED_SECOND_ENTRY;
        }
        else
        {
            exp_entry_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_LM_DB;
        }
    }
    else
    {
        /*
         * Delay case 
         */
        delay_ptr = (bcm_oam_delay_t *) loss_delay_ptr;
        flags = delay_ptr->flags;
        with_id_flag = BCM_OAM_DELAY_WITH_ID;
        mep_db_idx = delay_ptr->delay_id;
        sal_strncpy(entry_type_str, "delay", 5);
        sal_strncpy(other_entry_type_str, "loss", 4);
        update = (delay_ptr->flags & BCM_OAM_DELAY_UPDATE) ? 1 : 0;
        /*
         * Expected entry type to check in case of update 
         */
        if (offloaded)
        {
            exp_entry_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_OFFLOADED_SECOND_ENTRY;
        }
        else
        {
            if (flags & BCM_OAM_DELAY_ONE_WAY)
            {
                exp_entry_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_DM_STAT_ONE_WAY;
            }
            else
            {
                exp_entry_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_DM_STAT;
            }
        }
    }

    /*
     * Verify if the loss_id or delay_id given is above threshold.
     * If other entry exists, the ID should be 0
     */
    mep_db_threshold = dnx_data_oam.oamp.oamp_mep_full_entry_threshold_get(unit);
    if (!other_exists && mep_db_idx <= mep_db_threshold)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: %s ID needs to be above mep_db_threshold.\n", entry_type_str);
    }

    /*
     * Verify if loss_id or delay_id is required or not 
     */
    if (!other_exists)
    {
        /*
         * Self contained: User needs to provide LM/DM ID if there is no other LM/DM entry. 
         */
        /*
         * Offloaded LM/DM: User needs to provide Part 2 LM/DM ID if there is no other LM/DM entry. 
         */
        if (!(flags & with_id_flag) || !(mep_db_idx))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error: %s with valid ID is mandatory.\n", entry_type_str);
        }
    }
    else
    {
        /*
         * Self contained: If there is other entry, we will figure out the rest of the entries from there. User
         * should not provide the ID. 
         */
        /*
         * Offloaded: If there is other entry, we dont need to alloc any entry. User should not provide the ID. 
         */
        if ((flags & with_id_flag) || (mep_db_idx))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error: %s already exists. ID is not required.\n", other_entry_type_str);
        }
    }

    if (update)
    {
        /*
         * Read the mep db entry to see if the correct LM/DM type is configured. If not throw an error 
         */
        sal_memset(&mep_db_entry, 0, sizeof(mep_db_entry));
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_db_get(unit, mep_db_idx, &mep_db_entry));
        entry_type = mep_db_entry.mep_type;
        if (entry_type != exp_entry_type)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error: Entry type mismatch.\n");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify the loss information structure 
 *          for correct parameters.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] loss_ptr - Pointer to the structure,
 *                 containing information about the loss entry.
 * \param [in] ccm_mep_db_entry - Pointer to the CCM MEP DB structure
 * \param [in] dm_exists - Indicates whether DM entry exists.
 * \param [out] priority - Priority in case of LM with priority 
 *        is being used
 *   
 * \return
 *   shr_error_e 
 *   
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_loss_add_verify(
    int unit,
    const bcm_oam_loss_t * loss_ptr,
    const dnx_oam_oamp_ccm_endpoint_t * ccm_mep_db_entry,
    uint8 dm_exists,
    int *priority)
{
    uint32 correct_flags = 0;
    uint8 mpls_pwe_mep = 0;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * priority verification for lm with priority feature
     */
    if (dnx_data_oam.oamp.lmm_dmm_supported_get(unit) == 0) 
    {
       SHR_ERR_EXIT(_SHR_E_PARAM,"Loss add is not supported on this device.");
    }

    /*
     * Loss add is not supported on quarter entry endpoints 
     */
    if (ccm_mep_db_entry->flags & DNX_OAMP_OAM_CCM_MEP_Q_ENTRY)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: Invalid memory type.\n");

    }

    /*
     * Flags verification 
     */
    correct_flags |= BCM_OAM_LOSS_SINGLE_ENDED | BCM_OAM_LOSS_STATISTICS_EXTENDED |
        BCM_OAM_LOSS_WITH_ID | BCM_OAM_LOSS_UPDATE | BCM_OAM_LOSS_REPORT_MODE | BCM_OAM_LOSS_SLM;

    if (loss_ptr->flags & ~correct_flags)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: Illegal flags set\n");
    }

    if (loss_ptr->loss_farend ||  loss_ptr->loss_nearend || loss_ptr->gport || loss_ptr->int_pri || loss_ptr->loss_threshold ||
        loss_ptr->pkt_dp_bitmap || loss_ptr->pkt_pri || loss_ptr->rx_farend || loss_ptr->rx_nearend ||
        loss_ptr->rx_oam_packets || loss_ptr->tx_farend || loss_ptr->tx_nearend || loss_ptr->tx_oam_packets)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: Illegal field set.\n");
    }

    mpls_pwe_mep = DNX_OAM_MEP_TYPE_IS_MPLS_TP_OR_PWE(ccm_mep_db_entry->mep_type);

    /*
     * Piggy back cases and MPLS/PWE endpoints - DA needs to be zero
     */
    if (((loss_ptr->flags & BCM_OAM_LOSS_SINGLE_ENDED) == 0) || (mpls_pwe_mep))
    {
        if (!BCM_MAC_IS_ZERO(loss_ptr->peer_da_mac_address))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error: DA MAC needs to be zero.\n");
        }
    }

    switch (loss_ptr->pkt_pri_bitmap) 
    {
       case 0:
       case 255:
       case 1: *priority=0; break;
       case 2: *priority=1; break;
       case 4: *priority=2; break;
       case 8: *priority=3; break;
       case 16: *priority=4; break;
       case 32: *priority=5; break;
       case 64: *priority=6; break;
       case 128: *priority=7; break;
       default:  SHR_ERR_EXIT(_SHR_E_PARAM,"Loss with priority may be defined for one priority at a time. Please consider multiple bcm_oam_loss_add");
    }

    /*
     * Call mep db index verify function 
     */
    SHR_IF_ERR_EXIT(dnx_oam_lm_dm_add_mep_db_entry_verify(unit, loss_ptr, ccm_mep_db_entry, 0 /* LM */ , dm_exists));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Alloc/exchange a profile for the LMM/DMM DA NIC.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] oamp_mac_da_lsb - LSB part of MAC DA 
 * \param [in] old_oamp_mac_da_lsb - LSB part of existing MAC DA 
 * \param [in] old_oamp_da_lsb_prof - The existing MAC DA LSB profile
 * \param [in] exchange - Whether profile needs to be exchanged/allocated 
 * \param [out] hw_write_data - Pointer to the structure,
 *                 containing information about the 
 *                 OAMP LMM DMM HW data 
 *   
 * \return
 *   shr_error_e 
 *   
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_oamp_mac_da_lsb_profile_alloc(
    int unit,
    bcm_mac_t oamp_mac_da_lsb,
    bcm_mac_t old_oamp_mac_da_lsb,
    uint8 old_oamp_da_lsb_prof,
    uint8 exchange,
    dnx_oam_loss_delay_oamp_hw_write_data_t * hw_write_data)
{
    int oamp_da_lsb_prof = 0;
    uint8 write_hw = 0, delete_old_prof = 0;
    SHR_FUNC_INIT_VARS(unit);

    if (exchange)
    {
        /*
         * If the LSB is different, update the profile 
         */
        if (sal_memcmp(old_oamp_mac_da_lsb, oamp_mac_da_lsb, sizeof(bcm_mac_t)))
        {
            /*
             * Exchange the old profile with new profile. 
             */
            SHR_IF_ERR_EXIT(algo_oam_db.oam_da_mac_lsb_profile_template.exchange(unit, _SHR_CORE_ALL,
                                                                                 0 /* No flags */ , oamp_mac_da_lsb,
                                                                                 old_oamp_da_lsb_prof, NULL     /* No
                                                                                                                 * extra 
                                                                                                                 * * *
                                                                                                                 * * *
                                                                                                                 * * *
                                                                                                                 * * *
                                                                                                                 * * *
                                                                                                                 * * *
                                                                                                                 * * *
                                                                                                                 * * *
                                                                                                                 * * *
                                                                                                                 * * *
                                                                                                                 * * *
                                                                                                                 * * *
                                                                                                                 * *
                                                                                                                 * args 
                                                                                                                 */ ,
                                                                                 &(oamp_da_lsb_prof), &write_hw,
                                                                                 &delete_old_prof));
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(algo_oam_db.oam_da_mac_lsb_profile_template.allocate_single(unit, _SHR_CORE_ALL, 0      /* No
                                                                                                                 * flags 
                                                                                                                 */ , oamp_mac_da_lsb, NULL
                                                                                    /*
                                                                                     * No extra args 
                                                                                     */ ,
                                                                                    &(oamp_da_lsb_prof), &write_hw));
        delete_old_prof = 0;
    }

    hw_write_data->oamp_da_lsb_prof = oamp_da_lsb_prof;
    sal_memcpy(hw_write_data->oamp_mac_da_lsb, oamp_mac_da_lsb, sizeof(bcm_mac_t));
    if (write_hw)
    {
        hw_write_data->flags |= DNX_OAM_LM_DM_MAC_DA_LSB_PROFILE;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Alloc/exchange a profile for the LMM/DMM DA OUI.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] oamp_mac_da_msb - MSB part of MAC DA 
 * \param [in] old_oamp_mac_da_msb - MSB part of existing MAC DA 
 * \param [in] old_oamp_da_msb_prof - The existing MAC DA MSB profile
 * \param [in] exchange - Whether profile needs to be exchanged/allocated 
 * \param [out] hw_write_data - Pointer to the structure,
 *                 containing information about the 
 *                 OAMP LMM DMM HW data 
 *   
 * \return
 *   shr_error_e 
 *   
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_oamp_mac_da_msb_profile_alloc(
    int unit,
    bcm_mac_t oamp_mac_da_msb,
    bcm_mac_t old_oamp_mac_da_msb,
    uint8 old_oamp_da_msb_prof,
    uint8 exchange,
    dnx_oam_loss_delay_oamp_hw_write_data_t * hw_write_data)
{
    int oamp_da_msb_prof = 0;
    uint8 write_hw = 0, delete_old_prof = 0;
    SHR_FUNC_INIT_VARS(unit);

    if (exchange)
    {
        /*
         * If the MSB is different, update the profile 
         */
        if (old_oamp_mac_da_msb != oamp_mac_da_msb)
        {
            /*
             * Exchange the old profile with new profile. 
             */
            SHR_IF_ERR_EXIT(algo_oam_db.oam_da_mac_msb_profile_template.exchange(unit, _SHR_CORE_ALL,
                                                                                 0 /* No flags */ , oamp_mac_da_msb,
                                                                                 old_oamp_da_msb_prof, NULL     /* No
                                                                                                                 * extra 
                                                                                                                 * * *
                                                                                                                 * * *
                                                                                                                 * * *
                                                                                                                 * * *
                                                                                                                 * * *
                                                                                                                 * * *
                                                                                                                 * * *
                                                                                                                 * * *
                                                                                                                 * * *
                                                                                                                 * * *
                                                                                                                 * * *
                                                                                                                 * * *
                                                                                                                 * *
                                                                                                                 * args 
                                                                                                                 */ ,
                                                                                 &(oamp_da_msb_prof), &write_hw,
                                                                                 &delete_old_prof));
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(algo_oam_db.oam_da_mac_msb_profile_template.allocate_single(unit, _SHR_CORE_ALL, 0      /* No
                                                                                                                 * flags 
                                                                                                                 */ , oamp_mac_da_msb, NULL
                                                                                    /*
                                                                                     * No extra args 
                                                                                     */ ,
                                                                                    &(oamp_da_msb_prof), &write_hw));
        delete_old_prof = 0;
    }

    hw_write_data->oamp_da_msb_prof = oamp_da_msb_prof;
    sal_memcpy(hw_write_data->oamp_mac_da_msb, oamp_mac_da_msb, sizeof(bcm_mac_t));
    if (write_hw)
    {
        hw_write_data->flags |= DNX_OAM_LM_DM_MAC_DA_MSB_PROFILE;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Fill OAMP MEP profile data based on Loss information.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] loss_ptr - Pointer to the structure,
 *                 containing information about the delay entry.
 * \param [in] ccm_mep_db_entry - Pointer to CCM MEP DB entry contents.
 * \param [out] mep_profile_data - Pointer to the structure,
 *                 containing information about the 
 *                 OAMP MEP PROFILE content.
 *   
 * \return
 *   shr_error_e 
 *   
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_oamp_loss_info_to_mep_profile_data(
    int unit,
    const bcm_oam_loss_t * loss_ptr,
    const dnx_oam_oamp_ccm_endpoint_t * ccm_mep_db_entry,
    dnx_oam_mep_profile_t * mep_profile_data)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Fill up with LM params 
     */
    mep_profile_data->piggy_back_lm = ((loss_ptr->flags & BCM_OAM_LOSS_SINGLE_ENDED) == 0);
    mep_profile_data->slm_lm = ((loss_ptr->flags & BCM_OAM_LOSS_SLM) != 0);
    mep_profile_data->report_mode_lm = ((loss_ptr->flags & BCM_OAM_LOSS_REPORT_MODE) != 0);
    mep_profile_data->lmm_rate = loss_ptr->period;
    /*
     * Setting LMM/LMR offsets here since there could be change in piggyback and SLM 
     */
    DNX_OAM_SET_LMM_OFFSET(ccm_mep_db_entry->mep_type,
                           ccm_mep_db_entry->nof_vlan_tags,
                           mep_profile_data->piggy_back_lm, mep_profile_data->slm_lm, mep_profile_data->lmm_offset);
    DNX_OAM_SET_LMR_OFFSET(ccm_mep_db_entry->mep_type,
                           ccm_mep_db_entry->nof_vlan_tags,
                           mep_profile_data->piggy_back_lm, mep_profile_data->slm_lm, mep_profile_data->lmr_offset);

    /*
     * Setting phase count for LMM
     */
    SHR_IF_ERR_EXIT(dnx_oam_oamp_scan_count_calc(unit, 0 /* dont care for opcodes other than CCM */ ,
                                                 (ccm_mep_db_entry->flags & DNX_OAMP_OAM_CCM_MEP_Q_ENTRY),
                                                 ccm_mep_db_entry->ccm_interval, 0 /* don't care for opcodes other than CCM */,
                                                 OAMP_MEP_TX_OPCODE_LMM, mep_profile_data));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Remove loss information from OAMP MEP profile data.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] ccm_mep_db_entry - Pointer to CCM MEP DB entry contents.
 * \param [out] mep_profile_data - Pointer to the structure,
 *                 containing information about the 
 *                 OAMP MEP PROFILE content.
 *   
 * \return
 *   shr_error_e 
 *   
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_oamp_remove_loss_info_from_mep_profile_data(
    int unit,
    const dnx_oam_oamp_ccm_endpoint_t * ccm_mep_db_entry,
    dnx_oam_mep_profile_t * mep_profile_data)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Remove the LM params 
     */
    mep_profile_data->piggy_back_lm = 0;
    mep_profile_data->slm_lm = 0;
    mep_profile_data->report_mode_lm = 0;
    mep_profile_data->lmm_rate = 0;
    /*
     * Setting LMM/LMR offsets here since there is change in piggyback and SLM 
     */
    DNX_OAM_SET_LMM_OFFSET(ccm_mep_db_entry->mep_type,
                           ccm_mep_db_entry->nof_vlan_tags, 0, 0, mep_profile_data->lmm_offset);
    DNX_OAM_SET_LMR_OFFSET(ccm_mep_db_entry->mep_type,
                           ccm_mep_db_entry->nof_vlan_tags, 0, 0, mep_profile_data->lmr_offset);
    /*
     * Setting phase count for LMM
     */
    SHR_IF_ERR_EXIT(dnx_oam_oamp_scan_count_calc(unit, 0 /* dont care for opcodes other than CCM */ ,
                                                 (ccm_mep_db_entry->flags & DNX_OAMP_OAM_CCM_MEP_Q_ENTRY),
                                                 ccm_mep_db_entry->ccm_interval, 0 /* don't care for opcodes other than CCM */,
                                                 OAMP_MEP_TX_OPCODE_LMM, mep_profile_data));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -  Exchange the profile with currently allocated profile 
 *           (during endpoint create) and update the HW write data
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] old_mep_profile_id - The existing MEP Profile.
 * \param [in] mep_profile_data  - The MEP profile data to be written.
 * \param [out] hw_write_data - Pointer to the structure,
 *                 containing information about the 
 *                 OAMP LMM DMM HW data
 *   
 * \return
 *   shr_error_e 
 *   
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_oamp_mep_profile_exchange(
    int unit,
    uint8 old_mep_profile_id,
    const dnx_oam_mep_profile_t * mep_profile_data,
    dnx_oam_loss_delay_oamp_hw_write_data_t * hw_write_data)
{
    int new_mep_profile = 0;
    uint8 write_hw = 0, delete_old_prof = 0;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Update the MEP profile by exchanging 
     */
    SHR_IF_ERR_EXIT(algo_oam_db.oam_mep_profile.exchange(unit, _SHR_CORE_ALL, 0 /* No flags */ , mep_profile_data, old_mep_profile_id, NULL     /* No 
                                                                                                                                                 * extra 
                                                                                                                                                 * args 
                                                                                                                                                 */ ,
                                                         &new_mep_profile, &write_hw, &delete_old_prof));

    sal_memcpy(&(hw_write_data->mep_profile_data), mep_profile_data, sizeof(dnx_oam_mep_profile_t));
    hw_write_data->oamp_mep_profile = new_mep_profile;
    if (write_hw)
    {
        hw_write_data->flags |= DNX_OAM_LM_DM_OAMP_MEP_PROFILE;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Free up the MEP DB indexes for LM
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] loss_id - MEP DB index of the LM DB entry
 * \param [in] ccm_mep_db_entry - Pointer to the CCM MEP DB structure
 * \param [in] dm_exists - Whether DM already exists or not.
 * \param [in] dm_entry_idx - Index of DM entry if exists.
 * \param [in] lm_stat_exists - Whether LM_STAT exists or not.
 * \param [in] lm_stat_entry_idx- If statistics is enabled on self
 *                                contained endpoint, the entry ID.
 *   
 * \return
 *   shr_error_e 
 *   
 * \see
 *   * None
 */

static shr_error_e
dnx_oam_loss_mep_id_free(
    int unit,
    uint32 loss_id,
    const dnx_oam_oamp_ccm_endpoint_t * ccm_mep_db_entry,
    uint8 dm_exists,
    uint32 dm_entry_idx,
    uint8 lm_stat_exists,
    uint32 lm_stat_entry_idx)
{
    uint32 entry[2];            /* Max 2 entries only. */
    int num_entries = 0, entry_id;
    uint8 offloaded = 0;
    SHR_FUNC_INIT_VARS(unit);

    offloaded = (ccm_mep_db_entry->flags & DNX_OAMP_OAM_CCM_MEP_LM_DM_OFFLOADED) ? 1 : 0;

    if (offloaded)
    {
        if (!dm_exists)
        {
            /*
             * part2 ptr 
             */
            entry[0] = loss_id;
            num_entries++;
        }       /* Else If other entry exists, we have nothing to do */
    }
    else        /* Self contained */
    {
        if (dm_exists && (ccm_mep_db_entry->flex_lm_dm_entry != dm_entry_idx))
        {
            /*
             * LM entries are first.
             * DM entry will be moved to LM_DB entry index.
             * Deallocate DM entry
             */
            entry[0] = dm_entry_idx;
            num_entries++;
        }
        else
        {
            /*
             * DM entry is first.
             * Deallocate LM_DB entry
             */
            entry[0] = loss_id;
            num_entries++;
        }

        if (lm_stat_entry_idx)
        {
            entry[1] = lm_stat_entry_idx;
            num_entries++;
        }
    }

    for (entry_id = 0; entry_id < num_entries; entry_id++)
    {
        SHR_IF_ERR_EXIT(dnx_algo_oamp_mep_id_dealloc(unit, entry[entry_id]));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Allocate the MEP DB index for LM
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] loss_ptr - Pointer to loss information structure.
 * \param [in] ccm_mep_db_entry - Pointer to the CCM MEP DB structure
 * \param [in] dm_exists - Whether DM already exists or not.
 * \param [in] dm_entry_idx - If exists, what is the entry ID.
 *   
 * \return
 *   shr_error_e 
 *   
 * \see
 *   * None
 */

static shr_error_e
dnx_oam_loss_mep_id_alloc(
    int unit,
    const bcm_oam_loss_t * loss_ptr,
    const dnx_oam_oamp_ccm_endpoint_t * ccm_mep_db_entry,
    uint8 dm_exists,
    uint32 dm_entry_idx)
{
    uint32 entry[2];            /* Max 2 entries only. */
    int num_entries = 0, entry_id;
    uint8 extended_statistics = 0;
    dnx_oamp_mep_db_memory_type_t mep_id_memory_type;
    uint8 offloaded = 0;
    SHR_FUNC_INIT_VARS(unit);

    extended_statistics = (loss_ptr->flags & BCM_OAM_LOSS_STATISTICS_EXTENDED) ? 1 : 0;
    offloaded = (ccm_mep_db_entry->flags & DNX_OAMP_OAM_CCM_MEP_LM_DM_OFFLOADED) ? 1 : 0;

    if (offloaded)
    {
        if (!dm_exists)
        {
            /*
             * part2 ptr given by user 
             */
            entry[0] = loss_ptr->loss_id;
            num_entries++;
        }       /* Else If other entry exists, we have nothing to do */
    }
    else        /* Self contained */
    {
        if (dm_exists)
        {
            /*
             * calculate the ID(s) from the other entry. 
             */
            entry[0] = DNX_OAM_GET_NEXT_LM_DM_CHAIN_INDEX(dm_entry_idx);
            num_entries++;
        }
        else
        {
            /*
             * given by user 
             */
            entry[0] = loss_ptr->loss_id;
            num_entries++;
        }
        if (extended_statistics)
        {
            /*
             * Extended entry is added next in chain to the first entry 
             */
            entry[1] = DNX_OAM_GET_NEXT_LM_DM_CHAIN_INDEX(entry[0]);
            num_entries++;
        }
    }

    for (entry_id = 0; entry_id < num_entries; entry_id++)
    {
        /*
         * Since we pass the ID, mep_id_memory_type info need not be filled. 
         */
        SHR_IF_ERR_EXIT(dnx_algo_oamp_mep_id_alloc(unit,
                                                   SW_STATE_ALGO_RES_ALLOCATE_WITH_ID, &mep_id_memory_type,
                                                   &(entry[entry_id])));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Convert LOSS structure information to OAMP LM MEP DB
 *          HW contents.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] loss_ptr - Pointer to loss information structure.
 * \param [in] ccm_mep_db_entry - Pointer to the CCM MEP DB structure
 * \param [out] hw_write_data - Pointer to the structure,
 *                 containing information about the 
 *                 OAMP LM MEP DB content
 *   
 * \return
 *   shr_error_e 
 *   
 * \see
 *   * None
 */
static void
dnx_oam_loss_info_to_lm_mep_db_hw_write_info(
    int unit,
    const bcm_oam_loss_t * loss_ptr,
    const dnx_oam_oamp_ccm_endpoint_t * ccm_mep_db_entry,
    dnx_oam_loss_delay_oamp_hw_write_data_t * hw_write_data)
{
    /*
     * Offloaded endpoint is true if CCM MEP db entry get says offloaded 
     */
    if (ccm_mep_db_entry->flags & DNX_OAMP_OAM_CCM_MEP_LM_DM_OFFLOADED)
    {
        hw_write_data->offloaded = 1;
    }

    /*
     * CCM OAM id = loss -> id 
     */
    hw_write_data->ccm_oam_id = loss_ptr->id;
    /*
     * stat enable true if extended statistics is set 
     */
    hw_write_data->stat_enable = (loss_ptr->flags & BCM_OAM_LOSS_STATISTICS_EXTENDED) ? 1 : 0;
    hw_write_data->update_mep_db = (loss_ptr->flags & BCM_OAM_LOSS_UPDATE) ? 1 : 0;

    /*
     * part_2_ptr is filled if offloaded is true. lm_db entry is filled if selfcontained is true 
     */
    hw_write_data->flex_lm_dm_entry = OAM_ID_TO_MEP_DB_ENTRY(loss_ptr->loss_id);

}

/**
 * \brief - Write OAMP LM MEP DB info
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] hw_write_data - Pointer to the structure,
 *                 containing information about the 
 *                 OAMP LM MEP DB content
 *   
 * \return
 *   shr_error_e 
 *   
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_act_on_oamp_lm_mep_db_hw_write_info(
    int unit,
    const dnx_oam_loss_delay_oamp_hw_write_data_t * hw_write_data)
{
    dnx_oam_oamp_endpoint_lm_params_t lm_mep_db_write_data;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&lm_mep_db_write_data, 0, sizeof(lm_mep_db_write_data));
    lm_mep_db_write_data.flags |= hw_write_data->stat_enable ? DNX_OAMP_OAM_LM_MEP_ADD_STAT_ENTRY : 0;
    lm_mep_db_write_data.flags |= hw_write_data->update_mep_db ? DNX_OAMP_OAM_LM_MEP_UPDATE : 0;
    if (hw_write_data->offloaded)
    {
        /*
         * For offloaded, setting stat entry is must, since part 2 entry is required. 
         */
        lm_mep_db_write_data.flags |= DNX_OAMP_OAM_LM_MEP_ADD_STAT_ENTRY;
    }
    lm_mep_db_write_data.lmm_dmm_da_profile = hw_write_data->oamp_da_lsb_prof;
    lm_mep_db_write_data.mep_profile = hw_write_data->oamp_mep_profile;
    lm_mep_db_write_data.flex_lm_entry = MEP_DB_ENTRY_TO_OAM_ID(hw_write_data->flex_lm_dm_entry);

    SHR_IF_ERR_EXIT(dnx_oamp_mep_db_lm_endpoint_set(unit, hw_write_data->ccm_oam_id, &lm_mep_db_write_data));

exit:
    SHR_FUNC_EXIT;

}


/**
 * \brief - Write OAMP LM SESSION DB info 
 *          This table maps oam_id, priority to updated loss_id
 *          thus enables lm with priority.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] mep_id - Endpoint ID
 * \param [in] new_mep_id - id of the new entry
 * \param [in] priority - priority related to the session
 *   
 * \return
 *   shr_error_e 
 *   
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_oamp_lm_session_set(
    int unit,
    int mep_id,
    int new_mep_id,
    int priority
    )
{
   uint32 entry_handle_id;

   SHR_FUNC_INIT_VARS(unit);
   DBAL_FUNC_INIT_VARS(unit);

   SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_LM_SESSION_DB, &entry_handle_id));

   dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_ID, mep_id);
   dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY, priority);
   dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MEP_ID, 0);
   dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TEST_ID, 0);

   dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_ID, INST_SINGLE, new_mep_id);
   SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Add an OAM Loss entry.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] loss_ptr - Pointer to the structure,
 *                 containing information about the loss entry.
 *   
 * \return
 *   shr_error_e 
 *   
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_oam_loss_add(
    int unit,
    bcm_oam_loss_t * loss_ptr)
{
    bcm_oam_endpoint_info_t endpoint_info;
    bcm_oam_group_info_t group_info;
    bcm_oam_endpoint_t id = 0;
    bcm_oam_endpoint_t mep_id = 0;
    dnx_oam_loss_delay_oamp_hw_write_data_t *hw_write_info = NULL;
    dnx_oam_oamp_ccm_endpoint_t *ccm_mep_db_entry = NULL;
    dnx_oam_oamp_endpoint_dm_params_t *dm_mep_db_entry = NULL;
    dnx_oam_oamp_endpoint_lm_params_t *lm_mep_db_entry = NULL;
    dnx_oam_mep_profile_t *curr_mep_profile_data = NULL;
    uint8 update = 0, exchange_mac_prof = 0;
    uint8 piggy_back_lm = 0;
    uint8 mpls_pwe_mep = 0;
    uint8 offloaded = 0;
    uint32 dm_entry_idx = 0;
    int counter = 0;
    int priority = 0;

    bcm_mac_t oamp_mac_da_msb = { 0 }, old_oamp_mac_da_msb =
    {
    0};
    bcm_mac_t oamp_mac_da_lsb = { 0 }, old_oamp_mac_da_lsb =
    {
    0};
    uint8 old_oamp_da_lsb_prof = 0;
    uint8 dm_exists = 0, lm_exists = 0, lm_stat_exists = 0;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    update = (loss_ptr->flags & BCM_OAM_LOSS_UPDATE) ? 1 : 0;
    piggy_back_lm = ((loss_ptr->flags & BCM_OAM_LOSS_SINGLE_ENDED) == 0);

    SHR_ALLOC_SET_ZERO(hw_write_info, sizeof(dnx_oam_loss_delay_oamp_hw_write_data_t),
                       "Structure for HW WRITE Info", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(ccm_mep_db_entry, sizeof(dnx_oam_oamp_ccm_endpoint_t),
                       "Structure for CCM MEP DB entry", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(dm_mep_db_entry, sizeof(dnx_oam_oamp_endpoint_dm_params_t),
                       "Structure for DM MEP DB entry", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(lm_mep_db_entry, sizeof(dnx_oam_oamp_endpoint_lm_params_t),
                       "Structure for LM MEP DB entry", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(curr_mep_profile_data, sizeof(dnx_oam_mep_profile_t),
                       "Structure for CURR MEP PROFILE", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    /*
     * Get the current CCM mep db entry 
     */
    SHR_IF_ERR_EXIT(dnx_oamp_mep_db_ccm_endpoint_get(unit, loss_ptr->id, 0, ccm_mep_db_entry));
    mpls_pwe_mep = DNX_OAM_MEP_TYPE_IS_MPLS_TP_OR_PWE(ccm_mep_db_entry->mep_type);
    offloaded = (ccm_mep_db_entry->flags & DNX_OAMP_OAM_CCM_MEP_LM_DM_OFFLOADED) ? 1 : 0;

    /*
     * -- SW configurations -- 
     */

    /*
     * Check if Delay add is already configured on this endpoint 
     */
    SHR_IF_ERR_EXIT(dnx_oamp_mep_db_dm_endpoint_get(unit, loss_ptr->id, dm_mep_db_entry, &dm_exists));
    if (dm_exists)
    {
        DNX_OAM_GET_DELAY_ENTRY_CONFIGURED(offloaded, dm_mep_db_entry, dm_entry_idx);
    }

    /*
     * Call verify function 
     */
    SHR_IF_ERR_EXIT(dnx_oam_loss_add_verify(unit, loss_ptr, ccm_mep_db_entry, dm_exists, &priority));

    /*
     * Get the current OAMP MEP profile 
     */
    SHR_IF_ERR_EXIT(algo_oam_db.oam_mep_profile.profile_data_get(unit, _SHR_CORE_ALL,
                                                                 ccm_mep_db_entry->mep_profile, &counter,
                                                                 curr_mep_profile_data));

    /*
     * If this is an update case or in case Delay was already configured, there will be already a MAC configured.
     * Exchange is true for those 2 cases. 
     */
    exchange_mac_prof = (update || dm_exists);
    if (update)
    {
        SHR_IF_ERR_EXIT(dnx_oamp_mep_db_lm_endpoint_get
                        (unit, loss_ptr->id, lm_mep_db_entry, &lm_exists, &lm_stat_exists));
        if (!lm_exists)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error: Loss is not enabled on this endpoint.\n");
        }
        old_oamp_da_lsb_prof = lm_mep_db_entry->lmm_dmm_da_profile;
    }
    else if (dm_exists)
    {
        old_oamp_da_lsb_prof = dm_mep_db_entry->lmm_dmm_da_profile;
    }

    /*
     * For exchange, get the existing mac information 
     */
    if (exchange_mac_prof && !piggy_back_lm && !mpls_pwe_mep)
    {
        /*
         * Get the current LMM MAC DA MSB 
         */
        SHR_IF_ERR_EXIT(algo_oam_db.oam_da_mac_msb_profile_template.profile_data_get(unit,
                                                                                     _SHR_CORE_ALL,
                                                                                     curr_mep_profile_data->oamp_mac_da_oui_prof,
                                                                                     &counter, &old_oamp_mac_da_msb));

        /*
         * Get the current LMM MAC DA LSB 
         */
        SHR_IF_ERR_EXIT(algo_oam_db.oam_da_mac_lsb_profile_template.profile_data_get(unit,
                                                                                     _SHR_CORE_ALL,
                                                                                     old_oamp_da_lsb_prof, &counter,
                                                                                     &old_oamp_mac_da_lsb));
    }

    if (!piggy_back_lm && !mpls_pwe_mep)
    {
        /*
         * Extract the OUI (MSB) part from the mac address 
         */
        GET_MSB_FROM_MAC_ADDR(loss_ptr->peer_da_mac_address, oamp_mac_da_msb);
        /*
         * Alloc/exchange the MSB profile 
         */
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mac_da_msb_profile_alloc(unit, oamp_mac_da_msb,
                                                              old_oamp_mac_da_msb,
                                                              curr_mep_profile_data->oamp_mac_da_oui_prof,
                                                              exchange_mac_prof, hw_write_info));

        /*
         * Extract the NIC (LSB) part from the mac address 
         */
        GET_LSB_FROM_MAC_ADDR(loss_ptr->peer_da_mac_address, oamp_mac_da_lsb);
        /*
         * Alloc/exchange the LSB profile 
         */
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mac_da_lsb_profile_alloc(unit, oamp_mac_da_lsb,
                                                              old_oamp_mac_da_lsb, old_oamp_da_lsb_prof,
                                                              exchange_mac_prof, hw_write_info));
    }

    /*
     * Fill the current MEP profile with Loss params 
     */
    SHR_IF_ERR_EXIT(dnx_oam_oamp_loss_info_to_mep_profile_data
                    (unit, loss_ptr, ccm_mep_db_entry, curr_mep_profile_data));
    /*
     * update the current MEP profile with OUI (MSB) Profile 
     */
    curr_mep_profile_data->oamp_mac_da_oui_prof = hw_write_info->oamp_da_msb_prof;
    /*
     * Exchange the OAMP MEP profile with existing MEP profile 
     */
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_profile_exchange(unit, ccm_mep_db_entry->mep_profile,
                                                      curr_mep_profile_data, hw_write_info));


    /*
     * Write the session information for lm with priority
     */
    if ((loss_ptr->pkt_pri_bitmap != 0xff) && (loss_ptr->pkt_pri_bitmap != 0))
    {
       SHR_IF_ERR_EXIT(bcm_dnx_oam_endpoint_get(unit, loss_ptr->id, &endpoint_info));
       SHR_IF_ERR_EXIT(bcm_dnx_oam_group_get(unit,endpoint_info.group,&group_info));

       /* In case of Downmep except egress_injection case, we need to update the counter pointer 
        * counter_pointer is set with counter base in endpoint create. 
        * Here we add the priority to get the counter pointer.
        */
       if ((!DNX_OAM_DISSECT_IS_ENDPOINT_UPMEP(&endpoint_info))&&(!_SHR_IS_FLAG_SET(endpoint_info.flags2, BCM_OAM_ENDPOINT_FLAGS2_EGRESS_INJECTION_DOWN)))
       {
          ccm_mep_db_entry->counter_ptr += priority;
          ccm_mep_db_entry->flags = DNX_OAMP_OAM_CCM_MEP_UPDATE;
          SHR_IF_ERR_EXIT(dnx_oamp_mep_db_ccm_endpoint_set(unit,loss_ptr->id,ccm_mep_db_entry));
       }

       /*
       * Duplicate ccm entry
       */
       endpoint_info.flags &= (0xffffffff ^ BCM_OAM_ENDPOINT_WITH_ID);
       SHR_IF_ERR_EXIT(dnx_oam_local_accelerated_endpoint_fill_id(unit, &endpoint_info, group_info.name));
       id = endpoint_info.id;
   
      if ((!DNX_OAM_DISSECT_IS_ENDPOINT_UPMEP(&endpoint_info))&&(!_SHR_IS_FLAG_SET(endpoint_info.flags2, BCM_OAM_ENDPOINT_FLAGS2_EGRESS_INJECTION_DOWN)))
      {
         ccm_mep_db_entry->counter_ptr += priority;
      }
      ccm_mep_db_entry->ccm_interval = DBAL_ENUM_FVAL_CCM_INTERVAL_NO_TX;
      mep_id = loss_ptr->id;
      loss_ptr->id = id;
    }

    /*
     * If not update, go ahead and alloc mep_db index(es).
     */
    if (!update)
    {
       SHR_IF_ERR_EXIT(dnx_oam_loss_mep_id_alloc(unit, loss_ptr, ccm_mep_db_entry, dm_exists, dm_entry_idx));
    }

    /*
     * Update the HW write info from SW info for LM MEP DB 
     */
    dnx_oam_loss_info_to_lm_mep_db_hw_write_info(unit, loss_ptr, ccm_mep_db_entry, hw_write_info);

    /*
     * -- HW configurations -- 
     */

    if ((loss_ptr->pkt_pri_bitmap != 0xff) && (loss_ptr->pkt_pri_bitmap != 0))
    {
       SHR_IF_ERR_EXIT(dnx_oamp_mep_db_ccm_endpoint_set(unit,id,ccm_mep_db_entry));

       /*
        * Set oam_id session_map
        */
       SHR_IF_ERR_EXIT(dnx_oam_oamp_lm_session_set(unit,
                                                   mep_id,
                                                   loss_ptr->id,
                                                   priority));
    }

    /*
     * Write the DA MAC msb into register. 
     */
    if (hw_write_info->flags & DNX_OAM_LM_DM_MAC_DA_MSB_PROFILE)
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mac_da_msb_profile_set(unit,
                                                            hw_write_info->oamp_da_msb_prof,
                                                            hw_write_info->oamp_mac_da_msb));
    }

    /*
     * Write the DA MAC lsb into register. 
     */
    if (hw_write_info->flags & DNX_OAM_LM_DM_MAC_DA_LSB_PROFILE)
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mac_da_lsb_profile_set(unit,
                                                            hw_write_info->oamp_da_lsb_prof,
                                                            hw_write_info->oamp_mac_da_lsb));
    }

    /*
     * Write the OAMP_MEP_PROFILE (along with DA MAC MSB profile) 
     */
    if (hw_write_info->flags & DNX_OAM_LM_DM_OAMP_MEP_PROFILE)
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_profile_set(unit,
                                                     hw_write_info->oamp_mep_profile,
                                                     &(hw_write_info->mep_profile_data)));
    }

    /*
     * Write the MEP DB entry(ies) (along with DA MAC LSB profile) 
     */
    SHR_IF_ERR_EXIT(dnx_oam_act_on_oamp_lm_mep_db_hw_write_info(unit, hw_write_info));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FREE(hw_write_info);
    SHR_FREE(ccm_mep_db_entry);
    SHR_FREE(lm_mep_db_entry);
    SHR_FREE(dm_mep_db_entry);
    SHR_FREE(curr_mep_profile_data);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get an OAM Loss entry.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] loss_ptr - Pointer to the structure,
 *                 containing information about the loss entry.
 *   
 * \return
 *   shr_error_e 
 *   
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_oam_loss_get(
    int unit,
    bcm_oam_loss_t * loss_ptr)
{
   bcm_oam_endpoint_info_t endpoint_info;
   bcm_oam_group_info_t group_info;
   uint32 session_oam_id;
   int priority;
   uint8 extra_data_entries_on_mdb;
    dnx_oam_oamp_endpoint_lm_params_t *loss_params = NULL;
    dnx_oam_oamp_ccm_endpoint_t *ccm_mep_db_entry = NULL;
    dnx_oam_mep_profile_t *curr_mep_profile_data = NULL;
    bcm_mac_t oamp_mac_da_msb = { 0 };
    bcm_mac_t oamp_mac_da_lsb = { 0 };
    uint8 offloaded = 0, extended_statistics = 0, lm_exists = 0, lm_stat_exists = 0;
    uint8 mpls_pwe_mep = 0;
    int counter = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(loss_params, sizeof(dnx_oam_oamp_endpoint_lm_params_t),
                       "Structure for LM MEP DB entry", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(ccm_mep_db_entry, sizeof(dnx_oam_oamp_ccm_endpoint_t),
                       "Structure for CCM MEP DB entry", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(curr_mep_profile_data, sizeof(dnx_oam_mep_profile_t),
                       "Structure for CURR MEP PROFILE", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);


    /*
     * Get lm with priority info
     */
    if ((loss_ptr->pkt_pri_bitmap != 0xff) && (loss_ptr->pkt_pri_bitmap != 0))
    {
       SHR_IF_ERR_EXIT(bcm_dnx_oam_endpoint_get(unit, loss_ptr->id, &endpoint_info));

       SHR_IF_ERR_EXIT(bcm_oam_group_get(unit, endpoint_info.group, &group_info));

       extra_data_entries_on_mdb = _SHR_IS_FLAG_SET(group_info.flags, BCM_OAM_GROUP_FLEXIBLE_MAID_48_BYTE) &&
           (endpoint_info.endpoint_memory_type != bcmOamEndpointMemoryTypeSelfContained);


       switch (loss_ptr->pkt_pri_bitmap) 
       {
          case 255:
          case 1: priority=0; break;
          case 2: priority=1; break;
          case 4: priority=2; break;
          case 8: priority=3; break;
          case 16: priority=4; break;
          case 32: priority=5; break;
          case 64: priority=6; break;
          case 128: priority=7; break;
          default:  SHR_IF_ERR_EXIT(_SHR_E_PARAM);
       }
       SHR_IF_ERR_EXIT(dnx_oamp_lm_with_priority_session_map_get( unit, loss_ptr->id, priority, &session_oam_id));

       loss_ptr->id = session_oam_id;
    }

    /*
     * First, read the CCM entry 
     */
    SHR_IF_ERR_EXIT(dnx_oamp_mep_db_ccm_endpoint_get(unit, loss_ptr->id, 0, ccm_mep_db_entry));
    /*
     * Get the LM MEP DB entry 
     */
    SHR_IF_ERR_EXIT(dnx_oamp_mep_db_lm_endpoint_get(unit, loss_ptr->id, loss_params, &lm_exists, &lm_stat_exists));
    if (!lm_exists)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: Loss is not enabled on this endpoint.\n");
    }

    offloaded = (ccm_mep_db_entry->flags & DNX_OAMP_OAM_CCM_MEP_LM_DM_OFFLOADED) ? 1 : 0;
    mpls_pwe_mep = DNX_OAM_MEP_TYPE_IS_MPLS_TP_OR_PWE(ccm_mep_db_entry->mep_type);

    /*
     * Get the OAMP MEP profile 
     */
    SHR_IF_ERR_EXIT(algo_oam_db.oam_mep_profile.profile_data_get(unit, _SHR_CORE_ALL,
                                                                 ccm_mep_db_entry->mep_profile,
                                                                 &counter, curr_mep_profile_data));

    if (!curr_mep_profile_data->piggy_back_lm && !mpls_pwe_mep)
    {
        /*
         * Get the LMM DA OUI 
         */
        SHR_IF_ERR_EXIT(algo_oam_db.oam_da_mac_msb_profile_template.profile_data_get(unit,
                                                                                     _SHR_CORE_ALL,
                                                                                     curr_mep_profile_data->oamp_mac_da_oui_prof,
                                                                                     &counter, oamp_mac_da_msb));

        /*
         * Get the LMM DA NIC 
         */
        SHR_IF_ERR_EXIT(algo_oam_db.oam_da_mac_lsb_profile_template.profile_data_get(unit,
                                                                                     _SHR_CORE_ALL,
                                                                                     loss_params->lmm_dmm_da_profile,
                                                                                     &counter, oamp_mac_da_lsb));

        /*
         * Combined MSB and LSB to form FULL MAC DA 
         */
        GET_MAC_FROM_MSB_LSB(oamp_mac_da_msb, oamp_mac_da_lsb, loss_ptr->peer_da_mac_address);
    }

    /*
     * Fill the Loss pointer 
     */
    if (offloaded)
    {
        /*
         * In case of offloaded, statistics is always enabled 
         */
        extended_statistics = 1;
        loss_ptr->loss_id = loss_params->flex_lm_entry;
    }
    else
    {
        extended_statistics = (loss_params->lm_stat_entry_idx != 0);
        loss_ptr->loss_id = loss_params->lm_db_entry_idx;
    }
    loss_ptr->period = curr_mep_profile_data->lmm_rate;
    if (extended_statistics)
    {
        loss_ptr->flags |= BCM_OAM_LOSS_STATISTICS_EXTENDED;
    }
    if (!curr_mep_profile_data->piggy_back_lm)
    {
        loss_ptr->flags |= BCM_OAM_LOSS_SINGLE_ENDED;
    }
    if (curr_mep_profile_data->slm_lm)
    {
        loss_ptr->flags |= BCM_OAM_LOSS_SLM;
    }

    if (!curr_mep_profile_data->report_mode_lm)
    {
        loss_ptr->tx_nearend = loss_params->lm_my_tx;
        loss_ptr->rx_nearend = loss_params->lm_my_rx;
        loss_ptr->tx_farend = loss_params->lm_peer_tx;
        loss_ptr->rx_farend = loss_params->lm_peer_rx;
        if (extended_statistics)
        {
            loss_ptr->loss_farend =
                (loss_params->lm_my_tx == 0) ? 0 : (100 * loss_params->acc_lm_far) / loss_params->lm_my_tx;
            loss_ptr->loss_nearend =
                (loss_params->lm_peer_tx == 0) ? 0 : (100 * loss_params->acc_lm_near) / loss_params->lm_peer_tx;
            loss_ptr->loss_nearend_max = loss_params->max_lm_near;
            loss_ptr->loss_nearend_acc = loss_params->acc_lm_near;
            loss_ptr->loss_farend_max = loss_params->max_lm_far;
            loss_ptr->loss_farend_acc = loss_params->acc_lm_far;
        }
    }
    else
    {
        loss_ptr->flags |= BCM_OAM_LOSS_REPORT_MODE;
    }

exit:
    SHR_FREE(loss_params);
    SHR_FREE(curr_mep_profile_data);
    SHR_FREE(ccm_mep_db_entry);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Delete an OAM Loss entry.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] loss_ptr - Pointer to the structure,
 *                 containing information about the loss entry.
 *   
 * \return
 *   shr_error_e 
 *   
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_oam_loss_delete(
    int unit,
    bcm_oam_loss_t * loss_ptr)
{
    bcm_oam_endpoint_info_t endpoint_info;
    bcm_oam_group_info_t group_info;
    dnx_oam_oamp_ccm_endpoint_t *ccm_mep_db_entry = NULL;
    dnx_oam_oamp_endpoint_lm_params_t *loss_params = NULL;
    dnx_oam_oamp_endpoint_dm_params_t *dm_mep_db_entry = NULL;
    dnx_oam_mep_profile_t *curr_mep_profile_data = NULL;
    dnx_oam_loss_delay_oamp_hw_write_data_t *hw_write_info = NULL;
    uint8 oamp_da_lsb_prof = 0;
    int counter = 0;
    uint8 dm_exists = 0, delete_old_prof = 0, mpls_pwe_mep = 0;
    uint32 dm_entry_idx = 0;
    uint8 offloaded = 0, lm_exists = 0, lm_stat_exists = 0;
    uint8 piggy_back_lm = 0;
    uint32 session_oam_id;
    int priority;
    uint8 extra_data_entries_on_mdb = 0;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_ALLOC_SET_ZERO(hw_write_info, sizeof(dnx_oam_loss_delay_oamp_hw_write_data_t),
                       "Structure for HW WRITE Info", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(ccm_mep_db_entry, sizeof(dnx_oam_oamp_ccm_endpoint_t),
                       "Structure for CCM MEP DB entry", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(dm_mep_db_entry, sizeof(dnx_oam_oamp_endpoint_dm_params_t),
                       "Structure for DM MEP DB entry", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(loss_params, sizeof(dnx_oam_oamp_endpoint_lm_params_t),
                       "Structure for LM MEP DB entry", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(curr_mep_profile_data, sizeof(dnx_oam_mep_profile_t),
                       "Structure for CURR MEP PROFILE", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    /*
     * In case it's lm with priorty, delete the session map entry
     */
    if ((loss_ptr->pkt_pri_bitmap != 0xff) && (loss_ptr->pkt_pri_bitmap != 0))
    {
       SHR_IF_ERR_EXIT(bcm_dnx_oam_endpoint_get(unit, loss_ptr->id, &endpoint_info));

       SHR_IF_ERR_EXIT(bcm_oam_group_get(unit, endpoint_info.group, &group_info));

       extra_data_entries_on_mdb = _SHR_IS_FLAG_SET(group_info.flags, BCM_OAM_GROUP_FLEXIBLE_MAID_48_BYTE) &&
           (endpoint_info.endpoint_memory_type != bcmOamEndpointMemoryTypeSelfContained);


       switch (loss_ptr->pkt_pri_bitmap) 
       {
          case 255:
          case 1: priority=0; break;
          case 2: priority=1; break;
          case 4: priority=2; break;
          case 8: priority=3; break;
          case 16: priority=4; break;
          case 32: priority=5; break;
          case 64: priority=6; break;
          case 128: priority=7; break;
          default:  SHR_IF_ERR_EXIT(_SHR_E_PARAM);
       }
       SHR_IF_ERR_EXIT(dnx_oamp_lm_with_priority_session_map_get( unit, loss_ptr->id, priority, &session_oam_id));

       SHR_IF_ERR_EXIT(dnx_oamp_lm_with_priority_session_map_clear(unit, loss_ptr->id, priority));

       loss_ptr->id = session_oam_id;
    }

    /*
     * Get the current CCM mep db entry 
     */
    SHR_IF_ERR_EXIT(dnx_oamp_mep_db_ccm_endpoint_get(unit, loss_ptr->id, 0, ccm_mep_db_entry));
    mpls_pwe_mep = DNX_OAM_MEP_TYPE_IS_MPLS_TP_OR_PWE(ccm_mep_db_entry->mep_type);
    offloaded = (ccm_mep_db_entry->flags & DNX_OAMP_OAM_CCM_MEP_LM_DM_OFFLOADED) ? 1 : 0;

    /*
     * Get the LM MEP DB entry 
     */
    SHR_IF_ERR_EXIT(dnx_oamp_mep_db_lm_endpoint_get(unit, loss_ptr->id, loss_params, &lm_exists, &lm_stat_exists));

    if (offloaded)
    {
        loss_ptr->loss_id = loss_params->flex_lm_entry;
    }
    else
    {
        loss_ptr->loss_id = loss_params->lm_db_entry_idx;
    }
    oamp_da_lsb_prof = loss_params->lmm_dmm_da_profile;

    /*
     * Get the current OAMP MEP profile 
     */
    SHR_IF_ERR_EXIT(algo_oam_db.oam_mep_profile.profile_data_get(unit, _SHR_CORE_ALL,
                                                                 ccm_mep_db_entry->mep_profile, &counter,
                                                                 curr_mep_profile_data));
    piggy_back_lm = curr_mep_profile_data->piggy_back_lm;

    /*
     * -- SW configurations -- 
     */

    /*
     * Check if Delay add is already configured on this endpoint 
     */
    SHR_IF_ERR_EXIT(dnx_oamp_mep_db_dm_endpoint_get(unit, loss_ptr->id, dm_mep_db_entry, &dm_exists));
    if (dm_exists)
    {
        DNX_OAM_GET_DELAY_ENTRY_CONFIGURED(offloaded, dm_mep_db_entry, dm_entry_idx);
    }

    if (!piggy_back_lm && !mpls_pwe_mep && !dm_exists)
    {
        /*
         * Free the DA MAC LSB profile or (ref_count - 1) 
         */
        SHR_IF_ERR_EXIT(algo_oam_db.oam_da_mac_msb_profile_template.free_single(unit,
                                                                                _SHR_CORE_ALL,
                                                                                curr_mep_profile_data->oamp_mac_da_oui_prof,
                                                                                &delete_old_prof));
        /*
         * Free the DA MAC MSB profile or (ref_count - 1) 
         */
        SHR_IF_ERR_EXIT(algo_oam_db.oam_da_mac_lsb_profile_template.free_single(unit,
                                                                                _SHR_CORE_ALL, oamp_da_lsb_prof,
                                                                                &delete_old_prof));
    }

    /*
     * Free the OAMP MEP profile from loss params. 
     */
    SHR_IF_ERR_EXIT(dnx_oam_oamp_remove_loss_info_from_mep_profile_data(unit, ccm_mep_db_entry, curr_mep_profile_data));
    /*
     * Exchange the OLD profile and get NEW profile. 
     */
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_profile_exchange(unit, ccm_mep_db_entry->mep_profile,
                                                      curr_mep_profile_data, hw_write_info));

    /*
     * Free up the OAMP MEP ID(s) 
     */
    SHR_IF_ERR_EXIT(dnx_oam_loss_mep_id_free(unit, loss_ptr->loss_id, ccm_mep_db_entry,
                                             dm_exists, dm_entry_idx,
                                             lm_stat_exists, loss_params->lm_stat_entry_idx));

    /*
     * ---- HW configurations --- 
     */

    /*
     * Write the new OAMP MEP profile if required 
     */
    if (hw_write_info->flags & DNX_OAM_LM_DM_OAMP_MEP_PROFILE)
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_profile_set(unit,
                                                     hw_write_info->oamp_mep_profile,
                                                     &(hw_write_info->mep_profile_data)));
    }

    /*
     * Clear the Loss MEP DB entry in HW 
     */
    SHR_IF_ERR_EXIT(dnx_oamp_mep_db_lm_endpoint_clear(unit, loss_ptr->id));

    /*
     * In case it's lm with priority, clear the ccm entry
     */
    SHR_IF_ERR_EXIT(dnx_oamp_mep_db_ccm_endpoint_get(unit, loss_ptr->id, 0, ccm_mep_db_entry));
    if ((loss_ptr->pkt_pri_bitmap != 0xff) && (loss_ptr->pkt_pri_bitmap != 0))
    {
       SHR_IF_ERR_EXIT(dnx_oamp_mep_db_ccm_endpoint_clear(unit, loss_ptr->id, extra_data_entries_on_mdb));
    }
    else
    {
       /*
        * Update the CCM mep db entry with new OAMP MEP profile 
        */
       ccm_mep_db_entry->mep_profile = hw_write_info->oamp_mep_profile;
       ccm_mep_db_entry->flags |= DNX_OAMP_OAM_CCM_MEP_UPDATE;
       SHR_IF_ERR_EXIT(dnx_oamp_mep_db_ccm_endpoint_set(unit, loss_ptr->id, ccm_mep_db_entry));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FREE(hw_write_info);
    SHR_FREE(ccm_mep_db_entry);
    SHR_FREE(loss_params);
    SHR_FREE(dm_mep_db_entry);
    SHR_FREE(curr_mep_profile_data);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify the delay information structure 
 *          for correct parameters.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] delay_ptr - Pointer to the structure,
 *                 containing information about the delay entry.
 * \param [in] ccm_mep_db_entry - Pointer to the CCM MEP DB structure
 * \param [in] lm_exists - Indicates whether LM entry exists.
 * \param [out] priority - Priority value in case of dm with priority is being used
 *   
 * \return
 *   shr_error_e 
 *   
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_delay_add_verify(
    int unit,
    const bcm_oam_delay_t * delay_ptr,
    const dnx_oam_oamp_ccm_endpoint_t * ccm_mep_db_entry,
    uint8 lm_exists,
    uint8 *priority)
{
    uint32 correct_flags = 0;
    uint8 mpls_pwe_mep = 0;
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_oam.oamp.lmm_dmm_supported_get(unit) == 0) 
    {
       SHR_ERR_EXIT(_SHR_E_PARAM,"Delay add is not supported on this device.");
    }
    /*
     * Delay add is not supported on quarter entry endpoints 
     */
    if (ccm_mep_db_entry->flags & DNX_OAMP_OAM_CCM_MEP_Q_ENTRY)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: Invalid memory type.\n");

    }

    /*
     * Flags verification 
     */
    correct_flags |= BCM_OAM_DELAY_ONE_WAY | BCM_OAM_DELAY_UPDATE | BCM_OAM_DELAY_WITH_ID;

    if (delay_ptr->flags & ~correct_flags)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: Illegal flags set\n");
    }

    if ((ccm_mep_db_entry->flags & DNX_OAMP_OAM_CCM_MEP_LM_DM_OFFLOADED) && (delay_ptr->flags & BCM_OAM_DELAY_ONE_WAY))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: 1DM is not supported on OFFLOADED endpoints\n");
    }

    mpls_pwe_mep = DNX_OAM_MEP_TYPE_IS_MPLS_TP_OR_PWE(ccm_mep_db_entry->mep_type);

    /*
     * MPLS/PWE endpoints - DA needs to be zero
     */
    if (mpls_pwe_mep)
    {
        if (!BCM_MAC_IS_ZERO(delay_ptr->peer_da_mac_address))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error: DA MAC needs to be zero.\n");
        }
    }

    if (delay_ptr->timestamp_format != bcmOAMTimestampFormatIEEE1588v1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Only 1588 time stamp format supported for y1731.");
    }

    if (delay_ptr->int_pri || delay_ptr->pkt_pri || delay_ptr->rx_oam_packets || delay_ptr->tx_oam_packets)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported fields(INT pri, PKT pri, oam packets) are being set");
    }

    if (delay_ptr->period < 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Period below 0 not supported.");
    }

    /*
     * priority verification for lm with priority feature
     */
    switch (delay_ptr->pkt_pri_bitmap) 
    {
       case 0:
       case 255:
       case 1: *priority=0; break;
       case 2: *priority=1; break;
       case 4: *priority=2; break;
       case 8: *priority=3; break;
       case 16: *priority=4; break;
       case 32: *priority=5; break;
       case 64: *priority=6; break;
       case 128: *priority=7; break;
       default:  SHR_ERR_EXIT(_SHR_E_PARAM,"Delay with priority may be defined for one priority at a time. Please consider multiple bcm_oam_delay_add");
    }

    /*
     * Call mep db index verify function 
     */
    SHR_IF_ERR_EXIT(dnx_oam_lm_dm_add_mep_db_entry_verify(unit, delay_ptr,
                                                          ccm_mep_db_entry, 1 /* DM */ , lm_exists));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Fill OAMP MEP profile data based on Delay information.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] delay_ptr - Pointer to the structure,
 *                 containing information about the delay entry.
 * \param [in] ccm_mep_db_entry - Pointer to CCM MEP DB entry contents.
 * \param [out] mep_profile_data - Pointer to the structure,
 *                 containing information about the 
 *                 OAMP MEP PROFILE content.
 *   
 * \return
 *   shr_error_e 
 *   
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_oamp_delay_info_to_mep_profile_data(
    int unit,
    const bcm_oam_delay_t * delay_ptr,
    const dnx_oam_oamp_ccm_endpoint_t * ccm_mep_db_entry,
    dnx_oam_mep_profile_t * mep_profile_data)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Fill up with DM params 
     */
    mep_profile_data->report_mode_dm = (delay_ptr->flags & BCM_OAM_DELAY_REPORT_MODE);
    mep_profile_data->dmm_rate = delay_ptr->period;
    mep_profile_data->dm_measurement_type = (delay_ptr->flags & BCM_OAM_DELAY_ONE_WAY);

    /*
     * Setting phase count for DMM
     */
    SHR_IF_ERR_EXIT(dnx_oam_oamp_scan_count_calc(unit, 0 /* dont care for opcodes other than CCM */ ,
                                                 (ccm_mep_db_entry->flags & DNX_OAMP_OAM_CCM_MEP_Q_ENTRY),
                                                 ccm_mep_db_entry->ccm_interval, 0 /* don't care for opcodes other than CCM */,
                                                 OAMP_MEP_TX_OPCODE_DMM, mep_profile_data));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Remove delay information from OAMP MEP profile data.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] ccm_mep_db_entry - Pointer to CCM MEP DB entry contents.
 * \param [out] mep_profile_data - Pointer to the structure,
 *                 containing information about the 
 *                 OAMP MEP PROFILE content.
 *   
 * \return
 *   shr_error_e 
 *   
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_oamp_remove_delay_info_from_mep_profile_data(
    int unit,
    const dnx_oam_oamp_ccm_endpoint_t * ccm_mep_db_entry,
    dnx_oam_mep_profile_t * mep_profile_data)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Remove the DM params 
     */
    mep_profile_data->report_mode_dm = 0;
    mep_profile_data->dmm_rate = 0;
    mep_profile_data->dm_measurement_type = 0;
    /*
     * Setting phase count for DMM
     */
    SHR_IF_ERR_EXIT(dnx_oam_oamp_scan_count_calc(unit, 0 /* dont care for opcodes other than CCM */ ,
                                                 (ccm_mep_db_entry->flags & DNX_OAMP_OAM_CCM_MEP_Q_ENTRY),
                                                 ccm_mep_db_entry->ccm_interval, 0 /* don't care for opcodes other than CCM */,
                                                 OAMP_MEP_TX_OPCODE_DMM, mep_profile_data));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Allocate the MEP DB index for DM
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] delay_ptr - Pointer to delay information structure.
 * \param [in] ccm_mep_db_entry - Pointer to the CCM MEP DB structure
 * \param [in] lm_db_exists - Whether LM DB already exists or not.
 * \param [in] lm_db_entry_ptr - If exists, what is the LM DB entry ID.
 * \param [in] lm_stat_exists - Whether LM stat already exists or not.
 * \param [in] lm_stat_entry_ptr - If exists, what is the LM stat entry ID.
 *   
 * \return
 *   shr_error_e 
 *   
 * \see
 *   * None
 */

static shr_error_e
dnx_oam_delay_mep_id_alloc(
    int unit,
    const bcm_oam_delay_t * delay_ptr,
    const dnx_oam_oamp_ccm_endpoint_t * ccm_mep_db_entry,
    uint8 lm_db_exists,
    uint32 lm_db_entry_ptr,
    uint8 lm_stat_exists,
    uint32 lm_stat_entry_ptr)
{
    uint32 entry_id = 0;
    dnx_oamp_mep_db_memory_type_t mep_id_memory_type;
    uint8 offloaded = 0;
    SHR_FUNC_INIT_VARS(unit);

    offloaded = (ccm_mep_db_entry->flags & DNX_OAMP_OAM_CCM_MEP_LM_DM_OFFLOADED) ? 1 : 0;

    if (offloaded)
    {
        if (!lm_stat_exists)
        {
            /*
             * part2 ptr given by user 
             */
            entry_id = delay_ptr->delay_id;
        }       /* Else If stat entry exists, we have nothing to do */
    }
    else        /* Self contained */
    {
        /*
         * If there is no LM alreay, use the ID passed by application 
         */
        entry_id = delay_ptr->delay_id;
        if (lm_db_exists)
        {
            /*
             * calculate the ID(s) from the DB/STAT entry. 
             */
            entry_id = DNX_OAM_GET_NEXT_LM_DM_CHAIN_INDEX(lm_db_entry_ptr);
            if (lm_stat_exists)
            {
                entry_id = DNX_OAM_GET_NEXT_LM_DM_CHAIN_INDEX(lm_stat_entry_ptr);
            }
        }
    }

    if (entry_id)
    {
        /*
         * Since we pass the ID, mep_id_memory_type info need not be filled. 
         */
        SHR_IF_ERR_EXIT(dnx_algo_oamp_mep_id_alloc(unit,
                                                   SW_STATE_ALGO_RES_ALLOCATE_WITH_ID, &mep_id_memory_type, &entry_id));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Free up the MEP DB indexes for DM
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] delay_id - MEP DB index of the LM DB entry
 * \param [in] ccm_mep_db_entry - Pointer to the CCM MEP DB structure
 * \param [in] lm_exists - Whether LM already exists or not.
 * \param [in] lm_db_entry_idx - Index of LM_DB entry if exists.
 * \param [in] lm_stat_exists - Whether LM_STAT exists or not.
 * \param [in] lm_stat_entry_idx - Index of LM_STAT entry if exists.
 *   
 * \return
 *   shr_error_e 
 *   
 * \see
 *   * None
 */

static shr_error_e
dnx_oam_delay_mep_id_free(
    int unit,
    uint32 delay_id,
    const dnx_oam_oamp_ccm_endpoint_t * ccm_mep_db_entry,
    uint8 lm_exists,
    uint32 lm_db_entry_idx,
    uint8 lm_stat_exists,
    uint32 lm_stat_entry_idx)
{
    uint32 entry_id = 0;
    uint8 offloaded = 0;
    SHR_FUNC_INIT_VARS(unit);

    offloaded = (ccm_mep_db_entry->flags & DNX_OAMP_OAM_CCM_MEP_LM_DM_OFFLOADED) ? 1 : 0;

    if (offloaded)
    {
        if (!lm_exists)
        {
            /*
             * part2 ptr given by user 
             */
            entry_id = delay_id;
        }       /* Else If LM entry exists, we have nothing to do */
    }
    else        /* Self contained */
    {
        if (lm_exists && (ccm_mep_db_entry->flex_lm_dm_entry != lm_db_entry_idx))
        {
            /*
             * DM entry is first.
             * LM_DB entry will be moved to DM entry index.
             * LM_STAT entry (if exists) will be moved to LM_DB entry index.
             */
            if (lm_stat_exists)
            {
                /*
                 * Deallocate LM_STAT entry
                 */
                entry_id = lm_stat_entry_idx;
            }
            else
            {
                /*
                 * Deallocate LM_DB entry
                 */
                entry_id = lm_db_entry_idx;
            }
        }
        else
        {
            /*
             * LM entries are first.
             * Deallocate DM entry
             */
            entry_id = delay_id;
        }
    }

    if (entry_id)
    {
        SHR_IF_ERR_EXIT(dnx_algo_oamp_mep_id_dealloc(unit, entry_id));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Convert Delay structure information to OAMP DM MEP DB
 *          HW contents.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] delay_ptr - Pointer to delay information structure.
 * \param [in] ccm_mep_db_entry - Pointer to the CCM MEP DB structure
 * \param [out] hw_write_data - Pointer to the structure,
 *                 containing information about the 
 *                 OAMP DM MEP DB content
 *   
 * \return
 *   shr_error_e 
 *   
 * \see
 *   * None
 */
static void
dnx_oam_delay_info_to_dm_mep_db_hw_write_info(
    int unit,
    const bcm_oam_delay_t * delay_ptr,
    const dnx_oam_oamp_ccm_endpoint_t * ccm_mep_db_entry,
    dnx_oam_loss_delay_oamp_hw_write_data_t * hw_write_data)
{
    /*
     * Offloaded endpoint is true if CCM MEP db entry get says offloaded 
     */
    if (ccm_mep_db_entry->flags & DNX_OAMP_OAM_CCM_MEP_LM_DM_OFFLOADED)
    {
        hw_write_data->offloaded = 1;
    }

    /*
     * CCM OAM id  = delay -> id 
     */
    hw_write_data->ccm_oam_id = delay_ptr->id;
    /*
     * stat enable should be true on delay add API call 
     */
    hw_write_data->stat_enable = 1;
    hw_write_data->update_mep_db = (delay_ptr->flags & BCM_OAM_DELAY_UPDATE) ? 1 : 0;
    hw_write_data->is_1dm = (delay_ptr->flags & BCM_OAM_DELAY_ONE_WAY) ? 1 : 0;

    /*
     * part_2_ptr is filled if offloaded is true. 
     * dm_stat entry is filled if selfcontained is true 
     */
    hw_write_data->flex_lm_dm_entry = OAM_ID_TO_MEP_DB_ENTRY(delay_ptr->delay_id);

}

/**
 * \brief - Write OAMP DM MEP DB info
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] hw_write_data - Pointer to the structure,
 *                 containing information about the 
 *                 OAMP DM MEP DB content
 *   
 * \return
 *   shr_error_e 
 *   
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_act_on_oamp_dm_mep_db_hw_write_info(
    int unit,
    const dnx_oam_loss_delay_oamp_hw_write_data_t * hw_write_data)
{
    dnx_oam_oamp_endpoint_dm_params_t dm_mep_db_write_data = { 0 };
    SHR_FUNC_INIT_VARS(unit);

    dm_mep_db_write_data.flags |=
        hw_write_data->is_1dm ? DNX_OAMP_OAM_DM_MEP_MEASURE_ONE_WAY : DNX_OAMP_OAM_DM_MEP_MEASURE_TWO_WAY;
    if (hw_write_data->offloaded)
    {
        /*
         * For offloaded, setting TWO WAY entry is must, since part 2 entry is required. 
         */
        dm_mep_db_write_data.flags |= DNX_OAMP_OAM_DM_MEP_MEASURE_TWO_WAY;
    }
    dm_mep_db_write_data.lmm_dmm_da_profile = hw_write_data->oamp_da_lsb_prof;
    dm_mep_db_write_data.mep_profile = hw_write_data->oamp_mep_profile;
    dm_mep_db_write_data.flex_dm_entry = MEP_DB_ENTRY_TO_OAM_ID(hw_write_data->flex_lm_dm_entry);

    SHR_IF_ERR_EXIT(dnx_oamp_mep_db_dm_endpoint_set(unit, hw_write_data->ccm_oam_id, &dm_mep_db_write_data));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - Add an OAM Delay entry.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] delay_ptr - Pointer to the structure,
 *                 containing information about the delay entry.
 *   
 * \return
 *   shr_error_e 
 *   
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_oam_delay_add(
    int unit,
    bcm_oam_delay_t * delay_ptr)
{
    bcm_oam_endpoint_info_t endpoint_info;
    bcm_oam_group_info_t group_info;
    bcm_oam_endpoint_t id = 0;
    bcm_oam_endpoint_t mep_id = 0;
    dnx_oam_loss_delay_oamp_hw_write_data_t *hw_write_info = NULL;
    dnx_oam_oamp_ccm_endpoint_t *ccm_mep_db_entry = NULL;
    dnx_oam_mep_profile_t *curr_mep_profile_data = NULL;
    dnx_oam_oamp_endpoint_lm_params_t *lm_mep_db_entry = NULL;
    dnx_oam_oamp_endpoint_dm_params_t *dm_mep_db_entry = NULL;
    uint8 lm_db_exists = 0, update = 0, exchange_mac_prof = 0, lm_stat_exists = 0;
    uint8 mpls_pwe_mep = 0;
    uint32 lm_db_entry_ptr = 0, lm_stat_entry_ptr = 0;
    int counter = 0;
    uint8 priority = 0;
    bcm_mac_t oamp_mac_da_msb = { 0 }, old_oamp_mac_da_msb =
    {
    0};
    bcm_mac_t oamp_mac_da_lsb = { 0 }, old_oamp_mac_da_lsb =
    {
    0};
    uint8 old_oamp_da_lsb_prof = 0;
    uint8 offloaded = 0, dm_exists = 0;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    update = (delay_ptr->flags & BCM_OAM_DELAY_UPDATE) ? 1 : 0;

    SHR_ALLOC_SET_ZERO(hw_write_info, sizeof(dnx_oam_loss_delay_oamp_hw_write_data_t),
                       "Structure for HW WRITE Info", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(ccm_mep_db_entry, sizeof(dnx_oam_oamp_ccm_endpoint_t),
                       "Structure for CCM MEP DB entry", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(dm_mep_db_entry, sizeof(dnx_oam_oamp_endpoint_dm_params_t),
                       "Structure for DM MEP DB entry", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(lm_mep_db_entry, sizeof(dnx_oam_oamp_endpoint_lm_params_t),
                       "Structure for LM MEP DB entry", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(curr_mep_profile_data, sizeof(dnx_oam_mep_profile_t),
                       "Structure for CURR MEP PROFILE", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    /*
     * Get the current CCM mep db entry 
     */
    SHR_IF_ERR_EXIT(dnx_oamp_mep_db_ccm_endpoint_get(unit, delay_ptr->id, 0, ccm_mep_db_entry));
    mpls_pwe_mep = DNX_OAM_MEP_TYPE_IS_MPLS_TP_OR_PWE(ccm_mep_db_entry->mep_type);
    offloaded = (ccm_mep_db_entry->flags & DNX_OAMP_OAM_CCM_MEP_LM_DM_OFFLOADED) ? 1 : 0;

    /*
     * -- SW configurations -- 
     */

    /*
     * Check if Loss add is already configured on this endpoint 
     */
    SHR_IF_ERR_EXIT(dnx_oamp_mep_db_lm_endpoint_get
                    (unit, delay_ptr->id, lm_mep_db_entry, &lm_db_exists, &lm_stat_exists));

    /*
     * LM DB has to exist for LM stat to exist 
     */
    if (lm_db_exists)
    {
        DNX_OAM_GET_LOSS_ENTRY_CONFIGURED(offloaded, lm_mep_db_entry, lm_db_entry_ptr);
        if (lm_stat_exists)
        {
            DNX_OAM_GET_LOSS_STAT_ENTRY_CONFIGURED(lm_mep_db_entry, lm_stat_entry_ptr);
        }
    }

    /*
     * Call verify function 
     */
    SHR_IF_ERR_EXIT(dnx_oam_delay_add_verify(unit, delay_ptr, ccm_mep_db_entry, lm_db_exists, &priority));

    /*
     * Get the current OAMP MEP profile 
     */
    SHR_IF_ERR_EXIT(algo_oam_db.oam_mep_profile.profile_data_get(unit, _SHR_CORE_ALL,
                                                                 ccm_mep_db_entry->mep_profile, &counter,
                                                                 curr_mep_profile_data));

    /*
     * If this is an update case or in case Loss was already configured, there will be already a MAC configured.
     * Exchange is true for those 2 cases. 
     */
    exchange_mac_prof = (update || lm_db_exists);

    if (update)
    {
        SHR_IF_ERR_EXIT(dnx_oamp_mep_db_dm_endpoint_get(unit, delay_ptr->id, dm_mep_db_entry, &dm_exists));
        if (!dm_exists)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error: Delay is not enabled on this endpoint.\n");
        }
        old_oamp_da_lsb_prof = dm_mep_db_entry->lmm_dmm_da_profile;
    }
    else if (lm_db_exists)
    {
        old_oamp_da_lsb_prof = lm_mep_db_entry->lmm_dmm_da_profile;
    }

    /*
     * For exchange, get the existing mac information 
     */
    if (exchange_mac_prof && !mpls_pwe_mep)
    {
        /*
         * Get the current DMM MAC DA MSB 
         */
        SHR_IF_ERR_EXIT(algo_oam_db.oam_da_mac_msb_profile_template.profile_data_get(unit,
                                                                                     _SHR_CORE_ALL,
                                                                                     curr_mep_profile_data->oamp_mac_da_oui_prof,
                                                                                     &counter, &old_oamp_mac_da_msb));

        /*
         * Get the current DMM MAC DA LSB 
         */
        SHR_IF_ERR_EXIT(algo_oam_db.oam_da_mac_lsb_profile_template.profile_data_get(unit,
                                                                                     _SHR_CORE_ALL,
                                                                                     old_oamp_da_lsb_prof, &counter,
                                                                                     &old_oamp_mac_da_lsb));
    }

    if (!mpls_pwe_mep)
    {
        /*
         * Extract the OUI (MSB) part from the mac address 
         */
        GET_MSB_FROM_MAC_ADDR(delay_ptr->peer_da_mac_address, oamp_mac_da_msb);
        /*
         * Alloc/exchange the MSB profile 
         */
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mac_da_msb_profile_alloc(unit, oamp_mac_da_msb,
                                                              old_oamp_mac_da_msb,
                                                              curr_mep_profile_data->oamp_mac_da_oui_prof,
                                                              exchange_mac_prof, hw_write_info));

        /*
         * Extract the NIC (LSB) part from the mac address 
         */
        GET_LSB_FROM_MAC_ADDR(delay_ptr->peer_da_mac_address, oamp_mac_da_lsb);
        /*
         * Alloc/exchange the LSB profile 
         */
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mac_da_lsb_profile_alloc(unit, oamp_mac_da_lsb,
                                                              old_oamp_mac_da_lsb, old_oamp_da_lsb_prof,
                                                              exchange_mac_prof, hw_write_info));
    }

    /*
     * Fill the current MEP profile with delay params 
     */
    SHR_IF_ERR_EXIT(dnx_oam_oamp_delay_info_to_mep_profile_data
                    (unit, delay_ptr, ccm_mep_db_entry, curr_mep_profile_data));

    /*
     * update the current MEP profile with OUI (MSB) Profile 
     */
    curr_mep_profile_data->oamp_mac_da_oui_prof = hw_write_info->oamp_da_msb_prof;

    /*
     * Exchange the OAMP MEP profile with existing MEP profile 
     */
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_profile_exchange(unit, ccm_mep_db_entry->mep_profile,
                                                      curr_mep_profile_data, hw_write_info));

    /*
     * Write the session information for lm with priority
     */
    if ((delay_ptr->pkt_pri_bitmap != 0xff) && (delay_ptr->pkt_pri_bitmap != 0))
    {
       SHR_IF_ERR_EXIT(bcm_dnx_oam_endpoint_get(unit, delay_ptr->id, &endpoint_info));
       SHR_IF_ERR_EXIT(bcm_dnx_oam_group_get(unit,endpoint_info.group,&group_info));

       /*
       * Duplicate ccm entry
       */
       endpoint_info.flags &= (0xffffffff ^ BCM_OAM_ENDPOINT_WITH_ID);
       SHR_IF_ERR_EXIT(dnx_oam_local_accelerated_endpoint_fill_id(unit, &endpoint_info, group_info.name));
       id = endpoint_info.id;
   
      ccm_mep_db_entry->ccm_interval = DBAL_ENUM_FVAL_CCM_INTERVAL_NO_TX;
      mep_id = delay_ptr->id;
      delay_ptr->id = id;
    }

    /*
     * If not update, go ahead and alloc mep_db index(es).
     */
    if (!update)
    {
        SHR_IF_ERR_EXIT(dnx_oam_delay_mep_id_alloc(unit, delay_ptr, ccm_mep_db_entry,
                                                   lm_db_exists, lm_db_entry_ptr, lm_stat_exists, lm_stat_entry_ptr));
    }

    /*
     * Update the HW write info from SW info for DM MEP DB 
     */
    dnx_oam_delay_info_to_dm_mep_db_hw_write_info(unit, delay_ptr, ccm_mep_db_entry, hw_write_info);

    /*
     * -- HW configurations -- 
     */

    /*
     * Write replica ccm and lm_session_map entries in case of dm with priority
     */
    if ((delay_ptr->pkt_pri_bitmap != 0xff) && (delay_ptr->pkt_pri_bitmap != 0))
    {
       SHR_IF_ERR_EXIT(dnx_oamp_mep_db_ccm_endpoint_set(unit,id,ccm_mep_db_entry));

     /*
      * Set oam_id session_map
      */
     SHR_IF_ERR_EXIT(dnx_oam_oamp_lm_session_set(unit,
                                                   mep_id,
                                                   delay_ptr->id,
                                                   priority));
    }

    /*
     * Write the DA MAC msb into register. 
     */
    if (hw_write_info->flags & DNX_OAM_LM_DM_MAC_DA_MSB_PROFILE)
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mac_da_msb_profile_set(unit,
                                                            hw_write_info->oamp_da_msb_prof,
                                                            hw_write_info->oamp_mac_da_msb));
    }

    /*
     * Write the DA MAC lsb into register. 
     */
    if (hw_write_info->flags & DNX_OAM_LM_DM_MAC_DA_LSB_PROFILE)
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mac_da_lsb_profile_set(unit,
                                                            hw_write_info->oamp_da_lsb_prof,
                                                            hw_write_info->oamp_mac_da_lsb));
    }

    /*
     *  Write the OAMP_MEP_PROFILE (along with DA MAC MSB profile) 
     */
    if (hw_write_info->flags & DNX_OAM_LM_DM_OAMP_MEP_PROFILE)
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_profile_set(unit,
                                                     hw_write_info->oamp_mep_profile,
                                                     &(hw_write_info->mep_profile_data)));
    }

    /*
     * Write the MEP DB entry(ies) (along with DA MAC LSB profile) 
     */
    SHR_IF_ERR_EXIT(dnx_oam_act_on_oamp_dm_mep_db_hw_write_info(unit, hw_write_info));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FREE(hw_write_info);
    SHR_FREE(ccm_mep_db_entry);
    SHR_FREE(lm_mep_db_entry);
    SHR_FREE(dm_mep_db_entry);
    SHR_FREE(curr_mep_profile_data);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get an OAM delay entry.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] delay_ptr - Pointer to the structure,
 *                 containing information about the delay entry.
 *   
 * \return
 *   shr_error_e 
 *   
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_oam_delay_get(
    int unit,
    bcm_oam_delay_t * delay_ptr)
{
    uint32 session_oam_id;
    int priority;
    dnx_oam_oamp_endpoint_dm_params_t *delay_params = NULL;
    dnx_oam_oamp_ccm_endpoint_t *ccm_mep_db_entry = NULL;
    dnx_oam_mep_profile_t *curr_mep_profile_data = NULL;
    bcm_mac_t oamp_mac_da_msb = { 0 };
    bcm_mac_t oamp_mac_da_lsb = { 0 };
    uint8 offloaded = 0, dm_exists = 0;
    uint8 mpls_pwe_mep = 0;
    int counter = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(delay_params, sizeof(dnx_oam_oamp_endpoint_dm_params_t),
                       "Structure for DM MEP DB entry", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(ccm_mep_db_entry, sizeof(dnx_oam_oamp_ccm_endpoint_t),
                       "Structure for CCM MEP DB entry", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(curr_mep_profile_data, sizeof(dnx_oam_mep_profile_t),
                       "Structure for CURR MEP PROFILE", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    /*
     * Get lm with priority info
     */
    if ((delay_ptr->pkt_pri_bitmap != 0xff) && (delay_ptr->pkt_pri_bitmap != 0))
    {
       switch (delay_ptr->pkt_pri_bitmap) 
       {
          case 255:
          case 1: priority=0; break;
          case 2: priority=1; break;
          case 4: priority=2; break;
          case 8: priority=3; break;
          case 16: priority=4; break;
          case 32: priority=5; break;
          case 64: priority=6; break;
          case 128: priority=7; break;
          default:  SHR_IF_ERR_EXIT(_SHR_E_PARAM);
       }
       SHR_IF_ERR_EXIT(dnx_oamp_lm_with_priority_session_map_get( unit, delay_ptr->id, priority, &session_oam_id));

       delay_ptr->id = session_oam_id;
    }

     /*
     * First, read the CCM entry   
     */
    SHR_IF_ERR_EXIT(dnx_oamp_mep_db_ccm_endpoint_get(unit, delay_ptr->id, 0, ccm_mep_db_entry));
    /*
     * Get the DM MEP DB entry 
     */
    SHR_IF_ERR_EXIT(dnx_oamp_mep_db_dm_endpoint_get(unit, delay_ptr->id, delay_params, &dm_exists));

    if (!dm_exists)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: Delay is not enabled on this endpoint.\n");
    }

    offloaded = (ccm_mep_db_entry->flags & DNX_OAMP_OAM_CCM_MEP_LM_DM_OFFLOADED) ? 1 : 0;
    mpls_pwe_mep = DNX_OAM_MEP_TYPE_IS_MPLS_TP_OR_PWE(ccm_mep_db_entry->mep_type);

    /*
     * Get the OAMP MEP profile 
     */
    SHR_IF_ERR_EXIT(algo_oam_db.oam_mep_profile.profile_data_get(unit, _SHR_CORE_ALL,
                                                                 ccm_mep_db_entry->mep_profile,
                                                                 &counter, curr_mep_profile_data));

    if (!mpls_pwe_mep)
    {
        /*
         * Get the DMM DA OUI 
         */
        SHR_IF_ERR_EXIT(algo_oam_db.oam_da_mac_msb_profile_template.profile_data_get(unit,
                                                                                     _SHR_CORE_ALL,
                                                                                     curr_mep_profile_data->oamp_mac_da_oui_prof,
                                                                                     &counter, &oamp_mac_da_msb));

        /*
         * Get the DMM DA NIC 
         */
        SHR_IF_ERR_EXIT(algo_oam_db.oam_da_mac_lsb_profile_template.profile_data_get(unit,
                                                                                     _SHR_CORE_ALL,
                                                                                     delay_params->lmm_dmm_da_profile,
                                                                                     &counter, &oamp_mac_da_lsb));

        /*
         * Combined MSB and LSB to form FULL MAC DA 
         */
        GET_MAC_FROM_MSB_LSB(oamp_mac_da_msb, oamp_mac_da_lsb, delay_ptr->peer_da_mac_address);
    }

    /*
     * Fill the delay pointer 
     */
    if (offloaded)
    {
        delay_ptr->delay_id = delay_params->flex_dm_entry;
    }
    else
    {
        delay_ptr->delay_id = delay_params->dm_stat_entry_idx;
    }
    delay_ptr->period = curr_mep_profile_data->dmm_rate;

    if (!curr_mep_profile_data->report_mode_dm)
    {
        delay_ptr->delay.nanoseconds = (delay_params->last_delay[0]) |
            (delay_params->last_delay[1] << 8) |
            (delay_params->last_delay[2] << 16) | ((delay_params->last_delay[3] & 0x3F) << 24);
        delay_ptr->delay.seconds = ((delay_params->last_delay[3] & 0xC0) >> 6) |
            (delay_params->last_delay[4] << 2) | ((delay_params->last_delay[5] & 0x3) << 10);
        delay_ptr->delay_max.nanoseconds = (delay_params->max_delay[0]) |
            (delay_params->max_delay[1] << 8) |
            (delay_params->max_delay[2] << 16) | ((delay_params->max_delay[3] & 0x3F) << 24);
        delay_ptr->delay_max.seconds = ((delay_params->max_delay[3] & 0xC0) >> 6) |
            (delay_params->max_delay[4] << 2) | ((delay_params->max_delay[5] & 0x3) << 10);
        delay_ptr->delay_min.nanoseconds = (delay_params->min_delay[0]) |
            (delay_params->min_delay[1] << 8) |
            (delay_params->min_delay[2] << 16) | ((delay_params->min_delay[3] & 0x3F) << 24);
        delay_ptr->delay_min.seconds = ((delay_params->min_delay[3] & 0xC0) >> 6) |
            (delay_params->min_delay[4] << 2) | ((delay_params->min_delay[5] & 0x3) << 10);
        /*
         * OAMP truncates the two lower bits of the microsecdonds. SW must recompensate.
         */
        delay_ptr->delay.nanoseconds <<= 2;
        delay_ptr->delay_max.nanoseconds <<= 2;
        delay_ptr->delay_min.nanoseconds <<= 2;

    }
    else
    {
        delay_ptr->flags |= BCM_OAM_DELAY_REPORT_MODE;
    }
    delay_ptr->timestamp_format = bcmOAMTimestampFormatIEEE1588v1;

    if (delay_params->flags & DNX_OAMP_OAM_DM_MEP_MEASURE_ONE_WAY)
    {
        delay_ptr->flags |= BCM_OAM_DELAY_ONE_WAY;
    }
exit:
    SHR_FREE(delay_params);
    SHR_FREE(curr_mep_profile_data);
    SHR_FREE(ccm_mep_db_entry);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Delete an OAM delay entry.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] delay_ptr - Pointer to the structure,
 *                 containing information about the delay entry.
 *   
 * \return
 *   shr_error_e 
 *   
 * \see
 *   * None
 */
shr_error_e
bcm_dnx_oam_delay_delete(
    int unit,
    bcm_oam_delay_t * delay_ptr)
{
    bcm_oam_endpoint_info_t endpoint_info;
    bcm_oam_group_info_t group_info;
    uint32 session_oam_id;
    int priority;
    uint8 extra_data_entries_on_mdb = 0;
    dnx_oam_oamp_ccm_endpoint_t *ccm_mep_db_entry = NULL;
    dnx_oam_oamp_endpoint_dm_params_t *delay_params = NULL;
    dnx_oam_oamp_endpoint_lm_params_t *lm_mep_db_entry = NULL;
    dnx_oam_mep_profile_t *curr_mep_profile_data = NULL;
    dnx_oam_loss_delay_oamp_hw_write_data_t *hw_write_info = NULL;
    uint8 oamp_da_lsb_prof = 0;
    int counter = 0;
    uint8 lm_db_exists = 0, delete_old_prof = 0, mpls_pwe_mep = 0, lm_stat_exists = 0;
    uint8 offloaded = 0, dm_exists = 0;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_ALLOC_SET_ZERO(hw_write_info, sizeof(dnx_oam_loss_delay_oamp_hw_write_data_t),
                       "Structure for HW WRITE Info", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(ccm_mep_db_entry, sizeof(dnx_oam_oamp_ccm_endpoint_t),
                       "Structure for CCM MEP DB entry", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(lm_mep_db_entry, sizeof(dnx_oam_oamp_endpoint_lm_params_t),
                       "Structure for DM MEP DB entry", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(delay_params, sizeof(dnx_oam_oamp_endpoint_dm_params_t),
                       "Structure for LM MEP DB entry", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(curr_mep_profile_data, sizeof(dnx_oam_mep_profile_t),
                       "Structure for CURR MEP PROFILE", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    /*
     * In case it's lm with priorty, delete the session map entry
     */
    if ((delay_ptr->pkt_pri_bitmap != 0xff) && (delay_ptr->pkt_pri_bitmap != 0))
    {
       SHR_IF_ERR_EXIT(bcm_dnx_oam_endpoint_get(unit, delay_ptr->id, &endpoint_info));

       SHR_IF_ERR_EXIT(bcm_oam_group_get(unit, endpoint_info.group, &group_info));

       extra_data_entries_on_mdb = _SHR_IS_FLAG_SET(group_info.flags, BCM_OAM_GROUP_FLEXIBLE_MAID_48_BYTE) &&
           (endpoint_info.endpoint_memory_type != bcmOamEndpointMemoryTypeSelfContained);


       switch (delay_ptr->pkt_pri_bitmap) 
       {
          case 255:
          case 1: priority=0; break;
          case 2: priority=1; break;
          case 4: priority=2; break;
          case 8: priority=3; break;
          case 16: priority=4; break;
          case 32: priority=5; break;
          case 64: priority=6; break;
          case 128: priority=7; break;
          default:  SHR_IF_ERR_EXIT(_SHR_E_PARAM);
       }
       SHR_IF_ERR_EXIT(dnx_oamp_lm_with_priority_session_map_get( unit, delay_ptr->id, priority, &session_oam_id));

       SHR_IF_ERR_EXIT(dnx_oamp_lm_with_priority_session_map_clear(unit, delay_ptr->id, priority));

       delay_ptr->id = session_oam_id;
    }

    /*
     * Get the current CCM mep db entry 
     */
    SHR_IF_ERR_EXIT(dnx_oamp_mep_db_ccm_endpoint_get(unit, delay_ptr->id, 0, ccm_mep_db_entry));
    mpls_pwe_mep = DNX_OAM_MEP_TYPE_IS_MPLS_TP_OR_PWE(ccm_mep_db_entry->mep_type);
    offloaded = (ccm_mep_db_entry->flags & DNX_OAMP_OAM_CCM_MEP_LM_DM_OFFLOADED) ? 1 : 0;

    /*
     * Get the DM MEP DB entry 
     */
    SHR_IF_ERR_EXIT(dnx_oamp_mep_db_dm_endpoint_get(unit, delay_ptr->id, delay_params, &dm_exists));

    if (offloaded)
    {
        delay_ptr->delay_id = delay_params->flex_dm_entry;
    }
    else
    {
        delay_ptr->delay_id = delay_params->dm_stat_entry_idx;
    }
    oamp_da_lsb_prof = delay_params->lmm_dmm_da_profile;

    /*
     * Get the current OAMP MEP profile 
     */
    SHR_IF_ERR_EXIT(algo_oam_db.oam_mep_profile.profile_data_get(unit, _SHR_CORE_ALL,
                                                                 ccm_mep_db_entry->mep_profile, &counter,
                                                                 curr_mep_profile_data));

    /*
     *  -- SW configurations -- 
     */

    /*
     * Check if Loss add is already configured on this endpoint 
     */
    SHR_IF_ERR_EXIT(dnx_oamp_mep_db_lm_endpoint_get
                    (unit, delay_ptr->id, lm_mep_db_entry, &lm_db_exists, &lm_stat_exists));

    if (!mpls_pwe_mep && !lm_db_exists)
    {
        /*
         * Free the DA MAC LSB profile or (ref_count - 1) 
         */
        SHR_IF_ERR_EXIT(algo_oam_db.oam_da_mac_msb_profile_template.free_single(unit,
                                                                                _SHR_CORE_ALL,
                                                                                curr_mep_profile_data->oamp_mac_da_oui_prof,
                                                                                &delete_old_prof));
        /*
         * Free the DA MAC MSB profile or (ref_count - 1) 
         */
        SHR_IF_ERR_EXIT(algo_oam_db.oam_da_mac_lsb_profile_template.free_single(unit,
                                                                                _SHR_CORE_ALL, oamp_da_lsb_prof,
                                                                                &delete_old_prof));
    }

    /*
     * Free the OAMP MEP profile from delay params. 
     */
    SHR_IF_ERR_EXIT(dnx_oam_oamp_remove_delay_info_from_mep_profile_data
                    (unit, ccm_mep_db_entry, curr_mep_profile_data));
    /*
     * Exchange the OLD profile and get NEW profile. 
     */
    SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_profile_exchange(unit, ccm_mep_db_entry->mep_profile,
                                                      curr_mep_profile_data, hw_write_info));

    /*
     * Free up the OAMP MEP ID(s) 
     */
    SHR_IF_ERR_EXIT(dnx_oam_delay_mep_id_free(unit, delay_ptr->delay_id, ccm_mep_db_entry,
                                              lm_db_exists, lm_mep_db_entry->lm_db_entry_idx,
                                              lm_stat_exists, lm_mep_db_entry->lm_stat_entry_idx));

    /*
     * ---- HW configurations --- 
     */

    /*
     * Write the new OAMP MEP profile if required 
     */
    if (hw_write_info->flags & DNX_OAM_LM_DM_OAMP_MEP_PROFILE)
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_mep_profile_set(unit,
                                                     hw_write_info->oamp_mep_profile,
                                                     &(hw_write_info->mep_profile_data)));
    }

    /*
     * Clear the delay MEP DB entry in HW 
     */
    SHR_IF_ERR_EXIT(dnx_oamp_mep_db_dm_endpoint_clear(unit, delay_ptr->id));

    /*
     * In case it's dm with priority, clear the ccm entry
     */
    SHR_IF_ERR_EXIT(dnx_oamp_mep_db_ccm_endpoint_get(unit, delay_ptr->id, 0, ccm_mep_db_entry));
    if ((delay_ptr->pkt_pri_bitmap != 0xff) && (delay_ptr->pkt_pri_bitmap != 0))
    {
       SHR_IF_ERR_EXIT(dnx_oamp_mep_db_ccm_endpoint_clear(unit, delay_ptr->id, extra_data_entries_on_mdb));
    }
    else
    {
      /*
       * Update the CCM mep db entry with new OAMP MEP profile 
       */
      ccm_mep_db_entry->mep_profile = hw_write_info->oamp_mep_profile;
      ccm_mep_db_entry->flags |= DNX_OAMP_OAM_CCM_MEP_UPDATE;
      SHR_IF_ERR_EXIT(dnx_oamp_mep_db_ccm_endpoint_set(unit, delay_ptr->id, ccm_mep_db_entry));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FREE(hw_write_info);
    SHR_FREE(ccm_mep_db_entry);
    SHR_FREE(delay_params);
    SHR_FREE(lm_mep_db_entry);
    SHR_FREE(curr_mep_profile_data);
    SHR_FUNC_EXIT;
}
