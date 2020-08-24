/**
 * \file sand_pparse.h
 *
 * Header for dynamic packet parsing engine.
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef SOC_SAND_PPARSE_INCLUDED
#define SOC_SAND_PPARSE_INCLUDED

#include <shared/utilex/utilex_str.h>
#include <shared/utilex/utilex_rhlist.h>

#include <soc/sand/sand_signals.h>

#include <bcm/types.h>

/**
 * \brief Maximum number of 32bit words in a parsed packet data
 */
#define PPARSE_MAX_DATA_WORDS           128
/**
 * \brief Maximal number of extensions to a single object
 */
#define PPARSE_MAX_EXTENSIONS_NOF       20
/**
 * \brief Maximal number of conditions that can indicate whether an object exist, what's his size, ...
 */
#define PPARSE_MAX_CONDITIONS_NOF       4
/**
 * \brief Structure that holds the information used for parsing a buffer
 */
typedef struct
{
    rhentry_t entry;
    /*
     * Raw packet data 
     */
    uint32 data[PPARSE_MAX_DATA_WORDS];
    /*
     * data size in bits 
     */
    uint32 data_size;
    /*
     * first object on the packet 
     */
    char *data_desc;
    /*
     * gport associated with packet - used to obtain port properties
     */
    bcm_gport_t gport;
    /*
     * Protocols and their headers after parsing
     */
    rhlist_t *pit;
} pparse_packet_info_t;

typedef enum
{
    PPARSE_CONDITION_TYPE_FIELD,
    PPARSE_CONDITION_TYPE_SOC,
    PPARSE_CONDITION_TYPE_PLUGIN,

    /*
     * Must be last
     */
    PPARSE_CONDITION_TYPE_NOF
} pparse_cond_type_e;

typedef enum
{
    PPARSE_CONDITION_MODE_ENABLE,
    PPARSE_CONDITION_MODE_DISABLE,
    /*
     * Must be last
     */
    PPARSE_CONDITION_MODE_NOF
} pparse_cond_mode_e;

typedef struct
{
    uint8 enabled;              /* pre calculated / read */
    uint32 size;                /* pre calculated / read */
} pparse_cond_soc_content_t;

typedef struct
{
    char path[RHSTRING_MAX_SIZE];
    uint32 enable_value;
    uint32 enable_mask;
} pparse_cond_field_content_t;

typedef shr_error_e(
    *pparse_condition_check_plugin_cb_t) (
    int unit,
    int core,
    rhlist_t * args,
    uint32 *result);

typedef struct
{
    pparse_condition_check_plugin_cb_t callback;
    rhlist_t *args;
} pparse_cond_plugin_content_t;

typedef union
{
    pparse_cond_soc_content_t soc;
    pparse_cond_field_content_t field;
    pparse_cond_plugin_content_t plugin;
} pparse_cond_content_u;

typedef struct
{
    pparse_cond_type_e type;
    pparse_cond_mode_e mode;
    pparse_cond_content_u content;
} pparse_condition_t;

typedef struct
{
    char object_name[RHNAME_MAX_SIZE];
    pparse_condition_t conditions[PPARSE_MAX_CONDITIONS_NOF];
    int conditions_nof;
} pparse_object_extension_t;

typedef struct
{
    rhentry_t entry;
    uint32 field_value;
} next_header_option_t;

typedef struct
{
    rhentry_t entry;
    rhlist_t *option_lst;       /* next_header_option_t list */
} next_headers_t;

typedef struct
{
    rhlist_t *packet; /* list if signal_output_t for each header */
    rhlist_t *post_processors; /* List of callbacks for post-processing */
    uint32 total_packet_size; /* Size of entire packet in bytes. Should update Length fields */
} packet_compose_info_t;

/**
 * \brief
 * Upon composing, some headers require post processing for default values
 * for certain fields (like checksum, length, ...)
 */
typedef shr_error_e (*pparse_post_processor_cb_t)(
        int unit,
        packet_compose_info_t *compose_info,
        signal_output_t *header);

typedef struct
{
    rhentry_t entry;
    pparse_post_processor_cb_t post_processing_cb;
    signal_output_t *header;
} post_processing_entry_t;

typedef struct
{
    rhlist_t *obj_lst;          /* pparse_db_entry_t list */
    rhlist_t *next_headers_lst; /* next_headers_t list */
} parsing_db_t;

typedef struct pparse_db_entry_s pparse_db_entry_t;

/**
 * \brief
 *  Callback type that parses 'data' according to the information in 'pdb_entry'.
 *  The result is added to 'pit', which is a signal_output_t list. The actual number
 *  of bit from 'data' that were used, is returned in 'parsed_size'.
 *  of the
 */
typedef shr_error_e (*pparse_object_parser_cb_t)(
    int unit,
    pparse_db_entry_t * pdb_entry,
    uint32 * data,
    rhlist_t * pit,
    uint32 * parsed_size);

struct pparse_db_entry_s
{
    rhentry_t entry;
    int size;
    char struct_name[RHNAME_MAX_SIZE];
    char stage_from_name[RHNAME_MAX_SIZE];
    char block_name[RHNAME_MAX_SIZE];
    char desc_str[RHSTRING_MAX_SIZE];
    pparse_object_extension_t extensions[PPARSE_MAX_EXTENSIONS_NOF];
    int extensions_nof;
    next_headers_t *next_header_options;
    char next_header_field[RHSTRING_MAX_SIZE];
    pparse_object_parser_cb_t parsing_cb;
    pparse_post_processor_cb_t post_processing_cb;
} ;

typedef struct
{
    rhentry_t rhentry;
    pparse_db_entry_t *pdb_entry;
} pparse_parsing_object_t;

typedef struct
{
    /*
     * RHList entry
     */
    rhentry_t entry;
    /*
     * value string
     */
    char value[RHNAME_MAX_SIZE];
} packet_plugin_arg_t;

/**
 * \brief
 *   Reads and prepare the parsing database
 * \param [in] unit - Unit #
 * \param [out] pdb - Newly allocated parsing database (should be freed after use)
 * \return shr_error_e Standard Error
 */
shr_error_e pparse_init(
    int unit,
    parsing_db_t * pdb);

/**
 * \brief
 *   Free up resources of the parsing database
 * \param [in] pdb - parsing database to free
 */
void pparse_destroy(
    parsing_db_t * pdb);

/**
 * \brief
 *   Parses a packet depicted by packet_info and populates the result in the parsed info tree pit.
 * \param [in] unit - Unit #
 * \param [in] core - Core num
 * \param [in] pdb - Initialized parsing data base
 * \param [in] packet_info - packet information for parsing
 * \param [out] pit - resulting parsed information tree
 * \return shr_error_e Standard error handeling
 */
shr_error_e pparse_packet_process(
    int unit,
    int core,
    parsing_db_t * parsing_db,
    pparse_packet_info_t * packet_info,
    rhlist_t ** pit);

/**
 * \brief
 *   Decodes a single object (and its extensions). The name of the object is given in desc. the parsing is given back in
 *   parsed_info as a list of signal_output_t. If parsed_info points to NULL (*parsed_info==NULL), then a new list of
 *   signal_output_t is allocated. The user is responsible to release this list. If the list is given allocated, the
 *   parsed information is added at the tail.
 * \param [in] unit - Unit #
 * \param [in] core - Core ID
 * \param [in] desc - Name of the object to decode
 * \param [in] data - Data of the object to decode
 * \param [in] data_size_bits - Size of the data in bits
 * \param [out] parsed_info - Returned parsed data as a list of signal_output_t.
 * \param [in] from_n - from which stage the callback has been called for
 * \param [in] to_n - to which stage the callback has been called for
 * \return shr_error_e Standard error handeling
 */
shr_error_e pparse_decode(
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
 *   Parse 'packet_info' according to the information in 'parse_list' (which gets more items during parsing) and
 *   'pdb' which supplies the static data for the parsing procedures.
 */
shr_error_e pparse_list_process(
    int unit,
    int core,
    parsing_db_t * pdb,
    pparse_packet_info_t * packet_info,
    rhlist_t * parse_list,
    rhlist_t * pit);

/**
 * \brief
 *   Adds identifiers to siblings with the same name, in the parsed tree.
 *   For example: in an MPLS stack, each MPLS header should get a unique name for reference.
 * \param [in] unit - Unit #
 * \param [in,out] list - item list. Can be NULL if start_item is not.
 * \param [in,out] start_item - The first item to start enumerating from. Can be NULL, if it is, the 1st item in
 * 'list' will be used.
 */
shr_error_e pparse_parsed_list_duplicates_enumerate(
    int unit,
    rhlist_t * list,
    signal_output_t * start_item);

/**
 * \brief
 *   Composes a packet from a list of objects and field values.
 * \param [in] unit -
 * \param [in] packet_objects - list of objects with field values (entry-type:signal_output_t, initialized)
 * \param [in] packet_size_bytes - total size of the packet for use in length fields, etc.
 * \param [in] max_data_size_bits - size of the output buffer
 * \param [out] data -
 *      output buffer. Will contain the raw packet, data[0] LSBit is the packet's LSBit (innermost header LSB).
 * \param [in_out] data_size_bits - actual #bits written to the buffer. Can be set to non-zero value to prevent writing
 *                      to the buffer's LSB (for example, to leave room for payload).
 * \return shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e pparse_compose(
    int unit,
    rhlist_t * packet_objects,
    uint32 packet_size_bytes,
    uint32 max_data_size_bits,
    uint32 *data,
    uint32 *data_size_bits);

/**
 * \brief
 *   pparse_combine gets a list of signal names, retreive the structure for each signal and combine it into a single
 *   signal_output_t structure with flat list of fields (no sub-fields).
 *   It assumes that the requested signals are final and do not require extra information in order to resolve their
 *   structure (their xml source does not contain <option> child-nodes).
 * \param [in] unit - Unit #
 * \param [in] sig_list - list of signal_output_t with names of the requested signals/headers to combine in
 *                        RHNAME(entry) and possibly a prefix for identification of multiple headers/signals with the
 *                        same name in entry->expansion.
 * \param [out] combined_sig - Result combined, flattened, signal. The size will increase according to the added headers
 *                               and the field_list will have all the fields of the added headers in flat structure.
 * \return shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e pparse_combine(
    int unit,
    rhlist_t * sig_list,
    signal_output_t * combined_sig,
    uint8 is_network);

/*
 * Utils decleration
 */

/**
 * \brief
 *   Allocate and initialize the parsing information tree
 * \param [in] unit - Unit #
 * \param [in] pit - Parsing information tree pointer
 * \return shr_error_e Standard error handeling
 */
shr_error_e pparse_parsed_info_tree_create(
    int unit,
    rhlist_t ** pit);

/**
 * \brief
 *   Release resources used by the parsed info tree
 * \param [in] unit - Unit #
 * \param [in] pit - Parsed info tree
 */
void pparse_parsed_info_tree_free(
    int unit,
    rhlist_t * pit);

shr_error_e pparse_list_create(
    int unit,
    rhlist_t ** plist);

void pparse_list_free(
    int unit,
    rhlist_t * plist);

shr_error_e pparse_list_add(
    int unit,
    char *object_name,
    rhlist_t * plist,
    pparse_parsing_object_t ** new_item_p);

shr_error_e pparse_list_push(
    int unit,
    char *object_name,
    rhlist_t * plist);

pparse_parsing_object_t *pparse_list_get_next(
    int unit,
    rhlist_t * plist);

shr_error_e pparse_list_pop(
    int unit,
    rhlist_t * plist);

shr_error_e pparse_pdb_init(
    int unit,
    char *filename,
    uint8 device_specific,
    parsing_db_t * pdb);

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*
 *                    PLUGINS
 */
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/

/*
 * Extension plugins
 * {
 */
#ifdef BCM_DNX_SUPPORT
/**
 * \brief
 *   Checks the existence of PTCH on the incoming pocket, according to the available signals.
 *   DNX ONLY!
 * \param [in] unit
 *   Unit ID
 * \param [in] core
 *   Core ID
 * \param [in] args
 *   list of arguments to the plugin. Unused.
 * \param [in] pit
 *   Current parsed info tree. Ignored.
 * \param [out] enabled
 *   Is the incoming pocket containing PTCH
 */
shr_error_e packet_ptch(
    int unit,
    int core,
    rhlist_t * args,
    uint32 *enabled);
/**
 * \brief
 *   Checks the existence of RCH on the incoming pocket, according to the available signals.
 *   DNX ONLY!
 * \param [in] unit
 *   Unit ID
 * \param [in] core
 *   Core ID
 * \param [in] args
 *   list of arguments to the plugin. Unused
 * \param [in] pit
 *   Current parsed info tree. Ignored.
 * \param [out] enabled
 *   Is the incoming pocket containing RCH
 */
shr_error_e packet_rch(
    int unit,
    int core,
    rhlist_t * args,
    uint32 *enabled);
#endif /** BCM_DNX_SUPPORT */
/*
 * }
 */

/*
 * Parsing plugins
 * {
 */
/**
 * \brief
 *  Calls signal parser with the relevant signal details from the pdb entry.
 */
shr_error_e
pparse_sig_parse_wrapper_cb(
    int unit,
    pparse_db_entry_t * pdb_entry,
    uint32 * data,
    rhlist_t * pit,
    uint32 * parsed_size);

/**
 * \brief
 *   Parse ETH header according to the device's TPID configuration
 */
shr_error_e
pparse_eth_parser_cb(
    int unit,
    pparse_db_entry_t * pdb_entry,
    uint32 * data,
    rhlist_t * pit,
    uint32 * parsed_size);

/**
 * \brief
 *   Parse IPv4 header according to IHL field
 */
shr_error_e
pparse_ipv4_parser_cb(
    int unit,
    pparse_db_entry_t * pdb_entry,
    uint32 * data,
    rhlist_t * pit,
    uint32 * parsed_size);

/**
 * \brief
 *  Post processor for IPv4 header to update field in packet-compose
 */
shr_error_e
pparse_ipv4_post_processor(
    int unit,
    packet_compose_info_t *compose_info,
    signal_output_t *header);

/**
 * \brief
 *  Parse next header after MPLS. Assumes the last entry in pit is the MPLS BOS label.
 */
shr_error_e
pparse_2nd_parser_cb(
    int unit,
    pparse_db_entry_t * pdb_entry,
    uint32 * data,
    rhlist_t * pit,
    uint32 * parsed_size);

/**
 * \brief
 *  Parse SRv6 header
 */
shr_error_e
pparse_srv6_parser_cb(
    int unit,
    pparse_db_entry_t * pdb_entry,
    uint32 * data,
    rhlist_t * pit,
    uint32 * parsed_size);

/**
 * \brief
 *  Parse IPv6 Generic Extension header
 */
shr_error_e
pparse_ipv6_ext_parser_cb(
    int unit,
    pparse_db_entry_t * pdb_entry,
    uint32 * data,
    rhlist_t * pit,
    uint32 * parsed_size);

/*
 * }
 * Parsing plugins
 */

/*
 * SOC_SAND_PPARSE_INCLUDED
 */
#endif
