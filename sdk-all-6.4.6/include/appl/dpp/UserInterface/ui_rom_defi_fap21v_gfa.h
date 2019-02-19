/* $Id: ui_rom_defi_fap21v_gfa.h,v 1.2 Broadcom SDK $
 * $Copyright: Copyright 2015 Broadcom Corporation.
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


#ifndef __UI_ROM_DEFI_FAP21V_GFA_INCLUDED__
/* { */
#define __UI_ROM_DEFI_FAP21V_GFA_INCLUDED__

#ifdef  __cplusplus
extern "C" {
#endif

#ifdef _MSC_VER
  /*
   * On MS-Windows platform this attribute is not defined.
   */
  #define __ATTRIBUTE_PACKED__
  #pragma pack(push)
  #pragma pack(1)

#elif __GNUC__
    /*
     * GNUC packing attribute
     */
     #define __ATTRIBUTE_PACKED__  __attribute__ ((packed))
#else
     #error  "Add your system support for packed attribute."
#endif

#include <appl/dpp/UserInterface/ui_pure_defi_fap21v_gfa.h>

/********************************************************/

EXTERN CONST
   PARAM_VAL_RULES
     Fap21v_gfa_ddr_offset_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
        0,0,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        8,
        (VAL_PROC_PTR)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
EXTERN CONST
   PARAM_VAL_RULES
     Fap21v_gfa_free_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
        0,0,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
        (VAL_PROC_PTR)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
EXTERN CONST
   PARAM_VAL_RULES
     Fap21v_gfa_empty_vals[]
#ifdef INIT
   =
{
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
EXTERN CONST
   PARAM_VAL_RULES
     Fap21v_gfa_iddr_data_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "ddr_x_2",
    {
      {
        2,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        ""
        ""
        "",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "ddr_x_4",
    {
      {
        4,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        ""
        ""
        "",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;

/********************************************************/

EXTERN CONST
   PARAM
     fap21v_gfa_params[]
#ifdef INIT
   =
{
  {
    PARAM_FAP21V_GFA_MB_FGPA_DOWNLOAD_ID,
    "download",
    (PARAM_VAL_RULES *)&Fap21v_gfa_empty_vals[0],
    (sizeof(Fap21v_gfa_empty_vals) / sizeof(Fap21v_gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    ""
    "",
    ""
    ""
    "",
    "",
    {BIT(0)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP21V_GFA_MB_FPGA_ID,
    "fpga",
    (PARAM_VAL_RULES *)&Fap21v_gfa_empty_vals[0],
    (sizeof(Fap21v_gfa_empty_vals) / sizeof(Fap21v_gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    ""
    "",
    ""
    ""
    "",
    "",
    {BIT(0)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP21V_GFA_MB_FLASH_DOWNLOAD_ID,
    "download",
    (PARAM_VAL_RULES *)&Fap21v_gfa_empty_vals[0],
    (sizeof(Fap21v_gfa_empty_vals) / sizeof(Fap21v_gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    ""
    "",
    ""
    ""
    "",
    "",
    {BIT(1)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP21V_GFA_MB_FLASH_ERASE_ID,
    "erase",
    (PARAM_VAL_RULES *)&Fap21v_gfa_empty_vals[0],
    (sizeof(Fap21v_gfa_empty_vals) / sizeof(Fap21v_gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    ""
    "",
    ""
    ""
    "",
    "",
    {BIT(1)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP21V_GFA_MB_FLASH_STATUS_ID,
    "status",
    (PARAM_VAL_RULES *)&Fap21v_gfa_empty_vals[0],
    (sizeof(Fap21v_gfa_empty_vals) / sizeof(Fap21v_gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    ""
    "",
    ""
    ""
    "",
    "",
    {BIT(1)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP21V_GFA_MB_FLASH_BURN_FPGA_ID,
    "burn_fpga",
    (PARAM_VAL_RULES *)&Fap21v_gfa_empty_vals[0],
    (sizeof(Fap21v_gfa_empty_vals) / sizeof(Fap21v_gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    ""
    "",
    ""
    ""
    "",
    "",
    {BIT(1)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP21V_GFA_MB_FLASH_ID,
    "flash",
    (PARAM_VAL_RULES *)&Fap21v_gfa_empty_vals[0],
    (sizeof(Fap21v_gfa_empty_vals) / sizeof(Fap21v_gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    ""
    "",
    ""
    ""
    "",
    "",
    {BIT(1)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP21V_GFA_MB_ID,
    "mb",
    (PARAM_VAL_RULES *)&Fap21v_gfa_empty_vals[0],
    (sizeof(Fap21v_gfa_empty_vals) / sizeof(Fap21v_gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    ""
    "",
    ""
    ""
    "",
    "",
    {BIT(0)|BIT(1)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP21V_GFA_GENERAL_RESET_DOWN_MILISEC_ID,
    "reset_down_milisec",
    (PARAM_VAL_RULES *)&Fap21v_gfa_free_vals[0],
    (sizeof(Fap21v_gfa_free_vals) / sizeof(Fap21v_gfa_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    ""
    "",
    ""
    ""
    "",
    "",
    {BIT(10)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP21V_GFA_GENERAL_RESET_STAY_IN_RESET_ID,
    "stay_in_reset",
    (PARAM_VAL_RULES *)&Fap21v_gfa_empty_vals[0],
    (sizeof(Fap21v_gfa_empty_vals) / sizeof(Fap21v_gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    ""
    "",
    ""
    ""
    "",
    "",
    {BIT(10)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP21V_GFA_GENERAL_RESET_ID,
    "reset",
    (PARAM_VAL_RULES *)&Fap21v_gfa_empty_vals[0],
    (sizeof(Fap21v_gfa_empty_vals) / sizeof(Fap21v_gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    ""
    "",
    ""
    ""
    "",
    "",
    {BIT(10)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP21V_GFA_GENERAL_INQ_TEST_RW_ID,
    "inq_test_rw",
    (PARAM_VAL_RULES *)&Fap21v_gfa_empty_vals[0],
    (sizeof(Fap21v_gfa_empty_vals) / sizeof(Fap21v_gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    ""
    "",
    ""
    ""
    "",
    "",
    {BIT(1)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP21V_GFA_GENERAL_IDDR_TEST_RW_DATA_TO_TEST_ID,
    "data_to_test",
    (PARAM_VAL_RULES *)&Fap21v_gfa_ddr_offset_vals[0],
    (sizeof(Fap21v_gfa_ddr_offset_vals) / sizeof(Fap21v_gfa_ddr_offset_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    ""
    "",
    ""
    ""
    "",
    "",
    {BIT(3)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP21V_GFA_GENERAL_IDDR_TEST_RW_NOF_ITERATION_ID,
    "nof_iteration",
    (PARAM_VAL_RULES *)&Fap21v_gfa_free_vals[0],
    (sizeof(Fap21v_gfa_free_vals) / sizeof(Fap21v_gfa_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    ""
    "",
    ""
    ""
    "",
    "",
    {BIT(3)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP21V_GFA_GENERAL_IDDR_TEST_RW_OFFSET_ID,
    "offset",
    (PARAM_VAL_RULES *)&Fap21v_gfa_free_vals[0],
    (sizeof(Fap21v_gfa_free_vals) / sizeof(Fap21v_gfa_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    ""
    "",
    ""
    ""
    "",
    "",
    {BIT(3)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP21V_GFA_GENERAL_IDDR_TEST_RW_ID,
    "iddr_test_rw",
    (PARAM_VAL_RULES *)&Fap21v_gfa_empty_vals[0],
    (sizeof(Fap21v_gfa_empty_vals) / sizeof(Fap21v_gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    ""
    "",
    ""
    ""
    "",
    "",
    {BIT(3)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP21V_GFA_GENERAL_CPU_TEST_RW_ID,
    "cpu_test_rw",
    (PARAM_VAL_RULES *)&Fap21v_gfa_empty_vals[0],
    (sizeof(Fap21v_gfa_empty_vals) / sizeof(Fap21v_gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    ""
    "",
    ""
    ""
    "",
    "",
    {BIT(4)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP21V_GFA_GENERAL_CALIBRATE_BIST_FULL_ALL_OFFSET_ID,
    "offset_correct",
    (PARAM_VAL_RULES *)&Fap21v_gfa_free_vals[0],
    (sizeof(Fap21v_gfa_free_vals) / sizeof(Fap21v_gfa_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    ""
    "",
    ""
    ""
    "",
    "",
    {BIT(7)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP21V_GFA_GENERAL_CALIBRATE_BIST_ID,
    "bist",
    (PARAM_VAL_RULES *)&Fap21v_gfa_empty_vals[0],
    (sizeof(Fap21v_gfa_empty_vals) / sizeof(Fap21v_gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    ""
    "",
    ""
    ""
    "",
    "",
    {BIT(7)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP21V_GFA_GENERAL_CALIBRATE_LBG_ID,
    "lbg",
    (PARAM_VAL_RULES *)&Fap21v_gfa_empty_vals[0],
    (sizeof(Fap21v_gfa_empty_vals) / sizeof(Fap21v_gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    ""
    "",
    ""
    ""
    "",
    "",
    {BIT(7)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP21V_GFA_GENERAL_CALIBRATE_FULL_ALL_ID,
    "calibration",
    (PARAM_VAL_RULES *)&Fap21v_gfa_empty_vals[0],
    (sizeof(Fap21v_gfa_empty_vals) / sizeof(Fap21v_gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    ""
    "",
    ""
    ""
    "",
    "",
    {BIT(7)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP21V_GFA_GENERAL_CALIBRATE_DELETE_FILE_ID,
    "delete_calibration_file",
    (PARAM_VAL_RULES *)&Fap21v_gfa_empty_vals[0],
    (sizeof(Fap21v_gfa_empty_vals) / sizeof(Fap21v_gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    ""
    "",
    ""
    ""
    "",
    "",
    {BIT(11)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP21V_GFA_GENERAL_ID,
    "general",
    (PARAM_VAL_RULES *)&Fap21v_gfa_empty_vals[0],
    (sizeof(Fap21v_gfa_empty_vals) / sizeof(Fap21v_gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    ""
    "",
    ""
    ""
    "",
    "",
    {BIT(2)|BIT(3)|BIT(4)|BIT(5)|BIT(6)|BIT(7)|BIT(10)|BIT(11)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP21V_GFA_SCREENING_FTG_TEST_ID,
    "ftg_test",
    (PARAM_VAL_RULES *)&Fap21v_gfa_empty_vals[0],
    (sizeof(Fap21v_gfa_empty_vals) / sizeof(Fap21v_gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    ""
    "",
    ""
    ""
    "",
    "",
    {BIT(8)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP21V_GFA_SCREENING_INTERFACES_ID,
    "interfaces",
    (PARAM_VAL_RULES *)&Fap21v_gfa_empty_vals[0],
    (sizeof(Fap21v_gfa_empty_vals) / sizeof(Fap21v_gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    ""
    "",
    ""
    ""
    "",
    "",
    {BIT(8)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP21V_GFA_SCREENING_PRINT_MODE_ID,
    "print_mode",
    (PARAM_VAL_RULES *)&Fap21v_gfa_empty_vals[0],
    (sizeof(Fap21v_gfa_empty_vals) / sizeof(Fap21v_gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    ""
    "",
    ""
    ""
    "",
    "",
    {BIT(8)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP21V_GFA_SCREENING_ID,
    "screening",
    (PARAM_VAL_RULES *)&Fap21v_gfa_empty_vals[0],
    (sizeof(Fap21v_gfa_empty_vals) / sizeof(Fap21v_gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    ""
    "",
    ""
    ""
    "",
    "",
    {BIT(8)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP21V_GFA_STATUS_ID,
    "status",
    (PARAM_VAL_RULES *)&Fap21v_gfa_empty_vals[0],
    (sizeof(Fap21v_gfa_empty_vals) / sizeof(Fap21v_gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    ""
    "",
    ""
    ""
    "",
    "",
    {BIT(9)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  /*
   * Last element. Do not remove.  BIT(11)
   */
  {
    PARAM_END_OF_LIST
  }
};

#endif    /* } INIT*/

#ifdef _MSC_VER
  #pragma pack(pop)
#endif

#ifdef  __cplusplus
}
#endif

#endif    /* } __UI_ROM_DEFI_FAP21V_GFA_INCLUDED__*/
