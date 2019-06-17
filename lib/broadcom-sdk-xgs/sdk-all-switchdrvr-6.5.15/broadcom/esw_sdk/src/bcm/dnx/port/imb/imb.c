/*
 *         
 * $Id:$
 * 
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *         
 *     
 */
#include <sal/types.h>
#include <sal/core/alloc.h>
#include <soc/defs.h>
#include <soc/drv.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/common/debug.h>
#include <bcm_int/dnx/port/imb/imb.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/port/imb/imb_dispatch.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_port_imb_access.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_port_imb_types.h>
#include <bcm_int/dnx/port/imb/imb_common.h>

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

/**
 * \brief - get imb_type from imb_id
 *
 * \param [in] unit   - chip unit id.
 * \param [in] imb_id - imb id.
 * \param [in] type   - imb dispatcher type.
 *
 *
 * \return
 *   int  - err code. see _SHR_E_*
 *
 * \see
 *   * None
 */
int
imb_id_type_get(
    int unit,
    int imb_id,
    imb_dispatch_type_t * type)
{
    imb_create_info_t imb;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imbm.imb.get(unit, imb_id, &imb));
    *type = imb.type;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get imb_type from imb_id
 *
 * \param [in]  unit     - chip unit id.
 * \param [out] nof_imbs - number of imb instances in use
 *
 *
 * \return
 *   int  - err code. see _SHR_E_*
 *
 * \see
 *   * None
 */
int
imb_nof_get(
    int unit,
    uint32 *nof_imbs)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imbm.imbs_in_use.get(unit, nof_imbs));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - initialize the IMB (Interface Managment Block) SW 
 *        structure.
 * 
 * \param [in] unit - chip unit id.
 *   
 * \return
 *   int  - err code. see _SHR_E_*
 *   
 * \remark
 *   * this method should be called in init sequence
 *        regardless of phys/PMs actually in use by the user.
 * \see
 *   * None
 */
int
imb_init_all(
    int unit)
{

    bcm_port_t port;
    uint32 index, imb_id, nof_types, nof_cdus, nof_ilkn_units, nof_fabric_pms, nof_imbs;
    imb_create_info_t imb;
    portmod_pm_instances_t pm_types_and_instances[DNX_DATA_MAX_NIF_PORTMOD_PM_TYPES_NOF];
    SHR_FUNC_INIT_VARS(unit);

    nof_types = dnx_data_nif.portmod.pm_types_nof_get(unit);
    nof_cdus = dnx_data_nif.eth.cdu_nof_get(unit);
    nof_ilkn_units = dnx_data_nif.ilkn.ilkn_unit_nof_get(unit);
    nof_fabric_pms = dnx_data_fabric.blocks.nof_pms_get(unit);
    nof_imbs = nof_cdus + nof_ilkn_units + nof_fabric_pms;

    if (!SOC_WARM_BOOT(unit))
    {
        /*
         * Allocate a IMBM for this unit
         */
        SHR_IF_ERR_EXIT(imbm.init(unit));
        SHR_IF_ERR_EXIT(imbm.imbs_in_use.set(unit, 0));
        SHR_IF_ERR_EXIT(imbm.imb.alloc(unit, nof_imbs));

        for (port = 0; port < SOC_MAX_NUM_PORTS; ++port)
        {
            SHR_IF_ERR_EXIT(imbm.imb_type.set(unit, port, imbDispatchTypeNone));
        }
    }
    /*
     * Initialize portmod module 
     */
    for (index = 0; index < nof_types; ++index)
    {
        pm_types_and_instances[index].instances =
            dnx_data_nif.portmod.pm_types_and_interfaces_get(unit, index)->instances;
        pm_types_and_instances[index].type = dnx_data_nif.portmod.pm_types_and_interfaces_get(unit, index)->type;
    }
    SHR_IF_ERR_EXIT(portmod_create(unit, 0, SOC_MAX_NUM_PORTS, SOC_MAX_NUM_PORTS, nof_types, pm_types_and_instances));

    imb_id = 0;
    /*
     * Init all CDU units
     */
    for (index = 0; index < nof_cdus; ++index)
    {
        SHR_IF_ERR_EXIT(imbm.imb.get(unit, imb_id, &imb));
        imb.type = imbDispatchTypeImb_cdu;
        imb.inst_id = index;
        SHR_IF_ERR_EXIT(imb_init(unit, &imb, &imb.imb_specific_info));
        SHR_IF_ERR_EXIT(imbm.imb.set(unit, imb_id, imb));
        ++imb_id;
    }

    /*
     * Init all Fabric units
     */
    for (index = 0; index < nof_fabric_pms; ++index)
    {
        SHR_IF_ERR_EXIT(imbm.imb.get(unit, imb_id, &imb));
        imb.type = imbDispatchTypeImb_fabric;
        imb.inst_id = index;
        SHR_IF_ERR_EXIT(imb_init(unit, &imb, &imb.imb_specific_info));
        SHR_IF_ERR_EXIT(imbm.imb.set(unit, imb_id, imb));
        ++imb_id;
    }

    /*
     * Init all ILKN units
     */
    for (index = 0; index < nof_ilkn_units; ++index)
    {
        SHR_IF_ERR_EXIT(imbm.imb.get(unit, imb_id, &imb));
        imb.type = imbDispatchTypeImb_ile;
        imb.inst_id = index;
        SHR_IF_ERR_EXIT(imb_init(unit, &imb, &imb.imb_specific_info));
        SHR_IF_ERR_EXIT(imbm.imb.set(unit, imb_id, imb));
        ++imb_id;
    }

    if (!SOC_WARM_BOOT(unit))
    {
        SHR_IF_ERR_EXIT(imbm.imbs_in_use.set(unit, imb_id));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - De-init IMB SW structure. main functionality is to 
 *        free dynamically allocated memory.
 * 
 * \param [in] unit - chip unit id.
 *   
 * \return
 *   int - err code. see _SHR_E_*
 *   
 * \remark
 *   * None
 *  
 * \see
 *   * None
 */
int
imb_deinit_all(
    int unit)
{
    int index, nof_cdus, nof_ilkn_units, nof_imbs, nof_fabric_pms;
    imb_create_info_t imb;
    uint8 is_init = 0;
    SHR_FUNC_INIT_VARS(unit);

    nof_cdus = dnx_data_nif.eth.cdu_nof_get(unit);
    nof_ilkn_units = dnx_data_nif.ilkn.ilkn_unit_nof_get(unit);
    nof_fabric_pms = dnx_data_fabric.blocks.nof_pms_get(unit);
    nof_imbs = nof_cdus + nof_ilkn_units + nof_fabric_pms;

    /*
     * de-init Portmod module 
     */
    SHR_IF_ERR_CONT(portmod_destroy(unit));
    /*
     * de-init all IMBs
     */
    SHR_IF_ERR_CONT(imbm.is_init(unit, &is_init));
    if (is_init == TRUE)
    {
        for (index = 0; index < nof_imbs; ++index)
        {
            /*
             * de-init should continue even if one of the method fails
             */
            SHR_IF_ERR_CONT(imbm.imb.get(unit, index, &imb));
            SHR_IF_ERR_CONT(imb_deinit(unit, &imb, &imb.imb_specific_info));
        }
    }

    /*
     * sw state module deinit is done automatically at device deinit 
     */

    SHR_FUNC_EXIT;
}

int
imb_port_add(
    int unit,
    bcm_port_t port,
    uint32 flags)
{
    imb_dispatch_type_t imb_type;
    dnx_algo_port_type_e port_type;
    SHR_FUNC_INIT_VARS(unit);

    if (IMB_PORT_ADD_F_SKIP_SETTINGS_GET(flags) == FALSE)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_type_get(unit, port, &port_type));

        switch (port_type)
        {
            case DNX_ALGO_PORT_TYPE_NIF_ILKN_ELK:
                imb_type = imbDispatchTypeImb_ile;
                break;
            case DNX_ALGO_PORT_TYPE_NIF_ETH:
            case DNX_ALGO_PORT_TYPE_NIF_ETH_STIF:
                imb_type = imbDispatchTypeImb_cdu;
                break;
            case DNX_ALGO_PORT_TYPE_FABRIC:
                imb_type = imbDispatchTypeImb_fabric;
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_PARAM, "Port %d interface type is not supported in IMB", port);
        }
        SHR_IF_ERR_EXIT(imbm.imb_type.set(unit, port, imb_type));

        IMB_PORT_ADD_F_SKIP_SETTINGS_CLR(flags);
    }

    SHR_IF_ERR_EXIT(imb_port_attach(unit, port, flags));

exit:
    SHR_FUNC_EXIT;
}

int
imb_port_remove(
    int unit,
    bcm_port_t port)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_port_detach(unit, port));

    SHR_IF_ERR_EXIT(imbm.imb_type.set(unit, port, imbDispatchTypeNone));

exit:
    SHR_FUNC_EXIT;
}

int
imb_llfc_from_glb_rsc_enable_all(
    int unit,
    uint32 enable)
{
    int index;
    uint32 nof_imbs;
    imb_create_info_t imb;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imbm.imbs_in_use.get(unit, &nof_imbs));

    for (index = 0; index < nof_imbs; ++index)
    {
        SHR_IF_ERR_EXIT(imbm.imb.get(unit, index, &imb));
        if (__imb__dispatch__[imb.type]->f_imb_llfc_from_glb_rsc_enable_set != NULL)
        {
            SHR_IF_ERR_EXIT(__imb__dispatch__[imb.type]->f_imb_llfc_from_glb_rsc_enable_set(unit, &imb, enable));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

int
imb_fc_reset_all(
    int unit,
    uint32 in_reset)
{
    int index;
    uint32 nof_imbs;
    imb_create_info_t imb;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imbm.imbs_in_use.get(unit, &nof_imbs));

    for (index = 0; index < nof_imbs; ++index)
    {
        SHR_IF_ERR_EXIT(imbm.imb.get(unit, index, &imb));
        if (__imb__dispatch__[imb.type]->f_imb_fc_reset_set != NULL)
        {
            SHR_IF_ERR_EXIT(__imb__dispatch__[imb.type]->f_imb_fc_reset_set(unit, &imb, in_reset));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

#undef BSL_LOG_MODULE
