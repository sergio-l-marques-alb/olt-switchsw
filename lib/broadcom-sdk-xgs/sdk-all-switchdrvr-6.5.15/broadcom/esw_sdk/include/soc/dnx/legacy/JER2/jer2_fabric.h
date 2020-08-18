/** \file jer2_fabric.h
 *
 * Functions for handling Fabric.
 *
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef __JER2_FABRIC_INCLUDED__
/* { */
#define __JER2_FABRIC_INCLUDED__

#ifndef BCM_DNX_SUPPORT 
#error "This file is for use by DNX (Jer2) family only!" 
#endif

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dnx/legacy/cosq.h>
#include <soc/dnx/legacy/fabric.h>

/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */
/*
 * \brief
 *   Convert logical fabric port to fmac and lane
 */
void
soc_jer2_fabric_port_to_fmac(int unit, soc_port_t port, int *fmac_index, int *fmac_inner_link);

/**
 * \brief
 *   Initialize load-balancing related configurations.
 * \param [in] unit -
 *   The unit number.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * Called as part of the initialization sequence.
 * \see
 *   * None.
 */
shr_error_e soc_jer2_fabric_load_balance_init(
    int unit);

/**
 * \brief
 *   Initialization of scheduler fabric links adaptation.
 * \param [in] unit -
 *   The unit number.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * Called as part of the initialization sequence.
 *   * Not relevant for single core device.
 * \see
 *   * None.
 */
shr_error_e soc_jer2_fabric_scheduler_adaptation_init(
    int unit);
	
/**
 * \brief
 *   Configure ALDWP value.
 *   ALDWP = Active Link Down Watchdog Period.
 * \param [in] unit -
 *   The unit number.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * Called as part of the initialization sequence.
 * \see
 *   * None.
 */
shr_error_e soc_jer2_fabric_aldwp_config(
   int unit);

/**
 * \brief
 *   Initialize minimal links per destination device feature.
 * \param [in] unit -
 *   The unit number.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * Called as part of the initialization sequence.
 * \see
 *   * None.
 */
shr_error_e soc_jer2_fabric_minimal_links_to_dest_init(
    int unit);

/**
 * \brief
 *   Set minimal links per destination device.
 *   Once the number of active links of a destination device is lower than
 *   the configurable value, all of its cells will be dropped.
 * \param [in] unit -
 *   The unit number.
 * \param [in] module_id -
 *   Module id of the device to configure.
 *   SOC_MODID_ALL will configure all devices.
 * \param [in] min_links -
 *   The number of links to configure as minimum.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None.
 * \see
 *   * None.
 */
shr_error_e soc_jer2_fabric_minimal_links_to_dest_set(
    int          unit,
    soc_module_t module_id,
    int          min_links);

/**
 * \brief
 *   Get minimal links per destination device.
 *   Once the number of active links of a destination device is lower than
 *   the configurable value, all of its cells will be dropped.
 * \param [in] unit -
 *   The unit number.
 * \param [in] module_id -
 *   Module id of the device to get configuration.
 *   SOC_MODID_ALL will get configuration of device 0 (all devices was
 *   configured the same).
 * \param [out] min_links -
 *   The number of links that was configured as minimum.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None.
 * \see
 *   * None.
 */
shr_error_e soc_jer2_fabric_minimal_links_to_dest_get(
    int           unit,
    soc_module_t  module_id,
    int          *min_links);

/**
 * \brief
 *   Set minimal links for all-reachable vactor.
 *   In calculation of multicast distribution links,
 *   ignore FAPs with number of links below this number.
 * \param [in] unit -
 *   The unit number.
 * \param [in] min_links -
 *   The number of links to configure as minimum.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None.
 * \see
 *   * None.
 */
shr_error_e soc_jer2_fabric_minimal_links_all_reachable_set(
    int unit,
    int min_links);

/**
 * \brief
 *   Get minimal links for all-reachable vactor.
 *   In calculation of multicast distribution links,
 *   ignore FAPs with number of links below this number.
 * \param [in] unit -
 *   The unit number.
 * \param [out] min_links -
 *   The number of links that was configured as minimum.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None.
 * \see
 *   * None.
 */
shr_error_e soc_jer2_fabric_minimal_links_all_reachable_get(
    int  unit,
    int *min_links);

/**
 * \brief
 *   Initialize fabric contexts interleaving.
 * \param [in] unit -
 *   The unit number.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * Called as part of the initialization sequence.
 * \see
 *   * None.
 */
shr_error_e soc_jer2_fabric_interleaving_init(
    int unit);

/**
 * \brief
 *   Set the max amount of time to wait for a second cell
 *   if "delay single cell in fabric rx" feature is enabled.
 * \param [in] unit -
 *   The unit number.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * Called as part of the initialization sequence.
 * \see
 *   * soc_jer2_fabric_delay_single_cell_in_fabric_rx_enable_set.
 */
shr_error_e soc_jer2_fabric_delay_single_cell_in_fabric_rx_init(
    int unit);

/**
 * \brief
 *   Set whether a single cell should be delayed in Fabric RX
 *   until a second cell will arrive (or timeout).
 *   This feature should be enabled when there is a JR1 device
 *   in the system.
 *   The amount of time to wait for a second cell is configured
 *   in the init sequence.
 * \param [in] unit -
 *   The unit number.
 * \param [in] enable -
 *   1 - enable delay.
 *   0 - disable delay.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None.
 * \see
 *   * None.
 */
shr_error_e soc_jer2_fabric_delay_single_cell_in_fabric_rx_enable_set(
    int unit,
    int enable);

/**
 * \brief
 *   Get whether a single cell should be delayed in Fabric RX
 *   until a second cell will arrive (or timeout).
 * \param [in] unit -
 *   The unit number.
 * \param [out] enable -
 *   1 - enable delay.
 *   0 - disable delay.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None.
 * \see
 *   * None.
 */
shr_error_e soc_jer2_fabric_delay_single_cell_in_fabric_rx_enable_get(
    int unit,
    int *enable);

/**
 * \brief
 *   Set fabric force
 * \param [in] unit -
 *   The unit number.
 * \param [in] force -
 *   Force type.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None.
 * \see
 *   * None.
 */
shr_error_e soc_jer2_fabric_force_set(
    int                    unit, 
    soc_dnx_fabric_force_t force
);

/**
 * \brief
 *   Get reachability status.
 * \param [in] unit -
 *   The unit number.
 * \param [in] moduleid -
 *   Module to check reachbility to.
 * \param [in] links_max -
 *   Max size of links_array.
 * \param [out] links_array -
 *   Links which moduleid is erachable through.
 * \param [out] links_array -
 *   links_count Size of links_array.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None.
 * \see
 *   * None.
 */
shr_error_e
soc_jer2_fabric_reachability_status_get(
    int unit,
    int moduleid,
    int links_max,
    uint32 *links_array,
    int *links_count
);

/**
 * \brief
 *   Get topology connectivity status.
 * \param [in] unit -
 *   The unit number.
 * \param [in] link_index_min -
 *   Min link ID.
 * \param [in] link_index_max -
 *   Max link ID.
 * \param [out] connectivity_map -
 *   Connectivity map.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None.
 * \see
 *   * None.
 */
shr_error_e
soc_jer2_fabric_topology_status_connectivity_get(
    int unit,
    int link_index_min,
    int link_index_max,
    SOC_DNX_FABRIC_LINKS_CON_STAT_INFO_ARR *connectivity_map
);

/**
 * \brief
 *   Get link status.
 * \param [in] unit -
 *   The unit number.
 * \param [in] link_id -
 *   Link ID.
 * \param [out] link_status -
 *   According to link status get.
 * \param [out] errored_token_count -
 *   Errored token count.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None.
 * \see
 *   * None.
 */
shr_error_e 
soc_jer2_fabric_link_status_get(
    DNX_SAND_IN  int unit,
    DNX_SAND_IN  soc_port_t link_id,
    DNX_SAND_OUT uint32 *link_status,
    DNX_SAND_OUT uint32 *errored_token_count
);

/**
 * \brief
 *   Initialize fabric multicast.
 * \param [in] unit -
 *   The unit number.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * Called as part of the initialization sequence.
 * \see
 *   * None.
 */
shr_error_e
soc_jer2_fabric_multicast_init(
    int unit);

/**
 * \brief
 *   Initialize fabric ALUWP (Active-Link-Up-Watchdog-Period).
 * \param [in] unit -
 *   The unit number.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * Called as part of the initialization sequence.
 * \see
 *   * None.
 */
shr_error_e
soc_jer2_fabric_aluwp_init(
    int unit);

/**
 * \brief
 *   When link is disabled, we need to force the signal, otherwise it
 *   can cause other blocks to get stuck.
 * \param [in] unit -
 *   The unit number.
 * \param [in] link -
 *   Fabric link id.
 * \param [in] force_signal -
 *   1 - force signal.
 *   0 - don't force signal.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None.
 * \see
 *   * None.
 */
shr_error_e
soc_jer2_fabric_link_force_signal_set(
    int unit,
    int link,
    int force_signal);

/**
 * \brief
 *   Initialize FMAC configurations for a fabric port
 * \param [in] unit -
 *   The unit number.
 * \param [in] logical_port -
 *   Logical port of the fabric link.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None.
 * \see
 *   * None.
 */
shr_error_e
soc_jer2_fabric_link_mac_init(
    int unit,
    soc_port_t logical_port);

/**
 * \brief
 *   Initialize FMAC configurations
 * \param [in] unit -
 *   The unit number.
 * \param [in] fmac_index -
 *   The index of the FMAC.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None.
 * \see
 *   * None.
 */
shr_error_e
soc_jer2_fabric_mac_init(
    int unit,
    int fmac_index);

/**
 * \brief
 *   Enable/Disable fabric port
 * \param [in] unit -
 *   The unit number.
 * \param [in] logical_port -
 *  Fabric logical port to enable/disable.
 * \param [in] enable -
 *   1 - Enable.
 *   2 - Disable.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None.
 * \see
 *   * None.
 */
shr_error_e
soc_jer2_fabric_port_enable_set(
    int unit,
    soc_port_t logical_port,
    int enable);

/* } __JER2_FABRIC_INCLUDED__*/
#endif

