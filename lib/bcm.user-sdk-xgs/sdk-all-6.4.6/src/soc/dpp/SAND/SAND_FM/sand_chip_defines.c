/* $Id: sand_chip_defines.c,v 1.5 Broadcom SDK $
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
/******************************************************************
*
* FILENAME:       soc_sand_chip_defines.c
*
* FILE DESCRIPTION: soc_sand chip general utilities
*
* REMARKS:
* SW License Agreement: Dune Networks (c). CONFIDENTIAL PROPRIETARY INFORMATION.
* Any use of this Software is subject to Software License Agreement
* included in the Driver User Manual of this device.
* Any use of this Software constitutes an agreement to the terms
* of the above Software License Agreement.
******************************************************************/


#include <soc/dpp/SAND/Utils/sand_header.h>

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/SAND_FM/sand_chip_defines.h>
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
*NAME
*   soc_sand_entity_enum_to_str
*TYPE:
*  PROC
*DATE:
*  30/05/2006
*FUNCTION:
*  returns a string representing the appropriate entity type
*INPUT:
*  SOC_SAND_DIRECT:
*    const char* - the output string representing the entity_type
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    a string representing the appropriate entity type
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*    None.
*SEE ALSO:
*****************************************************/
char *
  soc_sand_entity_enum_to_str(
    SOC_SAND_IN SOC_SAND_DEVICE_ENTITY entity_type
  )
{
  char
    *str = "";

  switch(entity_type)
  {
   case(SOC_SAND_FE1_ENTITY):
      str = "FE1";
      break;
    case(SOC_SAND_FE2_ENTITY):
      str = "FE2";
      break;
    case(SOC_SAND_FE3_ENTITY):
      str = "FE3";
      break;
    case(SOC_SAND_FAP_ENTITY):
      str = "FAP";
      break;
    case(SOC_SAND_FOP_ENTITY):
      str = "FOP";
      break;
    case(SOC_SAND_FIP_ENTITY):
      str = "FIP";
      break;
    case(SOC_SAND_FE13_ENTITY):
      str = "FE13";
      break;
    case(SOC_SAND_DONT_CARE_ENTITY):
    default:
      str = "???";
      break;
  }

  return str;
}

/*****************************************************
*NAME
*  soc_sand_entity_from_level
*TYPE:
*  PROC
*DATE:
*  30/05/2006
*FUNCTION:
*  Returns soc_sand device entity corresponding to a given source level type value
*INPUT:
*  SOC_SAND_DIRECT:
*    SOC_SAND_ENTITY_LEVEL_TYPE level_val - source level type value
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    soc_sand device entity
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*    None.
*SEE ALSO:
*****************************************************/
SOC_SAND_DEVICE_ENTITY
  soc_sand_entity_from_level(
    SOC_SAND_IN SOC_SAND_ENTITY_LEVEL_TYPE level_val
  )
{
  SOC_SAND_DEVICE_ENTITY
    res = SOC_SAND_DONT_CARE_ENTITY;

  switch(level_val)
  {
    case(SOC_SAND_ACTUAL_FAP_VALUE_1):
    case(SOC_SAND_ACTUAL_FIP_VALUE):
    case(SOC_SAND_ACTUAL_FOP_VALUE):
    case(SOC_SAND_ACTUAL_FAP_VALUE):
      res = SOC_SAND_FAP_ENTITY;
      break;
    case(SOC_SAND_ACTUAL_FE1_VALUE):
      res = SOC_SAND_FE1_ENTITY;
      break;
    case(SOC_SAND_ACTUAL_FE2_VALUE):
    case(SOC_SAND_ACTUAL_FE2_VALUE_1):
      res = SOC_SAND_FE2_ENTITY;
      break;
    case(SOC_SAND_ACTUAL_FE3_VALUE):
      res = SOC_SAND_FE3_ENTITY;
      break;
    default:
      res = SOC_SAND_DONT_CARE_ENTITY;
      break;
  }

  return res;
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>
