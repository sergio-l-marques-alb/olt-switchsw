/*
 * $Id: dnxf_fabric_cell_snake_test.c,v 1.9 Broadcom SDK $
 *
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * SOC DNXF FABRIC SNAKE TEST
 */

#ifdef BSL_LOG_MODULE
  #error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_FABRIC
#include <shared/bsl.h>
#include <soc/dnxc/error.h>
#include <soc/dnxf/cmn/mbcm.h>
#include <soc/dnxf/cmn/dnxf_drv.h>
#include <soc/dnxf/cmn/dnxf_fabric_cell_snake_test.h>
#include <bcm_int/control.h>
/*
 * Function:
 *      soc_dnxf_cell_snake_test_prepare
 * Purpose:
 *      Prepare system to cell snake test.
 * Parameters:
 *      unit     - (IN)  Unit number.
 *      flags    - (IN)  Configuration parameters
 *      config   - (IN)  Cell snake test configuration
 * Returns:
 *      _SHR_E_xxx
 * Notes:
 */
int
  soc_dnxf_cell_snake_test_prepare(
    int unit, 
    uint32 flags)
{
    int rc;
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);
    
    if (!SOC_UNIT_VALID(unit)) {
        SHR_ERR_EXIT(_SHR_E_UNIT, "Invalid unit");
    }
    
    DNXF_UNIT_LOCK_TAKE(unit);
    
    rc = MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_cell_snake_test_prepare,(unit, flags));
    SHR_IF_ERR_EXIT(rc);
    
exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT;
}
    
    
/*
 * Function:
 *      soc_dnxf_cell_snake_test_run
 * Purpose:
 *      Run cell snake test.
 * Parameters:
 *      unit     - (IN)  Unit number.
 *      flags    - (IN)  Configuration parameters
 *      usec     - (IN)  Timeout
 *      rsults   - (IN)  Cell snake test results
 * Returns:
 *      _SHR_E_xxx
 * Notes:
 */
int
  soc_dnxf_cell_snake_test_run(
    int unit, 
    uint32 flags, 
    soc_dnxf_fabric_cell_snake_test_results_t* results)
{
    int rc;
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);
    
    if (!SOC_UNIT_VALID(unit)) {
        SHR_ERR_EXIT(_SHR_E_UNIT, "Invalid unit");
    }
    
    SHR_NULL_CHECK(results, _SHR_E_PARAM, "results");
    
    DNXF_UNIT_LOCK_TAKE(unit);
    
    rc = MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_cell_snake_test_run,(unit, flags, results));
    SHR_IF_ERR_EXIT(rc);
        
exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT;
}

#undef BSL_LOG_MODULE

