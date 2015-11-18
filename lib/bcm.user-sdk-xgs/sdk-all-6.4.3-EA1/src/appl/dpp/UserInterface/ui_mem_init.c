
/* $Id: ui_mem_init.c,v 1.5 Broadcom SDK $
 * $Copyright: Copyright 2012 Broadcom Corporation.
 * This program is the proprietary software of Broadcom Corporation
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software
 * and all intellectual property rights therein.  IF YOU HAVE
 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of
 * Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
*/
/*
 * Basic_include_file.
 */

/*
 * General include file for reference design.
 */
#include <appl/diag/dpp/ref_sys.h>
/*
 * User interface external include file.
 */
#include <appl/diag/dpp/ui_defx.h>
/*
 * Dune chips include file.
 */
#include <appl/dpp/UserInterface/ui_pure_defi.h>
#include <appl/diag/dpp/dune_chips.h>

/*
 * Allocate memory for rom variables.
 * {
 */
#ifdef EXTERN
#undef EXTERN
#endif
#define EXTERN

#ifdef INIT
#undef INIT
#endif
#define INIT

#ifdef __VXWORKS__
/* { */
#define CONST const
/* } */
#else
/* { */
/* !DUNE_BCM does not need undef */
#undef  CONST
#define CONST
/* } */
#endif
#include <appl/dpp/UserInterface/ui_rom_defi.h>
/*
 * }
 */

/*
 * Allocate memory for ram variables.
 * {
 */
#undef  INIT
#define INIT
#undef  EXTERN
#define EXTERN

#if !DUNE_BCM
#if LINK_FAP20V_LIBRARIES
#include <appl/dpp/UserInterface/ui_rom_defi_fap20v.h>
#include <appl/dpp/UserInterface/ui_rom_defi_fap20v_b.h>
#endif

#if LINK_FE600_LIBRARIES
#include <appl/dpp/UserInterface/ui_rom_defi_fe600_bsp.h>
#include <appl/dpp/UserInterface/ui_rom_defi_fe600_api.h>
#endif


/* { */
#if !(defined(LINUX) || defined(UNIX))
/* { */
  #include <appl/dpp/UserInterface/ui_rom_defi_line_gfa.h>
  #include <appl/dpp/UserInterface/ui_rom_defi_line_tgs.h>
  #include <appl/dpp/UserInterface/ui_rom_defi_line_tevb.h>
/* } */
#endif
#endif /* DUNE_BCM */


#include <appl/dpp/UserInterface/ui_ram_defi.h>

#include "ui_rom_defi.cx"

/*
 * }
 */
