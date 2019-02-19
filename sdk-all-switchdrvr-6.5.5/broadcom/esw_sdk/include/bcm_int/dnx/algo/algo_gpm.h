/*! \file algo_gpm.h
 * 
 * Internal DNX Gport Managment APIs 
 * 
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
 *
 */

#ifndef _ALGO_GPM_API_INCLUDED__
/* { */
#define _ALGO_GPM_API_INCLUDED__

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*!
 * \brief gport info structure.
 *
 * This struct contains physical information on gport of type physical
 * port or on the relevant physical port that a gport is mapped to.
 *
 */
typedef struct
{
  /*
   * !
   * Local port
   */
  bcm_port_t local_port;
  /*
   * !
   * System port
   */
  uint32 sys_port;
  
  uint32 flags;
  /*
   * !
   * Lane: relevant in case gport is of type PHYN
   */
  int lane;
  /*
   * !
   * Phyn: relevant in case gport is of type PHYN
   */
  int phyn;
  /*
   * !
   * Internal local port, to use with core_id
   */
  int internal_port;
  /*
   * !
   * Core id, to use with Internal local port
   */
  int core_id;
} dnx_algo_gpm_gport_info_t;

/*!
 * \brief Get physical information from GPORT.
 *
 * This function is used to retrieve information from GPORT \n
 * Mapping is done from bcm_gport_t to local and system port, core_id etc.
 *
 * \par DIRECT INPUT:
 *   \param [in] unit unit gport belongs to
 *   \param [in] gport just gport
 *   \param [in] operations required output
 *   \param [in, out] gport_info pointer to retrieved information
 * \par DIRECT OUTPUT:
 *   \retval Error indication according to shr_error_e enum
 * \par INDIRECT OUTPUT:
 *   *gport_info - information on physical port
 */
shr_error_e dnx_algo_gpm_gport_to_phy_port(
  int unit,
  bcm_gport_t gport,
  uint32 operations,
  dnx_algo_gpm_gport_info_t * gport_info);

/* } */
#endif/*_ALGO_GPM_API_INCLUDED__*/
