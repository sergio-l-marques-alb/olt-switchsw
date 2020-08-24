/** \file oam_reflector.c
 * $Id$
 *
 * OAM Reflector procedures for DNX.
 */
/*
 * $Copyright: (c) 2019 Broadcom.
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
#include <bcm/oam.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm/types.h>
#include "oam_internal.h"
#include "oam_counter.h"
#include <soc/dnx/swstate/auto_generated/access/oam_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <bcm_int/dnx/lif/lif_lib.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr.h>

/*
 * }
 */

/*
 * DEFINEs
 * {
 */
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

/**
 * \brief
 *  Verification of bcm_dnx_oam_reflector_encap_create() input parameters.
 *  For details about the parameters refer to bcm_dnx_oam_reflector_encap_create()
 *  side.
*/
static shr_error_e
dnx_oam_reflector_encap_create_verify(
    int unit,
    uint32 flags,
    bcm_if_t * encap_id)
{
    uint32 supported_flags;
    uint32 hw_resources_flags;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    bcm_gport_t gport;
    int is_allocated, global_lif_id;
    SHR_FUNC_INIT_VARS(unit);

    /** flags */
    supported_flags = BCM_OAM_REFELCTOR_ENCAP_WITH_ID;
    SHR_MASK_VERIFY(flags, supported_flags, _SHR_E_PARAM, "some of the flags are not supported \n");

    /** encap_id */
    SHR_NULL_CHECK(encap_id, _SHR_E_PARAM, "encap_id");
    if (flags & BCM_OAM_REFELCTOR_ENCAP_WITH_ID)
    {
        /*
         * range check 
         */
        global_lif_id = BCM_L3_ITF_VAL_GET(*encap_id);
        LIF_MNGR_GLOBAL_OUT_LIF_VERIFY(unit, global_lif_id);

         /** free check */
        BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(gport, *encap_id);
        hw_resources_flags =
            DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS |
            DNX_ALGO_GPM_GPORT_HW_RESOURCES_NON_STRICT;
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, gport, hw_resources_flags, &gport_hw_resources));
        if (gport_hw_resources.local_out_lif != DNX_ALGO_GPM_LIF_INVALID)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Encap ID 0x%x, was already allocated.\n", *encap_id);
        }
    }

    /** make sure that no other encap allocated for OAM reflector */
    SHR_IF_ERR_EXIT(oam_sw_db_info.reflector.is_allocated.get(unit, &is_allocated));
    if (is_allocated == 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "encap ID for oam reflector already allocated.\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Verification of bcm_dnx_oam_reflector_encap_destroy() input parameters.
 *  For details about the parameters refer to bcm_dnx_oam_reflector_encap_destroy()
 *  side.
*/
static shr_error_e
dnx_oam_reflector_encap_destroy_verify(
    int unit,
    uint32 flags,
    bcm_if_t encap_id)
{
    uint32 supported_flags;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    uint32 hw_resources_flags;
    bcm_gport_t gport;
    int global_lif_id;
    SHR_FUNC_INIT_VARS(unit);

    /** flags */
    supported_flags = 0;
    SHR_MASK_VERIFY(flags, supported_flags, _SHR_E_PARAM, "some of the flags are not supported \n");

    /** encap_id */
    /*
     * range check 
     */
    global_lif_id = BCM_L3_ITF_VAL_GET(encap_id);
    LIF_MNGR_GLOBAL_OUT_LIF_VERIFY(unit, global_lif_id);
    /** verify outlif allocated and  type */
    BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(gport, encap_id);
    hw_resources_flags =
        DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS |
        DNX_ALGO_GPM_GPORT_HW_RESOURCES_NON_STRICT;
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, gport, hw_resources_flags, &gport_hw_resources));
    if (gport_hw_resources.outlif_dbal_table_id != DBAL_TABLE_EEDB_OAM_REFLECTOR)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Expecting OAM reflector encap ID, got 0x%x", encap_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Set OUTLIF used for downmep reflector.
 *  The only field required to be set is the opcode to stamp (reflector requires LBR=2)
 *  side.
 * \param [in] unit  - unit #.
 * \param [in,out] local_lif - local LIF #
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref
 *           shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
*/
static shr_error_e
dnx_oam_reflector_encap_set(
    int unit,
    int local_lif)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Take DBAL handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_OAM_REFLECTOR, &entry_handle_id));

    /** Set KEY field */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, local_lif);

    /** Set DATA fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_EEDB_OAM_REFLECTOR_ETPS_REFLECTOR_OAM);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RAW_DATA, INST_SINGLE, bcmOamOpcodeLBR);
    /** Write to HW */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *  Clear OUTLIF used for downmep reflector.
 *  The only field required to be set is the opcode to stamp (reflector requires LBR=2)
 *  side.
 * \param [in] unit  - unit #.
 * \param [in,out] local_lif - local LIF #
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref
 *           shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
*/
static shr_error_e
dnx_oam_reflector_encap_clear(
    int unit,
    uint32 local_lif)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_OAM_REFLECTOR, &entry_handle_id));

    /** Setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, local_lif);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_EEDB_OAM_REFLECTOR_ETPS_REFLECTOR_OAM);

    /** clearing the entry */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *  Allocate OUTLIF used by unicast Ethernet loopback Down MEP.
 *  side.
 * \param [in] unit  - unit #.
 * \param [in] flags  - Set to BCM_OAM_REFELCTOR_ENCAP_WITH_ID to allocate a specific OUTLIF or 0 otherwise
 * \param [in,out] encap_id - IN: Specify the required OUTLIF in a case flag BCM_OAM_REFELCTOR_ENCAP_WITH_ID set.
 *                            OUT: the allocated OUTTLIF
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref
 *           shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
*/
int
bcm_dnx_oam_reflector_encap_create(
    int unit,
    uint32 flags,
    bcm_if_t * encap_id)
{
    lif_mngr_local_outlif_info_t outlif_info;
    uint32 lif_alloc_flags;
    int global_lif_id;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** Verification of input parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_oam_reflector_encap_create_verify(unit, flags, encap_id));

    /** Allocate OUTLIF: both global and local. */
    lif_alloc_flags = 0;
    if (flags & BCM_OAM_REFELCTOR_ENCAP_WITH_ID)
    {
        lif_alloc_flags |= LIF_MNGR_GLOBAL_LIF_WITH_ID;
        global_lif_id = BCM_L3_ITF_VAL_GET(*encap_id);
    }
    sal_memset(&outlif_info, 0, sizeof(lif_mngr_local_outlif_info_t));
    outlif_info.dbal_table_id = DBAL_TABLE_EEDB_OAM_REFLECTOR;
    outlif_info.dbal_result_type = DBAL_RESULT_TYPE_EEDB_OAM_REFLECTOR_ETPS_REFLECTOR_OAM;
    /** Allocate LIF - WITH_ID if the flag was set */
    SHR_IF_ERR_EXIT(dnx_lif_lib_allocate(unit, lif_alloc_flags, &global_lif_id, NULL, &outlif_info));
    BCM_L3_ITF_SET(*encap_id, BCM_L3_ITF_TYPE_LIF, global_lif_id);
    /** Set OUTLIF */
    SHR_IF_ERR_EXIT(dnx_oam_reflector_encap_set(unit, outlif_info.local_outlif));

    /** Write global to local LIF mapping to GLEM. */
    SHR_IF_ERR_EXIT(dnx_lif_lib_add_to_glem(unit, BCM_CORE_ALL, global_lif_id, outlif_info.local_outlif));

    /** store info in sw state */
    SHR_IF_ERR_EXIT(oam_sw_db_info.reflector.encap_id.set(unit, *encap_id));
    SHR_IF_ERR_EXIT(oam_sw_db_info.reflector.is_allocated.set(unit, 1));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  De-allocate OUTLIF used by UC Ethernet downmep reflector.
 *  side.
 * \param [in] unit  - unit #.
 * \param [in] flags  - Set to 0
 * \param [in] encap_id - Specify the OUTLIF allocated by bcm_dnx_oam_reflector_encap_create().
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref
 *           shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
*/
int
bcm_dnx_oam_reflector_encap_destroy(
    int unit,
    uint32 flags,
    bcm_if_t encap_id)
{
    uint32 local_lif, global_lif_id;
    uint32 hw_resources_flags;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_resources;
    bcm_gport_t gport;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /** Verification of input parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_oam_reflector_encap_destroy_verify(unit, flags, encap_id));

    /** get global lin and local lif*/
    global_lif_id = BCM_L3_ITF_VAL_GET(encap_id);
    BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(gport, encap_id);
    hw_resources_flags =
        DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS;
    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, gport, hw_resources_flags, &gport_hw_resources));
    local_lif = gport_hw_resources.local_out_lif;

    /** Remove global to local LIF mapping from GLEM. */
    SHR_IF_ERR_EXIT(dnx_lif_lib_remove_from_glem(unit, BCM_CORE_ALL, global_lif_id));

    /** Clear table values */
    SHR_IF_ERR_EXIT(dnx_oam_reflector_encap_clear(unit, local_lif));

    /** Free LIF */
    SHR_IF_ERR_EXIT(dnx_lif_lib_free(unit, global_lif_id, NULL, local_lif));

    /** mark as not allocated */
    SHR_IF_ERR_EXIT(oam_sw_db_info.reflector.encap_id.set(unit, -1));
    SHR_IF_ERR_EXIT(oam_sw_db_info.reflector.is_allocated.set(unit, 0));

exit:
    SHR_FUNC_EXIT;
}
