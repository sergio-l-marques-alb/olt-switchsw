/* $Id: petra_scheduler_element_converts.c,v 1.8 Broadcom SDK $
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
/******************************************************************
*
* FILENAME:       soc_petra_scheduler_element_converts.c
*
* MODULE PREFIX:  soc_petra_scheduler_element_converts
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
#include <shared/bsl.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/Petra/petra_scheduler_element_converts.h>
#include <soc/dpp/Petra/petra_chip_tbls.h>
#include <soc/dpp/Petra/petra_framework.h>
#include <soc/dpp/Petra/petra_chip_defines.h>
#include <soc/dpp/Petra/petra_reg_access.h>
#include <soc/dpp/Petra/petra_sw_db.h>
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
 * See details in soc_petra_sch_scheduler.h
 *****************************************************/
SOC_SAND_RET
  soc_petra_sch_INTERNAL_CLASS_TYPE_to_CLASS_TYPE_convert(
    SOC_SAND_IN     SOC_PETRA_SCH_SCT_TBL_DATA     *internal_class_type,
    SOC_SAND_OUT    SOC_PETRA_SCH_SE_CL_CLASS_INFO *class_type
  )
{
  SOC_PETRA_INIT_ERR_DEFS;

  SOC_PETRA_ERR_IF_NULL(class_type, 10);
  SOC_PETRA_ERR_IF_NULL(internal_class_type, 20);

  switch (internal_class_type->clconfig)
  {
  case 15:
    class_type->mode = SOC_PETRA_SCH_CL_MODE_1;
    break;
  case 3:
    class_type->mode = SOC_PETRA_SCH_CL_MODE_2;
    break;
  case 8:
    class_type->mode = SOC_PETRA_SCH_CL_MODE_3;
    break;
  case 1:
    class_type->mode = SOC_PETRA_SCH_CL_MODE_4;
    break;
  case 0:
    class_type->mode = SOC_PETRA_SCH_CL_MODE_5;
    break;
  default:
    SOC_PETRA_SET_ERR_AND_EXIT(SOC_PETRA_SCH_CLCONFIG_OUT_OF_RANGE_ERR);
  }
  class_type->weight[0] = internal_class_type->af0_inv_weight;
  class_type->weight[1] = internal_class_type->af1_inv_weight;
  class_type->weight[2] = internal_class_type->af2_inv_weight;
  class_type->weight[3] = internal_class_type->af3_inv_weight;

  class_type->weight_mode = internal_class_type->wfqmode;

  switch(internal_class_type->enh_clen)
  {
  case 0:
    class_type->enhanced_mode = SOC_PETRA_CL_ENHANCED_MODE_DISABLED;
    break;
  case 1:
    if (internal_class_type->enh_clsphigh)
    {
      class_type->enhanced_mode = SOC_PETRA_CL_ENHANCED_MODE_ENABLED_HP;
    }
    else
    {
      class_type->enhanced_mode = SOC_PETRA_CL_ENHANCED_MODE_ENABLED_LP;
    }
    break;
  default:
    SOC_PETRA_SET_ERR_AND_EXIT(SOC_PETRA_SCH_ENH_MODE_OUT_OF_RANGE_ERR);
  }

exit:
  SOC_PETRA_RETURN;
}

/*****************************************************
 * See details in soc_petra_sch_scheduler.h
 *****************************************************/
SOC_SAND_RET
  soc_petra_sch_CLASS_TYPE_to_INTERNAL_CLASS_TYPE_convert(
    SOC_SAND_IN     int                 unit,
    SOC_SAND_IN   SOC_PETRA_SCH_SE_CL_CLASS_INFO *class_type,
    SOC_SAND_OUT  SOC_PETRA_SCH_SCT_TBL_DATA     *internal_class_type
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PETRA_TBLS
    *tables;
  SOC_PETRA_SCH_SCT_TBL
    *sct_tbl;

  SOC_PETRA_INIT_ERR_DEFS;

  SOC_PETRA_ERR_IF_NULL(class_type, 10);
  SOC_PETRA_ERR_IF_NULL(internal_class_type, 20);

  sal_memset(
    internal_class_type,
    0x0,
    sizeof(SOC_PETRA_SCH_SCT_TBL_DATA)
  );

  res = soc_petra_tbls_get(&tables);
  SOC_PETRA_EXIT_IF_ERR(res, 30);

  sct_tbl = SOC_PETRA_TBL_REF(tables->sch.sct_tbl);

  switch (class_type->mode)
  {
  case SOC_PETRA_SCH_CL_MODE_1:
    internal_class_type->clconfig = 0xf;
    break;
  case SOC_PETRA_SCH_CL_MODE_2:
    internal_class_type->clconfig = 0x3;
    break;
  case SOC_PETRA_SCH_CL_MODE_3:
    internal_class_type->clconfig = 0x8;
    break;
  case SOC_PETRA_SCH_CL_MODE_4:
    internal_class_type->clconfig = 0x1;
    break;
  case SOC_PETRA_SCH_CL_MODE_5:
    internal_class_type->clconfig = 0x0;
    break;
  default:
    SOC_PETRA_SET_ERR_AND_EXIT(SOC_PETRA_SCH_CLCONFIG_OUT_OF_RANGE_ERR);
  }

  internal_class_type->af0_inv_weight = class_type->weight[0];
  SOC_SAND_LIMIT_FROM_ABOVE(internal_class_type->af0_inv_weight, SOC_PETRA_FLD_MAX(sct_tbl->af0_inv_weight));

  internal_class_type->af1_inv_weight = class_type->weight[1];
  SOC_SAND_LIMIT_FROM_ABOVE(internal_class_type->af1_inv_weight, SOC_PETRA_FLD_MAX(sct_tbl->af1_inv_weight));

  internal_class_type->af2_inv_weight = class_type->weight[2];
  SOC_SAND_LIMIT_FROM_ABOVE(internal_class_type->af2_inv_weight, SOC_PETRA_FLD_MAX(sct_tbl->af2_inv_weight));

  internal_class_type->af3_inv_weight = class_type->weight[3];
  SOC_SAND_LIMIT_FROM_ABOVE(internal_class_type->af3_inv_weight, SOC_PETRA_FLD_MAX(sct_tbl->af3_inv_weight));

  internal_class_type->wfqmode = class_type->weight_mode;

  /*
   *  There is a limitation in the device, that when work
   *  in mode 1 or mode 2, the weighting can't be
   *  independent per flow
   */
  if(
     (class_type->mode == SOC_PETRA_SCH_CL_MODE_1) ||
     (class_type->mode == SOC_PETRA_SCH_CL_MODE_2)
    )
  {
    if(class_type->weight_mode == SOC_PETRA_SCH_CL_WEIGHTS_MODE_INDEPENDENT_PER_FLOW)
    {
      internal_class_type->wfqmode = SOC_PETRA_SCH_CL_WEIGHTS_MODE_DISCRETE_PER_FLOW;
    }
  }

  /* In Mode 1 the weighting must be Discrete per Flow */
  if(class_type->mode == SOC_PETRA_SCH_CL_MODE_1)
  {
    internal_class_type->wfqmode = SOC_PETRA_SCH_CL_WEIGHTS_MODE_DISCRETE_PER_FLOW;
  }

  switch(class_type->enhanced_mode)
  {
  case SOC_PETRA_CL_ENHANCED_MODE_DISABLED:
    internal_class_type->enh_clen = 0;
    internal_class_type->enh_clsphigh = 0;
    break;
  case SOC_PETRA_CL_ENHANCED_MODE_ENABLED_HP:
    internal_class_type->enh_clen = 1;
    internal_class_type->enh_clsphigh = 1;
    break;
  case SOC_PETRA_CL_ENHANCED_MODE_ENABLED_LP:
    internal_class_type->enh_clen = 1;
    internal_class_type->enh_clsphigh = 0;
    break;
  default:
    SOC_PETRA_SET_ERR_AND_EXIT(SOC_PETRA_SCH_ENH_MODE_OUT_OF_RANGE_ERR);
    break;
  }

exit:
  SOC_PETRA_RETURN;
}


/* } */


#include <soc/dpp/SAND/Utils/sand_footer.h>
