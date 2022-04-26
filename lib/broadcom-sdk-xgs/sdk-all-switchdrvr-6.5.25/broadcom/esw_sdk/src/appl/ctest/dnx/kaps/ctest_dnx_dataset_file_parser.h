/*
 * ! \file ctest_dnx_dataset_file_parser.h contains declarations for parsing prefixes from dataset file
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifndef CTEST_DNX_DATASET_FILE_PARSER_H_INCLUDED
#define CTEST_DNX_DATASET_FILE_PARSER_H_INCLUDED

#include "kaps_key.h"

struct ctest_kaps_parse_key
{
    char *fname;                        /**< String name for the key. */
    uint32 width;                       /**< Width of key in bits. */
    uint32 offset;
    uint32 overlay_field;
    enum kaps_key_field_type type;      /**< Type of key field. */
    struct ctest_kaps_parse_key *next;  /**< Linked list of key fields. */
    char buf[];                         /**<< Memory for string name. */
};

struct ctest_kaps_parse_record
{
    uint8 *data;                      /**< Data bits. */
    uint8 *mask;                      /**< 1 == don't care. */

    uint32 length;                  /**< Prefix length, for LPM only. */

};

/**
 * \brief
 *  Parses the prefixes from the dataset file
 *  \param [in] unit - The unit number.
 *  \param [in] db_type - Type of database (use KAPS_DB_LPM)
 *  \param [in] fname - Name of the file along with path
 *  \param [out] result - The parsed prefix entries will be returned here
 *  \param [in] req_num_entries - Number of entries to be parsed from the dataset file
 *  \param [out] num_lines - Number of lines in the dataset file
 *  \param [out] num_entries - Number of entries parsed and stored in the result
 *  \param [out] key - The fields present in the prefix as indicated in dataset (VRF, SIP, DIP, etc)
 *  \return
 *   Error indication according to shr_error_e enum
 *  \remark
 *   None.
 *  \see
 *   None.
 */
shr_error_e ctest_dnx_parse_standard_dataset(
    int unit,
    enum kaps_db_type db_type,
    const char *fname,
    struct ctest_kaps_parse_record **result,
    uint32 req_num_entries,
    uint32 *num_lines,
    uint32 *num_entries,
    struct ctest_kaps_parse_key **key);

#endif
