/*
 * $Id: config_init_defaults.c,v 1.2 Broadcom SDK $
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File:        config_init_defaults.c
 * Purpose:     Declaration for compiled-in config variables, to be 
 *              used by Diag Shell if config.bcm file is not specified
 *              or not available.
 *
 *              Typically contains a sequence of multiple invocations
 *              of sal_config_set($attr,$val) for various configuration
 *              properties defined in $SDK/include/soc/property.h
 */

void
sal_config_init_defaults(void)
{
}
