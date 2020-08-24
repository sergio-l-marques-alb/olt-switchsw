/*
 * $Id: dnx_intr.c $
 *
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * Purpose:    Implement dnx interrupt.
 */

/*************
 * INCLUDES  *
 *************/
#include <shared/bsl.h>
#include <shared/shrextend/shrextend_debug.h>

#include <soc/drv.h>
#include <soc/mcm/allenum.h>
#include <soc/dnx/intr/dnx_intr.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_intr.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dram.h>
#include <soc/dnx/intr/auto_generated/jr2/jr2_intr.h>
#include <soc/dnx/intr/auto_generated/jr2/jr2_intr_cb_func.h>
#include <soc/dnx/intr/auto_generated/j2c/j2c_intr.h>
#include <soc/dnx/intr/auto_generated/j2c/j2c_intr_cb_func.h>
#include <soc/dnx/intr/auto_generated/q2a/q2a_intr.h>
#include <soc/dnx/intr/auto_generated/q2a/q2a_intr_cb_func.h>
#include <soc/dnx/intr/auto_generated/j2p/j2p_intr.h>
#include <soc/dnx/intr/auto_generated/j2p/j2p_intr_cb_func.h>
#include <soc/dnx/field/tcam_access/tcam_access.h>
#include <soc/dnxc/dnxc_intr.h>
/********************************************
 * DEFINES   *
 *********************************************/
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_INTR

/*********************************************
  *    Declaration
 **********************************************/
int jr2_get_int_id_by_name(
    char *name);
int j2c_get_int_id_by_name(
    char *name);
int q2a_get_int_id_by_name(
    char *name);
int j2p_get_int_id_by_name(
    char *name);

/*********************************************
 *  FUNCTION DEFINITION
 *********************************************/
/***********************************
  *  LOCAL
  *************************************/
static int
soc_dnx_ser_memory_mask_init(
    int unit)
{
    soc_reg_t reg;
    char *reg_name;
    int rc;
    int inst;
    int blk;
    soc_reg_above_64_val_t above_64;
    int array_index_max = 1;
    int array_index;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * unmask SER monitor registers
     */
    SOC_REG_ABOVE_64_ALLONES(above_64);

    for (reg = 0; reg < NUM_SOC_REG; reg++)
    {
        if (!SOC_REG_IS_VALID(unit, reg))
        {
            continue;
        }
        if (!SOC_REG_IS_ENABLED(unit, reg))
        {
            continue;
        }

        reg_name = SOC_REG_NAME(unit, reg);
        if ((sal_strstr(reg_name, "MEM_MASK")) == NULL || (sal_strstr(reg_name, "BRDC_") != NULL))
        {
            continue;
        }

        array_index_max = 1;
        if (SOC_REG_IS_ARRAY(unit, reg))
        {
            array_index_max = SOC_REG_NUMELS(unit, reg) + SOC_REG_FIRST_ARRAY_INDEX(unit, reg);
        }
        SOC_BLOCK_ITER_ALL(unit, blk, SOC_REG_FIRST_BLK_TYPE(SOC_REG_INFO(unit, reg).block))
        {
            if (SOC_INFO(unit).block_valid[blk])
            {
                inst = SOC_BLOCK_NUMBER(unit, blk);
                for (array_index = SOC_REG_FIRST_ARRAY_INDEX(unit, reg); array_index < array_index_max; array_index++)
                {
                    rc = soc_reg_above_64_set(unit, reg, inst, array_index, above_64);
                    SHR_IF_ERR_EXIT(rc);
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - transfer interrupt name to interrupt ident
 *
 * \param [in] unit - unit #
 *\param [in] name - interrupt name
 * \return  interrupt ident
 * \see
 *   * None
 */
int
soc_dnx_int_name_to_id(
    int unit,
    char *name)
{
    if (SOC_IS_JERICHO2_ONLY(unit))
    {
        return jr2_get_int_id_by_name(name);
    }
    else if (SOC_IS_J2C(unit))
    {
        return j2c_get_int_id_by_name(name);
    }
    else if (SOC_IS_Q2A(unit))
    {
        return q2a_get_int_id_by_name(name);
    }
    else if (SOC_IS_J2P(unit))
    {
        return j2p_get_int_id_by_name(name);
    }
    return -1;
}
/**
 * \brief - interrupt array init, init interrupt db
 *
 * \param [in] unit - unit #
 * \return int
 * \see
 *   * None
 */
int
soc_dnx_interrupts_array_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if (SOC_IS_JERICHO2_ONLY(unit))
    {
        SHR_IF_ERR_EXIT(jr2_interrupts_array_init(unit));
    }
    else if (SOC_IS_J2C(unit))
    {
        SHR_IF_ERR_EXIT(j2c_interrupts_array_init(unit));
    }
    else if (SOC_IS_Q2A(unit))
    {
        SHR_IF_ERR_EXIT(q2a_interrupts_array_init(unit));
    }
    else if (SOC_IS_J2P(unit))
    {
        SHR_IF_ERR_EXIT(j2p_interrupts_array_init(unit));
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - interrupt array deinit, deinit interrupt db
 *
 * \param [in] unit - unit #
 * \return int
 * \see
 *   * None
 */
int
soc_dnx_interrupts_array_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if (SOC_IS_JERICHO2_ONLY(unit))
    {
        jr2_interrupts_array_deinit(unit);
        SHR_EXIT();
    }
    else if (SOC_IS_J2C(unit))
    {
        j2c_interrupts_array_deinit(unit);
        SHR_EXIT();
    }
    else if (SOC_IS_Q2A(unit))
    {
        q2a_interrupts_array_deinit(unit);
        SHR_EXIT();
    }
    else if (SOC_IS_J2P(unit))
    {
        j2p_interrupts_array_deinit(unit);
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - interrupt handler function init
 *
 * \param [in] unit - unit #
 * \return int
 * \see
 *   * None
 */
int
soc_dnx_interrupt_cb_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if (SOC_IS_JERICHO2_ONLY(unit))
    {
        jr2_interrupt_cb_init(unit);
        SHR_EXIT();
    }
    else if (SOC_IS_J2C(unit))
    {
        j2c_interrupt_cb_init(unit);
        SHR_EXIT();
    }
    else if (SOC_IS_Q2A(unit))
    {
        q2a_interrupt_cb_init(unit);
        SHR_EXIT();
    }
    else if (SOC_IS_J2P(unit))
    {
        j2p_interrupt_cb_init(unit);
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - interrupt bit is assert at ECI_ECI_INTERRUPTSr
 *
 * \param [in] unit - unit #
 * \param [in] blk  - blk #
 * \param [in] eci_interrupt  - reg value of ECI_ECI_INTERRUPTSr #
 * \return
 * \see
 *   * None
 */
int
soc_dnx_is_block_eci_intr_assert(
    int unit,
    int blk,
    soc_reg_above_64_val_t eci_interrupt)
{
    int rc = 0;
    soc_reg_t reg = ECI_ECI_INTERRUPTSr;
    soc_field_t intr_field;
    const dnx_data_intr_general_block_intr_field_mapping_t *intr_mapping;

    intr_mapping = dnx_data_intr.general.block_intr_field_mapping_get(unit, blk);
    intr_field = (soc_field_t) intr_mapping->intr;
    if (intr_field != INVALIDf)
    {
        rc = (int) soc_reg_above_64_field32_get(unit, reg, eci_interrupt, intr_field);
    }
    return rc;
}
/*
* Function:
*      soc_dnx_ser_init
* Purpose:
*      List all mem_mask reigster, unmask all memory for SER
* Parameters:
*      unit                    - (IN)  Device number
*
* Returns:
*      _SHR_E_XXX
*/
int
soc_dnx_ser_init(
    int unit)
{
    const dnx_data_intr_ser_mem_mask_t *mem_mask;
    const dnxc_data_table_info_t *table_info;
    int mask_index;
    SHR_FUNC_INIT_VARS(unit);

    if (SOC_IS_DNX(unit))
    {
        SHR_IF_ERR_EXIT(soc_dnx_ser_memory_mask_init(unit));

        table_info = dnx_data_intr.ser.mem_mask_info_get(unit);
        for (mask_index = 0; mask_index < table_info->key_size[0]; mask_index++)
        {
            mem_mask = dnx_data_intr.ser.mem_mask_get(unit, mask_index);
            if (mem_mask->reg != INVALIDr)
            {
                switch (mem_mask->mode)
                {
                    case dnxc_mem_mask_mode_none:
                    {
                        /** do nothing */
                        break;
                    }
                    case dnxc_mem_mask_mode_zero:
                    {
                        SHR_IF_ERR_EXIT(soc_dnxc_set_mem_mask(unit, mem_mask->reg, mem_mask->field, 0));
                        break;
                    }
                    case dnxc_mem_mask_mode_one:
                    {
                        SHR_IF_ERR_EXIT(soc_dnxc_set_mem_mask(unit, mem_mask->reg, mem_mask->field, 1));
                        break;
                    }
                    default:
                    {
                        LOG_ERROR(BSL_LS_SOC_INTR, (BSL_META_U(unit, "Wrong memory mask mode %d, table index %d\n"),
                                                    mem_mask->mode, mask_index));
                        break;
                    }
                }
            }
        }

        /*
         * Enable TCAM ECC protect machine 
         */
        if (!dnx_data_intr.ser.tcam_protection_issue_get(unit))
        {
            SHR_IF_ERR_EXIT(dnx_ser_tcam_protect_machine_enable(unit, 1));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get entry_data if dram is present
 *
 * \param [in] unit - unit number
 * \param [in] is_exception - returned value
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_intr_exceptional_dram_not_present(
    int unit,
    uint32 *is_exception)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(is_exception, _SHR_E_PARAM, "is_exception is NULL");
    if (dnx_data_dram.general_info.dram_info_get(unit)->dram_bitmap == 0)
    {
        *is_exception = 1;
    }
    else
    {
        *is_exception = 0;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *    soc_dnx_interrupt_enable
 * Description:
 *    Enable interruupt with some exception
 * Parameters:
 *    unit              - Device unit number
 *    block_instance    - interrupt block instance.
 *    inter             - interrupt db
 * Returns:
 *      _SHR_E_xxx
 */
int
soc_dnx_interrupt_enable(
    int unit,
    int intr_id,
    int block_instance,
    const soc_interrupt_db_t * inter)
{
    const dnxc_data_table_info_t *table_info;
    const dnx_data_intr_ser_intr_exception_t *intr_exception;
    uint32 index, is_exception = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** Get size of table default size */
    table_info = dnx_data_intr.ser.intr_exception_info_get(unit);
    for (index = 0; index < table_info->key_size[0]; index++)
    {
        intr_exception = dnx_data_intr.ser.intr_exception_get(unit, index);
        if (intr_id == intr_exception->intr)
        {
            if (intr_exception->exception_get_cb != NULL)
            {
                SHR_IF_ERR_EXIT(intr_exception->exception_get_cb(unit, &is_exception));
                break;
            }
        }
    }

    if (is_exception)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "intr_id(%d) is exceptional and can't be enabled!!\n", intr_id);
    }

    SHR_IF_ERR_EXIT(soc_interrupt_enable(unit, block_instance, inter));
exit:
    SHR_FUNC_EXIT;
}

/*
 * Function:
 *    soc_dnx_is_interrupt_support
 * Description:
 *    If interruupt is supported
 * Parameters:
 *    unit              - Device unit number
 *    intr_id           - interrupt id
 *    is_support        - if interrupt support
 * Returns:
 *      _SHR_E_xxx
 */
int
soc_dnx_is_interrupt_support(
    int unit,
    int intr_id,
    int *is_support)
{
    const dnxc_data_table_info_t *table_info;
    const dnx_data_intr_ser_intr_exception_t *intr_exception;
    uint32 index, is_exception = 0;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(is_support, _SHR_E_PARAM, "is_support")
    /** Get size of table default size */
        table_info = dnx_data_intr.ser.intr_exception_info_get(unit);
    for (index = 0; index < table_info->key_size[0]; index++)
    {
        intr_exception = dnx_data_intr.ser.intr_exception_get(unit, index);
        if (intr_id == intr_exception->intr)
        {
            if (intr_exception->exception_get_cb != NULL)
            {
                SHR_IF_ERR_EXIT(intr_exception->exception_get_cb(unit, &is_exception));
                break;
            }
        }
    }

    if (is_exception)
    {
        *is_support = 0;
    }
    else
    {
        *is_support = 1;
    }

exit:
    SHR_FUNC_EXIT;
}

#undef BSL_LOG_MODULE
