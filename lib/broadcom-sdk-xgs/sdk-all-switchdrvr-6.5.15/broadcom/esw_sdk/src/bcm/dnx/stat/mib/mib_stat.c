/** \file mib_stat.c
 * $Id$
 *
 * MIB stat procedures for DNX.
 *
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COMMON

#include <soc/dnx/dbal/dbal.h>
#include <shared/utilex/utilex_bitstream.h>
#include <shared/shrextend/shrextend_error.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_mib.h>
#include <soc/dnx/stat/jer2_stat.h>
#include <soc/drv.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/init/init.h>
#include <soc/counter.h>
#include <bcm/types.h>
#include <bcm/error.h>

#include <sal/types.h>
#include "mib_stat_dbal.h"

extern soc_controlled_counter_t soc_jer2_controlled_counter[];

#define MAX_COUNTERS          ( 4 )

/** see .h file */
shr_error_e
dnx_mib_init(
    int unit)
{
    const dnx_data_mib_general_stat_pbmp_t *stat_pbmp;
    soc_control_t *soc = SOC_CONTROL(unit);
    bcm_pbmp_t fabric_ports_bitmap;
    bcm_pbmp_t nif_eth_ports_bitmap;
    bcm_pbmp_t nif_ilkn_ports_bitmap;
    SHR_FUNC_INIT_VARS(unit);

    SOC_CONTROL(unit)->controlled_counters = soc_jer2_controlled_counter;
    SHR_IF_ERR_EXIT(soc_counter_attach(unit));
    if (!SOC_WARM_BOOT(unit))
    {
        SHR_IF_ERR_EXIT(mib_stat_init(unit));
    }

    stat_pbmp = dnx_data_mib.general.stat_pbmp_get(unit);
    /*
     * This stage is after fabric init. Here, only fabric ports are
     * added to counter bitmap. For nif ports, they are added in
     * dnx_counter_port_add() during adding ports stage.
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                    (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_FABRIC, 0, &fabric_ports_bitmap));
    BCM_PBMP_AND(fabric_ports_bitmap, stat_pbmp->pbmp);
    /*
     * Druing Warm boot, add nif ports to counter bitmap here.
     * Because dnx_counter_port_add() was not called in adding ports stage
     */
    if (SOC_WARM_BOOT(unit))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                        (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_NIF_ETH, DNX_ALGO_PORT_LOGICALS_F_MASTER_ONLY,
                         &nif_eth_ports_bitmap));
        SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                        (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_NIF_ILKN, 0, &nif_ilkn_ports_bitmap));

        BCM_PBMP_AND(nif_eth_ports_bitmap, stat_pbmp->pbmp);
        BCM_PBMP_AND(nif_ilkn_ports_bitmap, stat_pbmp->pbmp);
        BCM_PBMP_OR(soc->counter_pbmp, nif_eth_ports_bitmap);
        BCM_PBMP_OR(soc->counter_pbmp, nif_ilkn_ports_bitmap);
    }
    /*
     * update counters bitmap in case counter thread is not start
     */
    BCM_PBMP_OR(soc->counter_pbmp, fabric_ports_bitmap);

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief -  Initialize mib stat
*
* \param [in] unit - unit id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e
mib_stat_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(mib_stat_fabric_init(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Deinit the BCM statistics module
 *
 * \param [in] unit - chip unit id.
 * \return
 *   shr_error_e - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */

int
bcm_dnx_stat_stop(
    int unit)
{
    int rc = _SHR_E_NONE;
    SHR_FUNC_INIT_VARS(unit);

    rc = soc_counter_stop(unit);
    SHR_IF_ERR_EXIT(rc);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for bcm_dnx_stat_clear() API
 */
static shr_error_e
dnx_stat_clear_verify(
    int unit,
    bcm_port_t port)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * make sure port argumet is local port, not Gport.
     */
    if (BCM_GPORT_IS_SET(port))
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "given port: 0x%x is not resolved as a local port.\r\n", port);
    }
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, port));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Clear the port-based statistics for the indicated device port
 *
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port.
 * \return
 *   shr_error_e - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_stat_clear(
    int unit,
    bcm_port_t port)
{
    int rv;
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_stat_clear_verify(unit, port));

    if (BCM_PBMP_MEMBER(PBMP_CMIC(unit), port))
    {
        /*
         * Rudimentary CPU statistics -- needs soc_reg_twork 
         */
        SOC_CONTROL(unit)->stat.dma_rbyt = 0;
        SOC_CONTROL(unit)->stat.dma_rpkt = 0;
        SOC_CONTROL(unit)->stat.dma_tbyt = 0;
        SOC_CONTROL(unit)->stat.dma_tpkt = 0;
        SHR_EXIT();
    }

    if (_SOC_CONTROLLED_COUNTER_USE(unit, port))
    {
        rv = soc_controlled_counter_clear(unit, port);
        SHR_IF_ERR_EXIT(rv);
    }

    if (!_SOC_CONTROLLED_COUNTER_USE(unit, port) && soc_feature(unit, soc_feature_generic_counters))
    {
        bcm_port_t port_base;
        int phy_port;
        pbmp_t pbm;

        /*
         * get port base for channelized case 
         */
        phy_port = SOC_INFO(unit).port_l2p_mapping[port];
        port_base = SOC_INFO(unit).port_p2l_mapping[phy_port];

        BCM_PBMP_CLEAR(pbm);
        BCM_PBMP_PORT_ADD(pbm, port_base);
        SHR_IF_ERR_EXIT(soc_counter_set32_by_port(unit, pbm, 0));

    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Synchronize software counters with hardware
 *
 * \param [in] unit - chip unit id.
 * \return
 *   shr_error_e - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_stat_sync(
    int unit)
{
    int rc = _SHR_E_NONE;
    SHR_FUNC_INIT_VARS(unit);

    rc = soc_counter_sync(unit);
    SHR_IF_ERR_EXIT(rc);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for bcm_dnx_stat_get() API
 */
static shr_error_e
dnx_stat_get_verify(
    int unit,
    bcm_port_t port,
    bcm_stat_val_t type,
    uint64 *value)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * make sure port argumet is local port, not Gport. 
     */
    if (BCM_GPORT_IS_SET(port))
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "given port: 0x%x is not resolved as a local port.\r\n", port);
    }
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, port));

    SHR_NULL_CHECK(value, _SHR_E_PARAM, "value");

    /*
     * Check valid type
     */
    if (type < 0 || type >= snmpValCount)
    {
        return _SHR_E_PARAM;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See h. for reference
 */
shr_error_e
dnx_stat_get_by_counter_idx(
    int unit,
    bcm_port_t port,
    bcm_stat_val_t type,
    int counter_idx,
    uint64 *value)
{
    int supported = 0, printable;
    int rv;
    SHR_FUNC_INIT_VARS(unit);

    rv = soc_jer2_stat_controlled_counter_enable_get(unit, port, counter_idx, &supported, &printable);
    SHR_IF_ERR_EXIT(rv);
    if (!supported)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Statistic type %d (counter index %d) is not supported for port %d\n",
                     type, counter_idx, port);
    }
    if (COUNTER_IS_COLLECTED(SOC_CONTROL(unit)->controlled_counters[counter_idx])
        && SOC_CONTROL(unit)->counter_interval != 0)
    {
        /*
         * counter is collected by counter thread
         */
        rv = soc_counter_get(unit, port, counter_idx, 0, value);
    }
    else
    {
        /*
         * counter isn't collected by counter thread
         */
        rv = SOC_CONTROL(unit)->controlled_counters[counter_idx].controlled_counter_f(unit,
                                                                                      SOC_CONTROL
                                                                                      (unit)->controlled_counters
                                                                                      [counter_idx].counter_id,
                                                                                      port, value, NULL);
    }
    SHR_IF_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get the specified statistics from the device for
 *        the port.
 *
 * \param [in] unit - chip unit id.
 * \param [in] port - logical port.
 * \param [in]  type  - SNMP statistics type defined in bcm_stat_val_t.
 * \param [out] value - collected 64-bit statistics values. 
 *   
 * \return
 *   shr_error_e - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_stat_get(
    int unit,
    bcm_port_t port,
    bcm_stat_val_t type,
    uint64 *value)
{
    int rv = _SHR_E_NONE;
    uint32 add_cnt_type[MAX_COUNTERS], sub_cnt_type[MAX_COUNTERS];
    int add_num_cntrs, sub_num_cntrs;
    int cntr_index;
    uint64 val;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_stat_get_verify(unit, port, type, value));

    sal_memset(add_cnt_type, 0xffffffff, sizeof(add_cnt_type));
    sal_memset(sub_cnt_type, 0xffffffff, sizeof(sub_cnt_type));

    add_num_cntrs = sub_num_cntrs = MAX_COUNTERS;

    if (BCM_PBMP_MEMBER(PBMP_CMIC(unit), port))
    {
        /*
         * Rudimentary CPU statistics -- needs work 
         */
        switch (type)
        {
            case snmpIfInOctets:
                COMPILER_64_SET(*value, 0, SOC_CONTROL(unit)->stat.dma_rbyt);
                break;
            case snmpIfInUcastPkts:
                COMPILER_64_SET(*value, 0, SOC_CONTROL(unit)->stat.dma_rpkt);
                break;
            case snmpIfOutOctets:
                COMPILER_64_SET(*value, 0, SOC_CONTROL(unit)->stat.dma_tbyt);
                break;
            case snmpIfOutUcastPkts:
                COMPILER_64_SET(*value, 0, SOC_CONTROL(unit)->stat.dma_tpkt);
                break;
            default:
                COMPILER_64_ZERO(*value);
                break;
        }
        SHR_EXIT();
    }

    if ((_SOC_CONTROLLED_COUNTER_USE(unit, port)))
    {

        rv = soc_jer2_mapping_stat_get(unit, port, add_cnt_type, &add_num_cntrs,
                                       sub_cnt_type, &sub_num_cntrs, type, MAX_COUNTERS);
        SHR_IF_ERR_EXIT(rv);

        COMPILER_64_ZERO(val);

        for (cntr_index = 0; cntr_index < add_num_cntrs; cntr_index++)
        {
            SHR_IF_ERR_EXIT(dnx_stat_get_by_counter_idx(unit, port, type, add_cnt_type[cntr_index], value));
            COMPILER_64_ADD_64(val, *value);    /* val += (*value) ; */
        }

        for (cntr_index = 0; cntr_index < sub_num_cntrs; cntr_index++)
        {
            SHR_IF_ERR_EXIT(dnx_stat_get_by_counter_idx(unit, port, type, sub_cnt_type[cntr_index], value));
            COMPILER_64_SUB_64(val, *value);    /* val -= (*value) ; */
        }

        *value = val;
    }
    else
    {
        return _SHR_E_UNAVAIL;
    }

    SHR_IF_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for bcm_dnx_stat_multi_get() API
 */
static shr_error_e
dnx_stat_multi_get_verify(
    int unit,
    bcm_port_t port,
    int nstat,
    bcm_stat_val_t * stat_arr,
    uint64 *value_arr)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * make sure port argumet is local port, not Gport. 
     */
    if (BCM_GPORT_IS_SET(port))
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "given port: 0x%x is not resolved as a local port.\r\n", port);
    }
    SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, port));

    if (nstat <= 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid nstat");
    }
    SHR_NULL_CHECK(stat_arr, _SHR_E_PARAM, "stat_arr");
    SHR_NULL_CHECK(value_arr, _SHR_E_PARAM, "value_arr");

exit:
    SHR_FUNC_EXIT;
}

 /**
 * \brief - Get the specified statistics from the device for 
 *        the port.
 * 
 * \param [in]  unit  - chip unit id.
 * \param [in]  port  - logical port.
 * \param [in]  nstat - number of elements in stat array.
 * \param [in]  stat_arr  - array of SNMP statistics types defined in bcm_stat_val_t.
 * \param [out] value_arr - collected 64-bit statistics values. 
 *   
 * \return
 *   shr_error_e - see _SHR_E_*
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_stat_multi_get(
    int unit,
    bcm_port_t port,
    int nstat,
    bcm_stat_val_t * stat_arr,
    uint64 *value_arr)
{
    int rc = _SHR_E_NONE;
    int i;
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_stat_multi_get_verify(unit, port, nstat, stat_arr, value_arr));
    for (i = 0; i < nstat; i++)
    {
        rc = bcm_dnx_stat_get(unit, port, stat_arr[i], &(value_arr[i]));
        SHR_IF_ERR_EXIT(rc);
    }

exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_mib_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * counters detach
     */
    SHR_IF_ERR_EXIT(soc_counter_detach(unit));

exit:
    SHR_FUNC_EXIT;
}

 /**
 * \brief - unsupported for JER2
 * 
 * \return
 *   BCM_E_UNAVAIL
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_stat_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(BCM_E_UNAVAIL, "The API is not supported for JER2.\n");

exit:
    SHR_FUNC_EXIT;
}

 /**
 * \brief - unsupported for JER2 
 * 
 * \return
 *   BCM_E_UNAVAIL
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_stat_get32(
    int unit,
    bcm_port_t port,
    bcm_stat_val_t type,
    uint32 *value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(BCM_E_UNAVAIL, "The API is not supported for JER2.\n");

exit:
    SHR_FUNC_EXIT;
}

 /**
 * \brief - unsupported for JER2 
 * 
 * \return
 *   BCM_E_UNAVAIL
 *   
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_stat_multi_get32(
    int unit,
    bcm_port_t port,
    int nstat,
    bcm_stat_val_t * stat_arr,
    uint32 *value_arr)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(BCM_E_UNAVAIL, "The API is not supported for JER2.\n");

exit:
    SHR_FUNC_EXIT;
}

/*
 * See h. for reference
 */
shr_error_e
dnx_counter_port_add(
    int unit)
{
    bcm_port_t port;
    dnx_algo_port_type_e port_type;
    int is_master_port;
    int is_init_sequence = 0;
    const dnx_data_mib_general_stat_pbmp_t *stat_pbmp;

    SHR_FUNC_INIT_VARS(unit);

    is_init_sequence = !(dnx_init_is_init_done_get(unit));

    /*
     * get logical port information
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_added_port_get(unit, &port));
    SHR_IF_ERR_EXIT(dnx_algo_port_type_get(unit, port, &port_type));

    SHR_IF_ERR_EXIT(dnx_algo_port_is_master_get(unit, port, &is_master_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_type_get(unit, port, &port_type));
    if ((DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_type, TRUE) && is_master_port)
        || DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_type, TRUE))
    {
        if (is_init_sequence)
        {
            stat_pbmp = dnx_data_mib.general.stat_pbmp_get(unit);
            if (BCM_PBMP_MEMBER(stat_pbmp->pbmp, port))
            {
                SHR_IF_ERR_EXIT(soc_counter_port_collect_enable_set(unit, port, 1));
            }
        }
        else
        {
            SHR_IF_ERR_EXIT(soc_counter_port_collect_enable_set(unit, port, 1));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See h. for reference
 */
shr_error_e
dnx_counter_port_remove(
    int unit)
{
    bcm_port_t port;
    dnx_algo_port_type_e port_type;
    int is_master_port;
    int nof_channels = 0;
    bcm_port_t next_master_port = DNX_ALGO_PORT_INVALID;
    int counter_interval;
    uint32 counter_flags;
    bcm_pbmp_t counter_pbmp;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * get logical port information
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_removed_port_get(unit, &port));

    SHR_IF_ERR_EXIT(dnx_algo_port_is_master_get(unit, port, &is_master_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_type_get(unit, port, &port_type));
    SHR_IF_ERR_EXIT(dnx_algo_port_channels_nof_get(unit, port, &nof_channels));
    if ((DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_type, TRUE) && is_master_port)
        || DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_type, TRUE))
    {
        if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_type, TRUE) && is_master_port && nof_channels > 1)
        {
            /** get next master */
            SHR_IF_ERR_EXIT(dnx_algo_port_master_get(unit, port, DNX_ALGO_PORT_MASTER_F_NEXT, &next_master_port));
        }
        SHR_IF_ERR_EXIT(soc_counter_status(unit, &counter_flags, &counter_interval, &counter_pbmp));

        SHR_IF_ERR_EXIT(soc_counter_port_collect_enable_set(unit, port, 0));
        if (next_master_port != DNX_ALGO_PORT_INVALID)
        {
            if (BCM_PBMP_MEMBER(counter_pbmp, port))
            {
                SHR_IF_ERR_EXIT(soc_controlled_counter_update_by_port(unit, next_master_port, port));
                SHR_IF_ERR_EXIT(soc_counter_port_collect_enable_set(unit, next_master_port, 1));
            }
        }
        else
        {
            SHR_IF_ERR_EXIT(soc_controlled_counter_clear(unit, port));
        }
    }

exit:
    SHR_FUNC_EXIT;
}
