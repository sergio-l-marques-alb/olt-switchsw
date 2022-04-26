
/** \file ctest_dnx_field_utils.c
 * $Id$
 *
 * DNX level C test Utility functions stored in this file.
 *
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDTESTSDNX

#define NUM_STAGES_WITH_CONTEXT                 4

 /*
  * Include files.
  * {
  */

#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/field/field.h>
#include <bcm_int/dnx/field/field_group.h>
#include <bcm_int/dnx/field/field_context.h>
#include <bcm_int/dnx/field/field_presel.h>
#include <bcm_int/dnx/field/field_map.h>
#include <bcm_int/dnx/algo/field/algo_field.h>
#include <appl/diag/sand/diag_sand_packet.h>
#include "ctest_dnx_field_utils.h"
#include <bcm_int/dnx/field/field_actions.h>

/**
 * See ctest_dnx_field_utils.h
 */
shr_error_e
ctest_dnx_field_utils_packet_tx(
    int unit,
    bcm_port_t src_port,
    dnx_field_utils_packet_info_t * packet_p)
{
    rhhandle_t packet_handle;
    int protocol;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&packet_handle, 0, sizeof(packet_handle));

    SHR_NULL_CHECK(packet_p, _SHR_E_PARAM, "packet_p");

    /*
     * Create packet handler, which will contain packet info, like protocols and relevant fields to them.
     */
    SHR_IF_ERR_EXIT(diag_sand_packet_create(unit, &packet_handle));

    /*
     * Construct the packet, which will be transmitted
     */
    for (protocol = 0;
         (protocol < DNX_FIELD_UTILS_NOF_PROTOCOLS_IN_PACKET)
         && (sal_strncmp(packet_p->header_info[protocol], "", sizeof(packet_p->header_info[protocol]))); protocol++)
    {
        if ((sal_strnlen(packet_p->header_info[protocol], sizeof(packet_p->header_info[protocol]))) >
            DNX_FIELD_UTILS_STR_SIZE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "The given string (%s) is bigger than max allowed (%d) !!!",
                         packet_p->header_info[protocol], DNX_FIELD_UTILS_STR_SIZE);
        }

        /*
         * Enter here if the given element contains no ".".
         * In that case, assume it represents a protocol.
         */
        if (sal_strnstr(packet_p->header_info[protocol], ".", sizeof(packet_p->header_info[protocol])) == NULL)
        {
            /*
             * Add protocol to the packet
             */
            SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_handle, packet_p->header_info[protocol]));
        }
        else
        {
            if (!(sal_strncmp(packet_p->header_info[protocol + 1], "", sizeof(packet_p->header_info[protocol + 1]))))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Value for field (%s) is not exposed!!!", packet_p->header_info[protocol]);
            }
            /*
             * Fill the Protocol Fields, if any.
             * The expected format of a field is <protocol>.<field>.
             * For example:
             *      ETH.DA, ETH.SA, ETH.Type
             *      VLAN.VID, VLAN.PCP, VLAN.DEI
             *      IPv4.SIP, IPv4.DIP, IPv4.Version
             */
            SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_handle, packet_p->header_info[protocol],
                                                           packet_p->header_info[protocol + 1]));
            protocol++;
        }
    }

    /*
     * Sending the constructed packet
     */
    SHR_IF_ERR_EXIT(diag_sand_packet_send(unit, src_port, packet_handle, SAND_PACKET_RESUME));

exit:
    diag_sand_packet_free(unit, packet_handle);
    SHR_FUNC_EXIT;
}
