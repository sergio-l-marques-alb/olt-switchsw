/*********************************************************************
*
* (C) Copyright PT Inovação S.A. 2013-2013
*
**********************************************************************
*
* @create    21/10/2013
*
* @author    Daniel Filipe Figueira
* @author    Marcio Daniel Melo
*
**********************************************************************/
#ifndef _PTIN_MGMD_DEFS_MAIN_H
#define _PTIN_MGMD_DEFS_MAIN_H

#include "l7_platformspecs.h"


/* OLT10 */
#if (PTIN_BOARD == PTIN_BOARD_OLT1T0)
#include "ptin_mgmd_defs_olt1t0.h"

/* OLT10F */
#elif (PTIN_BOARD == PTIN_BOARD_OLT1T0F)
#include "ptin_mgmd_defs_olt1t0f.h"

/* OLT7-8CH */
#elif (PTIN_BOARD == PTIN_BOARD_OLT7_8CH_B)
#include "ptin_mgmd_defs_olt7-8ch-b.h"

/* TOLT8G */
#elif (PTIN_BOARD == PTIN_BOARD_TOLT8G)
#include "ptin_mgmd_defs_tolt8g.h"

/* TG16G */
#elif (PTIN_BOARD == PTIN_BOARD_TG16G)
#include "ptin_mgmd_defs_tg16g.h"

/* TG16GF */
#elif (PTIN_BOARD == PTIN_BOARD_TG16GF)
#include "ptin_mgmd_defs_tg16gf.h"

/* TG16GF */
#elif (PTIN_BOARD == PTIN_BOARD_TT04SXG)
#include "ptin_mgmd_defs_tt04sxg.h"

/* TG4G */
#elif (PTIN_BOARD == PTIN_BOARD_TG4G)
#include "ptin_mgmd_defs_tg4g.h"

/* TA48GE */
#elif (PTIN_BOARD == PTIN_BOARD_TA48GE)
#include "ptin_mgmd_defs_ta48ge.h"

/* CXO160G */
#elif (PTIN_BOARD == PTIN_BOARD_CXO160G)
#include "ptin_mgmd_defs_cxo160g.h"

/* OLT360 Matrix card */
#elif (PTIN_BOARD == PTIN_BOARD_CXP360G)
#include "ptin_mgmd_defs_cxo360g.h"

/* OLT1T3 Matrix card */
#elif (PTIN_BOARD == PTIN_BOARD_CXO640G)
#include "ptin_mgmd_defs_cxo640g.h"

/* TA12XG card */
#elif (PTIN_BOARD == PTIN_BOARD_TA12XG)
#include "ptin_mgmd_defs_ta12xg.h"

#endif

#endif /* _PTIN_MGMD_DEFS_MAIN_H */