/** \file dnxc_adapter_regmem_access.h
 * This file handles the adapter new access "regmem" functionality
 */
/*
 * $Id:adapter_reg_access.h,v 1.312 Broadcom SDK $                                                           $
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$ $
 */

#ifndef DNXC_ADAPTER_REGMEM_ACCESS_H_INCLUDED
/* { */
#define DNXC_ADAPTER_REGMEM_ACCESS_H_INCLUDED

#ifdef ADAPTER_SERVER_MODE
#ifdef BCM_ACCESS_SUPPORT
/*
 * Include files
 * {
 */

#include <shared/shrextend/shrextend_debug.h>
#include <soc/access/auto_generated/common_enum.h>
/*
 * }
 */

/**
 * \brief - handle new regmem access
 *
 */
shr_error_e dnxc_adapter_regmem_access_handle(
    const int unit,
    uint32 flags,
    access_regmem_id_t regmem,
    access_block_instance_num_t inst,
    uint16 array_index,
    uint32 mem_index,
    uint32 *entry_data);

#endif /** BCM_ACCESS_SUPPORT */
#endif /** ADAPTER_SERVER_MODE */
/**DNXC_ADAPTER_REGMEM_ACCESS_H_INCLUDED*/
/* } */
#endif
