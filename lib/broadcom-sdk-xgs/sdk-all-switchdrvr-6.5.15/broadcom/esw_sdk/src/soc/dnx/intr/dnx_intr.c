/*
 * $Id: dnx_intr.c $
 *
 * $Copyright: (c) 2018 Broadcom.
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
#include <soc/dnx/intr/auto_generated/jer2/jer2_intr.h>
#include <soc/dnx/intr/auto_generated/jer2/jer2_intr_cb_func.h>

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
int jer2_get_int_id_by_name(
    char *name);

/*********************************************
 *  FUNCTION DEFINITION
 *********************************************/
/***********************************
  *  LOCAL
  *************************************/
static 
int
soc_dnx_ser_init_cb(int unit, soc_regaddrinfo_t *ainfo, void *data)
{
    soc_reg_t reg = ainfo->reg;
    char *reg_name = SOC_REG_NAME(unit,reg);
    int rc;
    int inst=0;
    int blk;
    soc_reg_above_64_val_t above_64;
    int array_index_max = 1;
    int array_index;
    SHR_FUNC_INIT_VARS(unit);

    if((sal_strstr(reg_name, "MEM_MASK")) == NULL || (sal_strstr(reg_name, "BRDC_") != NULL))
        SHR_EXIT();

    /* unmask SER monitor registers*/
    SOC_REG_ABOVE_64_ALLONES(above_64);

    SOC_BLOCK_ITER_ALL(unit, blk, SOC_REG_FIRST_BLK_TYPE(SOC_REG_INFO(unit, reg).block)) {
        if (SOC_INFO(unit).block_valid[blk]) {
            if (SOC_REG_IS_ARRAY(unit, reg)){
                array_index_max = SOC_REG_NUMELS(unit, reg) + SOC_REG_FIRST_ARRAY_INDEX(unit,reg);
            }
            for (array_index = SOC_REG_FIRST_ARRAY_INDEX(unit,reg); array_index < array_index_max; array_index++) {
                rc = soc_reg_above_64_set(unit, reg, inst, array_index, above_64);
                SHR_IF_ERR_EXIT(rc);
            }
        }
        inst++;
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
    if (SOC_IS_JERICHO_2_ONLY(unit))
    {
        return jer2_get_int_id_by_name(name);
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

    if (SOC_IS_JERICHO_2_ONLY(unit))
    {
        SHR_IF_ERR_EXIT(jer2_interrupts_array_init(unit));
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

    if (SOC_IS_JERICHO_2_ONLY(unit))
    {
        jer2_interrupts_array_deinit(unit);
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

    if (SOC_IS_JERICHO_2_ONLY(unit))
    {
        jer2_interrupt_cb_init(unit);
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
soc_dnx_ser_init(int unit)
{
    const dnx_data_intr_ser_mem_mask_t *mem_mask;
    const dnxc_data_table_info_t *table_info;
    int mask_index;
    SHR_FUNC_INIT_VARS(unit);

    if (SOC_IS_DNX(unit))
    {
        SHR_IF_ERR_EXIT(soc_reg_iterate(unit, soc_dnx_ser_init_cb, NULL));

        table_info = dnx_data_intr.ser.mem_mask_info_get(unit);
        for (mask_index = 0; mask_index < table_info->key_size[0]; mask_index++)
        {
            mem_mask = dnx_data_intr.ser.mem_mask_get(unit, mask_index);
            if (mem_mask->reg != INVALIDr)
            {
                switch(mem_mask->mode)
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
                        LOG_ERROR(BSL_LS_SOC_INTR, (BSL_META_U(unit,"Wrong memory mask mode %d, table index %d\n"),
                                                    mem_mask->mode, mask_index));
                        break;
                    }
                }
            }
        }

    }

exit:
    SHR_FUNC_EXIT;
}


#undef BSL_LOG_MODULE
