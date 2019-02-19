/* $Id: pb_pp_isem_access.h,v 1.7 Broadcom SDK $
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
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_PP/include/soc_pb_pp_isem_access.h
*
* MODULE PREFIX:  soc_pb_pp
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

#ifndef __SOC_PB_PP_ISEM_ACCESS_INCLUDED__
/* { */
#define __SOC_PB_PP_ISEM_ACCESS_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_framework.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_general.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_api_lif.h>

#include <soc/dpp/Petra/PB_TM/pb_pp_chip_regs.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_PP_ISEM_ACCESS_KEY_SIZE                                  (2)

#define SOC_PB_PP_ISEM_ACCESS_1ST_LOOKUP_PROGRAM_VD                     (0)
#define SOC_PB_PP_ISEM_ACCESS_1ST_LOOKUP_PROGRAM_VD_VID                 (1)
#define SOC_PB_PP_ISEM_ACCESS_1ST_LOOKUP_PROGRAM_VD_VID_VID             (2)
#define SOC_PB_PP_ISEM_ACCESS_1ST_LOOKUP_PROGRAM_ID_ISID_WO_BTAG        (3)
#define SOC_PB_PP_ISEM_ACCESS_1ST_LOOKUP_PROGRAM_ID_ISID_W_BTAG         (4)
#define SOC_PB_PP_ISEM_ACCESS_1ST_LOOKUP_PROGRAM_NO_LOOKUP              (5)
#define SOC_PB_PP_ISEM_ACCESS_1ST_LOOKUP_PROGRAM_EXT_KEY                (8)
#define SOC_PB_PP_ISEM_ACCESS_1ST_LOOKUP_PROGRAM_EXT_KEY_VID            (9)
#define SOC_PB_PP_ISEM_ACCESS_1ST_LOOKUP_PROGRAM_EXT_KEY_VID_VID        (10)
#define SOC_PB_PP_ISEM_ACCESS_1ST_LOOKUP_PROGRAM_VD_COMP_VID            (11)
#define SOC_PB_PP_ISEM_ACCESS_1ST_LOOKUP_PROGRAM_VD_COMP_VID_COMP_VID   (12)
                                                            
#define SOC_PB_PP_ISEM_ACCESS_2ND_LOOKUP_PROGRAM_DST_IP                 (0)
#define SOC_PB_PP_ISEM_ACCESS_2ND_LOOKUP_PROGRAM_IN_RIF_MPLS1           (1)
#define SOC_PB_PP_ISEM_ACCESS_2ND_LOOKUP_PROGRAM_IN_RIF_MPLS2           (2)
#define SOC_PB_PP_ISEM_ACCESS_2ND_LOOKUP_PROGRAM_IN_RIF_MPLS3           (3)
#define SOC_PB_PP_ISEM_ACCESS_2ND_LOOKUP_PROGRAM_TRILL_ING_NICK         (4)
#define SOC_PB_PP_ISEM_ACCESS_2ND_LOOKUP_PROGRAM_MPLS1                  (5)
#define SOC_PB_PP_ISEM_ACCESS_2ND_LOOKUP_PROGRAM_MPLS2                  (6)
#define SOC_PB_PP_ISEM_ACCESS_2ND_LOOKUP_PROGRAM_MPLS3                  (7)

#define SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_INVA                      {  0,   0,   0,   0,   0}
#define SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_VLAN_DOMAIN               {  5,  16,   0,   1,   1}
#define SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_INITIAL_VID               { 11,  19,   0,   1,   1}
#define SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_CMP_OUTER_VID             { 11,  22,   0,   1,   1}
#define SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_CMP_INNER_VID             { 11,  25,   0,   1,   1}
#define SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_ISID_DOMAIN               {  5,  14,   0,   1,   1}
#define SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_2ND_MPLS_IN_RIF           { 12,  46,   0,   1,   1}
#define SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_ISID_WO_BTAG              { 23,  28,   1,   0,   1}
#define SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_ISID_W_BTAG               { 23,  36,   1,   0,   1}


#define SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_DST_IP                    { 31,  32,   2,   0,   1}
#define SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_MPLS1                     { 19, 253,   2,   0,   1}
#define SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_MPLS2                     { 19,   5,   2,   0,   1}
#define SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_MPLS3                     { 19,  13,   2,   0,   1}
#define SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_MPLS1_LSB(n)              { n-1, 253,   2,   0,   1}
#define SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_MPLS2_LSB(n)              { n-1,   5,   2,   0,   1}
#define SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_MPLS3_LSB(n)              { n-1,  13,   2,   0,   1}
#define SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_TRILL_ING_NICK            { 15,   4,   2,   0,   1}
#define SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_HDR_INNER_VID             { 11,  32,   1,   0,   1}
#define SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_AH1_IN_AC_KEY             { 13, 252,   0,   0,   1}
#define SOC_PB_PP_ISEM_ACCESS_INSTRUCTION_CEK_AH1_IN_AC_KEY_LSB(n)      { n-1, 252,   0,   0,   1}

/* } */
/*************
 * MACROS    *
 *************/
/* { */

/* } */
/*************
 * TYPE DEFS *
 *************/
/* { */

typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PB_PP_ISEM_ACCESS_GET_PROCS_PTR = SOC_PB_PP_PROC_DESC_BASE_ISEM_ACCESS_FIRST,
  SOC_PB_PP_ISEM_ACCESS_GET_ERRS_PTR,
  /*
   * } Auto generated. Do not edit previous section.
   */

  /*
   * Last element. Do no touch.
   */
  SOC_PB_PP_ISEM_ACCESS_PROCEDURE_DESC_LAST
} SOC_PB_PP_ISEM_ACCESS_PROCEDURE_DESC;

typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  /*
   * } Auto generated. Do not edit previous section.
   */
  SOC_PB_PP_ISEM_ACCESS_UNKNOWN_KEY_TYPE_ERR = SOC_PB_PP_ERR_DESC_BASE_ISEM_ACCESS_FIRST,
  SOC_PB_PP_ISEM_ACCESS_NOT_READY_ERR,

  /*
   * Last element. Do no touch.
   */
  SOC_PB_PP_ISEM_ACCESS_ERR_LAST
} SOC_PB_PP_ISEM_ACCESS_ERR;

typedef enum
{
  SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_VD          = 0x0,  /* 4'b0000 */
  SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_VD_VID      = 0x2,  /* 4'b0010 */
  SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_VD_VID_VID  = 0x3,  /* 4'b0011 */
  SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_EXT_KEY     = 0xd,  /* 4'b1101 */ /* Identical to TRILL */
  SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_EXT_KEY_VID = 0x6,  /* 4'b0110 */
  SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_EXT_KEY_VID_VID=0x8, /* 4'b1xxx */
  SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_VD_COMP_VID = 0xe,  /* 4'b1110 */
  SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_VD_COMP_VID_COMP_VID=0xf, /* 4'b1111 */
  /* second lookup */
  SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_PBB         = 0xc,  /* 4'b1100 */
  SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_MPLS_IN_RIF = 0x8,  /* 4'b10xx */ /* tunnel_term*/
  SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_IP_TUNNEL   = 0x4,  /* 4'b01xx */
  SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_MPLS        = 0x1,  /* 4'b0001 */  /* pwe term*/
  SOC_PB_PP_ISEM_ACCESS_KEY_TYPE_TRILL       = 0x5/*0xd*/  /* 4'b1101 */  /* Identical to External-Key, differentiate in
                                                                  the key construction with '1's addition */
} SOC_PB_PP_ISEM_ACCESS_KEY_TYPE;

typedef union
{
  struct
  {
    uint32 vlan_domain;
    uint32 outer_vid;
    uint32 inner_vid;
  } l2_eth;

  struct
  {
    uint32 in_rif;
    uint32 label;
  } mpls;

  struct
  {
    uint32 dip;
  } ip_tunnel;

  struct
  {
    uint32 nick_name;
  } trill;

  struct
  {
    uint32 isid_domain;
    uint32 isid;
  } pbb;

  struct
  {
    uint32 raw_val;
    uint32 outer_vid;
    uint32 inner_vid;
  } raw;

} SOC_PB_PP_ISEM_ACCESS_KEY_INFO;

typedef struct
{
  SOC_PB_PP_ISEM_ACCESS_KEY_TYPE key_type;
  SOC_PB_PP_ISEM_ACCESS_KEY_INFO key_info;
} SOC_PB_PP_ISEM_ACCESS_KEY;

typedef struct
{
  uint32 sem_result_ndx;
  uint32 sem_op_code;

} SOC_PB_PP_ISEM_ACCESS_ENTRY;

typedef struct
{
  uint32 prog_idx;
  SOC_PB_PP_CE_INSTRUCTION key_inst0;
  SOC_PB_PP_CE_INSTRUCTION key_inst1;
  SOC_PB_PP_CE_INSTRUCTION key_inst2;
  SOC_PB_PP_CE_INSTRUCTION key_inst3;
  uint32 isem_lookup_enable;
  uint32 and_mask;
  uint32 or_mask;
}SOC_PB_PP_ISEM_ACCESS_1ST_LOOKUP_PROGRAM;

typedef struct
{
  uint32 prog_idx;
  SOC_PB_PP_CE_INSTRUCTION key_inst0;
  SOC_PB_PP_CE_INSTRUCTION key_inst1;
  uint32 and_mask;
  uint32 or_mask;
}SOC_PB_PP_ISEM_ACCESS_2ND_LOOKUP_PROGRAM;

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

uint32
  soc_pb_pp_isem_access_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );

uint32
  soc_pb_pp_isem_access_entry_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_ISEM_ACCESS_KEY                             *isem_key,
    SOC_SAND_OUT SOC_PB_PP_ISEM_ACCESS_ENTRY                           *isem_entry,
    SOC_SAND_OUT uint8                                 *success
  );

uint32
  soc_pb_pp_isem_access_entry_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_ISEM_ACCESS_KEY                             *svem_key,
    SOC_SAND_IN  SOC_PB_PP_ISEM_ACCESS_ENTRY                           *svem_entry,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

uint32
  soc_pb_pp_isem_access_entry_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_ISEM_ACCESS_KEY                             *svem_key,
    SOC_SAND_OUT uint8                                 *success
  );

uint32
  soc_pb_pp_isem_access_key_from_buffer(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  *buffer,
    SOC_SAND_OUT SOC_PB_PP_ISEM_ACCESS_KEY                     *isem_key
  );

uint32
  soc_pb_pp_isem_access_entry_from_buffer(
    SOC_SAND_IN  uint32                                  *buffer,
    SOC_SAND_OUT SOC_PB_PP_ISEM_ACCESS_ENTRY                   *isem_entry
  );

uint32
  soc_pb_pp_isem_access_key_to_buffer(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_ISEM_ACCESS_KEY                     *isem_key,
    SOC_SAND_OUT uint32                                  *buffer
  );

uint32
  soc_pb_pp_l2_lif_ac_key_to_sem_key_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_KEY                           *ac_key,
    SOC_SAND_OUT SOC_PB_PP_ISEM_ACCESS_KEY                         *sem_key
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_isem_access_get_procs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of procedures of the
 *   soc_pb_pp_api_isem_access module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_pp_isem_access_get_procs_ptr(void);

/*********************************************************************
* NAME:
 *   soc_pb_pp_isem_access_get_errs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of errors of the
 *   soc_pb_pp_api_isem_access module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_pp_isem_access_get_errs_ptr(void);

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_PP_ISEM_ACCESS_INCLUDED__*/
#endif
