/*
 * $Id: dbal)internal.h,v 1.13 Broadcom SDK $
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */

#ifndef DIAG_DNX_IKLEAP_LAYERS_TRANSLATION_TO_PPARSE_DEFINITION_H_INCLUDED
#  define DIAG_DNX_IKLEAP_LAYERS_TRANSLATION_TO_PPARSE_DEFINITION_H_INCLUDED

/** \brief Number of layers in lookups packet */
#define KLEAP_NOF_RELATIVE_LAYERS                   4

typedef struct
{
    int nof_signals;
    char signal_mame[5][64];
    char signal_prefix[5][64];

} kleap_layer_to_pparse_t;

#endif
