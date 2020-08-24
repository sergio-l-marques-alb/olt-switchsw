/*
 * flexe_acc.c  access layer for flexe core register/RAM access
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_REG

/*************
 * INCLUDES  *
*************/
#include <shared/shrextend/shrextend_debug.h>
#include <soc/memory.h>
#include <soc/feature.h>
#include <soc/types.h>
#include <soc/register.h>
#include <soc/mem.h>
#include <soc/drv.h>




int
flexe_acc_write(int unit, unsigned int offset, int *data, unsigned int nof_words)
{
    int ii;
    int retCode;
    if (data == NULL)
    {
        cli_out("flexe_acc_write: input data is NULL\n");
        return _SHR_E_PARAM;
    }

    for(ii = 0; ii < nof_words; ii++)
    {
        /** set burst size = 0 */
        retCode = soc_reg_field32_modify(unit, FSCL_FLEXE_CORE_REGISTERS_BRIDGEr, REG_PORT_ANY, FLEXE_CORE_REGISTERS_BURST_SIZEf, 0);
        if (retCode != _SHR_E_NONE)
        {
            cli_out("Err at flexe_acc_write: soc_reg_field32_modify return failure: %s\n", soc_errmsg(retCode));
            return retCode;
        }
        
        retCode = soc_mem_array_write(unit, FSCL_FLEXE_CORE_REGISTERSm, 0, SOC_BLOCK_ANY, offset + ii, &data[ii]);
        if (retCode != _SHR_E_NONE)
        {
            cli_out("Err at flexe_acc_write: soc_mem_array_write return failure: %s\n", soc_errmsg(retCode));
            return retCode;
        }
    }

    return _SHR_E_NONE;
}

int
flexe_acc_read(int unit, unsigned int offset, int *data, unsigned int nof_words)
{
    int ii;
    int retCode;
    int val[16];

    if (data == NULL)
    {
        cli_out("flexe_acc_read: input data is NULL\n");
        return _SHR_E_PARAM;
    }

    for(ii = 0; ii < nof_words; ii++)
    {
        /** set burst size = 0 */
        retCode = soc_reg_field32_modify(unit, FSCL_FLEXE_CORE_REGISTERS_BRIDGEr, REG_PORT_ANY, FLEXE_CORE_REGISTERS_BURST_SIZEf, 0);
        if (retCode != _SHR_E_NONE)
        {
            cli_out("Err at flexe_acc_read: soc_reg_field32_modify return failure: %s\n", soc_errmsg(retCode));
            return retCode;
        }
        retCode = soc_mem_array_read_flags(unit, FSCL_FLEXE_CORE_REGISTERSm, 0, SOC_BLOCK_ANY, offset + ii, val, SOC_MEM_DONT_USE_CACHE);
        if (retCode != _SHR_E_NONE)
        {
            cli_out("Err at flexe_acc_read: soc_reg_field32_modify return failure: %s\n", soc_errmsg(retCode));
            return retCode;
        }
        data[ii] = val[0]; 
    }

    return _SHR_E_NONE;
}


