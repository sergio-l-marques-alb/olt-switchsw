/* $Id: soc_petra_header_parsing_utils.h,v 1.6 Broadcom SDK $
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


#ifndef __SOC_PETRA_HEADER_PARSING_UTILS_INCLUDED__
/* { */
#define __SOC_PETRA_HEADER_PARSING_UTILS_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/Petra/petra_api_general.h>
#include <soc/dpp/TMC/tmc_api_header_parsing_utils.h>
#include <soc/dpp/Petra/petra_api_ports.h>
/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/* $Id: soc_petra_header_parsing_utils.h,v 1.6 Broadcom SDK $
 *  Soc_petra number of uint32s per FTMH.
 */
#define SOC_PETRA_HPU_FTMH_SIZE_UINT32S         2

#define SOC_PETRA_HPU_ITMH_DESTINATION_LSB        0
#define SOC_PETRA_HPU_ITMH_DESTINATION_MSB        17

#define SOC_PETRA_HPU_OTMH_SRC_PORT_LAG_ID_MSB    7
#define SOC_PETRA_HPU_OTMH_SRC_PORT_LAG_ID_LSB    0


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

typedef SOC_TMC_HPU_ITMH_HDR                                   SOC_PETRA_HPU_ITMH_HDR;
typedef SOC_TMC_HPU_FTMH_BASE                                  SOC_PETRA_HPU_FTMH_BASE;
typedef SOC_TMC_HPU_FTMH_EXT_OUTLIF                            SOC_PETRA_HPU_FTMH_EXT_OUTLIF;
typedef SOC_TMC_HPU_FTMH                                       SOC_PETRA_HPU_FTMH;
typedef SOC_TMC_HPU_FTMH_HDR                                   SOC_PETRA_HPU_FTMH_HDR;
typedef SOC_TMC_HPU_OTMH_BASE                                  SOC_PETRA_HPU_OTMH_BASE;
typedef SOC_TMC_HPU_OTMH_EXT_SRC_PORT                          SOC_PETRA_HPU_OTMH_EXT_SRC_PORT;
typedef SOC_TMC_HPU_OTMH_EXT_DEST_PORT                         SOC_PETRA_HPU_OTMH_EXT_DEST_PORT;
typedef SOC_TMC_HPU_OTMH_EXTENSIONS                            SOC_PETRA_HPU_OTMH_EXTENSIONS;
typedef SOC_TMC_HPU_OTMH                                       SOC_PETRA_HPU_OTMH;
typedef SOC_TMC_HPU_OTMH_HDR                                   SOC_PETRA_HPU_OTMH_HDR;

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
  soc_petra_hpu_itmh_build_verify(
    SOC_SAND_IN  SOC_PETRA_PORTS_ITMH            *info
  );

/*********************************************************************
*     This function receives the fields that construct the
*     Fabric TM Header and builds a 48-bit (or 64-bit with
*     extension) stream header.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_hpu_ftmh_build_verify(
    SOC_SAND_IN  SOC_PETRA_HPU_FTMH            *info
  );

/*********************************************************************
*     This function receives the fields that construct the
*     Outgoing TM Header and builds a 16-bit (or 32/48/64-bit
*     with extensions) stream header.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_hpu_otmh_build_verify(
    SOC_SAND_IN  SOC_PETRA_HPU_OTMH            *info
  );

/*********************************************************************
* NAME:
*     soc_petra_hpu_itmh_build
* TYPE:
*   PROC
* FUNCTION:
*     This function receives the fields that construct the
*     Incoming TM Header and builds a ITMH header (base +
*     extension).
* INPUT:
*  SOC_SAND_IN  SOC_PETRA_PORTS_ITMH          *info -
*     The information that is put in the header inc. the
*     extension info.
*  SOC_SAND_OUT SOC_PETRA_HPU_ITMH_HDR        *itmh -
*     The ITMH and extension.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_hpu_itmh_build(
    SOC_SAND_IN  SOC_PETRA_PORTS_ITMH          *info,
    SOC_SAND_OUT SOC_PETRA_HPU_ITMH_HDR        *itmh
  );

/*********************************************************************
* NAME:
*     soc_petra_hpu_itmh_parse
* TYPE:
*   PROC
* FUNCTION:
*     This function retrieves the fields that construct the
*     Incoming TM Header given an ITMH header (base +
*     extension).
* INPUT:
*  SOC_SAND_IN  SOC_PETRA_HPU_ITMH_HDR        *itmh -
*     The ITMH header + extension.
*  SOC_SAND_OUT SOC_PETRA_PORTS_ITMH          *info -
*     The information from the header inc. the extension
*     information.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_hpu_itmh_parse(
    SOC_SAND_IN  SOC_PETRA_HPU_ITMH_HDR        *itmh,
    SOC_SAND_OUT SOC_PETRA_PORTS_ITMH          *info
  );

/*********************************************************************
* NAME:
*     soc_petra_hpu_ftmh_build
* TYPE:
*   PROC
* FUNCTION:
*     This function receives the fields that construct the
*     Fabric TM Header and builds a FTMH header (base +
*     extension).
* INPUT:
*  SOC_SAND_IN  SOC_PETRA_HPU_FTMH            *info -
*     The information that is put in the header inc. the
*     extension information.
*  SOC_SAND_OUT SOC_PETRA_HPU_FTMH_HDR        *ftmh -
*     The FTMH header + extension.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_hpu_ftmh_build(
    SOC_SAND_IN  SOC_PETRA_HPU_FTMH            *info,
    SOC_SAND_OUT SOC_PETRA_HPU_FTMH_HDR        *ftmh
  );

/*********************************************************************
* NAME:
*     soc_petra_hpu_ftmh_parse
* TYPE:
*   PROC
* FUNCTION:
*     This function retrieves the fields that construct the
*     Fabric TM Header given an FTMH header (base +
*     extension).
* INPUT:
*  SOC_SAND_IN  SOC_PETRA_HPU_FTMH_HDR        *ftmh -
*     The FTMH header + extension.
*  SOC_SAND_OUT SOC_PETRA_HPU_FTMH            *info -
*     The information from the header inc. the extension
*     information.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_hpu_ftmh_parse(
    SOC_SAND_IN  SOC_PETRA_HPU_FTMH_HDR        *ftmh,
    SOC_SAND_OUT SOC_PETRA_HPU_FTMH            *info
  );

/*********************************************************************
* NAME:
*     soc_petra_hpu_otmh_build
* TYPE:
*   PROC
* FUNCTION:
*     This function receives the fields that construct the
*     Outgoing TM Header and builds a OTMH header (base +
*     extensions).
* INPUT:
*  SOC_SAND_IN  SOC_PETRA_HPU_OTMH            *info -
*     The information that is put in the header inc. the
*     extensions information.
*  SOC_SAND_OUT SOC_PETRA_HPU_OTMH_HDR        *otmh -
*     The OTMH header + extensions.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_hpu_otmh_build(
    SOC_SAND_IN  SOC_PETRA_HPU_OTMH            *info,
    SOC_SAND_OUT SOC_PETRA_HPU_OTMH_HDR        *otmh
  );

/*********************************************************************
* NAME:
*     soc_petra_hpu_otmh_parse
* TYPE:
*   PROC
* FUNCTION:
*     This function retrieves the fields that construct the
*     Outgoing TM Header given an OTMH header (base +
*     extension).
* INPUT:
*  SOC_SAND_IN  SOC_PETRA_HPU_OTMH_HDR        *otmh -
*     The 16-bit (or 32/48/64-bit with extension) stream
*     header.
*  SOC_SAND_OUT SOC_PETRA_HPU_OTMH            *info -
*     The information from the header inc. the extension
*     information.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_hpu_otmh_parse(
    SOC_SAND_IN  SOC_PETRA_HPU_OTMH_HDR        *otmh,
    SOC_SAND_OUT SOC_PETRA_HPU_OTMH            *info
  );

void
  soc_petra_PETRA_HPU_ITMH_HDR_clear(
    SOC_SAND_OUT SOC_PETRA_HPU_ITMH_HDR *info
  );

void
  soc_petra_PETRA_HPU_FTMH_BASE_clear(
    SOC_SAND_OUT SOC_PETRA_HPU_FTMH_BASE *info
  );

void
  soc_petra_PETRA_HPU_FTMH_EXT_OUTLIF_clear(
    SOC_SAND_OUT SOC_PETRA_HPU_FTMH_EXT_OUTLIF *info
  );

void
  soc_petra_PETRA_HPU_FTMH_clear(
    SOC_SAND_OUT SOC_PETRA_HPU_FTMH *info
  );

void
  soc_petra_PETRA_HPU_FTMH_HDR_clear(
    SOC_SAND_OUT SOC_PETRA_HPU_FTMH_HDR *info
  );

void
  soc_petra_PETRA_HPU_OTMH_BASE_clear(
    SOC_SAND_OUT SOC_PETRA_HPU_OTMH_BASE *info
  );

void
  soc_petra_PETRA_HPU_OTMH_EXT_SRC_PORT_clear(
    SOC_SAND_OUT SOC_PETRA_HPU_OTMH_EXT_SRC_PORT *info
  );

void
  soc_petra_PETRA_HPU_OTMH_EXT_DEST_PORT_clear(
    SOC_SAND_OUT SOC_PETRA_HPU_OTMH_EXT_DEST_PORT *info
  );

void
  soc_petra_PETRA_HPU_OTMH_EXTENSIONS_clear(
    SOC_SAND_OUT SOC_PETRA_HPU_OTMH_EXTENSIONS *info
  );

void
  soc_petra_PETRA_HPU_OTMH_clear(
    SOC_SAND_OUT SOC_PETRA_HPU_OTMH *info
  );

void
  soc_petra_PETRA_HPU_OTMH_HDR_clear(
    SOC_SAND_OUT SOC_PETRA_HPU_OTMH_HDR *info
  );

#if SOC_PETRA_DEBUG


void
  soc_petra_PETRA_HPU_ITMH_HDR_print(
    SOC_SAND_IN SOC_PETRA_HPU_ITMH_HDR *info
  );



void
  soc_petra_PETRA_HPU_FTMH_BASE_print(
    SOC_SAND_IN SOC_PETRA_HPU_FTMH_BASE *info
  );



void
  soc_petra_PETRA_HPU_FTMH_EXT_OUTLIF_print(
    SOC_SAND_IN SOC_PETRA_HPU_FTMH_EXT_OUTLIF *info
  );



void
  soc_petra_PETRA_HPU_FTMH_print(
    SOC_SAND_IN SOC_PETRA_HPU_FTMH *info
  );



void
  soc_petra_PETRA_HPU_FTMH_HDR_print(
    SOC_SAND_IN SOC_PETRA_HPU_FTMH_HDR *info
  );



void
  soc_petra_PETRA_HPU_OTMH_BASE_print(
    SOC_SAND_IN SOC_PETRA_HPU_OTMH_BASE *info
  );



void
  soc_petra_PETRA_HPU_OTMH_EXT_SRC_PORT_print(
    SOC_SAND_IN SOC_PETRA_HPU_OTMH_EXT_SRC_PORT *info
  );



void
  soc_petra_PETRA_HPU_OTMH_EXT_DEST_PORT_print(
    SOC_SAND_IN SOC_PETRA_HPU_OTMH_EXT_DEST_PORT *info
  );



void
  soc_petra_PETRA_HPU_OTMH_EXTENSIONS_print(
    SOC_SAND_IN SOC_PETRA_HPU_OTMH_EXTENSIONS *info
  );



void
  soc_petra_PETRA_HPU_OTMH_print(
    SOC_SAND_IN SOC_PETRA_HPU_OTMH *info
  );



void
  soc_petra_PETRA_HPU_OTMH_HDR_print(
    SOC_SAND_IN SOC_PETRA_HPU_OTMH_HDR *info
  );


#endif /* SOC_PETRA_DEBUG */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PETRA_HEADER_PARSING_UTILS_INCLUDED__*/
#endif
