/* $Id: $
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef __JER2_JER_PP_EM_SER_INCLUDED__
#define __JER2_JER_PP_EM_SER_INCLUDED__

#if !defined(BCM_DNX_SUPPORT) && !defined(BCM_DNXF_SUPPORT)
#error "This file is for use by DNX (Jer2) and DNXF family only!" 
#endif

/*************
 * INCLUDES  *
 *************/
#ifdef BCM_DNX_SUPPORT

#include <soc/dnx/legacy/SAND/Management/sand_general_macros.h>
#include <soc/dnx/legacy/JER2_ARAD/JER2_ARAD_PP/jer2_arad_pp_framework.h>

/*************
 * TYPE DEFS *
 *************/

typedef enum
{
    JER2_JER_PP_EM_SER_BLOCK_IHB,
    JER2_JER_PP_EM_SER_BLOCK_PPDB_A,
    JER2_JER_PP_EM_SER_BLOCK_PPDB_B,
    JER2_JER_PP_EM_SER_BLOCK_EDB,
    JER2_JER_PP_EM_SER_BLOCK_OAMP,
    JER2_JER_PP_EM_SER_NOF_BLOCKS
}JER2_JER_PP_EM_SER_BLOCK;

typedef enum
{
    JER2_JER_PP_EM_SER_TYPE_LEM_KEYT_PLDT,
    JER2_JER_PP_EM_SER_TYPE_ISEM_KEYT_PLDT,
    JER2_JER_PP_EM_SER_TYPE_GLEM_KEYT_PLDT,
    JER2_JER_PP_EM_SER_TYPE_ESEM_KEYT_PLDT,
    JER2_JER_PP_EM_SER_TYPE_OEMA_KEYT_PLDT,
    JER2_JER_PP_EM_SER_TYPE_OEMB_KEYT_PLDT,
    JER2_JER_PP_EM_SER_TYPE_RMAPEM_KEYT_PLDT,
    JER2_JER_PP_EM_SER_TYPE_LAST
}JER2_JER_PP_EM_SER_TYPE;

/*************
 * FUNCTIONS *
 *************/

int dnxc_pp_em_ser(int unit, soc_mem_t mem, unsigned array_index, int copyno, int index);

#endif

#endif /* __JER2_JER_PP_EM_SER_INCLUDED__ */



