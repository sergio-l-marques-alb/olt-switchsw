/**
 * \file diag_sand_packet_utils.c
 *
 * Implementation of diag command 'dnx packet'.
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_DIAGDNX

/*
 * INCLUDES
 * {
 */
/*
 * sal
 */
#include <sal/appl/sal.h>
/*
 * shared
 */
#include <shared/shrextend/shrextend_debug.h>
#include <shared/bsl.h>
#include <shared/dbx/dbx_file.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
/*
 * bcm
 */
#include <bcm/pkt.h>
#include <bcm/error.h>
#include <bcm/tx.h>
#include <bcm/rx.h>
/*
 * bcm_int
 */
#ifdef BCM_DNX_SUPPORT
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#endif
#ifdef BCM_PETRA_SUPPORT
#include <soc/dpp/port_sw_db.h>
#endif
/*
 * appl
 */
#include <appl/diag/shell.h>
#include <appl/diag/sand/diag_sand_packet.h>
#include <appl/diag/sand/diag_sand_prt.h>
/*
 * soc
 */
#include <soc/drv.h>
/*
 * }
 */

#if defined(BCM_DNX_SUPPORT) && !defined(ADAPTER_SERVER_MODE)
/** Size of module header on DNX devices, it should be written at the start of the packet */
#define DNX_MODULE_HEADER_SIZE         16
#else
/** No Module Header on ADAPTER and non DNX*/
#define DNX_MODULE_HEADER_SIZE         0
#endif

/*
 * GLOBAL DATA
 * {
 */

char *example_data_string =
    "0x0000000000010000000000028100000108004500002e0000000040ff79d20000000000000000000102030405060708090a0b0c0d0e0f1011";

static rhlist_t *rx_pkt_list = NULL;

static bcm_rx_t
dnx_sand_packet_receive(
    int unit,
    bcm_pkt_t * pkt,
    void *cookie)
{
    int i;
    pparse_packet_info_t *packet_info;
    rhhandle_t temp = NULL;
    int up32_size;

    LOG_CLI(("Packet with size:%d received with dest GPORT:0x%08x\n", pkt->tot_len, pkt->dst_gport));

    up32_size = UTILEX_DIV_ROUND_UP(pkt->pkt_len, 4);

    if (utilex_rhlist_entry_add_tail(rx_pkt_list, "ETH", RHID_TO_BE_GENERATED, &temp) != _SHR_E_NONE)
    {
        LOG_CLI(("Failed to add packet info to the list\n"));
        goto exit;
    }
    packet_info = (pparse_packet_info_t *) temp;
    packet_info->data_desc = "ETH";
    packet_info->data_size = up32_size * 32;
    packet_info->gport = pkt->dst_gport;
    {
        uint32 *src = (uint32 *) pkt->_pkt_data.data;
        for (i = 0; i < up32_size; i++)
        {
            packet_info->data[up32_size - (i + 1)] = src[i];
        }
    }

exit:
    return 3;
}

int
diag_sand_rx_nof(
    int unit,
    int clean)
{
    int pkt_num;
    /*
     * Wait a little for other threads to process
     */
    sal_usleep(10000);

    if (rx_pkt_list == NULL)
    {
        pkt_num = 0;
    }
    else
    {
        pkt_num = RHLNUM(rx_pkt_list);
        if (clean == TRUE)
        {
            utilex_rhlist_clean(rx_pkt_list);
        }
    }

    return pkt_num;
}

shr_error_e
diag_sand_rx_start(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * On stop list will be destroyed - so no leftovers
     */
    if (rx_pkt_list == NULL)
    {
        if ((rx_pkt_list = utilex_rhlist_create("rx_packets", sizeof(pparse_packet_info_t), 0)) == NULL)
        {
            SHR_CLI_EXIT(_SHR_E_MEMORY, "Failed to create signal list\n");
        }
    }

    SHR_IF_ERR_EXIT(bcm_rx_register(unit, "test", dnx_sand_packet_receive, 1, (void *) rx_pkt_list, BCM_RCO_F_ALL_COS));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
diag_sand_rx_stop(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    bcm_rx_unregister(unit, dnx_sand_packet_receive, 1);

    if (rx_pkt_list != NULL)
    {
        utilex_rhlist_free_all(rx_pkt_list);
        rx_pkt_list = NULL;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
diag_sand_rx_dump(
    int unit,
    sh_sand_control_t * sand_control)
{
    pparse_packet_info_t *pkt_entry;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Wait a little for other threads to process
     */
    sal_usleep(10000);

    RHITERATOR(pkt_entry, rx_pkt_list)
    {
        parsing_db_t pdb;
        rhlist_t *pit = NULL;
        /*
         * Initialize parsing database
         */
        SHR_IF_ERR_EXIT(pparse_init(unit, &pdb));

        SHR_IF_ERR_EXIT(pparse_packet_process(unit, 0, &pdb, pkt_entry, &pit));

        SHR_IF_ERR_EXIT(packet_decode_info_print(unit, pit, sand_control));
        pparse_parsed_info_tree_free(unit, pit);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
diag_sand_packet_parse_data(
    int unit,
    char *packet_data,
    uint8 **target_p,
    int *length_p)
{
    uint8 *p;
    char tmp, data_iter;
    int i, pkt_len, data_base;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * If string data starts with 0x or 0X, skip it
     */
    if ((packet_data[0] == '0') && ((packet_data[1] == 'x') || (packet_data[1] == 'X')))
    {
        data_base = 2;
    }
    else
    {
        data_base = 0;
    }

    pkt_len = strlen(packet_data) - data_base;
    if (pkt_len < 64)
    {
        pkt_len = 64;
    }

    if ((p = soc_cm_salloc(unit, pkt_len, "tx_string_packet")) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_MEMORY, "Failed to allocate memory for packet\n");
    }

    sal_memset(p, 0, pkt_len);

    /*
     * Convert char to value
     */
    i = 0;
    while (i < pkt_len)
    {
        data_iter = packet_data[data_base + i];
        if (('0' <= data_iter) && (data_iter <= '9'))
        {
            tmp = data_iter - '0';
        }
        else if (('a' <= data_iter) && (data_iter <= 'f'))
        {
            tmp = data_iter - 'a' + 10;
        }
        else if (('A' <= data_iter) && (data_iter <= 'F'))
        {
            tmp = data_iter - 'A' + 10;
        }
        else if (0 == data_iter)
        {
            /*
             * pad by zeroes in case of input string is not enough for ETH packet
             */
            tmp = 0;
        }
        else
        {
            SHR_CLI_EXIT(_SHR_E_MEMORY, "Unexpected char: %c\n", data_iter);
        }

        /*
         * String input is in 4b unit. Below we're filling in 8b:
         * offset is /2, and we shift by 4b if the input char is odd
         */
        p[i / 2] |= tmp << (((i + 1) % 2) * 4);
        ++i;
    }

    *length_p = pkt_len;
    *target_p = p;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
diag_sand_packet_tx(
    int unit,
    uint32 src_port,
    char *data_string,
    int resume_flag)
{
    uint8 *pkt_data = NULL;
    int data_size = 0;
    bcm_pkt_t pkt_info;
    int vlan_present;
    enet_hdr_t *ep;
    int cpu_channel = 0;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&pkt_info, 0, sizeof(pkt_info));

    SHR_IF_ERR_EXIT(diag_sand_packet_parse_data(unit, data_string, &pkt_data, &data_size));

    if ((pkt_info._pkt_data.data = (uint8 *) soc_cm_salloc(unit, data_size + DNX_MODULE_HEADER_SIZE, "tx")) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_MEMORY, "packet memory allocation failed\n");
    }
    else
    {
        pkt_info.pkt_data = &pkt_info._pkt_data;
        pkt_info.blk_count = 1;
        pkt_info._pkt_data.len = data_size + DNX_MODULE_HEADER_SIZE;
    }

    if (resume_flag == FALSE)
    {
        pkt_info.flags |= BCM_TX_NO_VISIBILITY_RESUME;
    }

    sal_memset(pkt_info.pkt_data[0].data, 0, pkt_info.pkt_data[0].len);
    /*
     * Data was read from file or string.  Copy into pkt_info
     */
    sal_memcpy(pkt_info.pkt_data[0].data + DNX_MODULE_HEADER_SIZE, pkt_data, data_size);

    ep = (enet_hdr_t *) BCM_PKT_IEEE(&pkt_info);

    if (!ENET_TAGGED(ep))
    {
        vlan_present = FALSE;
    }
    else
    {
        vlan_present = TRUE;
    }

    /*
     * Setup the packet
     */
    pkt_info.flags |= BCM_TX_CRC_APPEND;
    if (data_size < (vlan_present != 0 ? TAGGED_PACKET_LENGTH : UNTAGGED_PACKET_LENGTH))
    {
        pkt_info.flags |= BCM_TX_NO_PAD;
    }

    pkt_info.flags |= BCM_PKT_F_HGHDR;
#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_channel_get(unit, src_port, &cpu_channel));
    }
#endif
#ifdef BCM_PETRA_SUPPORT
    if (SOC_IS_JERICHO(unit))
    {
        int soc_sand_rv = soc_port_sw_db_channel_get(unit, src_port, (uint32 *) &cpu_channel);
        if (BCM_FAILURE(handle_sand_result(soc_sand_rv)))
        {
            return (CMD_FAIL);
        }
    }
#endif
    pkt_info._dpp_hdr[0] = cpu_channel;

    if (bcm_tx(unit, &pkt_info, NULL) != BCM_E_NONE)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "bcm_tx failed\n");
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
diag_sand_packet_create(
    int unit,
    rhhandle_t * packet_handle_p)
{
    rhlist_t *pkt_list;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(packet_handle_p, _SHR_E_PARAM, "packet_handle_p");

    if ((pkt_list = utilex_rhlist_create("packet", sizeof(signal_output_t), 0)) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_MEMORY, "Could not allocate memory for headers list\n");
    }

    *packet_handle_p = (rhhandle_t) pkt_list;
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
diag_sand_packet_proto_add(
    int unit,
    rhhandle_t packet_handle,
    char *header_name)
{
    rhhandle_t header_handle;
    rhlist_t *pkt_list = (rhlist_t *) packet_handle;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(utilex_rhlist_entry_add_tail(pkt_list, header_name, RHID_TO_BE_GENERATED, &header_handle));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
diag_sand_packet_field_add_str(
    int unit,
    rhhandle_t packet_handle,
    char *field_name,
    char *str_value)
{
    rhlist_t *pkt_list = (rhlist_t *) packet_handle;
    signal_output_t *header_h;
    rhhandle_t field_h;
    int str_length;
    uint32 realtokens = 0;
    char **tokens = NULL;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * We need to verify that field relates to the last header
     */
    if ((tokens = utilex_str_split(field_name, ".", 2, &realtokens)) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_MEMORY, "Inconsistent input field\"%s\"\n", field_name);
    }
    /*
     * Check that there are indeed 2 tokens
     */
    if (realtokens != 2)
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "%s does not match HeaderName.FieldName paradigm\n", field_name);
    }
    /*
     * Check for header in packet - starting from tail
     */
    if ((header_h = (signal_output_t *) utilex_rhlist_entry_get_last_by_name(pkt_list, tokens[0])) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "There is no %s header in packet\n", tokens[0]);
    }

    if (((str_length = sal_strlen(str_value)) >= DSIG_MAX_SIZE_STR - 1))
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "Input value string is too long:%d\n", str_length);
    }

    /*
     * Create field list if not exists already
     */
    if (header_h->field_list == NULL)
    {
        if ((header_h->field_list = utilex_rhlist_create("fields", sizeof(signal_output_t), 1)) == NULL)
        {
            SHR_CLI_EXIT(_SHR_E_MEMORY, "Could not allocate memory for field list of header '%s'.\n", RHNAME(header_h));
        }
    }
    /*
     * First check if such field was already added, positive answer means that request is to replace the value
     * On negative we must add the field to the list
     */
    if ((field_h = utilex_rhlist_entry_get_by_name(header_h->field_list, field_name)) == NULL)
    {
        SHR_IF_ERR_EXIT(utilex_rhlist_entry_add_tail(header_h->field_list, field_name, RHID_TO_BE_GENERATED, &field_h));
    }

    sal_strncpy(((signal_output_t *) field_h)->print_value, str_value, str_length);

exit:
    utilex_str_split_free(tokens, realtokens);
    SHR_FUNC_EXIT;
}

shr_error_e
diag_sand_packet_field_add_uint32(
    int unit,
    rhhandle_t packet_handle,
    char *field_name,
    uint32 *uint32_value,
    int bit_size)
{
    rhlist_t *pkt_list = (rhlist_t *) packet_handle;
    signal_output_t *header_h;
    rhhandle_t field_h;
    int uint32_size;
    uint32 realtokens = 0;
    char **tokens = NULL;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * We need to verify that field relates to the last header
     */
    if ((tokens = utilex_str_split(field_name, ".", 2, &realtokens)) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_MEMORY, "Inconsistent input field\"%s\"\n", field_name);
    }
    /*
     * Check that there are indeed 2 tokens
     */
    if (realtokens != 2)
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "%s does not match HeaderName.FieldName paradigm\n", field_name);
    }
    /*
     * Check for header in packet - starting from tail
     */
    if ((header_h = (signal_output_t *) utilex_rhlist_entry_get_last_by_name(pkt_list, tokens[0])) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "There is no %s header in packet\n", tokens[0]);
    }

    if ((uint32_size = BITS2WORDS(bit_size)) > DSIG_MAX_SIZE_UINT32)
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "Input value size is too long:%d\n", uint32_size);
    }

    /*
     * Create field list if not exists already
     */
    if (header_h->field_list == NULL)
    {
        if ((header_h->field_list = utilex_rhlist_create("fields", sizeof(signal_output_t), 1)) == NULL)
        {
            SHR_CLI_EXIT(_SHR_E_MEMORY, "Could not allocate memory for field list of header '%s'.\n", RHNAME(header_h));
        }
    }
    /*
     * First check if such field was already added, positive answer means that request is to replace the value
     * On negative we must add the field to the list
     */
    if ((field_h = utilex_rhlist_entry_get_by_name(header_h->field_list, field_name)) == NULL)
    {
        SHR_IF_ERR_EXIT(utilex_rhlist_entry_add_tail(header_h->field_list, field_name, RHID_TO_BE_GENERATED, &field_h));
    }

    memcpy(((signal_output_t *) field_h)->value, uint32_value, uint32_size * 4);

exit:
    utilex_str_split_free(tokens, realtokens);
    SHR_FUNC_EXIT;
}

void
diag_sand_packet_free(
    int unit,
    rhhandle_t packet_handle)
{
    signal_output_t *header;
    rhlist_t *pkt_list = (rhlist_t *) packet_handle;
    SHR_FUNC_INIT_VARS(unit);

    if (pkt_list != NULL)
    {
        RHITERATOR(header, pkt_list)
        {
            if (header->field_list)
            {
                utilex_rhlist_free_all(header->field_list);
                header->field_list = NULL;
            }
        }
        utilex_rhlist_free_all(pkt_list);
    }

    SHR_VOID_FUNC_EXIT;
}

shr_error_e
diag_sand_packet_send(
    int unit,
    uint32 src_port,
    rhhandle_t packet_handle,
    int resume_flag)
{
    char data_string[DSIG_MAX_SIZE_STR];
    uint32 data[DSIG_MAX_SIZE_UINT32];
    rhlist_t *pkt_list = (rhlist_t *) packet_handle;
    uint32 size = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(pparse_compose(unit, pkt_list, DSIG_MAX_SIZE_BITS, data, &size));

    sand_signal_value_to_str(unit, SAL_FIELD_TYPE_NONE, data, data_string, size, PRINT_BIG_ENDIAN);

    SHR_IF_ERR_EXIT(diag_sand_packet_tx(unit, src_port, data_string, resume_flag));

exit:
    SHR_FUNC_EXIT;

}

shr_error_e
diag_sand_packet_last_show(
    int unit,
    int core)
{
    signal_output_t *packet_output;
    rhlist_t *packet_list;
    match_t match;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    memset(&match, 0, sizeof(match_t));
    match.block = "IRPP";
#if defined(ADAPTER_SERVER_MODE)
    match.from = "llr";
#else
    match.from = "nif";
#endif
    match.name = "packet_header";
    match.flags = SIGNALS_MATCH_EXPAND | SIGNALS_MATCH_ONCE | SIGNALS_MATCH_EXACT;

    if ((packet_list = utilex_rhlist_create("prt_print", sizeof(signal_output_t), 0)) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_MEMORY, "Failed to create signal list\n");
    }

    sand_signal_list_get(unit, core, &match, packet_list);

    packet_output = utilex_rhlist_entry_get_first(packet_list);

    packet_decode_info_print(unit, packet_output->field_list, NULL);
    sand_signal_list_free(packet_list);

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

#if !defined(NO_FILEIO)

static shr_error_e
diag_sand_packet_find_by_name(
    int unit,
    char *packet_n,
    xml_node * curPacket)
{
    xml_node curTop = NULL, curSubTop = NULL, curLine;
    char filename[RHSTRING_MAX_SIZE];

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(curPacket, _SHR_E_PARAM, "curPacket");

    if (ISEMPTY(packet_n))
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "Empty packet name\n");
    }
    if (((curTop = dbx_file_get_xml_top(unit, "DNX-Devices.xml", "top", 0)) == NULL) ||
        ((curSubTop = dbx_xml_child_get_first(curTop, "case-objects")) == NULL))
    {
        /*
         * No parsing objects - just leave
         */
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }

    RHDATA_ITERATOR(curLine, curSubTop, "include")
    {
        char type[RHNAME_MAX_SIZE];

        RHDATA_GET_STR_DEF_NULL(curLine, "type", type);
        if (!sal_strcasecmp(type, "tests"))
        {
            xml_node curFile, curSection, cur;
            int device_specific = 0;
            int flags = 0;

            RHDATA_GET_INT_DEF(curLine, "device_specific", device_specific, 0);
            if (device_specific == TRUE)
            {
                flags = CONF_OPEN_PER_DEVICE;
            }

            RHDATA_GET_STR_CONT(curLine, "file", filename);
            if ((curFile = dbx_file_get_xml_top(unit, filename, "top", flags)) == NULL)
                continue;

            /*
             * Go through packets and found the requested one
             */
            if ((curSection = dbx_xml_child_get_first(curFile, "packets")) != NULL)
            {
                RHDATA_ITERATOR(cur, curSection, "packet")
                {
                    char name[RHNAME_MAX_SIZE];
                    RHDATA_GET_STR_DEF_NULL(cur, "name", name);
                    if (!sal_strcasecmp(packet_n, name))
                    {
                        *curPacket = cur;
                        SHR_EXIT();
                    }
                }
            }
            dbx_xml_top_close(curFile);
        }
    }

exit:
    dbx_xml_top_close(curTop);
    SHR_FUNC_EXIT;
}

shr_error_e
diag_sand_packet_send_by_case(
    int unit,
    char *packet_n,
    rhhandle_t * packet_h_p)
{
    xml_node curProto, curPacket = NULL;
    rhhandle_t packet_handle = NULL;
    int src_port;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(diag_sand_packet_find_by_name(unit, packet_n, &curPacket));

    if (curPacket == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "Packet case:'%s' was not found\n", packet_n);
    }
    SHR_IF_ERR_EXIT(diag_sand_packet_create(unit, &packet_handle));

    RHDATA_ITERATOR(curProto, curPacket, "proto")
    {
        char proto_name[RHNAME_MAX_SIZE];
        xml_node curField;
        RHDATA_GET_STR_STOP(curProto, "name", proto_name);
        /*
         * Header argument
         */
        SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_handle, proto_name));
        RHDATA_ITERATOR(curField, curProto, "field")
        {
            char field_name[RHNAME_MAX_SIZE], field_value[RHNAME_MAX_SIZE], full_name[RHNAME_MAX_SIZE];
            RHDATA_GET_STR_STOP(curField, "name", field_name);
            RHDATA_GET_STR_STOP(curField, "value", field_value);
            sal_snprintf(full_name, RHNAME_MAX_SIZE - 1, "%s.%s", proto_name, field_name);
            SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_handle, full_name, field_value));
        }
    }
    RHCHDATA_GET_INT_DEF(curPacket, "source", "port", src_port, 201);

    SHR_IF_ERR_EXIT(diag_sand_packet_send(unit, (uint32) src_port, packet_handle, TRUE));

exit:
    /*
     * If pointer to handle provided - let the caller deal with it
     * If packet_handle == NULL - both case have the same effect
     */
    if (packet_h_p != NULL)
    {
        *packet_h_p = packet_handle;
    }
    else
    {
        diag_sand_packet_free(unit, packet_handle);
    }
    SHR_FUNC_EXIT;
}
#endif /* !defined(NO_FILEIO) */
