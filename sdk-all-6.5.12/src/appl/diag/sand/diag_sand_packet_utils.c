/**
 * \file diag_sand_packet.c
 *
 * Implementation of diag command 'dnx packet'.
 */
/*
 * $Copyright: (c) 2017 Broadcom.
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
 * shared
 */
#include <shared/shrextend/shrextend_debug.h>
#include <shared/bsl.h>
/*
 * bcm
 */
#include <bcm/pkt.h>
#include <bcm/error.h>
#include <bcm/tx.h>
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
/*
 * GLOBAL DATA
 * {
 */

char *example_data_string =
    "0x0000000000010000000000028100000108004500002e0000000040ff79d20000000000000000000102030405060708090a0b0c0d0e0f1011";

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
        SHR_ERR_EXIT(_SHR_E_MEMORY, "Failed to allocate memory for packet\n");
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
            SHR_ERR_EXIT(_SHR_E_MEMORY, "Unexpected char: %c\n", data_iter);
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
    char *data_string)
{
    uint8 *pkt_data = NULL;
    int data_size = 0;
    bcm_pkt_t pkt_info;
    int vlan_present;
    enet_hdr_t *ep;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&pkt_info, 0, sizeof(pkt_info));

    SHR_IF_ERR_EXIT(diag_sand_packet_parse_data(unit, data_string, &pkt_data, &data_size));

    if ((pkt_info._pkt_data.data = (uint8 *) soc_cm_salloc(unit, data_size, "tx")) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "packet memory allocation failed\n");
    }
    else
    {
        pkt_info.pkt_data = &pkt_info._pkt_data;
        pkt_info.blk_count = 1;
        pkt_info._pkt_data.len = data_size;
    }

    sal_memset(pkt_info.pkt_data[0].data, 0, pkt_info.pkt_data[0].len);
    /*
     * Data was read from file or string.  Copy into pkt_info
     */
    sal_memcpy(pkt_info.pkt_data[0].data, pkt_data, data_size);

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

    if (bcm_tx(unit, &pkt_info, NULL) != BCM_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "bcm_tx failed\n");
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
        SHR_ERR_EXIT(_SHR_E_MEMORY, "Could not allocate memory for headers list\n");
    }

    *packet_handle_p = (rhhandle_t) pkt_list;
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
diag_sand_packet_proto_add(
    int unit,
    rhhandle_t packet_handle,
    char *proto,
    rhhandle_t * header_handle_p)
{
    rhhandle_t header_handle;
    rhlist_t *pkt_list = (rhlist_t *) packet_handle;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(header_handle_p, _SHR_E_PARAM, "proto_handle_p");

    SHR_IF_ERR_EXIT(utilex_rhlist_entry_add_tail(pkt_list, proto, RHID_TO_BE_GENERATED, &header_handle));
    *header_handle_p = header_handle;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
diag_sand_packet_field_add_str(
    int unit,
    rhhandle_t header_handle,
    char *field_name,
    char *str_value)
{
    signal_output_t *header = (signal_output_t *) header_handle;
    signal_output_t *sig;
    rhhandle_t rhhandle;
    int str_length;

    SHR_FUNC_INIT_VARS(unit);

    if (((str_length = sal_strlen(str_value)) >= DSIG_MAX_SIZE_STR - 1))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Input value string is too long:%d\n", str_length);
    }

    /*
     * Create field list if not exists already
     */
    if (header->field_list == NULL)
    {
        if ((header->field_list = utilex_rhlist_create("fields", sizeof(signal_output_t), 1)) == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_MEMORY, "Could not allocate memory for field list of header '%s'.\n", RHNAME(header));
        }
    }
    /*
     * First check if such field was already added, positive answer means that request is to replace the value
     * On negative we must add the field to the list
     */
    if ((rhhandle = utilex_rhlist_entry_get_by_name(header->field_list, field_name)) == NULL)
    {
        SHR_IF_ERR_EXIT(utilex_rhlist_entry_add_tail(header->field_list, field_name, RHID_TO_BE_GENERATED, &rhhandle));
    }

    sig = rhhandle;
    sal_strncpy(sig->print_value, str_value, str_length);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
diag_sand_packet_field_add_uint32(
    int unit,
    rhhandle_t header_handle,
    char *field_name,
    uint32 *uint32_value,
    int bit_size)
{
    signal_output_t *header = (signal_output_t *) header_handle;
    signal_output_t *sig;
    rhhandle_t rhhandle;
    int uint32_size;

    SHR_FUNC_INIT_VARS(unit);

    if ((uint32_size = BITS2WORDS(bit_size)) > DSIG_MAX_SIZE_UINT32)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Input value size is too long:%d\n", uint32_size);
    }

    /*
     * Create field list if not exists already
     */
    if (header->field_list == NULL)
    {
        if ((header->field_list = utilex_rhlist_create("fields", sizeof(signal_output_t), 1)) == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_MEMORY, "Could not allocate memory for field list of header '%s'.\n", RHNAME(header));
        }
    }
    /*
     * First check if such field was already added, positive answer means that request is to replace the value
     * On negative we must add the field to the list
     */
    if ((rhhandle = utilex_rhlist_entry_get_by_name(header->field_list, field_name)) == NULL)
    {
        SHR_IF_ERR_EXIT(utilex_rhlist_entry_add_tail(header->field_list, field_name, RHID_TO_BE_GENERATED, &rhhandle));
    }

    sig = rhhandle;
    memcpy(sig->value, uint32_value, uint32_size * 4);

exit:
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
    rhhandle_t packet_handle)
{
    char data_string[DSIG_MAX_SIZE_STR];
    uint32 data[DSIG_MAX_SIZE_UINT32];
    rhlist_t *pkt_list = (rhlist_t *) packet_handle;
    uint32 size = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(pparse_compose(unit, pkt_list, DSIG_MAX_SIZE_BITS, data, &size));

    sand_signal_value_to_str(unit, NULL, SAL_FIELD_TYPE_NONE, NULL, 0, data, data_string, size, PRINT_BIG_ENDIAN);

    SHR_IF_ERR_EXIT(diag_sand_packet_tx(unit, data_string));

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
    device_t *device;
    match_t match;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    memset(&match, 0, sizeof(match_t));
    match.block = "IRPP";
    match.from = "llr";
    match.name = "packet_header";
    match.flags = SIGNALS_MATCH_EXPAND | SIGNALS_MATCH_ONCE | SIGNALS_MATCH_EXACT;

    if ((device = sand_signal_device_get(unit)) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_NOT_FOUND, "");
    }

    if ((packet_list = utilex_rhlist_create("prt_print", sizeof(signal_output_t), 0)) == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_MEMORY, "Failed to create signal list\n");
    }

    sand_signal_list_get(device, unit, core, &match, packet_list);

    packet_output = utilex_rhlist_entry_get_first(packet_list);

    packet_decode_info_print(unit, packet_output->field_list, NULL);
    sand_signal_list_free(packet_list);

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}
