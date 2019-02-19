/**
 * \file diag_sand_packet.h
 *
 * Header for diag command 'dnx packet'.
 */
/*
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef DIAG_SAND_PACKET_INCLUDED
#define DIAG_SAND_PACKET_INCLUDED

/*
 * Dependencies (includes)
 * {
 */
#include <soc/sand/sand_pparse.h>
#include <appl/diag/shell.h>
#include <appl/diag/sand/diag_sand_framework.h>

/*
 * }
 * Dependencies (includes)
 */

#define TAGGED_PACKET_LENGTH           68
#define UNTAGGED_PACKET_LENGTH         64

shr_error_e diag_sand_packet_tx(
    int unit,
    char *data_string);

/**
 * \brief
 *    Allocate new packet object to be used by other diag_sand_packet* APIs
 * \param [in] unit - Unit #
 * \param [in,out] packet_handle_p - pointer to packet control handle allocated
 *
 * \retval _SHR_E_NONE for success
 * \retval _SHR_E_MEMORY problem with allocating new object
 * \retval shr_error_e Standard error handling
 * \remark
 *   Each invocation will create new packet, do not forget to free it using diag_sand_packet_free
 * \see
 *   diag_sand_packet_free
 */
shr_error_e diag_sand_packet_create(
    int unit,
    rhhandle_t * packet_handle_p);

/**
 * \brief
 *    Add proto to header stack of packet, order of adding will be reflected in packet structure, handle to added header an output
 * \param [in] unit - Unit #
 * \param [in] packet_handle - packet control handle
 * \param [in] proto - name of protocol to be used when adding header, see NetworkStructures.xml for the list of supported ones.
 * \param [in,out] header_handle_p - pointer to header handle that will be allocated to this call
 *
 * \retval _SHR_E_NONE for success
 * \retval _SHR_E_PARAM problem with protocol
 * \retval shr_error_e Standard error handling
 * \remark
 *   use "sig struct pkt" to list all supported protocols
 * \see
 *   NetworkStructures.xml
 */
shr_error_e diag_sand_packet_proto_add(
    int unit,
    rhhandle_t packet_handle,
    char *proto,
    rhhandle_t * header_handle_p);

/**
 * \brief
 *    Add field that need to be different from default value(provided by NetworkStructures.xml). Input value is provided by string
 * \param [in] unit - Unit #
 * \param [in] header_handle - handle to the header, the field is added to, obtained from diag_sand_packet_proto_add
 * \param [in] field_name - name of protocol field, including protocol name itself. E.g. VLAN.VID, ETH.DA.
 * 							Field name may be complex one and actually be subfield of a field. e.g. IPv6.sip.low
 * \param [in] str_value -  pointer to the string representing value of field specified by field_name,
 * 						    string size should match field definition and in anyway not bigger than DSIG_MAX_SIZE_STR
 * \retval _SHR_E_NONE for success
 * \retval _SHR_E_PARAM problem with field name
 * \retval shr_error_e Standard error handling
 * \remark
 *   use "sig struct pkt" to list all supported protocols
 *   Add second time same field to the same header will replace previous value, useful for testing different values of specific qualifiers
 * \see
 *   NetworkStructures.xml
 */
shr_error_e diag_sand_packet_field_add_str(
    int unit,
    rhhandle_t header_handle,
    char *field_name,
    char *str_value);

/**
 * \brief
 *    Add field that need to be different from default value(provided by NetworkStructures.xml). Input value is provided by uint32 array
 * \param [in] unit - Unit #
 * \param [in] header_handle - handle to the header, the field is added to, obtained from diag_sand_packet_proto_add
 * \param [in] field_name - name of protocol field, including protocol name itself. E.g. VLAN.VID, ETH.DA.
 * 							Field name may be complex one and actually be subfield of a field. e.g. IPv6.sip.low
 * \param [in] uint32_value - pointer to uint32 array representing value of field specified by field_name, size should match field definition
 * \param [in] bit_size - size(in bits) should match field definition
 * \retval _SHR_E_NONE for success
 * \retval _SHR_E_PARAM problem with field name
 * \retval shr_error_e Standard error handling
 * \remark
 *   use "sig struct pkt" to list all supported protocols
 *   Add second time same field to the same header will replace previous value, useful for testing different values of specific qualifiers
 * \see
 *   NetworkStructures.xml
 */
shr_error_e diag_sand_packet_field_add_uint32(
    int unit,
    rhhandle_t header_handle,
    char *field_name,
    uint32 *uint32_value,
    int bit_size);

/**
 * \brief
 *    Frees packet control structure and all underlying allocated objects
 * \param [in] unit - Unit #
 * \param [in] packet_handle - packet control handle
 *
 * \retval _SHR_E_NONE for success
 * \retval shr_error_e Standard error handling
 * \remark
 *   If the handle is NULL - no error will be issued and no log will be recorder
 * \see
 *   diag_sand_packet_create
 */
void diag_sand_packet_free(
    int unit,
    rhhandle_t packet_handle);

/**
 * \brief
 *    Send the packet. For cmodel there is no need to define the port, it is just presented to the beginning of th pipe
 * \param [in] unit - Unit #
 * \param [in] packet_handle - packet control handle
 *
 * \retval _SHR_E_NONE for success
 * \retval shr_error_e Standard error handling
 * \remark
 *   Packet control structure may be used multiple times, until diag_sand_packet_free is called,
 *   useful to have one field modification where others stay constant
 * \see
 */
shr_error_e diag_sand_packet_send(
    int unit,
    rhhandle_t packet_handle);

/**
 * \brief
 *   Prints the content of last packet sent on specific unti/core
 * \param [in] unit - Unit #
 * \param [in] core - Core #
 * \return shr_error_e Standard error handling
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e diag_sand_packet_last_show(
    int unit,
    int core);

/**
 * \brief
 *   Prints the information tree after parsing
 * \param [in] unit - Unit #
 * \param [in] pit - Parsed information tree holding all the data to be printed
 * \param [in] sand_control - Control structure for DNX shell framework
 * \return shr_error_e Standard error handling
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e packet_decode_info_print(
    int unit,
    rhlist_t * pit,
    sh_sand_control_t * sand_control);

/*
 * }
 * PParse defines and macros
 */
/*
 * Definitions for legacy command list in DPP only
 * {
 */
cmd_result_t cmd_sand_packet(
    int unit,
    args_t * args);

extern const char cmd_sand_packet_usage[];
/*
 * }
 */

/*
 * Definitions for shell framework
 */
extern sh_sand_man_t sh_sand_packet_man;
extern sh_sand_cmd_t sh_sand_packet_cmds[];

extern char *example_data_string;

#endif /* DIAG_SAND_PACKET_INCLUDED */
