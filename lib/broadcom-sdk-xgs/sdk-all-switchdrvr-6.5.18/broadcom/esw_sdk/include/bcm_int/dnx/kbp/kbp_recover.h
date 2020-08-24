/** \file kbp_recover.h
 *
 * Functions and defines for handling jericho2 kbp recover sequence.
 *
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#if defined(INCLUDE_KBP)

#ifndef _KBP_RECOVER_INCLUDED__
/*
 * {
 */

#define _KBP_RECOVER_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/*
 * {
 */

/*
 * }
 */

/*************
 * DEFINES   *
 *************/
/*
 * {
 */

#define DNX_KBP_CLAUSE45_ADDR(devad, regad)     \
            ((((devad) & 0x3F) << 16) |          \
             ((regad) & 0xFFFF))

#define DNX_KBP_TX_PCS_EN_REG    (0xF00F)

#define DNX_KBP_TX_PHASE_MATCHING_FIFO_CTRL_QUAD0_REG    (0xF160)
#define DNX_KBP_TX_PHASE_MATCHING_FIFO_CTRL_QUAD1_REG    (0xF161)
#define DNX_KBP_TX_PHASE_MATCHING_FIFO_CTRL_QUAD2_REG    (0xF162)
#define DNX_KBP_TX_PHASE_MATCHING_FIFO_CTRL_QUAD3_REG    (0xF163)
#define DNX_KBP_TX_PHASE_MATCHING_FIFO_CTRL_QUAD4_REG    (0xF164)
#define DNX_KBP_TX_PHASE_MATCHING_FIFO_CTRL_QUAD5_REG    (0xF165)
#define DNX_KBP_TX_PHASE_MATCHING_FIFO_CTRL_QUAD6_REG    (0xF166)
#define DNX_KBP_TX_PHASE_MATCHING_FIFO_CTRL_QUAD7_REG    (0xF167)
#define DNX_KBP_TX_PHASE_MATCHING_FIFO_CTRL_QUAD8_REG    (0xF168)

#define DNX_KBP_TX_PHASE_MATCHING_FIFO_IN_RESET_VAL    (0x3037)
#define DNX_KBP_TX_PHASE_MATCHING_FIFO_OUT_OF_RESET_VAL    (0x3035)

#define DNX_KBP_RECOVER_LINK_UP_LOOP_TIMES    (0x64)
#define DNX_KBP_RECORD_REPLY_VALID_CHECK_TIMES    (5)

/*
 * }
 */
/*************
 * MACROS    *
 *************/
/*
 * {
 */

/*
 * }
 */

/*************
 * ENUMS     *
 *************/
/*
 * {
 */

/*
 * }
 */

/*************
 * GLOBALS   *
 *************/
/*
 * {
 */
/*
 * }
 */
/*************
 * FUNCTIONS *
 *************/
/*
 * {
 */
shr_error_e dnx_kbp_recover_run_recovery_sequence(
    int unit,
    uint32 core,
    uint32 mdio_id);

/*
 * }
 */
#endif /* __KBP_RECOVER_INCLUDED__ */
#endif /* defined(INCLUDE_KBP) */
