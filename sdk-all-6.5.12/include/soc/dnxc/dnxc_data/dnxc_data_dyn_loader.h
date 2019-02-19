/** \file dnxc_data_dyn_loader.h
 * 
 * DEVICE DATA Dynamic Loader - Function used for dynamic DATA parsing.
 * This method is mostly useful for SKUs handling. 
 */
/*
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifndef _DNXC_DATA_DYN_LOADER_H_
/*{*/
#define _DNXC_DATA_DYN_LOADER_H_

/**
* \brief This file is only used by DNXC. Including it by
* software that is not specific to DNXC is an error.
*/
#if !defined(BCM_DNX_SUPPORT) && !defined(BCM_DNXF_SUPPORT)
#error "This file is for use by DNXC family only!"
#endif

/**
 * \brief - Parse dynamic DNX DATA. 
 * Usually DNX data is parsed statically (auto coder) but in order to support SKUs, some features 
 * require that their data will be loaded dynamically 
 * 
 * \param [in] unit - Unit ID
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * Unlike static DNX Data which is divided into XML files per module, in Dynamic Data all the information will be
 *     gathered inside a single file per chip(SKU).
 *
 *   * Limitations and assumptions:
 *      * Dynamic Data which depends on soc property (numeric, table) doesn't change the method type from the
 *        original data.
 *      * In case of 'Direct Mapping' method, Dynamic Data doesn't support new mapping (meaning that only disabling
 *        some of the originally supported mappings is allowed.).
 *
 * \see
 *   * None
 */
shr_error_e dnxc_data_dyn_loader_parse_init(
    int unit);

/*}*/
#endif /*_DNXC_DATA_DYN_LOADER_H_*/
