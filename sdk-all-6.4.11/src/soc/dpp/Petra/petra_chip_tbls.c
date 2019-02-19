/* $Id: petra_chip_tbls.c,v 1.9 Broadcom SDK $
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
* FILENAME:       soc_petra_chip_tbls.c
*
* MODULE PREFIX:  soc_petra_tblsO
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

#include <soc/dpp/Petra/petra_chip_tbls.h>
#include <soc/dpp/Petra/petra_api_framework.h>
/* } */

/*************
 * DEFINES   *
 *************/
/* { */

static SOC_PETRA_INVALID_DEVICE_TBL  Soc_petra_invalid_tbls;

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
static SOC_PETRA_TBLS  Soc_petra_tbls;
static uint8    Soc_petra_tbls_initialized = FALSE;
/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */
STATIC void
  soc_petra_tbl_fld_set(
    SOC_PETRA_TBL_FIELD *field,
    uint8       msb,
    uint8       lsb
  )
{
   field->msb  = msb;
   field->lsb  = lsb;
   return;
}

uint32
  soc_petra_invalid_tbls_init(void)
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_TBLS_INIT);

  if (Soc_petra_tbls_initialized == TRUE)
  {
    goto exit;
  }

  /* Invalid table */
  Soc_petra_invalid_tbls.addr.base = 0xFFFFFFFF;
  Soc_petra_invalid_tbls.addr.size = 0xFFFFFFFF;
  Soc_petra_invalid_tbls.addr.width_bits = 0;
  soc_petra_tbl_fld_set( &(Soc_petra_invalid_tbls.invalid_device), 0, 0);

  Soc_petra_tbls_initialized = TRUE;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_invalid_tbls_init",0,0);
}

uint32
  soc_petra_tbls_get(
    SOC_SAND_OUT  SOC_PETRA_TBLS  **soc_petra_tbls
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_TBLS_GET);

  if (Soc_petra_tbls_initialized == FALSE)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_TBLS_NOT_INITIALIZED, 10, exit);
  }

  *soc_petra_tbls = &Soc_petra_tbls;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pa_tbls_get()", 0, 0);
}

SOC_PETRA_TBLS*
  soc_petra_tbls(void)
{
  SOC_PETRA_TBLS*
    tbls = NULL;

  soc_petra_tbls_get(
    &tbls
  );

  return tbls;
}


uint32
  soc_petra_tbls_init(void)
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_TBLS_INIT);

  if (Soc_petra_tbls_initialized == TRUE)
  {
    goto exit;
  }

#ifdef LINK_PA_LIBRARIES
  soc_pa_tbls_init();
  Soc_petra_tbls.a_tables = soc_pa_tbls();
#endif
#ifdef LINK_PB_LIBRARIES
  soc_pb_tbls_init();
  Soc_petra_tbls.b_tables = soc_pb_tbls();
#endif
#if !(defined(LINK_PA_LIBRARIES) || defined(LINK_PB_LIBRARIES))
  soc_petra_invalid_tbls_init();
  Soc_petra_tbls.invalid_device_tbl = &Soc_petra_invalid_tbls;
#endif

  Soc_petra_tbls_initialized = TRUE;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_tbls_init",0,0);
}

/* } */


#include <soc/dpp/SAND/Utils/sand_footer.h>

