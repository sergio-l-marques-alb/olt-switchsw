/** \file field_key.c
 * $Id$
 *
 * Field key procedures for DNX.
 *
 * Key creation allocation and configuration
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDPROCDNX
/*
 * Include files.
 * {
 */

#include <shared/shrextend/shrextend_debug.h>

#include <bcm_int/dnx/field/field_key.h>
#include <bcm_int/dnx/field/field_actions.h>
#include <bcm_int/dnx/algo/field/algo_field.h>
#include <bcm_int/dnx/field/field_map.h>
#include <bcm_int/dnx/field/field_context.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_field.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_algo_field_key_alloc_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_access.h>
#include <bcm_int/dnx/kbp/kbp_mngr.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_access_profile_access.h>
/*
 * }Include files
 */

#define DNX_FIELD_KEY_KBR_APP_DP_ID_DEFAULT (0)
/** Please look at FLP spec section FFC usage (there is an example 1.12.2.4.3.1.1)  */

/** 
 * Returns the offset to be written to the FFC for header.
 * Retruns negative value if invalid.
 */
#define DNX_FIELD_KEY_FFC_LAYER_OFFSET(_unit, _stage, _offset, _size) \
   ((dnx_data_field.stage.stage_info_get(_unit, _stage)->pbus_header_length > 0) ? \
    (dnx_data_field.stage.stage_info_get(_unit, _stage)->pbus_header_length - (_offset) - (_size)) : \
    (-1))

/** 
 * The maximum number of sub qualifiers a constant qualifier can split to. 
 * The number is the maximum number of consecutive ones without intermittent zeros in uint32 (input argument).
 * Splitting of non native qualifiers (such as header qualifier in iPMF2) to multiple stages isn't counted in 
 * this define.
 */
#define DNX_FIELD_KEY_MAX_CONST_SUB_QUAL (SAL_UINT32_NOF_BITS/2+1)

/**
* See field_key.h
*/
shr_error_e
dnx_field_key_template_t_init(
    int unit,
    dnx_field_key_template_t * key_template_p)
{
    int qual_idx;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(key_template_p, _SHR_E_PARAM, "key_template_p");

    for (qual_idx = 0; qual_idx < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG; qual_idx++)
    {
        key_template_p->key_qual_map[qual_idx].qual_type = DNX_FIELD_QUAL_TYPE_INVALID;
    }

    key_template_p->key_size_in_bits = 0;
exit:
    SHR_FUNC_EXIT;
}

/** See field_key.h */
shr_error_e
dnx_field_qual_attach_info_t_init(
    int unit,
    dnx_field_qual_attach_info_t * qual_info_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(qual_info_p, _SHR_E_PARAM, "qual_info");

    qual_info_p->input_type = DNX_FIELD_INPUT_TYPE_INVALID;
    /*
     * Input argument and offset have no invalid values, just default values.
     * We use the same values as the ones initialized by BCM init function (bcm_field_qual_attach_info_t_init)
     */
    qual_info_p->input_arg = BCM_FIELD_INVALID;
    qual_info_p->offset = BCM_FIELD_INVALID;

exit:
    SHR_FUNC_EXIT;
}

/** See field_key.h */
shr_error_e
dnx_field_key_attach_info_in_t_init(
    int unit,
    dnx_field_key_attach_info_in_t * key_in_info_p)
{
    int ii = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(key_in_info_p, _SHR_E_PARAM, "key_in_info_p");

    key_in_info_p->app_db_id = DNX_FIELD_APP_DB_ID_INVALID;
    key_in_info_p->fg_type = DNX_FIELD_GROUP_INVALID;
    key_in_info_p->field_stage = DNX_FIELD_STAGE_INVALID;
    key_in_info_p->key_length = DNX_FIELD_KEY_LENGTH_TYPE_INVALID;
    for (ii = 0; ii < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG; ii++)
    {
        SHR_IF_ERR_EXIT(dnx_field_qual_attach_info_t_init(unit, &key_in_info_p->qual_info[ii]));
    }
    SHR_IF_ERR_EXIT(dnx_field_key_template_t_init(unit, &key_in_info_p->key_template));

    key_in_info_p->compare_id = DNX_FIELD_GROUP_COMPARE_ID_NONE;

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_key_attached_qual_info_t_init(
    int unit,
    dnx_field_key_attached_qual_info_t * attached_qual_info_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(attached_qual_info_p, _SHR_E_PARAM, "attached_qual_info_p");
    attached_qual_info_p->ffc_type = DNX_FIELD_FFC_TYPE_INVALID;
    attached_qual_info_p->input_type = DNX_FIELD_INPUT_TYPE_INVALID;
    attached_qual_info_p->index = BCM_FIELD_INVALID;
    attached_qual_info_p->offset = BCM_FIELD_INVALID;
    attached_qual_info_p->ranges = 0;
    attached_qual_info_p->size = 0;
    attached_qual_info_p->native_pbus = FALSE;

exit:
    SHR_FUNC_EXIT;
}
/**
* \brief
*  Allocate FFC for given Key
* \param [in] unit            - Device Id
* \param [in] field_stage     - Stage of PMF (IPMF1/IPMF2...)
* \param [in] context_id      - Context ID to configure the FFC for
* \param [in] ranges          - Available Ranges to allocate the ffc for the given qualifier
* \param [out] ffc_id    - hold the id of FFC that was allocated per qualifier
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_key_ffc_allocate(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    uint8 ranges,
    uint32 *ffc_id)
{
    int allocated_ffc_id = DNX_FIELD_FFC_ID_INVALID;

    SHR_FUNC_INIT_VARS(unit);

    switch (field_stage)
    {
        case DNX_FIELD_STAGE_IPMF1:
        {
            SHR_IF_ERR_EXIT(dnx_algo_field_key_ipmf_1_ffc_allocate(unit, 0, context_id, ranges, &allocated_ffc_id));
            break;
        }
        case DNX_FIELD_STAGE_IPMF2:
        {
            SHR_IF_ERR_EXIT(dnx_algo_field_key_ipmf_2_ffc_allocate(unit, 0, context_id, &allocated_ffc_id));
            break;
        }
        case DNX_FIELD_STAGE_IPMF3:
        {
            SHR_IF_ERR_EXIT(dnx_algo_field_key_ipmf_3_ffc_allocate(unit, 0, context_id, &allocated_ffc_id));
            break;
        }
        case DNX_FIELD_STAGE_EPMF:
        {
            SHR_IF_ERR_EXIT(dnx_algo_field_key_epmf_ffc_allocate(unit, 0, context_id, &allocated_ffc_id));
            break;
        }
        case DNX_FIELD_STAGE_EXTERNAL:
        {
            SHR_IF_ERR_EXIT(dnx_algo_field_key_ifwd2_ffc_allocate(unit, 0, context_id, &allocated_ffc_id));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal stage =%d \n", field_stage);
            break;
        }
    }

    *ffc_id = allocated_ffc_id;

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Deallocate  FFC
* \param [in] unit            - Device Id
* \param [in] field_stage     - Stage of PMF (IPMF1/IPMF2...)
* \param [in] context_id      - Context ID to deallocate the FFC for
* \param [in] ffc_id          - FFC id to deallocate
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_key_single_ffc_deallocate(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    uint32 ffc_id)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (field_stage)
    {
        case DNX_FIELD_STAGE_IPMF1:
        {
            SHR_IF_ERR_EXIT(dnx_algo_field_key_ipmf_1_ffc_deallocate(unit, context_id, ffc_id));
            break;
        }
        case DNX_FIELD_STAGE_IPMF2:
        {
            SHR_IF_ERR_EXIT(dnx_algo_field_key_ipmf_2_ffc_deallocate(unit, context_id, ffc_id));
            break;
        }
        case DNX_FIELD_STAGE_IPMF3:
        {
            SHR_IF_ERR_EXIT(dnx_algo_field_key_ipmf_3_ffc_deallocate(unit, context_id, ffc_id));
            break;
        }
        case DNX_FIELD_STAGE_EPMF:
        {
            SHR_IF_ERR_EXIT(dnx_algo_field_key_epmf_ffc_deallocate(unit, context_id, ffc_id));
            break;
        }
        case DNX_FIELD_STAGE_EXTERNAL:
        {
            SHR_IF_ERR_EXIT(dnx_algo_field_key_ifwd2_ffc_deallocate(unit, context_id, ffc_id));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal stage =%d \n", field_stage);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
*  Allocate a placeholder in the initial key for a given qualifier in IPMF1
* \param [in] unit         - Device Id
* \param [in] context_id   - Context ID to allocate the given range
* \param [in] ffc_size     - ffc size that we try to allocate
* \param [in] dnx_qual     - The encoded qualifier, only used for error messages.
* \param [out] ffc_initial_p  - info of the allocated range.
*
* \return
* \retval Zero if no error was detected
* \retval Negative if error was detected. See \ref shr_error_e
* \remark
*   None* \remark
* * None
* \see
*   * None
*/

static shr_error_e
dnx_field_key_ipmf1_initial_key_occupation_bmp_allocate(
    int unit,
    int context_id,
    dnx_field_qual_t dnx_qual,
    uint32 ffc_size,
    dnx_field_key_initial_ffc_info_t * ffc_initial_p)
{
    shr_error_e rv;
    int dest_offset = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(ffc_initial_p, _SHR_E_PARAM, "ffc_initial_p");

    /*
     * Try to allocate in Key F
     * */
    rv = dnx_algo_field_key_ipmf1_initial_key_f_occupation_bmp_allocate(unit, SW_STATE_ALGO_RES_ALLOCATE_SIMULATION,
                                                                        context_id, ffc_size, &dest_offset);
    /*
     * If successful - update the parameters
     */
    if (rv == _SHR_E_NONE)
    {
        /*
         * Succeeded to test for allocation in Key-F - now allocate it.
         */
        SHR_IF_ERR_EXIT(dnx_algo_field_key_ipmf1_initial_key_f_occupation_bmp_allocate(unit, 0,
                                                                                       context_id, ffc_size,
                                                                                       &dest_offset));
        ffc_initial_p->ffc.key_id = DBAL_ENUM_FVAL_FIELD_KEY_F;
        ffc_initial_p->key_dest_offset = dest_offset;
    }
    else        /* (rv != _SHR_E_NONE) */
    {
        /*
         * If not succeeded with F try to allocate in Key G
         * */
        rv = dnx_algo_field_key_ipmf1_initial_key_g_occupation_bmp_allocate(unit, SW_STATE_ALGO_RES_ALLOCATE_SIMULATION,
                                                                            context_id, ffc_size, &dest_offset);
        if (rv == _SHR_E_NONE)
        {
            /*
             * Succeeded to test for allocation in Key-G - now allocate it.
             */
            SHR_IF_ERR_EXIT(dnx_algo_field_key_ipmf1_initial_key_g_occupation_bmp_allocate(unit, 0,
                                                                                           context_id, ffc_size,
                                                                                           &dest_offset));
            ffc_initial_p->ffc.key_id = DBAL_ENUM_FVAL_FIELD_KEY_G;
            ffc_initial_p->key_dest_offset = dest_offset;
        }
        else    /* (rv != _SHR_E_NONE) */
        {
            /*
             * If not succeeded with F & G try to allocate in Key H
             * */
            rv = dnx_algo_field_key_ipmf1_initial_key_h_occupation_bmp_allocate(unit,
                                                                                SW_STATE_ALGO_RES_ALLOCATE_SIMULATION,
                                                                                context_id, ffc_size, &dest_offset);
            if (rv == _SHR_E_NONE)
            {
                /*
                 * Succeeded to test for allocation in Key-H - now allocate it.
                 */
                SHR_IF_ERR_EXIT(dnx_algo_field_key_ipmf1_initial_key_h_occupation_bmp_allocate(unit, 0,
                                                                                               context_id, ffc_size,
                                                                                               &dest_offset));
                ffc_initial_p->ffc.key_id = DBAL_ENUM_FVAL_FIELD_KEY_H;
                ffc_initial_p->key_dest_offset = dest_offset;
            }
            else        /* (rv != _SHR_E_NONE) */
            {
                /*
                 * If not succeeded with F & G & H try to allocate in Key I
                 * */
                rv = dnx_algo_field_key_ipmf1_initial_key_i_occupation_bmp_allocate(unit,
                                                                                    SW_STATE_ALGO_RES_ALLOCATE_SIMULATION,
                                                                                    context_id, ffc_size, &dest_offset);
                if (rv == _SHR_E_NONE)
                {
                    /*
                     * Succeeded to test for allocation in Key-I - now allocate it.
                     */
                    SHR_IF_ERR_EXIT(dnx_algo_field_key_ipmf1_initial_key_i_occupation_bmp_allocate(unit, 0,
                                                                                                   context_id, ffc_size,
                                                                                                   &dest_offset));
                    ffc_initial_p->ffc.key_id = DBAL_ENUM_FVAL_FIELD_KEY_I;
                    ffc_initial_p->key_dest_offset = dest_offset;
                }
                else    /* (rv != _SHR_E_NONE) */
                {
                    /*
                     * If not succeeded with F & G & H & I try to allocate in Key J
                     * */
                    rv = dnx_algo_field_key_ipmf1_initial_key_j_occupation_bmp_allocate(unit,
                                                                                        SW_STATE_ALGO_RES_ALLOCATE_SIMULATION,
                                                                                        context_id, ffc_size,
                                                                                        &dest_offset);
                    if (rv == _SHR_E_NONE)
                    {
                        /*
                         * Succeeded to test for allocation in Key-J - now allocate it.
                         */
                        SHR_IF_ERR_EXIT(dnx_algo_field_key_ipmf1_initial_key_j_occupation_bmp_allocate(unit, 0,
                                                                                                       context_id,
                                                                                                       ffc_size,
                                                                                                       &dest_offset));
                        ffc_initial_p->ffc.key_id = DBAL_ENUM_FVAL_FIELD_KEY_J;
                        ffc_initial_p->key_dest_offset = dest_offset;
                    }
                    else        /* (rv != _SHR_E_NONE) */
                    {
                        /*
                         * Failed to allocate initial key. Failure. Return.
                         * */
                        LOG_ERROR_EX(BSL_LOG_MODULE,
                                     "Failed to allocate initial key for qual_type %s size 0x%X field_stage ipmf1 pgm_id %d %s\n",
                                     dnx_field_dnx_qual_text(unit, dnx_qual), ffc_size, context_id, EMPTY);
                        SHR_IF_ERR_EXIT(rv);
                    }
                }
            }
        }
    }

    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "allocated initial key for qual_type %s size 0x%X field_stage ipmf1 dest_offset 0x%X key_id %d\n",
                 dnx_field_dnx_qual_text(unit, dnx_qual), ffc_size, ffc_initial_p->key_dest_offset,
                 ffc_initial_p->ffc.key_id);

exit:
    SHR_FUNC_EXIT;
}

/**
*  De-Allocate the placeholder in the initial key for a given qualifier in IPMF1
* \param [in] unit              - Device Id
* \param [in] context_id        - Context ID to deallocate the given ffc
* \param [in] ffc_initial_p     - info of the initial ffc.
*
* \return
* \retval Zero if no error was detected
* \retval Negative if error was detected. See \ref shr_error_e
* \remark
*   None* \remark
* * None
* \see
*   * None
*/

static shr_error_e
dnx_field_key_ipmf1_initial_key_occupation_bmp_deallocate(
    int unit,
    int context_id,
    dnx_field_key_initial_ffc_info_t * ffc_initial_p)
{
    int ffc_size;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(ffc_initial_p, _SHR_E_PARAM, "ffc_initial_p");

    /*
     * The size inside the ffc_instruction stored as 5'b, thus the 0 value in the size means ffc_size of 1 bit, and a
     * instruction size value of 0x1f (31'b) means the ffc size is 32. Thus, add (+ 1) to the instruction size. 
     */
    ffc_size = ffc_initial_p->ffc.ffc_instruction.size + 1;

    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "de-allocate initial placeholder for context_id %d key_id %d size 0x%X field_stage ipmf1 dest_offset 0x%X\n",
                 context_id, ffc_initial_p->ffc.key_id, ffc_size, ffc_initial_p->key_dest_offset);

    switch (ffc_initial_p->ffc.key_id)
    {
        case DBAL_ENUM_FVAL_FIELD_KEY_F:
        {
            SHR_IF_ERR_EXIT(dnx_algo_field_key_ipmf1_initial_key_f_occupation_bmp_deallocate
                            (unit, context_id, ffc_size, ffc_initial_p->key_dest_offset));
            break;
        }
        case DBAL_ENUM_FVAL_FIELD_KEY_G:
        {
            SHR_IF_ERR_EXIT(dnx_algo_field_key_ipmf1_initial_key_g_occupation_bmp_deallocate
                            (unit, context_id, ffc_size, ffc_initial_p->key_dest_offset));
            break;
        }
        case DBAL_ENUM_FVAL_FIELD_KEY_H:
        {
            SHR_IF_ERR_EXIT(dnx_algo_field_key_ipmf1_initial_key_h_occupation_bmp_deallocate
                            (unit, context_id, ffc_size, ffc_initial_p->key_dest_offset));
            break;
        }
        case DBAL_ENUM_FVAL_FIELD_KEY_I:
        {
            SHR_IF_ERR_EXIT(dnx_algo_field_key_ipmf1_initial_key_i_occupation_bmp_deallocate
                            (unit, context_id, ffc_size, ffc_initial_p->key_dest_offset));
            break;
        }
        case DBAL_ENUM_FVAL_FIELD_KEY_J:
        {
            SHR_IF_ERR_EXIT(dnx_algo_field_key_ipmf1_initial_key_j_occupation_bmp_deallocate
                            (unit, context_id, ffc_size, ffc_initial_p->key_dest_offset));
            break;
        }
        default:
        {
            LOG_ERROR_EX(BSL_LOG_MODULE,
                         "Fail to de-allocate initial placeholder for  context_id %d key_id %d size 0x%X field_stage ipmf1 dest_offset 0x%X \n",
                         context_id, ffc_initial_p->ffc.key_id, ffc_size, ffc_initial_p->key_dest_offset);
            SHR_IF_ERR_EXIT(_SHR_E_BADID);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_key_qual_initial_key_to_meta2_qual_get(
    int unit,
    dbal_enum_value_field_field_key_e initial_key_id,
    dnx_field_ipmf2_qual_e * qual_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(qual_id, _SHR_E_PARAM, "qual_id");

    switch (initial_key_id)
    {
        case DBAL_ENUM_FVAL_FIELD_KEY_F:
        {
            *qual_id = DNX_FIELD_IPMF2_QUAL_KEY_F_INITIAL;
            break;
        }
        case DBAL_ENUM_FVAL_FIELD_KEY_G:
        {
            *qual_id = DNX_FIELD_IPMF2_QUAL_KEY_G_INITIAL;
            break;
        }
        case DBAL_ENUM_FVAL_FIELD_KEY_H:
        {
            *qual_id = DNX_FIELD_IPMF2_QUAL_KEY_H_INITIAL;
            break;
        }
        case DBAL_ENUM_FVAL_FIELD_KEY_I:
        {
            *qual_id = DNX_FIELD_IPMF2_QUAL_KEY_I_INITIAL;
            break;
        }
        case DBAL_ENUM_FVAL_FIELD_KEY_J:
        {
            *qual_id = DNX_FIELD_IPMF2_QUAL_KEY_J_INITIAL;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal  id =%d \n", initial_key_id);
            break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief
* This function will get the qualifier for the action, for the input type CASCADED
* (meaning our qualifier takes the action buffer as input) and return the appropriate
* dnx qualifier name. *
* in case the qual size is bigger than DNX_DATA_MAX_FIELD_TCAM_ACTION_SIZE_SINGLE we
* expect to get the double action (as TCAM_0_1) as input qualifier. The data between
* the action buffers is continuous so there should be no special handling for the ffcs.
* \param [in] unit              - Device Id
* \param [in] qual_action_id    - DBAL Action qualifier
* \param [out] dnx_qual_p       - Qualifier for the given payload.
*
* \return
* \retval Zero if no error was detected
* \retval Negative if error was detected. See \ref shr_error_e
* \remark
*/
static shr_error_e
dnx_field_key_qual_action_to_meta2_qual_get(
    int unit,
    dbal_enum_value_field_field_io_e qual_action_id,
    dnx_field_qual_t * dnx_qual_p)
{
    dnx_field_qual_id_t qual_id;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(dnx_qual_p, _SHR_E_PARAM, "dnx_qual_p");

    switch (qual_action_id)
    {
        case DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_0:
        {
            qual_id = DNX_FIELD_IPMF2_QUAL_PMF1_TCAM_ACTION_0;
            break;
        }
        case DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_1:
        {
            qual_id = DNX_FIELD_IPMF2_QUAL_PMF1_TCAM_ACTION_1;
            break;
        }
        case DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_2:
        {
            qual_id = DNX_FIELD_IPMF2_QUAL_PMF1_TCAM_ACTION_2;
            break;
        }
        case DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_3:
        {
            qual_id = DNX_FIELD_IPMF2_QUAL_PMF1_TCAM_ACTION_3;
            break;
        }
        case DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_0_1:
        {
            qual_id = DNX_FIELD_IPMF2_QUAL_PMF1_TCAM_ACTION_0;
            break;
        }
        case DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_1_2:
        {
            qual_id = DNX_FIELD_IPMF2_QUAL_PMF1_TCAM_ACTION_1;
            break;
        }
        case DBAL_ENUM_FVAL_FIELD_IO_INTERNAL_TCAM_2_3:
        {
            qual_id = DNX_FIELD_IPMF2_QUAL_PMF1_TCAM_ACTION_2;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Id %d Not supported\n", qual_action_id);
            break;
        }
    }

    *dnx_qual_p = DNX_QUAL(DNX_FIELD_QUAL_CLASS_META2, DNX_FIELD_STAGE_IPMF2, qual_id);

exit:
    SHR_FUNC_EXIT;
}

static void
dnx_field_key_ffc_instruction_info_init(
    dnx_field_key_ffc_instruction_info_t * ffc_instruction_p)
{
    ffc_instruction_p->ffc_type = DBAL_FIELD_EMPTY;
    ffc_instruction_p->offset = 0;
    ffc_instruction_p->field_index = 0;
    ffc_instruction_p->key_offset = 0;
    ffc_instruction_p->size = 0;
    ffc_instruction_p->full_instruction = 0;
}

static void
dnx_field_key_qualifier_ffc_info_init(
    dnx_field_key_qualifier_ffc_info_t * qualifier_ffc_info_p,
    uint32 num_of_elements)
{
    uint32 qual_idx, jj;

    for (qual_idx = 0; qual_idx < num_of_elements; qual_idx++)
    {
        for (jj = 0; jj < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC_IN_QUAL; jj++)
        {
            qualifier_ffc_info_p[qual_idx].qual_type = DNX_FIELD_QUAL_TYPE_INVALID;
            qualifier_ffc_info_p[qual_idx].ffc_info[jj].ffc.ffc_id = DNX_FIELD_FFC_ID_INVALID;
            qualifier_ffc_info_p[qual_idx].ffc_info[jj].ffc_initial.ffc.ffc_id = DNX_FIELD_FFC_ID_INVALID;
            qualifier_ffc_info_p[qual_idx].ffc_info[jj].ffc_initial.key_dest_offset = 0;
            qualifier_ffc_info_p[qual_idx].ffc_info[jj].ffc_initial.ffc.key_id = 0;
            dnx_field_key_ffc_instruction_info_init(&(qualifier_ffc_info_p[qual_idx].ffc_info[jj].ffc.ffc_instruction));
            dnx_field_key_ffc_instruction_info_init(&
                                                    (qualifier_ffc_info_p[qual_idx].ffc_info[jj].ffc_initial.
                                                     ffc.ffc_instruction));
        }
    }
}

/**
* \brief
*  Allocate FFC for given qualifier
* \param [in] unit            - Device Id
* \param [in] field_stage     - Stage of PMF (IPMF1/IPMF2...)
* \param [in] context_id      - Context ID to configure the qualifier for
* \param [in] qual_size       -
*   qualifier size - we allocate appropriate number of ffc according to that size
* \param [in] ranges          - A bitmap representing the Ranges of FFCs we can allocate for the qualifier in iPMF1.
* \param [out] qualifier_ffc_info_p    - to be loaded with the FFCs (including initial FFCs for non native iPMF2 
*                                        qualifiers) that would be allocated.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_key_ffc_allocate_per_qualifier(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    int qual_size,
    uint8 ranges,
    dnx_field_key_qualifier_ffc_info_t * qualifier_ffc_info_p)
{
    uint32 ffc_idx = 0;
    int size = qual_size;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(qualifier_ffc_info_p, _SHR_E_PARAM, "qualifier_ffc_info_p");

    /*
     * Allocate the appropriate number of ffc per given size of qualifier
     * The maximal size of a ffc is 32b. In case the qualifier is bigger
     * than 32b, we will need to allocate more than one ffc to handle this qualifier.
     * Every loop subtracts 32b (DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_BITS_IN_FFC) from
     * the given qualifier size.
     */
    for (ffc_idx = 0; ffc_idx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC_IN_QUAL && (size > 0);
         ffc_idx++, size -= DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_BITS_IN_FFC)
    {
        SHR_IF_ERR_EXIT_WITH_LOG(dnx_field_key_ffc_allocate(unit, field_stage, context_id, ranges,
                                                            &(qualifier_ffc_info_p->ffc_info[ffc_idx].ffc.ffc_id)),
                                 "Failed to allocate FFC %d for qualifier \"%s\" for context %d.",
                                 ffc_idx, dnx_field_dnx_qual_text(unit, qualifier_ffc_info_p->qual_type), context_id);
        LOG_DEBUG_EX(BSL_LOG_MODULE, "ffc_id[%d] %d, qual size %d, context_id %d\n", ffc_idx,
                     qualifier_ffc_info_p->ffc_info[ffc_idx].ffc.ffc_id, qual_size, context_id);

    }
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Allocate the FFC's for the IPMF1 (to serve initial qualifier)
* \param [in] unit            - Device Id
* \param [in] context_id      - Context ID to configure the key for
*   qualifier size - we allocate appropriate number of ffc according to that size
* \param [in] qual_ffc_ranges - The FFC ranges available for this qualifier
* \param [out] ffc_id_p       - Array of FFC's

* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_key_allocate_initial_ffc(
    int unit,
    int context_id,
    uint8 qual_ffc_ranges,
    uint32 *ffc_id_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(ffc_id_p, _SHR_E_PARAM, "ffc_id_p");

    SHR_IF_ERR_EXIT(dnx_field_key_ffc_allocate(unit, DNX_FIELD_STAGE_IPMF1, context_id, qual_ffc_ranges, ffc_id_p));
    LOG_DEBUG_EX(BSL_LOG_MODULE, "ffc_id %d, context_id %d, qual_ffc_ranges %d %s\n", *ffc_id_p, context_id,
                 qual_ffc_ranges, EMPTY);

exit:
    SHR_FUNC_EXIT;

}

/**
* \brief
*  In case we currently process a qualifier that requires allocation and configuration of resources in IPMF1 -
*  Allocate the FFC's for the given qualifier.
*  This function allocates initial ffc's explicitly. All the handling is done for IPMF1.
* \param [in] unit            - Device Id
* \param [in] context_id      - Context ID to configure the key for
* \param [in] dnx_qual        - Encoded qualifier - This is the qualifier that should be allocated
*                               as initial qual in ipmf1
* \param [in] dnx_qual_info_p   - Additional information for header qualifiers,
*                             to know from which layer the qualifier should be taken
* \param [out] qualifier_ffc_info_p    - Qualifier full info
*   This array holds the id's and additional data of FFC that were allocated per qualifier. Number of elements
*   on this array is assumed to be DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC_IN_QUAL.
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_key_ffc_allocate_per_initial_qualifier(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_qual_t dnx_qual,
    dnx_field_key_attached_qual_info_t * dnx_qual_info_p,
    dnx_field_key_qualifier_ffc_info_t * qualifier_ffc_info_p)
{
    dnx_field_qual_class_e qual_class = DNX_QUAL_CLASS(dnx_qual);
    int qual_size = dnx_qual_info_p->size;
    int ffc_idx;
    int ffc_size = 0;
    dnx_field_context_t cascaded_from_context_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(dnx_qual_info_p, _SHR_E_PARAM, "dnx_qual_info_p");
    SHR_NULL_CHECK(qualifier_ffc_info_p, _SHR_E_PARAM, "qualifier_ffc_info_p");

    /*
     * Get the cascaded_from_context_id - the context id in ipmf1 
     */

    SHR_IF_ERR_EXIT(dnx_field_context_cascaded_from_context_id_get
                    (unit, DNX_FIELD_STAGE_IPMF2, context_id, &cascaded_from_context_id));

    LOG_DEBUG_EX(BSL_LOG_MODULE, " context_id %d cascaded_from_context_id %d %s%s\r\n", context_id,
                 cascaded_from_context_id, EMPTY, EMPTY);

    /*
     * If the qualifier is in ipmf2 and it is not meta2 then we need to allocate ffc and key in ipmf1 to handle it
     * properly.
     */
    /*
     * Allocate the appropriate number of ffc per given size of qualifier
     * The maximal size of a ffc is 32b. In case the qualifier is bigger
     * than 32b, we will need to allocate more than one ffc to handle this qualifier.
     * Every loop subtracts 32b (DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_BITS_IN_FFC) from
     * the given qualifier size.
     */
    for (ffc_idx = 0; ffc_idx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC_IN_QUAL && (qual_size > 0);
         ffc_idx++, qual_size -= DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_BITS_IN_FFC)
    {
        ffc_size = ((qual_size > DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_BITS_IN_FFC) ?
                    DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_BITS_IN_FFC : qual_size);
        /*
         * Allocate a placeholder in initial key for current ffc
         */
        SHR_IF_ERR_EXIT_WITH_LOG(dnx_field_key_ipmf1_initial_key_occupation_bmp_allocate(unit,
                                                                                         cascaded_from_context_id,
                                                                                         dnx_qual, ffc_size,
                                                                                         &
                                                                                         (qualifier_ffc_info_p->ffc_info
                                                                                          [ffc_idx].ffc_initial)),
                                 "Failed to allocate place in initial key for FFC %d for qualifier \"%s\""
                                 "for context %d.", ffc_idx, dnx_field_dnx_qual_text(unit,
                                                                                     qualifier_ffc_info_p->qual_type),
                                 context_id);

        SHR_IF_ERR_EXIT_WITH_LOG(dnx_field_key_allocate_initial_ffc(unit, cascaded_from_context_id,
                                                                    dnx_qual_info_p->ranges,
                                                                    &(qualifier_ffc_info_p->
                                                                      ffc_info[ffc_idx].ffc_initial.ffc.ffc_id)),
                                 "Failed to allocate FFC %d in initial key for qualifier \"%s\" for context %d.",
                                 ffc_idx, dnx_field_dnx_qual_text(unit, qualifier_ffc_info_p->qual_type), context_id);

        LOG_DEBUG_EX(BSL_LOG_MODULE,
                     "allocated initial ffc&key for qual_type %s key_id %d qual_size %d field_stage ipmf1 dest_offset %d\n",
                     dnx_field_dnx_qual_text(unit, dnx_qual),
                     qualifier_ffc_info_p->ffc_info[ffc_idx].ffc_initial.ffc.key_id,
                     qual_size, qualifier_ffc_info_p->ffc_info[ffc_idx].ffc_initial.ffc.ffc_instruction.key_offset);
    }
    LOG_DEBUG_EX(BSL_LOG_MODULE, "initial_key qual_type 0x%x class %d size %d cascaded_from_context_id %d\n",
                 dnx_qual, qual_class, qual_size, cascaded_from_context_id);
exit:
    SHR_FUNC_EXIT;
}
/**
* \brief
*  Choose the correct table for KBR HW set based on STAGE
* \param [in] unit          - Device ID
* \param [in] field_stage   - Field Stage (IPMF1,IPMF3...)
* \param [in] key_id        - Key id
* \param [out] table_name_p  - DBAL Table name to configure KBR (key Id with which FFC's)
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_key_kbr_hw_table_name_get(
    int unit,
    dnx_field_stage_e field_stage,
    dbal_enum_value_field_field_key_e key_id,
    dbal_tables_e * table_name_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(table_name_p, _SHR_E_PARAM, "table_name_p");

    switch (field_stage)
    {
        case DNX_FIELD_STAGE_IPMF1:
        {
            /*
             *  In KLEAP implementation of IPMF1 KBR construction we
             *  divided KBRs to two tables: DBAL_TABLE_KLEAP_IPMF1_KBR_INFO, where we are
             *  constructing first 5 keys("A" to "E") and we should update DBAL_FIELD_FIELD_KEY,
             *  second table is DBAL_TABLE_KLEAP_IPMF1_INITIAL_KBR_INFO,
             *  where we are constructing second five keys for IPMF1 for keys "F" to "J"
             */
            if (key_id <= DBAL_ENUM_FVAL_FIELD_KEY_E)
            {
                *table_name_p = DBAL_TABLE_KLEAP_IPMF1_KBR_INFO;
            }
            else
            {
                *table_name_p = DBAL_TABLE_KLEAP_IPMF1_INITIAL_KBR_INFO;
            }
            break;
        }
        case DNX_FIELD_STAGE_IPMF2:
        {
            *table_name_p = DBAL_TABLE_KLEAP_IPMF2_KBR_INFO;
            break;
        }
        case DNX_FIELD_STAGE_IPMF3:
        {
            *table_name_p = DBAL_TABLE_KLEAP_IPMF3_KBR_INFO;
            break;
        }
        case DNX_FIELD_STAGE_EPMF:
        {
            *table_name_p = DBAL_TABLE_KLEAP_E_PMF_KBR_INFO;
            break;
        }
        case DNX_FIELD_STAGE_EXTERNAL:
        {
            *table_name_p = DBAL_TABLE_KLEAP_FWD12_KBR_INFO;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Illegal field stage %d \n", field_stage);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Converts given ffc_arr[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC] to a bitmap in a way that each element's value in the array
*  gets converted to a set bit inside the bitmap
* \param [in] unit - Device ID
* \param [in] ffc_arr - The ffc array to convert
* \param [out] ffc_bitmap - The converted ffc bitmap
* \return
*   shr_error_e             - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_key_ffc_arr_2_bmp(
    int unit,
    uint32 *ffc_arr,
    uint32 *ffc_bitmap)
{
    int ffc_id;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(ffc_arr, _SHR_E_PARAM, "ffc_arr");
    SHR_NULL_CHECK(ffc_bitmap, _SHR_E_PARAM, "ffc_bitmap");

    for (ffc_id = 0;
         ffc_id < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC && ffc_arr[ffc_id] != DNX_FIELD_FFC_ID_INVALID; ffc_id++)
    {
        DNX_FIELD_ARR32_SET_BIT(ffc_bitmap, (ffc_arr[ffc_id]));
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Converts given ffc_bitmap to an array of indexes in a way that each set bit index converts
*  to an element of value index in the array
* \param [in] unit - Device ID
* \param [in] ffc_bitmap - The ffc bitmap to convert
* \param [out] ffc_arr - The converted array
* \return
*   shr_error_e             - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_key_ffc_bmp_2_arr(
    int unit,
    uint32 *ffc_bitmap,
    uint32 *ffc_arr)
{
    int ffc_id;
    int ffc_count = 0;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(ffc_bitmap, _SHR_E_PARAM, "ffc_bitmap");
    SHR_NULL_CHECK(ffc_arr, _SHR_E_PARAM, "ffc_arr");

    for (ffc_id = 0; ffc_id < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC; ffc_id++)
    {
        if (DNX_FIELD_ARR32_IS_BIT_SET(ffc_bitmap, ffc_id))
        {
            ffc_arr[ffc_count++] = ffc_id;
        }
    }

    /** Terminate array - Fill the rest of the array with DNX_FIELD_FFC_ID_INVALID */
    for (; ffc_count < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC; ffc_count++)
    {
        ffc_arr[ffc_count] = DNX_FIELD_FFC_ID_INVALID;
    }
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Set the FFC id's bitmap, i.e. set which FFC's will construct the key
* \param [in] unit          -  Device ID
* \param [in] field_stage   -  For which Field stage to Configure HW
* \param [in] context_id    - Context ID
* \param [in] key_id        - Key id to set
* \param [in] ffc_id   - Array FFC ID's to configured in KBR for specific key
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_key_kbr_ffc_array_hw_set(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dbal_enum_value_field_field_key_e key_id,
    uint32 ffc_id[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC])
{
    uint32 entry_handle_id;
    dbal_tables_e table_name;
    dbal_fields_e dbal_ctx_id;
    dbal_fields_e dbal_key;
    uint32 ffc_alloc_bmp[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC_IN_UINT32] = { 0, 0 };
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(ffc_id, _SHR_E_PARAM, "ffc_id");
    /** DBAL_NOF_TABLES as invalid value*/
    table_name = DBAL_NOF_TABLES;

    SHR_IF_ERR_EXIT(dnx_field_key_ffc_arr_2_bmp(unit, ffc_id, ffc_alloc_bmp));

    LOG_DEBUG_EX(BSL_LOG_MODULE, "Set  ffc bitmap context_id %d, key_id %d ffc_alloc_bmp 0x%X%X\n",
                 context_id, key_id, ffc_alloc_bmp[1], ffc_alloc_bmp[0]);

    SHR_IF_ERR_EXIT(dnx_field_key_kbr_hw_table_name_get(unit, field_stage, key_id, &table_name));
    SHR_IF_ERR_EXIT(dnx_field_map_ctx_id_to_field(unit, field_stage, &dbal_ctx_id));
    SHR_IF_ERR_EXIT(dnx_field_map_key_to_field(unit, field_stage, &dbal_key));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_name, &entry_handle_id));

    /** key construction*/
    dbal_entry_key_field32_set(unit, entry_handle_id, dbal_ctx_id, context_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, dbal_key, key_id);

    /** Set the ffc id's bitmap*/
    dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_FFC_BITMAP, INST_SINGLE, ffc_alloc_bmp);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** See field_key.h */
shr_error_e
dnx_field_key_kbr_ffc_array_hw_get(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dbal_enum_value_field_field_key_e key_id,
    uint32 ffc_id[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC])
{
    uint32 entry_handle_id;
    dbal_tables_e table_name;
    dbal_fields_e dbal_ctx_id;
    dbal_fields_e dbal_key;
    uint32 ffc_alloc_bmp[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC_IN_UINT32] = { 0, 0 };
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(ffc_id, _SHR_E_PARAM, "ffc_id");
    /** DBAL_NOF_TABLES as invalid value*/
    table_name = DBAL_NOF_TABLES;

    SHR_IF_ERR_EXIT(dnx_field_key_kbr_hw_table_name_get(unit, field_stage, key_id, &table_name));
    SHR_IF_ERR_EXIT(dnx_field_map_ctx_id_to_field(unit, field_stage, &dbal_ctx_id));
    SHR_IF_ERR_EXIT(dnx_field_map_key_to_field(unit, field_stage, &dbal_key));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_name, &entry_handle_id));

    /** key construction*/
    dbal_entry_key_field32_set(unit, entry_handle_id, dbal_ctx_id, context_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, dbal_key, key_id);

    /** Set the ffc id's bitmap*/
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_FFC_BITMAP, INST_SINGLE, ffc_alloc_bmp);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    LOG_DEBUG_EX(BSL_LOG_MODULE, "Read ffc bitmap context_id %d, key_id %d ffc_alloc_bmp 0x%X%X\n",
                 context_id, key_id, ffc_alloc_bmp[1], ffc_alloc_bmp[0]);

    SHR_IF_ERR_EXIT(dnx_field_key_ffc_bmp_2_arr(unit, ffc_alloc_bmp, ffc_id));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Get the FFC id's bitmap, add the given single ffc, and save the new KBR
* \param [in] unit          -  Device ID
* \param [in] field_stage   -  Field Stage
* \param [in] context_id    -  Context ID
* \param [in] key_id        -  Key id of which FFC needs to be read
* \param [in] ffc_id_new    -  FFC ID's that should be added to the KBR HW for specific key
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_key_kbr_single_ffc_hw_add(
    int unit,
    dnx_field_stage_e field_stage,
    int context_id,
    dbal_enum_value_field_field_key_e key_id,
    uint32 ffc_id_new)
{
    uint32 ffc_id[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC];
    uint32 ffc_index = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_key_kbr_ffc_array_hw_get(unit, field_stage, context_id, key_id, ffc_id));

    /*
     * Cycle for the end of allocated ffc's
     */
    for (ffc_index = 0;
         (ffc_index < (DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC - 1))
         && (ffc_id[ffc_index] != DNX_FIELD_FFC_ID_INVALID); ffc_index++)
    {
        /*
         * Break if we got to invalid - this is an empty spot
         */
    }
    /*
     * If the ffc_id[] has free place in the array for the new ffc (only if ffc_index is less than 64-1 (63)
     * */
    if (ffc_id[ffc_index] == DNX_FIELD_FFC_ID_INVALID)
    {
        /*
         * Add the new ffc to the already existing array of allocated ffc's. Add on free spot at the end of the list
         */
        ffc_id[ffc_index] = ffc_id_new;
        SHR_IF_ERR_EXIT(dnx_field_key_kbr_ffc_array_hw_set(unit, field_stage, context_id, key_id, ffc_id));
    }
    else
        SHR_ERR_EXIT(_SHR_E_FULL, "FFC array is full ");
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Get the FFC id's bitmap, remove the given single ffc, and save the new KBR
* \param [in] unit          -  Device ID
* \param [in] field_stage   -  Field Stage
* \param [in] context_id    -  Context ID
* \param [in] key_id        -  Key id of which FFC should be removed
* \param [in] ffc_id_remove -  FFC ID's that should be removed from the KBR HW for specific key
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_key_kbr_single_ffc_hw_remove(
    int unit,
    dnx_field_stage_e field_stage,
    int context_id,
    dbal_enum_value_field_field_key_e key_id,
    uint32 ffc_id_remove)
{
    uint32 ffc_id[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC];
    uint32 ffc_index = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_key_kbr_ffc_array_hw_get(unit, field_stage, context_id, key_id, ffc_id));

    /*
     * Cycle through all allocated ffc's, until we find the requested ffc
     */
    for (ffc_index = 0;
         (ffc_index < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC) && (ffc_id[ffc_index] != ffc_id_remove); ffc_index++);

    /*
     * If we have reached the last element in the array - the requested ffc_id_remove wasn't found. 
     */
    if (ffc_index == DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "ffc id %d wasn't found in key %d\n", ffc_id_remove, key_id);
    }
    /*
     * Shift the rest of the array while overriding the ffc_id_remove
     * */
    for (; ffc_index < (DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC - 1)
         && (ffc_id[ffc_index] != DNX_FIELD_FFC_ID_INVALID); ffc_index++)
    {
        /*
         * shift the remaining array of ffc's, while skipping the ffc_id_remove
         */
        ffc_id[ffc_index] = ffc_id[ffc_index + 1];
    }
    /*
     * Terminate the array: Set the last element in the array to INVALID value;
     * Works also in case the requested ffc_id_remove is the last element in the array.
     * */
    ffc_id[ffc_index] = DNX_FIELD_FFC_ID_INVALID;

    SHR_IF_ERR_EXIT(dnx_field_key_kbr_ffc_array_hw_set(unit, field_stage, context_id, key_id, ffc_id));

exit:
    SHR_FUNC_EXIT;
}
/**
* \brief
*  Get the FFC id's bitmap, add the given ffc's, and save the new KBR
* \param [in] unit          -  Device ID
* \param [in] field_stage   -  Field Stage
* \param [in] context_id    -  Context ID in pmf2, cascaded from another context in the ipmf1
* \param [in] qualifier_ffc_info_p  - qualifier ffc info, required for extraction of the initial ffc info
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_key_kbr_qualifier_initial_ffc_hw_add(
    int unit,
    dnx_field_stage_e field_stage,
    int context_id,
    dnx_field_key_qualifier_ffc_info_t * qualifier_ffc_info_p)
{
    uint32 ffc_index = 0;
    dnx_field_context_t cascaded_from_context_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(qualifier_ffc_info_p, _SHR_E_PARAM, "qualifier_ffc_info_p");

    cascaded_from_context_id = context_id;
    SHR_IF_ERR_EXIT(dnx_field_context_cascaded_from_context_id_get
                    (unit, DNX_FIELD_STAGE_IPMF2, context_id, &cascaded_from_context_id));

    LOG_DEBUG_EX(BSL_LOG_MODULE, " context_id %d cascaded_from_context_id %d %s%s\r\n", context_id,
                 cascaded_from_context_id, EMPTY, EMPTY);

    for (ffc_index = 0; (ffc_index < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC_IN_QUAL)
         && (qualifier_ffc_info_p->ffc_info[ffc_index].ffc_initial.ffc.ffc_id != DNX_FIELD_FFC_ID_INVALID); ffc_index++)
    {
        SHR_IF_ERR_EXIT(dnx_field_key_kbr_single_ffc_hw_add(unit, DNX_FIELD_STAGE_IPMF1, cascaded_from_context_id,
                                                            qualifier_ffc_info_p->ffc_info[ffc_index].ffc_initial.
                                                            ffc.key_id,
                                                            qualifier_ffc_info_p->ffc_info[ffc_index].ffc_initial.
                                                            ffc.ffc_id));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Get the FFC id's bitmap, add the given ffc's, and save the new KBR
* \param [in] unit          -  Device ID
* \param [in] field_stage   -  Field Stage
* \param [in] context_id    - Context ID
* \param [in] key_id        - Key id of which FFC needs to be read
* \param [in] qualifier_ffc_info_p    - Array of FFC info that should be added to the KBR HW for specific key
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_key_kbr_qualifier_ffc_hw_add(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dbal_enum_value_field_field_key_e key_id,
    dnx_field_key_qualifier_ffc_info_t * qualifier_ffc_info_p)
{
    uint32 ffc_index = 0;
    dnx_field_qual_class_e qual_class = DNX_FIELD_QUAL_CLASS_INVALID;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(qualifier_ffc_info_p, _SHR_E_PARAM, "qualifier_ffc_info_p");

    for (ffc_index = 0;
         (ffc_index < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC_IN_QUAL)
         && (qualifier_ffc_info_p->ffc_info[ffc_index].ffc.ffc_id != DNX_FIELD_FFC_ID_INVALID); ffc_index++)
    {
        SHR_IF_ERR_EXIT(dnx_field_key_kbr_single_ffc_hw_add
                        (unit, field_stage, context_id, key_id, qualifier_ffc_info_p->ffc_info[ffc_index].ffc.ffc_id));
    }

    /**
      * Not empty ffc list means that there are initial qualifiers and ffc's were allocated for them
      */
    qual_class = DNX_QUAL_CLASS(qualifier_ffc_info_p->qual_type);
    if ((qual_class != DNX_FIELD_QUAL_CLASS_META2) && (field_stage == DNX_FIELD_STAGE_IPMF2))
    {
        SHR_IF_ERR_EXIT(dnx_field_key_kbr_qualifier_initial_ffc_hw_add(unit, DNX_FIELD_STAGE_IPMF1, context_id,
                                                                       qualifier_ffc_info_p));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Remove the FFC id's from the KBR.
*  In case this is a SINGLE or DOUBLE key - the function will clean the KBR for the given key.
*  In case this is a half key (80b') - the function will remove only the ffc's relevant for
*  the given key, but will not touch the rest of the ffc's.
* \param [in] unit          - Device ID
* \param [in] field_stage   - For which Field stage to Configure HW
* \param [in] context_id    - Context ID
* \param [in] group_ffc_info_p - Array FFC ID's to delete from KBR for specific key
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_key_kbr_delete(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_key_group_ffc_info_t * group_ffc_info_p)
{
    uint32 qual_idx;
    uint32 ffc_idx = DNX_FIELD_FFC_ID_INVALID;
    dnx_field_context_t cascaded_from_context_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(group_ffc_info_p, _SHR_E_PARAM, "group_ffc_info_p");

    if (field_stage == DNX_FIELD_STAGE_IPMF2)
    {
        SHR_IF_ERR_EXIT(dnx_field_context_cascaded_from_context_id_get
                        (unit, field_stage, context_id, &cascaded_from_context_id));

        LOG_DEBUG_EX(BSL_LOG_MODULE, " context_id %d cascaded_from_context_id %d %s%s\r\n", context_id,
                     cascaded_from_context_id, EMPTY, EMPTY);
    }
    else
    {
        cascaded_from_context_id = context_id;
    }
    /*
     * Handle the Double key.
     * */
    for (qual_idx = 0; (qual_idx < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG); qual_idx++)
    {
        /*
         * go through the list of ffc's for the given qualifier and free the valid ffc's
         */
        for (ffc_idx = 0; (ffc_idx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC_IN_QUAL) &&
             (group_ffc_info_p->key.qualifier_ffc_info[qual_idx].ffc_info[ffc_idx].ffc.ffc_id !=
              DNX_FIELD_FFC_ID_INVALID); ffc_idx++)
        {
            SHR_IF_ERR_EXIT(dnx_field_key_kbr_single_ffc_hw_remove
                            (unit, field_stage, context_id,
                             group_ffc_info_p->key.qualifier_ffc_info[qual_idx].ffc_info[ffc_idx].ffc.key_id,
                             group_ffc_info_p->key.qualifier_ffc_info[qual_idx].ffc_info[ffc_idx].ffc.ffc_id));

            /*
             * Free the initial ffc's from the cascaded_from_context_id context 
             */
            if (group_ffc_info_p->key.qualifier_ffc_info[qual_idx].ffc_info[ffc_idx].ffc_initial.ffc.ffc_id !=
                DNX_FIELD_FFC_ID_INVALID)
            {
                SHR_IF_ERR_EXIT(dnx_field_key_kbr_single_ffc_hw_remove
                                (unit, DNX_FIELD_STAGE_IPMF1, cascaded_from_context_id,
                                 group_ffc_info_p->key.qualifier_ffc_info[qual_idx].ffc_info[ffc_idx].ffc_initial.
                                 ffc.key_id,
                                 group_ffc_info_p->key.qualifier_ffc_info[qual_idx].ffc_info[ffc_idx].ffc_initial.
                                 ffc.ffc_id));
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Set the app_db_id for specific key_id
* Defines the profile for lookup for the key in input
* \param [in] unit          -  Device ID
* \param [in] field_stage   -  For which Field stage to Configure HW
* \param [in] context_id    - Context ID
* \param [in] key_id        - Key id to set
* \param [in] app_db_id     - This value indicate how the key used by HW (TCAM/ MSB ...) which banks to look etc..
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_key_app_db_id_hw_set(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dbal_enum_value_field_field_key_e key_id,
    int app_db_id)
{
    uint32 entry_handle_id;
    dbal_tables_e table_name;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /** DBAL_NOF_TABLES as invalid value*/
    table_name = DBAL_NOF_TABLES;

    SHR_IF_ERR_EXIT(dnx_field_key_kbr_hw_table_name_get(unit, field_stage, key_id, &table_name));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_name, &entry_handle_id));

    /** key construction*/
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_PMF_CTX_ID, context_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_KEY, key_id);

    /** Set the app_db_id*/
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_APP_DB_ID, INST_SINGLE, app_db_id);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Set the app_db_id for general key (80/160/320'b), if applicable.
* Defines the profile for lookup for the key in input
* \param [in] unit          -  Device ID
* \param [in] field_stage   -  For which Field stage to Configure HW
* \param [in] context_id    - Context ID
* \param [in] key_id_p      - Structure that holds an array of Key Ids.
* \param [in] app_db_id     - This value indicate how the key used by HW (TCAM/ MSB ...) which banks to look etc..
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_key_app_db_id_set(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_key_id_t *key_id_p,
    int app_db_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(key_id_p, _SHR_E_PARAM, "key_id_p");
    if (key_id_p->id[0] == DNX_FIELD_KEY_ID_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Key is invalid");
    }
    SHR_IF_ERR_EXIT(dnx_field_key_app_db_id_hw_set(unit, field_stage, context_id, key_id_p->id[0], app_db_id));

    if (key_id_p->id[1] != DNX_FIELD_KEY_ID_INVALID)
    {
        /*
         * No two keys are allowed to use the same app_db_id, therefore, we set a different app_db_id for
         * each key in a double key pair. In JER2, we have TCAM double keys mode only, therefore we use
         * the paired ID supplied by the TCAM access profile to determine the app_db_id to use for the
         * second key.
         */
        SHR_IF_ERR_EXIT(dnx_field_key_app_db_id_hw_set
                        (unit, field_stage, context_id, key_id_p->id[1],
                         DNX_FIELD_TCAM_ACCESS_PROFILE_PAIRED_ID_GET(app_db_id)));
    }

exit:
    SHR_FUNC_EXIT;
}
/**
* \brief
*  Receive the app_db_id for specific key_id
* Defines the profile for lookup for the key in input
* \param [in] unit          -  Device ID
* \param [in] field_stage   -  For which Field stage to Configure HW
* \param [in] context_id    - Context ID
* \param [in] key_id        - Key id to set
* \param [in] app_db_id_p   - This value indicate how the key used by HW (TCAM/ MSB ...) which banks to look etc..
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_key_app_db_id_hw_get(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dbal_enum_value_field_field_key_e key_id,
    uint32 *app_db_id_p)
{
    uint32 entry_handle_id;
    dbal_tables_e table_name;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /** DBAL_NOF_TABLES as invalid value*/
    table_name = DBAL_NOF_TABLES;

    SHR_IF_ERR_EXIT(dnx_field_key_kbr_hw_table_name_get(unit, field_stage, key_id, &table_name));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_name, &entry_handle_id));

    /** key construction*/
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_PMF_CTX_ID, context_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_KEY, key_id);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    /** Get the app_db_id*/
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_APP_DB_ID, INST_SINGLE, app_db_id_p));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Splits a constant qualifier into "all zeros" and "all ones" qualifiers.
* \param [in] unit                    - Device Id
* \param [in] field_stage             - For which Field stage to Configure HW
* \param [in] key_qual_map_p          - Qual Map - includes Encoded qualifier for which we get 
*                                       the qual_info, it's size and lsb in key.
* \param [in] dnx_qual_info_p         - full information for the const qualifier. 
* \param [out] key_qual_map_sub_qual  - a fake mapping of the sub qual to be presented for configuring the FFC.
*                                       An array with an element for each sub qualifier.
* \param [out] dnx_qual_info_sub_qual - full output information for the sub qualifiers. 
*                                       An array with an element for each sub qualifier.
* \param [out] nof_sub_qual_p         - number of sub qualifiers 
*                                       (number of elements in dnx_qual_info_sub_qual and key_qual_map_sub_qual).
* \return
*   shr_error_e             - Error Type
* \remark
*   * None.
* \see
*   * dnx_field_key_qual_info_get().
*/
static shr_error_e
dnx_field_key_qual_info_get_split_const(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_qual_map_in_key_t * key_qual_map_p,
    dnx_field_key_attached_qual_info_t * dnx_qual_info_p,
    dnx_field_qual_map_in_key_t key_qual_map_sub_qual[DNX_FIELD_KEY_MAX_CONST_SUB_QUAL],
    dnx_field_key_attached_qual_info_t dnx_qual_info_sub_qual[DNX_FIELD_KEY_MAX_CONST_SUB_QUAL],
    unsigned int *nof_sub_qual_p)
{
    uint32 in_const;
    unsigned int is_first_digit_set;
    unsigned int length_sub;
    dnx_field_qual_t all_ones_qual;
    dnx_field_qual_t all_zeros_qual;
    int all_ones_offset;
    uint32 all_ones_size;
    unsigned int sub_position;
    _shr_error_t rv;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(key_qual_map_p, _SHR_E_PARAM, "key_qual_map_p");
    SHR_NULL_CHECK(dnx_qual_info_p, _SHR_E_PARAM, "dnx_qual_info_p");
    SHR_NULL_CHECK(key_qual_map_sub_qual, _SHR_E_PARAM, "key_qual_map_sub_qual");
    SHR_NULL_CHECK(dnx_qual_info_sub_qual, _SHR_E_PARAM, "key_dnx_qual_info_sub_qualqual_map_p");
    SHR_NULL_CHECK(nof_sub_qual_p, _SHR_E_PARAM, "nof_sub_qual_p");

    if (dnx_qual_info_p->size <= 0 || dnx_qual_info_p->size > SAL_UINT32_NOF_BITS)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Constant qualifier size must be between %d and %d. It is %d.\r\n",
                     1, SAL_UINT32_NOF_BITS, dnx_qual_info_p->size);
    }

    /*
     * Initialize the parameters.
     */
    in_const = dnx_qual_info_p->index;
    if (in_const != dnx_qual_info_p->index)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Constant in input argument %d was truncated when converted to uint32, "
                     "from %u to %u.\r\n", dnx_qual_info_p->index, dnx_qual_info_p->index, in_const);
    }
    (*nof_sub_qual_p) = 0;

    /*
     * Sanity check.
     */
    if ((dnx_qual_info_p->size < SAL_UINT32_NOF_BITS) && ((in_const & SAL_FROM_BIT(dnx_qual_info_p->size)) != 0))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Constant %u doesn't fit inside bit size %d.\r\n", in_const, dnx_qual_info_p->size);
    }

    /*
     * Obtain the qualifier for obtaining constant ones to the key.
     * find it's size and offset.
     */
    all_ones_qual = DNX_FIELD_ACTION_INVALID;
    all_ones_offset = 0;
    all_ones_size = 0;
    if (in_const != 0)
    {
        rv = dnx_field_map_field_id_dnx_qual(unit, field_stage, DBAL_FIELD_ALL_ONES, &all_ones_qual);
        if (rv == _SHR_E_NOT_FOUND)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Stage \"%s\" does not support non-zero constant qualifiers. Input argument for const "
                         "qualifier 0x%x (\"%s\") is 0x%x.\r\n",
                         dnx_field_stage_text(unit, field_stage),
                         key_qual_map_p->qual_type, dnx_field_dnx_qual_text(unit, key_qual_map_p->qual_type),
                         dnx_qual_info_p->index);
        }
        else
        {
            SHR_IF_ERR_EXIT(rv);
        }
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_offset(unit, field_stage, all_ones_qual, &all_ones_offset));
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_size(unit, field_stage, all_ones_qual, &all_ones_size));
    }

    all_zeros_qual = DNX_QUAL(DNX_FIELD_QUAL_CLASS_SW, field_stage, DNX_FIELD_SW_QUAL_ALL_ZERO);

    sub_position = 0;
    while (sub_position < dnx_qual_info_p->size)
    {
        /*
         * Sanity check: Verify we do not exceed array limit
         */
        if (*nof_sub_qual_p >= DNX_FIELD_KEY_MAX_CONST_SUB_QUAL)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Number of sub qualifiers reached maximum (%d) and still  %d bits left in "
                         "const (out of %d).\r\n",
                         DNX_FIELD_KEY_MAX_CONST_SUB_QUAL, dnx_qual_info_p->size - sub_position, dnx_qual_info_p->size);
        }
        is_first_digit_set = DNX_FIELD_UINT_IS_BIT_SET(in_const, sub_position);
        /*
         * Count the number of consecutive ones or zeros and remove them from the constant.
         */
        for (length_sub = 0;
             (sub_position + length_sub < dnx_qual_info_p->size)
             && (DNX_FIELD_UINT_IS_BIT_SET(in_const, sub_position + length_sub) == is_first_digit_set); length_sub++)
        {
            /*
             * Check that we do not exceed the maximum size of an all_ones qualifier. 
             * All zero does nothing and is therefore not limited by size.
             */
            if (is_first_digit_set && length_sub >= all_ones_size)
            {
                break;
            }
        }
        /*
         * Sanity checks.
         */
        if (length_sub <= 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Loop did not progress, consecutive number of zeros or ones less than one (%d).\r\n,",
                         length_sub);
        }
        /*
         * Copy the configuration of the original 'dnx_qual_info' and 'key_qual_map'.
         */
        sal_memcpy(&(dnx_qual_info_sub_qual[*nof_sub_qual_p]), dnx_qual_info_p, sizeof(dnx_qual_info_sub_qual[0]));
        sal_memcpy(&(key_qual_map_sub_qual[*nof_sub_qual_p]), key_qual_map_p, sizeof(key_qual_map_sub_qual[0]));
        /*
         * Set the parameters.
         */
        dnx_qual_info_sub_qual[*nof_sub_qual_p].size = length_sub;
        key_qual_map_sub_qual[*nof_sub_qual_p].size = length_sub;
        key_qual_map_sub_qual[*nof_sub_qual_p].lsb += sub_position;
        if (is_first_digit_set)
        {
            key_qual_map_sub_qual[*nof_sub_qual_p].qual_type = all_ones_qual;
            dnx_qual_info_sub_qual[*nof_sub_qual_p].ffc_type = DNX_FIELD_FFC_LITERALLY;
            dnx_qual_info_sub_qual[*nof_sub_qual_p].offset = all_ones_offset;
        }
        else
        {
            key_qual_map_sub_qual[*nof_sub_qual_p].qual_type = all_zeros_qual;
            dnx_qual_info_sub_qual[*nof_sub_qual_p].ffc_type = DNX_FIELD_FFC_TYPE_INVALID;
            /*
             * We just write offset for predictability. It is meaningless for zero qualifier.
             */
            dnx_qual_info_sub_qual[*nof_sub_qual_p].offset = 0;
        }
        /*
         * We write a value to index for predictability. It is meaningless for invalid/literally FFC.
         */
        dnx_qual_info_sub_qual[*nof_sub_qual_p].index = 0;
        /*
         * Increment the counter.
         */
        (*nof_sub_qual_p)++;
        sub_position += length_sub;
    }

    if (sub_position != dnx_qual_info_p->size)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "%d bits were read out of %d.\r\n", sub_position, dnx_qual_info_p->size);
    }

    /*
     * Sanity check to ensure that no ones were written for a zero qualifier.
     */
    if ((in_const == 0) && (*nof_sub_qual_p != 1)
        && (key_qual_map_sub_qual[*nof_sub_qual_p].qual_type != all_zeros_qual))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Ones were written for a zero const.\r\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Verify function for cascaded input type attach info
* \param [in] unit             - Device Id
* \param [in] field_stage      - For which Field stage to Configure HW
* \param [in] qual_type        - Qual Type - includes Encoded qualifier
* \param [in] qual_info_p      - info from attach info.
* \return
*   shr_error_e               - Error Type
* \remark
*   * None.
* \see
*   * None
*/
static shr_error_e
dnx_field_key_qual_info_get_cascaded_verify(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_qual_t qual_type,
    dnx_field_qual_attach_info_t * qual_info_p)
{
    dnx_field_action_length_type_e action_length_type;
    dnx_field_stage_e cascaded_field_stage;
    int min_offset;
    int max_offset;
    uint32 qual_size;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(qual_info_p, _SHR_E_PARAM, "qual_info_p");

    if (qual_info_p->input_type != DNX_FIELD_INPUT_TYPE_CASCADED)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "This function should only be reached for input type cascaded. "
                     "(input type %d, input argument %d, qualifier %s, stage %s).\r\n",
                     qual_info_p->input_type, qual_info_p->input_arg, dnx_field_dnx_qual_text(unit, qual_type),
                     dnx_field_stage_text(unit, field_stage));
    }
    /*
     * For cascaded qualifiers, verify stages. Also verify we only read from the field group's payload.
     */

    if (field_stage != DNX_FIELD_STAGE_IPMF2)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Cascaded input type from other PMF stages only supported in iPMF2. "
                     "(input type %d, input argument %d, qualifier %s, stage %s).\r\n",
                     qual_info_p->input_type, qual_info_p->input_arg, dnx_field_dnx_qual_text(unit, qual_type),
                     dnx_field_stage_text(unit, field_stage));
    }
    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, qual_info_p->input_arg, &cascaded_field_stage));
    if (cascaded_field_stage != DNX_FIELD_STAGE_IPMF1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Cascaded input type from PMF stages can only cascade from stage iPMF1. "
                     "(input type %d, input argument (field group) %d, "
                     "qualifier %s, cascaded field group stage %s).\r\n",
                     qual_info_p->input_type, qual_info_p->input_arg, dnx_field_dnx_qual_text(unit, qual_type),
                     dnx_field_stage_text(unit, cascaded_field_stage));
    }

    /*
     * For cascaded from PMF qualifiers, verify that we only read within the key.
     */
    SHR_IF_ERR_EXIT(dnx_field_group_action_length_type_get(unit, qual_info_p->input_arg, &action_length_type));

    switch (action_length_type)
    {
        case DNX_FIELD_ACTION_LENGTH_TYPE_TCAM_HALF:
        {
            min_offset = 0;
            max_offset = dnx_data_field.tcam.action_size_half_get(unit);
            if (dnx_data_field.minor_changes_j2_a0_b0.tcam_result_flip_eco_get(unit))
            {
                min_offset = dnx_data_field.tcam.action_size_half_get(unit);
                max_offset = dnx_data_field.tcam.action_size_single_get(unit);
            }
            break;
        }
        case DNX_FIELD_ACTION_LENGTH_TYPE_TCAM_SINGLE:
        {
            min_offset = 0;
            max_offset = dnx_data_field.tcam.action_size_single_get(unit);
            break;
        }
        case DNX_FIELD_ACTION_LENGTH_TYPE_TCAM_DOUBLE:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Cascading not supported from double key TCAM field groups "
                         "(input type %d, input argument (field group) %d, "
                         "qualifier %s, cascaded field group stage %s).\r\n",
                         qual_info_p->input_type, qual_info_p->input_arg,
                         dnx_field_dnx_qual_text(unit, qual_type), dnx_field_stage_text(unit, cascaded_field_stage));
            break;
        }
        case DNX_FIELD_ACTION_LENGTH_TYPE_LEXEM:
        {
            min_offset = 0;
            max_offset = dnx_data_field.exem.large_max_result_size_get(unit);
            break;
        }
        case DNX_FIELD_ACTION_LENGTH_TYPE_SEXEM:
        {
            min_offset = 0;
            max_offset = dnx_data_field.exem.small_max_result_size_get(unit);
            break;
        }
        case DNX_FIELD_ACTION_LENGTH_TYPE_ACE:
        {
            min_offset = 0;
            max_offset = dnx_data_field.ace.payload_size_get(unit);
            break;
        }
        case DNX_FIELD_ACTION_LENGTH_TYPE_DIR_EXT:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Direct extraction not supported for cascading, "
                         "as it is not supported on iPMF1. "
                         "(input type %d, input argument (field group) %d, "
                         "qualifier %s, cascaded field group stage %s).\r\n",
                         qual_info_p->input_type, qual_info_p->input_arg,
                         dnx_field_dnx_qual_text(unit, qual_type), dnx_field_stage_text(unit, cascaded_field_stage));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown action_length_type %d. "
                         "(input type %d, input argument (field group) %d, "
                         "qualifier %s, cascaded field group stage %s).\r\n",
                         action_length_type,
                         qual_info_p->input_type, qual_info_p->input_arg,
                         dnx_field_dnx_qual_text(unit, qual_type), dnx_field_stage_text(unit, cascaded_field_stage));
            break;
        }
    }
    SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_size(unit, field_stage, qual_type, &qual_size));
    if (qual_info_p->offset < min_offset)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Offset (%d) cannot be smaller then (%d). "
                     "(input type %d, input argument (field group) %d, qualifier %s.\r\n",
                     qual_info_p->offset, min_offset, qual_info_p->input_type, qual_info_p->input_arg,
                     dnx_field_dnx_qual_text(unit, qual_type));
    }
    if (qual_info_p->offset + qual_size > max_offset)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Offset (%d) plus qualifier size (%d) (together %d) cannot exceed (%d). "
                     "(input type %d, input argument (field group) %d, qualifier %s.\r\n",
                     qual_info_p->offset, qual_size, qual_info_p->offset + qual_size, max_offset,
                     qual_info_p->input_type, qual_info_p->input_arg, dnx_field_dnx_qual_text(unit, qual_type));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Verify function for KBP input type attach info
* \param [in] unit             - Device Id
* \param [in] field_stage      - For which Field stage to Configure HW
* \param [in] qual_type        - Qual Type - includes Encoded qualifier
* \param [in] qual_info_p      - info from attach info.
* \return
*   shr_error_e               - Error Type
* \remark
*   * None.
* \see
*   * None
*/
static shr_error_e
dnx_field_key_qual_info_get_kbp_verify(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_qual_t qual_type,
    dnx_field_qual_attach_info_t * qual_info_p)
{
    dnx_field_qual_t dnx_qual_kbp_result_part_0;
    dnx_field_qual_t dnx_qual_kbp_result_part_1;
    uint32 qual_size_part_0;
    uint32 qual_size_part_1;
    uint32 qual_size;
    int kbp_total_payload_size;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(qual_info_p, _SHR_E_PARAM, "qual_info_p");

    if (qual_info_p->input_type != DNX_FIELD_INPUT_TYPE_KBP)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "This function should only be reached for input type KNP. "
                     "(input type %d, input argument %d, qualifier %s, stage %s).\r\n",
                     qual_info_p->input_type, qual_info_p->input_arg, dnx_field_dnx_qual_text(unit, qual_type),
                     dnx_field_stage_text(unit, field_stage));
    }

    /*
     * For KBP input type, verify stage. Also verify we only read from the range of KBP payloads.
     */

    if (field_stage != DNX_FIELD_STAGE_IPMF1 && field_stage != DNX_FIELD_STAGE_IPMF2)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Cascaded input type from KBP only supported in iPMF1 and iPMF2. "
                     "(input type %d, input argument %d, qualifier %s, stage %s).\r\n",
                     qual_info_p->input_type, qual_info_p->input_arg, dnx_field_dnx_qual_text(unit, qual_type),
                     dnx_field_stage_text(unit, DNX_QUAL_STAGE(qual_type)));
    }
    dnx_qual_kbp_result_part_0 =
        DNX_QUAL(DNX_FIELD_QUAL_CLASS_META, field_stage, DNX_FIELD_IPMF1_QUAL_ELK_LKP_PAYLOAD_ALL_PART_0);
    dnx_qual_kbp_result_part_1 =
        DNX_QUAL(DNX_FIELD_QUAL_CLASS_META, field_stage, DNX_FIELD_IPMF1_QUAL_ELK_LKP_PAYLOAD_ALL_PART_1);
    SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_size(unit, field_stage, dnx_qual_kbp_result_part_0, &qual_size_part_0));
    SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_size(unit, field_stage, dnx_qual_kbp_result_part_1, &qual_size_part_1));
    kbp_total_payload_size = qual_size_part_0 + qual_size_part_1;

    SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_size(unit, field_stage, qual_type, &qual_size));
    if (qual_info_p->offset < 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Offset (%d) cannot be smaller then 0. "
                     "(input type %d, input argument (field group) %d, qualifier %s.\r\n",
                     qual_info_p->offset, qual_info_p->input_type, qual_info_p->input_arg,
                     dnx_field_dnx_qual_text(unit, qual_type));
    }
    if (qual_info_p->offset + qual_size > kbp_total_payload_size)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Offset (%d) plus qualifier size (%d) (together %d) cannot exceed (%d). "
                     "(input type %d, input argument (field group) %d, qualifier %s.\r\n",
                     qual_info_p->offset, qual_size, qual_info_p->offset + qual_size, kbp_total_payload_size,
                     qual_info_p->input_type, qual_info_p->input_arg, dnx_field_dnx_qual_text(unit, qual_type));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Verify function for dnx_field_key_qual_info_get().
*  Checks for unsupported input_type and generalize_ffc_type combinations.
* \param [in] unit             - Device Id
* \param [in] field_stage      - For which Field stage to Configure HW
* \param [in] qual_type        - Qual Type - includes Encoded qualifier
* \param [in] qual_info_p      - info from attach info.
* \param [in] dnx_qual_info_p  - full output information for this qualifier. 
* \return
*   shr_error_e               - Error Type
* \remark
*   * None.
* \see
*   * None
*/
static shr_error_e
dnx_field_key_qual_info_get_verify(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_qual_t qual_type,
    dnx_field_qual_attach_info_t * qual_info_p,
    dnx_field_key_attached_qual_info_t * dnx_qual_info_p)
{
    dnx_field_qual_class_e dnx_qual_class;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(qual_info_p, _SHR_E_PARAM, "qual_info_p");
    SHR_NULL_CHECK(dnx_qual_info_p, _SHR_E_PARAM, "dnx_qual_info_p");

    dnx_qual_class = DNX_QUAL_CLASS(qual_type);

    /*
     * UDF can accept any input type. For other qualifiers classes, check if the input type and class match.
     */
    switch (qual_info_p->input_type)
    {
        case DNX_FIELD_INPUT_TYPE_LAYER_FWD:
        /** Fall through */
        case DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE:
        {
            if (field_stage == DNX_FIELD_STAGE_IPMF3)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "The header is not accessible to iPMF3, and thus layer (header) "
                             "input types cannot be used in that stage (qualifier %s).\r\n",
                             dnx_field_dnx_qual_text(unit, qual_type));
            }
            if ((dnx_qual_class != DNX_FIELD_QUAL_CLASS_HEADER) && (dnx_qual_class != DNX_FIELD_QUAL_CLASS_USER))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Input type layer (header) can only support header or "
                             "user defined qualifiers (qualifier %s).\r\n", dnx_field_dnx_qual_text(unit, qual_type));
            }
            break;
        }
        case DNX_FIELD_INPUT_TYPE_META_DATA:
        {
            if ((dnx_qual_class != DNX_FIELD_QUAL_CLASS_META) && (dnx_qual_class != DNX_FIELD_QUAL_CLASS_USER))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Input type metadata can only support metadata or user defined "
                             "qualifiers (qualifier %s).\r\n", dnx_field_dnx_qual_text(unit, qual_type));
            }
            break;
        }
        case DNX_FIELD_INPUT_TYPE_META_DATA2:
        {
            if (field_stage != DNX_FIELD_STAGE_IPMF2)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Only stage iPMF2 support input type metadata2 (%d), stage used: %s (%d). "
                             "Qualifier %s.\r\n",
                             DNX_FIELD_INPUT_TYPE_META_DATA2, dnx_field_stage_text(unit, field_stage), field_stage,
                             dnx_field_dnx_qual_text(unit, qual_type));
            }

            if ((dnx_qual_class != DNX_FIELD_QUAL_CLASS_META2) && (dnx_qual_class != DNX_FIELD_QUAL_CLASS_USER))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Input type secondary metadata can only support metadata2 or "
                             "user defined qualifiers (qualifier %s).\r\n", dnx_field_dnx_qual_text(unit, qual_type));
            }
            break;
        }
        case DNX_FIELD_INPUT_TYPE_LAYER_RECORDS_FWD:
        /** Fall through */
        case DNX_FIELD_INPUT_TYPE_LAYER_RECORDS_ABSOLUTE:
        {
            if ((dnx_qual_class != DNX_FIELD_QUAL_CLASS_LAYER_RECORD) && (dnx_qual_class != DNX_FIELD_QUAL_CLASS_USER))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Input type layer record can only support layer record or user defined "
                             "qualifiers (qualifier %s).\r\n", dnx_field_dnx_qual_text(unit, qual_type));
            }
            break;
        }
        case DNX_FIELD_INPUT_TYPE_CONST:
        {
            
            if ((dnx_qual_class != DNX_FIELD_QUAL_CLASS_SW) && (dnx_qual_class != DNX_FIELD_QUAL_CLASS_USER))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Input type Zero can only support software or user defined "
                             "qualifiers (qualifier %s).\r\n", dnx_field_dnx_qual_text(unit, qual_type));
            }
            break;
        }
        case DNX_FIELD_INPUT_TYPE_KBP:
        {
            if (dnx_qual_class != DNX_FIELD_QUAL_CLASS_USER)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Input type KBP can only support user defined qualifiers. "
                             "Got qual class %s (qualifier %s).\r\n",
                             dnx_field_qual_class_text(dnx_qual_class), dnx_field_dnx_qual_text(unit, qual_type));
            }
            break;
        }
        case DNX_FIELD_INPUT_TYPE_CASCADED:
        {
            if ((dnx_qual_class != DNX_FIELD_QUAL_CLASS_META2) && (dnx_qual_class != DNX_FIELD_QUAL_CLASS_USER))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Input type cascade can only support user defined qualifiers or meta2. "
                             "Got qual class %s (qualifier %s).\r\n",
                             dnx_field_qual_class_text(dnx_qual_class), dnx_field_dnx_qual_text(unit, qual_type));
            }
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Input type %d for qualifier %s is not supported.\r\n",
                         qual_info_p->input_type, dnx_field_dnx_qual_text(unit, qual_type));
        }
            break;
    }

    /*
     * For absolute input types, verify that the index isn't negative.
     */
    if ((qual_info_p->input_type == DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE)
        || (qual_info_p->input_type == DNX_FIELD_INPUT_TYPE_LAYER_RECORDS_ABSOLUTE))
    {
        if (qual_info_p->input_arg < 0)
            SHR_ERR_EXIT(_SHR_E_PARAM, "Absolute input types cannot take negative layer index "
                         "(input type %d, input argument %d, qualifier %s).\r\n",
                         qual_info_p->input_type, qual_info_p->input_arg, dnx_field_dnx_qual_text(unit, qual_type));
    }

    /*
     * For cascaded qualifiers, verify stages. Also verify we only read from the field group's payload.
     */
    if (qual_info_p->input_type == DNX_FIELD_INPUT_TYPE_CASCADED)
    {
        SHR_IF_ERR_EXIT(dnx_field_key_qual_info_get_cascaded_verify(unit, field_stage, qual_type, qual_info_p));
    }

    /*
     * For KBP input type, verify stage. Also verify we only read from the range of KBP payloads.
     */
    if (qual_info_p->input_type == DNX_FIELD_INPUT_TYPE_KBP)
    {
        SHR_IF_ERR_EXIT(dnx_field_key_qual_info_get_kbp_verify(unit, field_stage, qual_type, qual_info_p));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  This function will get the base meta2 qualifier, for the input type CASCADED
*  (meaning our qualifier takes the action buffer as input)
*  we will need to find the Key-Id of the field group in ipmf1, while that field group is attached
*  to a cascaded_from context in ipmf1.
* \param [in] unit                - Device Id
* \param [in] context_id          - Context ID
* \param [in] cascaded_from_fg_id - The field group we cascade from.
* \param [out] ipmf2_base_qual_p  - base  qualifier for configuration of the action qualifier
* * \return
*   shr_error_e             - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_key_ipmf2_base_cascaded_qual_get(
    int unit,
    int context_id,
    dnx_field_group_t cascaded_from_fg_id,
    dnx_field_qual_t * ipmf2_base_qual_p)
{
    dnx_field_key_id_t ipfm1_cascaded_from_key_id;
    dnx_field_context_t cascaded_from_context_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(ipmf2_base_qual_p, _SHR_E_PARAM, "ipmf2_base_qual_p");

    /*
     * Get the cascaded_from_context_id - the context in ipmf1 that context_id has cascaded from
     */
    SHR_IF_ERR_EXIT(dnx_field_context_cascaded_from_context_id_get
                    (unit, DNX_FIELD_STAGE_IPMF2, context_id, &cascaded_from_context_id));

    /** Get the key_id for the field_group we have cascaded from.
     * - cascaded_from_context_id is the context in IPMF1 we have
     * cascaded from
     * - qual_info_p->input_arg represents the FG id in IPMF1 that we
     * need to use the action buffer of.
     * - ipfm1_cascaded_from_key_id is the key-id that this FG (ipmf1)
     * received in cascaded_from_context_id
     * */
    SHR_IF_ERR_EXIT(dnx_field_group_key_get(unit,
                                            cascaded_from_context_id,
                                            cascaded_from_fg_id, &ipfm1_cascaded_from_key_id));

    /** Use the cascaded_from_context_id to receive the meta2 qualifier for the action buffer */
    SHR_IF_ERR_EXIT(dnx_field_key_qual_action_to_meta2_qual_get(unit,
                                                                ipfm1_cascaded_from_key_id.id[0], ipmf2_base_qual_p));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  This function will get the base meta2 qualifier, for the input type CASCADED
*  (meaning our qualifier takes the ACTION buffer as input)
*  we will need to find the Key-Id of the field group in ipmf1, while that
*  field group is attached to a cascaded_from context in ipmf1.
* \param [in]  unit                - Device Id
* \param [in]  field_stage         - The field stage where the qualifier is used.
* \param [in]  context_id          - Context ID to configure FFC's for.
* \param [in]  cascaded_from_fg_id - The field group we cascade from.
* \param [out] internal_offet_p    - Offset on the PBUS of the info we want to read.
* \param [out] native_pbus_p       - TRUE if the offset is in the native PBUS of iPMF2 (Metadata2).
* * \return
*   shr_error_e             - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_key_prepare_cascaded_qual_info(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_group_t cascaded_from_fg_id,
    int *internal_offet_p,
    int *native_pbus_p)
{
    dnx_field_qual_t internal_ipmf2_qual_type = DNX_FIELD_QUAL_TYPE_INVALID;
    dnx_field_qual_attach_info_t internal_qual_attach_info;
    dnx_field_stage_e cascaded_from_field_stage;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(internal_offet_p, _SHR_E_PARAM, "internal_offet_p");
    SHR_NULL_CHECK(native_pbus_p, _SHR_E_PARAM, "native_pbus_p");

    /*
     * Sanity check. Verify we cascade from iPMF1 to iPMF2.
     */
    SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, cascaded_from_fg_id, &cascaded_from_field_stage));
    if (field_stage != DNX_FIELD_STAGE_IPMF2 || cascaded_from_field_stage != DNX_FIELD_STAGE_IPMF1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Cascaded from field group %d is in stage \"%s\" to stage \"%s\". "
                     "Only cascading from iPMF1 to iPMF2 supported.\r\n",
                     cascaded_from_fg_id,
                     dnx_field_stage_text(unit, cascaded_from_field_stage), dnx_field_stage_text(unit, field_stage));
    }

    /*
     * Create the attach info for the internal qualifier - we need to set the input type to META_DATA2
     * */
    dnx_field_qual_attach_info_t_init(unit, &internal_qual_attach_info);
    internal_qual_attach_info.input_type = DNX_FIELD_INPUT_TYPE_META_DATA2;

    /**  This function will get the base meta2 qualifier,for the input type
     * CASCADED (meaning our qualifier takes the action buffer as input)
    *  we will need to use the Key-Id of the field group in ipmf1, while that field group is attached
    *  to a cascaded_from context in ipmf1. That key-id will be used to get the meta2 qual.
     * */
    SHR_IF_ERR_EXIT(dnx_field_key_ipmf2_base_cascaded_qual_get
                    (unit, context_id, cascaded_from_fg_id, &internal_ipmf2_qual_type));
    /*
     * Get the offset of the qualifier.
     */
    SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_offset
                    (unit, DNX_FIELD_STAGE_IPMF2, internal_ipmf2_qual_type, internal_offet_p));

    /*
     * The information comes from the native PBUS of iPMF2.
     */
    (*native_pbus_p) = TRUE;

    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "Qualifier for action %s context %d BASE internal_ipmf2_qual_info.offset 0x%X BASE "
                 "mapped_offset 0x%X\n",
                 dnx_field_dnx_qual_text(unit, internal_ipmf2_qual_type), context_id,
                 internal_qual_attach_info.offset, *internal_offet_p);

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Get the information for a given qualifier
* \param [in] unit             - Device Id
* \param [in] field_stage      - For which Field stage to Configure HW
* \param [in] context_id       - Context ID
* \param [in] qual_type   - qualifier map for which we get the qual_info
* \param [in] qual_info_p      - info from attach info.
* \param [out] dnx_qual_info_p - full output information for this qualifier. 
* \return
*   shr_error_e                - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_key_qual_info_get(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_qual_t qual_type,
    dnx_field_qual_attach_info_t * qual_info_p,
    dnx_field_key_attached_qual_info_t * dnx_qual_info_p)
{
    int index;
    int mapped_offset;
    int layer_record_offset;
    int base_offset;
    int added_offset;
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_field_key_qual_info_get_verify
                          (unit, field_stage, qual_type, qual_info_p, dnx_qual_info_p));

    /*
     * Initialize the output structure.
     */
    SHR_IF_ERR_EXIT(dnx_field_key_attached_qual_info_t_init(unit, dnx_qual_info_p));
    /*
     * Get the offset of the qualifier.
     */
    SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_offset(unit, field_stage, qual_type, &mapped_offset));

    /*
     * The size is taken entirely from the qualifier mapping.
     */
    SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_size(unit, field_stage, qual_type, &(dnx_qual_info_p->size)));

    /*
     * For user defined qualifier, the offset in the input type replaces the mapped offset.
     * Should UDQ ever get a base qualifier, here the user offset should be added to the mapped offset,
     * So we add the metadata offset to the user offset, as for now the mapped offset is always zero.
     */
    if (DNX_QUAL_CLASS(qual_type) == DNX_FIELD_QUAL_CLASS_USER)
    {
        base_offset = mapped_offset + qual_info_p->offset;
        added_offset = 0;
    }
    else
    {
        /*
         * For predefined qualifiers, the base offset is the mapped offset and the added offset is the attach info
         * offset. some input types use it, and some don't (unlike user defined qualifiers where all must use it).
         */
        base_offset = mapped_offset;
        added_offset = qual_info_p->offset;
    }

    dnx_qual_info_p->input_type = qual_info_p->input_type;

    /*
     * Write the legitimate FFC ranges (only applicable to iPMF1 and non-native iPMF2 qualifiers.
     */
    LOG_DEBUG_EX(BSL_LOG_MODULE, "qual_type %s field_stage %s qual_info_p->input_type %d size %d.\n",
                 dnx_field_dnx_qual_text(unit, qual_type), dnx_field_stage_text(unit, field_stage),
                 qual_info_p->input_type, dnx_qual_info_p->size);

    SHR_IF_ERR_EXIT(dnx_field_map_qual_input_type_to_ranges
                    (unit, field_stage, qual_info_p->input_type, &(dnx_qual_info_p->ranges)));

    /*
     * Add the attach info.
     */
    switch (qual_info_p->input_type)
    {
        case DNX_FIELD_INPUT_TYPE_LAYER_FWD:
        {
            dnx_qual_info_p->ffc_type = DNX_FIELD_FFC_RELATIVE_HEADER;
            SHR_IF_ERR_EXIT(dnx_field_map_layer_index_modulo(unit, field_stage, qual_info_p->input_arg, &index));
            dnx_qual_info_p->index = index;
            dnx_qual_info_p->offset =
                DNX_FIELD_KEY_FFC_LAYER_OFFSET(unit, field_stage, base_offset + added_offset, dnx_qual_info_p->size);
            break;
        }
        case DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE:
        {
            dnx_qual_info_p->ffc_type = DNX_FIELD_FFC_ABSOLUTE_HEADER;
            SHR_IF_ERR_EXIT(dnx_field_map_layer_index_modulo(unit, field_stage, qual_info_p->input_arg, &index));
            dnx_qual_info_p->index = index;
            dnx_qual_info_p->offset =
                DNX_FIELD_KEY_FFC_LAYER_OFFSET(unit, field_stage, base_offset + added_offset, dnx_qual_info_p->size);
            break;
        }
        case DNX_FIELD_INPUT_TYPE_META_DATA:
        {
            dnx_qual_info_p->ffc_type = DNX_FIELD_FFC_LITERALLY;
            /*
             * We write the default input argument for predictability, to make sure we ignore the input argument
             * in attach info.
             */
            dnx_qual_info_p->index = 0;
            dnx_qual_info_p->offset = base_offset;
            break;
        }
        case DNX_FIELD_INPUT_TYPE_META_DATA2:
        {
            dnx_qual_info_p->ffc_type = DNX_FIELD_FFC_LITERALLY;
            /*
             * We write the default input argument for predictability, to make sure we ignore the input argument
             * in attach info.
             */
            dnx_qual_info_p->index = 0;
            dnx_qual_info_p->offset = base_offset;
            dnx_qual_info_p->native_pbus = TRUE;
            break;
        }
        case DNX_FIELD_INPUT_TYPE_LAYER_RECORDS_FWD:
        {
            dnx_qual_info_p->ffc_type = DNX_FIELD_FFC_LAYER_RECORD;
            SHR_IF_ERR_EXIT(dnx_field_map_layer_index_modulo(unit, field_stage, qual_info_p->input_arg, &index));
            dnx_qual_info_p->index = index;
            dnx_qual_info_p->offset = base_offset;
            break;
        }
        case DNX_FIELD_INPUT_TYPE_LAYER_RECORDS_ABSOLUTE:
        {
            dnx_qual_info_p->ffc_type = DNX_FIELD_FFC_LITERALLY;
            SHR_IF_ERR_EXIT(dnx_field_map_layer_index_modulo(unit, field_stage, qual_info_p->input_arg, &index));
            dnx_qual_info_p->index = 0;
            SHR_IF_ERR_EXIT(dnx_field_map_layer_record_offset(unit, field_stage, index, &layer_record_offset));
            dnx_qual_info_p->offset = layer_record_offset + base_offset;
            break;
        }
        case DNX_FIELD_INPUT_TYPE_KBP:
        {
            /*
             * We cascade from KBP.
             */
            int kbp_payload_offset;
            dnx_field_qual_t dnx_qual_kbp_result;

            dnx_qual_info_p->ffc_type = DNX_FIELD_FFC_LITERALLY;
            dnx_qual_info_p->index = 0;

            dnx_qual_kbp_result =
                DNX_QUAL(DNX_FIELD_QUAL_CLASS_META, field_stage, DNX_FIELD_IPMF1_QUAL_ELK_LKP_PAYLOAD_ALL_PART_0);
            SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_offset(unit, field_stage, dnx_qual_kbp_result, &kbp_payload_offset));

            /*
             * If in the future we'll add predefined KBP qualifiers, we'll need to decide if we can add offset to them.
             */
            dnx_qual_info_p->offset = kbp_payload_offset + base_offset;
            break;
        }
        case DNX_FIELD_INPUT_TYPE_CASCADED:
        {
            int action_buffer_offset;
            /*
             * Used for INPUT_TYPE_CASCADED qualifier. INPUT_TYPE_CASCADED is used for ipmf2 qualifiers that extract
             * the data from the action buffer from ipmf1 - input_arg supplies the field group id to take the 
             * action from - offset is the exact offset inside the buffer in the pbus for the data to be extracted from. 
             */

            /*
             * Get the information for the internal action (cascaded) qualifier
             * that will be used as the base qualifier for configuration of the ffc
             * */
            SHR_IF_ERR_EXIT(dnx_field_key_prepare_cascaded_qual_info(unit, field_stage, context_id,
                                                                     qual_info_p->input_arg, &action_buffer_offset,
                                                                     &(dnx_qual_info_p->native_pbus)));

            dnx_qual_info_p->ffc_type = DNX_FIELD_FFC_LITERALLY;
            /*
             * Save the FG-ID for future use 
             */
            dnx_qual_info_p->index = qual_info_p->input_arg;
            /*
             * Update the (ffc src ) offset for the ipmf2 qualifier.
             * The base is the offset of the action buffer of appropriate FG, plus the user-provided
             * offset of the data inside the buffer.
             */
            dnx_qual_info_p->offset = action_buffer_offset + qual_info_p->offset;
            break;
        }
        case DNX_FIELD_INPUT_TYPE_CONST:
        {
            /*
             * For input type const, the qualifier will be broken into sub qualifiers of zeros and ones.
             * This parameters will later be filled for each sub qualifier, so for now they are irrelevant.
             */
            dnx_qual_info_p->ffc_type = DNX_FIELD_FFC_TYPE_INVALID;
            dnx_qual_info_p->index = qual_info_p->input_arg;
            dnx_qual_info_p->offset = DNX_FIELD_ATTACH_INFO_FIELD_OFFSET_DEFAULT;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported input type: %d.\r\n", qual_info_p->input_type);
        }
    }

    /*
     * Sanity check, verify that offset isn't negative. 
     */
    if (dnx_qual_info_p->offset < 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Calculated offset (%d) is negative.\r\n", dnx_qual_info_p->offset);
    }

    LOG_DEBUG_EX(BSL_LOG_MODULE, "qual_type %s field_stage %s offset %d original offset %d.\n",
                 dnx_field_dnx_qual_text(unit, qual_type), dnx_field_stage_text(unit, field_stage),
                 dnx_qual_info_p->offset, qual_info_p->offset);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Calculate DBAL values for FFC instructions
 * \param [in] unit              - Device ID
 * \param [in] dnx_qual_info_p   - pointer of type of dnx_field_key_attached_qual_info_t
 * \param [in] ffc_index         - FFC index
 * \param [in] is_read_from_initial  -
 *   Whether the qualifier is ipmf2 and not meta2, meaning
 *   if this ffc takes content written by initial ffc in ipmf1.
 *   This will determine the offset calculations
 * \param [in] is_initial        -
 *   Whether the ffc is initial or not.
 *   This will determine the key_offset calculations
 * \param [in] key_qual_map_p    -
 *   Key qualifier map to configure
 * \param [in] bit_range_offset_within_key -
 *   Location of key template within the key.
 *   For example, a key of 160 bits may host a few key templates:
 *     One of size, say, 21 bits, starting from bit0 of the key
 *     One of size, say, 32 bits, starting from bit21 of the key
 *     Etc.
 *   The 'key_offset' on 'ffc_instruction' is the physical location on the key
 *   and is, therefore, the sum of this input and the offset on the key template.
 *   This input is not relevant when 'is_initial' is TRUE.
 * \param [out] ffc_instruction_p -
 *   Pointer to dnx_field_key_ffc_instruction_info_t. This procedure loads pointed memory
 *   with 'FFC instruction' to be loaded into HW.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_field_key_calc_ffc_instruction_values(
    int unit,
    dnx_field_key_attached_qual_info_t * dnx_qual_info_p,
    uint32 ffc_index,
    uint32 is_read_from_initial,
    uint32 is_initial,
    dnx_field_qual_map_in_key_t * key_qual_map_p,
    uint32 bit_range_offset_within_key,
    dnx_field_key_ffc_instruction_info_t * ffc_instruction_p)
{
    int qual_size = 0;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(dnx_qual_info_p, _SHR_E_PARAM, "dnx_qual_info_p");
    SHR_NULL_CHECK(key_qual_map_p, _SHR_E_PARAM, "key_qual_map_p");
    SHR_NULL_CHECK(ffc_instruction_p, _SHR_E_PARAM, "ffc_instruction_p");

    qual_size = dnx_qual_info_p->size;

    /*
     * Some qualifiers size is bigger then 32b (MAC for example). Thus, we
     * allocate an appropriate number of ffc in order to handle such qualifier.
     * In such case, the ffc instruction should be calculated in 32'b quanta,
     * until a residue which is smaller then 32'b will be received. The last ffc
     * will be pointed to this residue.
     * If the qualifier size is smaller then 32'b, we will have the ffc_index=0
     * and the calculations will be straight forward.
     */

    /*
     * The offset is the starting point of the data in the pbus. calculated accordingly to ffc_index
     */
    if (is_read_from_initial)
    {
        /*
         * In case this is ipmf2 ffc that takes content from initial ffc (not META2 qual),
         * then the source offset of that ffc should be determined explicitly
         * by the initial ffc destination.
         * In this case, we do not take the ffc_index*32'b into calculations.
         */
        ffc_instruction_p->offset = dnx_qual_info_p->offset;
    }
    else
    {
        ffc_instruction_p->offset =
            dnx_qual_info_p->offset + ffc_index * DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_BITS_IN_FFC;
    }

    /*
     * field_index is the header number
     */
    ffc_instruction_p->field_index = dnx_qual_info_p->index;

    LOG_DEBUG_EX(BSL_LOG_MODULE, "*ffc_offset_p 0x%X, ffc_index %d, dnx_qual_info_p->offset 0x%X field_index %d\n",
                 ffc_instruction_p->offset, ffc_index, dnx_qual_info_p->offset, ffc_instruction_p->field_index);

    /*
     * Key offset is the location in the key. Calculated according to 32'b
     * quanta.
     */
    if (is_initial)
    {
        /*
         * In case this is initial ffc, then the destination of the ffc should be determined explicitly
         * by the occupation bmp allocation (see dnx_field_key_ipmf1_initial_key_occupation_bmp_allocate).
         * In this case, we do not take the ffc_index*32'b into calculations.
         */
        ffc_instruction_p->key_offset = (key_qual_map_p->lsb) % DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_KBR_SIZE;
    }
    else
    {
        /*
         * This is the only place that needs to be changed so that the offset on the key is
         * specified correctly. (This offset, then, is NOT necessarily the same as the offset on the
         * key template.)
         * This procedure is called from three places:
         * a. dnx_field_key_initial_key_ffc_set() -
         *    Handling of initial key (in which case we reach
         *    the 'if' above and the physical offset within the key is irrelevant).
         * b. dnx_field_key_ffc_hw_set() -
         *    Handling of all stages except for 'IPMF2' - Offset within key is relevant.
         * c. dnx_field_key_ipmf2_ffc_set() -
         *    Handling of IPMF2 - Offset within key is relevant.
         */
        ffc_instruction_p->key_offset =
            (key_qual_map_p->lsb +
             ffc_index * DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_BITS_IN_FFC +
             bit_range_offset_within_key) % DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_KBR_SIZE;

    }

    /*
     * Calculate the size of the data to be extracted by this ffc. In case the remaining
     * size is bigger then 32'b we will set the size to 32 bits, and the next
     * ffc will handle the residue. If the remaining size is smaller then 32'b
     * we will set the size to the residue.
     */
    /*
     * The size of the data in the ffc instruction is 5 bits, when:
     * in order to get 32 bits we need to set the ffc instruction to 0x1f
     * (32-1=31).
     * in order to get 1 bits we need to set the ffc instruction to 0x0
     * (1-1=0)
     * Thus, we subtract 1 from the size.
     * */
    if ((qual_size - ffc_index * DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_BITS_IN_FFC) >=
        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_BITS_IN_FFC)
    {
        ffc_instruction_p->size = DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_BITS_IN_FFC - 1;
    }
    else
    {
        ffc_instruction_p->size = qual_size - ffc_index * DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_BITS_IN_FFC - 1;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Configure a single FFC per context ID with given instruction info
* \param [in] unit          - Device Id
* \param [in] field_stage   - For which Field stage to Configure HW
* \param [in] context_id    - Context ID to configure FFC's for
* \param [in] ffc_p          - FFC id and instruction to be configured here
* \param [in] dnx_qual_info_p - Qualifiers Info
* * \return
*   shr_error_e             - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_key_single_ffc_hw_set(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_key_ffc_hw_info_t * ffc_p,
    dnx_field_key_attached_qual_info_t * dnx_qual_info_p)
{
    dbal_fields_e ffc_type;
    dbal_fields_e dbal_ctx_id;
    uint32 ffc_instruction_buffer = 0;
    uint32 entry_handle_id = 0;
    dbal_tables_e table_name;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(ffc_p, _SHR_E_PARAM, "ffc_p");
    SHR_NULL_CHECK(dnx_qual_info_p, _SHR_E_PARAM, "dnx_qual_info_p");

    /** Get FFC_TYPE dbal field id */
    SHR_IF_ERR_EXIT(dnx_field_map_stage_to_ffc_table(unit, field_stage, &table_name));
    SHR_IF_ERR_EXIT(dnx_field_map_ffc_type_to_field(unit, field_stage, dnx_qual_info_p->ffc_type, &ffc_type));
    SHR_IF_ERR_EXIT(dnx_field_map_ctx_id_to_field(unit, field_stage, &dbal_ctx_id));
    if (ffc_type == DBAL_FIELD_EMPTY)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "No ffc type %d in stage %s.\r\n", dnx_qual_info_p->ffc_type,
                     dnx_field_stage_text(unit, field_stage));
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_name, &entry_handle_id));

    /** Key construction*/
    dbal_entry_key_field32_set(unit, entry_handle_id, dbal_ctx_id, context_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FFC_IDX, ffc_p->ffc_id);
    LOG_DEBUG_EX(BSL_LOG_MODULE, "ffc_id %d, context ID %d stage %s %s\n", ffc_p->ffc_id,
                 context_id, dnx_field_stage_text(unit, field_stage), EMPTY);

    /** Fill the DBAL table Values*/
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                    (unit, ffc_type, DBAL_FIELD_FFC_FIELD_OFFSET, ((uint32 *) &(ffc_p->ffc_instruction.offset)),
                     &ffc_instruction_buffer));
    /*
     * Here comparison is unnatural - should be reevaluated
     */
    if ((ffc_type != DBAL_FIELD_IPMF1_FFC_LITERALLY) && (ffc_type != DBAL_FIELD_IPMF2_FFC_LITERALLY) &&
        (ffc_type != DBAL_FIELD_IPMF3_FFC_LITERALLY) && (ffc_type != DBAL_FIELD_EPMF_FFC_LITERALLY) &&
        (ffc_type != DBAL_FIELD_FWD12_FFC_LITERALLY))
    {
        /** Index field is not present in FFC type of Literally*/
        SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                        (unit, ffc_type, DBAL_FIELD_FFC_INDEX, ((uint32 *) &(ffc_p->ffc_instruction.field_index)),
                         &ffc_instruction_buffer));
    }

    /** Offset in key and the size is taken directly from the key */
    LOG_DEBUG_EX(BSL_LOG_MODULE, "offset in key 0x%X ffc_offset 0x%X index %d  offset %d\n",
                 ffc_p->ffc_instruction.key_offset, ffc_p->ffc_instruction.offset, ffc_p->ffc_instruction.field_index,
                 ffc_p->ffc_instruction.offset);
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                    (unit, ffc_type, DBAL_FIELD_FFC_KEY_OFFSET, &ffc_p->ffc_instruction.key_offset,
                     &ffc_instruction_buffer));

    /** Set the key qual size */
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                    (unit, ffc_type, DBAL_FIELD_FFC_FIELD_SIZE, &ffc_p->ffc_instruction.size, &ffc_instruction_buffer));

    /** Void function, Set encoded buffer about FFC instruction to ffc_instruction */
    dbal_entry_value_field32_set(unit, entry_handle_id, ffc_type, INST_SINGLE, ffc_instruction_buffer);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** Clear the entry for other FFC's to config */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_name, entry_handle_id));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  This function will calculate the values required for the ffc configuration
*  for the ipmf2 ffc that have need to extract the data from the ipmf2 pbus
*  (placed there by initial qualifier)
* \param [in] unit          - Device Id
* \param [in] context_id    - Context ID to configure FFC's for
* \param [in] key_qual_map_p- Key qualifier map to configure. Represents
* \param [in] initial_ffc_info_p           - information about the pre-allocated initial ffc.
*                                          - parameters from this structure will be used in order
*                                          - to calculate the instruction for the ipmf2 ffc.
* \param [out] internal_ipmf2_qual_info_p  - info structure for the internal ipmf2 qualifier
* * \return
*   shr_error_e             - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_key_ipmf2_prepare_internal_qual_info(
    int unit,
    int context_id,
    dnx_field_qual_map_in_key_t * key_qual_map_p,
    dnx_field_key_initial_ffc_info_t * initial_ffc_info_p,
    dnx_field_key_attached_qual_info_t * internal_ipmf2_qual_info_p)
{
    dnx_field_qual_t internal_ipmf2_dnx_qual;
    dnx_field_ipmf2_qual_e ipmf2_base_qual = DNX_FIELD_QUAL_ID_INVALID;
    dnx_field_qual_attach_info_t qual_attach_info;

    /*
     * *  We will translate the initial key-id into the meta2 qualifier. we have received that key-id while allocating a
     *  place-holder in the occupation bitmap for that qualifier.
     *  to a cascaded_from context in ipmf1.
     *
     * Set the ffc (ipmf2) instruction. src offset will point to the internally allocated placeholder of the
     * initial key buffer. destination offset will point to the offset given by the key-template, stored in
     * key_qual_map_p
     */
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(key_qual_map_p, _SHR_E_PARAM, "key_qual_map_p");
    SHR_NULL_CHECK(initial_ffc_info_p, _SHR_E_PARAM, "initial_ffc_info_p");
    SHR_NULL_CHECK(internal_ipmf2_qual_info_p, _SHR_E_PARAM, "internal_ipmf2_qual_info_p");

    /*
     * Create the attach info for the internal qualifier - we need to set the input type to META_DATA2
     * */
    dnx_field_qual_attach_info_t_init(unit, &qual_attach_info);
    qual_attach_info.input_type = DNX_FIELD_INPUT_TYPE_META_DATA2;
    /*
     * Convert the initial Key-Id into meta2 qualifier for ipmf2
     * */

    SHR_IF_ERR_EXIT(dnx_field_key_qual_initial_key_to_meta2_qual_get
                    (unit, initial_ffc_info_p->ffc.key_id, &ipmf2_base_qual));
    /*
     * Create internal (dummy) qualifier for extracting the data from initial key buffer into the ipmf2 Key
     */
    internal_ipmf2_dnx_qual = DNX_QUAL(DNX_FIELD_QUAL_CLASS_META2, DNX_FIELD_STAGE_IPMF2, ipmf2_base_qual);
    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "allocated place in context %d initial key %s BASE offset in key 0x%X %s\n",
                 context_id, dnx_field_dnx_qual_text(unit, internal_ipmf2_dnx_qual),
                 initial_ffc_info_p->key_dest_offset, EMPTY);

    SHR_IF_ERR_EXIT(dnx_field_key_qual_info_get
                    (unit, DNX_FIELD_STAGE_IPMF2, context_id, internal_ipmf2_dnx_qual, &qual_attach_info,
                     internal_ipmf2_qual_info_p));

    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "allocated place in context %d initial key %s BASE internal_ipmf2_qual_info.offset 0x%X BASE offset in key 0x%X\n",
                 context_id, dnx_field_dnx_qual_text(unit, internal_ipmf2_dnx_qual), internal_ipmf2_qual_info_p->offset,
                 initial_ffc_info_p->key_dest_offset);

    /*
     * Update the src offset for the ipmf2 qualifier.
     * The base is the key_id of the initial key, plus the internal
     * offset of the data inside the key.
     */
    internal_ipmf2_qual_info_p->offset += initial_ffc_info_p->key_dest_offset;
    /*
     * Update the size of the required qualifier. for example, we don't need the whole 160b' of initial Key-F, we
     * are looking for a specific size inside Key-F.
     */
    internal_ipmf2_qual_info_p->size = key_qual_map_p->size;

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   In case we process a non META2 qualifier that requires allocation and configuration
 *   of resources in IPMF2 - Configure the FFC's for the given qualifier in IPMF2
 *   This function will be called in one of the 2 following cases -
 *     First case - We handle a qualifier that requires an allocation of initial key
 *       (such as HEADER/META) and this function will handle the configuration
 *       of the ffc that 'derives' from the ffc in initial key
 *     Second case - We handle a qualifier that should process data from an Action Result of a FG
 *       in IPMF1  and this function will handle the configuration
 *       of the ffc that 'parses' the action buffer from IPMF1. Such qualifier comes
 *       with the input_type = DNX_FIELD_INPUT_TYPE_CASCADED.
 * \param [in] unit          - Device Id
 * \param [in] context_id    - Context ID to configure FFC's for
 * \param [in] key_qual_map_p        - Key qualifier map to configure.
 * \param [in] qualifier_ffc_info_p  - Info structure for the current qualifier
 * \param [in] bit_range_offset_within_key -
 *   Location of key template, containing this qualifier, within the key.
 *   For example, a key of 160 bits may host a few key templates:
 *     One of size, say, 21 bits, starting from bit0 of the key
 *     One of size, say, 32 bits, starting from bit21 of the key
 *     Etc.
 *   The 'key_offset' on 'ffc_instruction' is the physical location on the key
 *   and is, therefore, the sum of this input and the offset on the key template.
* * \return
*   shr_error_e             - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_key_ipmf2_ffc_set(
    int unit,
    int context_id,
    dnx_field_qual_map_in_key_t * key_qual_map_p,
    dnx_field_key_qualifier_ffc_info_t * qualifier_ffc_info_p,
    uint32 bit_range_offset_within_key)
{
    dnx_field_key_attached_qual_info_t internal_ipmf2_qual_info;

    int ffc_idx;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(key_qual_map_p, _SHR_E_PARAM, "key_qual_map_p");
    SHR_NULL_CHECK(qualifier_ffc_info_p, _SHR_E_PARAM, "qualifier_ffc_info_p");

    for (ffc_idx = 0;
         (ffc_idx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC_IN_QUAL)
         && (qualifier_ffc_info_p->ffc_info[ffc_idx].ffc.ffc_id != DNX_FIELD_FFC_ID_INVALID); ffc_idx++)
    {
        /*
         * Get the information of the internal IPMF2 qualifier that will be used as the base qualifier for
         * configuration of the ffc
         */
        SHR_IF_ERR_EXIT(dnx_field_key_ipmf2_prepare_internal_qual_info(unit,
                                                                       context_id,
                                                                       key_qual_map_p,
                                                                       &qualifier_ffc_info_p->
                                                                       ffc_info[ffc_idx].ffc_initial,
                                                                       &internal_ipmf2_qual_info));

        /*
         * Calculate the instruction values for the ffcs
         */
        SHR_IF_ERR_EXIT(dnx_field_key_calc_ffc_instruction_values
                        (unit, &internal_ipmf2_qual_info, ffc_idx, TRUE, FALSE, key_qual_map_p,
                         bit_range_offset_within_key, &qualifier_ffc_info_p->ffc_info[ffc_idx].ffc.ffc_instruction));

        SHR_IF_ERR_EXIT(dnx_field_key_single_ffc_hw_set
                        (unit, DNX_FIELD_STAGE_IPMF2, context_id,
                         &(qualifier_ffc_info_p->ffc_info[ffc_idx].ffc), &internal_ipmf2_qual_info));

        LOG_DEBUG_EX(BSL_LOG_MODULE,
                     "Qual %s internal_ipmf2_qual_info.offset 0x%X offset in key 0x%X %s\n",
                     dnx_field_dnx_qual_text(unit, key_qual_map_p->qual_type), internal_ipmf2_qual_info.offset,
                     qualifier_ffc_info_p->ffc_info[ffc_idx].ffc_initial.key_dest_offset, EMPTY);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Get information on a single, indicated, FFC which is used by specified context and stage
* \param [in] unit          - Device Id
* \param [in] field_stage   - For which Field stage to receive the HW information
* \param [in] context_id    - PMF program this FFC belongs to
* \param [in] ffc_id        - Index of the required FFC
* \param [out] ffc_instruction_info_p - Instruction Info
* * \return
*   shr_error_e             - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_key_single_ffc_hw_get(
    int unit,
    dnx_field_stage_e field_stage,
    int context_id,
    uint32 ffc_id,
    dnx_field_key_ffc_instruction_info_t * ffc_instruction_info_p)
{
    dbal_tables_e dbal_table_name;
    dbal_fields_e dbal_ffc_type;
    dbal_fields_e dbal_ffc_instruction;

    uint32 entry_handle_id = 0;
    uint32 ffc_30_bit_instruction = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(ffc_instruction_info_p, _SHR_E_PARAM, "ffc_instruction_info_p");

    /** Get table name */
    SHR_IF_ERR_EXIT(dnx_field_map_stage_to_ffc_table(unit, field_stage, &dbal_table_name));
    SHR_IF_ERR_EXIT(dnx_field_map_stage_to_ffc_instruction(unit, field_stage, &dbal_ffc_instruction));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_name, &entry_handle_id));

    /** Key construction*/
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_PMF_CTX_ID, context_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FFC_IDX, ffc_id);

    LOG_DEBUG_EX(BSL_LOG_MODULE, "context_id %d field_stage %s ffc_id %d, program %d\n", context_id,
                 dnx_field_stage_text(unit, field_stage), ffc_id, context_id);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, dbal_ffc_instruction, INST_SINGLE,
                                                        &(ffc_instruction_info_p->full_instruction)));

    /*
     * decode the 32'b ffc_full_instruction according to the ffc type ffc_30_bit_instruction is the full instruction
     * without the ffc_type (2b')
     */
    SHR_IF_ERR_EXIT(dbal_fields_parent_field_decode
                    (unit, dbal_ffc_instruction, &(ffc_instruction_info_p->full_instruction), &dbal_ffc_type,
                     &ffc_30_bit_instruction));

    if ((dbal_ffc_type != DBAL_FIELD_IPMF1_FFC_LITERALLY) && (dbal_ffc_type != DBAL_FIELD_IPMF2_FFC_LITERALLY) &&
        (dbal_ffc_type != DBAL_FIELD_IPMF3_FFC_LITERALLY) && (dbal_ffc_type != DBAL_FIELD_EPMF_FFC_LITERALLY))
    {
        /** Index field is not present in FFC type of Literally*/
        SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode(unit, dbal_ffc_type, DBAL_FIELD_FFC_INDEX,
                                                        ((uint32 *) &(ffc_instruction_info_p->field_index)),
                                                        &ffc_30_bit_instruction));
    }
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                    (unit, dbal_ffc_type, DBAL_FIELD_FFC_FIELD_OFFSET, ((uint32 *) &(ffc_instruction_info_p->offset)),
                     &ffc_30_bit_instruction));
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                    (unit, dbal_ffc_type, DBAL_FIELD_FFC_FIELD_SIZE, &(ffc_instruction_info_p->size),
                     &ffc_30_bit_instruction));
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                    (unit, dbal_ffc_type, DBAL_FIELD_FFC_KEY_OFFSET, &(ffc_instruction_info_p->key_offset),
                     &ffc_30_bit_instruction));

    ffc_instruction_info_p->ffc_type = dbal_ffc_type;

    LOG_DEBUG_EX(BSL_LOG_MODULE, "ffc_size 0x%X, ffc_field_index %d, offset in key 0x%X ffc_offset 0x%X\n",
                 ffc_instruction_info_p->size, ffc_instruction_info_p->field_index, ffc_instruction_info_p->key_offset,
                 ffc_instruction_info_p->offset);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Delete the FFC's configuration per context ID
* \param [in] unit          - Device Id
* \param [in] context_id - PMF context to configure FFC's for
* \param [in] ffc_table_name- ffc instruction table name to configure FFC's for
* \param [in] ffc_id        - FFC's allocated for the current qualifier
* * \return
*   shr_error_e             - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_key_ffc_instruction_hw_clear(
    int unit,
    dnx_field_context_t context_id,
    dbal_tables_e ffc_table_name,
    uint32 ffc_id)
{
    uint32 entry_handle_id = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, ffc_table_name, &entry_handle_id));

    /** Key construction*/
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_PMF_CTX_ID, context_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FFC_IDX, ffc_id);

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

    LOG_DEBUG_EX(BSL_LOG_MODULE, "FFC instruction delete ffc_id %d, context ID %d, %s%s\n",
                 ffc_id, context_id, EMPTY, EMPTY);
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Delete the FFC configuration per context ID with given Keys
* \param [in] unit          - Device Id
* \param [in] field_stage   - For which Field stage to Configure HW
* \param [in] context_id    - PMF context to configure FFC's for
* \param [in] ffc_id        - FFC allocated for the current qualifier
* * \return
*   shr_error_e             - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_key_qual_single_ffc_instruction_clear(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    uint32 ffc_id)
{
    dbal_tables_e ffc_table_name;
    SHR_FUNC_INIT_VARS(unit);

    LOG_DEBUG_EX(BSL_LOG_MODULE, "Delete ffc_id %d field_stage %s context_id %d %s\n", ffc_id,
                 dnx_field_stage_text(unit, field_stage), context_id, EMPTY);

    /** Per field stage choose the correct table to configure*/
    SHR_IF_ERR_EXIT(dnx_field_map_stage_to_ffc_table(unit, field_stage, &ffc_table_name));

    SHR_IF_ERR_EXIT(dnx_field_key_ffc_instruction_hw_clear(unit, context_id, ffc_table_name, ffc_id));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  In case we process a qualifier that requires allocation and configuration of resources in IPMF1 -
*  Configure the FFC's for the given qualifier
* \param [in] unit          - Device Id
* \param [in] context_id    - Context ID to configure FFC's for
* \param [in] key_qual_map_p- Key qualifier map to configure. Represents
* \param [in] qualifier_ffc_info_p   - info structure for the current qualifier
* \param [in] qual_info_p   - Qualifiers Info
* * \return
*   shr_error_e             - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_key_initial_key_ffc_set(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_qual_map_in_key_t * key_qual_map_p,
    dnx_field_key_qualifier_ffc_info_t * qualifier_ffc_info_p,
    dnx_field_key_attached_qual_info_t * qual_info_p)
{
    int ffc_idx;
    dnx_field_qual_map_in_key_t initial_key_qual_map;
    dnx_field_context_t cascaded_from_context_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(key_qual_map_p, _SHR_E_PARAM, "key_qual_map_p");
    SHR_NULL_CHECK(qualifier_ffc_info_p, _SHR_E_PARAM, "qualifier_ffc_info_p");
    SHR_NULL_CHECK(qual_info_p, _SHR_E_PARAM, "qual_info_p");

    /*
     * Update the initial_key_qual_map_p with pointers to initial key
     */
    initial_key_qual_map.qual_type = key_qual_map_p->qual_type;
    initial_key_qual_map.size = key_qual_map_p->size;

    /*
     * Get the cascaded_from_context_id - the context id in ipmf1 
     */
    SHR_IF_ERR_EXIT(dnx_field_context_cascaded_from_context_id_get
                    (unit, DNX_FIELD_STAGE_IPMF2, context_id, &cascaded_from_context_id));

    LOG_DEBUG_EX(BSL_LOG_MODULE, "Qualifier %s size %d context_id %d cascaded_from_context_id %d\r\n",
                 dnx_field_dnx_qual_text(unit, qualifier_ffc_info_p->qual_type),
                 key_qual_map_p->size, context_id, cascaded_from_context_id);

    /*
     * Prepare the list of the initial ffc's
     */
    for (ffc_idx = 0;
         ffc_idx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC_IN_QUAL
         && (qualifier_ffc_info_p->ffc_info[ffc_idx].ffc_initial.ffc.ffc_id != DNX_FIELD_FFC_ID_INVALID); ffc_idx++)
    {
        /*
         * initial_key_dest_offset is also the src offset for the ipmf2 ffc
         */
        initial_key_qual_map.lsb = qualifier_ffc_info_p->ffc_info[ffc_idx].ffc_initial.key_dest_offset;

        /*
         * Calculate the ffc instructions, considering initial ffc 
         * Note that the 'bit_range_offset_within_key' input parameter is irrelevant.
         */
        SHR_IF_ERR_EXIT(dnx_field_key_calc_ffc_instruction_values
                        (unit, qual_info_p, ffc_idx, FALSE, TRUE, &initial_key_qual_map, 0,
                         &(qualifier_ffc_info_p->ffc_info[ffc_idx].ffc_initial.ffc.ffc_instruction)));

        /*
         * Set the ffc instruction for initial key (ipmf1) for the cascaded_from context_id .
         * src offset will point to the ipmf1 pbus given by dnx_qual_info,
         * destination will point to the internally allocated placeholder on the initial key buffer.
         */
        SHR_IF_ERR_EXIT(dnx_field_key_single_ffc_hw_set
                        (unit, DNX_FIELD_STAGE_IPMF1, cascaded_from_context_id,
                         &(qualifier_ffc_info_p->ffc_info[ffc_idx].ffc_initial.ffc), qual_info_p));

        LOG_DEBUG_EX(BSL_LOG_MODULE,
                     "ffc_id[%d] %d belongs to initial key %d key_dest_index 0x%X\n",
                     ffc_idx, qualifier_ffc_info_p->ffc_info[ffc_idx].ffc_initial.ffc.ffc_id,
                     qualifier_ffc_info_p->ffc_info[ffc_idx].ffc_initial.ffc.key_id,
                     qualifier_ffc_info_p->ffc_info[ffc_idx].ffc_initial.ffc.ffc_instruction.key_offset);

    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  In case we process a qualifier that requires allocation and configuration of resources in IPMF1 -
 *  Configure the FFC's for the given qualifier
 * \param [in] unit          - Device Id
 * \param [in] field_stage   - For which Field stage to Configure HW
 * \param [in] context_id    - Context ID to configure FFC's for
 * \param [in] key_qual_map_p- Key qualifier map to configure. Represents
 * \param [in] qualifier_ffc_info_p   - info structure for the current qualifier
 * \param [in] qual_info_p   - Qualifiers Info
 * \param [in] bit_range_offset_within_key -
 *   Location of key template, containing this qualifier, within the key.
 *   For example, a key of 160 bits may host a few key templates:
 *     One of size, say, 21 bits, starting from bit0 of the key
 *     One of size, say, 32 bits, starting from bit21 of the key
 *     Etc.
 *   The 'key_offset' on 'ffc_instruction' is the physical location on the key
 *   and is, therefore, the sum of this input and the offset on the key template.
* * \return
*   shr_error_e             - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_key_ffc_hw_set(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_qual_map_in_key_t * key_qual_map_p,
    dnx_field_key_qualifier_ffc_info_t * qualifier_ffc_info_p,
    dnx_field_key_attached_qual_info_t * qual_info_p,
    uint32 bit_range_offset_within_key)
{
    int ffc_idx;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(key_qual_map_p, _SHR_E_PARAM, "key_qual_map_p");
    SHR_NULL_CHECK(qualifier_ffc_info_p, _SHR_E_PARAM, "qualifier_ffc_info_p");
    SHR_NULL_CHECK(qual_info_p, _SHR_E_PARAM, "qual_info_p");

    /*
     * Prepare the list of the initial ffc's
     */
    for (ffc_idx = 0;
         ffc_idx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC_IN_QUAL
         && (qualifier_ffc_info_p->ffc_info[ffc_idx].ffc.ffc_id != DNX_FIELD_FFC_ID_INVALID); ffc_idx++)
    {
        /** Calculate relevant values for the FFC instructions accordingly to
         *  the ffc index in qualifier size. */
        SHR_IF_ERR_EXIT(dnx_field_key_calc_ffc_instruction_values
                        (unit, qual_info_p, ffc_idx, FALSE, FALSE, key_qual_map_p,
                         bit_range_offset_within_key, &qualifier_ffc_info_p->ffc_info[ffc_idx].ffc.ffc_instruction));

        /*
         * Set the ffc instruction for initial key (ipmf1). src offset will point to the ipmf1 pbus given by
         * dnx_qual_info, destination will point to the internally allocated placeholder on the initial key buffer.
         */
        SHR_IF_ERR_EXIT(dnx_field_key_single_ffc_hw_set
                        (unit, field_stage, context_id, &(qualifier_ffc_info_p->ffc_info[ffc_idx].ffc), qual_info_p));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Configure the FFC's per context ID with given qualifier
 * \param [in] unit          - Device Id
 * \param [in] field_stage   - For which Field stage to Configure HW
 * \param [in] context_id    - Context ID to configure FFC's for
 * \param [in] key_qual_map_p- Key qualifier map to be configured
 * \param [in] dnx_qual_info_p   - Additional information for header qualifiers,
 *                                 to know from which layer the qualifier should be taken
 * \param [in] key_id         - Key Id for the the key being used for the TCAM lookup.
 *
 * \param [in] bit_range_offset_within_key -
 *   Location of key template, containing this qualifier, within the key.
 *   For example, a key of 160 bits may host a few key templates:
 *     One of size, say, 21 bits, starting from bit0 of the key
 *     One of size, say, 32 bits, starting from bit21 of the key
 *     Etc.
 *   The 'key_offset' on 'ffc_instruction' is the physical location on the key
 *   and is, therefore, the sum of this input and the offset on the key template.
*
* \return
*   shr_error_e             - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_key_ffc_set(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_qual_map_in_key_t * key_qual_map_p,
    dnx_field_key_attached_qual_info_t * dnx_qual_info_p,
    dbal_enum_value_field_field_key_e key_id,
    uint32 bit_range_offset_within_key)
{
    dnx_field_key_qualifier_ffc_info_t qualifier_ffc_info;
    uint8 per_key_ranges;
    uint8 final_ranges;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(key_qual_map_p, _SHR_E_PARAM, "key_qual_map_p");
    SHR_NULL_CHECK(dnx_qual_info_p, _SHR_E_PARAM, "dnx_qual_info_p");

    /** Limit the range of FFC according to the key.*/
    SHR_IF_ERR_EXIT(dnx_field_map_qual_key_to_ranges(unit, field_stage, key_id, &per_key_ranges));
    final_ranges = dnx_qual_info_p->ranges & per_key_ranges;

    /** Init FFC output arrays*/
    dnx_field_key_qualifier_ffc_info_init(&qualifier_ffc_info, 1);

    if ((DNX_QUAL_CLASS(key_qual_map_p->qual_type) == DNX_FIELD_QUAL_CLASS_SW)
        && (DNX_QUAL_ID(key_qual_map_p->qual_type) == DNX_FIELD_SW_QUAL_ALL_ZERO))
    {
        /**
         * If the qual is all zero no need to allocate FFC
         * just leave the place in the Key and by default it will be zero
         */
        SHR_EXIT();
    }

    qualifier_ffc_info.qual_type = key_qual_map_p->qual_type;
    /**
     * Allocate FFC for a specific qualifier for specific context ID .
     * Allocation is per qualifier, meaning that if the qualifier needs more then one ffc
     * then appropriate number of resources are allocated.
     */
    SHR_IF_ERR_EXIT_WITH_LOG(dnx_field_key_ffc_allocate_per_qualifier(unit, field_stage, context_id,
                                                                      dnx_qual_info_p->size, final_ranges,
                                                                      &qualifier_ffc_info),
                             "Failed to allocate FFCs for qualifier \"%s\" in stage \"%s\" for context %d.",
                             dnx_field_dnx_qual_text(unit, qualifier_ffc_info.qual_type), dnx_field_stage_text(unit,
                                                                                                               field_stage),
                             context_id);

    if ((dnx_qual_info_p->native_pbus == FALSE) && (field_stage == DNX_FIELD_STAGE_IPMF2))
    {
        /*
         * TBD: FFC Sharing: Add check if the qualifier already exists in the initial data. If so, skip the allocation &
         * configuration of the initial qual configuration, go directly to the configuration of the ipmf2
         */

        SHR_IF_ERR_EXIT(dnx_field_key_ffc_allocate_per_initial_qualifier(unit, context_id, key_qual_map_p->qual_type,
                                                                         dnx_qual_info_p, &qualifier_ffc_info));
        /*
         * Set the initial ffc (ipmf1) instruction.
         * */
        SHR_IF_ERR_EXIT(dnx_field_key_initial_key_ffc_set
                        (unit, context_id, key_qual_map_p, &qualifier_ffc_info, dnx_qual_info_p));
        /*
         * Set the ffc (ipmf2) instruction.
         * */
        SHR_IF_ERR_EXIT(dnx_field_key_ipmf2_ffc_set
                        (unit, context_id, key_qual_map_p, &qualifier_ffc_info, bit_range_offset_within_key));
    }
    else
    {
        /*
         * Set the hw instructions for the given qualifier
         */
        SHR_IF_ERR_EXIT(dnx_field_key_ffc_hw_set
                        (unit, field_stage, context_id, key_qual_map_p, &qualifier_ffc_info, dnx_qual_info_p,
                         bit_range_offset_within_key));
    }

    /** Configure the KEY_ID and the FFC  in HW. */
    SHR_IF_ERR_EXIT(dnx_field_key_kbr_qualifier_ffc_hw_add(unit, field_stage, context_id, key_id, &qualifier_ffc_info));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_key_const_quals_set(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_key_attached_qual_info_t * dnx_qual_info_p,
    dnx_field_qual_map_in_key_t * key_qual_map_p,
    dbal_enum_value_field_field_key_e key_id,
    uint32 bit_range_offset_within_key)
{
    /*
     * We use const input type and need to split the qualifier into multiple sub qualifiers.
     */
    unsigned int nof_sub_qual = 0;
    unsigned int sub_qual_index;
    dnx_field_qual_map_in_key_t key_qual_map_sub_qual[DNX_FIELD_KEY_MAX_CONST_SUB_QUAL];
    dnx_field_key_attached_qual_info_t dnx_qual_info_sub_qual[DNX_FIELD_KEY_MAX_CONST_SUB_QUAL];

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_key_qual_info_get_split_const
                    (unit, field_stage, key_qual_map_p,
                     dnx_qual_info_p, &key_qual_map_sub_qual[0], &dnx_qual_info_sub_qual[0], &nof_sub_qual));
    /*
     * We now break the qualifier into multiple sub qualifiers.
     */

    for (sub_qual_index = 0; sub_qual_index < nof_sub_qual; sub_qual_index++)
    {

        SHR_IF_ERR_EXIT(dnx_field_key_ffc_set(unit, field_stage, context_id,
                                              &key_qual_map_sub_qual[sub_qual_index],
                                              &(dnx_qual_info_sub_qual[sub_qual_index]),
                                              key_id, bit_range_offset_within_key));

        LOG_DEBUG_EX(BSL_LOG_MODULE, "subqual_index %d%s%s%s\n", sub_qual_index, EMPTY, EMPTY, EMPTY);
        LOG_DEBUG_EX(BSL_LOG_MODULE, "offset %d,  %d size %d,  ffc input_type %d\n",
                     dnx_qual_info_sub_qual[sub_qual_index].offset,
                     dnx_qual_info_p->offset,
                     dnx_qual_info_sub_qual[sub_qual_index].size, dnx_qual_info_sub_qual[sub_qual_index].input_type);
    }

exit:
    SHR_FUNC_EXIT;
}

/** See field_key.h */
shr_error_e
dnx_field_key_template_create(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_qual_t * qual_types_p,
    uint32 starting_offset,
    dnx_field_key_template_t * key_template_p)
{
    int qual_idx;
    dnx_field_qual_map_in_key_t *key_qual_map_p;

    int offset_in_key = starting_offset;
    int total_key_len = 0;
    uint32 qual_size = 0;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(qual_types_p, _SHR_E_PARAM, "qual_types_p");
    SHR_NULL_CHECK(key_template_p, _SHR_E_PARAM, "key_template_p");

    key_template_p->key_size_in_bits = 0;
    /*
     * Scan all the qualifiers configured for current Database
     */
    for (qual_idx = 0;
         (qual_idx < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)
         && (qual_types_p[qual_idx] != DNX_FIELD_QUAL_TYPE_INVALID); qual_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_size(unit, field_stage, qual_types_p[qual_idx], &qual_size));

        /*
         * Handle the given qualifier addition here 
         */
        key_qual_map_p = &(key_template_p->key_qual_map[qual_idx]);

        /** Set key template fields based on size, just concatenate all qualifiers one after the other */
        key_qual_map_p->lsb = offset_in_key;
        key_qual_map_p->size = qual_size;
        key_qual_map_p->qual_type = qual_types_p[qual_idx];
        offset_in_key = key_qual_map_p->size + offset_in_key;
        /** Sanity check: Verify that there is no wrap around.*/
        if (total_key_len + key_qual_map_p->size < total_key_len)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Key is too big, after qual number %d keyy size changed from %d to %d.\r\n",
                         qual_idx, total_key_len, total_key_len + key_qual_map_p->size);
        }
        total_key_len += key_qual_map_p->size;

        LOG_DEBUG_EX(BSL_LOG_MODULE, "Qual %s size %d, offset in key %d total_key_len %d\n",
                     dnx_field_dnx_qual_text(unit, key_qual_map_p->qual_type), qual_size, key_qual_map_p->lsb,
                     total_key_len);
    }

    /** The total_key_len that was calculated is the actual size of the key*/
    key_template_p->key_size_in_bits = total_key_len;

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Returns the lookup interface type for the given fg_type/key_id
* \param [in] unit             - unit
* \param [in] key_id           - The key id: composes the lookup interface together with the field group
* \param [in] fg_type          - The field group type: composes the lookup interface together with the key id
* \param [out] pmf_lookup_type - The returned lookup interface type for the given (fg_type/key_id) pair
* \return
*   shr_error_e             - Error Type
* \remark
*   * None
* \see
*   * dnx_field_key_lookup_change_state
*/
static shr_error_e
dnx_field_key_lookup_type_get(
    int unit,
    dbal_enum_value_field_field_key_e key_id,
    dnx_field_group_type_e fg_type,
    dbal_enum_value_field_pmf_lookup_type_e * pmf_lookup_type)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(pmf_lookup_type, _SHR_E_PARAM, "pmf_lookup_type");

    switch (fg_type)
    {
        case DNX_FIELD_GROUP_TYPE_TCAM:
        case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM:
        {
            switch (key_id)
            {
                case DBAL_ENUM_FVAL_FIELD_KEY_A:
                case DBAL_ENUM_FVAL_FIELD_KEY_F:
                    *pmf_lookup_type = DBAL_ENUM_FVAL_PMF_LOOKUP_TYPE_TCAM_0;
                    break;
                case DBAL_ENUM_FVAL_FIELD_KEY_B:
                case DBAL_ENUM_FVAL_FIELD_KEY_G:
                    *pmf_lookup_type = DBAL_ENUM_FVAL_PMF_LOOKUP_TYPE_TCAM_1;
                    break;
                case DBAL_ENUM_FVAL_FIELD_KEY_C:
                case DBAL_ENUM_FVAL_FIELD_KEY_H:
                    *pmf_lookup_type = DBAL_ENUM_FVAL_PMF_LOOKUP_TYPE_TCAM_2;
                    break;
                case DBAL_ENUM_FVAL_FIELD_KEY_D:
                case DBAL_ENUM_FVAL_FIELD_KEY_I:
                    *pmf_lookup_type = DBAL_ENUM_FVAL_PMF_LOOKUP_TYPE_TCAM_3;
                    break;
                default:
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid TCAM key usage: key_id: %d can't be used as TCAM\r\n", key_id);
            }
            break;
        }
        case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_MDB:
        {
            *pmf_lookup_type = DBAL_ENUM_FVAL_PMF_LOOKUP_TYPE_DIRECT;
            break;
        }
        case DNX_FIELD_GROUP_TYPE_EXEM:
        {
            *pmf_lookup_type = DBAL_ENUM_FVAL_PMF_LOOKUP_TYPE_EXEM;
            break;
        }
        case DNX_FIELD_GROUP_TYPE_STATE_TABLE:
        {
            *pmf_lookup_type = DBAL_ENUM_FVAL_PMF_LOOKUP_TYPE_STATE_TABLE;
            break;
        }
        case DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION:
        {
            /** For Direct Extraction (DE) an d Hash Stamp, no lookup interface is defined,
             *  so we set the lookup interface type to invalid.
             */
            *pmf_lookup_type = DBAL_NOF_ENUM_PMF_LOOKUP_TYPE_VALUES;
            break;
        }
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid type! %d\r\n", fg_type);
    }

exit:
    SHR_FUNC_EXIT;
}
/**
* \brief
*  Changes the state of the given lookup interface type (fg_type/key_id) in the given context id for
*  the given stage in the lookup enabler table (DBAL_TABLE_FIELD_IPMFx_LOOKUP_ENABLER) to the given
*  new_state.
* \param [in] unit        - Device ID
* \param [in] field_stage - The stage of the field
* \param [in] context_id  - The context id
* \param [in] key_id_p    - structure type of dnx_field_key_id_t, holds an array of allocated keys
*                           key id: composes the lookup interface together with the field group
* \param [in] fg_type     - The field group type: composes the lookup interface together with the key id
* \param [in] new_state   - The new state to set (TRUE to enable the lookup/FALSE to disable)
* \return
*   shr_error_e             - Error Type
* \remark
*   Note that the pair (fg_type/key_id) defines the lookup interface in case
*   fg_type is TCAM. In all other cases, fg_type alone defines the lookup interface
*   and key_id is ignored.
* \see
*   * None
*/
static shr_error_e
dnx_field_key_lookup_change_state_hw_set(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_key_id_t *key_id_p,
    dnx_field_group_type_e fg_type,
    uint8 new_state)
{
    uint32 entry_handle_id;
    uint32 key_idx;
    dbal_tables_e table_id = DBAL_NOF_TABLES;
    dbal_enum_value_field_pmf_lookup_type_e pmf_lookup_type = DBAL_NOF_ENUM_PMF_LOOKUP_TYPE_VALUES;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(key_id_p, _SHR_E_PARAM, "key_id_p");

    SHR_IF_ERR_EXIT(dnx_field_map_lookup_dbal_table_get(unit, field_stage, &table_id));
    /*
     * Not all stages have lookup enabler, exit silently when no lookup enabler table is found
     */
    if (table_id == DBAL_NOF_TABLES)
    {
        SHR_EXIT();
    }

    for (key_idx = 0; (key_idx < DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX) &&
         (key_id_p->id[key_idx] != DNX_FIELD_KEY_ID_INVALID); key_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_field_key_lookup_type_get(unit, key_id_p->id[key_idx], fg_type, &pmf_lookup_type));

        /*
         * Some fg types (e.g. direct extraction) have no lookup interface, exit silently in such case
         */
        if (pmf_lookup_type == DBAL_NOF_ENUM_PMF_LOOKUP_TYPE_VALUES)
        {
            SHR_EXIT();
        }

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_PMF_CTX_ID, context_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PMF_LOOKUP_TYPE, pmf_lookup_type);

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, new_state);

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_key_quals_over_key_split(
    int unit,
    dnx_field_group_type_e fg_type,
    uint32 offset_within_key,
    dnx_field_qual_map_in_key_t * key_qual_map_p,
    dnx_field_key_attached_qual_info_t * dnx_qual_info_p,
    dnx_field_qual_map_in_key_t * second_qual_map_p,
    dnx_field_key_attached_qual_info_t * second_dnx_qual_info_p,
    uint8 *was_split_p)
{
    uint32 max_single_key_size;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(key_qual_map_p, _SHR_E_PARAM, "key_qual_map_p");
    SHR_NULL_CHECK(dnx_qual_info_p, _SHR_E_PARAM, "dnx_qual_info_p");
    SHR_NULL_CHECK(second_qual_map_p, _SHR_E_PARAM, "second_qual_map_p");
    SHR_NULL_CHECK(second_dnx_qual_info_p, _SHR_E_PARAM, "second_dnx_qual_info_p");

    if (fg_type == DNX_FIELD_GROUP_TYPE_TCAM)
    {
        max_single_key_size = DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_KBR_SIZE;

        /*
         * If the staring offset is bigger then the KBr size - it is not a split case. it is simply a qualifier in the
         * second key. Return.
         */
        if (key_qual_map_p->lsb + offset_within_key >= max_single_key_size)
        {
            LOG_DEBUG_EX(BSL_LOG_MODULE,
                         "---------NO SPLIT: Qualifier %s fits the second key completely, offset in key %d qual_size %d lsb+offset %d \n",
                         dnx_field_dnx_qual_text(unit, key_qual_map_p->qual_type), key_qual_map_p->lsb,
                         key_qual_map_p->size, (key_qual_map_p->lsb + offset_within_key));

            SHR_FUNC_EXIT;
        }
        /*
         * Handle the split of the qualifier in case the qual_size plus starting offset is bigger then
         * the single KBR size for appropriate FG.
         * In this case we put the second qualifier on next physical key.
         * We update the map and qual_info accordingly for both 'first' and 'second' qualifier.
         */
        if (key_qual_map_p->lsb + offset_within_key + key_qual_map_p->size > max_single_key_size)
        {
            /*
             * If the qualifiers list requires key bigger than max_key_size (160b' for TCAM), handle it here.
             */

            sal_memcpy(second_qual_map_p, key_qual_map_p, sizeof(*second_qual_map_p));
            sal_memcpy(second_dnx_qual_info_p, dnx_qual_info_p, sizeof(*second_dnx_qual_info_p));

            /*
             * Update the second (split) qualifier map to fit the second key - to be the rest of the bits 
             */
            second_qual_map_p->size =
                key_qual_map_p->lsb + offset_within_key + key_qual_map_p->size - max_single_key_size;
            second_dnx_qual_info_p->size =
                key_qual_map_p->lsb + offset_within_key + key_qual_map_p->size - max_single_key_size;

            /*
             * Update the original map to fit the first key - to be the residue of the key 
             */
            key_qual_map_p->size = max_single_key_size - (key_qual_map_p->lsb + offset_within_key);
            dnx_qual_info_p->size = max_single_key_size - (key_qual_map_p->lsb + offset_within_key);

            /*
             * The lsb is the destination of the data inside the key.
             * We need to write the second qualifier adjacently after the first qualifier
             * In case the lsb overlaps the 160b' (to the second key) we will reset the offset to 0 in dnx_field_key_calc_ffc_instruction_values().
             * The offset_within_key is not added intentionally, it will also be handled in dnx_field_key_calc_ffc_instruction_values().
             */
            second_qual_map_p->lsb += key_qual_map_p->size;

            /*
             * The second_dnx_qual_info_p->offset is the source of the data inside the pbus. It should be updated to
             * represent the second qualifier.
             */
            second_dnx_qual_info_p->offset += key_qual_map_p->size;

            *was_split_p = TRUE;

            LOG_DEBUG_EX(BSL_LOG_MODULE,
                         "---------SPLIT first qual name %s, first  key lsb %d first  qual_size %d first  lsb+offset %d \n",
                         dnx_field_dnx_qual_text(unit, key_qual_map_p->qual_type), key_qual_map_p->lsb,
                         key_qual_map_p->size, (key_qual_map_p->lsb + offset_within_key));
            LOG_DEBUG_EX(BSL_LOG_MODULE,
                         "---------SPLIT second qual name %s, second key lsb %d second qual_size %d second lsb+offset %d \n",
                         dnx_field_dnx_qual_text(unit, second_qual_map_p->qual_type), second_qual_map_p->lsb,
                         second_dnx_qual_info_p->size, (second_qual_map_p->lsb + offset_within_key));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Configures the ffc/kbr according to key's qualifiers
 * \param [in] unit           - Device ID
 * \param [in] context_id     - PMF program ID to perform the operation for
 * \param [in] key_in_info_p  - Key in info look at dnx_field_key_attach_info_in_t
 * \param [in] key_id_p       - Structure that holds an array of Key Ids, the key being used for the TCAM lookup.
 * \param [in] bit_range_offset_within_key -
 *   Location of key template, containing this qualifier, within the key.
 *   For example, a key of 160 bits may host a few key templates:
 *     One of size, say, 21 bits, starting from bit0 of the key
 *     One of size, say, 32 bits, starting from bit21 of the key
 *     Etc.
 *   The 'key_offset' on 'ffc_instruction' is the physical location on the key
 *   and is, therefore, the sum of this input and the offset on the key template.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_field_key_quals_set(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_attach_info_in_t * key_in_info_p,
    dnx_field_key_id_t *key_id_p,
    uint32 bit_range_offset_within_key)
{
    uint32 qual_index = 0;
    dnx_field_qual_map_in_key_t *key_qual_map_p;
    dnx_field_qual_map_in_key_t split_qual_map;
    dnx_field_key_attached_qual_info_t dnx_qual_info;
    dnx_field_key_attached_qual_info_t split_dnx_qual_info;
    dnx_field_stage_e field_stage;
    uint8 was_split = FALSE;
    uint32 key_index = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(key_in_info_p, _SHR_E_PARAM, "key_in_info_p");
    SHR_NULL_CHECK(key_id_p, _SHR_E_PARAM, "key_id_p");

    dnx_field_key_attached_qual_info_t_init(unit, &dnx_qual_info);
    dnx_field_key_attached_qual_info_t_init(unit, &split_dnx_qual_info);

    field_stage = key_in_info_p->field_stage;

    key_qual_map_p = key_in_info_p->key_template.key_qual_map;

    for (qual_index = 0;
         (qual_index < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG) &&
         (key_qual_map_p[qual_index].qual_type != DNX_FIELD_QUAL_TYPE_INVALID); qual_index++)
    {
        SHR_IF_ERR_EXIT(dnx_field_key_qual_info_get(unit, field_stage, context_id, key_qual_map_p[qual_index].qual_type,
                                                    &key_in_info_p->qual_info[qual_index], &dnx_qual_info));

        was_split = FALSE;
        key_index =
            ((key_qual_map_p[qual_index].lsb + bit_range_offset_within_key) >=
             DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_KBR_SIZE) ? 1 : 0;

        LOG_DEBUG_EX(BSL_LOG_MODULE, "qual name %s, field_stage %s, offset in key %d key_index %d\n",
                     dnx_field_dnx_qual_text(unit, key_qual_map_p[qual_index].qual_type),
                     dnx_field_stage_text(unit, field_stage),
                     (key_qual_map_p[qual_index].lsb + bit_range_offset_within_key), key_index);
        /*
         * Split over-key qualifier should be here
         * */

        if (key_index == 0)
        {
            LOG_DEBUG_EX(BSL_LOG_MODULE,
                         "---------SET: POTENTIAL SPLIT: for qual name %s, offset in key %d qual_size %d lsb+offset %d \n",
                         dnx_field_dnx_qual_text(unit, key_qual_map_p[qual_index].qual_type),
                         key_qual_map_p[qual_index].lsb, key_qual_map_p[qual_index].size,
                         (key_qual_map_p[qual_index].lsb + bit_range_offset_within_key));

            SHR_IF_ERR_EXIT(dnx_field_key_quals_over_key_split(unit,
                                                               key_in_info_p->fg_type,
                                                               bit_range_offset_within_key,
                                                               &key_qual_map_p[qual_index], &dnx_qual_info,
                                                               &split_qual_map, &split_dnx_qual_info, &was_split));
        }
        if (dnx_qual_info.input_type == DNX_FIELD_INPUT_TYPE_CONST)
        {
            /*
             * The qualifier represents a constant qualifier.
             */
            SHR_IF_ERR_EXIT(dnx_field_key_const_quals_set
                            (unit, field_stage, context_id, &dnx_qual_info, &key_qual_map_p[qual_index],
                             key_id_p->id[key_index], bit_range_offset_within_key));
            if (was_split == TRUE)
            {
                key_index = 1;
                SHR_IF_ERR_EXIT(dnx_field_key_const_quals_set
                                (unit, field_stage, context_id, &split_dnx_qual_info, &split_qual_map,
                                 key_id_p->id[key_index], bit_range_offset_within_key));
            }
            continue;
        }
        SHR_IF_ERR_EXIT(dnx_field_key_ffc_set(unit, field_stage, context_id,
                                              &key_qual_map_p[qual_index], &dnx_qual_info,
                                              key_id_p->id[key_index], bit_range_offset_within_key));

        if (was_split == TRUE)
        {
            key_index = 1;
            SHR_IF_ERR_EXIT(dnx_field_key_ffc_set(unit, field_stage, context_id,
                                                  &split_qual_map, &split_dnx_qual_info,
                                                  key_id_p->id[key_index], bit_range_offset_within_key));
            LOG_DEBUG_EX(BSL_LOG_MODULE,
                         "SET Second SPLIT qual name %s, field_stage %s, offset in key %d qual_size %d\n",
                         dnx_field_dnx_qual_text(unit, split_qual_map.qual_type), dnx_field_stage_text(unit,
                                                                                                       field_stage),
                         split_qual_map.lsb, split_dnx_qual_info.size);
        }

        LOG_DEBUG_EX(BSL_LOG_MODULE, "offset %d,  %d size %d,  ffc input_type %d\n",
                     dnx_qual_info.offset, key_in_info_p->qual_info[qual_index].offset,
                     dnx_qual_info.size, dnx_qual_info.input_type);

    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  General Key handling:
 *  Attach context ID per KEY (I.e., allocate key per context
 *  and configure FFC and KBR accordingly)
 * \param [in] unit             -
 *   HW device ID
 * \param [in] context_id       -
 *   Context ID ID to perform the key attach for
 * \param [in] key_in_info_p    -
 *   Key in info (see  dnx_field_key_attach_info_in_t for details)
 * \param [out] key_id_p        -
 *   Pointer type of dnx_field_key_id_t, holds an array of allocated keys
 *   needed as input to FES/FEM, and for FFC configuration
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_field_key_general_key_attach(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_attach_info_in_t * key_in_info_p,
    dnx_field_key_id_t *key_id_p)
{
    dnx_field_stage_e field_stage;
    dnx_field_group_type_e fg_type;
    uint32 key_allocation_id;
    uint32 bit_range_offset_within_key;
    dnx_algo_field_key_flags_e flags = 0;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(key_in_info_p, _SHR_E_PARAM, "key_in_info_p");
    SHR_NULL_CHECK(key_id_p, _SHR_E_PARAM, "key_id_p");

    /*
     * Initializations.
     */
    /*
     * Initialize location of bit-range which is to be assigned for this
     * 'key_in_info_p->key_template' on the key, to '0' (LS bit of the key)
     */
    bit_range_offset_within_key = 0;
    field_stage = key_in_info_p->field_stage;
    fg_type = key_in_info_p->fg_type;
    /*
     * Sanity check: verify that only TCAM field groups from stage ipmf1 can use compare.
     */
    if (key_in_info_p->compare_id != DNX_FIELD_GROUP_COMPARE_ID_NONE &&
        (key_in_info_p->field_stage != DNX_FIELD_STAGE_IPMF1 ||
         (key_in_info_p->fg_type != DNX_FIELD_GROUP_TYPE_TCAM &&
          key_in_info_p->fg_type != DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Compare used (%d), but compare can only be used in iPMF1 for TCAM/direct TCAM results. "
                     "stage %s, fg_type %s, context ID %d.\r\n", key_in_info_p->compare_id, dnx_field_stage_text(unit,
                                                                                                                 key_in_info_p->field_stage),
                     dnx_field_group_type_e_get_name(key_in_info_p->fg_type), context_id);
    }
    /*
     * When we want FG-to-be-created result to be compared an initial key (initial.F/initial.G)
     * we need to FORCE allocation of key B/D respectively
     */
    switch (key_in_info_p->compare_id)
    {
        case DNX_FIELD_GROUP_COMPARE_ID_CMP_1:
            flags = flags | DNX_ALGO_FIELD_KEY_FLAG_WITH_ID;
            key_id_p->id[0] = DBAL_ENUM_FVAL_FIELD_KEY_B;
            break;
        case DNX_FIELD_GROUP_COMPARE_ID_CMP_2:
            flags = flags | DNX_ALGO_FIELD_KEY_FLAG_WITH_ID;
            key_id_p->id[0] = DBAL_ENUM_FVAL_FIELD_KEY_D;
            break;
        default:
            break;
    }

    /*
     * Allocate key_id for TCAM Field Group. Allocation is done by half key.
     */
    SHR_IF_ERR_EXIT(dnx_algo_field_key_id_allocate(unit, flags,
                                                   field_stage, fg_type, context_id,
                                                   key_in_info_p->key_length, key_id_p, &key_allocation_id));
    /*
     * If allocation is 'by half key' and the allocated half was 'MS half' then
     * set 'bit_range_offset_within_key' to the offset of the second half (80).
     * Otherwise, leave it as '0'.
     */
    if (key_id_p->key_part == DNX_FIELD_KEY_PART_TYPE_MSB_HALF)
    {
        bit_range_offset_within_key = DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_HALF;
    }
    /*
     * Set the app_db_id for allocated key
     */
    SHR_IF_ERR_EXIT(dnx_field_key_app_db_id_set(unit, field_stage, context_id, key_id_p, key_in_info_p->app_db_id));

    SHR_IF_ERR_EXIT(dnx_field_key_quals_set(unit, context_id, key_in_info_p, key_id_p, bit_range_offset_within_key));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Attach context ID per KEY (I.e., allocate key per context
 *  and configure FFC and KBR accordingly) for TCAM FG
 * \param [in] unit             -
 *   HW device ID
 * \param [in] context_id       -
 *   Context ID ID to perform the key attach for
 * \param [in] key_in_info_p    -
 *   Key in info (see  dnx_field_key_attach_info_in_t for details)
 * \param [out] key_id_p        -
 *   Pointer type of dnx_field_key_id_t, holds an array of allocated keys
 *   needed as input to FES/FEM, and for FFC configuration
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_field_key_tcam_key_attach(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_attach_info_in_t * key_in_info_p,
    dnx_field_key_id_t *key_id_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_key_general_key_attach(unit, context_id, key_in_info_p, key_id_p));

    SHR_IF_ERR_EXIT(dnx_field_key_lookup_change_state_hw_set
                    (unit, key_in_info_p->field_stage, context_id, key_id_p, key_in_info_p->fg_type, TRUE));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Attach context ID per KEY (I.e., allocate key per context
 *  and configure FFC and KBR accordingly) for DE FG
 * \param [in] unit             -
 *   HW device ID
 * \param [in] context_id       -
 *   Context ID ID to perform the key attach for
 * \param [in] key_in_info_p    -
 *   Key in info (see  dnx_field_key_attach_info_in_t for details)
 * \param [out] key_id_p        -
 *   Pointer type of dnx_field_key_id_t, holds an array of allocated keys
 *   needed as input to FES/FEM, and for FFC configuration
 *   Sub param of the key_id_p - bit_range     -
 *   \b As \b input - \n
 *     If bit_range is invalid then this parameter is ignored and allocation
 *     is done per 'half key'.
 *     If bit_range is valid then the boolean 'bit_range_p->bit_range_was_aligned'
 *     indicates whether to align the allocated 'key template' and the element
 *     'bit_range_p->bit_range_aligned_offset' indicates on which bit (in 'key template'
 *     to align).
 *   \b As \b output - \n
 *     This procedure loads bit_range with information regarding bit-range allocated on specified key.
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_field_key_de_key_attach(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_attach_info_in_t * key_in_info_p,
    dnx_field_key_id_t *key_id_p)
{
    dnx_field_stage_e field_stage;
    dnx_field_group_type_e fg_type;
    uint32 bit_range_offset_within_key;
    dnx_field_bit_range_t *bit_range_p = &key_id_p->bit_range;
    uint32 do_align;
    int16 aligned_bit;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(key_in_info_p, _SHR_E_PARAM, "key_in_info_p");
    SHR_NULL_CHECK(key_id_p, _SHR_E_PARAM, "key_id_p");

    /*
     * Initializations.
     */
    /*
     * Initialize location of bit-range which is to be assigned for this
     * 'key_in_info_p->key_template' on the key, to '0' (LS bit of the key)
     */
    bit_range_offset_within_key = 0;
    field_stage = key_in_info_p->field_stage;
    fg_type = key_in_info_p->fg_type;
    /*
     * Note that 'bit_range_p' may be NULL (in which case, it should be ignored).
     */

    if (key_in_info_p->key_template.key_size_in_bits == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Direct extraction field group with 0 bits in key, please use field group type "
                     "const instead.\r\n");
    }

    /*
     * Allocate key_id and, on it, some range of bits for PMF program. For Direct-Extraction field group
     * types then, allocate by bit-range. Temporarily, ignore the FEM issue.
     */
    do_align = bit_range_p->bit_range_was_aligned;
    aligned_bit = bit_range_p->bit_range_aligned_offset;
    SHR_IF_ERR_EXIT(dnx_field_key_bit_range_t_init(unit, bit_range_p));
    bit_range_p->bit_range_size = (uint8) (key_in_info_p->key_template.key_size_in_bits);
    SHR_IF_ERR_EXIT(dnx_algo_field_key_id_bit_range_allocate(unit, field_stage, fg_type, context_id,
                                                             bit_range_p->bit_range_size,
                                                             do_align, aligned_bit, key_id_p,
                                                             &bit_range_offset_within_key));
    bit_range_p->bit_range_valid = TRUE;
    bit_range_p->bit_range_offset = bit_range_offset_within_key;
    bit_range_p->bit_range_was_aligned = do_align;
    bit_range_p->bit_range_aligned_offset = aligned_bit;
    if (bit_range_p->bit_range_was_aligned)
    {
        LOG_DEBUG_EX(BSL_LOG_MODULE,
                     "Allocated key %c and, on it, a range of %d bits at offset %d. Aligned on bit %d.\r\n",
                     'A' + (int) (key_id_p->id[0]),
                     (int) (bit_range_p->bit_range_size),
                     (int) (bit_range_p->bit_range_offset), (int) bit_range_p->bit_range_aligned_offset);
    }
    else
    {
        LOG_DEBUG_EX(BSL_LOG_MODULE,
                     "Allocated key %c and, on it, a range of %d bits at offset %d. "
                     "No alignment required. %s\r\n",
                     'A' + (int) (key_id_p->id[0]),
                     (int) (bit_range_p->bit_range_size), (int) (bit_range_p->bit_range_offset), EMPTY);
    }

    SHR_IF_ERR_EXIT(dnx_field_key_quals_set(unit, context_id, key_in_info_p, key_id_p, bit_range_offset_within_key));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief
 *  Attach context ID per KEY (I.e., allocate key per context
 *  and configure FFC and KBR accordingly) for State  FG
 * \param [in] unit             -
 *   HW device ID
 * \param [in] context_id       -
 *   Context ID ID to perform the key attach for
 * \param [in] key_in_info_p    -
 *   Key in info (see  dnx_field_key_attach_info_in_t for details)
 * \param [out] key_id_p        -
 *   Pointer type of dnx_field_key_id_t, holds an array of allocated keys
 *   needed as input to FES/FEM, and for FFC configuration
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_field_key_exem_key_attach(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_attach_info_in_t * key_in_info_p,
    dnx_field_key_id_t *key_id_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_key_general_key_attach(unit, context_id, key_in_info_p, key_id_p));

    SHR_IF_ERR_EXIT(dnx_field_key_lookup_change_state_hw_set
                    (unit, key_in_info_p->field_stage, context_id, key_id_p, key_in_info_p->fg_type, TRUE));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Configure FFC for State table and enable lookup bit
 * \param [in] unit             -
 *   HW device ID
 * \param [in] context_id       -
 *   Context ID ID to perform the key attach for
 * \param [in] key_in_info_p    -
 *   Key in info (see  dnx_field_key_attach_info_in_t for details)
 * \param [out] key_id_p        -
 *   Pointer type of dnx_field_key_id_t, holds an array of allocated keys
 *   needed as input to FES/FEM, and for FFC configuration
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_field_key_state_table_key_attach(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_attach_info_in_t * key_in_info_p,
    dnx_field_key_id_t *key_id_p)
{
    uint32 bit_range_offset_within_key = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(key_in_info_p, _SHR_E_PARAM, "key_in_info_p");
    SHR_NULL_CHECK(key_id_p, _SHR_E_PARAM, "key_id_p");

    /*
     * Update the key_id allocation (reservation) into the context information that is stored for this field group 
     */
    SHR_IF_ERR_EXIT(dnx_algo_field_state_table_key_id_get(unit, key_id_p));
    /*
     * If allocation is 'by half key' and the allocated half was 'MS half' then
     * set 'bit_range_offset_within_key' to the offset of the second half (80).
     * Otherwise, leave it as '0'.
     */
    if (key_id_p->key_part == DNX_FIELD_KEY_PART_TYPE_MSB_HALF)
    {
        bit_range_offset_within_key = DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_HALF;
    }
    SHR_IF_ERR_EXIT(dnx_field_key_quals_set(unit, context_id, key_in_info_p, key_id_p, bit_range_offset_within_key));

    SHR_IF_ERR_EXIT(dnx_field_key_lookup_change_state_hw_set
                    (unit, DNX_FIELD_STAGE_IPMF1, context_id, key_id_p, DNX_FIELD_GROUP_TYPE_STATE_TABLE, TRUE));
exit:
    SHR_FUNC_EXIT;
}

/**
  * \brief
  *  Attach PMF context per KEY for special contexts (hash, state table etc)
  * \param [in] unit               - Device ID
  * \param [in] context_id         - Context ID ID to perform the key attach for
  * \param [in] key_in_info_p      - First key in info look at dnx_field_key_attach_info_in_t
  * \param [in] key_id_p          - Pointer type of dnx_field_key_id_t, holds an array of allocated keys
                                     needed as input to FES/FEM, and for FFC configuration
  * \return
  *   shr_error_e - Error Type
  * \remark
  *   * None
  * \see
  *   * None
  */
shr_error_e
dnx_field_key_context_feature_key_attach(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_attach_info_in_t * key_in_info_p,
    dnx_field_key_id_t *key_id_p)
{
    uint32 bit_range_offset_within_key;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(key_in_info_p, _SHR_E_PARAM, "key_in_info_p");
    SHR_NULL_CHECK(key_id_p, _SHR_E_PARAM, "key_id_p");
    bit_range_offset_within_key = 0;
    /*
     * For this procedure, key has already been pre-allocated by the feature and
     * is 'input'. Just adjust to 'MS half key' (although it is probably never going to happen.
     */
    /*
     * If allocation is 'by half key' and the allocated half was 'MS half' then
     * set 'bit_range_offset_within_key' to the offset of the second half (80).
     * Otherwise, leave it as '0'.
     */
    if (key_id_p->key_part == DNX_FIELD_KEY_PART_TYPE_MSB_HALF)
    {
        bit_range_offset_within_key = DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_HALF;
    }
    SHR_IF_ERR_EXIT(dnx_field_key_quals_set(unit, context_id, key_in_info_p, key_id_p, bit_range_offset_within_key));

exit:
    SHR_FUNC_EXIT;
}

/**
  * \brief
  *  In ePMF, every key must use at list one FFC. This function verifies that the allocated keys indeed have FFCs.
  * \param [in] unit               - Device ID
  * \param [in] context_id         - Context ID ID to perform the key attach for
  * \param [in] key_in_info_p      - The key attach info for the field group.
  * \param [in] key_id_p           - The keys allocated to the field group.
  * \return
  *   shr_error_e - Error Type
  * \remark
  *   * None
  * \see
  *   * None
  */
shr_error_e
dnx_field_key_one_ffc_verify(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_attach_info_in_t * key_in_info_p,
    dnx_field_key_id_t *key_id_p)
{
    int key_ndx;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(key_in_info_p, _SHR_E_PARAM, "key_in_info_p");
    SHR_NULL_CHECK(key_id_p, _SHR_E_PARAM, "key_id_p");
    /*
     * Verify that an ePMF lookup has at least one FFC.
     */
    if (key_in_info_p->field_stage == DNX_FIELD_STAGE_EPMF)
    {
        for (key_ndx = 0; key_ndx < DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX; key_ndx++)
        {
            if (key_id_p->id[key_ndx] != DNX_FIELD_KEY_ID_INVALID)
            {
                uint32 ffc_id[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC] = { 0 };

                SHR_IF_ERR_EXIT(dnx_field_key_kbr_ffc_array_hw_get
                                (unit, key_in_info_p->field_stage, context_id, key_id_p->id[key_ndx], ffc_id));
                if (ffc_id[0] == DNX_FIELD_FFC_ID_INVALID)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "An ePMF lookup (non const field group) must have at least one FFC "
                                 "(at least one qualifier that is not const zero). "
                                 "Field group type %s, context %d stage %s.\n",
                                 dnx_field_group_type_e_get_name(key_in_info_p->fg_type), context_id,
                                 dnx_field_stage_text(unit, key_in_info_p->field_stage));
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_key_attach(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_attach_info_in_t * key_in_info_p,
    dnx_field_key_id_t *key_id_p)
{
    int key_ndx;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(key_in_info_p, _SHR_E_PARAM, "key_in_info_p");
    SHR_NULL_CHECK(key_id_p, _SHR_E_PARAM, "key_id_p");

    /*
     * Since key_id_p is both input and output, we cannot initialize it, and must rely on it being initialized
     * beforehand. For that reason we perform the following sanity check:
     */
    for (key_ndx = 0; key_ndx < DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX; key_ndx++)
    {
        if (key_id_p->id[key_ndx] != DNX_FIELD_KEY_ID_INVALID)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "key_id_p is not initialized, key %d isn't invalid. "
                         "stage %s, fg_type %s context ID %d.\n",
                         key_ndx, dnx_field_stage_text(unit, key_in_info_p->field_stage),
                         dnx_field_group_type_e_get_name(key_in_info_p->fg_type), context_id);
        }
    }

    switch (key_in_info_p->fg_type)
    {
        case DNX_FIELD_GROUP_TYPE_TCAM:
        case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM:
        {
            SHR_IF_ERR_EXIT(dnx_field_key_tcam_key_attach(unit, context_id, key_in_info_p, key_id_p));
            break;
        }
        case DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION:
        {
            SHR_IF_ERR_EXIT(dnx_field_key_de_key_attach(unit, context_id, key_in_info_p, key_id_p));
            break;
        }
        case DNX_FIELD_GROUP_TYPE_EXEM:
        {
            SHR_IF_ERR_EXIT(dnx_field_key_exem_key_attach(unit, context_id, key_in_info_p, key_id_p));
            break;
        }
        case DNX_FIELD_GROUP_TYPE_STATE_TABLE:
            SHR_IF_ERR_EXIT(dnx_field_key_state_table_key_attach(unit, context_id, key_in_info_p, key_id_p));
            break;
        case DNX_FIELD_GROUP_TYPE_KBP:
        case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_MDB:
        {
            SHR_ERR_EXIT(_SHR_E_DISABLED,
                         "Should not get here. Probably an internal error. Stage %s FG type: %s Attach to Context (%d)\r\n",
                         dnx_field_stage_text(unit, key_in_info_p->field_stage),
                         dnx_field_group_type_e_get_name(key_in_info_p->fg_type), context_id);
            break;
        }
        case DNX_FIELD_GROUP_TYPE_CONST:
        {
            /** Do nothing.*/
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, " Field Group type = %d (%s) Not supported\n", key_in_info_p->fg_type,
                         dnx_field_group_type_e_get_name(key_in_info_p->fg_type));
            break;
        }
    }

    /*
     * Verify that an ePMF lookup has at least one FFC.
     */
    SHR_INVOKE_VERIFY_DNX(dnx_field_key_one_ffc_verify(unit, context_id, key_in_info_p, key_id_p));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief
 *  Check if the given ffc belongs to the given qualifier by checking if
 *  the ffc destination (key_offset) is in the range of the qualifier given by the key template (lsb ~ (lsb+size))
 *  Note that more then 1 ffc may belong to this range. This could happen in case the qualifier is bigger
 *  than 32'b; then there will be few ffc's allocated for it. So, in order to locate all ffc's we search for ffc's
 *  that fit in the range.
 *
 * \param [in] unit          - Device Id
 * \param [in] key_qual_map_p- Key qualifier map of the given qualifier
 * \param [in] ffc_instruction_info_p - Instruction Info for current ffc
 * \param [in] bit_range_offset_within_key -
 *   Location of key template, containing this qualifier, within the key.
 *   For example, a key of 160 bits may host a few key templates:
 *     One of size, say, 21 bits, starting from bit0 of the key
 *     One of size, say, 32 bits, starting from bit21 of the key
 *     Etc.
 *   The 'key_offset' on 'ffc_instruction' is the physical location on the key
 *   and is, therefore, the sum of this input and the offset on the key template.
 * \param [out] is_relative_p - return TRUE if the FFC is in range of the qualifier
 * * \return
 *   shr_error_e             - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_field_key_ffc_to_qual_is_relative(
    int unit,
    dnx_field_qual_map_in_key_t * key_qual_map_p,
    dnx_field_key_ffc_instruction_info_t * ffc_instruction_info_p,
    uint32 bit_range_offset_within_key,
    uint32 *is_relative_p)
{
    /*
     *
     * This procedure compares the actual location of the FFC (stored in the 'instruction',
     * in HW, and retrieved, here, using 'ffc_instruction_info_p') and the calculated
     * location on the key. The later is built from the location on the 'key template'
     * plus the 'offset' of the actual bit-range assigned for this qualifier.
     * This procedure is called from one place, in the code:
     *
     * dnx_field_key_qual_ffc_get()
     *   which uses the output of dnx_field_key_single_ffc_hw_get()
     *   to call this procedure.
     *
     * Both need to pass the 'offset'
     */
    uint32 qual_range_min;
    uint32 qual_range_max;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(key_qual_map_p, _SHR_E_PARAM, "key_qual_map_p");
    SHR_NULL_CHECK(ffc_instruction_info_p, _SHR_E_PARAM, "ffc_instruction_info_p");
    SHR_NULL_CHECK(is_relative_p, _SHR_E_PARAM, "is_relative_p");

    qual_range_min = (key_qual_map_p->lsb + bit_range_offset_within_key) % DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_KBR_SIZE;
    qual_range_max =
        (key_qual_map_p->lsb + bit_range_offset_within_key) % DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_KBR_SIZE +
        key_qual_map_p->size;

    *is_relative_p = FALSE;

    /*
     *  Check if the given ffc belongs to the given qualifier by checking if
     *  the ffc destination (key_offset) is in the range of the qualifier given by the key template (lsb ~ (lsb+size))
     */
    if ((ffc_instruction_info_p->key_offset >= qual_range_min) && (ffc_instruction_info_p->key_offset < qual_range_max))
    {
        *is_relative_p = TRUE;
    }
    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "qual_range_min %d qual_range_max %d ffc_instruction_info_p->key_offset %d key_qual_map_p->lsb %d\n",
                 qual_range_min, qual_range_max, ffc_instruction_info_p->key_offset, key_qual_map_p->lsb);

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Check if the given ffc belongs to the given qualifier by checking if
*  the ffc destination (key_offset) is in the range of the qualifier given by the key template (lsb ~ (lsb+size))
* \param [in] unit                      - Device Id
* \param [in] inital_key_id             - Key_id should be taken into consideration because it affects the offset(src)
*                                         of the ipmf2 ffc in pbus
* \param [in] ipmf2_ffc_instruction_info_p  - ipmf2 ffc instruction info. the offset in this information
*                                             will be compared with the key_offset (destination) of the ipmf1 ffc instruction
* \param [out] ipmf1_ffc_instruction_info_p - ipmf1 ffc instruction info. the key_offsest in this information
*                                             will be compared with the offset of the ipmf1 ffc instruction
* \param [out] is_relative_p - return TRUE if the FFC is in range of the qualifier
* * \return
*   shr_error_e             - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_key_ffc_to_initial_ffc_is_relative(
    int unit,
    dbal_enum_value_field_field_key_e inital_key_id,
    dnx_field_key_ffc_instruction_info_t * ipmf2_ffc_instruction_info_p,
    dnx_field_key_ffc_instruction_info_t * ipmf1_ffc_instruction_info_p,
    uint32 *is_relative_p)
{
    dnx_field_qual_t internal_ipmf2_qual_type;
    dnx_field_ipmf2_qual_e ipmf2_base_qual;
    int mapped_offset;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(ipmf2_ffc_instruction_info_p, _SHR_E_PARAM, "ipmf2_ffc_instruction_info_p");
    SHR_NULL_CHECK(ipmf1_ffc_instruction_info_p, _SHR_E_PARAM, "ipmf1_ffc_instruction_info_p");
    SHR_NULL_CHECK(is_relative_p, _SHR_E_PARAM, "is_relative_p");

    *is_relative_p = FALSE;

    /*
     * Convert the initial Key-Id into meta2 qualifier for ipmf2
     * */
    SHR_IF_ERR_EXIT(dnx_field_key_qual_initial_key_to_meta2_qual_get(unit, inital_key_id, &ipmf2_base_qual));

    internal_ipmf2_qual_type = DNX_QUAL(DNX_FIELD_QUAL_CLASS_META2, DNX_FIELD_STAGE_IPMF2, ipmf2_base_qual);
    SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_offset
                    (unit, DNX_FIELD_STAGE_IPMF2, internal_ipmf2_qual_type, &mapped_offset));

    /*
     *  Check if the given ffc belongs to the given qualifier by checking if
     *  the ipmf1-ffc destination (key_offset) is equals to the ipmf2-ffc source (offset)
     */
    if (((ipmf2_ffc_instruction_info_p->offset) ==
         ipmf1_ffc_instruction_info_p->key_offset + mapped_offset)
        && (ipmf2_ffc_instruction_info_p->size == ipmf1_ffc_instruction_info_p->size))
    {
        *is_relative_p = TRUE;
        LOG_DEBUG_EX(BSL_LOG_MODULE,
                     "ipmf2_ffc_instruction_info_p->offset 0x%X "
                     "ipmf1_ffc_instruction_info_p->key_offset 0x%X "
                     "mapped_offset 0x%X "
                     "size 0x%X\n",
                     ipmf2_ffc_instruction_info_p->offset,
                     ipmf1_ffc_instruction_info_p->key_offset, mapped_offset, ipmf2_ffc_instruction_info_p->size);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Copy the instruction from src to dest
* \param [in] unit              - Device Id
* \param [in] src_p             - Source Instruction - information that we want to copy to destination
* \param [out] dest_p           - Destination Instruction - output information
*
* \return
*   shr_error_e             - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_key_ffc_copy_instruction(
    int unit,
    dnx_field_key_ffc_instruction_info_t * src_p,
    dnx_field_key_ffc_instruction_info_t * dest_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(src_p, _SHR_E_PARAM, "src_p");
    SHR_NULL_CHECK(dest_p, _SHR_E_PARAM, "dest_p");

    dest_p->ffc_type = src_p->ffc_type;
    dest_p->offset = src_p->offset;
    dest_p->field_index = src_p->field_index;
    dest_p->key_offset = src_p->key_offset;
    dest_p->size = src_p->size;
    dest_p->full_instruction = src_p->full_instruction;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Get the FFC's for a given qualifier, per context ID with given qualifier
 * \param [in] unit                  - Device Id
 * \param [in] field_stage           - For which Field stage to receive the HW information
 * \param [in] context_id            - Context ID
 * \param [in] key_id                - Key id to get its info
 * \param [in] key_qual_map_p        - Key qualifier map represents the qualifier an it's location in the key
 * \param [in] bit_range_offset_within_key -
 *   Location of key template, containing this qualifier, within the key.
 *   For example, a key of 160 bits may host a few key templates:
 *     One of size, say, 21 bits, starting from bit0 of the key
 *     One of size, say, 32 bits, starting from bit21 of the key
 *     Etc.
 *   The 'key_offset' on 'ffc_instruction' is the physical location on the key
 *   and is, therefore, the sum of this input and the offset on the key template.
 * \param [out] qualifier_ffc_info_p - Qualifier information
 * \param [in,out] output_ffc_index_p - start index for the ffc index. Used to concatenate the ffc's that belong to a qualifier
 *                                   in case the qualifier was split between different keys (for example tcam double key).
 *                                   In all other cases should be set to 0.
 * \return
 *   shr_error_e             - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_field_key_qual_ffc_get_internal(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dbal_enum_value_field_field_key_e key_id,
    dnx_field_qual_map_in_key_t * key_qual_map_p,
    uint32 bit_range_offset_within_key,
    dnx_field_key_qualifier_ffc_info_t * qualifier_ffc_info_p,
    uint32 *output_ffc_index_p)
{
    uint32 ffc_idx;
    uint32 is_relative = 0;
    uint32 ffc_id[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC];
    dnx_field_key_ffc_instruction_info_t local_instruction;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(key_qual_map_p, _SHR_E_PARAM, "key_qual_map_p");
    SHR_NULL_CHECK(qualifier_ffc_info_p, _SHR_E_PARAM, "qualifier_ffc_info_p");

    SHR_IF_ERR_EXIT(dnx_field_key_kbr_ffc_array_hw_get(unit, field_stage, context_id, key_id, ffc_id));

    qualifier_ffc_info_p->qual_type = key_qual_map_p->qual_type;

    for (ffc_idx = 0;
         (ffc_idx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC) && (ffc_id[ffc_idx] != DNX_FIELD_FFC_ID_INVALID)
         && (*output_ffc_index_p < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC_IN_QUAL); ffc_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_field_key_single_ffc_hw_get
                        (unit, field_stage, context_id, ffc_id[ffc_idx], &local_instruction));

        SHR_IF_ERR_EXIT(dnx_field_key_ffc_to_qual_is_relative
                        (unit, key_qual_map_p, &local_instruction, bit_range_offset_within_key, &is_relative));

        if (is_relative)
        {
            qualifier_ffc_info_p->ffc_info[*output_ffc_index_p].ffc.ffc_id = ffc_id[ffc_idx];

            SHR_IF_ERR_EXIT(dnx_field_key_ffc_copy_instruction
                            (unit, &local_instruction,
                             &(qualifier_ffc_info_p->ffc_info[*output_ffc_index_p].ffc.ffc_instruction)));
            
            qualifier_ffc_info_p->ffc_info[*output_ffc_index_p].ffc.key_id = key_id;

            (*output_ffc_index_p)++;

            LOG_DEBUG_EX(BSL_LOG_MODULE,
                         "ffc_id[%d] %d belongs to qual %s output_ffc_index %d\n",
                         ffc_idx, ffc_id[ffc_idx], dnx_field_dnx_qual_text(unit, key_qual_map_p->qual_type),
                         (*output_ffc_index_p - 1));
        }
    }
exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief
 *  Get the FFC's for a given qualifier, per context ID with given qualifier
 * \param [in] unit                  - Device Id
 * \param [in] field_stage           - For which Field stage to receive the HW information
 * \param [in] context_id            - Context ID
 * \param [in] fg_type               - The field group type: composes the lookup interface together with the key id
 * \param [in] key_id_p              - Key id for this field group inside the cpontext_id
 * \param [in] key_qual_map_p        - Key qualifier map represents the qualifier an it's location in the key
 * \param [in] bit_range_offset_within_key -
 *   Location of key template, containing this qualifier, within the key.
 *   For example, a key of 160 bits may host a few key templates:
 *     One of size, say, 21 bits, starting from bit0 of the key
 *     One of size, say, 32 bits, starting from bit21 of the key
 *     Etc.
 *   The 'key_offset' on 'ffc_instruction' is the physical location on the key
 *   and is, therefore, the sum of this input and the offset on the key template.
 * \param [out] qualifier_ffc_info_p - Qualifier information
 *
 * \return
 *   shr_error_e             - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_field_key_qual_ffc_get(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_group_type_e fg_type,
    dnx_field_key_id_t *key_id_p,
    dnx_field_qual_map_in_key_t * key_qual_map_p,
    uint32 bit_range_offset_within_key,
    dnx_field_key_qualifier_ffc_info_t * qualifier_ffc_info_p)
{
    dnx_field_qual_map_in_key_t second_qual_map;
    dnx_field_key_attached_qual_info_t dummy_split_qual_info;
    dnx_field_key_attached_qual_info_t dummy_dnx_qual_info;
    uint32 key_index = 0;
    uint8 qual_was_split = FALSE;
    uint32 ffc_idx = 0;
    SHR_FUNC_INIT_VARS(unit);

    key_index =
        ((key_qual_map_p->lsb + bit_range_offset_within_key) >= DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_KBR_SIZE) ? 1 : 0;

    LOG_DEBUG_EX(BSL_LOG_MODULE, "qual name %s, field_stage %s, offset in key %d key_index %d\n",
                 dnx_field_dnx_qual_text(unit, key_qual_map_p->qual_type), dnx_field_stage_text(unit, field_stage),
                 (key_qual_map_p->lsb + bit_range_offset_within_key), key_index);

    if (key_index == 0)
    {
        LOG_DEBUG_EX(BSL_LOG_MODULE,
                     "---------GET: POTENTIAL SPLIT: for qual name %s, offset in key %d qual_size %d lsb+offset %d \n",
                     dnx_field_dnx_qual_text(unit, key_qual_map_p->qual_type), key_qual_map_p->lsb,
                     key_qual_map_p->size, (key_qual_map_p->lsb + bit_range_offset_within_key));

        /*
         * Qualifier could be potentially split. 
         */
        SHR_IF_ERR_EXIT(dnx_field_key_quals_over_key_split(unit, fg_type,
                                                           bit_range_offset_within_key, key_qual_map_p,
                                                           &dummy_dnx_qual_info, &second_qual_map,
                                                           &dummy_split_qual_info, &qual_was_split));

        if (qual_was_split == TRUE)
        {
            SHR_IF_ERR_EXIT(dnx_field_key_qual_ffc_get_internal(unit, field_stage, context_id, key_id_p->id[1],
                                                                &second_qual_map,
                                                                bit_range_offset_within_key,
                                                                qualifier_ffc_info_p, &ffc_idx));
            LOG_DEBUG_EX(BSL_LOG_MODULE,
                         "---------GET: The SECOND SPLIT: for qual name %s, offset in key %d qual_size %d ffc_idx %d \n",
                         dnx_field_dnx_qual_text(unit, second_qual_map.qual_type), second_qual_map.lsb,
                         second_qual_map.size, ffc_idx);

        }
    }

    SHR_IF_ERR_EXIT(dnx_field_key_qual_ffc_get_internal(unit, field_stage, context_id, key_id_p->id[key_index],
                                                        key_qual_map_p,
                                                        bit_range_offset_within_key, qualifier_ffc_info_p, &ffc_idx));

    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "qual %s number of ffc's %d key_index %d size %d\n",
                 dnx_field_dnx_qual_text(unit, key_qual_map_p->qual_type), ffc_idx, key_index, key_qual_map_p->size);

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_key_qual_initial_ffc_get(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_ffc_hw_info_t * ipmf2_ffc_p,
    dnx_field_key_initial_ffc_info_t * ipmf1_ffc_initial_p)
{
    uint32 ffc_idx;

    uint32 ffc_id[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC];
    uint32 is_relative = 0;
    dnx_field_key_ffc_instruction_info_t initial_instr_temp;
    dbal_enum_value_field_field_key_e initial_key_id;
    dnx_field_context_t cascaded_from_context_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(ipmf2_ffc_p, _SHR_E_PARAM, "ipmf2_ffc_p");
    SHR_NULL_CHECK(ipmf1_ffc_initial_p, _SHR_E_PARAM, "ipmf1_ffc_initial_p");

    SHR_IF_ERR_EXIT(dnx_field_context_cascaded_from_context_id_get
                    (unit, DNX_FIELD_STAGE_IPMF2, context_id, &cascaded_from_context_id));

    LOG_DEBUG_EX(BSL_LOG_MODULE, " context_id %d cascaded_from_context_id %d %s%s\r\n", context_id,
                 cascaded_from_context_id, EMPTY, EMPTY);

    if (ipmf2_ffc_p->ffc_id != DNX_FIELD_FFC_ID_INVALID)
    {
        /** Go through all keys */
        for (initial_key_id = DBAL_ENUM_FVAL_FIELD_KEY_F; initial_key_id <= DBAL_ENUM_FVAL_FIELD_KEY_J;
             initial_key_id++)
        {
            SHR_IF_ERR_EXIT(dnx_field_key_kbr_ffc_array_hw_get
                            (unit, DNX_FIELD_STAGE_IPMF1, cascaded_from_context_id, initial_key_id, ffc_id));

            for (ffc_idx = 0;
                 (ffc_idx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC) && (ffc_id[ffc_idx] != DNX_FIELD_FFC_ID_INVALID);
                 ffc_idx++)
            {
                /*
                 * Read the instruction from the cascaded_from context id
                 */
                SHR_IF_ERR_EXIT(dnx_field_key_single_ffc_hw_get
                                (unit, DNX_FIELD_STAGE_IPMF1, cascaded_from_context_id, ffc_id[ffc_idx],
                                 &initial_instr_temp));

                SHR_IF_ERR_EXIT(dnx_field_key_ffc_to_initial_ffc_is_relative
                                (unit, initial_key_id, &(ipmf2_ffc_p->ffc_instruction), &initial_instr_temp,
                                 &is_relative));
                if (is_relative)
                {

                    ipmf1_ffc_initial_p->ffc.ffc_id = ffc_id[ffc_idx];
                    SHR_IF_ERR_EXIT(dnx_field_key_ffc_copy_instruction
                                    (unit, &initial_instr_temp, &(ipmf1_ffc_initial_p->ffc.ffc_instruction)));

                    ipmf1_ffc_initial_p->ffc.key_id = initial_key_id;
                    ipmf1_ffc_initial_p->key_dest_offset = initial_instr_temp.key_offset;

                    LOG_DEBUG_EX(BSL_LOG_MODULE,
                                 "ffc_id[%d] %d belongs to initial key %d key_dest_index 0x%X\n",
                                 ffc_idx, ffc_id[ffc_idx], ipmf1_ffc_initial_p->ffc.key_id,
                                 ipmf1_ffc_initial_p->key_dest_offset);
                    /*
                     * Break the loop - we found the match 
                     */
                    SHR_EXIT();
                }
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * See field_key.h
 */
shr_error_e
dnx_field_key_single_initial_ffc_get(
    int unit,
    dnx_field_context_t context_id,
    dbal_enum_value_field_field_key_e initial_key_id,
    uint32 ffc_id,
    dnx_field_key_initial_ffc_info_t * ipmf1_ffc_initial_p)
{
    dnx_field_key_ffc_instruction_info_t initial_instr_temp;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(ipmf1_ffc_initial_p, _SHR_E_PARAM, "ipmf1_ffc_initial_p");

    /** Read the instruction from the context id. */
    SHR_IF_ERR_EXIT(dnx_field_key_single_ffc_hw_get
                    (unit, DNX_FIELD_STAGE_IPMF1, context_id, ffc_id, &initial_instr_temp));

    ipmf1_ffc_initial_p->ffc.ffc_id = ffc_id;
    SHR_IF_ERR_EXIT(dnx_field_key_ffc_copy_instruction
                    (unit, &initial_instr_temp, &(ipmf1_ffc_initial_p->ffc.ffc_instruction)));

    ipmf1_ffc_initial_p->ffc.key_id = initial_key_id;
    ipmf1_ffc_initial_p->key_dest_offset = initial_instr_temp.key_offset;

exit:
    SHR_FUNC_EXIT;
}

/**
 * See field_key.h
 */
shr_error_e
dnx_field_key_get(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_id_t *key_id_p,
    dnx_field_key_attach_info_in_t * key_out_info_p,
    dnx_field_key_group_ffc_info_t * group_ffc_info_p)
{
    uint32 qual_idx;
    uint32 ffc_idx;
    uint32 bit_range_offset_within_key;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(key_id_p, _SHR_E_PARAM, "key_id_p");
    SHR_NULL_CHECK(key_out_info_p, _SHR_E_PARAM, "key_out_info_p");
    SHR_NULL_CHECK(group_ffc_info_p, _SHR_E_PARAM, "group_ffc_info_p");

    dnx_field_key_qualifier_ffc_info_init(group_ffc_info_p->key.qualifier_ffc_info,
                                          DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG);
    /*
     * Get offset of allocated bit range. If 'bit-range' feature was activated then 'bit_range_valid' should
     * be TRUE and 'bit_range->bit_range_offset' should be used.
     * Otherwise, 'half key' allocation was employed. So either LS half (or full key) was allocated
     * ('bit_range_offset_within_key' should be set to '0') or MS half was allocated ('bit_range_offset_within_key'
     * should be set to '80'). See dnx_field_key_part_type_e
     */
    if (key_id_p->bit_range.bit_range_valid)
    {
        bit_range_offset_within_key = key_id_p->bit_range.bit_range_offset;
    }
    else
    {
        if (key_id_p->key_part == DNX_FIELD_KEY_PART_TYPE_MSB_HALF)
        {
            bit_range_offset_within_key = DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_HALF;
        }
        else
        {
            bit_range_offset_within_key = 0;
        }
    }

    /*
     * Get the app_db_id 
     * The app_db_id is only applicable for TCAM, EXEM and direct table Field Groups. 
     * Thus, skip the get for direct extraction.
     * Also, dnx_field_key_hash_detach() uses this function with field group type invalid, so also don't get APP_DB_ID
     * for invalid field group type.
     */
    if ((key_out_info_p->fg_type == DNX_FIELD_GROUP_TYPE_TCAM)
        || (key_out_info_p->fg_type == DNX_FIELD_GROUP_TYPE_EXEM))
    {
        SHR_IF_ERR_EXIT(dnx_field_key_app_db_id_hw_get
                        (unit, key_out_info_p->field_stage, context_id, key_id_p->id[0],
                         (uint32 *) &(key_out_info_p->app_db_id)));
    }

    for (qual_idx = 0; (qual_idx < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG) &&
         (key_out_info_p->key_template.key_qual_map[qual_idx].qual_type != DNX_FIELD_QUAL_TYPE_INVALID); qual_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_field_key_qual_ffc_get
                        (unit, key_out_info_p->field_stage, context_id, key_out_info_p->fg_type, key_id_p,
                         &(key_out_info_p->key_template.key_qual_map[qual_idx]), bit_range_offset_within_key,
                         &(group_ffc_info_p->key.qualifier_ffc_info[qual_idx])));

        /** Get Information for the initial ffc's, if applicable*/
        if (key_out_info_p->field_stage == DNX_FIELD_STAGE_IPMF2)
        {
            /** If the qualifier is valid - go through it's ffc's*/
            if (DNX_QUAL_CLASS(key_out_info_p->key_template.key_qual_map[qual_idx].qual_type) !=
                DNX_FIELD_QUAL_CLASS_META2)
            {
                /** go through the list of ffc's for the given qualifier and find the related ffc in the cascaded_from context */
                for (ffc_idx = 0; (ffc_idx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC_IN_QUAL) &&
                     (group_ffc_info_p->key.qualifier_ffc_info[qual_idx].ffc_info[ffc_idx].ffc.ffc_id !=
                      DNX_FIELD_FFC_ID_INVALID); ffc_idx++)
                {
                    SHR_IF_ERR_EXIT(dnx_field_key_qual_initial_ffc_get(unit, context_id,
                                                                       &(group_ffc_info_p->key.
                                                                         qualifier_ffc_info[qual_idx].ffc_info[ffc_idx].
                                                                         ffc),
                                                                       &(group_ffc_info_p->key.
                                                                         qualifier_ffc_info[qual_idx].ffc_info[ffc_idx].
                                                                         ffc_initial)));
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Delete the FFC configuration per context ID with given Keys
* \param [in] unit                  - Device Id
* \param [in] field_stage           - For which Field stage to Configure HW
* \param [in] context_id            - PMF context to configure FFC's for
* \param [in] group_ffc_info_p  - Information of the FFC's related to a specific qualifier
* * \return
*   shr_error_e             - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_key_ffc_deallocate(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_key_group_ffc_info_t * group_ffc_info_p)
{
    uint32 ffc_idx = 0;
    uint32 qual_idx;
    dnx_field_context_t cascaded_from_context_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(group_ffc_info_p, _SHR_E_PARAM, "group_ffc_info_p");

    if (field_stage == DNX_FIELD_STAGE_IPMF2)
    {
        SHR_IF_ERR_EXIT(dnx_field_context_cascaded_from_context_id_get
                        (unit, field_stage, context_id, &cascaded_from_context_id));

        LOG_DEBUG_EX(BSL_LOG_MODULE, " context_id %d cascaded_from_context_id %d %s%s\r\n", context_id,
                     cascaded_from_context_id, EMPTY, EMPTY);

        for (qual_idx = 0; (qual_idx < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG); qual_idx++)
        {
            /*
             * If the initial ffc is valid - go through ffc's and deallocate them
             */
            for (ffc_idx = 0; (ffc_idx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC_IN_QUAL) &&
                 (group_ffc_info_p->key.qualifier_ffc_info[qual_idx].ffc_info[ffc_idx].ffc_initial.ffc.ffc_id !=
                  DNX_FIELD_FFC_ID_INVALID); ffc_idx++)
            {
                /*
                 * go through the list of ffc's for the given qualifier and deallocate the valid initial ffc's
                 */

                /** Free the occupation bmp */
                SHR_IF_ERR_EXIT(dnx_field_key_ipmf1_initial_key_occupation_bmp_deallocate
                                (unit, cascaded_from_context_id,
                                 &(group_ffc_info_p->key.qualifier_ffc_info[qual_idx].ffc_info[ffc_idx].ffc_initial)));

                /** Free the ffc resource */
                SHR_IF_ERR_EXIT(dnx_field_key_single_ffc_deallocate
                                (unit, DNX_FIELD_STAGE_IPMF1, cascaded_from_context_id,
                                 group_ffc_info_p->key.qualifier_ffc_info[qual_idx].ffc_info[ffc_idx].ffc_initial.
                                 ffc.ffc_id));

                /** Clean the instructions of the selected ffc */
                SHR_IF_ERR_EXIT(dnx_field_key_qual_single_ffc_instruction_clear
                                (unit, DNX_FIELD_STAGE_IPMF1, cascaded_from_context_id,
                                 group_ffc_info_p->key.qualifier_ffc_info[qual_idx].ffc_info[ffc_idx].ffc_initial.
                                 ffc.ffc_id));
            }
        }
    }

    /** Deallocate FFC id's*/
    for (qual_idx = 0; qual_idx < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG; qual_idx++)
    {
        for (ffc_idx = 0; ffc_idx < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC_IN_QUAL &&
             group_ffc_info_p->key.qualifier_ffc_info[qual_idx].ffc_info[ffc_idx].ffc.ffc_id !=
             DNX_FIELD_FFC_ID_INVALID; ffc_idx++)
        {
            SHR_IF_ERR_EXIT(dnx_field_key_single_ffc_deallocate
                            (unit, field_stage, context_id,
                             group_ffc_info_p->key.qualifier_ffc_info[qual_idx].ffc_info[ffc_idx].ffc.ffc_id));

                /** Clean the instructions of the selected ffc's */
            SHR_IF_ERR_EXIT(dnx_field_key_qual_single_ffc_instruction_clear
                            (unit, field_stage, context_id,
                             group_ffc_info_p->key.qualifier_ffc_info[qual_idx].ffc_info[ffc_idx].ffc.ffc_id));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Detach key information:
 *      1. deallocate KBRs
 *      2. deallocate FFCs that were allocated for context ID to key at input
 * \param [in] unit        - Device Id
 * \param [in] context_id  - Context ID to detach
 * \param [in] key_in_info_p - key information
 * \param [in] key_id_p    - structure that holds the array of KBRs to deallocate
 *   Sub param of the key_id_p - bit_range     -
 *   Pointer to structure that holds all information regarding the bit-range allocated on key
 *   (specified in 'key_id', above) for this Context ID.

 *
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_field_key_kbr_ffc_detach(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_attach_info_in_t * key_in_info_p,
    dnx_field_key_id_t *key_id_p)
{
    dnx_field_key_group_ffc_info_t group_ffc_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(key_in_info_p, _SHR_E_PARAM, "key_in_info_p");
    SHR_NULL_CHECK(key_id_p, _SHR_E_PARAM, "key_id_p");

    /** Get the information  for FFC's related to the given key */
    SHR_IF_ERR_EXIT(dnx_field_key_get(unit, context_id, key_id_p, key_in_info_p, &group_ffc_info));

    /*
     * Remove the FFCs that listed in the group_ffc_info from the KBR FFC bitmap.
     */
    SHR_IF_ERR_EXIT(dnx_field_key_kbr_delete(unit, key_in_info_p->field_stage, context_id, &group_ffc_info));

    /** De-allocate all ffc's*/
    SHR_IF_ERR_EXIT(dnx_field_key_ffc_deallocate(unit, key_in_info_p->field_stage, context_id, &group_ffc_info));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Detach all context ID key information for DE FG, i.e. deallocate all resources that were allocated for context ID to key at input
 * All relevant information about the Key can be read from the HW
 * \param [in] unit        - Device Id

 * \param [in] context_id  - Context ID to detach
 * \param [in] key_in_info_p - key information
 * \param [in] key_id_p    -
 *   Structure that holds the array of Key Id to deallocate
 *   Sub param of the key_id_p - bit_range     -
 *   Structure that holds all information regarding the bit-range allocated on key
 *   (specified in 'key_id', above) for this Context ID.

 *
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_field_key_de_key_detach(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_attach_info_in_t * key_in_info_p,
    dnx_field_key_id_t *key_id_p)
{
    
    uint32 key_id_temp_handle = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(key_in_info_p, _SHR_E_PARAM, "key_in_info_p");
    SHR_NULL_CHECK(key_id_p, _SHR_E_PARAM, "key_id_p");

    /*
     * Deallocate all physical resources for the given [key, template]
     */
    SHR_IF_ERR_EXIT(dnx_field_key_kbr_ffc_detach(unit, context_id, key_in_info_p, key_id_p));

    /** Deallocate key id*/
    if (key_id_p->id[0] != DNX_FIELD_KEY_ID_INVALID)
    {
        CONSTRUCT_KEY_ALLOCATION_ID(key_id_temp_handle, DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION,
                                    key_in_info_p->field_stage, context_id, key_id_p->id[0], key_id_p->id[1],
                                    key_id_p->key_part);
        if (key_id_p->bit_range.bit_range_valid != TRUE)
        {
            SHR_IF_ERR_EXIT(dnx_algo_field_key_id_free(unit, key_id_temp_handle));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_algo_field_key_id_bit_range_free
                            (unit, key_in_info_p->field_stage, DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION, context_id,
                             *key_id_p, key_id_p->bit_range.bit_range_offset));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Detach all context ID key information for TCAM FG, i.e. deallocate all resources that were allocated for context ID to key at input
 * All relevant information about the Key can be read from the HW
 * \param [in] unit        - Device Id
 * \param [in] context_id  - Context ID to detach
 * \param [in] key_in_info_p - Key information
 * \param [in] key_id_p    -
 *   Structure that holds the array of Key Id to deallocate
 *   Sub param of the key_id_p - bit_range     -
 *   Structure that holds all information regarding the bit-range allocated on key
 *   (specified in 'key_id', above) for this Context ID.
 *
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_field_key_tcam_key_detach(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_attach_info_in_t * key_in_info_p,
    dnx_field_key_id_t *key_id_p)
{
    uint32 key_id_temp_handle = 0;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Deallocate all physical resources for the given [key, template]
     */
    SHR_IF_ERR_EXIT(dnx_field_key_kbr_ffc_detach(unit, context_id, key_in_info_p, key_id_p));

    /** Reset KBR app_db_id*/
    SHR_IF_ERR_EXIT(dnx_field_key_app_db_id_set(unit, key_in_info_p->field_stage, context_id,
                                                key_id_p, DNX_FIELD_KEY_KBR_APP_DP_ID_DEFAULT));

    /** Deallocate key id*/
    if (key_id_p->id[0] != DNX_FIELD_KEY_ID_INVALID)
    {
        CONSTRUCT_KEY_ALLOCATION_ID(key_id_temp_handle, key_in_info_p->fg_type, key_in_info_p->field_stage,
                                    context_id, key_id_p->id[0], key_id_p->id[1], key_id_p->key_part);

        SHR_IF_ERR_EXIT(dnx_algo_field_key_id_free(unit, key_id_temp_handle));
    }

    SHR_IF_ERR_EXIT(dnx_field_key_lookup_change_state_hw_set
                    (unit, key_in_info_p->field_stage, context_id, key_id_p, DNX_FIELD_GROUP_TYPE_TCAM, FALSE));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Detach all context ID key information for State table FG, i.e. deallocate all resources that were allocated
 *  for context ID to key at input
 * All relevant information about the Key can be read from the HW
 * \param [in] unit        - Device Id
 * \param [in] context_id  - Context ID to detach
 * \param [in] key_in_info_p - key information
 * \param [in] key_id_p    -
 *   Structure that holds the array of Key Id to deallocate
 *   Sub param of the key_id_p - bit_range     -
 *   Structure that holds all information regarding the bit-range allocated on key
 *   (specified in 'key_id', above) for this Context ID.
 *
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_field_key_state_table_key_detach(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_attach_info_in_t * key_in_info_p,
    dnx_field_key_id_t *key_id_p)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Deallocate all physical resources for the given [key, template]
     */
    SHR_IF_ERR_EXIT(dnx_field_key_kbr_ffc_detach(unit, context_id, key_in_info_p, key_id_p));

    SHR_IF_ERR_EXIT(dnx_field_key_lookup_change_state_hw_set
                    (unit, key_in_info_p->field_stage, context_id, key_id_p, DNX_FIELD_GROUP_TYPE_STATE_TABLE, FALSE));

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *  Detach all context ID key information for EXEM FG, i.e. deallocate all resources that were allocated for context ID to key at input
 * All relevant information about the Key can be read from the HW
 * \param [in] unit        - Device Id
 * \param [in] context_id  - Context ID to detach
 * \param [in] key_in_info_p   - Key info
 * \param [in] key_id_p    -
 *   Structure that holds the array of Key Id to deallocate
 *   Sub param of the key_id_p - bit_range     -
 *   Structure that holds all information regarding the bit-range allocated on key
 *   (specified in 'key_id', above) for this Context ID.

 *
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_field_key_exem_key_detach(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_attach_info_in_t * key_in_info_p,
    dnx_field_key_id_t *key_id_p)
{

    uint32 key_id_temp_handle = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(key_id_p, _SHR_E_PARAM, "key_id_p");
    SHR_NULL_CHECK(key_in_info_p, _SHR_E_PARAM, "key_in_info_p");
    /*
     * Deallocate all physical resources for the given [key, template]
     */
    SHR_IF_ERR_EXIT(dnx_field_key_kbr_ffc_detach(unit, context_id, key_in_info_p, key_id_p));

    /** Reset KBR app_db_id*/
    SHR_IF_ERR_EXIT(dnx_field_key_app_db_id_set(unit, key_in_info_p->field_stage, context_id,
                                                key_id_p, DNX_FIELD_KEY_KBR_APP_DP_ID_DEFAULT));

    /** Deallocate key id*/
    if (key_id_p->id[0] != DNX_FIELD_KEY_ID_INVALID)
    {
        CONSTRUCT_KEY_ALLOCATION_ID(key_id_temp_handle, DNX_FIELD_GROUP_TYPE_EXEM, key_in_info_p->field_stage,
                                    context_id, key_id_p->id[0], key_id_p->id[1], key_id_p->key_part);
        SHR_IF_ERR_EXIT(dnx_algo_field_key_id_free(unit, key_id_temp_handle));
    }

    SHR_IF_ERR_EXIT(dnx_field_key_lookup_change_state_hw_set
                    (unit, key_in_info_p->field_stage, context_id, key_id_p, DNX_FIELD_GROUP_TYPE_EXEM, FALSE));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_key_context_feature_key_detach(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_context_t context_id,
    dnx_field_key_id_t *key_id_p,
    dnx_field_key_template_t * key_template_p)
{
    dnx_field_key_attach_info_in_t key_in_info;
    SHR_FUNC_INIT_VARS(unit);

    dnx_field_key_attach_info_in_t_init(unit, &key_in_info);

    SHR_NULL_CHECK(key_id_p, _SHR_E_PARAM, "key_id_p");
    SHR_NULL_CHECK(key_template_p, _SHR_E_PARAM, "key_template_p");

    key_in_info.field_stage = stage;
    sal_memcpy(&key_in_info.key_template, key_template_p, sizeof(key_in_info.key_template));

    /*
     * For this procedure, key has already been pre-allocated by the feature and
     * is 'input'. Just adjust 'bit_range' to be empty because it is not used in such cases.
     */
    /*
     * Deallocate all physical resources for the given [key, template]
     */
    SHR_IF_ERR_EXIT(dnx_field_key_kbr_ffc_detach(unit, context_id, &key_in_info, key_id_p));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_key_detach(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_attach_info_in_t * key_in_info_p,
    dnx_field_key_id_t *key_id_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(key_id_p, _SHR_E_PARAM, "key_id_p");
    SHR_NULL_CHECK(key_in_info_p, _SHR_E_PARAM, "key_in_info_p");

    /*
     *  Detach all context ID key information, according to the FG type.
     */
    switch (key_in_info_p->fg_type)
    {
        case DNX_FIELD_GROUP_TYPE_TCAM:
        case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM:
            SHR_IF_ERR_EXIT(dnx_field_key_tcam_key_detach(unit, context_id, key_in_info_p, key_id_p));
            break;
        case DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION:
            SHR_IF_ERR_EXIT(dnx_field_key_de_key_detach(unit, context_id, key_in_info_p, key_id_p));
            break;
        case DNX_FIELD_GROUP_TYPE_EXEM:
            SHR_IF_ERR_EXIT(dnx_field_key_exem_key_detach(unit, context_id, key_in_info_p, key_id_p));
            break;
        case DNX_FIELD_GROUP_TYPE_STATE_TABLE:
            SHR_IF_ERR_EXIT(dnx_field_key_state_table_key_detach(unit, context_id, key_in_info_p, key_id_p));
            break;
        case DNX_FIELD_GROUP_TYPE_KBP:
        case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_MDB:
            SHR_ERR_EXIT(_SHR_E_DISABLED,
                         "Should not get here. Probably an internal error. Stage %s FG type: type %s Detach from Context (%d)\r\n",
                         dnx_field_stage_text(unit, key_in_info_p->field_stage),
                         dnx_field_group_type_e_get_name(key_in_info_p->fg_type), context_id);
            break;
        case DNX_FIELD_GROUP_TYPE_CONST:
            /** Do nothing.*/
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Stage %s FG type: type %s Detach from Context (%d) not supported. Unknown field group type.\r\n",
                         dnx_field_stage_text(unit, key_in_info_p->field_stage),
                         dnx_field_group_type_e_get_name(key_in_info_p->fg_type), context_id);
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * Procedures related to SW STATE for 'key'
 * {
 */

/**
 * See field_key.h
 */
shr_error_e
dnx_field_key_sw_state_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_field_key_alloc_sw_state_init(unit));
exit:
    SHR_FUNC_EXIT;
}
/*
 * }
 */
/*
 * Utility procedures to be used by, say, diag_dnx_field and ctest_dnx_field
 * {
 */
/**
 * See field_key.h
 */
shr_error_e
dnx_field_key_is_any_key_occupied(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_group_type_e group_type,
    int bit_range_only,
    int *none_occupied_p)
{
    uint32 ii;
    uint8 available_key;
    uint8 half_key_nof, key_nof;
    dnx_field_key_alloc_detail_t dnx_field_key_alloc_detail[DNX_FIELD_KEY_NUM_HALVES_IN_KEY];
    uint8 half_key, half_key_index;
    /**
     * Number of keys available for this stage.
     */
    uint8 nof_available_keys_on_stage;
    /**
     * Indicates the first key that is available for this stage.
     * 'uint8' is used here, to save swstate space, but the implied
     * type is dbal_enum_value_field_field_key_e
     */
    uint8 first_available_key_on_stage;
    /**
     * Boolean. Indicates whether the combination (field_stage,group_type)
     * supports the 'bit-range' feature.
     * See dnx_field_key_bit_range_detail_t.
     */
    uint8 supports_bitmap_allocation;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(none_occupied_p, _SHR_E_PARAM, "none_occupied_p");

    *none_occupied_p = TRUE;
    DNX_FIELD_STAGE_VERIFY(field_stage);
    DNX_FIELD_CONTEXT_ID_VERIFY(unit, context_id);

    SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_STAGE_INFO.
                    nof_available_keys_on_stage.get(unit, field_stage, &nof_available_keys_on_stage));
    SHR_IF_ERR_EXIT(KEYS_AVAILABILITY_STAGE_INFO.
                    first_available_key_on_stage.get(unit, field_stage, &first_available_key_on_stage));
    LOG_DEBUG_EX(BSL_LOG_MODULE, "nof_available_keys_on_stage : %d%s%s%s\r\n", nof_available_keys_on_stage, EMPTY,
                 EMPTY, EMPTY);
    LOG_DEBUG_EX(BSL_LOG_MODULE, "first_available_key_on_stage: %d (KEY %c) %s%s\r\n", first_available_key_on_stage,
                 ('A' + first_available_key_on_stage), EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(KEY_STAGE_PROG_OCCUPATION.key_occupation_nof.get(unit, field_stage, context_id, &(half_key_nof)));
    if (!(half_key_nof))
    {
        LOG_DEBUG_EX(BSL_LOG_MODULE, "field_stage (%s) has no keys to allocate from. %s%s%s\r\n",
                     dnx_field_stage_text(unit, field_stage), EMPTY, EMPTY, EMPTY);
        SHR_EXIT();
    }
    /*
     * Convert 'number of half keys' to 'number of keys'.
     */
    key_nof = half_key_nof / DNX_FIELD_KEY_NUM_HALVES_IN_KEY;
    if (half_key_nof != (key_nof * DNX_FIELD_KEY_NUM_HALVES_IN_KEY))
    {
        /*
         * The number of half keys must be twice the the number of keys!
         */
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Number of half keys (%d) is not equal to double the number of keys (%d).\r\n",
                     half_key_nof, key_nof);
    }
    available_key = first_available_key_on_stage;
    for (ii = 0; ii < key_nof; ii++)
    {
        for (half_key_index = 0; half_key_index < DNX_FIELD_KEY_NUM_HALVES_IN_KEY; half_key_index++)
        {
            half_key = DNX_FIELD_KEY_NUM_HALVES_IN_KEY * ii + half_key_index;
            SHR_IF_ERR_EXIT(KEY_OCCUPATION.key_occupation_state.get(unit, field_stage, context_id, half_key,
                                                                    &(dnx_field_key_alloc_detail
                                                                      [half_key_index].key_occupation_state)));
            SHR_IF_ERR_EXIT(KEY_OCCUPATION.key_field_group_type.get(unit, field_stage, context_id, half_key,
                                                                    &(dnx_field_key_alloc_detail
                                                                      [half_key_index].key_field_group_type)));
            if (!(dnx_field_key_alloc_detail[half_key_index].key_occupation_state == DNX_FIELD_KEY_OCCUPY_NONE))
            {
                if (group_type == DNX_FIELD_GROUP_TYPE_INVALID)
                {
                    /*
                     * If 'group_type' is ignored then a valid entry has been found.
                     */
                    *none_occupied_p = FALSE;
                    SHR_EXIT();
                }
                else
                {
                    /*
                     * If 'group_type' matches the required value then a valid entry has been found.
                     */
                    if (dnx_field_key_alloc_detail[half_key_index].key_field_group_type == group_type)
                    {
                        unsigned int found;
                        /*
                         * A half key was found, which is occupied.
                         * If caller did not specify 'bit_range_only' then we are done:
                         *   Mark that some half key is occupied and exit.
                         */
                        found = FALSE;
                        if (bit_range_only == FALSE)
                        {
                            found = TRUE;
                        }
                        else
                        {
                            /*
                             * Enter if we are looking for half keys which
                             * are both occupied and comply to the 'bit-range' feature.
                             * The list of these keys is in swstate, in 'key_on_stage_supports_bitmap_allocation[]'
                             */
                            SHR_IF_ERR_EXIT(KEY_STAGE_SUPPORTS_BITMAP_ALLOCATION.get
                                            (unit, field_stage, (first_available_key_on_stage + ii),
                                             &supports_bitmap_allocation));
                            if (supports_bitmap_allocation)
                            {
                                found = TRUE;
                            }
                        }
                        if (found)
                        {
                            *none_occupied_p = FALSE;
                            SHR_EXIT();
                        }
                    }
                }
            }
        }
        available_key++;
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * See field_key.h
 */
shr_error_e
dnx_field_key_id_t_init(
    int unit,
    dnx_field_key_id_t *key_p)
{
    int key_index;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(key_p, _SHR_E_PARAM, "key_p");

    for (key_index = 0; key_index < (sizeof(key_p->id) / sizeof(key_p->id[0])); key_index++)
    {
        key_p->id[key_index] = DNX_FIELD_KEY_ID_INVALID;
    }
    key_p->key_part = DNX_FIELD_KEY_PART_TYPE_INVALID;

    SHR_IF_ERR_EXIT(dnx_field_key_bit_range_t_init(unit, &key_p->bit_range));
exit:
    SHR_FUNC_EXIT;
}
/**
 * See field_key.h
 */
shr_error_e
dnx_field_key_bit_range_t_init(
    int unit,
    dnx_field_bit_range_t * bit_range_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(bit_range_p, _SHR_E_PARAM, "bit_range_p");

    sal_memset(bit_range_p, 0, sizeof(*bit_range_p));
    /*
     * Not necessary but cleaner.
     */
    bit_range_p->bit_range_valid = FALSE;
    bit_range_p->bit_range_was_aligned = FALSE;

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_algo_field_key_ipmf1_initial_key_occupation_bmp_with_key_id_allocate(
    int unit,
    int alloc_flags,
    dnx_field_context_t context_id,
    dbal_enum_value_field_field_key_e key_id,
    uint32 size,
    uint32 *initial_offset_in_key_p)
{
    SHR_FUNC_INIT_VARS(unit);

    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "Allocate bmp in initial key: context_id (%d) key_id %d size %d initial_offset_in_key_p %d\r\n",
                 context_id, key_id, size, *initial_offset_in_key_p);

    switch (key_id)
    {
        case DBAL_ENUM_FVAL_FIELD_KEY_F:
            SHR_IF_ERR_EXIT(dnx_algo_field_key_ipmf1_initial_key_f_occupation_bmp_allocate
                            (unit, SW_STATE_ALGO_RES_ALLOCATE_WITH_ID, context_id, size,
                             (int *) initial_offset_in_key_p));
            break;
        case DBAL_ENUM_FVAL_FIELD_KEY_G:
            SHR_IF_ERR_EXIT(dnx_algo_field_key_ipmf1_initial_key_g_occupation_bmp_allocate
                            (unit, SW_STATE_ALGO_RES_ALLOCATE_WITH_ID, context_id, size,
                             (int *) initial_offset_in_key_p));
            break;
        case DBAL_ENUM_FVAL_FIELD_KEY_H:
            SHR_IF_ERR_EXIT(dnx_algo_field_key_ipmf1_initial_key_h_occupation_bmp_allocate
                            (unit, SW_STATE_ALGO_RES_ALLOCATE_WITH_ID, context_id, size,
                             (int *) initial_offset_in_key_p));
            break;
        case DBAL_ENUM_FVAL_FIELD_KEY_I:
            SHR_IF_ERR_EXIT(dnx_algo_field_key_ipmf1_initial_key_i_occupation_bmp_allocate
                            (unit, SW_STATE_ALGO_RES_ALLOCATE_WITH_ID, context_id, size,
                             (int *) initial_offset_in_key_p));
            break;
        case DBAL_ENUM_FVAL_FIELD_KEY_J:
            SHR_IF_ERR_EXIT(dnx_algo_field_key_ipmf1_initial_key_j_occupation_bmp_allocate
                            (unit, SW_STATE_ALGO_RES_ALLOCATE_WITH_ID, context_id, size,
                             (int *) initial_offset_in_key_p));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Illegal initial key_id =%d , valid values are [%d-%d]\n", key_id,
                         DBAL_ENUM_FVAL_FIELD_KEY_F, DBAL_ENUM_FVAL_FIELD_KEY_J);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_algo_field_key_ipmf1_initial_key_occupation_bmp_with_key_id_free(
    int unit,
    int alloc_flags,
    dnx_field_context_t context_id,
    dbal_enum_value_field_field_key_e key_id,
    uint32 size,
    uint32 initial_offset_in_key)
{
    SHR_FUNC_INIT_VARS(unit);

    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "Free bmp in initial key: context_id (%d) key_id %d size %d initial_offset_in_key %d\r\n", context_id,
                 key_id, size, initial_offset_in_key);

    switch (key_id)
    {
        case DBAL_ENUM_FVAL_FIELD_KEY_F:
            SHR_IF_ERR_EXIT(dnx_algo_field_key_ipmf1_initial_key_f_occupation_bmp_deallocate(unit,
                                                                                             context_id, size,
                                                                                             initial_offset_in_key));
            break;
        case DBAL_ENUM_FVAL_FIELD_KEY_G:
            SHR_IF_ERR_EXIT(dnx_algo_field_key_ipmf1_initial_key_g_occupation_bmp_deallocate(unit,
                                                                                             context_id, size,
                                                                                             initial_offset_in_key));
            break;
        case DBAL_ENUM_FVAL_FIELD_KEY_H:
            SHR_IF_ERR_EXIT(dnx_algo_field_key_ipmf1_initial_key_h_occupation_bmp_deallocate(unit,
                                                                                             context_id, size,
                                                                                             initial_offset_in_key));
            break;
        case DBAL_ENUM_FVAL_FIELD_KEY_I:
            SHR_IF_ERR_EXIT(dnx_algo_field_key_ipmf1_initial_key_i_occupation_bmp_deallocate(unit,
                                                                                             context_id, size,
                                                                                             initial_offset_in_key));
            break;
        case DBAL_ENUM_FVAL_FIELD_KEY_J:
            SHR_IF_ERR_EXIT(dnx_algo_field_key_ipmf1_initial_key_j_occupation_bmp_deallocate(unit,
                                                                                             context_id, size,
                                                                                             initial_offset_in_key));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Illegal initial key_id =%d , valid values are [%d-%d]\n", key_id,
                         DBAL_ENUM_FVAL_FIELD_KEY_F, DBAL_ENUM_FVAL_FIELD_KEY_J);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See field_key.h
 */
shr_error_e
dnx_field_key_compare_mode_single_resources_reserve(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_id_t *initial_key_id_p,
    dnx_field_key_id_t *tcam_key_id_p)
{
    uint32 key_idx;
    uint32 compare_key_starting_offset = DNX_FIELD_KEY_COMPARE_STARTING_OFFSET(unit);

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(initial_key_id_p, _SHR_E_PARAM, "initial_key_id_p");
    SHR_NULL_CHECK(tcam_key_id_p, _SHR_E_PARAM, "tcam_key_id_p");

    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "Reserve keys for Compare context %d mode Single key_id %d Tcam key_id %d %s\n",
                 context_id, initial_key_id_p->id[0], tcam_key_id_p->id[0], EMPTY);

    /*
     * Compare mode uses <compare_size> MSB bits out of keys in initial_key_id_p.
     * For that matter we will reserve those bits in the required key.
     */
    for (key_idx = 0; (key_idx < DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX) &&
         (initial_key_id_p->id[key_idx] != DNX_FIELD_KEY_ID_INVALID); key_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_algo_field_key_ipmf1_initial_key_occupation_bmp_with_key_id_allocate
                        (unit, SW_STATE_ALGO_RES_ALLOCATE_WITH_ID, context_id, initial_key_id_p->id[key_idx],
                         dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->compare_key_size,
                         &compare_key_starting_offset));
    }

    /*
     * Compare mode uses the keys in tcam_key_id_p TCAM result buffer.
     * Therefore, we reserve those keys so that they can be used only by
     * compare FGs.
     */
    for (key_idx = 0; (key_idx < DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX) &&
         (tcam_key_id_p->id[key_idx] != DNX_FIELD_KEY_ID_INVALID); key_idx++)
    {
        

    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_key_compare_mode_single_resources_free(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_id_t *initial_key_id_p,
    dnx_field_key_id_t *tcam_key_id_p)
{
    uint32 key_idx;
    uint32 compare_key_starting_offset = DNX_FIELD_KEY_COMPARE_STARTING_OFFSET(unit);

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(initial_key_id_p, _SHR_E_PARAM, "initial_key_id_p");
    SHR_NULL_CHECK(tcam_key_id_p, _SHR_E_PARAM, "tcam_key_id_p");

    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "Free keys for Compare context %d mode Single key_id %d Tcam key_id %d %s\n",
                 context_id, initial_key_id_p->id[0], tcam_key_id_p->id[0], EMPTY);

    /*
     * Compare mode uses <compare_size> MSB bits out of keys in initial_key_id_p.
     * So we free those bits.
     */
    for (key_idx = 0; (key_idx < DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX) &&
         (initial_key_id_p->id[key_idx] != DNX_FIELD_KEY_ID_INVALID); key_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_algo_field_key_ipmf1_initial_key_occupation_bmp_with_key_id_free
                        (unit, SW_STATE_ALGO_RES_ALLOCATE_WITH_ID, context_id, initial_key_id_p->id[key_idx],
                         dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->compare_key_size,
                         compare_key_starting_offset));
    }

    /*
     * Compare mode uses the keys in tcam_key_id_p TCAM result buffer.
     * So we free that key.
     */
    for (key_idx = 0; (key_idx < DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX) &&
         (tcam_key_id_p->id[key_idx] != DNX_FIELD_KEY_ID_INVALID); key_idx++)
    {
        

    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_key_compare_mode_double_resources_reserve(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_id_t *initial_key_id_1_p,
    dnx_field_key_id_t *initial_key_id_2_p)
{
    uint32 key_idx;
    uint32 compare_key_starting_offset = DNX_FIELD_KEY_COMPARE_STARTING_OFFSET(unit);

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(initial_key_id_1_p, _SHR_E_PARAM, "initial_key_id_1_p");
    SHR_NULL_CHECK(initial_key_id_2_p, _SHR_E_PARAM, "initial_key_id_2_p");

    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "Reserve keys for Compare context %d mode Double key_id %d key_id %d %s\n",
                 context_id, initial_key_id_1_p->id[0], initial_key_id_2_p->id[0], EMPTY);

    /*
     * Compare_1 mode uses <compare_size> MSB bits out of keys in initial_key_id_1_p.
     * For that matter we will reserve those bits in the required key.
     */
    for (key_idx = 0; (key_idx < DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX) &&
         (initial_key_id_1_p->id[key_idx] != DNX_FIELD_KEY_ID_INVALID); key_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_algo_field_key_ipmf1_initial_key_occupation_bmp_with_key_id_allocate
                        (unit, SW_STATE_ALGO_RES_ALLOCATE_WITH_ID, context_id, initial_key_id_1_p->id[key_idx],
                         dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->compare_key_size,
                         &compare_key_starting_offset));
    }

    /*
     * Compare_2 mode uses <compare_size> MSB bits out of keys in initial_key_id_2_p.
     * For that matter we will reserve those bits in the required key.
     */
    for (key_idx = 0; (key_idx < DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX) &&
         (initial_key_id_2_p->id[key_idx] != DNX_FIELD_KEY_ID_INVALID); key_idx++)
    {

        SHR_IF_ERR_EXIT(dnx_algo_field_key_ipmf1_initial_key_occupation_bmp_with_key_id_allocate
                        (unit, SW_STATE_ALGO_RES_ALLOCATE_WITH_ID, context_id, initial_key_id_2_p->id[key_idx],
                         dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->compare_key_size,
                         &compare_key_starting_offset));

    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_key_compare_mode_double_resources_free(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_id_t *initial_key_id_1_p,
    dnx_field_key_id_t *initial_key_id_2_p)
{
    uint32 key_idx;
    uint32 compare_key_starting_offset = DNX_FIELD_KEY_COMPARE_STARTING_OFFSET(unit);

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(initial_key_id_1_p, _SHR_E_PARAM, "initial_key_id_1_p");
    SHR_NULL_CHECK(initial_key_id_2_p, _SHR_E_PARAM, "initial_key_id_2_p");

    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "Free keys for Compare context %d mode Double key_id %d key_id %d %s\n",
                 context_id, initial_key_id_1_p->id[0], initial_key_id_2_p->id[0], EMPTY);
    /*
     * Compare_1 mode uses <compare_size> MSB bits out of keys in initial_key_id_1_p.
     * So we free those bits.
     */
    for (key_idx = 0; (key_idx < DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX) &&
         (initial_key_id_1_p->id[key_idx] != DNX_FIELD_KEY_ID_INVALID); key_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_algo_field_key_ipmf1_initial_key_occupation_bmp_with_key_id_free
                        (unit, SW_STATE_ALGO_RES_ALLOCATE_WITH_ID, context_id, initial_key_id_1_p->id[key_idx],
                         dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->compare_key_size,
                         compare_key_starting_offset));

    }

    /*
     * Compare_2 mode uses the 32 MSB bits out of keys in initial_key_id_2_p.
     * So we free those bits.
     */
    for (key_idx = 0; (key_idx < DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX) &&
         (initial_key_id_2_p->id[key_idx] != DNX_FIELD_KEY_ID_INVALID); key_idx++)
    {

        SHR_IF_ERR_EXIT(dnx_algo_field_key_ipmf1_initial_key_occupation_bmp_with_key_id_free
                        (unit, SW_STATE_ALGO_RES_ALLOCATE_WITH_ID, context_id, initial_key_id_2_p->id[key_idx],
                         dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->compare_key_size,
                         compare_key_starting_offset));

    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_key_hash_resources_reserve(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_id_t *initial_key_id_p)
{
    uint32 key_idx;
    uint32 key_starting_offset = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(initial_key_id_p, _SHR_E_PARAM, "initial_key_id_p");

    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "Reserve keys for Hash context %d key_id %d key_id %d %s\n",
                 context_id, initial_key_id_p->id[0], initial_key_id_p->id[1], EMPTY);

    /*
     * Hash mode uses <hash_size> bits out of keys in initial_key_id_p.
     * For that matter we will reserve those bits in the required key.
     */
    for (key_idx = 0; (key_idx < DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX) &&
         (initial_key_id_p->id[key_idx] != DNX_FIELD_KEY_ID_INVALID); key_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_algo_field_key_ipmf1_initial_key_occupation_bmp_with_key_id_allocate
                        (unit, SW_STATE_ALGO_RES_ALLOCATE_WITH_ID, context_id, initial_key_id_p->id[key_idx],
                         DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_SINGLE, &key_starting_offset));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_key_hash_resources_free(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_id_t *initial_key_id_p)
{
    uint32 key_idx;
    uint32 key_starting_offset = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(initial_key_id_p, _SHR_E_PARAM, "initial_key_id_p");

    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "Free keys for Hash context %d key_id %d key_id %d %s\n",
                 context_id, initial_key_id_p->id[0], initial_key_id_p->id[1], EMPTY);
    /*
     * Hash mode uses <hash_size> bits out of keys in initial_key_id_p.
     * So we free those bits.
     */
    for (key_idx = 0; (key_idx < DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX) &&
         (initial_key_id_p->id[key_idx] != DNX_FIELD_KEY_ID_INVALID); key_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_algo_field_key_ipmf1_initial_key_occupation_bmp_with_key_id_free
                        (unit, SW_STATE_ALGO_RES_ALLOCATE_WITH_ID, context_id, initial_key_id_p->id[key_idx],
                         DNX_DATA_MAX_FIELD_TCAM_KEY_SIZE_SINGLE, key_starting_offset));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See field_key.h
 */
shr_error_e
dnx_field_key_state_table_resources_reserve(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_id_t *initial_key_id_p)
{
    uint32 key_idx;
    uint32 state_table_key_starting_offset = DNX_FIELD_KEY_STATE_TABLE_STARTING_OFFSET(unit);

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(initial_key_id_p, _SHR_E_PARAM, "initial_key_id_p");

    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "Reserving resources for state table, context %d, key_id %d%s%s\r\n",
                 context_id, initial_key_id_p->id[0], EMPTY, EMPTY);

    /*
     * State Table mode uses the <state_table_size> MSB bits out of keys in initial_key_id_p.
     * For that matter we will reserve those bits in the required key.
     */
    for (key_idx = 0; (key_idx < DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX) &&
         (initial_key_id_p->id[key_idx] != DNX_FIELD_KEY_ID_INVALID); key_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_algo_field_key_ipmf1_initial_key_occupation_bmp_with_key_id_allocate
                        (unit, SW_STATE_ALGO_RES_ALLOCATE_WITH_ID, context_id, initial_key_id_p->id[key_idx],
                         DNX_DATA_MAX_FIELD_STATE_TABLE_KEY_SIZE, &state_table_key_starting_offset));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_key_state_table_resources_free(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_key_id_t *initial_key_id_p)
{
    uint32 key_idx;
    uint32 state_table_key_starting_offset = DNX_FIELD_KEY_STATE_TABLE_STARTING_OFFSET(unit);

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(initial_key_id_p, _SHR_E_PARAM, "initial_key_id_p");

    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "Freeing resources for state table, context %d, key_id %d%s%s\r\n",
                 context_id, initial_key_id_p->id[0], EMPTY, EMPTY);

    /*
     * State Table mode uses the <state_table_size> MSB bits out of keys in initial_key_id_p.
     * So we free those bits.
     */
    for (key_idx = 0; (key_idx < DNX_DATA_MAX_FIELD_GROUP_NOF_KEYS_PER_FG_MAX) &&
         (initial_key_id_p->id[key_idx] != DNX_FIELD_KEY_ID_INVALID); key_idx++)
    {
        SHR_IF_ERR_EXIT(dnx_algo_field_key_ipmf1_initial_key_occupation_bmp_with_key_id_free
                        (unit, SW_STATE_ALGO_RES_ALLOCATE_WITH_ID, context_id, initial_key_id_p->id[key_idx],
                         DNX_DATA_MAX_FIELD_STATE_TABLE_KEY_SIZE, state_table_key_starting_offset));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_key_qual_attach_verify(
    int unit,
    dnx_field_stage_e field_stage,
    dnx_field_context_t context_id,
    dnx_field_group_t fg_id,
    dnx_field_qual_t dnx_quals[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG],
    dnx_field_qual_attach_info_t qual_info[DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG])
{
    uint32 qual_index;
    dnx_field_qual_class_e cur_class;
    dnx_field_input_type_e cur_input_type;
    uint32 qual_size;

    SHR_FUNC_INIT_VARS(unit);
    for (qual_index = 0; qual_index < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG; qual_index++)
    {
        if (dnx_quals[qual_index] != DNX_FIELD_QUAL_TYPE_INVALID)
        {
            cur_class = DNX_QUAL_CLASS(dnx_quals[qual_index]);
            cur_input_type = qual_info[qual_index].input_type;

            /*
             * Validating input params per Input type.
             */
            switch (cur_input_type)
            {
                case DNX_FIELD_INPUT_TYPE_LAYER_FWD:
                case DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE:
                {
                    if (cur_class != DNX_FIELD_QUAL_CLASS_HEADER && cur_class != DNX_FIELD_QUAL_CLASS_USER)
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "The supplied input type(%s) for qual %s in FG %d context %d does not match with the corresponding qualifier class(%s). \n",
                                     dnx_field_input_type_text(cur_input_type),
                                     dnx_field_dnx_qual_text(unit, dnx_quals[qual_index]),
                                     fg_id, context_id, dnx_field_qual_class_text(cur_class));
                    }
                    if (qual_info[qual_index].input_arg >= DNX_FIELD_LAYER_NOF)
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "The supplied input arg(%d) for qual %s in FG %d context %d is out of range for input type LAYER. \n",
                                     qual_info[qual_index].input_arg, dnx_field_dnx_qual_text(unit,
                                                                                              dnx_quals[qual_index]),
                                     fg_id, context_id);
                    }
                    if (qual_info[qual_index].offset >
                        dnx_data_field.stage.stage_info_get(unit, field_stage)->pbus_header_length
                        || qual_info[qual_index].offset < -dnx_data_field.stage.stage_info_get(unit,
                                                                                               field_stage)->pbus_header_length)
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "The supplied offset(%d) for qual %s in FG %d context %d is out of range (%d:%d) "
                                     "for input type LAYER for stage %s. \n",
                                     qual_info[qual_index].offset, dnx_field_dnx_qual_text(unit, dnx_quals[qual_index]),
                                     fg_id, context_id,
                                     -dnx_data_field.stage.stage_info_get(unit, field_stage)->pbus_header_length,
                                     dnx_data_field.stage.stage_info_get(unit, field_stage)->pbus_header_length,
                                     dnx_field_stage_text(unit, field_stage));
                    }
                    break;
                }
                case DNX_FIELD_INPUT_TYPE_META_DATA:
                {
                    if (cur_class != DNX_FIELD_QUAL_CLASS_META && cur_class != DNX_FIELD_QUAL_CLASS_USER
                        && cur_class != DNX_FIELD_QUAL_CLASS_SW)
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "The supplied input type(%s) for qual %s in FG %d context %d does not match with the corresponding qualifier class(%s). \n",
                                     dnx_field_input_type_text(cur_input_type),
                                     dnx_field_dnx_qual_text(unit, dnx_quals[qual_index]),
                                     fg_id, context_id, dnx_field_qual_class_text(cur_class));
                    }
                    if (cur_class == DNX_FIELD_QUAL_CLASS_USER && qual_info[qual_index].offset < 0)
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "The supplied offset(%d) for user defined qual %s in FG %d context %d is negative. "
                                     "Offset may not be negative for user defined qualifier with input type metadata.\n",
                                     qual_info[qual_index].offset, dnx_field_dnx_qual_text(unit, dnx_quals[qual_index]),
                                     fg_id, context_id);
                    }
                    SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_size(unit, field_stage, dnx_quals[qual_index], &qual_size));
                    if (cur_class == DNX_FIELD_QUAL_CLASS_USER && (qual_info[qual_index].offset + qual_size) >
                        dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_bits_main_pbus)
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "The supplied offset(%d) plus size (%d) is %d, exceeds the size of the PBUS on "
                                     "stage %s (%d).\n",
                                     qual_info[qual_index].offset, qual_size, qual_info[qual_index].offset + qual_size,
                                     dnx_field_stage_text(unit, field_stage),
                                     dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_bits_native_pbus);
                    }
                    break;
                }
                case DNX_FIELD_INPUT_TYPE_META_DATA2:
                {
                    if (cur_class != DNX_FIELD_QUAL_CLASS_META2 && cur_class != DNX_FIELD_QUAL_CLASS_USER)
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "The supplied input type(%s) for qual %s in FG %d context %d does not match with the corresponding qualifier class(%s). \n",
                                     dnx_field_input_type_text(cur_input_type), dnx_field_dnx_qual_text(unit,
                                                                                                        dnx_quals
                                                                                                        [qual_index]),
                                     fg_id, context_id, dnx_field_qual_class_text(cur_class));
                    }
                    if (cur_class == DNX_FIELD_QUAL_CLASS_USER && qual_info[qual_index].offset < 0)
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "The supplied offset(%d) for user defined qual %s in FG %d context %d is negative. "
                                     "Offset may not be negative for user defined qualifier with input type metadata2.\n",
                                     qual_info[qual_index].offset, dnx_field_dnx_qual_text(unit, dnx_quals[qual_index]),
                                     fg_id, context_id);
                    }
                    SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_size(unit, field_stage, dnx_quals[qual_index], &qual_size));
                    if (cur_class == DNX_FIELD_QUAL_CLASS_USER && (qual_info[qual_index].offset + qual_size) >
                        dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_bits_native_pbus)
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "The supplied offset(%d) plus size (%d) is %d, exceeds the size of the PBUS on "
                                     "stage %s (%d).\n",
                                     qual_info[qual_index].offset, qual_size, qual_info[qual_index].offset + qual_size,
                                     dnx_field_stage_text(unit, field_stage),
                                     dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_bits_native_pbus);
                    }
                    break;
                }
                case DNX_FIELD_INPUT_TYPE_LAYER_RECORDS_FWD:
                case DNX_FIELD_INPUT_TYPE_LAYER_RECORDS_ABSOLUTE:
                {
                    if (cur_class != DNX_FIELD_QUAL_CLASS_LAYER_RECORD && cur_class != DNX_FIELD_QUAL_CLASS_USER)
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "The supplied input type(%s) for qual %s in FG %d context %d does not match with the corresponding qualifier class(%s). \n",
                                     dnx_field_input_type_text(cur_input_type),
                                     dnx_field_dnx_qual_text(unit, dnx_quals[qual_index]),
                                     fg_id, context_id, dnx_field_qual_class_text(cur_class));
                    }
                    if (qual_info[qual_index].input_arg >= DNX_FIELD_LAYER_NOF)
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "The supplied input arg(%d) is out of range for input type LAYER_RECORDS. \n",
                                     qual_info[qual_index].input_arg);
                    }
                    if (cur_class == DNX_FIELD_QUAL_CLASS_USER &&
                        (qual_info[qual_index].offset > DNX_INPUT_TYPE_MAX_OFFSET_LAYER_RECORDS ||
                         qual_info[qual_index].offset < 0))
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "The supplied offset(%d) for qual %s in FG %d context %d is out of range for input type LAYER_RECORDS (%d:%d). \n",
                                     qual_info[qual_index].offset, dnx_field_dnx_qual_text(unit, dnx_quals[qual_index]),
                                     fg_id, context_id, 0, DNX_INPUT_TYPE_MAX_OFFSET_LAYER_RECORDS);
                    }
                    break;
                }
                case DNX_FIELD_INPUT_TYPE_KBP:
                {
                    if ((cur_class != DNX_FIELD_QUAL_CLASS_USER))
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "The supplied input type(%s) for qual %s in FG %d context %d does not match with the corresponding qualifier class(%s). \n",
                                     dnx_field_input_type_text(cur_input_type),
                                     dnx_field_dnx_qual_text(unit, dnx_quals[qual_index]),
                                     fg_id, context_id, dnx_field_qual_class_text(cur_class));
                    }
                    if (cur_class == DNX_FIELD_QUAL_CLASS_USER && qual_info[qual_index].offset < 0)
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "The supplied offset(%d) for user defined qual %s in FG %d context %d is negative. "
                                     "Offset may not be negative for user defined qualifier with input type KBP.\n",
                                     qual_info[qual_index].offset, dnx_field_dnx_qual_text(unit, dnx_quals[qual_index]),
                                     fg_id, context_id);
                    }
                    break;
                }
                case DNX_FIELD_INPUT_TYPE_CASCADED:
                {
                    if ((cur_class != DNX_FIELD_QUAL_CLASS_USER))
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "The supplied input type(%s) for qual %s in FG %d context %d does not match with the corresponding qualifier class(%s). \n",
                                     dnx_field_input_type_text(cur_input_type),
                                     dnx_field_dnx_qual_text(unit, dnx_quals[qual_index]),
                                     fg_id, context_id, dnx_field_qual_class_text(cur_class));
                    }
                    if (qual_info[qual_index].input_arg >= dnx_data_field.group.nof_fgs_get(unit))
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "The supplied input arg(%d) for qual %s in FG %d context %d is out of range for input type CASCADED - must be valid FG ID. \n",
                                     qual_info[qual_index].input_arg, dnx_field_dnx_qual_text(unit,
                                                                                              dnx_quals[qual_index]),
                                     fg_id, context_id);
                    }
                    break;
                }
                case DNX_FIELD_INPUT_TYPE_CONST:
                {
                    if (cur_class != DNX_FIELD_QUAL_CLASS_META && cur_class != DNX_FIELD_QUAL_CLASS_USER
                        && cur_class != DNX_FIELD_QUAL_CLASS_SW)
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "The supplied input type(%s) for qual %s in FG %d context %d does not match with the corresponding qualifier class(%s). \n",
                                     dnx_field_input_type_text(cur_input_type), dnx_field_dnx_qual_text(unit,
                                                                                                        dnx_quals
                                                                                                        [qual_index]),
                                     fg_id, context_id, dnx_field_qual_class_text(cur_class));
                    }
                    break;
                }
                default:
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "The input type(%d) supplied for qual %s in FG %d context %d is not valid. \n",
                                 cur_input_type, dnx_field_dnx_qual_text(unit, dnx_quals[qual_index]), fg_id,
                                 context_id);
                    break;
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function gets the keys unused by a FWD context for KBP using "is_acl" (which context selects the KBP).
 *
 * \param [in] unit             - Device Id
 * \param [in] fwd_context      - FWD context
 * \param [out] keys_unused_bmp - Bitmap of the keys unused by the FWD context.
 *
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_field_key_ifwd2_fwd_context_unused_keys_is_acl_get(
    int unit,
    uint32 fwd_context,
    uint32 *keys_unused_bmp)
{
    uint32 entry_handle_id;
    uint8 is_acl;
    int key_id;
    uint32 kbp_key_bmp = dnx_data_field.kbp.key_bmp_get(unit);
    int kbp_key = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    (*keys_unused_bmp) = 0;

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KLEAP_FWD12_KBR_INFO, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_PROFILE, fwd_context);

    /*
     * Go over all of the keys in FWD stage that can be used by the KBP.
     */
    for (key_id = 0; kbp_key_bmp != 0; (key_id++), (kbp_key_bmp >>= 1))
    {
        if ((kbp_key_bmp & 1) == 0)
        {
            continue;
        }
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD12_KBR_ID, key_id);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                        (unit, entry_handle_id, DBAL_FIELD_KBR_IS_ACL, INST_SINGLE, &is_acl));

        if (is_acl == TRUE)
        {
            (*keys_unused_bmp) |= (1 << kbp_key);
        }
        kbp_key++;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function gets the keys unsused by a FWD context for KBP using KBR.
 *
 * \param [in] unit             - Device Id
 * \param [in] fwd_context      - FWD context
 * \param [out] keys_unused_bmp - Bitmap of the keys unused by the FWD context for KBP.
 *
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_field_key_ifwd2_fwd_context_unused_keys_kbr_get(
    int unit,
    uint32 fwd_context,
    uint32 *keys_unused_bmp)
{
    uint32 entry_handle_id;
    int key_id;
    uint32 kbp_key_bmp = dnx_data_field.kbp.key_bmp_get(unit);
    uint32 kbp_ffc_bmp[BITS2WORDS(DNX_DATA_MAX_FIELD_BASE_IFWD2_NOF_FFC)] = { 0 };
    int ffc_bmp_ndx;
    int ffc_in_bitmap;
    int kbp_key = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    (*keys_unused_bmp) = 0;

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KLEAP_FWD12_KBR_INFO, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_PROFILE, fwd_context);

    /*
     * Go over all of the keys in FWD stage that can be used by the KBP.
     */
    for (key_id = 0; kbp_key_bmp != 0; (key_id++), (kbp_key_bmp >>= 1))
    {
        if ((kbp_key_bmp & 1) == 0)
        {
            continue;
        }
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD12_KBR_ID, key_id);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_FFC_BITMAP, INST_SINGLE, kbp_ffc_bmp));
        ffc_in_bitmap = FALSE;
        for (ffc_bmp_ndx = 0; ffc_bmp_ndx < (sizeof(kbp_ffc_bmp) / sizeof(kbp_ffc_bmp[0])); ffc_bmp_ndx++)
        {
            if (kbp_ffc_bmp[ffc_bmp_ndx] != 0)
            {
                ffc_in_bitmap = TRUE;
                break;
            }
        }
        if (ffc_in_bitmap == FALSE)
        {
            (*keys_unused_bmp) |= (1 << kbp_key);
        }
        kbp_key++;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function gets the keys unused by a FWD context for KBP using aligner mapping.
 *
 * \param [in] unit             - Device Id
 * \param [in] fwd_context      - FWD context
 * \param [out] keys_unused_bmp - Bitmap of the keys unused by the FWD context for KBP.
 *
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_field_key_ifwd2_fwd_context_unused_keys_aligner_get(
    int unit,
    uint32 fwd_context,
    uint32 *keys_unused_bmp)
{
    uint32 entry_handle_id;
    dbal_enum_value_field_kbp_kbr_idx_e kbp_key;
    uint32 aligner_key_size;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    (*keys_unused_bmp) = 0;

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KLEAP_FWD12_ALIGNER_MAPPING, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_PROFILE, fwd_context);

    for (kbp_key = 0; kbp_key < DBAL_NOF_ENUM_KBP_KBR_IDX_VALUES; kbp_key++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_KBP_KBR_IDX, kbp_key);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_FWD_KEY_SIZE, INST_SINGLE, &aligner_key_size));

        if (aligner_key_size == 0)
        {
            /*
             * Key is in use.
             */
            (*keys_unused_bmp) |= (1 << kbp_key);
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_key_aligner_mapping_mem_hw_set(
    int unit,
    int context_id,
    dbal_enum_value_field_kbp_kbr_idx_e key_id,
    uint32 key_size)
{
    uint32 entry_handle_id;
    dbal_tables_e table_id = DBAL_TABLE_KLEAP_FWD12_ALIGNER_MAPPING;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_PROFILE, context_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_KBP_KBR_IDX, key_id);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ACL_KEY_SIZE, INST_SINGLE, key_size);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function converts the FWD2 (external stage) KBR form values 16-17 to
 *  KBP KBR values 2-3.
 *
 * \param [in] unit          - Device Id
 * \param [in] key_id        - Key Id
 * \param [out] kbp_kbr_id_p - Converted kbr id
 *
 * \return
 *   shr_error_e -   Error Type
 * \remark
 *   * None
 * \see
 *   * dnx_field_key_fwd2_to_acl_key_id_convert
 */
static shr_error_e
dnx_field_key_fwd2_to_acl_key_id_convert(
    int unit,
    dbal_fields_e key_id,
    dbal_enum_value_field_kbp_kbr_idx_e * kbp_kbr_id_p)
{
    SHR_FUNC_INIT_VARS(unit);
    
    switch (key_id)
    {
        case DBAL_DEFINE_FWD12_KBR_ID_KBP_0:
        {
            *kbp_kbr_id_p = DBAL_ENUM_FVAL_KBP_KBR_IDX_KEY_0;
            break;
        }
        case DBAL_DEFINE_FWD12_KBR_ID_KBP_1:
        {
            *kbp_kbr_id_p = DBAL_ENUM_FVAL_KBP_KBR_IDX_KEY_1;
            break;
        }
        case DBAL_DEFINE_FWD12_KBR_ID_KBP_2:
        {
            *kbp_kbr_id_p = DBAL_ENUM_FVAL_KBP_KBR_IDX_KEY_2;
            break;
        }
        case DBAL_DEFINE_FWD12_KBR_ID_KBP_3:
        {
            *kbp_kbr_id_p = DBAL_ENUM_FVAL_KBP_KBR_IDX_KEY_3;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "The key_id (%d) supplied is not valid KBP ACL key. %s%s%s\n", key_id, EMPTY, EMPTY, EMPTY);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_field_key_aligner_mapping_mem_set(
    int unit,
    int context_id,
    dbal_fields_e key_id,       /* TBD: Replace with the relevant FWD12 enum */
    uint32 key_size)
{
    dbal_enum_value_field_kbp_kbr_idx_e kbp_kbr_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_key_fwd2_to_acl_key_id_convert(unit, key_id, &kbp_kbr_id));

    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "aligner_mapping_mem_set context %d, key_id %d, kbp_kbr_id %d key_size %d\r\n",
                 context_id, key_id, kbp_kbr_id, key_size);

    SHR_IF_ERR_EXIT(dnx_field_key_aligner_mapping_mem_hw_set(unit, context_id, kbp_kbr_id, BITS2BYTES(key_size)));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
dnx_field_key_kbp_master_key_id_get(
    int unit,
    uint32 fwd_context,
    dbal_enum_value_field_field_key_e key_ids[DNX_DATA_MAX_FIELD_KBP_NOF_ACL_KEYS_MASTER_MAX])
{
    uint32 acl_keys_bmp;
    int key_id;
    uint32 kbp_key_bmp = dnx_data_field.kbp.key_bmp_get(unit);
    int max_acl_keys = dnx_data_field.kbp.nof_acl_keys_master_max_get(unit);
    int key_index;
    int key_is_acl;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(key_ids, _SHR_E_PARAM, "key_ids");

    SHR_IF_ERR_EXIT(dnx_field_key_ifwd2_fwd_context_unused_keys_is_acl_get(unit, fwd_context, &acl_keys_bmp));

    key_index = 0;
    for (key_id = 0; (kbp_key_bmp != 0) && (key_index < max_acl_keys); (key_id++), (kbp_key_bmp >>= 1))
    {
        /** If the key is not used for KBP, do not use it. */
        if ((kbp_key_bmp & 1) == 0)
        {
            continue;
        }
        key_is_acl = acl_keys_bmp & 1;
        acl_keys_bmp >>= 1;
        /** If the key is used for FWD, do not use it. */
        if (key_is_acl == 0)
        {
            continue;
        }

        key_ids[key_index] = key_id;

        key_index++;
    }

    /** Fill the rest if the array with an invalid key.*/
    for (; key_index < DNX_DATA_MAX_FIELD_KBP_NOF_ACL_KEYS_MASTER_MAX; key_index++)
    {
        key_ids[key_index] = DNX_FIELD_KEY_ID_INVALID;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Configure the KBP opcode on the required contexts.
 * \param [in] unit        - Device Id
 * \param [in] opcode_id   - Opcode ID. Currently equivalent to Context ID.
 * \param [in] context_id  - Context ID.
 * \param [in] kbp_info_p  - structure that holds KBP info, including the array of segments to configure.
 *                           The information given per segment is the fg_idx and pointers(indexes) into the fg_info.
 *                           We need those pointers in order to extract the attach information about a given qualifier.
 *
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_field_key_kbp_set(
    int unit,
    int opcode_id,
    dnx_field_context_t context_id,
    dnx_field_group_kbp_info_t * kbp_info_p)
{
    int seg_idx;
    dnx_field_group_fg_info_for_sw_t fg_info;
    dnx_field_qual_t qual_type;
    dnx_field_qual_map_in_key_t dummy_qual_map;
    dbal_enum_value_field_field_key_e key_ids[DNX_DATA_MAX_FIELD_KBP_NOF_ACL_KEYS_MASTER_MAX];
    uint32 location_in_key;
    uint32 key_idx;
    uint32 key_idx_3;
    uint32 key_size[DNX_DATA_MAX_FIELD_KBP_NOF_ACL_KEYS_MASTER_MAX];
    uint32 key_size_inverted[DNX_DATA_MAX_FIELD_KBP_NOF_ACL_KEYS_MASTER_MAX];
    uint32 qual_lsb_unaligned[DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY];
    uint32 qual_key_index[DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY];
    int ffc_needed[DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY] = { 0 };
    uint32 qual_in_key_template_idx_arr[DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY];
    dnx_field_group_t fg_id_arr[DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY];
    uint32 msb_aligned_location_on_key;
    uint8 fwd_context;
    uint8 fwd_nof_contexts;
    kbp_mngr_fwd_acl_context_mapping_t fwd_acl_ctx_mapping[DNX_DATA_MAX_FIELD_KBP_MAX_FWD_CONTEXT_NUM_FOR_ONE_APPTYPE];

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&dummy_qual_map, 0, sizeof(dummy_qual_map));

    /*
     * Get the FWD context for the Apptype.
     */
    SHR_IF_ERR_EXIT(kbp_mngr_opcode_to_contexts_get(unit, opcode_id, &fwd_nof_contexts, fwd_acl_ctx_mapping));
    
    if (fwd_nof_contexts != 1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "For opcode_id %d, the number of FWD contexts is %d."
                     "Only one FWD context per opcode is supported.\n", opcode_id, fwd_nof_contexts);
    }
    fwd_context = fwd_acl_ctx_mapping[0].fwd_context;

    /*
     * Get the key id and the free offset inside that key for the KBP use 
     */
    SHR_IF_ERR_EXIT(dnx_field_key_kbp_master_key_id_get(unit, fwd_context, key_ids));

    location_in_key = 0;

    key_idx = 0;

    /*
     * We calculate the location of each qualifier on key. Note that the segments run from MSB to lsb so we iterate 
     * over the KBP segments from high to low. 
     */
    for (seg_idx = (DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY - 1); seg_idx >= 0; seg_idx--)
    {
        dnx_field_qual_t qual_type;
        uint32 qual_size;
        uint32 qual_in_key_template_idx;
        dnx_field_group_t fg_id;
        uint32 aligned_offset_in_key;

        /*
         * In case we derive from a FWD context the first elements of the master key info will be an empty
         * 'place-holder/representer' of the fwd information. In order to maintain multiple number of fg per opcode we
         * will add new segments to the empty place in the master key. Thus, the condition (fg_id !=
         * DNX_FIELD_GROUP_INVALID) is not a stopper for the configuration loop because of the first empty elements.
         * See dnx_field_group_kbp_opcode_attach() for more info 
         */
        if (kbp_info_p->master_key_info.segment_info[seg_idx].fg_id == DNX_FIELD_GROUP_INVALID)
        {
            continue;
        }
        /*
         * Preparation stage begins. 
         */

        /*
         * Extract the fg_id from the given segment info 
         */
        fg_id = kbp_info_p->master_key_info.segment_info[seg_idx].fg_id;

        /*
         * Get the fg_info for a given segment 
         */
        SHR_IF_ERR_EXIT(dnx_field_group_sw_state_get(unit, fg_id, &fg_info));

        /*
         * Extract the qualifier index from the given segment info. This is the index of the qualifier_type AND of the
         * attach_info for this qualifier inside the fg_info. 
         */
        qual_in_key_template_idx = kbp_info_p->master_key_info.segment_info[seg_idx].qual_idx;

        /*
         * Qualifier type is pointed by the qual_in_key_template_idx, extracted from the fg_info 
         */
        qual_type = fg_info.key_template.key_qual_map[qual_in_key_template_idx].qual_type;

        /*
         * Get the qual size.
         */
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_size(unit, DNX_FIELD_STAGE_EXTERNAL, qual_type, &qual_size));

        /*
         * (Byte) Aligned offset in key is the offset we add to the beginning of the ffc content in order to align the
         * segments in the master key.
         * In case the content is not byte aligned we will add the
         * (BYTE - mod(qual_size)) to the beginning of the content (location_in_key)
         */
        aligned_offset_in_key =
            ((qual_size % SAL_UINT8_NOF_BITS) ? (SAL_UINT8_NOF_BITS - (qual_size % SAL_UINT8_NOF_BITS)) : 0);
        /*
         * Add mod8 to the location. (segment location should be byte aligned).
         * For this matter, we align the qualifier to LSB.
         * Thus, we add the modulo8 to the location_in_key, and the size of the qualifier
         * will be aligned inside the key.
         */
        location_in_key += aligned_offset_in_key;

        /*
         * Verify that the size of the (aligned) qualifier fits into the first key. If the size of the qualifier
         * doesn't fit, start using the next key.
         */
        if (location_in_key + qual_size > dnx_data_field.kbp.max_single_key_size_get(unit))
        {
            if (key_idx + 1 >= dnx_data_field.kbp.nof_acl_keys_master_max_get(unit))
            {
                SHR_ERR_EXIT(_SHR_E_FULL, "KBP key for ACL lookups in opcode_id %d context %d does not fit into "
                             "maximum %d keys for KBP ACL.\n",
                             opcode_id, context_id, dnx_data_field.kbp.nof_acl_keys_master_max_get(unit));
            }
            if ((key_idx + 1 >= sizeof(key_ids) / sizeof(key_ids[0])) ||
                key_ids[key_idx + 1] == DNX_FIELD_KEY_ID_INVALID)
            {
                SHR_ERR_EXIT(_SHR_E_FULL, "KBP key for ACL lookups in opcode_id %d context %d does not fit into "
                             "%d keys availalbe for ACL lookups.\n",
                             opcode_id, context_id, dnx_data_field.kbp.nof_acl_keys_master_max_get(unit));
            }

            LOG_INFO_EX(BSL_LOG_MODULE,
                        "Moving to next Key %d because qual %s doesn't fit, seg_idx %d, current offset in key %d\n",
                        key_ids[key_idx + 1], dnx_field_dnx_qual_text(unit, qual_type), seg_idx, location_in_key);

            key_size_inverted[key_idx] = location_in_key;

            /*
             * Switch to the next key 
             */
            location_in_key = aligned_offset_in_key;
            key_idx++;
        }

        /*
         * Updated the location of the FFC to be configured.
         */
        qual_lsb_unaligned[seg_idx] = location_in_key;
        qual_key_index[seg_idx] = key_idx;
        qual_in_key_template_idx_arr[seg_idx] = qual_in_key_template_idx;
        fg_id_arr[seg_idx] = fg_id;
        ffc_needed[seg_idx] = TRUE;

        /*
         * location_in_key is used as a pointer inside the key. we add the size of each qualifier in the segment to the
         * location.
         */
        location_in_key += qual_size;
    }

    key_size_inverted[key_idx] = location_in_key;

    /*
     * Revers the order of the keys as the first key is filled from the MSB by DBAL.
     */
    {
        int seg_idx_2;
        int key_idx_2;

        /*
         * Swap the keys for each qualifier.
         */
        if (key_idx > 0)
        {
            for (seg_idx_2 = (DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY - 1); seg_idx_2 >= 0; seg_idx_2--)
            {
                qual_key_index[seg_idx_2] = key_idx - qual_key_index[seg_idx_2];
            }
        }
        /*
         * Swap the key size for each key.
         */
        for (key_idx_2 = 0; key_idx_2 <= key_idx; key_idx_2++)
        {
            key_size[key_idx_2] = key_size_inverted[key_idx - key_idx_2];
        }
    }
    /*
     * Update the master key length for the given key.
     */
    for (key_idx_3 = 0; key_idx_3 <= key_idx; key_idx_3++)
    {
        SHR_IF_ERR_EXIT(dnx_field_key_aligner_mapping_mem_set
                        (unit, context_id, key_ids[key_idx_3], key_size[key_idx_3]));
    }

    /*
     * Configure the FFCs
     */
    for (seg_idx = (DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY - 1); seg_idx >= 0; seg_idx--)
    {
        dnx_field_key_attached_qual_info_t dnx_qual_info;
        dnx_field_qual_attach_info_t *qual_attach_info_p;

        if (ffc_needed[seg_idx] == FALSE)
        {
            continue;
        }

        /*
         * Get the fg_info for a given segment 
         */
        SHR_IF_ERR_EXIT(dnx_field_group_sw_state_get(unit, fg_id_arr[seg_idx], &fg_info));

        /*
         * Qualifier type is pointed by the qual_in_key_template_idx, extracted from the fg_info 
         */
        qual_type = fg_info.key_template.key_qual_map[qual_in_key_template_idx_arr[seg_idx]].qual_type;

        /*
         * Create dummy map. All but qual_type params are invalid
         */
        dummy_qual_map.qual_type = qual_type;

        /*
         * Qualifier attach_info is pointed by the qual_in_key_template_idx, extracted from the fg_info 
         */
        qual_attach_info_p = &(fg_info.context_info[opcode_id].qual_attach_info[qual_in_key_template_idx_arr[seg_idx]]);

        /*
         * Get the qual_info based on the qual_type+attach_info 
         */
        SHR_IF_ERR_EXIT(dnx_field_key_qual_info_get(unit, DNX_FIELD_STAGE_EXTERNAL, context_id, qual_type,
                                                    qual_attach_info_p, &dnx_qual_info));

        /*
         * Configure the HW for the given qualifier (allocate the required FFCs, construct the instruction for them.
         * location_in_key is used as location in the key. Lately, the content of the key will be
         */
        msb_aligned_location_on_key =
            dnx_data_field.kbp.max_single_key_size_get(unit) - key_size[qual_key_index[seg_idx]] +
            qual_lsb_unaligned[seg_idx];
        SHR_IF_ERR_EXIT(dnx_field_key_ffc_set
                        (unit, DNX_FIELD_STAGE_EXTERNAL, context_id, &dummy_qual_map, &dnx_qual_info,
                         key_ids[qual_key_index[seg_idx]], msb_aligned_location_on_key));

        LOG_DEBUG_EX(BSL_LOG_MODULE, "field_stage %s, context_id %d fg_id %d, qual_in_key_template_idx %d\n",
                     dnx_field_stage_text(unit, DNX_FIELD_STAGE_EXTERNAL), context_id, fg_id_arr[seg_idx],
                     qual_in_key_template_idx_arr[seg_idx]);
        LOG_DEBUG_EX(BSL_LOG_MODULE, " qual in key %s, seg_idx %d, offset in key %d key_id %d\n",
                     dnx_field_dnx_qual_text(unit, qual_type), seg_idx,
                     msb_aligned_location_on_key, key_ids[qual_key_index[seg_idx]]);
        LOG_DEBUG_EX(BSL_LOG_MODULE, "qual attach info: offset 0x%X, size %d, input_type %d %s\n",
                     dnx_qual_info.offset, dnx_qual_info.size, dnx_qual_info.input_type, EMPTY);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function returns the hw is_acl value for a given quad.
 *
 * \param [in] unit        - Device Id
 * \param [in] context_id  - Context ID to detach
 * \param [in] quad_id     - Quad id we would like to check
 * \param [out] quad_is_acl_p    - TRUE if the quad belongs to ACL context
 *
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_field_key_kbp_is_quad_acl_hw_get(
    int unit,
    dnx_field_context_t context_id,
    uint32 quad_id,
    uint32 *quad_is_acl_p)
{
    uint32 entry_handle_id;
    dbal_tables_e table_name;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    table_name = DBAL_TABLE_KLEAP_FWD12_FFC_QUAD_IS_ACL;

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_name, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_PROFILE, context_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FFC_QUAD_IDX, quad_id);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    /** Get the quad_is_acl */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_FFC_QUAD_IS_ACL, INST_SINGLE, quad_is_acl_p));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

/**
 * \brief
 *  This function gets the stage for the given ffc group.
 *
 * \param [in] unit        - Device Id
 * \param [in] group_id    - Group Id
 * \param [in] stage_p     - Value to return
 *
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */

static shr_error_e
dnx_field_key_ifwd2_resource_mapping_ffc_group_stage_hw_get(
    int unit,
    uint32 group_id,
    uint32 *stage_p)
{
    uint32 entry_handle_id;
    dbal_tables_e table_id = DBAL_TABLE_KLEAP_FWD12_RESOURCES_MAPPING;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    /** Get the quad_is_acl */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_FFC_GROUP_SELECTOR, group_id, stage_p));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function manages the available for ACL context ffc by
 *  masking out the ffc's that are used by FWD context.
 *  The function goes over all ifwd2 contexts and checks which ffc's were marked as used by
 *  FWD context. Those ffc's are set as 'used' in the ffc resource manager, thus will never
 *  be allocated for the ACL context.
 *
 * \param [in] unit        - Device Id
 *
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * dnx_field_key_ifwd2_acl_ffc_update_for_new_context
 */
shr_error_e
dnx_field_key_ifwd2_acl_ffc_init(
    int unit)
{
    int ffc_id;
    uint32 quad_id;
    uint32 group_id;
    uint32 fwd_stage;
    uint32 quad_is_acl;
    dnx_field_context_t context_id;
    uint32 nof_ifwd2_contexts = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_EXTERNAL)->nof_contexts;
    uint32 nof_ifwd2_ffc = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_EXTERNAL)->nof_ffc;

    SHR_FUNC_INIT_VARS(unit);

    
    for (context_id = 0; context_id < nof_ifwd2_contexts; context_id++)
    {
        for (ffc_id = 0; ffc_id < nof_ifwd2_ffc; ffc_id++)
        {
            /*
             * Map the ffc_id to it's quad
             */
            quad_id = dnx_data_field.kbp.ffc_to_quad_and_group_map_get(unit, ffc_id)->quad_id;
            /*
             * Check if the quad is_acl - belongs to acl context
             */
            SHR_IF_ERR_EXIT(dnx_field_key_kbp_is_quad_acl_hw_get(unit, context_id, quad_id, &quad_is_acl));
            /*
             * Map the ffc_id to it's group
             */
            group_id = dnx_data_field.kbp.ffc_to_quad_and_group_map_get(unit, ffc_id)->group_id;
            /*
             * Check if the group is in the FWD2
             */
            SHR_IF_ERR_EXIT(dnx_field_key_ifwd2_resource_mapping_ffc_group_stage_hw_get(unit, group_id, &fwd_stage));
            /*
             * In case this ffc DOES NOT belongs to acl context - mark this ffc as used (by FWD context) in the resource
             * manager
             * Some of the ffc's are used in FWD1 (stageA) and cannot be used in the acl (which is FWD2).
             * We will mark those ffc's as 'used' and unavailable for the ACL.
             */
            if ((!quad_is_acl) || (fwd_stage == 0 /* FWD1 */ ))
            {
                /*
                 * This ffc is used by Pemla (FWD context) or by FWD1 stage  - should not be presented as available in the ACL context
                 */
                SHR_IF_ERR_EXIT(dnx_algo_field_key_ifwd2_ffc_allocate
                                (unit, SW_STATE_ALGO_RES_ALLOCATE_WITH_ID, context_id, &ffc_id));
                LOG_DEBUG_EX(BSL_LOG_MODULE,
                             "context_id %d, group_id %d quad_id %d ffc_id %d set to busy (used by FWD)\n",
                             context_id, group_id, quad_id, ffc_id);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_key_ifwd2_acl_ffc_update_for_new_context(
    int unit,
    uint8 fwd_context_id,
    dnx_field_context_t acl_context_id)
{
    int ffc_id;
    uint32 quad_id;
    uint32 group_id;
    uint32 fwd_stage;
    uint32 quad_is_acl;
    uint32 quad_is_acl_in_new_ctx;
    uint32 nof_ifwd2_ffc = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_EXTERNAL)->nof_ffc;

    SHR_FUNC_INIT_VARS(unit);

    for (ffc_id = 0; ffc_id < nof_ifwd2_ffc; ffc_id++)
    {
        /*
         * Map the ffc_id to it's quad
         */
        quad_id = dnx_data_field.kbp.ffc_to_quad_and_group_map_get(unit, ffc_id)->quad_id;
        /*
         * Check if the quad is_acl - belongs to acl context, in the original FWD context.
         */
        SHR_IF_ERR_EXIT(dnx_field_key_kbp_is_quad_acl_hw_get(unit, fwd_context_id, quad_id, &quad_is_acl));
        /*
         * Sanity check: Verify that the quad is available for ACL in the new context.
         * There shouldn't be any quads not belonging to FFC in the dynamic range.
         */
        SHR_IF_ERR_EXIT(dnx_field_key_kbp_is_quad_acl_hw_get(unit, acl_context_id, quad_id, &quad_is_acl_in_new_ctx));
        if (quad_is_acl_in_new_ctx == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "FFC quad %d does not belong to ACL in new ACL context %d in iFWD2.\n",
                         quad_id, acl_context_id);
        }
        /*
         * Map the ffc_id to it's group
         */
        group_id = dnx_data_field.kbp.ffc_to_quad_and_group_map_get(unit, ffc_id)->group_id;
        /*
         * Check if the group is in the FWD2
         */
        SHR_IF_ERR_EXIT(dnx_field_key_ifwd2_resource_mapping_ffc_group_stage_hw_get(unit, group_id, &fwd_stage));
        /*
         * In case this ffc does not belongs to acl context in the fwd context, mark this ffc as used (by FWD context) 
         * in the resource manager.
         * Some of the ffc's are used in FWD1 (stageA) and cannot be used in the acl (which is FWD2).
         * We expect those to have already been marked as 'used' in init.
         */
        if ((!quad_is_acl) && (fwd_stage != 0 /* FWD1 */ ))
        {
            SHR_IF_ERR_EXIT(dnx_algo_field_key_ifwd2_ffc_allocate
                            (unit, SW_STATE_ALGO_RES_ALLOCATE_WITH_ID, acl_context_id, &ffc_id));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function finds what keys are being used by FWD by checking both the KBR mapping of FFCs
 *  and the aligner mapping.
 *  It verifies that there is no mismatsch between the different tables.
 *
 * \param [in] unit        - Device Id
 *
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_field_key_ifwd2_used_keys_on_init_verify(
    int unit)
{
    uint32 fwd_context;
    
    uint32 nof_fwd_contexts = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_EXTERNAL)->nof_contexts;
    uint32 unused_bmp_by_is_acl;
    uint32 unused_bmp_by_kbr;
    uint32 unused_bmp_by_aligner;

    SHR_FUNC_INIT_VARS(unit);

    for (fwd_context = 0; fwd_context < nof_fwd_contexts; fwd_context++)
    {
        SHR_IF_ERR_EXIT(dnx_field_key_ifwd2_fwd_context_unused_keys_is_acl_get
                        (unit, fwd_context, &unused_bmp_by_is_acl));
        SHR_IF_ERR_EXIT(dnx_field_key_ifwd2_fwd_context_unused_keys_kbr_get(unit, fwd_context, &unused_bmp_by_kbr));
        SHR_IF_ERR_EXIT(dnx_field_key_ifwd2_fwd_context_unused_keys_aligner_get
                        (unit, fwd_context, &unused_bmp_by_aligner));
        if ((unused_bmp_by_is_acl & (~unused_bmp_by_kbr)) != 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "On FWD context %d mismatch between KBR context allocation (keys %x) "
                         "and KBR values (keys %x).\n", fwd_context, unused_bmp_by_is_acl, unused_bmp_by_kbr);
        }
        if ((unused_bmp_by_is_acl & (~unused_bmp_by_aligner)) != 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "On FWD context %d mismatch between KBR context allocation (keys %x) "
                         "and aligner (keys %x).\n", fwd_context, unused_bmp_by_is_acl, unused_bmp_by_kbr);
        }
        if (utilex_nof_on_bits_in_long(unused_bmp_by_is_acl) < DNX_FIELD_KBP_ACL_KEYS_MIN)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "On FWD context %d There are %d keys available for ACL, at least %d are "
                         "needed.\n",
                         fwd_context, utilex_nof_on_bits_in_long(unused_bmp_by_is_acl), DNX_FIELD_KBP_ACL_KEYS_MIN);
        }

    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function verifies the minimum of available for ACL hw
 *  resources such as ffc, per each context.
 *
 * \param [in] unit        - Device Id
 *
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_field_key_ifwd2_ffc_verify(
    int unit)
{
    dnx_field_context_t context_id;
    int nof_ffc;
    uint32 min_nof_acl_ffc = dnx_data_field.kbp.min_acl_nof_ffc_get(unit);
    uint32 nof_ifwd2_contexts = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_EXTERNAL)->nof_contexts;

    SHR_FUNC_INIT_VARS(unit);
    for (context_id = 0; context_id < nof_ifwd2_contexts; context_id++)
    {
        SHR_IF_ERR_EXIT(dnx_algo_field_key_ifwd2_ffc_num_of_elements_get(unit, context_id, &nof_ffc));
        if (nof_ffc < min_nof_acl_ffc)
        {
            LOG_ERROR_EX(BSL_LOG_MODULE,
                         "context_id %d has only %d ffc for ACL purposes. Expected number is at least %d %s\n",
                         context_id, nof_ffc, min_nof_acl_ffc, EMPTY);
            return _SHR_E_RESOURCE;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function gets the stage (iFWD1 or iFWD2) for the given FWD key.
 *
 * \param [in] unit        - Device Id
 * \param [in] key_id      - Key Id
 * \param [in] stage_p     - Value to get, which stage (iFWD1 or iFWD2) uses the key.
 *
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_field_key_ifwd2_resource_mapping_key_stage_hw_get(
    int unit,
    uint32 key_id,
    dbal_enum_value_field_kleap_stage_selector_e * stage_p)
{
    uint32 entry_handle_id;
    dbal_tables_e table_id = DBAL_TABLE_KLEAP_FWD12_RESOURCES_MAPPING;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    /** Get the stage for the given key */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_KBR_SELECTOR, key_id, stage_p));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function verifies that the aligner mapping for KBP ACL lookups is all zeros.
 *
 * \param [in] unit        - Device Id
 *
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_field_key_ifwd2_kbp_acl_aligner_mapping_on_init_verify(
    int unit)
{
    uint32 entry_handle_id;
    dbal_enum_value_field_kbp_kbr_idx_e kbp_key = 0;
    dnx_field_context_t acl_context;
    uint32 nof_acl_contexts = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_EXTERNAL)->nof_contexts;
    uint32 aligner_key_size;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KLEAP_FWD12_ALIGNER_MAPPING, &entry_handle_id));

    for (acl_context = 0; acl_context < nof_acl_contexts; acl_context++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_PROFILE, acl_context);
        for (kbp_key = 0; kbp_key < DBAL_NOF_ENUM_KBP_KBR_IDX_VALUES; kbp_key++)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_KBP_KBR_IDX, kbp_key);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_ACL_KEY_SIZE, INST_SINGLE, &aligner_key_size));

            if (aligner_key_size != 0)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "ACL iFWD2 context %d has size %d in KBP ACL key aligner mapping of key %d.\n",
                             acl_context, aligner_key_size, kbp_key);
            }
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function verifies that all keys used by KBP are assigned to iFWD2.
 *
 * \param [in] unit        - Device Id
 *
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_field_key_ifwd2_kbp_stage_keys_verify(
    int unit)
{
    dbal_enum_value_field_kleap_stage_selector_e key_stage;
    uint32 kbp_key_bmp = dnx_data_field.kbp.key_bmp_get(unit);
    int key_id;
    SHR_FUNC_INIT_VARS(unit);

    for (key_id = 0; kbp_key_bmp != 0; (key_id++), (kbp_key_bmp >>= 1))
    {
        if ((kbp_key_bmp & 1) == 0)
        {
            continue;
        }

        SHR_IF_ERR_EXIT(dnx_field_key_ifwd2_resource_mapping_key_stage_hw_get(unit, key_id, &key_stage));

        if (key_stage != DBAL_ENUM_FVAL_KLEAP_STAGE_SELECTOR_STAGE_B)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "FWD key %d on is not assigned to FWD stage B.\n", key_id);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_key_ifwd2_acl_key_verify(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_field.kbp.nof_acl_keys_master_max_get(unit) > DBAL_NOF_ENUM_KBP_KBR_IDX_VALUES)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Maximum number of ACL keyes in KBP %d is bigger than the maximum number "
                     "of KBP keys in DBAL %d.\n",
                     dnx_data_field.kbp.nof_acl_keys_master_max_get(unit), DBAL_NOF_ENUM_KBP_KBR_IDX_VALUES);
    }

    if (dnx_kbp_device_enabled(unit))
    {
        /*
         * Verifies minimum number of FFCs available.
         */
        SHR_IF_ERR_EXIT(dnx_field_key_ifwd2_ffc_verify(unit));
        /*
         * Verify that the keys being used by FWD for KBP match between different HW tables.
         */
        SHR_IF_ERR_EXIT(dnx_field_key_ifwd2_used_keys_on_init_verify(unit));
        /*
         * Verify that the KBP ACL aligner is empty.
         */
        SHR_IF_ERR_EXIT(dnx_field_key_ifwd2_kbp_acl_aligner_mapping_on_init_verify(unit));
        /*
         * Verify that all of the keys used by KBP are mapped to FWD stage B (iFWD2).
         */
        SHR_IF_ERR_EXIT(dnx_field_key_ifwd2_kbp_stage_keys_verify(unit));
    }
exit:
    SHR_FUNC_EXIT;
}
