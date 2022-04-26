/*
 * ! \file ctest_dnx_port_util.h Contains common dnx ctest port utilities
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#include <shared/bsl.h>
#include <bcm_int/dnx/port/imb/imb.h>
#include <shared/shrextend/shrextend_debug.h>
#include <bcm/types.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>

/** sal */
#include <sal/appl/sal.h>
#include <sal/appl/config.h>
#include <sal/appl/field_types.h>

#include <shared/dbx/dbx_file.h>

#include <soc/sand/sand_aux_access.h>

#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/ctest/dnxc/ctest_dnxc_utils.h>

#include <appl/ctest/dnx/ctest_dnx_port_utils.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_PORT

shr_error_e
ctest_dnx_port_ucode_port_type_group_get(
    int unit,
    int speed,
    int lanes,
    char **ucode_port_type_p,
    char **port_group_p)
{
    char *ucode_port_type;
    char *port_group;

    SHR_FUNC_INIT_VARS(unit);
    switch (speed)
    {
        case 400000:
        {
            ucode_port_type = "CDGE";
            port_group = "cd";
            break;
        }
        case 200000:
        {
            ucode_port_type = "CCGE";
            port_group = "cc";
            break;
        }
        case 100000:
        {
            if (lanes == 2)
            {
                ucode_port_type = "CGE2_";
            }
            else
            {
                ucode_port_type = "CGE";
            }
            port_group = "ce";
            break;
        }
        case 50000:
        {
            if (lanes == 2)
            {
                ucode_port_type = "XLGE2_";
                port_group = "xl";
            }
            else
            {
                ucode_port_type = "LGE";
                port_group = "le";
            }
            break;
        }
        case 40000:
        {
            if (lanes == 2)
            {
                ucode_port_type = "XLGE2_";
            }
            else
            {
                ucode_port_type = "XLGE";
            }
            port_group = "xl";
            break;
        }
        case 25000:
        case 20000:
        case 12000:
        case 10000:
        case 5000:
        {
            ucode_port_type = "XE";
            port_group = "xe";
            break;
        }
        case 1000:
        {
            ucode_port_type = "GE";
            port_group = "ge";
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported speed: %d lane: %d\n", speed, lanes);
        }
    }
    *ucode_port_type_p = ucode_port_type;
    *port_group_p = port_group;
exit:
    SHR_FUNC_EXIT;
}

#ifdef BCM_DNX2_SUPPORT
static shr_error_e
ctest_dnx_device_pms_get(
    int unit,
    bcm_pbmp_t * cdu_pms,
    bcm_pbmp_t * clu_pms)
{
    int ethu_nof = dnx_data_nif.eth.ethu_nof_get(unit);
    int ethu_id;
    int dispatch_type;
    int nof_pms_in_cdu, nof_pms_in_clu;
    const int *pms;
    int id;

    SHR_FUNC_INIT_VARS(unit);

    BCM_PBMP_CLEAR(*cdu_pms);
    BCM_PBMP_CLEAR(*clu_pms);
    nof_pms_in_cdu = dnx_data_nif.eth.nof_pms_in_cdu_get(unit);
    nof_pms_in_clu = dnx_data_nif.eth.nof_pms_in_clu_get(unit);

    for (ethu_id = 0; ethu_id < ethu_nof; ethu_id++)
    {
        dispatch_type = dnx_data_nif.eth.ethu_properties_get(unit, ethu_id)->type;
        pms = dnx_data_nif.eth.ethu_properties_get(unit, ethu_id)->pms;

        switch (dispatch_type)
        {
            case imbDispatchTypeImb_cdu:
                for (id = 0; id < nof_pms_in_cdu; id++)
                {
                    BCM_PBMP_PORT_ADD(*cdu_pms, pms[id]);
                }

                /*
                 * nof_ethu_lanes_in_pm = dnx_data_nif.eth.nof_cdu_lanes_in_pm_get(unit);
                 */
                break;
            case imbDispatchTypeImb_clu:
                for (id = 0; id < nof_pms_in_clu; id++)
                {
                    BCM_PBMP_PORT_ADD(*clu_pms, pms[id]);
                }
                /*
                 * nof_ethu_lanes_in_pm = dnx_data_nif.eth.nof_clu_lanes_in_pm_get(unit);
                 */
                break;
            default:
                SHR_CLI_EXIT(_SHR_E_FAIL, "Wrong dispatch_type =%d\n", dispatch_type);
        }
    }

exit:
    SHR_FUNC_EXIT;
}
#endif

void
ctest_dnx_pm_first_last_phy_get(
    int unit,
    int pm_id,
    int *first_phy,
    int *last_phy)
{
    bcm_pbmp_t pm_phys = dnx_data_nif.eth.pm_properties_get(unit, pm_id)->phys;
    _SHR_PBMP_FIRST(pm_phys, *first_phy);
    _SHR_PBMP_LAST(pm_phys, *last_phy);
}

shr_error_e
ctest_dnx_port_first_last_phy_get(
    int unit,
    int port,
    int *first_phy,
    int *last_phy)
{
    uint32 flags;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_port_get(unit, port, &flags, &interface_info, &mapping_info));
    *first_phy = interface_info.phy_port;
    *last_phy = *first_phy + interface_info.num_lanes - 1;
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - get free pms in system.
 * \param [in] unit                 - Number of hardware unit used
 * \param [in] ethu_type            - 0/1/-1, 1 for cdu, 0 for clu, -1 for all
 * \param [in] core                 - 0/1/-1, -1 for all cores
 * \param [in] reserved_port_pbmp   - Reserved port pbmp in system
 * \param [in] reserved_phy_pbmp    - Reserved phy pbmp in system
 * \param [out] free pms            - All cdu/clu pms can be used in sytem excluding pms with
 *                                    the phys used by reserved port and reserved phy
 */
shr_error_e
ctest_dnx_port_free_pms_get(
    int unit,
    int ethu_type,
    int core,
    bcm_pbmp_t reserved_port_pbmp,
    bcm_pbmp_t reserved_phy_pbmp,
    bcm_pbmp_t * free_pms)
{
    bcm_pbmp_t cdu_pms, clu_pms, ethu_pms;
    bcm_pbmp_t support_phy_pbmp;
    int pm_id;
    int pm_is_free;
    int first_phy_of_pm;
    int last_phy_of_pm;
    int first_phy, last_phy;
    int core_id;
    int port, phy;
    int nof_ethu_pms;

    SHR_FUNC_INIT_VARS(unit);

    support_phy_pbmp = dnx_data_nif.phys.general_get(unit)->supported_phys;
    nof_ethu_pms = dnx_data_nif.eth.total_nof_ethu_pms_in_device_get(unit);
#ifdef BCM_DNX2_SUPPORT

    ctest_dnx_device_pms_get(unit, &cdu_pms, &clu_pms);
#endif
    if (ethu_type == 1)
    {
        ethu_pms = cdu_pms;
    }
    else if (ethu_type == 0)
    {
        ethu_pms = clu_pms;
    }
    else if (ethu_type == -1)
    {
        BCM_PBMP_OR(ethu_pms, cdu_pms);
        BCM_PBMP_OR(ethu_pms, clu_pms);
    }
    BCM_PBMP_CLEAR(*free_pms);
    for (pm_id = 0; pm_id < nof_ethu_pms; pm_id++)
    {
        pm_is_free = 1;
        ctest_dnx_pm_first_last_phy_get(unit, pm_id, &first_phy_of_pm, &last_phy_of_pm);
        for (phy = first_phy_of_pm; phy <= last_phy_of_pm; phy++)
        {

            if (!BCM_PBMP_MEMBER(support_phy_pbmp, phy))
            {
                pm_is_free = 0;
                break;
            }

        }
        BCM_PBMP_ITER(reserved_port_pbmp, port)
        {
            SHR_IF_ERR_EXIT(ctest_dnx_port_first_last_phy_get(unit, port, &first_phy, &last_phy));
            if (first_phy >= first_phy_of_pm && last_phy <= last_phy_of_pm)
            {
                pm_is_free = 0;
                break;
            }
        }

        BCM_PBMP_ITER(reserved_phy_pbmp, phy)
        {
            if (phy >= first_phy_of_pm && phy <= last_phy_of_pm)
            {
                pm_is_free = 0;
                break;
            }
        }
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_phy_to_core_id_get(unit, first_phy_of_pm, &core_id));

        if (pm_is_free)
        {
            if (core == -1)
            {
                BCM_PBMP_PORT_ADD(*free_pms, pm_id);
            }
            else if (core == core_id)
            {
                BCM_PBMP_PORT_ADD(*free_pms, pm_id);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
ctest_dnx_port_type_pbmp_get(
    int unit,
    char *port_type,
    bcm_pbmp_t * pbmp)
{
    static int port_type_size = 8;
    bcm_port_config_t port_config;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_port_config_get(unit, &port_config));
    if (!sal_strncmp(port_type, "port", port_type_size))
    {
        *pbmp = port_config.port;
    }
    else if (!sal_strncmp(port_type, "cpu", port_type_size))
    {
        *pbmp = port_config.cpu;
    }
    else if (!sal_strncmp(port_type, "nif", port_type_size))
    {
        *pbmp = port_config.nif;
    }
    else if (!sal_strncmp(port_type, "il", port_type_size))
    {
        *pbmp = port_config.il;
    }
    else if (!sal_strncmp(port_type, "stat", port_type_size))
    {
        *pbmp = port_config.stat;
    }
    else if (!sal_strncmp(port_type, "sat", port_type_size))
    {
        *pbmp = port_config.sat;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported port_type \"%s\"\n", port_type);
    }

exit:
    SHR_FUNC_EXIT;

}
