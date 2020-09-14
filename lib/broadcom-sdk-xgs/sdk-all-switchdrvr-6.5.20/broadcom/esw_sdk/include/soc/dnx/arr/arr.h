/*
 * $Id: arr.h,v 1.13 Broadcom SDK $
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef DNX_ARR_H_INCLUDED
#  define DNX_ARR_H_INCLUDED

#include "../../../shared/shrextend/shrextend_error.h"
#include "../../../shared/utilex/utilex_rhlist.h"

/*
 * Defines and macros
 * {
 */
/** \brief ARR string size limitation  */
#  define ARR_MAX_STRING_LENGTH                    128

/** \brief ARR max number of formats  */
#  define ARR_MAX_NUMBER_OF_FORMATS                128

/** \brief ARR max number of fields in formats  */
#  define ARR_MAX_NUMBER_OF_FIELDS_IN_FORMATS      50

/** \brief ARR max number of ARR sets  */
#  define ARR_MAX_NUMBER_OF_ARR_SETS               20
/*
 * }
 * Defines and macros
 */
/*
 * Types
 * {
 */
/**
 *  \brief this struct represents a single field in the ARR.
 */
typedef struct
{
    char name[ARR_MAX_STRING_LENGTH];
    int size;
    int offset;
    int prefix;
    int prefix_value;
    int prefix_size;
} arr_field_info_t;

/**
 *  \brief this struct represnts all the fields in a single ARR format.
 */
typedef struct
{
    char name[ARR_MAX_STRING_LENGTH];
    int nof_fields;

    /** all ARR legal fields for this format */
    int arr_nof_fields;
    arr_field_info_t arr_field[ARR_MAX_NUMBER_OF_FIELDS_IN_FORMATS];

    /** ArrIndex is the parsed entry type */
    int arr_index;

} arr_format_info_t;

/**
 *  \brief this struct represnts all ARR information ( all set of types relevant to a specific ARR)
 */
typedef struct
{
    char name[ARR_MAX_STRING_LENGTH];
    int size;
    int max_nof_fields;
    int nof_instances;
    int nof_formats;
    arr_format_info_t arr_format[ARR_MAX_NUMBER_OF_FORMATS];
} arr_set_info_t;

/**
 *  \brief this struct represnts all the ARR information (parsed from the input file)
 */
typedef struct
{
    int is_initilized;

    int nof_ingress_arr_sets;
    arr_set_info_t ingress_arr[ARR_MAX_NUMBER_OF_ARR_SETS];

    int nof_egress_arr_sets;
    arr_set_info_t egress_arr[ARR_MAX_NUMBER_OF_ARR_SETS];
} root_arr_info_t;
/*
 * }
 * Types
 */

/*
 * Globals
 * {
 */
extern root_arr_info_t arr_info;
/*
 * }
 * Globals
 */

/*
 * Internal APIs
 * {
 */

/**
 * \brief
 *   decodes an EES/ETPS entry according to ARR xmls file arr_logical_formats.xml.
 *   If the specific ETPS is given in 'desc' than the fields that appear in that ETPS are marked with a star (*).
 * \param [in] unit - Unit ID
 * \param [in] core - Core ID
 * \param [in] desc - Specific ETPS names. Use null for no specific ETPS. Use comma separated list for multiple entries.
 *                      Use "TYPE_ONLY" to get only the type of the entry (if not NULL)
 * \param [in] data - EES/ETPS payload
 * \param [in] data_size_bits - size of the payload in bits
 * \param [out] parsed_info - Results will be appended to this list. If points to NULL, the list will be allocated.
 *                              If the type is NULL, nothing will be changed.
 * \param [in] from_n - from which stage the callback has been called for
 * \param [in] to_n - to which stage the callback has been called for
 * inside and should be released at the end.
 * \return shr_error_e Std. error handling
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_etps_arr_decode(
    int unit,
    int core,
    char *desc,
    uint32 *data,
    uint32 data_size_bits,
    rhlist_t ** parsed_info,
    char from_n[RHNAME_MAX_SIZE],
    char to_n[RHNAME_MAX_SIZE]);

/**
 * \brief
 *   Gets the data type from the given data buffer
 * \param [in] unit - Unit ID
 * \param [in] data - Data buffer. User's responsibility to be allocated with at lease 'size' bits
 * \param [in] size - Data buffer size in bits
 * \param [out] data_type - Result type code
 * \return shr_error_e Std. error hendling
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_etps_arr_decode_data_type_get(
    int unit,
    uint32 *data,
    uint32 size,
    uint32 *data_type);

/*
 * DNX_ARR_H_INCLUDED
 */
#endif
