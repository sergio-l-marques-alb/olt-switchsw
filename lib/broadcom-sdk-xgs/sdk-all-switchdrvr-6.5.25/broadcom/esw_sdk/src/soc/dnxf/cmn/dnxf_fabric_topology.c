/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * SOC DNXF FABRIC TOPOLOGY
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_FABRIC
#include <shared/bsl.h>

#include <soc/dnxf/cmn/dnxf_drv.h>
#include <soc/dnxc/error.h>
#include <soc/defs.h>
#include <soc/error.h>

/**
 * \brief
 *   Map FAP to a group.
 *
 * \param [in] unit -
 *   The unit number.
 * \param [in] modid -
 *   The FAP's module id.
 * \param [in] group -
 *   The group to map the FAP into.
 * \return
 *   See \ref shr_error_e.
 * \remark
 *   None.
 * \see
 *   None.
 */
shr_error_e
soc_dnxf_fabric_topology_modid_group_map_set(
    int unit,
    soc_module_t modid,
    int group)
{
    uint32 val = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_RTP_FLGMm(unit, MEM_BLOCK_ANY, modid, &val));
    soc_mem_field_set(unit, RTP_FLGMm, &val, GROUP_NUMf, (uint32 *) &group);
    SHR_IF_ERR_EXIT(WRITE_RTP_FLGMm(unit, MEM_BLOCK_ALL, modid, &val));

    /*
     * Force MCLBT calculation 
     */
    SHR_IF_ERR_EXIT(READ_RTP_ALL_REACHABLE_AND_GROUP_REACHABLE_CFGr(unit, &val));
    soc_reg_field_set(unit, RTP_ALL_REACHABLE_AND_GROUP_REACHABLE_CFGr, &val, ALRC_FORCE_CALCULATIONf, 1);
    SHR_IF_ERR_EXIT(WRITE_RTP_ALL_REACHABLE_AND_GROUP_REACHABLE_CFGr(unit, val));

    sal_usleep(1000);

    soc_reg_field_set(unit, RTP_ALL_REACHABLE_AND_GROUP_REACHABLE_CFGr, &val, ALRC_FORCE_CALCULATIONf, 0);
    SHR_IF_ERR_EXIT(WRITE_RTP_ALL_REACHABLE_AND_GROUP_REACHABLE_CFGr(unit, val));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Enable SW group link configuration (disable dynamic update).
 *
 * \param [in] unit -
 *   The unit number.
 * \param [in] group -
 *   The FAP's module id.
 * \param [in] enable -
 *   Enable/disable SW configuration.
 * \return
 *   See \ref shr_error_e.
 * \remark
 *   None.
 * \see
 *   None.
 */
shr_error_e
soc_dnxf_fabric_topology_mc_sw_control_set(
    int unit,
    int group,
    int enable)
{
    soc_reg_above_64_val_t reg_val;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_RTP_MCLBT_GROUP_STATIC_CONFIGr(unit, reg_val));
    SHR_BITWRITE(reg_val, group, enable);
    SHR_IF_ERR_EXIT(WRITE_RTP_MCLBT_GROUP_STATIC_CONFIGr(unit, reg_val));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get is enable SW group link configuration (disable dynamic update).
 *
 * \param [in] unit -
 *   The unit number.
 * \param [in] group -
 *   The FAP's module id.
 * \param [out] enable -
 *   Is SW configuration is enabled.
 * \return
 *   See \ref shr_error_e.
 * \remark
 *   None.
 * \see
 *   None.
 */
shr_error_e
soc_dnxf_fabric_topology_mc_sw_control_get(
    int unit,
    int group,
    int *enable)
{
    soc_reg_above_64_val_t reg_val;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_RTP_MCLBT_GROUP_STATIC_CONFIGr(unit, reg_val));
    *enable = (SHR_BITGET(reg_val, group)) ? (TRUE) : (FALSE);

exit:
    SHR_FUNC_EXIT;
}

#undef BSL_LOG_MODULE
