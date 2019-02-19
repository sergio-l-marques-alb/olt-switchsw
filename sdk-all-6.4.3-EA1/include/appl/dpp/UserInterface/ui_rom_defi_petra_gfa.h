/* $Id: ui_rom_defi_petra_gfa.h,v 1.4 Broadcom SDK $
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


#ifndef __UI_ROM_DEFI_PETRA_GFA_INCLUDED__
/* { */
#define __UI_ROM_DEFI_PETRA_GFA_INCLUDED__

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <appl/dpp/UserInterface/ui_pure_defi_petra_gfa.h>

#include <appl/diag/dpp/utils_line_gfa_petra.h>

EXTERN CONST
   PARAM_VAL_RULES
     soc_petra_gfa_free_vals[]
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
     soc_petra_gfa_empty_vals[]
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
     soc_petra_gfa_synt_sizer_type_rule[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "fabric",
    {
      {
        GFA_PETRA_SYNT_TYPE_FABRIC,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  GFA_TG_SYNT_TYPEs.GFA_PETRA_SYNT_TYPE_FABRIC:\r\n"
        "  Fabric \r\n"
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
    "combo",
    {
      {
        GFA_PETRA_SYNT_TYPE_COMBO,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  GFA_TG_SYNT_TYPEs.GFA_PETRA_SYNT_TYPE_COMBO:\r\n"
        "  Fabric \r\n"
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
    "nif",
    {
      {
        GFA_PETRA_SYNT_TYPE_NIF,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  GFA_TG_SYNT_TYPEs.GFA_PETRA_SYNT_TYPE_NIF:\r\n"
        "  Fabric \r\n"
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
    "core",
    {
      {
        GFA_PETRA_SYNT_TYPE_CORE,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  GFA_TG_SYNT_TYPEs.GFA_PETRA_SYNT_TYPE_CORE:\r\n"
        "  Fabric \r\n"
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
    "ddr",
    {
      {
        GFA_PETRA_SYNT_TYPE_DDR,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  GFA_TG_SYNT_TYPEs.GFA_PETRA_SYNT_TYPE_DDR:\r\n"
        "  Fabric \r\n"
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
    "qdr",
    {
      {
        GFA_PETRA_SYNT_TYPE_QDR,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  GFA_TG_SYNT_TYPEs.GFA_PETRA_SYNT_TYPE_QDR:\r\n"
        "  Fabric \r\n"
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
    "pcp_core",
    {
      {
        GFA_PETRA_SYNT_TYPE_PCP_CORE,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  GFA_BI_SYNT_TYPEs.GFA_PETRA_SYNT_TYPE_PCP_CORE:\r\n"
        "  Pcp core \r\n"
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
      "pcp_elk",
    {
      {
        GFA_PETRA_SYNT_TYPE_PCP_ELK,
          /*
          * Casting added here just to keep the compiler silent.
          */
          (long)""
          "  GFA_BI_SYNT_TYPEs.GFA_PETRA_SYNT_TYPE_PCP_ELK:\r\n"
          "  Pcp elk interface \r\n"
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
      "phy",
    {
      {
        GFA_PETRA_SYNT_TYPE_PHY,
          /*
          * Casting added here just to keep the compiler silent.
          */
          (long)""
          "  GFA_BI_SYNT_TYPEs.GFA_PETRA_SYNT_TYPE_PHY:\r\n"
          "  Phy\r\n"
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
/********************************************************/
EXTERN CONST
   PARAM
     soc_petra_gfa_params[]
#ifdef INIT
   =
{
  {
    PARAM_GFA_PETRA_SYNT_ASSERT_ID,
    "assert",
    (PARAM_VAL_RULES *)&soc_petra_gfa_empty_vals[0],
    (sizeof(soc_petra_gfa_empty_vals) / sizeof(soc_petra_gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "", "", "",
    {BIT(0), 0, 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GFA_PETRA_SYNT_SET_ID,
    "set",
    (PARAM_VAL_RULES *)&soc_petra_gfa_empty_vals[0],
    (sizeof(soc_petra_gfa_empty_vals) / sizeof(soc_petra_gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "", "", "",
    {BIT(1), 0, 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GFA_PETRA_SYNT_SET_SYN_TYPE_ID,
    "synt_type",
    (PARAM_VAL_RULES *)&soc_petra_gfa_synt_sizer_type_rule[0],
    (sizeof(soc_petra_gfa_synt_sizer_type_rule) / sizeof(soc_petra_gfa_synt_sizer_type_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  synt_type:\r\n"
    "  The synthisizer Type: \r\n"
    "",
    "",
    "",
    {BIT(1), 0, 0, 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GFA_PETRA_SYNT_SET_FREQ_VAL_ID,
    "freq",
    (PARAM_VAL_RULES *)&soc_petra_gfa_free_vals[0],
    (sizeof(soc_petra_gfa_free_vals) / sizeof(soc_petra_gfa_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  freq:\r\n"
    "  frequency value. \r\n"
    "",
    "",
    "",
    {BIT(1), 0, 0, 0},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GFA_PETRA_SYNT_ID,
    "synt",
    (PARAM_VAL_RULES *)&soc_petra_gfa_empty_vals[0],
    (sizeof(soc_petra_gfa_empty_vals) / sizeof(soc_petra_gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "", "", "",
    {BIT(0) | BIT(1), 0, 0, 0},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GFA_PETRA_START_CSR_ID,
    "start_csr",
    (PARAM_VAL_RULES *)&soc_petra_gfa_empty_vals[0],
    (sizeof(soc_petra_gfa_empty_vals) / sizeof(soc_petra_gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "", "", "",
    {0, BIT(2), 0, 0},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GFA_PETRA_GPIO_OP_IO_VAL_ID,
    "io_val",
    (PARAM_VAL_RULES *)&soc_petra_gfa_free_vals[0],
    (sizeof(soc_petra_gfa_free_vals) / sizeof(soc_petra_gfa_free_vals[0])) - 1,
    HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  EPLD GPIO I/O reg value\r\n"
    "",
    "",
    "",
    {0, BIT(3), 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GFA_PETRA_GPIO_OP_DATA_VAL_ID,
    "data_val",
    (PARAM_VAL_RULES *)&soc_petra_gfa_free_vals[0],
    (sizeof(soc_petra_gfa_free_vals) / sizeof(soc_petra_gfa_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  EPLD GPIO I/O data value\r\n"
    "",
    "",
    "",
    {0, BIT(3), 0, 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GFA_PETRA_I2C_WRITE_ID,
      "i2c_write",
      (PARAM_VAL_RULES *)&soc_petra_gfa_empty_vals[0],
      (sizeof(soc_petra_gfa_empty_vals) / sizeof(soc_petra_gfa_empty_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "", "", "",
    {0, 0, BIT(5), 0},
    1,
    /*
    * Pointer to a function to call after symbolic parameter
    * has been accepted.
    */
    (VAL_PROC_PTR)NULL
  },    
  {
    PARAM_GFA_PETRA_I2C_READ_ID,
      "i2c_read",
      (PARAM_VAL_RULES *)&soc_petra_gfa_empty_vals[0],
      (sizeof(soc_petra_gfa_empty_vals) / sizeof(soc_petra_gfa_empty_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "", "", "",
    {0, 0, BIT(6), 0},
    1,
    /*
    * Pointer to a function to call after symbolic parameter
    * has been accepted.
    */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GFA_PETRA_I2C_WRITE_DEV_ID,
    "dev",
    (PARAM_VAL_RULES *)&soc_petra_gfa_free_vals[0],
    (sizeof(soc_petra_gfa_free_vals) / sizeof(soc_petra_gfa_free_vals[0])) - 1,
    HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  i2c device id\r\n"
    "",
    "",
    "",
    {0, 0, BIT(5), 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GFA_PETRA_I2C_WRITE_REG_ID,
    "reg",
    (PARAM_VAL_RULES *)&soc_petra_gfa_free_vals[0],
    (sizeof(soc_petra_gfa_free_vals) / sizeof(soc_petra_gfa_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  i2c device inner register id\r\n"
    "",
    "",
    "",
    {0, 0, BIT(5) , 0},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GFA_PETRA_I2C_WRITE_VAL_ID,
    "val",
    (PARAM_VAL_RULES *)&soc_petra_gfa_free_vals[0],
    (sizeof(soc_petra_gfa_free_vals) / sizeof(soc_petra_gfa_free_vals[0])) - 1,
    HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  value \r\n"
    "",
    "",
    "",
    {0, 0, BIT(5), 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
    {
    PARAM_GFA_PETRA_I2C_READ_DEV_ID,
    "dev",
    (PARAM_VAL_RULES *)&soc_petra_gfa_free_vals[0],
    (sizeof(soc_petra_gfa_free_vals) / sizeof(soc_petra_gfa_free_vals[0])) - 1,
    HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  i2c device id\r\n"
    "",
    "",
    "",
    {0, 0, BIT(6), 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GFA_PETRA_I2C_READ_REG_ID,
    "reg",
    (PARAM_VAL_RULES *)&soc_petra_gfa_free_vals[0],
    (sizeof(soc_petra_gfa_free_vals) / sizeof(soc_petra_gfa_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  i2c device inner register id\r\n"
    "",
    "",
    "",
    {0, 0, BIT(6) , 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GFA_PETRA_BI_HOT_SWAP_ID,
    "bi_hot_swap",
    (PARAM_VAL_RULES *)&soc_petra_gfa_free_vals[0],
    (sizeof(soc_petra_gfa_free_vals) / sizeof(soc_petra_gfa_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "", "", "",
    {BIT(2), 0, 0, 0},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GFA_PETRA_BI_PS_AD_READ_ALL_ID,
    "bi_ps_ad_read_all",
    (PARAM_VAL_RULES *)&soc_petra_gfa_empty_vals[0],
    (sizeof(soc_petra_gfa_empty_vals) / sizeof(soc_petra_gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "", "", "",
    {BIT(3), 0, 0, 0},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GFA_PETRA_BI_BSP_I2C_WRITE_GEN_ID,
    "bi_bsp_i2c_write_gen",
    (PARAM_VAL_RULES *)&soc_petra_gfa_empty_vals[0],
    (sizeof(soc_petra_gfa_empty_vals) / sizeof(soc_petra_gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "", "", "",
    {BIT(4), 0, 0, 0},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GFA_PETRA_BI_BSP_I2C_WRITE_GEN_DEV_ID,
    "device_address",
    (PARAM_VAL_RULES *)&soc_petra_gfa_free_vals[0],
    (sizeof(soc_petra_gfa_free_vals) / sizeof(soc_petra_gfa_free_vals[0])) - 1,
    HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "", "", "",
    {BIT(4), 0, 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GFA_PETRA_BI_BSP_I2C_WRITE_GEN_INT_ADDR_ID,
    "internal_address",
    (PARAM_VAL_RULES *)&soc_petra_gfa_free_vals[0],
    (sizeof(soc_petra_gfa_free_vals) / sizeof(soc_petra_gfa_free_vals[0])) - 1,
    HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "", "", "",
    {BIT(4), 0, 0, 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GFA_PETRA_BI_BSP_I2C_WRITE_GEN_INT_ADDR_LEN_ID,
    "internal_address_len",
    (PARAM_VAL_RULES *)&soc_petra_gfa_free_vals[0],
    (sizeof(soc_petra_gfa_free_vals) / sizeof(soc_petra_gfa_free_vals[0])) - 1,
    HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "", "", "",
    {BIT(4), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GFA_PETRA_BI_BSP_I2C_WRITE_GEN_WRITE_DATA_ID,
    "write_data",
    (PARAM_VAL_RULES *)&soc_petra_gfa_free_vals[0],
    (sizeof(soc_petra_gfa_free_vals) / sizeof(soc_petra_gfa_free_vals[0])) - 1,
    HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "", "", "",
    {BIT(4), 0, 0, 0},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GFA_PETRA_BI_BSP_I2C_WRITE_GEN_WRITE_DATA_LEN_ID,
    "write_data_len",
    (PARAM_VAL_RULES *)&soc_petra_gfa_free_vals[0],
    (sizeof(soc_petra_gfa_free_vals) / sizeof(soc_petra_gfa_free_vals[0])) - 1,
    HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "", "", "",
    {BIT(4), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GFA_PETRA_BI_BSP_I2C_READ_GEN_ID,
    "bi_bsp_i2c_read_gen",
    (PARAM_VAL_RULES *)&soc_petra_gfa_empty_vals[0],
    (sizeof(soc_petra_gfa_empty_vals) / sizeof(soc_petra_gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "", "", "",
    {BIT(5), 0, 0, 0},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GFA_PETRA_BI_BSP_I2C_READ_GEN_DEV_ID,
    "device_address",
    (PARAM_VAL_RULES *)&soc_petra_gfa_free_vals[0],
    (sizeof(soc_petra_gfa_free_vals) / sizeof(soc_petra_gfa_free_vals[0])) - 1,
    HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "", "", "",
    {BIT(5), 0, 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GFA_PETRA_BI_BSP_I2C_READ_GEN_INT_ADDR_ID,
    "internal_address",
    (PARAM_VAL_RULES *)&soc_petra_gfa_free_vals[0],
    (sizeof(soc_petra_gfa_free_vals) / sizeof(soc_petra_gfa_free_vals[0])) - 1,
    HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "", "", "",
    {BIT(5), 0, 0, 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GFA_PETRA_BI_BSP_I2C_READ_GEN_INT_ADDR_LEN_ID,
    "internal_address_len",
    (PARAM_VAL_RULES *)&soc_petra_gfa_free_vals[0],
    (sizeof(soc_petra_gfa_free_vals) / sizeof(soc_petra_gfa_free_vals[0])) - 1,
    HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "", "", "",
    {BIT(5), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GFA_PETRA_BI_BSP_I2C_READ_GEN_EXP_READ_LEN_ID,
    "exp_read_len",
    (PARAM_VAL_RULES *)&soc_petra_gfa_free_vals[0],
    (sizeof(soc_petra_gfa_free_vals) / sizeof(soc_petra_gfa_free_vals[0])) - 1,
    HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "", "", "",
    {BIT(5), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GFA_PETRA_BI_BSP_NLP1024_PHY_INIT_ID,
    "bi_bsp_nlp1024_phy_init",
    (PARAM_VAL_RULES *)&soc_petra_gfa_free_vals[0],
    (sizeof(soc_petra_gfa_free_vals) / sizeof(soc_petra_gfa_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "", "", "",
    {BIT(6), 0, 0, 0},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  /*
   * Last element. Do not remove.
   */
  {
    PARAM_END_OF_LIST
  }
}
#endif
;

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __UI_ROM_DEFI_PETRA_GFA_INCLUDED__*/
#endif
