/**
 *\file kbp_common.h
 * Common utilities for KAPS and ELK.
 */
/**
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef DNX_KBP_COMMON_H
/*
 * {
 */
#define DNX_KBP_COMMON_H

#include <shared/shrextend/shrextend_error.h>
#include <soc/dnx/dbal/dbal_structures.h>

extern const int g_kbp_compiled;

#define KBP_COMPILATION_CHECK                                                                                   \
    if (!g_kbp_compiled)                                                                                        \
    {                                                                                                           \
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Cannot perform KBP action, KBP lib not compiled");                       \
    }

#if defined(INCLUDE_KBP)

#include <soc/kbp/alg_kbp/include/errors.h>
#include <soc/kbp/alg_kbp/include/default_allocator.h>
#include <soc/kbp/alg_kbp/include/db.h>
#include <soc/kbp/alg_kbp/include/ad.h>
#include <soc/kbp/alg_kbp/include/instruction.h>
#include <soc/kbp/alg_kbp/include/errors.h>
#include <soc/kbp/alg_kbp/include/device.h>
#include <soc/kbp/alg_kbp/include/key.h>
#include <soc/kbp/alg_kbp/include/init.h>
#include <soc/kbp/alg_kbp/include/kbp_pcie.h>
#include <soc/kbp/alg_kbp/include/arch.h>
#include <soc/kbp/alg_kbp/include/hw_limits.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_elk.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_elk.h>
#include <soc/dnx/swstate/auto_generated/access/kbp_access.h>

/**
 * Holds the translation from KBP errors to SHR errors
 */
extern const shr_error_e dnx_kbp_error_translation[KBP_STATUS_LAST_UNUSED];
#define KBP_TO_DNX_RESULT(result) ((result < 0) ? _SHR_E_INTERNAL : dnx_kbp_error_translation[result])

#ifndef KBP_WARMBOOT_TYPE_DEFINED
#define KBP_WARMBOOT_TYPE_DEFINED

#define KBP_FWD_TCAM_ACCESS kbp_fwd_tcam_access_mapper

/** Maximum FWD key and result sizes */
#define DNX_KBP_MAX_FWD_KEY_SIZE_IN_BITS        320 /** based on the largest FWD key for IPv6 */
#define DNX_KBP_MAX_FWD_KEY_SIZE_IN_BYTES       BITS2BYTES(DNX_KBP_MAX_FWD_KEY_SIZE_IN_BITS)
#define DNX_KBP_MAX_FWD_KEY_SIZE_IN_WORDS       BITS2WORDS(DNX_KBP_MAX_FWD_KEY_SIZE_IN_BITS)
#define DNX_KBP_MAX_FWD_PAYLOAD_SIZE_IN_BITS    DNX_KBP_RESULT_SIZE_FWD
#define DNX_KBP_MAX_FWD_PAYLOAD_SIZE_IN_BYTES   BITS2BYTES(DNX_KBP_MAX_FWD_PAYLOAD_SIZE_IN_BITS)
#define DNX_KBP_MAX_FWD_PAYLOAD_SIZE_IN_WORDS   BITS2WORDS(DNX_KBP_MAX_FWD_PAYLOAD_SIZE_IN_BITS)

/** Maximum ACL key and result sizes */
#define DNX_KBP_MAX_ACL_KEY_SIZE_IN_BITS        DNX_KBP_MAX_KEY_LENGTH_IN_BITS
#define DNX_KBP_MAX_ACL_KEY_SIZE_IN_BYTES       BITS2BYTES(DNX_KBP_MAX_ACL_KEY_SIZE_IN_BITS)
#define DNX_KBP_MAX_ACL_KEY_SIZE_IN_WORDS       BITS2WORDS(DNX_KBP_MAX_ACL_KEY_SIZE_IN_BITS)
#define DNX_KBP_MAX_ACL_PAYLOAD_SIZE_IN_BITS    DNX_KBP_MAX_PAYLOAD_LENGTH_IN_BITS
#define DNX_KBP_MAX_ACL_PAYLOAD_SIZE_IN_BYTES   BITS2BYTES(DNX_KBP_MAX_ACL_PAYLOAD_SIZE_IN_BITS)
#define DNX_KBP_MAX_ACL_PAYLOAD_SIZE_IN_WORDS   BITS2WORDS(DNX_KBP_MAX_ACL_PAYLOAD_SIZE_IN_BITS)

/*
 * kbp device warmboot info
 */
typedef struct kbp_warmboot_s
{
    FILE *kbp_file_fp;
    kbp_device_issu_read_fn kbp_file_read;
    kbp_device_issu_write_fn kbp_file_write;
} kbp_warmboot_t;
#endif /* KBP_WARMBOOT_TYPE_DEFINED */

typedef enum
{
    /*
     * No device
     */
    DNX_KBP_DEV_TYPE_NONE = 0,
    /*
     * Optimus Prime
     */
    DNX_KBP_DEV_TYPE_BCM52311 = 1,
    /*
     * Optimus Prime 2
     */
    DNX_KBP_DEV_TYPE_BCM52321 = 2,
    /*
     *  Number of device types
     */
    DNX_KBP_NOF_DEV_TYPE = 2,
} dnx_kbp_device_type_e;

/**
 * Convenience macro to wrap function calls, capture and translate error codes and their descriptions.
 *
 * A verbose error is printed when an error occurs.
 *
 * Based on KBP_TRY.
 */
#define DNX_KBP_TRY(A)                                                                                              \
    do                                                                                                              \
    {                                                                                                               \
        kbp_status __tmp_status = A;                                                                                \
        if (__tmp_status != KBP_OK)                                                                                 \
        {                                                                                                           \
            SHR_IF_ERR_EXIT_WITH_LOG(dnx_kbp_error_translation[__tmp_status],                                       \
                                       "\n"#A" failed: %s%s%s\n",                                                   \
                                       kbp_get_status_string(__tmp_status), EMPTY, EMPTY);                          \
        }                                                                                                           \
    }                                                                                                               \
    while(0)

/**
 * Convenience macro to wrap function calls, capture and translate error codes and their descriptions.
 *
 * A verbose error is printed when an error occurs.
 *
 * Based on KBP_TRY.
 */
#define DNX_KBP_TRY_PRINT(A, str, val)                                                                              \
    do                                                                                                              \
    {                                                                                                               \
        kbp_status __tmp_status = A;                                                                                \
        if (__tmp_status != KBP_OK)                                                                                 \
        {                                                                                                           \
            LOG_CLI((BSL_META("%s %d"), str, val));                                                                 \
            SHR_IF_ERR_EXIT_WITH_LOG(dnx_kbp_error_translation[__tmp_status],                                       \
                                       "\n"#A" failed: %s%s%s\n",                                                   \
                                       kbp_get_status_string(__tmp_status), EMPTY, EMPTY);                          \
        }                                                                                                           \
    }                                                                                                               \
    while(0)

/**
 * Convenience macro to wrap retrieval of db_p and ad_db_p from dbal_entry_handle.
 * Used in all dbal_kbp_access APIs.
 */
#define DNX_KBP_RETRIEVE_HANDLES_FROM_DBAL_ENTRY(_dbal_entry_handle_, _db_p_, _ad_db_p_)                            \
    do                                                                                                              \
    {                                                                                                               \
        if (_dbal_entry_handle_->table->kbp_handles)                                                                \
        {                                                                                                           \
            _db_p_ = ((kbp_db_handles_t *) _dbal_entry_handle_->table->kbp_handles)->db_p;                   \
            _ad_db_p_ = ((kbp_db_handles_t *) _dbal_entry_handle_->table->kbp_handles)->ad_db_p;             \
        }                                                                                                           \
        else                                                                                                        \
        {                                                                                                           \
            SHR_ERR_EXIT(_SHR_E_INTERNAL,                                                                           \
                    "kbp handles not initialized for table %s\n",                                                   \
                    _dbal_entry_handle_->table->table_name);                                                        \
        }                                                                                                           \
    }                                                                                                               \
    while(0)

/**
 * \brief
 * This function calculates and returns the prefix length of the
 * provided payload_mask. It assumes the on bits are aligned to
 * the msb.
 */
uint32 dnx_kbp_calculate_prefix_length(
    uint32 *payload_mask,
    uint32 max_key_size_in_bits);

/**
 * \brief - Creates a mask using a given prefix length.
 * \param [in] unit - Relevant unit.
 * \param [in] prefix_length - The prefix length.
 * \param [in] max_key_size_in_bits - The maximum size of the
 *        key the mask is created for.
 * \param [in] dbal_entry_handle_t - Pointer to a DBAL entry
 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_kbp_prefix_len_to_mask(
    int unit,
    int prefix_length,
    uint32 max_key_size_in_bits,
    dbal_physical_entry_t * entry);

/**
 * \brief - The function converts data from DBAL physical entry
 *        in the byte order the KBP expects the data.
 *        All valid bits will be MSB aligned.
 * \param [in] unit - Relevant unit.
 * \param [in] nof_bits - Number of valid bits in the data array.
 * \param [in] data_in - Pointer to the input array.
 * \param [in] data_out - Pointer to the output array.
 *        handle.
 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_kbp_buffer_dbal_to_kbp(
    int unit,
    uint32 nof_bits,
    uint32 *data_in,
    uint8 *data_out);

/**
 * \brief - The function converts data from KBP in the byte order
 *        the DBAL physical entry expects the data.
 *        All valid bits will be LSB aligned.
 * \param [in] unit - Relevant unit.
 * \param [in] nof_bits - Number of valid bits in the data array.
 * \param [in] data_in - Pointer to the input array.
 * \param [in] data_out - Pointer to the output array.
 *        handle.
 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_kbp_buffer_kbp_to_dbal(
    int unit,
    uint32 nof_bits,
    uint8 *data_in,
    uint32 *data_out);

/**
 * \brief - KBP and KAPS common function for getting an LPM entry.
 * \param [in] db_p - KBP db pointer.
 * \param [in] ad_db_p - KBP ad_db pointer.
 * \param [in] ad_hb_p - KBP hb_db pointer.
 * \param [in] key - The key to be searched for.
 * \param [in] key_size_in_bits - Number of valid bits in the key.
 * \param [out] payload - The found payload.
 * \param [in] payload_size_in_bits - Number of valid bits in the payload.
 * \param [in] prefix_length - The key prefix length.
 * \param [inout] hitbit - Hitbit get indication and the hitbit get result.
 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_kbp_entry_lpm_get(
    int unit,
    kbp_db_t_p db_p,
    kbp_ad_db_t_p ad_db_p,
    kbp_hb_db_t_p hb_db_p,
    uint32 key[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS],
    uint32 key_size_in_bits,
    uint32 payload[DBAL_PHYSICAL_RES_SIZE_IN_WORDS],
    uint32 payload_size_in_bits,
    uint32 prefix_length,
    uint8 *hitbit);

/**
 * \brief - KBP and KAPS common function for adding an LPM entry.
 * \param [in] db_p - KBP db pointer.
 * \param [in] ad_db_p - KBP ad_db pointer.
 * \param [in] ad_hb_p - KBP hb_db pointer.
 * \param [in] key - The key, that will be added.
 * \param [in] key_size_in_bits - Number of valid bits in the key.
 * \param [in] payload - The pauload, that will be added.
 * \param [in] payload_size_in_bits - Number of valid bits in the payload.
 * \param [in] prefix_length - The key prefix length.
 * \param [in] is_for_update - Indication, if the entry needs to be updated.
 * \param [in] hitbit - Hitbit indication to initialize the relevant hitbit entry.
 * \param [in] is_default_entry - Default entry indication. Used only in external KBP to set lower entry priority.
 *
 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_kbp_entry_lpm_add(
    int unit,
    kbp_db_t_p db_p,
    kbp_ad_db_t_p ad_db_p,
    kbp_hb_db_t_p hb_db_p,
    uint32 key[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS],
    uint32 key_size_in_bits,
    uint32 payload[DBAL_PHYSICAL_RES_SIZE_IN_WORDS],
    uint32 payload_size_in_bits,
    uint32 prefix_length,
    uint8 is_for_update,
    uint8 hitbit,
    uint8 is_default_entry);

/**
 * \brief - KBP and KAPS common function for deleting an LPM entry.
 * \param [in] db_p - KBP db pointer.
 * \param [in] ad_db_p - KBP ad_db pointer.
 * \param [in] ad_hb_p - KBP hb_db pointer.
 * \param [in] key - The key to be searched for.
 * \param [in] key_size_in_bits - Number of valid bits in the key.
 * \param [in] prefix_length - The key prefix length.
 * \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_kbp_entry_lpm_delete(
    int unit,
    kbp_db_t_p db_p,
    kbp_ad_db_t_p ad_db_p,
    kbp_hb_db_t_p hb_db_p,
    uint32 key[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS],
    uint32 key_size_in_bits,
    uint32 prefix_length);

/**
 * \brief - KBP and KAPS common function for reading saved state from non-volatile memory.
 * \param [in] handle - Handle to the non-volatile memory, passed to API kbp_device_save_state()/ kbp_device_restore_state().
 * \param [in] buffer - Valid buffer memory in which to read the data. Only the specified size must be copied, or a buffer overrun can result..
 * \param [in] size - The number of bytes to read.
 * \param [in] offset - The offset in non-volatile memory to read from.
 * \return
 *   0 on success and contents of buffer are valid. Nonzero on failure reading data; buffer contains garbage.
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnx_kbp_file_read_func(
    void *handle,
    uint8 * buffer,
    uint32 size,
    uint32 offset);

/**
 * \brief - KBP and KAPS common function for writing saved state into non-volatile memory.
 * \param [in] handle - Handle to the non-volatile memory, passed to API kbp_device_save_state()/ kbp_device_restore_state().
 * \param [in] buffer - Valid buffer memory to take the data from and write into non-volatile memory.
 * \param [in] size - The number of bytes to write.
 * \param [in] offset - The offset in non-volatile memory to write to.
 * \return
 *   0 on success and contents of buffer were written out. Nonzero on failure writing data. This result is fatal.
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
int dnx_kbp_file_write_func(
    void *handle,
    uint8 * buffer,
    uint32 size,
    uint32 offset);

#endif /* defined(INCLUDE_KBP) */

/**
 * \brief - Indicates if KBP device is enabled.
 * \param [in] unit - Relevant unit.
 * \return
 *   TRUE if KBP device is enabled, FALSE otherwise.
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
uint8 dnx_kbp_device_enabled(
    int unit);

/**
 * \brief - Indicates if KBP interface needs to be enabled.
 * \param [in] unit - Relevant unit.
 * \return
 *   TRUE if KBP interface needs to be enabled, FALSE otherwise.
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
uint8 dnx_kbp_interface_enabled(
    int unit);

/**
 * \brief - Calculates the valid KBP key width based on input width.
 * \param [in]  unit  - Relevant unit.
 * \param [in]  width - The input width value.
 * \param [out] valid_width - The calculated valid width.
  \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_kbp_valid_key_width(
    int unit,
    uint32 width,
    uint32 *valid_width);

/**
 * \brief - Calculates the valid KBP result width based on input width.
 * \param [in]  unit  - Relevant unit.
 * \param [in]  width - The input width value.
 * \param [out] valid_width - The calculated valid width.
  \return
 *   Error indication according to shr_error_e enum
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_kbp_valid_result_width(
    int unit,
    uint32 width,
    uint32 *valid_width);

#endif /* DNX_KBP_COMMON_H */
