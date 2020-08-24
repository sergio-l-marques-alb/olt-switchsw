/* $Id: ui_rom_defi.h,v 1.5 Broadcom SDK $
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
*/
#ifndef UI_ROM_DEFI_INCLUDED
/* { */
#define UI_ROM_DEFI_INCLUDED
/*
 * General include file for reference design.
 */
#if !DUNE_BCM
#if !(defined(LINUX) || defined(UNIX))
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


/* } */
#endif
