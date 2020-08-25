/** \file algo_rx.c
 *
 * Mirror related algorithm functions for DNX.
 *
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_RX
/**
* INCLUDE FILES:
* {
*/

/*
 * Include files which are specifically for DNX. Final location.
 * {
 */
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_trap.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_lif.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/algo/rx/algo_rx.h>
#include <bcm_int/dnx/rx/rx_trap.h>
#include <soc/dnx/swstate/auto_generated/access/algo_rx_access.h>

/*
 * }
 */

/*
 * Other include files. 
 * { 
 */

#include <shared/utilex/utilex_bitstream.h>
#include <shared/utilex/utilex_framework.h>

/*
 * }
 */

/**
 * }
 */

/**
* \brief  
*   Allocate trap id in DNX_ALGO_RX_TRAP_RES_MNGR_INGRESS_PREDEFINED pool
* \par DIRECT INPUT: 
*   \param [in] unit     -  Unit id
*   \param [in] trap_id_p  -  Pointer to a memory type of int. \n
*           \b As \b output - \n
*           This pocedure loads pointed memory by ingress predefined trap_id newly allocaed
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   shr_error_e - Error type
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * None
* \see
*   * None
*/
shr_error_e
dnx_algo_rx_trap_id_ingress_predefined_allocate(
    int unit,
    int *trap_id_p)
{
    int alloc_flags;
    SHR_FUNC_INIT_VARS(unit);

    alloc_flags = SW_STATE_ALGO_RES_ALLOCATE_WITH_ID;

    /*
     * allocate ingress predefined id, in this case only to mark that trap is already created once
     */
    SHR_IF_ERR_EXIT(algo_rx_db.trap_ingress_predefined.allocate_single(unit, alloc_flags, NULL, trap_id_p));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief  
*   Allocate trap id in DNX_ALGO_RX_TRAP_RES_MNGR_INGRESS_USER_DEFINED pool
* \par DIRECT INPUT: 
*   \param [in] unit     -  Unit id
*   \param [in] alloc_flags  -  User define trap can be allocated BCM_RX_TRAP_WITH_ID
*   \param [in] alloc_id_p  -  Pointer to a memory type of int. \n
*           \b As \b output - \n
*           This pocedure loads pointed memory by ingress user_defined trap_id newly allocaed
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   shr_error_e - Error type
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * None
* \see
*   * None
*/
shr_error_e
dnx_algo_rx_trap_id_ingress_user_define_allocate(
    int unit,
    int alloc_flags,
    int *alloc_id_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(alloc_id_p, _SHR_E_PARAM, "alloc_id_p");

    /*
     * allocate ingress user_define trap id
     */
    SHR_IF_ERR_EXIT(algo_rx_db.trap_ingress_user_defined.allocate_single(unit, alloc_flags, NULL, alloc_id_p));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief  
*   Allocate trap id in DNX_ALGO_RX_TRAP_RES_MNGR_ERPP pool
* \par DIRECT INPUT: 
*   \param [in] unit     - Unit id
*   \param [in] flags    - Flags
*   \param [in] trap_id_p  -  Pointer to a memory type of int. \n
*           \b As \b output - \n
*           This pocedure loads pointed memory by ERPP trap_id
*           newly allocaed
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   shr_error_e - Error type
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * None
* \see
*   * None
*/
shr_error_e
dnx_algo_rx_trap_id_erpp_allocate(
    int unit,
    int flags,
    int *trap_id_p)
{
    int alloc_flags;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(trap_id_p, _SHR_E_PARAM, "trap_id_p");
    alloc_flags = 0;
    if (flags == BCM_RX_TRAP_WITH_ID)
    {
        alloc_flags = SW_STATE_ALGO_RES_ALLOCATE_WITH_ID;
    }

    /*
     * allocate erpp id, in this case only to mark that trap is already created once
     */
    SHR_IF_ERR_EXIT(algo_rx_db.trap_erpp.allocate_single(unit, alloc_flags, NULL, trap_id_p));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief  
*   Allocate trap id in DNX_ALGO_RX_TRAP_RES_MNGR_ETPP_ pool
* \par DIRECT INPUT: 
*   \param [in] unit      -  Unit id
*   \param [in] flags     - Flags(WITH_ID)
*   \param [in] trap_id_p  -  Pointer to a memory type of int. \n
*           \b As \b output - \n
*           This pocedure loads pointed memory by ETPP trap_id newly allocaed
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   shr_error_e - Error type
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * None
* \see
*   * None
*/
shr_error_e
dnx_algo_rx_trap_id_etpp_allocate(
    int unit,
    int flags,
    int *trap_id_p)
{
    int alloc_flags;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(trap_id_p, _SHR_E_PARAM, "trap_id_p");
    alloc_flags = 0;
    if (flags == BCM_RX_TRAP_WITH_ID)
    {
        alloc_flags = SW_STATE_ALGO_RES_ALLOCATE_WITH_ID;
    }

    /*
     * allocate etpp id, in this case only to mark that trap is already created once
     */
    SHR_IF_ERR_EXIT(algo_rx_db.trap_etpp.allocate_single(unit, alloc_flags, NULL, trap_id_p));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief  
*   Allocate trap id in DNX_ALGO_RX_TRAP_RES_MNGR_ETPP_OAM pool
* \par DIRECT INPUT: 
*   \param [in] unit     -  Unit id
*   \param [in] trap_id_p  -  Pointer to a memory type of int. \n
*           \b As \b output - \n
*           This pocedure loads pointed memory by ETPP OAM trap_id newly allocaed
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   shr_error_e - Error type
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * None
* \see
*   * None
*/
shr_error_e
dnx_algo_rx_trap_id_etpp_oam_allocate(
    int unit,
    int *trap_id_p)
{
    int alloc_flags;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(trap_id_p, _SHR_E_PARAM, "trap_id_p");

    alloc_flags = SW_STATE_ALGO_RES_ALLOCATE_WITH_ID;

    /*
     * allocate etpp oam id, in this case only to mark that trap is already created once
     */
    SHR_IF_ERR_EXIT(algo_rx_db.trap_etpp_oam.allocate_single(unit, alloc_flags, NULL, trap_id_p));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   Deallocate trap id in DNX_ALGO_RX_TRAP_RES_MNGR_INGRESS_PREDEFINED pool
* \par DIRECT INPUT: 
*   \param [in] unit     -  Unit id
*   \param [in] trap_id  -  Trap id to deallocate
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   shr_error_e - Error type
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * None
* \see
*   * None
*/
shr_error_e
dnx_algo_rx_trap_id_ingress_predefined_deallocate(
    int unit,
    int trap_id)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * deallocate trap_id
     */
    SHR_IF_ERR_EXIT(algo_rx_db.trap_ingress_predefined.free_single(unit, trap_id));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief  
*   Deallocate trap id in DNX_ALGO_RX_TRAP_RES_MNGR_INGRESS_USER_DEFINED pool
* \par DIRECT INPUT: 
*   \param [in] unit     -  Unit id
*   \param [in] trap_id  -  Trap id to deallocate
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   shr_error_e - Error type
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * None
* \see
*   * None
*/
shr_error_e
dnx_algo_rx_trap_id_ingress_user_define_deallocate(
    int unit,
    int trap_id)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * deallocate trap_id
     */
    SHR_IF_ERR_EXIT(algo_rx_db.trap_ingress_user_defined.free_single(unit, trap_id));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief  
*   Deallocate ERPP trap id
* \par DIRECT INPUT: 
*   \param [in] unit     -  Unit id
*   \param [in] trap_id  -  Trap id to deallocate
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   shr_error_e - Error type
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * None
* \see
*   * None
*/
shr_error_e
dnx_algo_rx_trap_id_erpp_deallocate(
    int unit,
    int trap_id)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * deallocate trap_id
     */
    SHR_IF_ERR_EXIT(algo_rx_db.trap_erpp.free_single(unit, trap_id));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief  
*   Deallocate trap id in DNX_ALGO_RX_TRAP_RES_MNGR_ETPP pool
* \par DIRECT INPUT: 
*   \param [in] unit     -  Unit id
*   \param [in] trap_id  -  Trap id to deallocate
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   shr_error_e - Error type
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * None
* \see
*   * None
*/
shr_error_e
dnx_algo_rx_trap_id_etpp_deallocate(
    int unit,
    int trap_id)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * deallocate trap_id
     */
    SHR_IF_ERR_EXIT(algo_rx_db.trap_etpp.free_single(unit, trap_id));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief  
*   Deallocate trap id in DNX_ALGO_RX_TRAP_RES_MNGR_ETPP_OAM pool
* \par DIRECT INPUT: 
*   \param [in] unit     -  Unit id
*   \param [in] trap_id  -  Trap id to deallocate
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   shr_error_e - Error type
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * None
* \see
*   * None
*/
shr_error_e
dnx_algo_rx_trap_id_etpp_oam_deallocate(
    int unit,
    int trap_id)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * deallocate trap_id
     */
    SHR_IF_ERR_EXIT(algo_rx_db.trap_etpp_oam.free_single(unit, trap_id));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   Check whether specific ingress pre-defined trap was already allocated 
*   in DNX_ALGO_RX_TRAP_RES_MNGR_INGRESS_PREDEFINED
* \par DIRECT INPUT: 
*   \param [in] unit            -  Unit ID
*   \param [in] trap_id         -  Pre-defined trap id to check
*   \param [in] is_allocated_p  -  Pointer to a memory type of int. \n
*           \b As \b output - \n
*           This pocedure loads pointed memory by TRUE if the trap_id was allocated, otherwise false.
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   shr_error_e - Error type
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * None
* \see
*   * None
*/
shr_error_e
dnx_algo_rx_trap_id_ingress_predefined_is_allocated(
    int unit,
    int trap_id,
    uint8 *is_allocated_p)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Check if given trap_id is already allocated */
    SHR_IF_ERR_EXIT(algo_rx_db.trap_ingress_predefined.is_allocated(unit, trap_id, is_allocated_p));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief  
*   Check whether specific ingress user define trap was already allocated
*   in DNX_ALGO_RX_TRAP_RES_MNGR_INGRESS_USER_DEFINED
* \par DIRECT INPUT: 
*   \param [in] unit            -  Unit Id
*   \param [in] trap_id         -  User define trap id to check
*   \param [in] is_allocated_p  -  Pointer to a memory type of int. \n
*           \b As \b output - \n
*           This pocedure loads pointed memory by TRUE if ingress user_define trap_id was allocated, otherwise FALSE.
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   shr_error_e - Error type
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * None
* \see
*   * None
*/
shr_error_e
dnx_algo_rx_trap_id_ingress_user_define_is_allocated(
    int unit,
    int trap_id,
    uint8 *is_allocated_p)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Check if given trap_id is already allocated */
    SHR_IF_ERR_EXIT(algo_rx_db.trap_ingress_user_defined.is_allocated(unit, trap_id, is_allocated_p));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief  
*   Check whether specific erpp trap was already allocated
*   in DNX_ALGO_RX_TRAP_RES_MNGR_ERPP
* \par DIRECT INPUT: 
*   \param [in] unit            -  Unit id
*   \param [in] trap_id         -  Id of erpp trap
*   \param [in] is_allocated_p  -  Pointer to a memory type of int. \n
*           \b As \b output - \n
*           This pocedure loads pointed memory by TRUE if the erpp trap_id was allocated, otherwise FALSE.
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   shr_error_e - Error type
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * None
* \see
*   * None
*/
shr_error_e
dnx_algo_rx_trap_id_erpp_is_allocated(
    int unit,
    int trap_id,
    uint8 *is_allocated_p)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Check if given trap_id is already allocated */
    SHR_IF_ERR_EXIT(algo_rx_db.trap_erpp.is_allocated(unit, trap_id, is_allocated_p));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief  
*   Check whether specific etpp trap was already allocated
*   in DNX_ALGO_RX_TRAP_RES_MNGR_ETPP
* \par DIRECT INPUT: 
*   \param [in] unit            -  Unit id
*   \param [in] trap_id         -  Id of etpp trap
*   \param [in] is_allocated_p  -  Pointer to a memory type of int. \n
*           \b As \b output - \n
*           This pocedure loads pointed memory by TRUE if the etpp trap_id was allocated, otherwise FALSE.
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   shr_error_e - Error type
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * None
* \see
*   * None
*/
shr_error_e
dnx_algo_rx_trap_id_etpp_is_allocated(
    int unit,
    int trap_id,
    uint8 *is_allocated_p)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Check if given trap_id is already allocated */
    SHR_IF_ERR_EXIT(algo_rx_db.trap_etpp.is_allocated(unit, trap_id, is_allocated_p));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief  
*   Check whether specific etpp trap was already allocated
*   in DNX_ALGO_RX_TRAP_RES_MNGR_ETPP_OAM
* \par DIRECT INPUT: 
*   \param [in] unit            -  Unit id
*   \param [in] trap_id         -  Id of etpp oam trap
*   \param [in] is_allocated_p  -  Pointer to a memory type of int. \n
*           \b As \b output - \n
*           This pocedure loads pointed memory by TRUE if the etpp oam trap_id was allocated, otherwise FALSE.
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   shr_error_e - Error type
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * None
* \see
*   * None
*/
shr_error_e
dnx_algo_rx_trap_id_etpp_oam_is_allocated(
    int unit,
    int trap_id,
    uint8 *is_allocated_p)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Check if given trap_id is already allocated */
    SHR_IF_ERR_EXIT(algo_rx_db.trap_etpp_oam.is_allocated(unit, trap_id, is_allocated_p));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   Allocate trap id in DNX_ALGO_RX_TRAP_RES_MNGR_PROTOCOL_ICMP_TYPE_MAP pool
* \par DIRECT INPUT:
*   \param [in] unit                    - Unit id
*   \param [in] alloc_flags             - Allocation Flags
*   \param [out] compressed_type_p      - Compressed ICMP type
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e - Error type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
shr_error_e
dnx_algo_rx_trap_icmp_compressed_type_allocate(
    int unit,
    int alloc_flags,
    int *compressed_type_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(compressed_type_p, _SHR_E_PARAM, "compressed_type_p");

    /*
     * allocate ICMP Compressed type
     */
    SHR_IF_ERR_EXIT(algo_rx_db.trap_protocol_icmp_type_map.allocate_single(unit, alloc_flags, NULL, compressed_type_p));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   Check if the ICMP compression type is allocated.
* \par DIRECT INPUT:
*   \param [in] unit            -  Unit id
*   \param [in] compressed_type -  ICMP compressed type
*   \param [out] is_allocated_p -  Is allocated
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e - Error type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
shr_error_e
dnx_algo_rx_trap_icmp_compressed_type_is_allocated(
    int unit,
    int compressed_type,
    uint8 *is_allocated_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(is_allocated_p, _SHR_E_PARAM, "is_allocated_p");
    /*
     * deallocate ICMP compressed type
     */
    SHR_IF_ERR_EXIT(algo_rx_db.trap_protocol_icmp_type_map.is_allocated(unit, compressed_type, is_allocated_p));

exit:
    SHR_FUNC_EXIT;
}
/**
* \brief
*   Deallocate ICMP compression type
* \par DIRECT INPUT:
*   \param [in] unit            -  Unit id
*   \param [in] compressed_type -  ICMP compressed type
* \par INDIRECT INPUT:
*   * None
* \par DIRECT OUTPUT:
*   shr_error_e - Error type
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * None
*/
shr_error_e
dnx_algo_rx_trap_icmp_compressed_type_deallocate(
    int unit,
    int compressed_type)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * deallocate ICMP compressed type
     */
    SHR_IF_ERR_EXIT(algo_rx_db.trap_protocol_icmp_type_map.free_single(unit, compressed_type));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Initialize resource manager for ingress traps.
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit ID
 *   
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \par INDIRECT OUTPUT
 *   * RX resource manager tables
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_rx_trap_ingress_res_mngr_init(
    int unit)
{
    sw_state_algo_res_create_data_t data;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&data, 0, sizeof(sw_state_algo_res_create_data_t));
    /*
     * Ingress Predefined Trap resource manager  
     */
    {

        /** First trap id in trap res mngr */
        data.first_element = 0;
        /** Number of ingress predefined traps (highset HW trap ID) */
        data.nof_elements = dnx_data_trap.ingress.nof_predefeind_traps_get(unit);
        /** Currently sniffing isn't done per core */
        data.flags = 0;
        sal_strncpy(data.name, DNX_ALGO_RX_TRAP_RES_MNGR_INGRESS_PREDEFINED,
                    SW_STATE_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
        /*
         * data.desc = "TRAP - pool for ingress predefined traps";
         */
        SHR_IF_ERR_EXIT(algo_rx_db.trap_ingress_predefined.create(unit, &data, NULL));
    }

    /*
     * Ingress User-Defined Trap resource manager  
     */
    {

        /** First trap id in trap res mngr */
        data.first_element = 0;
        /** Number of ingress user-defined traps */
        data.nof_elements = dnx_data_trap.ingress.nof_user_defined_traps_get(unit);
        /** Currently sniffing isn't done per core */
        data.flags = 0;
        sal_strncpy(data.name, DNX_ALGO_RX_TRAP_RES_MNGR_INGRESS_USER_DEFINED,
                    SW_STATE_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
        /*
         * data.desc = "TRAP - pool for ingress user-defined traps";
         */
        SHR_IF_ERR_EXIT(algo_rx_db.trap_ingress_user_defined.create(unit, &data, NULL));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Initialize resource manager for ERPP traps
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit ID
 *   
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \par INDIRECT OUTPUT
 *   * RX resource manager tables
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_rx_trap_erpp_res_mngr_init(
    int unit)
{
    sw_state_algo_res_create_data_t data;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&data, 0, sizeof(sw_state_algo_res_create_data_t));

    /** ERPP Trap resource manager  */
    {

        /** First trap id in trap res mngr - ID 0 is reserved as BCM_RX_TRAP_EG_TRAP_ID_DEFAULT*/
        data.first_element = DNX_RX_TRAP_ID_TYPE_GET(BCM_RX_TRAP_EG_TRAP_ID_DEFAULT) + 1;
        /** Number of erpp traps */
        data.nof_elements = dnx_data_trap.erpp.nof_erpp_user_configurable_profiles_get(unit);
        /** Currently sniffing isn't done per core */
        data.flags = 0;
        sal_strncpy(data.name, DNX_ALGO_RX_TRAP_RES_MNGR_ERPP, SW_STATE_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
        /*
         * data.desc = "TRAP - pool for ERPP traps";
         */
        SHR_IF_ERR_EXIT(algo_rx_db.trap_erpp.create(unit, &data, NULL));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Initialize resource manager for etpp traps.
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit ID
 *   
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \par INDIRECT OUTPUT
 *   * RX resource manager tables
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_rx_trap_etpp_res_mngr_init(
    int unit)
{
    sw_state_algo_res_create_data_t data;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&data, 0, sizeof(sw_state_algo_res_create_data_t));

    /** ETPP Trap resource manager  */
    {
        /** First trap id in trap res mngr - ID 0 is reserved as BCM_RX_TRAP_EG_TX_TRAP_ID_DEFAULT
                *                                          ID 1 is reserved as BCM_RX_TRAP_EG_TX_TRAP_ID_DROP
                */
        data.first_element = DNX_RX_TRAP_ETPP_FWD_ACT_PROFILE_USER_TRAP_1;
        /** Number of erpp traps */
        data.nof_elements = dnx_data_trap.etpp.nof_etpp_user_configurable_profiles_get(unit);
        /** Currently isn't done per core */
        data.flags = 0;
        sal_strncpy(data.name, DNX_ALGO_RX_TRAP_RES_MNGR_ETPP, SW_STATE_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
        /*
         * data.desc = "TRAP - pool for ETPP traps";
         */
        SHR_IF_ERR_EXIT(algo_rx_db.trap_etpp.create(unit, &data, NULL));
    }

    /** ETPP OAM Trap resource manager  */
    {

        /** First trap id in trap res mngr */
        data.first_element = 0;
        /** Number of erpp traps */
        data.nof_elements = dnx_data_trap.etpp.nof_etpp_oam_traps_get(unit);
        /** Currently isn't done per core */
        data.flags = 0;
        sal_strncpy(data.name, DNX_ALGO_RX_TRAP_RES_MNGR_ETPP_OAM, SW_STATE_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);
        /*
         * data.desc = "TRAP - pool for ETPP OAM traps";
         */
        SHR_IF_ERR_EXIT(algo_rx_db.trap_etpp_oam.create(unit, &data, NULL));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Initialize resource manager for ERPP traps
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit ID
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
 *   * RX resource manager tables
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_rx_trap_icmp_compressed_type_res_mngr_init(
    int unit)
{
    sw_state_algo_res_create_data_t data;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&data, 0, sizeof(sw_state_algo_res_create_data_t));

    /** ICMP Type compression resource manager  */
    {

        data.first_element = 1;
        /** Number of ICMP Compressed types traps */
        data.nof_elements = dnx_data_trap.ingress.nof_icmp_compressed_types_get(unit) - 1;
        data.flags = 0;
        sal_strncpy(data.name, DNX_ALGO_RX_TRAP_RES_MNGR_PROTOCOL_ICMP_TYPE_MAP,
                    SW_STATE_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

        SHR_IF_ERR_EXIT(algo_rx_db.trap_protocol_icmp_type_map.create(unit, &data, NULL));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Initialize resource manager for Rx traps.
 * 
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit ID
 *   
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e 
 * \par INDIRECT OUTPUT
 *   * RX resource manager tables
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_rx_trap_res_mngr_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_rx_trap_ingress_res_mngr_init(unit));

    SHR_IF_ERR_EXIT(dnx_algo_rx_trap_erpp_res_mngr_init(unit));

    SHR_IF_ERR_EXIT(dnx_algo_rx_trap_etpp_res_mngr_init(unit));

    SHR_IF_ERR_EXIT(dnx_algo_rx_trap_icmp_compressed_type_res_mngr_init(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \see
 *   algo_rx.h file
 */
void
dnx_algo_rx_trap_recycle_cmd_print_entry_cb(
    int unit,
    const void *data,
    dnx_algo_template_print_t * print_cb_data)
{
    dnx_rx_trap_recycle_cmd_config_t *template_data = (dnx_rx_trap_recycle_cmd_config_t *) data;
    DNX_ALGO_TEMPLATE_PRINT_CB_INIT_VARIABLES(print_cb_data);

    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_UINT8, "recycle_forward_strength",
                                        template_data->recycle_strength, NULL, "%d");
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_UINT32, "ingress_trap_id",
                                        template_data->ingress_trap_id, NULL, "%d");
    DNX_ALGO_TEMPLATE_PRINT_CB_FINISH(print_cb_data);
    return;
}

/**
 * \see
 *   algo_rx.h file
 */
void
dnx_algo_rx_trap_gport_print_entry_cb(
    int unit,
    const void *data,
    dnx_algo_template_print_t * print_cb_data)
{
    bcm_gport_t *template_data = (bcm_gport_t *) data;
    DNX_ALGO_TEMPLATE_PRINT_CB_INIT_VARIABLES(print_cb_data);

    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_UINT32, "trap_gport", *template_data, NULL, "%d");
    DNX_ALGO_TEMPLATE_PRINT_CB_FINISH(print_cb_data);
    return;
}

/**
 * \see
 *   algo_rx.h file
 */
void
dnx_algo_rx_trap_mtu_profile_print_entry_cb(
    int unit,
    const void *data,
    dnx_algo_template_print_t * print_cb_data)
{
    dnx_rx_trap_mtu_profile_config_t *template_data = (dnx_rx_trap_mtu_profile_config_t *) data;
    DNX_ALGO_TEMPLATE_PRINT_CB_INIT_VARIABLES(print_cb_data);

    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_UINT32, "mtu", template_data->mtu, NULL, "%d");
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_UINT8, "fwd_strength",
                                        template_data->fwd_strength, NULL, "%d");
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_UINT8, "snp_strength",
                                        template_data->snp_strength, NULL, "%d");
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_UINT8, "trap_action_profile",
                                        template_data->trap_action_profile, NULL, "%d");
    DNX_ALGO_TEMPLATE_PRINT_CB_ADD_LINE(TEMPLATE_MNGR_PRINT_TYPE_UINT8, "compressed_fwd_layer_type",
                                        template_data->compressed_layer_type, NULL, "%d");
    DNX_ALGO_TEMPLATE_PRINT_CB_FINISH(print_cb_data);
    return;
}

/**
 * \brief - Create template manager for recycle command. 
 * One profile is used as default profile (profile number 0). 
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit ID
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
 *   * RX template manager table
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_rx_trap_recycle_cmd_template_init(
    int unit)
{

    sw_state_algo_template_create_data_t data;
    dnx_rx_trap_recycle_cmd_config_t default_profile, profile;
    int profile_id;
    uint8 is_first, is_last;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&data, 0, sizeof(data));
    sal_memset(&default_profile, 0, sizeof(default_profile));

    /** Set data for template create */
    data.first_profile = 0;
    data.nof_profiles = dnx_data_trap.ingress.nof_recycle_cmds_get(unit);
    data.max_references = dnx_data_trap.etpp.nof_etpp_app_traps_get(unit)
        + dnx_data_trap.etpp.nof_etpp_oam_traps_get(unit);
    data.data_size = sizeof(dnx_rx_trap_recycle_cmd_config_t);
    data.default_data = &default_profile;
    data.default_profile = 0;
    data.flags = SW_STATE_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE;
    sal_strncpy(data.name, DNX_ALGO_RX_TRAP_TEMPLATE_MNGR_RECYCLE_CMD, SW_STATE_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_rx_db.trap_recycle_cmd.create(unit, &data, NULL));

    /** Allocate reserved recycle commands */
    profile_id = DNX_RX_TRAP_FWD_RCY_PROFILE_EGRESS_TRAPPED;
    /*
     * This structure must be fully initialized since it is use (all of it) as
     * key down in the hash table utilities. (See 'key' in sw_state_hash_table_find_entry())
     */
    sal_memset(&profile, 0, sizeof(profile));
    profile.ingress_trap_id = DBAL_ENUM_FVAL_INGRESS_TRAP_ID_EGRESS_TRAPPED_2ND_PASS;
    profile.recycle_strength = dnx_data_trap.strength.max_strength_get(unit);
    SHR_IF_ERR_EXIT(algo_rx_db.trap_recycle_cmd.exchange
                    (unit, _SHR_CORE_ALL, SW_STATE_ALGO_TEMPLATE_ALLOCATE_WITH_ID, &profile, 0, NULL, &profile_id,
                     &is_first, &is_last));
    profile_id = DNX_RX_TRAP_FWD_RCY_PROFILE_OAM_UPMEP_REFLECTOR;
    profile.ingress_trap_id = DBAL_ENUM_FVAL_INGRESS_TRAP_ID_TRAP_NONE;
    profile.recycle_strength = 0;
    SHR_IF_ERR_EXIT(algo_rx_db.trap_recycle_cmd.exchange
                    (unit, _SHR_CORE_ALL, SW_STATE_ALGO_TEMPLATE_ALLOCATE_WITH_ID, &profile, 0, NULL, &profile_id,
                     &is_first, &is_last));
    profile_id = DNX_RX_TRAP_FWD_RCY_PROFILE_DROP;
    profile.ingress_trap_id = DBAL_ENUM_FVAL_INGRESS_TRAP_ID_LLR_ACCEPTABLE_FRAME_TYPE0;
    profile.recycle_strength = dnx_data_trap.strength.max_strength_get(unit);
    SHR_IF_ERR_EXIT(algo_rx_db.trap_recycle_cmd.exchange
                    (unit, _SHR_CORE_ALL, SW_STATE_ALGO_TEMPLATE_ALLOCATE_WITH_ID, &profile, 0, NULL, &profile_id,
                     &is_first, &is_last));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Create template manager for recycle command.
 * One profile is used as default profile (profile number 0).
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit ID
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
 *   * RX template manager table
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_rx_trap_mtu_profile_template_init(
    int unit)
{

    sw_state_algo_template_create_data_t data;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&data, 0, sizeof(data));

    /** Set data for template create */
    data.first_profile = 0;
    data.nof_profiles = dnx_data_trap.etpp.nof_mtu_profiles_get(unit);
    data.max_references = UTILEX_I32_MAX;
    data.data_size = sizeof(dnx_rx_trap_mtu_profile_config_t);

    data.advanced_algorithm = DNX_ALGO_TEMPLATE_ADVANCED_ALGORITHM_TRAP_MTU_PROFILE;
    data.flags = SW_STATE_ALGO_TEMPLATE_CREATE_USE_ADVANCED_ALGORITHM;
    sal_strncpy(data.name, DNX_ALGO_RX_TRAP_TEMPLATE_MNGR_MTU_PROFILE, SW_STATE_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_rx_db.trap_mtu_profile.create(unit, &data, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Create template manager for ingress lif traps. 
 * One profile is used as default profile (profile number 0). 
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit ID
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
 *   * RX template manager table
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_rx_trap_lif_ingress_template_init(
    int unit)
{

    sw_state_algo_template_create_data_t data;
    bcm_gport_t default_profile = BCM_GPORT_INVALID;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&data, 0, sizeof(data));

    /** Set data for template create */
    data.first_profile = 0;
    data.nof_profiles = dnx_data_trap.ingress.nof_lif_traps_get(unit);
    data.max_references = dnx_data_lif.in_lif.nof_in_lif_profiles_get(unit);
    data.data_size = sizeof(bcm_gport_t);
    data.default_data = &default_profile;
    data.default_profile = 0;
    data.flags = SW_STATE_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE;
    sal_strncpy(data.name, DNX_ALGO_RX_TRAP_TEMPLATE_MNGR_INGRESS_LIF, SW_STATE_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_rx_db.trap_lif_ingress.create(unit, &data, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Create template manager for ETPP lif traps. 
 * One profile is used as default profile (profile number 0). 
 * \par DIRECT_INPUT:
 *   \param [in] unit - Unit ID
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
 *   * RX template manager table
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_algo_rx_trap_lif_etpp_template_init(
    int unit)
{

    sw_state_algo_template_create_data_t data;
    bcm_gport_t default_profile = BCM_GPORT_INVALID;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&data, 0, sizeof(data));

    /** Set data for template create */
    data.first_profile = 0;
    data.nof_profiles = dnx_data_trap.etpp.nof_etpp_lif_traps_get(unit);
    data.max_references = dnx_data_lif.out_lif.nof_out_lif_profiles_get(unit);
    data.data_size = sizeof(bcm_gport_t);
    data.default_data = &default_profile;
    data.default_profile = DNX_RX_TRAP_LIF_PROFILE_DEFAULT;
    data.advanced_algorithm = DNX_ALGO_TEMPLATE_ADVANCED_ALGORITHM_TRAP_ETPP_LIF_PROFILE;
    data.flags = SW_STATE_ALGO_TEMPLATE_CREATE_USE_DEFAULT_PROFILE
        | SW_STATE_ALGO_TEMPLATE_CREATE_USE_ADVANCED_ALGORITHM;
    sal_strncpy(data.name, DNX_ALGO_RX_TRAP_TEMPLATE_MNGR_ETPP_LIF, SW_STATE_ALGO_TEMPLATE_MNGR_MAX_NAME_LENGTH - 1);

    SHR_IF_ERR_EXIT(algo_rx_db.trap_lif_etpp.create(unit, &data, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief  
*    Create Algo Rx traps templates.
* \par DIRECT INPUT: 
*   \param [in] unit  - Unit ID 
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   shr_error_e - Error
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * None
* \see
*   * None
*/
shr_error_e
dnx_algo_rx_trap_template_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_rx_trap_recycle_cmd_template_init(unit));

    SHR_IF_ERR_EXIT(dnx_algo_rx_trap_lif_ingress_template_init(unit));

    SHR_IF_ERR_EXIT(dnx_algo_rx_trap_lif_etpp_template_init(unit));

    SHR_IF_ERR_EXIT(dnx_algo_rx_trap_mtu_profile_template_init(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief  
*    Initialize Algo Rx module.
* \par DIRECT INPUT: 
*   \param [in] unit  - Unit ID 
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   shr_error_e - Error
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * None
* \see
*   * None
*/
shr_error_e
dnx_algo_rx_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Init sw state algo_rx_db.
     */
    SHR_IF_ERR_EXIT(algo_rx_db.init(unit));

    SHR_IF_ERR_EXIT(dnx_algo_rx_trap_res_mngr_init(unit));

    SHR_IF_ERR_EXIT(dnx_algo_rx_trap_template_init(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief  
*    De-Initialize Algo Rx traps resource managers.
* \par DIRECT INPUT: 
*   \param [in] unit  - Unit Id 
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   shr_error_e - Error type
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_algo_rx_trap_res_mngr_deinit(
    int unit)
{

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Sw state resources will be deinit centralized.
     */

    /** Ingress traps deinit*/

    /** ERPP traps deinit*/

    /** ETPP traps deinit*/

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief  
*    De-Initialize Algo Rx traps templates.
* \par DIRECT INPUT: 
*   \param [in] unit  - Unit Id 
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   shr_error_e - Error type
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_algo_rx_trap_template_deinit(
    int unit)
{

    SHR_FUNC_INIT_VARS(unit);
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief  
*    De-Initialize Algo Rx module.
* \par DIRECT INPUT: 
*   \param [in] unit  - Unit Id 
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   shr_error_e - Error type
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * None
* \see
*   * None
*/
shr_error_e
dnx_algo_rx_deinit(
    int unit)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_rx_trap_res_mngr_deinit(unit));

    SHR_IF_ERR_EXIT(dnx_algo_rx_trap_template_deinit(unit));

exit:
    SHR_FUNC_EXIT;
}
