/* $Id: pb_api_general.c,v 1.10 Broadcom SDK $
 * $Copyright: Copyright 2016 Broadcom Corporation.
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
/******************************************************************
*
* FILENAME:       DuneDriver/pb/src/soc_pb_api_general.c
*
* MODULE PREFIX:  pb
*
* FILE DESCRIPTION:
*
* REMARKS:
* SW License Agreement: Dune Networks (c). CONFIDENTIAL PROPRIETARY INFORMATION.
* Any use of this Software is subject to Software License Agreement
* included in the Driver User Manual of this device.
* Any use of this Software constitutes an agreement to the terms
* of the above Software License Agreement.
******************************************************************/


/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/Petra/petra_sw_db.h>

#include <soc/dpp/Petra/PB_TM/pb_api_general.h>

/* } */

/*************
 * DEFINES   *
 *************/
/* { */

/* } */

/*************
 *  MACROS   *
 *************/
/* { */

/* } */

/*************
 * TYPE DEFS *
 *************/
/* { */

/* } */

/*************
 * GLOBALS   *
 *************/
/* { */

/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

/*****************************************************
* NAME
*    soc_pb_interface_id_verify
* TYPE:
*   PROC
* DATE:
*   03/12/2007
* FUNCTION:
*   Input Verification for interface index
* INPUT:
*   SOC_SAND_IN  SOC_PETRA_INTERFACE_ID    if_ndx -
*     Interface index.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*    None.
*****************************************************/
uint32
  soc_pb_interface_id_verify(
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID    soc_pb_nif_id
  )
{
  uint8
    not_found = TRUE;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_INTERFACE_ID_VERIFY);

  if (SOC_PB_NIF_IS_TYPE_ID(XAUI, soc_pb_nif_id))
  {
    not_found = FALSE;
  }
  else if (SOC_PB_NIF_IS_TYPE_ID(RXAUI, soc_pb_nif_id))
  {
    not_found = FALSE;
  }
  else if (SOC_PB_NIF_IS_TYPE_ID(SGMII, soc_pb_nif_id))
  {
    not_found = FALSE;
  }
  else if (SOC_PB_NIF_IS_TYPE_ID(QSGMII, soc_pb_nif_id))
  {
    not_found = FALSE;
  }
  else if (SOC_PB_NIF_IS_TYPE_ID(ILKN, soc_pb_nif_id))
  {
    not_found = FALSE;
  }
  else if (SOC_PB_IS_CPU_IF_ID(soc_pb_nif_id))
  {
    not_found = FALSE;
  }
  else if (SOC_PB_IS_OLP_IF_ID(soc_pb_nif_id))
  {
    not_found = FALSE;
  }
  else if (SOC_PB_IS_RCY_IF_ID(soc_pb_nif_id))
  {
    not_found = FALSE;
  }
  else if (SOC_PB_IS_ERP_IF_ID(soc_pb_nif_id))
  {
    not_found = FALSE;
  }
  else if (SOC_PB_IS_NONE_IF_ID(soc_pb_nif_id))
  {
    not_found = FALSE;
  }

  if(not_found)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_INTERFACE_INDEX_OUT_OF_RANGE_ERR, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_interface_id_verify()",soc_pb_nif_id,0);
}

uint32
  soc_pb_if_type_from_id(
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID   soc_pb_nif_id,
    SOC_SAND_OUT SOC_PETRA_INTERFACE_TYPE *interface_type
  )
{
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IF_TYPE_FROM_ID);
  
  SOC_SAND_CHECK_NULL_INPUT(interface_type);

  *interface_type = SOC_PETRA_IF_TYPE_NONE;

  if (SOC_PB_NIF_IS_TYPE_ID(XAUI, soc_pb_nif_id))
  {
    *interface_type = SOC_PETRA_IF_TYPE_NIF;
  }
  else if (SOC_PB_NIF_IS_TYPE_ID(RXAUI, soc_pb_nif_id))
  {
    *interface_type = SOC_PETRA_IF_TYPE_NIF;
  }
  else if (SOC_PB_NIF_IS_TYPE_ID(SGMII, soc_pb_nif_id))
  {
    *interface_type = SOC_PETRA_IF_TYPE_NIF;
  }
  else if (SOC_PB_NIF_IS_TYPE_ID(QSGMII, soc_pb_nif_id))
  {
    *interface_type = SOC_PETRA_IF_TYPE_NIF;
  }
  else if (SOC_PB_NIF_IS_TYPE_ID(ILKN, soc_pb_nif_id))
  {
    *interface_type = SOC_PETRA_IF_TYPE_NIF;
  }
  else if (SOC_PB_IS_CPU_IF_ID(soc_pb_nif_id))
  {
    *interface_type = SOC_PETRA_IF_TYPE_CPU;
  }
  else if (SOC_PB_IS_OLP_IF_ID(soc_pb_nif_id))
  {
    *interface_type = SOC_PETRA_IF_TYPE_OLP;
  }
  else if (SOC_PB_IS_RCY_IF_ID(soc_pb_nif_id))
  {
    *interface_type = SOC_PETRA_IF_TYPE_RCY;
  }
  else if (SOC_PB_IS_ERP_IF_ID(soc_pb_nif_id))
  {
    *interface_type = SOC_PETRA_IF_TYPE_ERP;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_if_type_from_id()",soc_pb_nif_id,0);
}

uint32
  soc_pb_mal_equivalent_id_verify(
    SOC_SAND_IN  uint32 mal_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MAL_EQUIVALENT_ID_VERIFY);

  if (
      (mal_ndx != SOC_PB_MAL_ID_CPU) &&
      (mal_ndx != SOC_PB_MAL_ID_OLP) &&
      (mal_ndx != SOC_PB_MAL_ID_RCY) &&
      (mal_ndx != SOC_PB_MAL_ID_ERP)
     )
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(
      mal_ndx, SOC_PB_NOF_MAC_LANES-1,
      SOC_PETRA_NIF_MAL_INDEX_OUT_OF_RANGE_ERR, 10, exit
    );
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_mal_equivalent_id_verify()",mal_ndx,0);
}

SOC_PETRA_MAL_EQUIVALENT_TYPE
  soc_pb_mal_type_from_id(
    SOC_SAND_IN uint32 mal_id
  )
{
  SOC_PETRA_MAL_EQUIVALENT_TYPE
    mal_type = SOC_PB_MAL_TYPE_NONE;

  if (mal_id == SOC_PB_MAL_ID_CPU)
  {
    mal_type = SOC_PB_MAL_TYPE_CPU;
  }
  else if (mal_id == SOC_PB_MAL_ID_OLP)
  {
    mal_type = SOC_PB_MAL_TYPE_OLP;
  }
  else if (mal_id == SOC_PB_MAL_ID_RCY)
  {
    mal_type = SOC_PB_MAL_TYPE_RCY;
  }
  else if (mal_id == SOC_PB_MAL_ID_ERP)
  {
    mal_type = SOC_PB_MAL_TYPE_ERP;
  }
  else if (mal_id < SOC_PB_NOF_MAC_LANES)
  {
    mal_type = SOC_PB_MAL_TYPE_NIF;
  }

  return mal_type;

}

STATIC uint32
  soc_pb_revision_fld_get(
    SOC_SAND_IN  int unit,
    SOC_SAND_OUT uint32 *fld_val
  )
{
  uint32
    res;
  SOC_PETRA_REGS
    *regs;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  
  SOC_SAND_CHECK_NULL_INPUT(fld_val);

  regs = soc_petra_regs();
  SOC_PB_FLD_GET(regs->eci.version_reg.chip_ver, *fld_val, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_revision_fld_get()",0,0);
}

SOC_PETRA_REV
  soc_pb_revision_get(
    SOC_SAND_IN  int  unit
  )
{
  uint32
    res,
    rev_val;
  SOC_PETRA_REV
    rev;

  res = soc_pb_revision_fld_get(
          unit,
          &rev_val
        );
  if(soc_sand_get_error_code_from_error_word(res) != SOC_SAND_OK)
  {
    rev = SOC_PETRA_NOF_REVS;
  }

  switch(rev_val) {
  case SOC_PB_REVISION_FLD_VAL_A0:
    rev = SOC_PB_REV_A0;
    break;
  case SOC_PB_REVISION_FLD_VAL_A1:
    rev = SOC_PB_REV_A1;
    break;
  case SOC_PB_REVISION_FLD_VAL_B0:
    rev = SOC_PB_REV_B0;
    break;
  default:
    rev = SOC_PETRA_NOF_REVS;
  }

  return rev;
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>
