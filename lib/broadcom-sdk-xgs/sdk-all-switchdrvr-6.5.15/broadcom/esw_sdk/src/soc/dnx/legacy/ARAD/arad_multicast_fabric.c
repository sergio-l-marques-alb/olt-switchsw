#include <soc/mcm/memregs.h>
/* $Id: jer2_arad_multicast_fabric.c,v 1.20 Broadcom SDK $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
*/

#ifdef BSL_LOG_MODULE
  #error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_MULTICAST

/*************
 * INCLUDES  *
 *************/
/* { */
#include <shared/bsl.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnxc/legacy/error.h>
#include <soc/dnx/legacy/drv.h>


#include <soc/dnx/legacy/ARAD/arad_multicast_fabric.h>
#include <soc/dnx/legacy/ARAD/arad_reg_access.h>
#include <soc/dnx/legacy/ARAD/arad_tbl_access.h>
#include <soc/dnx/legacy/ARAD/arad_general.h>
#include <soc/dnx/legacy/ARAD/arad_chip_tbls.h>
#include <soc/dnx/legacy/ARAD/arad_mgmt.h>
#include <soc/dnx/legacy/SAND/Management/sand_general_macros.h>

#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <soc/dnx/legacy/JER2/jer2_ingress_packet_queuing.h>


/* } */

/*************
 * DEFINES   *
 *************/
/* { */



#define JER2_ARAD_MULT_NOF_INGRESS_SHAPINGS             (2)
#define JER2_ARAD_MULT_TC_MAPPING_FABRIC_MULT_NO_IS     (16)
#define JER2_ARAD_MULT_TC_MAPPING_FABRIC_MULT_WITH_IS   (17)

/* } */

/*************
 *  MACROS   *
 *************/
/* { */

/* } */

/*************
 * TYPE DEFS *
 *************/
/* { */

/* } */

/*************
 * GLOBALS   *
 *************/
/* { */

/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

int
jer2_arad_multicast_fabric_init(
    DNX_SAND_IN int unit)
{
    uint64 data64;
    uint32 rate = 0;
    SHR_FUNC_INIT_VARS(unit);

    /* 
     * Configure fmc_top_shaper to max. In addition, Each core has its own shaper.
     * The top shaper can also set weights between cores. Leave the wight
     * on default, which is equal weight.
     */
    COMPILER_64_ZERO(data64);
    SHR_IF_ERR_EXIT(READ_IPS_FMC_TOP_SHAPER_CONFIGr(unit, _SHR_CORE_ALL ,&data64));
    rate = (0x1 << soc_reg_field_length(unit, IPS_FMC_TOP_SHAPER_CONFIGr, FMC_TOP_MAX_CRDT_RATEf)) - 1;
    soc_reg64_field_set(unit, IPS_FMC_TOP_SHAPER_CONFIGr, &data64, FMC_TOP_MAX_CRDT_RATEf, rate);
    SHR_IF_ERR_EXIT(WRITE_IPS_FMC_TOP_SHAPER_CONFIGr(unit, _SHR_CORE_ALL, data64));
    
exit:
    SHR_FUNC_EXIT;
}

/* } */

#undef BSL_LOG_MODULE


