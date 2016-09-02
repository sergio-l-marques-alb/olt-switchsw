/*! \file mdb.h
 *
 * Contains all of the MDB access functions provided to the DBAL.
 *
 */
/*
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

#ifndef MDB_H_INCLUDED
/* { */
#define MDB_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <soc/mem.h>
#include <soc/drv.h>
#include <soc/dnx/dbal_defines.h>
#include <soc/dnx/dbal_structures.h>

#include <shared/shrextend/shrextend_debug.h>
#include <shared/shrextend/shrextend_error.h>

/*! ****************************************************
 * \brief
 *   Initialize the MDB databases based on the connection array retrieved from the XML.
 * \par DATE:
 *   23/MAY/2016\n
 * \par DIRECT INPUT:
 *   \param [in] unit -\n
 *      The unit number.
 * \par DIRECT OUTPUT:
 *   shr_error_e -\n
 *     Value of corresponding BCM API error.
 * \par INDIRECT OUTPUT:
 *   * None.
 * \remark
 *   None
 * \see
 *   shr_error_e
 *****************************************************/
shr_error_e mdb_init(
  int unit);

/*! ****************************************************
 * \brief
 *   Retrieve the allocated capacity for the specified physical table in 120bit rows.
 * \par DATE:
 *   23/MAY/2016\n
 * \par DIRECT INPUT:
 *   \param [in] unit -\n
 *      The unit number.
 *   \param [in] dbal_physical_table -\n
 *      The physical table in which we set the entry.
 *   \param [out] capacity -\n
 *      The capacity allocated for the physical_table in 120bit rows.
 * \par DIRECT OUTPUT:
 *   shr_error_e -\n
 *     Value of corresponding BCM API error.
 * \par INDIRECT OUTPUT:
 *   * None.
 * \remark
 *   None
 * \see
 *   shr_error_e
 *****************************************************/
shr_error_e mdb_get_capacity(
  int unit,
  dbal_physical_table_def_t * dbal_physical_table,
  int *capacity);

/*! ****************************************************
 * \brief
 *   Set a direct access entry in the specified table at the specified index.
 * \par DATE:
 *   23/MAY/2016\n
 * \par DIRECT INPUT:
 *   \param [in] unit -\n
 *      The unit number.
 *   \param [in] physical_table -\n
 *      The physical table in which we set the entry.
 *   \param [in] app_id -\n
 *      The application id.
 *   \param [in] entry -\n
 *      The entry we would like to set. \n
 *      * entry.key - the entry index. \n
 *      * entry.payload - the entry content. \n
 *      * entry.payload_size - the entry size.
 * \par DIRECT OUTPUT:
 *   shr_error_e -\n
 *     Value of corresponding BCM API error.
 * \par INDIRECT OUTPUT:
 *   * None.
 * \remark
 *   Currently only supports up to 120bits.
 * \see
 *   shr_error_e
 *****************************************************/
shr_error_e mdb_direct_table_entry_add(
  int unit,
  dbal_physical_tables_e physical_table,
  uint32 app_id,
  dbal_physical_entry_t * entry);

/*! ****************************************************
 * \brief
 *   Get a direct access entry in the specified table at the specified index.\n
 *   Currently only supports up to 120bits.
 * \par DATE:
 *   23/MAY/2016\n
 * \par DIRECT INPUT:
 *   \param [in] unit -\n
 *      The unit number.
 *   \param [in] physical_table -\n
 *      The physical table from which we get the entry.
 *   \param [in] app_id -\n
 *      The application id.
 *   \param [in,out] entry -\n
 *      The entry we would like to get. \n
 *      entry.key - the entry index. \n
 *      entry.payload_size - the entry size.
 * \par DIRECT OUTPUT:
 *   shr_error_e -\n
 *     Value of corresponding BCM API error.
 * \par INDIRECT OUTPUT:
 *   \param [in,out] entry -\n
 *      The entry we would like to get. \n
 *      entry.payload - the entry content.
 * \remark
 *   None
 * \see
 *   shr_error_e
 *****************************************************/
shr_error_e mdb_direct_table_entry_get(
  int unit,
  dbal_physical_tables_e physical_table,
  uint32 app_id,
  dbal_physical_entry_t * entry);

/*! ****************************************************
 * \brief
 *   Delete a direct access entry in the specified table at the specified index.\n
 *   This function is invalid for direct access tables and is only provided for completeness.
 * \par DATE:
 *   23/MAY/2016\n
 * \par DIRECT INPUT:
 *   \param [in] unit -\n
 *      The unit number.
 *   \param [in] physical_table -\n
 *      The physical table from which we delete the entry.
 *   \param [in] app_id -\n
 *      The application id.
 *   \param [in,out] entry -\n
 *      The entry we would like to delete. \n
 *      entry.key - the entry index. \n
 *      entry.payload_size - the entry size.
 * \par DIRECT OUTPUT:
 *   shr_error_e -\n
 *     Value of corresponding BCM API error.
 * \par INDIRECT OUTPUT:
 *   * None.
 * \remark
 *   None
 * \see
 *   shr_error_e
 *****************************************************/
shr_error_e mdb_direct_table_entry_delete(
  int unit,
  dbal_physical_tables_e physical_table,
  uint32 app_id,
  dbal_physical_entry_t * entry);

/*! ****************************************************
 * \brief
 *   Add an exact match entry in the specified table.
 * \par DATE:
 *   23/MAY/2016\n
 * \par DIRECT INPUT:
 *   \param [in] unit -\n
 *      The unit number.
 *   \param [in] physical_table -\n
 *      The physical table in which we set the entry.
 *   \param [in] app_id -\n
 *      The application id.
 *   \param [in] entry -\n
 *      The entry we would like to set. \n
 *      entry.key - the entry key. \n
 *      entry.key_size - the entry key size. \n
 *      entry.payload - the entry content. \n
 *      entry.payload_size - the entry size.
 * \par DIRECT OUTPUT:
 *   shr_error_e -\n
 *     Value of corresponding BCM API error.
 * \par INDIRECT OUTPUT:
 *   * None.
 * \remark
 *   None
 * \see
 *   shr_error_e
 *****************************************************/
shr_error_e mdb_em_entry_add(
  int unit,
  dbal_physical_tables_e physical_table,
  uint32 app_id,
  dbal_physical_entry_t * entry);

/*! ****************************************************
 * \brief
 *   Get an exact match entry from the specified table.
 * \par DATE:
 *   23/MAY/2016\n
 * \par DIRECT INPUT:
 *   \param [in] unit -\n
 *      The unit number.
 *   \param [in] physical_table -\n
 *      The physical table from which we get the entry.
 *   \param [in] app_id -\n
 *      The application id.
 *   \param [in] entry -\n
 *      The entry we would like to set. \n
 *      entry.key - the entry key. \n
 *      entry.key_size - the entry key size. \n
 *      entry.payload - the entry content. \n
 *      entry.payload_size - the entry size.
 * \par DIRECT OUTPUT:
 *   shr_error_e -\n
 *     Value of corresponding BCM API error.
 * \par INDIRECT OUTPUT:
 *   * None.
 * \remark
 *   None
 * \see
 *   shr_error_e
 *****************************************************/
shr_error_e mdb_em_entry_get(
  int unit,
  dbal_physical_tables_e physical_table,
  uint32 app_id,
  dbal_physical_entry_t * entry);

/*! ****************************************************
 * \brief
 *   Delete an exact match entry in the specified table.
 * \par DATE:
 *   23/MAY/2016\n
 * \par DIRECT INPUT:
 *   \param [in] unit -\n
 *      The unit number.
 *   \param [in] physical_table -\n
 *      The physical table from which we delete the entry.
 *   \param [in] app_id -\n
 *      The application id.
 *   \param [in] entry -\n
 *      The entry we would like to set. \n
 *      entry.key - the entry key. \n
 *      entry.key_size - the entry key size.
 * \par DIRECT OUTPUT:
 *   shr_error_e -\n
 *     Value of corresponding BCM API error.
 * \par INDIRECT OUTPUT:
 *   * None.
 * \remark
 *   None
 * \see
 *   shr_error_e
 *****************************************************/
shr_error_e mdb_em_entry_delete(
  int unit,
  dbal_physical_tables_e physical_table,
  uint32 app_id,
  dbal_physical_entry_t * entry);

/* } */
#endif /* !MDB_H_INCLUDED */
