/** \file dnx/pemladrv/pemladrv.h
 * 
 * PEMLA related functions that should be exposed
 * 
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */
#ifndef PEMLADRV_INCLUDED
/*
 * { 
 */
#define PEMLADRV_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

int dnx_pemladrv_init(
    int unit,
    int restore_after_reset,
    const char *file_name);

    
int dnx_pemladrv_deinit(
    int unit);
/*
 * } 
 */
#endif /* PEMLADRV_INCLUDED */

