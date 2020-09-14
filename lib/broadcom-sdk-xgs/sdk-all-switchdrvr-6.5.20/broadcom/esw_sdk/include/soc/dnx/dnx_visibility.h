/** \file dnx_visibility.h
 *  Miscellaneous routines for managing visibility capabilities in JR2 and further
 */

#ifndef INCLUDE_SOC_DNX_VISIBILITY_H_
#define INCLUDE_SOC_DNX_VISIBILITY_H_

#include <shared/shrextend/shrextend_error.h>

#include <soc/dnx/pp_stage.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_tables.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_device.h>

#include <bcm/instru.h>
#include <bcm/types.h>

#define DNX_VISIBILITY_RETRY_READY_COUNT        3
#define DNX_VISIBILITY_BLOCK_UNDEFINED          -1

#define DNX_VISIBILITY_BLOCK_MAX_NOF_LINES         1000

/*
 * Indicates that cache is empty, and need to be fetch from HW
 */
#define DNX_VISIBILITY_SIG_CACHE_STATUS_EMPTY       (0)

/*
 * Indicates that cache contains valid values
 */
#define DNX_VISIBILITY_SIG_CACHE_STATUS_VALID       (1)

/*
 * Indicates that cache contains N/A values - no need to try to fetch again
 */
#define DNX_VISIBILITY_SIG_CACHE_STATUS_VALUE_NA    (2)

/**
 * \brief - Return block name
 *
 * \param [in] unit - Unit id
 * \param [in] asic_block - block id
 *
 * \return
 *      "Unknown" if asic_block is invalid
 *      Block name
 * \remark
 *
 * \see
 *
 */
char *dnx_debug_mem_name(
    int unit,
    dbal_enum_value_field_pp_asic_block_e asic_block);

/**
 * \brief - If visibility mode is selective, fetches all debug buses per core to cache.
 *          Do nothing for none selective mode
 *
 * \param [in] unit       - Unit id
 * \param [in] core       - Core id
 *
 * \return
 *      other errors - per standard shr_error_e
 */
shr_error_e dnx_visibility_selective_fetch_all(
    int unit,
    int core);

/**
 * \brief - Get number of memory lines per block
 *
 * \param [in] unit       - Unit id
 * \param [in] asic_block - Memory readiness is checked per asic block
 * \param [out] size_p    - Number of memory lines will be assigned here
 *
 * \return
 *      other errors - per standard shr_error_e
 */
shr_error_e dnx_debug_mem_lines(
    int unit,
    dbal_enum_value_field_pp_asic_block_e asic_block,
    int *size_p);

/**
 * \brief - Get cache status per block/core
 *
 * \param [in] unit       - Unit id
 * \param [in] core       - core ID
 * \param [in] asic_block - Memory readiness is checked per ASIC block
 * \param [in] line_index - Line index inside the ASIC block
 * \param [out] status_p  - Status of cache will be assigned here
 *
 * \return
 *      other errors - per standard shr_error_e
 */
shr_error_e dnx_debug_mem_cache(
    int unit,
    int core,
    dbal_enum_value_field_pp_asic_block_e asic_block,
    int line_index,
    int *status_p);

/**
 * \brief - Check memory status
 *
 * \param [in] unit       - Unit id
 * \param [in] core       - core ID
 * \param [in] asic_block - Memory readiness is checked per asic block
 *
 * \return
 *      _SHR_E_NONE  - memory ready for read
 *      _SHR_E_EMPTY - memory not ready yet
 *      other errors - per standard shr_error_e
 * \remark
 *      If memory is ready
 * \see
 *    dnx_visibility_mode_set, dnx_visibility_mode_get
 */
shr_error_e dnx_debug_mem_is_ready(
    int unit,
    int core,
    dbal_enum_value_field_pp_asic_block_e asic_block);

/**
 * \brief - Check memory status of specific memory line
 *
 * \param [in] unit       - Unit id
 * \param [in] core       - core ID
 * \param [in] asic_block - Memory readiness is checked per asic block
 * \param [in] line_index - Memory line index
 *
 * \return
 *      _SHR_E_NONE  - memory ready for read
 *      _SHR_E_EMPTY - memory not ready yet
 *      other errors - per standard shr_error_e
 * \remark
 *      If memory is ready
 * \see
 *    dnx_visibility_mode_set, dnx_visibility_mode_get
 */
shr_error_e dnx_debug_mem_line_is_ready(
    int unit,
    int core,
    dbal_enum_value_field_pp_asic_block_e asic_block,
    int line_index);

/**
 * \brief - Check memory status of PP block
 *
 * \param [in] unit       - Unit id
 * \param [in] core       - core ID
 * \param [in] pp_block   - specific PP block according to dnx_pp_block_e
 * \return
 *      _SHR_E_NONE  - memory ready for read
 *      _SHR_E_EMPTY - memory not ready yet
 *      other errors - per standard shr_error_e
 * \see
 *    dnx_debug_mem_is_ready
 */
shr_error_e dnx_debug_block_is_ready(
    int unit,
    int core,
    dnx_pp_block_e pp_block);

/**
 * \brief Read debug register field
 * \param [in]  unit        - Identifier of HW platform.
 * \param [in]  core        - core ID
 * \param [in]  hw_name     - name constructed from DBAL_table.DABL_field,
 * \param [in]  size        - field size to know what kind of get function to use
 * \param [out] value_array - pointer location were memory content will be copied
 * \return
 *      Standard shr_error
 * \remark
 */
shr_error_e dnx_debug_reg_read(
    int unit,
    int core,
    char *hw_name,
    int size,
    uint32 *value_array);

/**
 * \brief Read pem debug register
 * \param [in]  unit        - Identifier of HW platform.
 * \param [in]  core        - core ID
 * \param [in]  asic_block  - asic block, memory is associated with,
 * \param [in]  direction   - 0 - load bus to PEMA, 1 - update bus from PEMA
 * \param [in]  pema_id     - enum for PEMA_ID - key to the PEMA debug table
 * \param [in]  window      -
 * \param [out] value_array - memory allocated by caller to get register data
 * \return
 *      Standard shr_error
 * \remark
 *      Major function is to verify input and call fetch routine
 */
shr_error_e dnx_debug_pema_read(
    int unit,
    int core,
    dbal_enum_value_field_pp_asic_block_e asic_block,
    int direction,
    dbal_enum_value_field_pema_id_e pema_id,
    int window,
    uint32 *value_array);

/**
 * \brief Read debug memory
 * \param [in]  unit        - Identifier of HW platform.
 * \param [in]  core        - core ID
 * \param [in]  asic_block  - asic block, memory is associated with,
 * \param [in]  line_index  - line index, represents certain logical unit
 * \param [out] data_ptr    - pointer, where needed pointer will be returned
 * \return
 *      Standard shr_error
 * \remark
 *      Major function is to verify input and call fetch routine
 * \see
 *      dnx_debug_mem_fetch
 */
shr_error_e dnx_debug_mem_read(
    int unit,
    int core,
    dbal_enum_value_field_pp_asic_block_e asic_block,
    int line_index,
    uint32 **data_ptr);

/*
 * Zero cache on resume
 */
#define         DNX_VISIBILITY_RESUME_ZERO       0x02

/**
 * \brief - Clean memory and prepare for new packet to enter
 *
 * \param [in] unit      - Unit id
 * \param [in] core      - core ID
 * \param [in] flags     - We may set for Ingress, Egress or Both, DNX_VISIBILITY_RESUME_* for the list
 *
 * \return
 *      Standard Error
 * \remark
 *      3 step action - disable visibility, clean fifo, enable visibility
 *      Action will be taken only in Selective Mode - error will be return in other modes
 * \remark
 *      Usually called either on CPU packet send or initiated by User/Test Sequence via shell command
 * \see
 *    dnx_visibility_mode_set, dnx_visibility_mode_get
 */
shr_error_e dnx_visibility_resume(
    int unit,
    int core,
    uint32 flags);

/**
 * \brief - Enable visibility mode for the specified device. Enabling one of mode will disable all the others.
 *
 * \param [in] unit - Unit id
 * \param [in] mode - Visibility mode - see bcm_instru_vis_mode_control_t
 *
 * \return
 *      Standard Error
 * \remark
 *   This enable is not enough to have visible packet recorded, either port, PRT_Qualifier or Statistics should be
 *   enabled
 *   Use dnx_visibility_legacy* routines to manage legacy mode
 *   Only one may be enabled simultaneously
 *   Mode will catch both directions
 * \see
 *    dnx_visibility_port_enable, dnx_visibility_qual_enable, dnx_visibility_sampling_enable
 */
shr_error_e dnx_visibility_mode_set(
    int unit,
    bcm_instru_vis_mode_control_t mode);

/**
 * \brief - Get the enabled mode.
 *
 * \param [in] unit - Unit id
 * \param [out] mode_p - pointer to visibility mode, where mode will be placed in case of success
 * \param [in] full_flag - use to trigger full screening of setting over all blocks, otherwise take one as an example
 *
 * \return
 *    Standard Error
 * \remark
 *    DNX_VISIBILITY_NONE is legitimate mode and will be returned if no mode is enabled
 *    Mode should be the same for all directions
 * \see
 */
shr_error_e dnx_visibility_mode_get(
    int unit,
    bcm_instru_vis_mode_control_t * mode_p,
    int full_flag);

/**
 * \brief - Enable/Disable sampling visibility and sampling period.
 *
 * \param [in] unit - Unit id
 * \param [in] direction - We may set for Ingress, Egress or Both
 * \param [in] period - Sampling period, if 0 sampling will be disabled
 *
 * \return
 *      Standard Error
 * \remark
 *      Sampling disabled by default
 * \see
 *    dnx_visibility_sampling_get
 */
shr_error_e dnx_visibility_sampling_set(
    int unit,
    uint32 direction,
    uint32 period);

/**
 * \brief - Get sampling period, 0 means sampling was disabled.
 *
 * \param [in] unit - Unit id
 * \param [in] direction - We may get for Ingress, Egress or Both
 * \param [out] period_p - pointer to sampling period
 *
 * \return
 *    Standard Error
 * \remark
 *
 * \see
 *    dnx_visibility_sampling_set
 */
shr_error_e dnx_visibility_sampling_get(
    int unit,
    uint32 direction,
    uint32 *period_p);

/**
 * \brief - Enable port visibility per pp_port - Ingress
 *
 * \param [in] unit - Unit id
 * \param [in] core - Core id
 * \param [in] pp_port - pp_port id 8b
 * \param [in] enable - command to enable(TRUE) or disable(FALSE) port
 *
 * \return
 *      Standard Error
 * \remark
 *
 * \see
 *    dnx_visibility_pp_port_enable_get
 */
shr_error_e dnx_visibility_pp_port_enable_set(
    int unit,
    int core,
    int pp_port,
    int enable);

/**
 * \brief - Get port visibility per pp_port - Ingress
 *
 * \param [in] unit - Unit id
 * \param [in] core - Core id
 * \param [in] pp_port - pp_port id 8b
 * \param [out] enable_p - pointer to 'Enable' status
 *
 * \return
 *      Standard Error
 * \remark
 *
 * \see
 *    dnx_visibility_pp_port_enable_set
 */
shr_error_e dnx_visibility_pp_port_enable_get(
    int unit,
    int core,
    int pp_port,
    int *enable_p);

/**
 * \brief - Enable port visibility
 *
 * \param [in] unit - Unit id
 * \param [in] direction - We may set for Ingress, Egress or Both
 * \param [in] gport - global port, trunk and logical ports are actually supported, for others error will be returned
 * \param [in] enable - command to enable(TRUE) or disable(FALSE) port
 *
 * \return
 *      Standard Error
 * \remark
 *
 * \see
 *    dnx_visibility_port_enable_get
 */
shr_error_e dnx_visibility_port_enable_set(
    int unit,
    uint32 direction,
    bcm_gport_t gport,
    int enable);

/**
 * \brief - Disable port visibility per port, packets from this port matching other conditions may still be recorded
 *
 * \param [in] unit - Unit id
 * \param [in] direction - We may get for Ingress, Egress or Both
 * \param [in] gport - global port, trunk and logical ports are actually supported, for others error will be returned
 * \param [out] enable_p - pointer where status will be assigned
 *
 * \return
 *      Standard Error
 * \remark
 *      If request for both direction discovers different status for ingress and egress, error will be returned
 * \see
 *    dnx_visibility_port_enable_set
 */
shr_error_e dnx_visibility_port_enable_get(
    int unit,
    uint32 direction,
    bcm_gport_t gport,
    int *enable_p);

/**
 * \brief - Force port visibility per pp_port - Ingress
 *
 * \param [in] unit - Unit id
 * \param [in] core - Core id
 * \param [in] pp_port - pp_port id 8b
 * \param [in] enable - command to force(TRUE) or unforce(FALSE) port
 *
 * \return
 *      Standard Error
 * \remark
 *
 * \see
 *    dnx_visibility_pp_port_force_get
 */
shr_error_e dnx_visibility_pp_port_force_set(
    int unit,
    int core,
    int pp_port,
    int enable);

/**
 * \brief - Get port visibility force status per pp_port - Ingress
 *
 * \param [in] unit - Unit id
 * \param [in] core - Core id
 * \param [in] pp_port - pp_port id 8b
 * \param [out] enable_p - pointer to 'Force' status
 *
 * \return
 *      Standard Error
 * \remark
 *
 * \see
 *    dnx_visibility_pp_port_force_set
 */
shr_error_e dnx_visibility_pp_port_force_get(
    int unit,
    int core,
    int pp_port,
    int *enable_p);

/**
 * \brief - Forces port visibility - all packets from this port will carry visibility mark
 *
 * \param [in] unit - Unit id
 * \param [in] direction - We may set for Ingress, Egress or Both
 * \param [in] gport - global port, only logical ports are actually supported, for others error will be returned
 * \param [in] enable - command to enable(TRUE) or disable(FALSE) port being forced
 *
 * \return
 *      Standard Error
 * \remark
 *
 * \see
 *    dnx_visibility_port_force_get
 */
shr_error_e dnx_visibility_port_force_set(
    int unit,
    uint32 direction,
    bcm_gport_t gport,
    int enable);

/**
 * \brief - Disable port visibility per port, packets from this port matching other conditions may still be recorded
 *
 * \param [in] unit - Unit id
 * \param [in] direction - We may get for Ingress, Egress or Both
 * \param [in] gport - global port, only logical ports are actually supported, for others error will be returned
 * \param [out] enable_p - pointer where force port status will be assigned
 *
 * \return
 *      Standard Error
 * \remark
 *      If request for both direction discovers different status for ingress and egress, error will be returned
 * \see
 *    dnx_visibility_port_force_set
 */
shr_error_e dnx_visibility_port_force_get(
    int unit,
    uint32 direction,
    bcm_gport_t gport,
    int *enable_p);

/**
 * \brief - Get enable visibility status for the packets with specific PRT qualifier
 *
 * \param [in] unit - Unit id
 * \param [in] prt_qualifier_p - pointer to prt_qualifier that will make traffic visible
 *
 * \return
 *      Standard Error
 */
shr_error_e dnx_visibility_prt_qualifier_get(
    int unit,
    uint32 *prt_qualifier_p);

/**
 * \brief - Enables egress visibility for added local port
 *
 * \param [in] unit - Unit id
 *
 * \return
 *      Standard Error
 * \remark
 *      Initial state should is ALWAYS visible for both direction - visibility application may set things differently
 */
shr_error_e dnx_visibility_port_egress_enable(
    int unit);

/**
 * \brief - Disables egress visibility for removed local port
 *
 * \param [in] unit - Unit id
 *
 * \return
 *      Standard Error
 * \remark
 *      Initial state should is ALWAYS visible for both direction - visibility application may set things differently
 */
shr_error_e dnx_visibility_port_egress_disable(
    int unit);

/**
 * \brief - Initialize visibility module to known state
 *
 * \param [in] unit - Unit id
 *
 * \return
 *      Standard Error
 * \remark
 *      Initial state should is ALWAYS visible for both direction - visibility application may set things differently
 * \see
 *    dnx_visibility_prt_qualifier_set
 */
shr_error_e dnx_visibility_init(
    int unit);

/**
 * \brief - DeInitialize visibility module
 *
 * \param [in] unit - Unit id
 *
 * \return
 *      Standard Error
 * \remark
 *      Initial state should is ALWAYS visible for both direction - visibility application may set things differently
 * \see
 *    dnx_visibility_prt_qualifier_set
 */
shr_error_e dnx_visibility_deinit(
    int unit);

/**
 * \brief - Clear engine visibility so memory will be refreshed on next attempt
 *
 * \param [in] unit - Unit id
 * \param [in] core - core id
 *
 * \return
 * \remark
 * \see
 */
shr_error_e dnx_visibility_engine_clear(
    int unit,
    int core);

/*
 * Definitions limited to visibility module internal usage - not to be used outside
 * {
 */
/**
 * \brief   - Macro verifying that pers_status once modified stays at the same value, insuring that multiple objects have the same
 * \param [in] pers_status - status holding persistent value
 * \param [in] temp_status - status of single object under check
 * \param [in] status_str  - used to identify object

 * \remark
 *      Used to understand device visibility mode
 */
#define DISCERN_STATUS(pers_status, temp_status, status_str)                                    \
    if(pers_status == SAL_UINT32_MAX)                                                           \
    {                                                                                           \
        pers_status = temp_status;                                                              \
    }                                                                                           \
    else                                                                                        \
    {                                                                                           \
        if(pers_status != temp_status)                                                          \
        {                                                                                       \
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Inconsistent %s\n", status_str);                     \
        }                                                                                       \
    }

/**
 * \brief - Structure representing data per ASIC PP block, used to keep static info, not device specific
 */
typedef struct
{
    /*
     * Block name - for output purposes
     */
    char *name;
    /*
     * DBAL table for signals of this block
     */
    dbal_tables_e table;
    /*
     * DBAL field representing debug memory valid status - does it contain valid data to be used
     */
    dbal_fields_e valids_field;
    /*
     * Block place inside device
     */
    uint32 direction;
} dnx_visibility_block_map_t;

#define CHUNK_SIZE_BITS 512
/**
 * \brief - Structure representing dynamic data per ASIC PP block
 */
typedef struct
{
    /*
     * Number of lines (buses) in per block debug memory
     */
    int line_nof;
    /*
     * Memory width - actually the size in bits of longest line in the bus
     */
    int width;
    /*
     * Actual allocation per line in bytes so that it is a multiple of BITS2BYTES(CHUNK_SIZE_BITS)
     */
    int line_size;
    /*
     * Number of maximum size fields (64B) in signal data array
     */
    int data_nof;
    /*
     * Pointer to the signal cash data
     */
    uint32 *sig_cash_p[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES][DNX_VISIBILITY_BLOCK_MAX_NOF_LINES];
    /*
     * Cache status - FALSE - need to fetch from HW, TRUE we may read from cache
     */
    int sig_cash_status[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES][DNX_VISIBILITY_BLOCK_MAX_NOF_LINES];
} dnx_visibility_block_info_t;

/**
 * \brief - Structure representing memory global status
 *          No SwState requested for this data, initialized each time WB or not
 */
typedef struct
{
    /*
     * Mode is set to DNX_VISIBILITY_ALWAYS on init and modified by application
     */
    bcm_instru_vis_mode_control_t mode;
} dnx_visibility_status_t;
/*
 * }
 * End if internal visibility module definitions
 */

#define DNX_PP_VERIFY_DIRECTION(unit, direction_m)                                                   \
    if (!(direction_m & BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS) &&                                \
                                         !(direction_m & BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS))  \
    {                                                                                                \
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal PP Direction value:%d\n", direction_m);                  \
    }

#endif /* INCLUDE_SOC_DNX_VISIBILITY_H_ */
