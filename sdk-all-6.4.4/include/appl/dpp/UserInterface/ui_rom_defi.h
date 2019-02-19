/* $Id: ui_rom_defi.h,v 1.5 Broadcom SDK $
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
#ifndef UI_ROM_DEFI_INCLUDED
/* { */
#define UI_ROM_DEFI_INCLUDED
/*
 * General include file for reference design.
 */
#if !DUNE_BCM
#ifdef __VXWORKS__
/* { */
  #include <ChipSim/chip_sim.h>
  #include <appl/diag/dpp/ref_sys.h>
  #include <vxWorks.h>
  #include <drv/mem/eeprom.h>
  #include <appl/diag/dpp/ui_defx.h>
  #include <appl/diag/dpp/utils_defx.h>
/* } */
#elif !(defined(LINUX) || defined(UNIX))
/* { */
  #include <bcm_app/dpp/../H/drv/mem/eeprom.h>
/* } */
#endif
#else
  #include <appl/diag/dpp/ref_sys.h>
  #include <appl/diag/dpp/ui_defx.h>
  #include <appl/diag/dpp/utils_defx.h>
#endif

/********************************************************
********************************************************/
extern CONST
   SUBJECT
     Subjects_list_rom[];

#ifdef __VXWORKS__

extern const
  MEMORY_BLOCK
    Memory_block_rom_00[];

extern const
  MEMORY_BLOCK
    Memory_block_rom_01[];

/*
 * This is the new PCI mezzanine block.
 */
extern const
  MEMORY_BLOCK
    Memory_block_rom_02[];

/*
 * This is the new PCI mezzanine block on GFA line-card.
 */

extern const
  MEMORY_BLOCK
    Memory_block_rom_03[];

/*
 * GFA-MB line-card.
 */

extern const
  MEMORY_BLOCK
    Memory_block_rom_gfa_mb[];

/*
 * GFA-SOC_SAND_FAP21V line-card.
 */

extern const
  MEMORY_BLOCK
    Memory_block_rom_gfa_fap21v[];

extern const
  MEMORY_BLOCK
    Memory_block_rom_gfa_petra[];

extern const
  MEMORY_BLOCK
    Memory_block_rom_gfa_petra_streaming[];

/* 
 *  GFA-BI line-card
 */
extern const
  MEMORY_BLOCK
    Memory_block_rom_gfa_bi[];

/*
 * This is front-end TEVB in standalone mode (no GFA connected)
 */
extern const
  MEMORY_BLOCK
    Memory_block_rom_04[];

  /* } __VXWORKS__ */
#endif


/* } */
#endif
