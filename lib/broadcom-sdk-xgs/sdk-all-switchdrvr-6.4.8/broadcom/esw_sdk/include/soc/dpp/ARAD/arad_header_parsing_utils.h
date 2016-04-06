/* $Id: arad_header_parsing_utils.h,v 1.6 Broadcom SDK $
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


#ifndef __ARAD_HEADER_PARSING_UTILS_INCLUDED__
/* { */
#define __ARAD_HEADER_PARSING_UTILS_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/ARAD/arad_api_general.h>
#include <soc/dpp/TMC/tmc_api_header_parsing_utils.h>
#include <soc/dpp/ARAD/arad_api_ports.h>
/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/*
 *  Arad number of longs (32 bit) per FTMH.
 */
#define ARAD_HPU_FTMH_SIZE_UINT32S         2

#define ARAD_HPU_ITMH_DESTINATION_LSB(mode)     ((itmh_prog_mode) ? 8 : 0 )        
#define ARAD_HPU_ITMH_DESTINATION_MSB(mode)     ((itmh_prog_mode) ? 26 : 19 )         

#define ARAD_HPU_OTMH_SRC_PORT_LAG_ID_MSB    7
#define ARAD_HPU_OTMH_SRC_PORT_LAG_ID_LSB    0


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

typedef SOC_TMC_HPU_ITMH_HDR                                   ARAD_HPU_ITMH_HDR;
typedef SOC_TMC_HPU_FTMH_BASE                                  ARAD_HPU_FTMH_BASE;
typedef SOC_TMC_HPU_FTMH_EXT_OUTLIF                            ARAD_HPU_FTMH_EXT_OUTLIF;
typedef SOC_TMC_HPU_FTMH                                       ARAD_HPU_FTMH;
typedef SOC_TMC_HPU_FTMH_HDR                                   ARAD_HPU_FTMH_HDR;
typedef SOC_TMC_HPU_OTMH_BASE                                  ARAD_HPU_OTMH_BASE;
typedef SOC_TMC_HPU_OTMH_EXT_SRC_PORT                          ARAD_HPU_OTMH_EXT_SRC_PORT;
typedef SOC_TMC_HPU_OTMH_EXT_DEST_PORT                         ARAD_HPU_OTMH_EXT_DEST_PORT;
typedef SOC_TMC_HPU_OTMH_EXTENSIONS                            ARAD_HPU_OTMH_EXTENSIONS;
typedef SOC_TMC_HPU_OTMH                                       ARAD_HPU_OTMH;
typedef SOC_TMC_HPU_OTMH_HDR                                   ARAD_HPU_OTMH_HDR;

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

/*********************************************************************
*     This function receives the fields that construct the
*     Incoming TM Header and builds a ITMH header (base +
*     extension).
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  arad_hpu_itmh_build_verify(
    SOC_SAND_IN  ARAD_PORTS_ITMH            *info
  );

/*********************************************************************
* NAME:
*     arad_hpu_itmh_build
* TYPE:
*   PROC
* FUNCTION:
*     This function receives the fields that construct the
*     Incoming TM Header and builds a ITMH header (base +
*     extension).
* INPUT:
*  SOC_SAND_IN  ARAD_PORTS_ITMH          *info -
*     The information that is put in the header inc. the
*     extension info.
*  SOC_SAND_OUT ARAD_HPU_ITMH_HDR        *itmh -
*     The ITMH and extension.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32 arad_hpu_itmh_build(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PORTS_ITMH          *info,
    SOC_SAND_OUT ARAD_HPU_ITMH_HDR        *itmh);

/*********************************************************************
* NAME:
*     arad_hpu_itmh_parse
* TYPE:
*   PROC
* FUNCTION:
*     This function retrieves the fields that construct the
*     Incoming TM Header given an ITMH header (base +
*     extension).
* INPUT:
*  SOC_SAND_IN  ARAD_HPU_ITMH_HDR        *itmh -
*     The ITMH header + extension.
*  SOC_SAND_OUT ARAD_PORTS_ITMH          *info -
*     The information from the header inc. the extension
*     information.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  arad_hpu_itmh_parse(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  ARAD_HPU_ITMH_HDR        *itmh,
    SOC_SAND_OUT ARAD_PORTS_ITMH          *info
  );

/*********************************************************************
* NAME:
*     arad_hpu_itmh_fwd_dest_info_build
* TYPE:
*   PROC
* FUNCTION:
*     This function retrieves the ITMH destination struct
*     and builds the ITMH destination field
* INPUT:
*  SOC_SAND_IN  int        unit -
*     Device id
*  SOC_SAND_IN  ARAD_DEST_INFO    destination -
*     Destination info struct
*  SOC_SAND_OUT  uint32        *dest -
*     ITMH destination field.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32 arad_hpu_itmh_fwd_dest_info_build(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  ARAD_DEST_INFO            destination,
    SOC_SAND_IN  uint8                     is_advanced_mode, 
    SOC_SAND_IN  uint8                     is_extension,  
    SOC_SAND_OUT uint32                    *dest);

uint32 arad_hpu_itmh_fwd_dest_info_parse(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN uint32                                  dest_in,
    SOC_SAND_IN  uint8                     is_advanced_mode, 
    SOC_SAND_IN  uint8                     is_extension,  
    SOC_SAND_OUT  ARAD_DEST_INFO            *destination);

void
  arad_ARAD_HPU_ITMH_HDR_clear(
    SOC_SAND_OUT ARAD_HPU_ITMH_HDR *info
  );

void
  arad_ARAD_HPU_FTMH_BASE_clear(
    SOC_SAND_OUT ARAD_HPU_FTMH_BASE *info
  );

void
  arad_ARAD_HPU_FTMH_EXT_OUTLIF_clear(
    SOC_SAND_OUT ARAD_HPU_FTMH_EXT_OUTLIF *info
  );

void
  arad_ARAD_HPU_FTMH_clear(
    SOC_SAND_OUT ARAD_HPU_FTMH *info
  );

void
  arad_ARAD_HPU_FTMH_HDR_clear(
    SOC_SAND_OUT ARAD_HPU_FTMH_HDR *info
  );

void
  arad_ARAD_HPU_OTMH_BASE_clear(
    SOC_SAND_OUT ARAD_HPU_OTMH_BASE *info
  );

void
  arad_ARAD_HPU_OTMH_EXT_SRC_PORT_clear(
    SOC_SAND_OUT ARAD_HPU_OTMH_EXT_SRC_PORT *info
  );

void
  arad_ARAD_HPU_OTMH_EXT_DEST_PORT_clear(
    SOC_SAND_OUT ARAD_HPU_OTMH_EXT_DEST_PORT *info
  );

void
  arad_ARAD_HPU_OTMH_EXTENSIONS_clear(
    SOC_SAND_OUT ARAD_HPU_OTMH_EXTENSIONS *info
  );

void
  arad_ARAD_HPU_OTMH_clear(
    SOC_SAND_OUT ARAD_HPU_OTMH *info
  );

void
  arad_ARAD_HPU_OTMH_HDR_clear(
    SOC_SAND_OUT ARAD_HPU_OTMH_HDR *info
  );

#if ARAD_DEBUG


void
  arad_ARAD_HPU_ITMH_HDR_print(
    SOC_SAND_IN ARAD_HPU_ITMH_HDR *info
  );



void
  arad_ARAD_HPU_FTMH_BASE_print(
    SOC_SAND_IN ARAD_HPU_FTMH_BASE *info
  );



void
  arad_ARAD_HPU_FTMH_EXT_OUTLIF_print(
    SOC_SAND_IN ARAD_HPU_FTMH_EXT_OUTLIF *info
  );



void
  arad_ARAD_HPU_FTMH_print(
    SOC_SAND_IN ARAD_HPU_FTMH *info
  );



void
  arad_ARAD_HPU_FTMH_HDR_print(
    SOC_SAND_IN ARAD_HPU_FTMH_HDR *info
  );



void
  arad_ARAD_HPU_OTMH_BASE_print(
    SOC_SAND_IN ARAD_HPU_OTMH_BASE *info
  );



void
  arad_ARAD_HPU_OTMH_EXT_SRC_PORT_print(
    SOC_SAND_IN ARAD_HPU_OTMH_EXT_SRC_PORT *info
  );



void
  arad_ARAD_HPU_OTMH_EXT_DEST_PORT_print(
    SOC_SAND_IN ARAD_HPU_OTMH_EXT_DEST_PORT *info
  );



void
  arad_ARAD_HPU_OTMH_EXTENSIONS_print(
    SOC_SAND_IN ARAD_HPU_OTMH_EXTENSIONS *info
  );



void
  arad_ARAD_HPU_OTMH_print(
    SOC_SAND_IN ARAD_HPU_OTMH *info
  );



void
  arad_ARAD_HPU_OTMH_HDR_print(
    SOC_SAND_IN ARAD_HPU_OTMH_HDR *info
  );


#endif /* ARAD_DEBUG */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __ARAD_HEADER_PARSING_UTILS_INCLUDED__*/
#endif

