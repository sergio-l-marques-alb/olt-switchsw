/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

/** \file lif_mngr_api.h
 * $Id$
 *
 * This file contains the public APIs required for lif algorithms.
 *
 */

#ifndef  INCLUDE_LIF_MNGR_API_H_INCLUDED
#define  INCLUDE_LIF_MNGR_API_H_INCLUDED

/*************
 * INCLUDES  *
 *************/
/*
 * {
 */

#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_device.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/algo/res_mngr/res_mngr_types.h>
#include <soc/dnx/mdb.h>
#include <soc/dnx/swstate/auto_generated/types/lif_mngr_types.h>

/*
 * }
 */
/*************
 * DEFINES   *
 *************/
/*
 * {
 */

/**
 * \brief This flag is used to indicate that the allocated global lif should have a specific ID.
 *
 * \see  dnx_lif_mngr_lif_allocate
 */
#define LIF_MNGR_GLOBAL_LIF_WITH_ID   SAL_BIT(0)

/**
 * \brief This flag is used to indicate that no global lif is required for this allocation.
 *
 * \see dnx_lif_mngr_lif_allocate
 */
#define LIF_MNGR_DONT_ALLOCATE_GLOBAL_LIF SAL_BIT(1)

/**
 * \brief This flag is used to allocate an asymmetric global lif, but use an ID that is available on both
 * ingress and egress, so it could be followed with allocating the second side.
 * The second side must be allocated WITH_ID before any other lif is allocated, or it's no longer guaranteed to
 * be reserved.
 *
 * \see dnx_lif_mngr_lif_allocate
 */
#define LIF_MNGR_ONE_SIDED_SYMMETRIC_GLOBAL_LIF SAL_BIT(2)

/**
 * \brief
 * If a global lif is used for AC or PWE, then the vlan and mpls ports don't have enough bits to support the full address
 *   of global lif. Use this flag to indicate that a smaller address should be used.
 */
#define LIF_MNGR_L2_GPORT_GLOBAL_LIF   (SAL_BIT(26))

/**
 *  \brief Illegal lif indication.
 *
 *  \see dnx_lif_mngr_lif_free
 */
#define LIF_MNGR_INVALID             (-1)

/**
 * Flags for lif mngr APIs, internal and external.
 *
 * \see
 *   lif_mapping_local_to_global_get
 *   lif_mapping_global_to_local_get
 *   dnx_algo_lif_mapping_create
 *   dnx_algo_lif_mapping_remove
 *   dnx_algo_global_lif_allocation_allocate
 *   dnx_algo_global_lif_allocation_free
 *  {
 */
/**
 * Perform ingress lif operation.
 */
#define DNX_ALGO_LIF_INGRESS             SAL_BIT(0)
/**
 * Perform egress lif operation.
 */
#define DNX_ALGO_LIF_EGRESS              SAL_BIT(1)

/*
 * lif table manager flags
 */
#define LIF_TABLE_MNGR_LIF_INFO_REPLACE            SAL_BIT(0)
/*
 * Egress: GLEM & SW mapping update is not required
 * Ingress: SW global to local mapping update is not required
 */
#define LIF_TABLE_MNGR_LIF_INFO_DONT_UPDATE_GLOBAL_TO_LOCAL_MAPPING        SAL_BIT(1)

/**
 *********************************************
 lif table manager info table specific flags
 *********************************************
 */
/*
 * MPLS
 */
/** Number of special labels, used for JR2_A0 labels reordering */
#define DNX_ALGO_LIF_TABLE_SPECIFIC_MPLS_ENCAP_ONE_SPECIAL_LABEL       SAL_BIT(0)
#define DNX_ALGO_LIF_TABLE_SPECIFIC_MPLS_ENCAP_TWO_SPECIAL_LABELS      SAL_BIT(1)
#define DNX_ALGO_LIF_TABLE_SPECIFIC_MPLS_ENCAP_THREE_SPECIAL_LABELS    SAL_BIT(2)
/** TANDEM indication */
#define DNX_ALGO_LIF_TABLE_SPECIFIC_MPLS_ENCAP_TANDEM                  SAL_BIT(3)
/** Tunnel is PHP */
#define DNX_ALGO_LIF_TABLE_SPECIFIC_MPLS_ENCAP_PHP                     SAL_BIT(4)
/** Tunnel with two labels */
#define DNX_ALGO_LIF_TABLE_SPECIFIC_MPLS_ENCAP_SECOND_LABEL_EXISTS     SAL_BIT(5)
/** IML tunnel (EVPN) */
#define DNX_ALGO_LIF_TABLE_SPECIFIC_MPLS_ENCAP_IML                     SAL_BIT(6)

/*
 * ARP
 */
/** VLAN translation indication (ARP+AC entry) */
#define DNX_ALGO_LIF_TABLE_SPECIFIC_ARP_VLAN_TRANSLATION               SAL_BIT(0)
/** No VSI on entry */
#define DNX_ALGO_LIF_TABLE_SPECIFIC_ARP_NO_VSI                         SAL_BIT(1)
/** No source address on entry (SMAC is not custom) */
#define DNX_ALGO_LIF_TABLE_SPECIFIC_ARP_NO_SOURCE_IDX                  SAL_BIT(2)

/**
 * }
 */

/*
 * }
 */
/*************
 * MACROS    *
 *************/
/*
 * {
 */

/*
 * }
 */
/*************
 * TYPE DEFS *
 *************/
/*
 * {
 */

/**
 * \brief Allocation info for local inlif allocation.
 *
 * This struct is used by several different lif_mngr APIs (see below), and some of the fields have different
 * meanings in each. The general description for each field is provided here, and the individual
 * APIs hightlight the fields that are use differently.
 *
 * /see
 * dnx_lif_mngr_lif_allocate
 * dnx_lif_mngr_lif_free
 */
typedef struct
{
    /**
     * Dbal table id. This, in combination with the core id, will determine
     * which inlif table to access. This field must be set in all APIs that use this struct.
     */
    dbal_tables_e dbal_table_id;
    /**
     * Dbal result type. This, in combination with the dbal table id, will give us the dbal entry size,
     * which will determine how many inlif indexes need to be allocated.
     * This argument is an integer instead of an enum because result types are using several different enums.
     * This field is ignored in all APIs other than dnx_lif_mngr_lif_allocate.
     */
    int dbal_result_type;
    /**
     * The core in which the inlif is used.
     * There are two types of inlif tables: shared by cores, and duplicated per core.
     * In the first case, 'core_id' should be set to _SHR_CORE_ALL.
     * In the second case, 'core_id' identifies the specific tables. _SHR_CORE_ALL can be used to allocate the
     * inlif on all cores, and core number can be used to allocate on a specific core. After the allocation, all operations
     * using this inlif must be used with the same core indication.
     */
    int core_id;
    /**
     *  The local inlif ID. This can be input or output, depending on the API.
     */
    int local_inlif;
} lif_mngr_local_inlif_info_t;

/* \brief Allocation info for local inlif allocation using string */
typedef struct
{
    char *table_name;
    char *result_type_name;
    int core_id;
    int local_inlif;
} lif_mngr_local_inlif_str_info_t;

/**
 * \brief This enum represents the legal eedb phases, it's just here so they won't be meaningless numbers spread
 *          throughout the code.
 *
 * /see
 * dnx_lif_mngr_lif_allocate
 */
typedef enum
{
    LIF_MNGR_OUTLIF_PHASE_INVALID = -1,
    LIF_MNGR_OUTLIF_PHASE_FIRST,
    LIF_MNGR_OUTLIF_PHASE_AC = LIF_MNGR_OUTLIF_PHASE_FIRST,
    LIF_MNGR_OUTLIF_PHASE_NATIVE_AC,
    LIF_MNGR_OUTLIF_PHASE_ARP,
    LIF_MNGR_OUTLIF_PHASE_NATIVE_ARP,
    LIF_MNGR_OUTLIF_PHASE_MPLS_TUNNEL_1,
    LIF_MNGR_OUTLIF_PHASE_MPLS_TUNNEL_2,
    LIF_MNGR_OUTLIF_PHASE_MPLS_TUNNEL_3,
    LIF_MNGR_OUTLIF_PHASE_MPLS_TUNNEL_4,
    LIF_MNGR_OUTLIF_PHASE_SRV6_BASE,
    LIF_MNGR_OUTLIF_PHASE_IPV6_RAW_SRV6_TUNNEL,
    LIF_MNGR_OUTLIF_PHASE_SRV6_TUNNEL_1,
    LIF_MNGR_OUTLIF_PHASE_SRV6_TUNNEL_2,
    LIF_MNGR_OUTLIF_PHASE_SRV6_TUNNEL_3,
    LIF_MNGR_OUTLIF_PHASE_SRV6_TUNNEL_4,
    LIF_MNGR_OUTLIF_PHASE_IP_TUNNEL_1,
    LIF_MNGR_OUTLIF_PHASE_IP_TUNNEL_2,
    LIF_MNGR_OUTLIF_PHASE_IP_TUNNEL_3,
    LIF_MNGR_OUTLIF_PHASE_IP_TUNNEL_4,
    LIF_MNGR_OUTLIF_PHASE_VPLS_1,
    LIF_MNGR_OUTLIF_PHASE_VPLS_2,
    LIF_MNGR_OUTLIF_PHASE_RIF,
    LIF_MNGR_OUTLIF_PHASE_SFLOW,
    LIF_MNGR_OUTLIF_PHASE_SFLOW_RAW_1,
    LIF_MNGR_OUTLIF_PHASE_SFLOW_RAW_2,
    LIF_MNGR_OUTLIF_PHASE_REFLECTOR,
    LIF_MNGR_OUTLIF_PHASE_RCH,
    LIF_MNGR_OUTLIF_PHASE_LAST = LIF_MNGR_OUTLIF_PHASE_RCH,
    LIF_MNGR_OUTLIF_PHASE_COUNT
} lif_mngr_outlif_phase_e;

/**
 * \brief Allocation info for local outlif allocation.
 *
 * Pass this struct to dnx_lif_mngr_lif_allocate to allocate a local lif with an egress side.
 *
 * /see
 * dnx_lif_mngr_lif_allocate
 */
typedef struct
{
    /**
     * Dbal table id. This, will determine which eedb phase to use.
     * This field must be set in all APIs that use this struct.
     */
    dbal_tables_e dbal_table_id;
    /**
     * Dbal result type. This, in combination with the dbal table id, will give us the dbal entry size,
     * which will determine how many outlif indexes need to be allocated.
     * This argument is an integer instead of an enum because result types are using several different enums.
     * This field is ignored in all APIs other than dnx_lif_mngr_lif_allocate.
     */
    uint32 dbal_result_type;
    /**
     * Phase. The outlifs are divided to phases according to the dbal table id. However, the user may wish to
     * override this value manually. Set this value to 0 to use the default phase. Any other value will override the
     * default phase.
     */
    lif_mngr_outlif_phase_e outlif_phase;
    /**
     *  Flags for local lif allocation. Currently not in use.
     */
    uint32 local_lif_flags;
    /**
     *  Local outlif id that was allocated.
     */
    int local_outlif;
} lif_mngr_local_outlif_info_t;

/* \brief Allocation info for local inlif allocation using string */
typedef struct
{
    char* table_name;
    char* result_type_name;
    char* outlif_phase_name;
    uint32 local_lif_flags;
    int local_outlif;
} lif_mngr_local_outlif_str_info_t;

/**
 * \brief Info for local outlif creation.
 * Includes information that enables manager to decide on optimal result type,
 * allocate the local lif and fill LIF table.
 *
 * /see
 * dnx_lif_table_mngr_allocate_local_lif_and_set_hw
 */
typedef struct
{
    /**
     * Flags LIF_TABLE_MNGR_LIF_INFO_XXX
     */
    uint32 flags;
    /**
     * In case of outlif - the phase requested
     */
    lif_mngr_outlif_phase_e outlif_phase;
    /**
     * Flags that are used for tables where advanced rules should be applied
     */
    uint32 table_specific_flags;
    /**
     * Global lif in case global to local mapping has to be updated
     * Not relevant if LIF_TABLE_MNGR_LIF_INFO_DONT_UPDATE_GLOBAL_TO_LOCAL_MAPPING is set
     */
    uint32 global_lif;

} lif_table_mngr_outlif_info_t;

/**
 * \brief Info for local inlif creation.
 * Includes information that enables manager to decide on optimal result type,
 * allocate the local lif and fill LIF table.
 *
 * /see
 * dnx_lif_table_mngr_allocate_local_lif_and_set_hw
 */
typedef struct
{
    /**
     * Flags LIF_TABLE_MNGR_LIF_INFO_XXX
     */
    uint32 flags;
    
    int core_id;
    /**
     * Flags that are used for tables where advanced rules should be applied
     */
    uint32 table_specific_flags;
    /**
     * Global lif in case global to local mapping has to be updated
     * Not relevant if LIF_TABLE_MNGR_LIF_INFO_DONT_UPDATE_GLOBAL_TO_LOCAL_MAPPING is set
     */
    uint32 global_lif;
} lif_table_mngr_inlif_info_t;

/*
 * }
 */
/*************
 * GLOBALS   *
 *************/
/*
 * {
 */

/*
 * }
 */
/*************
 * FUNCTIONS *
 *************/
/*
 * {
 */

/*
 * GLOBAL LIF ALLOCATION FUNCTIONS - START
 * {
 */

/**
 * \brief
 *   Allocate a global lif.
 *
 * \param [in] unit -
 *   Identifier of the device to access.
 * \param [in] allocation_flags -
 *   Flags for the resource manager. Currently only supports \ref SW_STATE_ALGO_RES_ALLOCATE_WITH_ID.
 * \param [in] direction_flags -
 *   Ingress or egress indication:
 *   \ref DNX_ALGO_LIF_INGRESS and/or \ref DNX_ALGO_LIF_EGRESS
 * \param [in] global_lif -
 *   Int pointer to memory to write output into. \n
 *   \b As \b output - \n
 *     This procedure loads pointed memory with the global lif allocated.
 *   \b As \b input - \n
 *     If allocation flag SW_STATE_ALGO_RES_ALLOCATE_WITH_ID is set, this holds
 *       the id to be allocated.
 *
 * \return DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 *
 * \see
 *   dnx_lif_mngr_lif_allocate
 */
shr_error_e dnx_algo_global_lif_allocation_allocate(
    int unit,
    uint32 allocation_flags,
    uint32 direction_flags,
    int *global_lif);

/**
 * \brief
 *   Free the allocated global lif.
 *
 * \param [in] unit -
 *   Identifier of the device to access.
 * \param [in] direction_flags -
 *   Ingress or egress indication. They must match the direction flags given at creation.
 *   \ref DNX_ALGO_LIF_INGRESS and/or \ref DNX_ALGO_LIF_EGRESS
 * \param [in] global_lif -
 *   Global lif to be freed.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 *
 * \see
 *   dnx_lif_mngr_lif_free
 */
shr_error_e dnx_algo_global_lif_allocation_free(
    int unit,
    uint32 direction_flags,
    int global_lif);

/**
 * }
 * GLOBAL LIF ALLOCATION FUNCTIONS - END
 */

/**
 * LIF MAPPING FUNCTIONS - START
 * {
 */

/**
 * \brief
 *   Create mapping between the given local lif and global lif.
 *
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] flags -
 *     Exactly one of \ref DNX_ALGO_LIF_INGRESS or \ref DNX_ALGO_LIF_EGRESS.
 *   \param [in] global_lif -
 *     Global lif to be mapped.
 *   \param [in] local_lif_info -
 *     Info of the local lif to be mapped.
 * \par INDIRECT INPUT
 *   Ingress and egress global to local and local to global lif maps
 *     in the lif mapping sw state submodule.
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   Ingress and egress global to local and local to global lif maps
 *     in the lif mapping sw state submodule are updated with the mapping.
 * \remark
 *   None.
 */
shr_error_e dnx_algo_lif_mapping_create(
    int unit,
    uint32 flags,
    int global_lif,
    lif_mapping_local_lif_info_t * local_lif_info);

/**
 * \brief
 *   Remove the mapping between the given local lif and global lif.
 *
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] flags -
 *     Exactly one of \ref DNX_ALGO_LIF_INGRESS or \ref DNX_ALGO_LIF_EGRESS.
 *   \param [in] global_lif -
 *     Global lif to be unmapped.
 * \par INDIRECT INPUT
 *   Ingress and egress global to local and local to global lif maps
 *     in the lif mapping sw state submodule.
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   Ingress and egress global to local and local to global lif maps
 *     in the lif mapping sw state submodule are updated with the deleted mapping.
 * \remark
 *   None.
 */
shr_error_e dnx_algo_lif_mapping_remove(
    int unit,
    uint32 flags,
    int global_lif);

/**
 * \brief
 *   Get the local lif mapped to this global lif.
 *
 * The structure lif_mapping_local_lif_info_t is defined in:
 * include/soc/dnx/swstate/auto_generated/types/lif_mngr_types.h
 *
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] flags -
 *     Exactly one of \ref DNX_ALGO_LIF_INGRESS or \ref DNX_ALGO_LIF_EGRESS.
 *   \param [in] global_lif -
 *     The global lif whose mapping we require.
 *   \param [out] local_lif_info -
 *       This procedure loads pointed memory with the local lif info mapped
 *       from the given global lif.
 * \par INDIRECT INPUT
 *   Ingress and egress global to local lif maps in the lif mapping sw state submodule.
 * \par DIRECT OUTPUT
 *   \retval \ref _SHR_E_NOT_FOUND if the global lif's mapping doesn't exist.
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   \b *local_lif -\n
 *     See DIRECT INPUT above
 * \remark
 *   None.
 */
shr_error_e dnx_algo_lif_mapping_global_to_local_get(
    int unit,
    int flags,
    int global_lif,
    lif_mapping_local_lif_info_t * local_lif_info);

/**
 * }
 * LIF MAPPING FUNCTIONS - END
 */

/* LIF MNGR CINT exported API - START */

shr_error_e
dnx_cint_lif_lib_allocate(
    int unit,
    uint32 flags,
    int *global_lif,
    lif_mngr_local_inlif_str_info_t *inlif_str_info,
    lif_mngr_local_outlif_str_info_t *outlif_str_info);


/* LIF MNGR CINT exported API - END */

/*
 * }
 */

#endif /* INCLUDE_LIF_MNGR_API_H_INCLUDED */
