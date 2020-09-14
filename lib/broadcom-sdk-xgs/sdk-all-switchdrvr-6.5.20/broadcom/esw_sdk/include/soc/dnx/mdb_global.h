/*
 * $Id$ Contains the minimal MDB defines shared by both DNXDATA and SWSTATE
 */
/*
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifndef MDB_GLOBAL_H_INCLUDED

#define MDB_GLOBAL_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

typedef enum
{

    MDB_MACRO_A = 0,

    MDB_MACRO_B = 1,

    MDB_EEDB_BANK = 2,

    MDB_NOF_MACRO_TYPES = 3
} mdb_macro_types_e;

#endif
