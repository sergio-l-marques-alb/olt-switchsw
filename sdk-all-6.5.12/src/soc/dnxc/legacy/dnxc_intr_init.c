
/*
 * $Id: soc_dnxc_intr_handler.c, v1 16/06/2014 09:55:39 azarrin $
 *
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * Purpose:    Implement soc interrupt handler.
 */

/*************
 * INCLUDES  *
 *************/
#include <shared/bsl.h>

#include <soc/intr.h>
#include <soc/ipoll.h>

#include <soc/dnxc/legacy/dnxc_intr_handler.h>
#include <soc/dnxc/legacy/error.h>
#include <soc/drv.h>
#include <soc/register.h>
#include <shared/shrextend/shrextend_debug.h>

/*************
 * DEFINES   *
 *************/
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_INTR

/*************
 * DECLARATIONS *
 *************/

/*************
 * FUNCTIONS *
 *************/
static 
int
soc_dnxc_ser_init_cb(int unit, soc_regaddrinfo_t *ainfo, void *data)
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

    if(sal_strstr(reg_name, "MEM_MASK") == NULL)
        SHR_EXIT();

    /* unmask SER monitor registers*/
    SOC_REG_ABOVE_64_ALLONES(above_64);
    SOC_BLOCK_ITER_ALL(unit, blk, SOC_REG_FIRST_BLK_TYPE(SOC_REG_INFO(unit, reg).block)) {
        if (SOC_INFO(unit).block_valid[blk]) {
            if (SOC_REG_IS_ARRAY(unit, reg)){
				array_index_max = SOC_REG_NUMELS(unit, reg);
            }
            for (array_index = 0; array_index < array_index_max; array_index++) {
                rc = soc_reg_above_64_set(unit, reg, inst, array_index, above_64);
                SHR_IF_ERR_EXIT(rc);
            }
        }
        inst++;
    }
exit:
    SHR_FUNC_EXIT;

}

int
soc_dnxc_ser_init(int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(soc_reg_iterate(unit, soc_dnxc_ser_init_cb, NULL));
exit:
    SHR_FUNC_EXIT;
}

#undef BSL_LOG_MODULE
