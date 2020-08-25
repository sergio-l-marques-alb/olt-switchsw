/** \file dnx_visibility.h
 *  Miscellaneous routines for managing visibility capabilities in JR2 and further
 */

#ifndef INCLUDE_SOC_DNX_VISIBILITY_H_
#define INCLUDE_SOC_DNX_VISIBILITY_H_

#include <shared/shrextend/shrextend_error.h>

#include <soc/dnx/pp_stage.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_device.h>

#include <bcm/instru.h>
#include <bcm/types.h>

#define DNX_VISIBILITY_RETRY_READY_COUNT        3
#define DNX_VISIBILITY_BLOCK_UNDEFINED          -1

/*
 * Visibility mask in PRT Qualifier
 * There are three bits, second one is visibility
 * Any qualifier that conform to  (QUAL & DNX_VISIBILITY_BIT) == DNX_VISIBILITY_BIT will be visibility enabled
 */
#define DNX_VISIBILITY_PRT_QUAL_MASK        (1 << 1)
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
 * \brief - Check memory status of entire direction
 *
 * \param [in] unit       - Unit id
 * \param [in] core       - core ID
 * \param [in] direction  - Memory readiness is checked per direction
 *                          BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS or
 *                          BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS  or
 *                          BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS | BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS
 * \param [in] block      - specific PP block, if direction is not fine enough
 * \return
 *      _SHR_E_NONE  - memory ready for read
 *      _SHR_E_EMPTY - memory not ready yet
 *      other errors - per standard shr_error_e
 * \remark
 *      If memory is ready
 * \see
 *    dnx_debug_mem_is_ready
 */
shr_error_e dnx_debug_direction_is_ready(
    int unit,
    int core,
    uint32 direction,
    dbal_enum_value_field_pp_asic_block_e block);

/**
 * \brief - Check memory status
 *
 * \param [in] unit      - Unit id
 * \param [in] core      - core ID
 * \param [in] direction - We may set for Ingress, Egress or Both
 *
 * \return
 *      _SHR_E_NONE  - memory ready for read
 *      _SHR_E_EMPTY - memory not ready yet
 *      other errors - per standard shr_error_e
 * \remark
 *      If memory is ready e
 * \see
 *    dnx_visibility_mode_set, dnx_visibility_mode_get
 */
shr_error_e dnx_debug_mem_clear(
    int unit,
    int core,
    uint32 direction);

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
 * Clear debug memory as well, otherwise only invalidate cache, causing just another read from the memory
 */
#define         DNX_VISIBILITY_RESUME_MEM        0x01
/*
 * Zero cache on resume
 */
#define         DNX_VISIBILITY_RESUME_ZERO       0x02

/**
 * \brief - Clean memory and prepare for new packet to enter
 *
 * \param [in] unit      - Unit id
 * \param [in] core      - core ID
 * \param [in] direction - We may set for Ingress, Egress or Both
 * \param [in] flags     - DNX_VISIBILITY_RESUME_* for the list
 *
 * \return
 *      Standard Error
 * \remark
 *      3 step action - disable visibility, clean fifo, enable visibility
 *      Action will be taken only in Selective Mode - error will be return in other modes
 * \remark
 *      Usaully called either on CPU packet send or initiated by User/Test Sequence via shell command
 * \see
 *    dnx_visibility_mode_set, dnx_visibility_mode_get
 */
shr_error_e dnx_visibility_resume(
    int unit,
    int core,
    uint32 direction,
    int flags);

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
shr_error_e
dnx_visibility_pp_port_enable_get(
    int unit,
    int core,
    int pp_port,
    uint32 *enable_p);

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
shr_error_e
dnx_visibility_pp_port_force_get(
    int unit,
    int core,
    int pp_port,
    uint32 *enable_p);

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
 * \brief - Enable visibility for the packets with specific PRT qualifier
 *
 * \param [in] unit - Unit id
 * \param [in] prt_qualifier - prt qualifier enabling visibility
 * \param [in] enable - command to enable(TRUE) or disable(FALSE) prt qualifier
 *
 * \return
 *      Standard Error
 * \remark
 *
 * \see
 *    dnx_visibility_prt_qualifier_get
 */
shr_error_e dnx_visibility_prt_qualifier_set(
    int unit,
    int prt_qualifier,
    int enable);

/**
 * \brief - Get enable visibility status for the packets with specific PRT qualifier
 *
 * \param [in] unit - Unit id
 * \param [in] prt_qualifier - prt qualifier enabling visibility
 * \param [out] enable_p - pointer to enable status for prt qualifier
 *
 * \return
 *      Standard Error
 * \remark
 *
 * \see
 *    dnx_visibility_prt_qualifier_set
 */
shr_error_e dnx_visibility_prt_qualifier_get(
    int unit,
    int prt_qualifier,
    int *enable_p);

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
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Inconsistent %s on:0x%x\n", status_str, gport);      \
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
     * DBAL field representing debug memory valid status - does it contain valid data to be used
     */
    dbal_fields_e valids_field;
    /*
     * Block place inside device
     */
    uint32 direction;
} dnx_visibility_block_map_t;

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
     * Memory width - actually the size of longest line in the bus
     */
    int width;
    /*
     * Actual allocation per line in byte so that is whole number of 512b(64B) chunks
     */
    int line_size;
    /*
     * Memory width - actually the size of longest line in the bus
     */
    char *sig_cash_p[DNX_DATA_MAX_DEVICE_GENERAL_MAX_NOF_CORES];
    /*
     * Last debug memory valids - currently there are no more than 64 lines per block, so 2 * uint32 should be enough
     */
    uint32 valids_data[DNX_DATA_MAX_DEVICE_GENERAL_MAX_NOF_CORES][2];
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
    /*
     * Cache status - FALSE - need to fetch from HW, TRUE we may read from cache
     */
    int ing_sig_cash_status[DNX_DATA_MAX_DEVICE_GENERAL_MAX_NOF_CORES];
    int eg_sig_cash_status[DNX_DATA_MAX_DEVICE_GENERAL_MAX_NOF_CORES];
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
