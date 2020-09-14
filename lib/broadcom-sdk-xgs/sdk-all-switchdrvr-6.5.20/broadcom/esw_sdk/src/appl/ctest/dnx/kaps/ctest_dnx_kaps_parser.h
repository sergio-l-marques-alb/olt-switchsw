
#ifndef CTEST_DNX_KAPS_PARSER_H_INCLUDED
#define CTEST_DNX_KAPS_PARSER_H_INCLUDED

#include "../libs/kaps/include/kaps_key.h"

struct ctest_kaps_parse_key
{
    char *fname;                        /**< String name for the key. */
    uint32_t width;                     /**< Width of key in bits. */
    uint32_t offset;
    uint32_t overlay_field;
    enum kaps_key_field_type type;       /**< Type of key field. */
    struct ctest_kaps_parse_key *next;         /**< Linked list of key fields. */
    char buf[];                         /**<< Memory for string name. */
};

struct ctest_kaps_parse_record
{
    uint8_t *data;                      /**< Data bits. */
    uint8_t *mask;                      /**< 1 == don't care. */

    uint32_t length;                  /**< Prefix length, for LPM only. */

};

kaps_status ctest_kaps_parse_standard_db_file(
    enum kaps_db_type db_type,
    const char *fname,
    struct ctest_kaps_parse_record **result,
    uint32_t req_num_entries,
    uint32_t * num_lines,
    uint32_t * num_entries,
    struct ctest_kaps_parse_key **key);

#endif
