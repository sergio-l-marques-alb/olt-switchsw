/** \file algo_port_verify.h
 *  
 *  PORT manager, Port utilities, and Port verifiers.
 *  
 *  Algo port managment:
 *  * MGMT APIs
 *    - Used to configure algo port module
 *    - located in algo_port_mgmt.h (source code algo_port_mgmt.c)
 *  * Utility APIs
 *    - Provide utilities functions which will make the BCM module cleaner and simpler
 *    - located in algo_port_utils.h (source code algo_port_utils.c
 *  * Verify APIs
 *    - Provide a set of APIs which verify port attributes
 *    - These verifiers used to verify algo port functions input.
 *    - These verifiers can be used out side of the module, if requried.
 *    - located in algo_port_verify.h (source code algo_port_verify.c)
 *  * SOC APIs
 *    - Provide set of functions to set / restore  and remove soc info data
 *    - Only the port related members will be configured by this module (other modules should not set those memebers directly)
 *    - The relevant members are: 'port to block access', 'port names' and 'port bitmaps'
 *    - located in algo_port_soc.c (all the functions are module internals)
 *  * Module internal definitions - algo_port_internal.h
 *  * Data bases - use sw state mechanism - XML file is algo_port.xml
 *  
 *  No need to include each sub module seperately.
 *  Including: 'algo_port_mgmt.h' will includes all the sub modules
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifndef ALGO_PORT_VERIFY_H_INCLUDED
/*
 * {
 */
#define ALGO_PORT_VERIFY_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/**
 * \brief - 
 * Verify that logical port in range and free
 *           
 * \par DIRECT_INPUT:
 *   \param [in] unit - Relevant unit.
 *   \param [in] logical_port - logical port #
 * \par INDIRECT INPUT:
 *   * dnx data
 *   * internal algo port data base
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_free_verify(
    int unit,
    bcm_port_t logical_port);

/**
 * \brief - 
 * Verify that logical port in range and valid (i.e. allocated)
 *           
 * \par DIRECT_INPUT:
 *   \param [in] unit - Relevant unit.
 *   \param [in] logical_port - logical port #
 * \par INDIRECT INPUT:
 *   * dnx data
 *   * internal algo port data base
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_valid_verify(
    int unit,
    bcm_port_t logical_port);

/**
 * \brief - 
 * Verify that port type is suppored in device.
 *           
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit #
 *   \param [in] port_type - port type to check
 * \par INDIRECT INPUT:
 *   * dnx data
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_type_supported_verify(
    int unit,
    dnx_algo_port_type_e port_type);

/**
 * \brief - 
 * Verify nif phy id supported
 *  
 * \param [in] unit - unit #
 * \param [in] nif_phy - nif phy id
 * 
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_nif_phy_verify(
    int unit,
    int nif_phy);

/**
 * \brief - 
 * Verify fabric phy id supported
 *  
 * \param [in] unit - unit #
 * \param [in] fabric_phy - fabric phy id
 * 
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_fabric_phy_verify(
    int unit,
    int fabric_phy);

/**
 * \brief - 
 * Verify new nif port
 * Relevant for both ethrnet port and interlaken ports. 
 * * If channlized interface - makes sure that nif is identical 
 * * Otherwise - checks that other ports dont the same phys. 
 *  
 * \param [in] unit - unit #
 * \param [in] logical_port - required logical port.
 * \param [in] port_type - see dnx_algo_port_type_e.
 * \param [in] info - info required for adding nif port (see struct definition)
 *   
 * 
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_nif_add_verify(
    int unit,
    bcm_port_t logical_port,
    dnx_algo_port_type_e port_type,
    dnx_algo_port_if_add_t * info);

/**
 * \brief - 
 * Should be used before adding or changing ilkn over fabric master port phys.
 * * Verify that all phys supported by the current device (using dnx data)
 * * Verify that the phys are not used by any other port
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Relevant unit.
 *   \param [in] logical_port - logical port #
 *   \param [in] phys- required phys bitmap to check
 * \par INDIRECT INPUT:
 *   * dnx data
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_ilkn_over_fabric_phys_verify(
    int unit,
    bcm_port_t logical_port,
    bcm_pbmp_t phys);

/**
 * \brief -
 * Should be used before adding or changing master port phys. 
 * * Verify that all phys supported by the current device (using dnx data) 
 * * Verify that the phys are not used by any other port
 *           
 * \par DIRECT_INPUT:
 *   \param [in] unit - Relevant unit.
 *   \param [in] logical_port - logical port #
 *   \param [in] phys- required phys bitmap to check
 * \par INDIRECT INPUT:
 *   * dnx data
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_nif_phys_verify(
    int unit,
    bcm_port_t logical_port,
    bcm_pbmp_t phys);

/**
 * \brief - 
 * Verify that core X tm_port are in supported range (per device) 
 * And the relevant port mark as valid.
 *           
 * \param [in] unit - Relevant unit.
 * \param [in] core - core #
 * \param [in] tm_port - tm port #
 * 
 * \return  
 *   see shr_error_e 
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_tm_port_valid_verify(
    int unit,
    bcm_core_t core,
    int tm_port);

/**
 * \brief - 
 * Verify that core X tm_port are in supported range (per device) 
 * And the relevant port mark as free.
 *           
 * \param [in] unit - Relevant unit.
 * \param [in] port_type - see dnx_algo_port_type_e
 * \param [in] core - core #
 * \param [in] tm_port - tm port #
 * 
 * \return  
 *   see shr_error_e 
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_tm_port_free_verify(
    int unit,
    dnx_algo_port_type_e port_type,
    bcm_core_t core,
    int tm_port);

/**
 * \brief - 
 * Verify that core X base_hr..base_hr+nof_sch_priorities are in supported range (per device)
 *
 * \param [in] unit - Relevant unit.
 * \param [in] core - core #
 * \param [in] base_hr - base hr #
 * \param [in] nof_sch_priorities - number of required HRs
 *
 * \return
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_hr_free_verify(
    int unit,
    bcm_core_t core,
    int base_hr,
    int nof_sch_priorities);
/**
 * \brief -
 * Verify that core X pp_port are in supported range (per device) 
 * And the relevant port mark as valid.
 *           
 * \par DIRECT_INPUT:
 *   \param [in] unit - Relevant unit.
 *   \param [in] core - core #
 *   \param [in] pp_port - pp port #
 * \par INDIRECT INPUT:
 *   * dnx data
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_pp_port_valid_verify(
    int unit,
    bcm_core_t core,
    int pp_port);

/**
 * \brief - 
 * Verify that fabric link is in supported range (per device) 
 * And the relevant link is mark as valid.
 *           
 * \par DIRECT_INPUT:
 *   \param [in] unit - Relevant unit.
 *   \param [in] fabric_link - fabric link #
 * \par INDIRECT INPUT:
 *   * dnx data
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_fabric_link_valid_verify(
    int unit,
    int fabric_link);

/**
 * \brief - 
 * Verify that tdm mode is supported in device.
 *           
 * \par DIRECT_INPUT:
 *   \param [in] unit - Relevant unit.
 *   \param [in] tdm_mode - tdm mode to check
 * \par INDIRECT INPUT:
 *   * dnx data
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_tdm_mode_supported_verify(
    int unit,
    dnx_algo_port_tdm_mode_e tdm_mode);

/**
 * \brief - 
 * Verify that egress interface id is supported.
 *           
 * \param [in] unit - unit #.
 * \param [in] if_id - egress interface id
 *  
 * \return 
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_egr_if_id_verify(
    int unit,
    int if_id);

/**
 * \brief - 
 * Verify the logical port port_type
 *           
 * \param [in] unit - unit #.
 * \param [in] logical_port - logical port # 
 * \param [in] port_type - required port type
 *  
 * \return 
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_type_verify(
    int unit,
    bcm_port_t logical_port,
    dnx_algo_port_type_e port_type);
/**
 * \brief - 
 * Verify that the required channel_id is free
 *           
 * \param [in] unit - unit #. 
 * \param [in] logical_port - new logical port
 * \param [in] tm_interface_handle - the tm interface of the new logical port 
 * \param [in] channel - the required channel for the new logical port
 *  
 * \return 
 *   see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_channel_free_verify(
    int unit,
    bcm_port_t logical_port,
    int tm_interface_handle,
    int channel);

/**
 * \brief - 
 * Verify that inteface offset is supported
 *           
 * \param [in] unit - Unit #.
 * \param [in] port_type - see  dnx_algo_port_type_e
 * \param [in] interface_offset - interface_offset
 * 
 * \return  
 *   see shr_error_e 
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_special_interface_offset_verify(
    int unit,
    dnx_algo_port_type_e port_type,
    int interface_offset);

#endif /* ALGO_PORT_VERIFY_H_INCLUDED */
