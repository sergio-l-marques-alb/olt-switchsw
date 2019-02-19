/* $Id: pcp_api_framework.c,v 1.5 Broadcom SDK $
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

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>

#include <soc/dpp/PCP/pcp_framework.h>
#include <soc/dpp/PCP/pcp_api_framework.h>
#include <soc/dpp/PCP/pcp_framework.h>

#include <soc/dpp/PCP/pcp_general.h>
#include <soc/dpp/PCP/pcp_mgmt.h>
#include <soc/dpp/PCP/pcp_statistics.h>
#include <soc/dpp/PCP/pcp_diagnostics.h>
#include <soc/dpp/PCP/pcp_oam_general.h>
#include <soc/dpp/PCP/pcp_oam_eth.h>
#include <soc/dpp/PCP/pcp_oam_bfd.h>
#include <soc/dpp/PCP/pcp_oam_mpls.h>
#include <soc/dpp/PCP/pcp_chip_regs.h>
#include <soc/dpp/PCP/pcp_chip_tbls.h>
#include <soc/dpp/PCP/pcp_tbl_access.h>
#include <soc/dpp/PCP/pcp_frwrd_mact.h>
#include <soc/dpp/PCP/pcp_frwrd_mact_mgmt.h>
#include <soc/dpp/PCP/pcp_frwrd_ilm.h>

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

#define PCP_PROCS_DESC_VALIDATE(module, err_num) \
  do { \
  if (PCP_PROC_DESC_BASE_##module##_LAST <= PCP_##module##_PROCEDURE_DESC_LAST) \
    { \
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_PROCS_FOUND_DUPLICATES_ERR, err_num, exit); \
    } \
  } while (0)

#define PCP_ERRORS_DESC_VALIDATE(module, err_num) \
  do { \
    if (PCP_ERR_DESC_BASE_##module##_LAST <= PCP_##module##_ERR_LAST) \
    { \
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERRORS_FOUND_DUPLICATES_ERR, err_num, exit); \
    } \
  } while (0)

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

CONST STATIC SOC_ERROR_DESC_ELEMENT* Pcp_error_desc_element[100];
CONST STATIC SOC_PROCEDURE_DESC_ELEMENT *Pcp_procedure_desc_element[100];

CONST STATIC
  SOC_PROCEDURE_DESC_ELEMENT
    Pcp_procedure_desc_element_api_framework[] =
{
 /*
 * pcp_oam_general.h
*/
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_GEN_ERR_NUM_ALLOC),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_GEN_ERR_NUM_ALLOC_ANY_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_GEN_ERR_NUM_FREE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_GEN_ERR_NUM_CLEAR),

/*
 * pcp_init.h
*/
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_MGMT_ECI_TBLS_INIT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_MGMT_ELK_TBLS_INIT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_MGMT_OAM_TBLS_INIT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_MGMT_STS_TBLS_INIT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_DIAG_TBLS_DUMP_TABLES_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_MGMT_TBLS_INIT),

/*
 * pcp_reg_access.c
*/
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_BASE_OFFSET_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FIELD_FROM_REG_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_FIELD_FROM_REG_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_READ_FLD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_WRITE_FLD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_READ_REG_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_WRITE_REG_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_READ_FLD_AT_INDX_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_WRITE_FLD_AT_INDX_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_READ_REG_AT_INDX_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_WRITE_REG_AT_INDX_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_READ_REG_BUFFER_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_WRITE_REG_BUFFER_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_WRITE_ARRAY_OF_FLDS),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(PCP_READ_ARRAY_OF_FLDS),


  /*
   * Last element. Do not touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
} ;

/* } */
/*************
 * FUNCTIONS *
 *************/
/* { */

/*****************************************************
*NAME
* pcp_errors_ptr_get
*TYPE:
*  PROC
*DATE:
*  10/OCT/2007
*FUNCTION:
*  Get the pointer to the list of errors of the 'Soc_petra'
*  module.
*CALLING SEQUENCE:
*  pcp_errors_ptr_get()
*INPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    list of Soc_petra errors: Pcp_error_desc_element.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    SOC_ERROR_DESC_ELEMENT * -
*      Pointer to the static list of Soc_petra errors.
*  SOC_SAND_INDIRECT:
*    .
*REMARKS:
*  This utility is mainly for external users (to the 'Soc_petra'
*  module) such as 'soc_sand module'.
*SEE ALSO:
*
*****************************************************/
CONST SOC_ERROR_DESC_ELEMENT
  *pcp_errors_ptr_get(
    void
  )
{
  CONST SOC_ERROR_DESC_ELEMENT
    *ret ;
  ret = Pcp_error_desc_element[0] ;
  return (ret) ;
}

/*****************************************************
*NAME
* pcp_procedures_ptr_get
*TYPE:
*  PROC
*DATE:
*  10/OCT/007
*FUNCTION:
*  Get the pointer to the list of procedure descriptors
*  of the 'soc_petra' module.
*CALLING SEQUENCE:
*  pcp_procedures_ptr_get()
*INPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    list of soc_petra procedures:
*      Pcp_procedure_desc_element.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    SOC_PROCEDURE_DESC_ELEMENT * -
*      Pointer to the static list of soc_petra procedures.
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*  This utility is mainly for external users (to
*  the 'soc_petra' module) such as 'soc_sand module'.
*SEE ALSO:
*
*****************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT
  *pcp_procedures_ptr_get(
    void
  )
{
  CONST SOC_PROCEDURE_DESC_ELEMENT
    *ret ;
  ret = Pcp_procedure_desc_element[0] ;
  return (ret) ;
}

/*****************************************************
*NAME
* pcp_proc_id_to_string
*TYPE:
*  PROC
*DATE:
*  17/FEB/2003
*FUNCTION:
*  Get ASCII names of module and procedure from input
*  procedure id.
*CALLING SEQUENCE:
*  pcp_proc_id_to_string(
*        proc_id,out_module_name,out_proc_name)
*INPUT:
*  SOC_SAND_DIRECT:
*    uint32  proc_id -
*      Procedure id to locate name and module of.
*    char           **out_module_name -
*      This procedure loads pointed memory with
*      pointer to null terminated string containing
*      the name of the module.
*    char           **out_proc_name -
*      This procedure loads pointed memory with
*      pointer to null terminated string containing
*      the name of the procedure.
*  SOC_SAND_INDIRECT:
*    All-system procedure descriptor pools
*    (e.g. pcp_procedure_desc_element).
*OUTPUT:
*  SOC_SAND_DIRECT:
*    int -
*      If non-zero then some error has occurred and
*      procedure string has not been located.
*  SOC_SAND_INDIRECT:
*    See out_module_name, out_module_name.
*REMARKS:
*SEE ALSO:
*****************************************************/
uint32
  pcp_proc_id_to_string(
    uint32 in_proc_id,
    char          **out_module_name,
    char          **out_proc_name
  )
{
  uint32
    ex,
    no_err,
    exit_place;
  int
    err ;
  char
    *module_name,
    *proc_name ;
  CONST SOC_PROCEDURE_DESC_ELEMENT
    *procedure_desc_element_ptr ;
  unsigned
    int
  module_id ;

  /*
   * Initialize error word
   */
  soc_sand_initialize_error_word(0,0,&ex) ;
  no_err = ex;
  exit_place = 0;

  procedure_desc_element_ptr = (SOC_PROCEDURE_DESC_ELEMENT*)0;
  /*
   * There is no failure in module name so it must be loaded
   * upon return from 'soc_sand_proc_id_to_string'.
   */
  err =
    soc_sand_proc_id_to_string(in_proc_id,&module_name,&proc_name) ;
  *out_module_name = module_name ;

  /*
   * If this search has failed then use a more basic,
   * linear search. This may be required if this procedure
   * is invoked before starting soc_sand or registering any soc_petra.
   */
  if (err)
  {
    proc_name = (char *)0 ;
    module_id = in_proc_id >> (SOC_PROC_ID_NUM_BITS - SOC_SAND_MODULE_ID_NUM_BITS) ;
    module_id &= (SOC_SAND_BIT(SOC_SAND_MODULE_ID_NUM_BITS) - 1) ;

    switch (module_id)
    {
      case SOC_SAND_MODULE_IDENTIFIER:
      {
        soc_sand_linear_find_procedure(
          in_proc_id,
          soc_sand_get_procedures_ptr(),&procedure_desc_element_ptr
        );
        break;
      }

      case PCP_MODULE_IDENTIFIER:
      {
        soc_sand_linear_find_procedure(
          in_proc_id,
          pcp_procedures_ptr_get(),&procedure_desc_element_ptr
        );
        break;
      }
    }
  }
  if (procedure_desc_element_ptr)
  {
    proc_name = (char *)procedure_desc_element_ptr->proc_name ;
  }

  if (proc_name)
  {
    *out_proc_name = proc_name ;
  }
  else
  {
    *out_proc_name = "Unknown" ;
    soc_sand_set_error_code_into_error_word(
                0,&ex) ;
  }
  if (ex != no_err)
  {
    if((0x0 | no_err) != ex)
    {
      soc_sand_error_handler(ex, "error in pcp_proc_id_to_string()", exit_place,0,0,0,0,0) ;
    }
  }
  return ex ;
}

/*********************************************************************
*     Add the pool of PCP procedure descriptors to the
 *     all-system sorted pool.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_procedure_desc_add(
  )
{
  uint32
    res;
  int32
    nof_modules = 0;
  static uint8
    Pcp_proc_desc_initialized  = FALSE;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_PROCEDURE_DESC_ADD);

  if (Pcp_proc_desc_initialized == TRUE)
  {
    PCP_DO_NOTHING_AND_EXIT;
  }

  PCP_PROCS_DESC_VALIDATE(MGMT, 20);
  PCP_PROCS_DESC_VALIDATE(FRAMEWORK, 40);
  PCP_PROCS_DESC_VALIDATE(STATISTICS, 50);
  PCP_PROCS_DESC_VALIDATE(DIAGNOSTICS, 60);
  PCP_PROCS_DESC_VALIDATE(OAM_GENERAL, 70);
  PCP_PROCS_DESC_VALIDATE(OAM_ETH, 80);
  PCP_PROCS_DESC_VALIDATE(OAM_BFD, 90);
  PCP_PROCS_DESC_VALIDATE(OAM_MPLS, 100);
  PCP_PROCS_DESC_VALIDATE(CHIP_REGS, 110);
  PCP_PROCS_DESC_VALIDATE(CHIP_TBLS, 120);
  PCP_PROCS_DESC_VALIDATE(TBL_ACCESS, 130);
  PCP_PROCS_DESC_VALIDATE(GENERAL, 140);
  PCP_PROCS_DESC_VALIDATE(FRWRD_MACT, 150);
  PCP_PROCS_DESC_VALIDATE(FRWRD_MACT_MGMT, 160);
  PCP_PROCS_DESC_VALIDATE(FRWRD_ILM, 170);

  Pcp_procedure_desc_element[nof_modules++] = Pcp_procedure_desc_element_api_framework;
  Pcp_procedure_desc_element[nof_modules++] = pcp_mgmt_get_procs_ptr();
  Pcp_procedure_desc_element[nof_modules++] = pcp_framework_get_procs_ptr();
  Pcp_procedure_desc_element[nof_modules++] = pcp_statistics_get_procs_ptr();
  Pcp_procedure_desc_element[nof_modules++] = pcp_diagnostics_get_procs_ptr();
  Pcp_procedure_desc_element[nof_modules++] = pcp_oam_general_get_procs_ptr();
  Pcp_procedure_desc_element[nof_modules++] = pcp_oam_eth_get_procs_ptr();
  Pcp_procedure_desc_element[nof_modules++] = pcp_oam_bfd_get_procs_ptr();
  Pcp_procedure_desc_element[nof_modules++] = pcp_oam_mpls_get_procs_ptr();
  Pcp_procedure_desc_element[nof_modules++] = pcp_chip_regs_get_procs_ptr();
  Pcp_procedure_desc_element[nof_modules++] = pcp_chip_tbls_get_procs_ptr();
  Pcp_procedure_desc_element[nof_modules++] = pcp_tbl_access_get_procs_ptr();
  Pcp_procedure_desc_element[nof_modules++] = pcp_general_get_procs_ptr();
  Pcp_procedure_desc_element[nof_modules++] = pcp_frwrd_mact_get_procs_ptr();
  Pcp_procedure_desc_element[nof_modules++] = pcp_frwrd_mact_mgmt_get_procs_ptr();
  Pcp_procedure_desc_element[nof_modules++] = pcp_frwrd_ilm_get_procs_ptr();
  res = soc_sand_add_proc_id_pools(Pcp_procedure_desc_element, nof_modules) ;
  SOC_SAND_CHECK_FUNC_RESULT(res, 180, exit);

  Pcp_proc_desc_initialized = TRUE;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in pcp_procedure_desc_add()",0,0);

}
/*********************************************************************
*     Add the pool of error descriptors to the all-system
 *     sorted pool.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_errors_desc_add(
  )
{
  uint32
    res;
  int32
    nof_modules = 0;
  static uint8
    Pcp_error_pool_initialized = FALSE;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_ERRORS_DESC_ADD);

   if (Pcp_error_pool_initialized == TRUE)
  {
    PCP_DO_NOTHING_AND_EXIT;
  }

  PCP_ERRORS_DESC_VALIDATE(MGMT, 20);
  PCP_ERRORS_DESC_VALIDATE(STATISTICS, 50);
  PCP_ERRORS_DESC_VALIDATE(DIAGNOSTICS, 60);
  PCP_ERRORS_DESC_VALIDATE(OAM_GENERAL, 70);
  PCP_ERRORS_DESC_VALIDATE(OAM_ETH, 80);
  PCP_ERRORS_DESC_VALIDATE(OAM_BFD, 90);
  PCP_ERRORS_DESC_VALIDATE(OAM_MPLS, 100);
  PCP_ERRORS_DESC_VALIDATE(CHIP_REGS, 110);
  PCP_ERRORS_DESC_VALIDATE(CHIP_TBLS, 120);
  PCP_ERRORS_DESC_VALIDATE(TBL_ACCESS, 130);
  PCP_ERRORS_DESC_VALIDATE(GENERAL, 140);
  PCP_ERRORS_DESC_VALIDATE(FRWRD_MACT, 150);
  PCP_ERRORS_DESC_VALIDATE(FRWRD_MACT_MGMT, 160);
  PCP_ERRORS_DESC_VALIDATE(FRWRD_ILM, 170);

  Pcp_error_desc_element[nof_modules++] = pcp_mgmt_get_errs_ptr();
  Pcp_error_desc_element[nof_modules++] = pcp_statistics_get_errs_ptr();
  Pcp_error_desc_element[nof_modules++] = pcp_diagnostics_get_errs_ptr();
  Pcp_error_desc_element[nof_modules++] = pcp_oam_general_get_errs_ptr();
  Pcp_error_desc_element[nof_modules++] = pcp_oam_eth_get_errs_ptr();
  Pcp_error_desc_element[nof_modules++] = pcp_oam_bfd_get_errs_ptr();
  Pcp_error_desc_element[nof_modules++] = pcp_oam_mpls_get_errs_ptr();
  Pcp_error_desc_element[nof_modules++] = pcp_chip_regs_get_errs_ptr();
  Pcp_error_desc_element[nof_modules++] = pcp_chip_tbls_get_errs_ptr();
  Pcp_error_desc_element[nof_modules++] = pcp_tbl_access_get_errs_ptr();
  Pcp_error_desc_element[nof_modules++] = pcp_general_get_errs_ptr();
  Pcp_error_desc_element[nof_modules++] = pcp_frwrd_mact_get_errs_ptr();
  Pcp_error_desc_element[nof_modules++] = pcp_frwrd_mact_mgmt_get_errs_ptr();
  Pcp_error_desc_element[nof_modules++] = pcp_frwrd_ilm_get_errs_ptr();
  res = soc_sand_add_error_pools(Pcp_error_desc_element, nof_modules) ;
  SOC_SAND_CHECK_FUNC_RESULT(res, 180, exit);

  Pcp_error_pool_initialized = TRUE;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in pcp_errors_desc_add()",0,0);

}
#if PCP_DEBUG_IS_LVL1

#endif /* PCP_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

