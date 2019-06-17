/** \file algo_port_utils.h
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
#ifndef ALGO_PORT_UTILS_H_INCLUDED
/*
 * {
 */
#define ALGO_PORT_UTILS_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif
/*
 * Includes 
 * { 
 */
/*
 * }
 */

/*
 * MACROs
 * {
 */
/**
 * \brief "logicals bitmap get" flag to filter the result - 
 * all non master logicals are filtered.
 */
#define DNX_ALGO_PORT_LOGICALS_F_MASTER_ONLY (0x1)
/**
 * \brief "logicals bitmap get" flag to filter the result - 
 * all ELK ports are filtered.
 */
#define DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_ELK (0x1 << 1)
/**
 * \brief "logicals bitmap get" flag to filter the result -
 * all STIF ports are filtered.
 */
#define DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_STIF (0x1 << 2)

/**
 * \brief channels get flag - get tdm only channels
 */
#define DNX_ALGO_PORT_CHANNELS_F_TDM_ONLY   (0x1)
/**
 * \brief channels get flag - get non tdm only channels
 */
#define DNX_ALGO_PORT_CHANNELS_F_NON_TDM_ONLY   (0x2)
/**
 * \brief supported flags mask
 */
#define DNX_ALGO_PORT_CHANNELS_F_MASK (DNX_ALGO_PORT_CHANNELS_F_TDM_ONLY | DNX_ALGO_PORT_CHANNELS_F_NON_TDM_ONLY)

/**
 * \brief return true iff port exist and port type is DNX_ALGO_PORT_TYPE_CPU
 */
#define DNX_ALGO_PORT_TYPE_IS_CPU(unit, port_type)   (port_type == DNX_ALGO_PORT_TYPE_CPU)

/**
 * \brief return true iff port exist and port type is DNX_ALGO_PORT_TYPE_ERP
 */
#define DNX_ALGO_PORT_TYPE_IS_ERP(unit, port_type)   (port_type == DNX_ALGO_PORT_TYPE_ERP)

/**
 * \brief return true if port exist and port type is DNX_ALGO_PORT_TYPE_FABRIC
 */
#define DNX_ALGO_PORT_TYPE_IS_FABRIC(unit, port_type)   (port_type == DNX_ALGO_PORT_TYPE_FABRIC)

/**
 * \brief return true if port exist and port type is DNX_ALGO_PORT_TYPE_NIF_ETH_STIF
 */
#define DNX_ALGO_PORT_TYPE_IS_NIF_ETH_STIF(unit, port_type)   (port_type == DNX_ALGO_PORT_TYPE_NIF_ETH_STIF)

/**
 * \brief return true if port exist and port type is DNX_ALGO_PORT_TYPE_NIF_ETH
 *      or DNX_ALGO_PORT_TYPE_IS_NIF_ETH_STIF according to the parameter "include_stif"
 */
#define DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_type, include_stif) \
        ((port_type == DNX_ALGO_PORT_TYPE_NIF_ETH) || (include_stif && DNX_ALGO_PORT_TYPE_IS_NIF_ETH_STIF(unit, port_type)))

/**
 * \brief return true if port exist and port type is 
 *        DNX_ALGO_PORT_TYPE_NIF_ILKN_ELK
 */
#define DNX_ALGO_PORT_TYPE_IS_NIF_ILKN_ELK(unit, port_type)   (port_type == DNX_ALGO_PORT_TYPE_NIF_ILKN_ELK)

/**
 * \brief return true if port exist and port type is DNX_ALGO_PORT_TYPE_NIF_ILKN
 *        or DNX_ALGO_PORT_TYPE_NIF_ILKN_ELK according to the parameter "include_elk"
 */
#define DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_type, include_elk) \
        ((port_type == DNX_ALGO_PORT_TYPE_NIF_ILKN) || (include_elk && DNX_ALGO_PORT_TYPE_IS_NIF_ILKN_ELK(unit, port_type)))

/**
 * \brief return true if port exist and port type is DNX_ALGO_PORT_TYPE_NIF_ILKN or DNX_ALGO_PORT_TYPE_IS_NIF_ETH
 */
#define DNX_ALGO_PORT_TYPE_IS_NIF(unit, port_type, include_elk, include_stif)   \
        (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_type, include_elk) || DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_type, include_stif))
/**
 * \brief return true if port exist and port type is dispachable
 *        to IMB (DNX_ALGO_PORT_TYPE_NIF_ILKN,
 *        DNX_ALGO_PORT_TYPE_NIF_ETH,
 *        DNX_ALGO_PORT_TYPE_NIF_ILKN_ELK,
 *        DNX_ALGO_PORT_TYPE_FABRIC)
 */
#define DNX_ALGO_PORT_TYPE_IS_IMB(unit, port_type)   \
        (DNX_ALGO_PORT_TYPE_IS_NIF(unit, port_type, TRUE, TRUE) || DNX_ALGO_PORT_TYPE_IS_FABRIC(unit, port_type))

/**
 * \brief return true if port exist and port type is DNX_ALGO_PORT_TYPE_OLP
 */
#define DNX_ALGO_PORT_TYPE_IS_OLP(unit, port_type)  (port_type == DNX_ALGO_PORT_TYPE_OLP)

/**
 * \brief return true if port exist and port type is DNX_ALGO_PORT_TYPE_RCY
 */
#define DNX_ALGO_PORT_TYPE_IS_RCY(unit, port_type)   (port_type ==  DNX_ALGO_PORT_TYPE_RCY)

/**
 * \brief return true if port exist and port type is DNX_ALGO_PORT_TYPE_OAMP
 */
#define DNX_ALGO_PORT_TYPE_IS_OAMP(unit, port_type)   (port_type ==  DNX_ALGO_PORT_TYPE_OAMP)

/**
 * \brief return true if port exist and port type is DNX_ALGO_PORT_TYPE_EVENTOR
 */
#define DNX_ALGO_PORT_TYPE_IS_EVENTOR(unit, port_type)   (port_type == DNX_ALGO_PORT_TYPE_EVENTOR)

/**
 * \brief return true if port exist and port type is DNX_ALGO_PORT_TYPE_SAT
 */
#define DNX_ALGO_PORT_TYPE_IS_SAT(unit, port_type)   (port_type == DNX_ALGO_PORT_TYPE_SAT)

/**
 * \brief return true if port exist and port type is DNX_ALGO_PORT_TYPE_RCY_MIRROR
 */
#define DNX_ALGO_PORT_TYPE_IS_RCY_MIRROR(unit, port_type)  (port_type == DNX_ALGO_PORT_TYPE_RCY_MIRROR)

/**
 * \brief return true if port exist and port type represents tm port
 */
#define DNX_ALGO_PORT_TYPE_IS_TM(unit, port_type)   \
        (!DNX_ALGO_PORT_TYPE_IS_FABRIC(unit, port_type) && !DNX_ALGO_PORT_TYPE_IS_NIF_ETH_STIF(unit, port_type) && !DNX_ALGO_PORT_TYPE_IS_NIF_ILKN_ELK(unit, port_type))

/**
 * \brief return true if port exist and port type represents pp port
 *                    currently all TM ports are also PP - will be changed when LAG SCH implemented.
 */
#define DNX_ALGO_PORT_TYPE_IS_PP(unit, port_type)   \
        (DNX_ALGO_PORT_TYPE_IS_ING_TM(unit, port_type))
/**
 * \brief return true if port exist and port type represents ingress pp port
 */
#define DNX_ALGO_PORT_TYPE_IS_ING_PP(unit, port_type)   \
        (DNX_ALGO_PORT_TYPE_IS_PP(unit, port_type))
/**
 * \brief return true if port exist and port type represents egress pp port
 */
#define DNX_ALGO_PORT_TYPE_IS_EGR_PP(unit, port_type)   \
        (DNX_ALGO_PORT_TYPE_IS_PP(unit, port_type) && !DNX_ALGO_PORT_TYPE_IS_RCY_MIRROR(unit, port_type))
/**
 * \brief return true if port exist and port type represents ingress tm port
 */
#define DNX_ALGO_PORT_TYPE_IS_ING_TM(unit, port_type)   \
        (DNX_ALGO_PORT_TYPE_IS_TM(unit, port_type) && !DNX_ALGO_PORT_TYPE_IS_ERP(unit, port_type))

/**
 * \brief return true if port exist and port type represents egress tm port
 */
#define DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, port_type)   \
        (DNX_ALGO_PORT_TYPE_IS_TM(unit, port_type) && !DNX_ALGO_PORT_TYPE_IS_ERP(unit, port_type) && !DNX_ALGO_PORT_TYPE_IS_RCY_MIRROR(unit, port_type))

/**
 * \brief return true if port exist and port type represents e2e scheduler port
 */
#define DNX_ALGO_PORT_TYPE_IS_E2E_SCH(unit, port_type)   \
        (DNX_ALGO_PORT_TYPE_IS_TM(unit, port_type) && !DNX_ALGO_PORT_TYPE_IS_RCY_MIRROR(unit, port_type))

/**
 * \brief max length of port type string
 */
#define DNX_ALGO_PORT_TYPE_STR_LENGTH           (20)

/**
 * \brief max length of tdm mode string
 */
#define DNX_ALGO_PORT_TDM_MODE_STR_LENGTH        (20)

/*
 * }
 */

/*
 * Typedefs
 * {
 */
/**
 * \brief 
 * Group of port types - will be added upon requirement.
 */
typedef enum
{
    /**
     * Unused  logical ports.
     */
    DNX_ALGO_PORT_LOGICALS_TYPE_INVALID = -1,
    /**
     * All valid ports.
     */
    DNX_ALGO_PORT_LOGICALS_TYPE_VALID = 0,
    /**
     * Network interface logical ports - both ethernet and interlaken interfaces  
     */
    DNX_ALGO_PORT_LOGICALS_TYPE_NIF = 1,
    /**
     * Network interface ethernet logical ports.
     * Not including Statistic inteface.
     */
    DNX_ALGO_PORT_LOGICALS_TYPE_NIF_ETH = 2,
    /**
     * Fabric logical ports  
     */
    DNX_ALGO_PORT_LOGICALS_TYPE_FABRIC = 3,
    /**
     * TM logical ports for egress SCH
     * Relevant for Egress SCH
     */
    DNX_ALGO_PORT_LOGICALS_TYPE_TM_E2E_SCH = 4,
    /**
     * TM logical ports - not including ERP
     * Relevant for ingress TM
     */
    DNX_ALGO_PORT_LOGICALS_TYPE_TM_ING = 5,
    /**
     * PP logical ports 
     * Currently - same as DNX_ALGO_PORT_LOGICALS_TYPE_TM_WITHOUT_ERP
     */
    DNX_ALGO_PORT_LOGICALS_TYPE_PP = 6,
    /**
     * TM logical ports - not including ERP
     * Relevant only for egress queuing
     */
    DNX_ALGO_PORT_LOGICALS_TYPE_TM_EGR_QUEUING = 7,
    /**
      * Network interface ILKN logical ports.
      * Including ELK ports
      */
    DNX_ALGO_PORT_LOGICALS_TYPE_NIF_ILKN = 8,
    /**
      * NIF ELK ports
      */
    DNX_ALGO_PORT_LOGICALS_TYPE_NIF_ILKN_ELK = 9,
    /**
      * CPU ports
      */
    DNX_ALGO_PORT_LOGICALS_TYPE_CPU = 10,
    /**
     * number of logicals types
     */
    DNX_ALGO_PORT_LOGICALS_TYPE_NOF
} dnx_algo_port_logicals_type_e;

/**
 * \brief - significant info about logical port (used for diag command)
 */
typedef struct
{
    /**
     * Logical port #
     */
    bcm_port_t logical_port;
    /**
     * Port type
     */
    dnx_algo_port_type_e port_type;
    /**
     * core_id or BCM_CORE_ALL if relevant to both
     */
    bcm_core_t core_id;
    /**
     * phy_id - first_phy_port for nif or link id for fabric
     */
    int phy_id;
    /**
     * tm_port - tm port id if relevant or DNX_ALGO_PORT_INVALID.
     */
    uint32 tm_port;
    /**
     * channel - tm channel id if relevant or -1.
     */
    int channel_id;

} dnx_algo_port_diag_info_t;

/**
 * \brief -  logical port info (used for diag command)
 */
typedef struct
{
    /**
     * Logical port
     */
    bcm_port_t logical_port;
    /**
     * Port type
     */
    dnx_algo_port_type_e port_type;
    /**
     * Interface offset
     */
    int interface_offset;
    /**
     * Master port
     */
    bcm_port_t master_port;

} dnx_algo_port_diag_info_logical_t;

/**
 * \brief -  nif info (used for diag command)
 */
typedef struct
{
    /**
     * is this info valid
     */
    int valid;
    /**
     * Phy ports bitmap
     */
    bcm_pbmp_t phy_ports;
    /**
     * First phy port
     */
    int first_phy;
    /**
     * Logical Fifos ids
     */
    bcm_pbmp_t logical_fifos;

} dnx_algo_port_diag_info_nif_t;

/**
 * \brief -  ILKN info (used for diag command)
 */
typedef struct
{
    /**
     * is this info valid
     */
    int valid;
    /**
     * Nof Segments
     */
    int nof_segments;

} dnx_algo_port_diag_info_ilkn_t;

/**
 * \brief -  tm & pp info (used for diag command)
 */
typedef struct
{
    /**
     * is this info valid
     */
    int valid;
    /**
     * TM port
     */
    uint32 tm_port;
    /**
     * Channel ID
     */
    int channel_id;
    /**
     * Channelized port - Flag that set when port added to DB
     */
    int is_channelized;
    /**
     * Base queue pair
     */
    int base_q_pair;
    /**
     * Number of port priorities
     */
    int num_priorities;
    /**
     * Base hr
     */
    int base_hr;
    /**
     * Number of scheduler priorities
     */
    int sch_priorities;
    /**
     * PP port
     */
    uint32 pp_port;
    /**
     * TM interface
     */
    int tm_interface;
    /**
     * Egress interface id
     */
    int egr_if_id;
    /**
     * Scheduler interface id
     */
    int sch_if_id;
    /**
     * Logical ports with the same interface
     */
    bcm_pbmp_t channels_ports;
    /**
     * tdm mode
     */
    dnx_algo_port_tdm_mode_e tdm_mode;
    /**
     * tdm master
     */
    bcm_port_t tdm_master;
    /**
     * non tdm master
     */
    bcm_port_t non_tdm_master;
} dnx_algo_port_diag_info_tm_pp_t;

/**
 * \brief -  tm & pp info (used for diag command)
 */
typedef struct
{
    /**
     * is this info valid
     */
    int valid;
    /**
     * Core id
     */
    bcm_core_t core_id;
    /**
     * speed of the interface [kbps]
     */
    int interface_speed;

} dnx_algo_port_diag_info_interface_t;

/**
 * \brief -  fabric info (used for diag command)
 */
typedef struct
{
    /**
     * is this info valid
     */
    int valid;
    /**
     * Port interface type
     */
    int link_id;

} dnx_algo_port_diag_info_fabric_t;

/**
 * \brief - all info about logical port (used for diag command)
 */
typedef struct
{
    /**
     * Logical port info
     */
    dnx_algo_port_diag_info_logical_t logical_port_info;
    /**
     * NIF info
     */
    dnx_algo_port_diag_info_nif_t nif_info;

    /**
     * TM and PP info
     */
    dnx_algo_port_diag_info_interface_t interface_info;

    /**
     * TM and PP info
     */
    dnx_algo_port_diag_info_tm_pp_t tm_pp_info;
    /**
     * Fabric info
     */
    dnx_algo_port_diag_info_fabric_t fabric_info;
    /**
     * ILKN info
     */
    dnx_algo_port_diag_info_ilkn_t ilkn_info;

} dnx_algo_port_diag_info_all_t;

/**
 * \brief -  Access port info (used for dbal tables access)
 */
typedef struct
{
    /**
     * Core
     */
    bcm_core_t core;
    /**
     * CDU index in device
     */
    int cdu_id;
    /**
     * CDU index inside the core
     */
    int inner_cdu_id;
    /**
     * first internal lane inside the CDU
     */
    int first_lane_in_cdu;
    /**
     * first lane of the CDU
     */
    int cdu_first_phy;
    /**
     * MAC id inside the CDU;
     */
    uint32 mac_id;

} dnx_algo_port_cdu_access_info_t;

/*
 * }
 */

/**
 * \brief - 
 * Get bitmap of required logical ports
 * \par DIRECT_INPUT:
 *   \param [in] unit - Relevant unit.
 *   \param [in] core - required core #, or BCM_CORE_ALL for all cores
 *   \param [in] logicals_type - type of logical ports to get (see dnx_algo_port_logical_type_t)
 *   \param [in] flags - see DNX_ALGO_PORT_LOGICALS_F_*
 *   
 * \par INDIRECT INPUT:
 *   * dnx data
 *   * internal algo port data base
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT
 *   \param [out] logicals - required logicals bitmap
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_logicals_get(
    int unit,
    bcm_core_t core,
    dnx_algo_port_logicals_type_e logicals_type,
    uint32 flags,
    bcm_pbmp_t * logicals);

/**
 * \brief - 
 * Get bitmap of all the members of interface
 * \par DIRECT_INPUT:
 *   \param [in] unit - Relevant unit.
 *   \param [in] logical_port - logical port of one of the channels
 *   \param [in] flags - see DNX_ALGO_PORT_CHANNELS_F_*
 *   
 * \par INDIRECT INPUT:
 *   * dnx data
 *   * internal algo port data base
 * \par DIRECT OUTPUT:
 *   see shr_error_e
 * \par INDIRECT OUTPUT
 *   \param [out] logical_port_channnels - all the logical ports with the same interface.
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_channels_get(
    int unit,
    bcm_port_t logical_port,
    uint32 flags,
    bcm_pbmp_t * logical_port_channnels);

/**
 * \brief - 
 * Get the number of all the members of interface 
 *  
 * \param [in] unit - Relevant unit.
 * \param [in] logical_port - logical port of one of the channels
 * \param [out] nof_channels - number of channels 
 *   
 * \return 
 *   see shr_error_e
 *  
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_channels_nof_get(
    int unit,
    bcm_port_t logical_port,
    int *nof_channels);

/**
 * \brief - Utility provided to map logical port pbmp to TM port 
 *       pbmp
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit - Relevant unit.
 *   \param [in] logical_pbmp - logical ports bitmap.
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT: 
 *   * See shr_error_e  
 * \par INDIRECT OUTPUT
 *   \param [out] pbmp_tm_arr - An array of TM ports bitmap per core. Will be cleared and filled by the procedure.
 * \remark
 *   * None
 * \see
 *   * For PBMP usage and its types see include/bcm/types.h
 */
shr_error_e dnx_algo_port_logical_pbmp_to_tm_pbmp_get(
    int unit,
    bcm_pbmp_t logical_pbmp,
    bcm_pbmp_t * pbmp_tm_arr);

/*
 * }
 */

/**
 * \brief - Utility provided to map TM port to  logical pbmp
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Relevant unit.
 *   \param [in] core - Device core.
 *   \param [in] tm_ports - TM ports bitmap of the core above.
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   * See shr_error_e
 * \par INDIRECT OUTPUT
 *   \param [out] logical_pbmp - logical ports bitmap.
 * \remark
 *   * Relies on a function that maps TM port to local port
 * \see
 *   * For PBMP usage and its types see include/bcm/types.h
 */

shr_error_e dnx_algo_port_tm_pbmp_to_logical_pbmp_get(
    int unit,
    bcm_core_t core,
    bcm_pbmp_t tm_ports,
    bcm_pbmp_t * logical_pbmp);

/**
 * \brief - Utility provided to map logical port pbmp to PP port pbmp
 *       pbmp
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit - Relevant unit.
 *   \param [in] logical_pbmp - logical ports bitmap.
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT: 
 *   * See shr_error_e  
 * \par INDIRECT OUTPUT
 *   \param [out] pbmp_pp_arr - An array of PP ports bitmap per core. Will be cleared and filled by the procedure.
 * \remark
 *   * None
 * \see
 *   * For PBMP usage and its types see include/bcm/types.h
 */
shr_error_e dnx_algo_port_logical_pbmp_to_pp_pbmp_get(
    int unit,
    bcm_pbmp_t logical_pbmp,
    bcm_pbmp_t * pbmp_pp_arr);

/*
 * }
 */

/**
 * \brief - Utility provided to map PP port to  logical pbmp
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Relevant unit.
 *   \param [in] core - Device core.
 *   \param [in] pp_ports - PP ports of the core above
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   * See shr_error_e
 * \par INDIRECT OUTPUT
 *   \param [out] logical_pbmp - logical ports bitmap.
 * \remark
 *   * Relies on a function that maps PP port to local port
 * \see
 *   * For PBMP usage and its types see include/bcm/types.h
 */
shr_error_e dnx_algo_port_pp_pbmp_to_logical_pbmp_get(
    int unit,
    bcm_core_t core,
    bcm_pbmp_t pp_ports,
    bcm_pbmp_t * logical_pbmp);

/**
 * \brief - Utility provided to map single PP port to tm pbmp
 *
 * \param [in] unit - Relevant unit.
 * \param [in] core - Device core.
 * \param [in] pp_port - PP port.
 * \param [out] pbmp_tm_ports - TM ports bitmap.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_pp_to_tm_pbmp_get(
    int unit,
    bcm_core_t core,
    bcm_port_t pp_port,
    bcm_pbmp_t * pbmp_tm_ports);

/**
 * \brief - Get interface rate:
 *          For ILKN: interface_rate = num_of_lanes * port_speed
 *          Otherwise: interface_rate = port_speed
 *          
 * \param [in] unit - Relevant unit.
 * \param [in] logical_port - logical port #.
 * \param [in] flags - each bit represent a speed type, such as \
 *                  - DNX_ALGO_PORT_SPEED_F_MBPS \
 *                  - DNX_ALGO_PORT_SPEED_F_KBPS \
 * \param [out] rate - rate in Mbps or Kbps
 *  
 * \return
 *   * See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_interface_rate_get(
    int unit,
    bcm_port_t logical_port,
    uint32 flags,
    int *rate);

/**
 * \brief - Relevant for NIF ports only.
 *          Return the number of phys used by logical_port.
 *          
 * \param [in] unit - Relevant unit.
 * \param [in] logical_port - logical port #.
 * \param [out] nof_phys - nof_phys
 *  
 * \return
 *   * See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_nif_phys_nof_get(
    int unit,
    bcm_port_t logical_port,
    int *nof_phys);

/**
 * \brief - Get significant info about logical port (used for diag command)
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Relevant unit.
 *   \param [in] logical_port - logical port #.
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   * See shr_error_e
 * \par INDIRECT OUTPUT
 *   \param [out] diag_info - significant info about logical port
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_diag_info_get(
    int unit,
    bcm_port_t logical_port,
    dnx_algo_port_diag_info_t * diag_info);

/**
 * \brief - Get all info about logical port (used for diag command)
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Relevant unit.
 *   \param [in] logical_port - logical port #.
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   * See shr_error_e
 * \par INDIRECT OUTPUT
 *   \param [out] diag_info_all - all info about logical port
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_diag_info_all_get(
    int unit,
    bcm_port_t logical_port,
    dnx_algo_port_diag_info_all_t * diag_info_all);

/**
 * \brief - Convert port type to string
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit #.
 *   \param [in] port_type - port type.
 *   \param [out] port_type_str - buffer to hold the string.
 */
shr_error_e dnx_algo_port_type_str_get(
    int unit,
    dnx_algo_port_type_e port_type,
    char port_type_str[DNX_ALGO_PORT_TYPE_STR_LENGTH]);

/**
 * \brief - Convert tdm mode to string
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit #.
 *   \param [in] tdm_mode - port type.
 *   \param [out] tdm_mode_str - buffer to hold the string.
 */
shr_error_e dnx_algo_port_tdm_mode_str_get(
    int unit,
    dnx_algo_port_tdm_mode_e tdm_mode,
    char tdm_mode_str[DNX_ALGO_PORT_TDM_MODE_STR_LENGTH]);

/**
 * \brief - Get all information required for CDU DABL access
 *
 * \param [in] unit - unit #.
 * \param [in] logical_port - logical port number.
 * \param [out] cdu_access_info - cdu access info struct. 
 */
shr_error_e dnx_algo_port_cdu_access_info_get(
    int unit,
    bcm_port_t logical_port,
    dnx_algo_port_cdu_access_info_t * cdu_access_info);

/**
 * \brief - Get first logical_port with the requested port type and core
 *
 * \param [in] unit - unit #.
 * \param [in] core - requested core or BCM_CORE_ALL for both 
 * \param [in] port_type - requested port_type
 * \param [out] logical_port - the first logical port or DNX_ALGO_PORT_INVALID if not found
 */
shr_error_e dnx_algo_port_first_port_type_get(
    int unit,
    bcm_core_t core,
    dnx_algo_port_type_e port_type,
    bcm_port_t * logical_port);

/**
 * \brief - Get the PLL index for given logical port
 *
 * \param [in] unit - unit #.
 * \param [in] logical_port - logical port#
 * \param [out] pll_index - pll index
 */
shr_error_e dnx_algo_port_pll_index_get(
    int unit,
    bcm_port_t logical_port,
    int *pll_index);

/**
 * \brief -
 *  Return NIF phys (0-95) from ILKN lanes
 *
 * \param [in] unit - unit #.
 * \param [in] interface_id - ILKN Interface Id
 * \param [in] ilkn_lanes - ILKN lanes bitmap (0-23)
 * \param [out] phys - NIF phys bitmap (0-95)
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_algo_port_ilkn_lanes_to_nif_phys_get(
    int unit,
    int interface_id,
    bcm_pbmp_t * ilkn_lanes,
    bcm_pbmp_t * phys);

#endif /* !ALGO_PORT_UTILS_H_INCLUDED */
