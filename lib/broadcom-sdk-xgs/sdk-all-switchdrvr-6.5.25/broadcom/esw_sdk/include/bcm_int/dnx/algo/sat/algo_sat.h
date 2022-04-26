/**
 * \file algo_sat.h
 *
 *  Internal DNX SAT Managment APIs
 *
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *  All Rights Reserved.
 *
 */

#ifndef ALGO_SAT_H_INCLUDED
/*
 * {
 */
#define ALGO_SAT_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/**
 * }
 */

/*
 * Defines
 * {
 */
#define DNX_ALGO_RES_TYPE_SAT_GTF_ID                 "SAT_RES_ALLOC_GTF_ID"
#define DNX_ALGO_RES_TYPE_SAT_CTF_ID                 "SAT_RES_ALLOC_CTF_ID"
#define DNX_ALGO_RES_TYPE_SAT_TRAP_ID                "SAT_RES_ALLOC_TRAP_ID"
#define DNX_ALGO_RES_TYPE_SAT_TRAP_DATA              "SAT_RES_ALLOC_TRAP_DATA"
#define DNX_ALGO_RES_TYPE_SAT_PKT_HEADER_MEM         "DNX_ALGO_RES_TYPE_SAT_PKT_HEADER_MEM"

/**
 * }
 */

/*
 * MACROs
 * {
 */

/**
 * }
 */

shr_error_e dnx_algo_sat_init(
    int unit);

shr_error_e dnx_algo_sat_deinit(
    int unit);

/*
 * }
 */
#endif /* ALGO_SAT_H_INCLUDED */
