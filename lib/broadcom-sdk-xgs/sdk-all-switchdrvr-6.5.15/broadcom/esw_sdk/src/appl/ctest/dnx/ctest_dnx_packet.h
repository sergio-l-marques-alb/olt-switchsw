/*
 * ! \file ctest_dnx_packet.h Contains ctest packet utilities and ctest command
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifndef CTEST_DNX_PACKET_H_INCLUDED
#define CTEST_DNX_PACKET_H_INCLUDED

extern sh_sand_option_t dnx_packet_test_options[];
extern sh_sand_man_t dnx_packet_test_man;

shr_error_e dnx_packet_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

#endif /* CTEST_DNX_PACKET_H_INCLUDED */
