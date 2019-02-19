
 
/* $Id: jer_pp_lbp_init.c,v 1.22 Broadcom SDK $
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

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_INGRESS

/*************
 * INCLUDES  *
 *************/
/* { */
#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>
#include <soc/dpp/SAND/Utils/sand_bitstream.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>

#include <soc/dpp/JER/JER_PP/jer_pp_lbp_init.h>

#include <soc/dpp/drv.h>

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



/* set configuration mapping table: PPH-VSI-Source:
 * 
 * format: 
 * ------------------------------------------------------------------------------------
 * |                         INDEX                        ||       VALUE             | 
 * ------------------------------------------------------------------------------------
 * |  Forwarding code | stamp-     |  is-MC  |  OutLif-   ||  pph-vsi-outRif-Source  | 
 * |      (4b)        | native-vsi |         |  is-outRif ||        (3b)             | 
 * ------------------------------------------------------------------------------------
 * 
 */

/* value ( pph-vsi-outRif-Source) for PPH-VSI-SOURCE */
#define SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_PPH_VSI_OUTRIF_SOURCE_VRF          0
#define SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_PPH_VSI_OUTRIF_SOURCE_NATIVE_VSI   1
#define SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_PPH_VSI_OUTRIF_SOURCE_OUTLIF       2
#define SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_PPH_VSI_OUTRIF_SOURCE_MINUS_1      3
#define SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_PPH_VSI_OUTRIF_SOURCE_INRIF        4
#define SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_PPH_VSI_OUTRIF_SOURCE_VSI          5
/* valuenof bits */
#define SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_PPH_VSI_OUTRIF_SOURCE_NOF_BITS     3

/* index fields lsbs */
#define SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_INDEX_OUTLIF_IS_OUTRIF_LSB (0)
#define SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_INDEX_IS_MC_LSB            (SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_INDEX_OUTLIF_IS_OUTRIF_LSB + 1)
#define SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_INDEX_STAMP_NATIVE_VSI_LSB (SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_INDEX_IS_MC_LSB + 1)
#define SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_INDEX_FWD_CODE_LSB         (SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_INDEX_STAMP_NATIVE_VSI_LSB + 1)

/* get the index according to index fields */
#define SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_INDEX_GET(fwd_code, stamp_native_vsi, is_mc, outlif_is_outrif) \
         (((outlif_is_outrif) << (SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_INDEX_OUTLIF_IS_OUTRIF_LSB)) \
         | ((is_mc) << (SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_INDEX_IS_MC_LSB)) \
         | ((stamp_native_vsi) << (SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_INDEX_STAMP_NATIVE_VSI_LSB)) \
         | ((fwd_code) << (SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_INDEX_FWD_CODE_LSB))) 


/* get the offset of the register according to index */
#define SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_OFFSET_GET(fwd_code, stamp_native_vsi, is_mc, outlif_is_outrif) \
         (SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_PPH_VSI_OUTRIF_SOURCE_NOF_BITS * SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_INDEX_GET(fwd_code, stamp_native_vsi, is_mc, outlif_is_outrif))



 /* - set configuration mapping table: PPH-VSI-Source:
  *   Purpose: HW selection of the appropriate value to set in PPH-base field: "VSI/OUT_RIF/IN_RIF" in Jericho.
  *    In arad, this field was: VRF/VSI.
  *    If jericho's system headers is configured in arad mode, this configuration table configure VRF/VSI. 
  * - set outRif range configuration
  *   Purpose: Part of HW selection of the appropriate value to set in PPH-base field: "VSI/OUT_RIF/IN_RIF":
  *            set outRif range for outLif. if outLif in outRif range then outLif-is-outRif (see mapping table PHP-VSI-Source)
  *            outRif range is initialized by soc property */ 
soc_error_t
soc_jer_lbp_init(SOC_SAND_IN int unit) {
    uint32
       res = SOC_SAND_OK, 
       system_header_mode; 

    /* register content */
    soc_reg_above_64_val_t pph_vsi_source; 

    /* variables for the index */
    uint32 fwd_codes_IPvX_XC[4] = {ARAD_PP_FWD_CODE_IPV4_UC, ARAD_PP_FWD_CODE_IPV4_MC, ARAD_PP_FWD_CODE_IPV6_UC, ARAD_PP_FWD_CODE_IPV6_MC}; 
    uint32 boolean_values[2] = {0,1}; 
    uint32 fwd_codes_index, stamp_native_vsi_index, is_mc_index, outlif_is_outrif_index; 
    uint32 fwd_code; 

    uint32 value; 

    SOC_SAND_INIT_ERROR_DEFINITIONS(0); 

    SOC_REG_ABOVE_64_CLEAR(pph_vsi_source);

    /* set configuration mapping table: PPH-VSI-Source */


    /* get system header mode */    
    res = soc_reg_above_64_field32_read(unit, (SOC_IS_JERICHO(unit)? ECI_GLOBAL_SYS_HEADER_CFGr: ECI_GLOBALFr), REG_PORT_ANY, 0, SYSTEM_HEADERS_MODEf, &system_header_mode);
    SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);


    /* 
     * system headers in jericho mode: 
     * ----------------------------------------------------------------------------------------------------------------- 
     * |                         INDEX                        ||       VALUE             |    Applications             | 
     * ----------------------------------------------------------------------------------------------------------------- 
     * |  Forwarding code | stamp-     |  is-MC  |  OutLif-   ||  pph-vsi-outRif-Source  |                             | 
     * |      (4b)        | native-vsi |         |  is-outRif ||        (3b)             |                             | 
     * ----------------------------------------------------------------------------------------------------------------- 
     * |    X             |  1         |   X     |     X      ||      Native-VSI         | ROO, IPoE with FEC format A |       
     * ----------------------------------------------------------------------------------------------------------------- 
     * |  IPvX-XC         |  0         |   0     |     1      ||      OutLif(outRif)     | IPoE with !FEC format A     | 
     * ----------------------------------------------------------------------------------------------------------------- 
     * |  IPvX-XC         |  0         |   0     |     0      ||      -1                 | Routing into Tunnel         | 
     * ----------------------------------------------------------------------------------------------------------------- 
     * |  IPvX-XC         |  0         |   1     |     X      ||      inRif              | MC IP                       | 
     * ----------------------------------------------------------------------------------------------------------------- 
     * |  !(IPvX-XC)      |  0         |   X     |     X      ||      VSI                | No routing                  | 
     * ----------------------------------------------------------------------------------------------------------------- 
     */ 
    if (system_header_mode == ARAD_PP_SYSTEM_HEADERS_MODE_JERICHO) {
        /* ----------------------------------------------------------------------------------- 
         * |  Forwarding code | stamp-     |  is-MC  |  OutLif-   ||  pph-vsi-outRif-Source  | 
         * |      (4b)        | native-vsi |         |  is-outRif ||        (3b)             | 
         * ----------------------------------------------------------------------------------- 
         * |    X             |  1         |   X     |     X      ||      Native-VSI         |
         * -----------------------------------------------------------------------------------  */

        value = SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_PPH_VSI_OUTRIF_SOURCE_NATIVE_VSI; 

        for (fwd_code=0; fwd_code<ARAD_PP_FWD_CODE_NOF_FWD_CODE; fwd_code++) {
            for (is_mc_index=0; is_mc_index<2; is_mc_index++) {
                for (outlif_is_outrif_index=0; outlif_is_outrif_index<2; outlif_is_outrif_index++) {
                  SHR_BITCOPY_RANGE(pph_vsi_source,                                                                                                                 
                                    SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_OFFSET_GET(fwd_code, 1, boolean_values[is_mc_index], boolean_values[outlif_is_outrif_index]), 
                                    &value, 0, SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_PPH_VSI_OUTRIF_SOURCE_NOF_BITS);                                                                                                                  
                }
            }
        }

        /* ----------------------------------------------------------------------------------- 
         * |  Forwarding code | stamp-     |  is-MC  |  OutLif-   ||  pph-vsi-outRif-Source  | 
         * |      (4b)        | native-vsi |         |  is-outRif ||        (3b)             | 
         * ----------------------------------------------------------------------------------- 
         * |  IPvX-XC         |  0         |   0     |     1      ||      OutLif(outRif)     |
         * ----------------------------------------------------------------------------------- */

        value = SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_PPH_VSI_OUTRIF_SOURCE_OUTLIF;

        for (fwd_codes_index=0; fwd_codes_index<4; fwd_codes_index++) {
          SHR_BITCOPY_RANGE(pph_vsi_source,                                                                            
                            SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_OFFSET_GET(fwd_codes_IPvX_XC[fwd_codes_index], 0, 0, 1), 
                            &value, 0, SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_PPH_VSI_OUTRIF_SOURCE_NOF_BITS);                                                                             
        }

        /* ----------------------------------------------------------------------------------- 
         * |  Forwarding code | stamp-     |  is-MC  |  OutLif-   ||  pph-vsi-outRif-Source  | 
         * |      (4b)        | native-vsi |         |  is-outRif ||        (3b)             | 
         * ----------------------------------------------------------------------------------- 
         * |  IPvX-XC         |  0         |   0     |     0      ||      -1                 | 
         * ----------------------------------------------------------------------------------- */
        value = SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_PPH_VSI_OUTRIF_SOURCE_MINUS_1; 

        for (fwd_codes_index=0; fwd_codes_index<4; fwd_codes_index++) {
          SHR_BITCOPY_RANGE(pph_vsi_source,                                                                            
                            SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_OFFSET_GET(fwd_codes_IPvX_XC[fwd_codes_index], 0, 0, 0), 
                            &value, 0, SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_PPH_VSI_OUTRIF_SOURCE_NOF_BITS);                                                                             
        }

        /* ----------------------------------------------------------------------------------- 
         * |  Forwarding code | stamp-     |  is-MC  |  OutLif-   ||  pph-vsi-outRif-Source  | 
         * |      (4b)        | native-vsi |         |  is-outRif ||        (3b)             | 
         * ----------------------------------------------------------------------------------- 
         * |  IPvX-XC         |  0         |   1     |     X      ||      inRif              | 
         * ----------------------------------------------------------------------------------- */
        value = SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_PPH_VSI_OUTRIF_SOURCE_INRIF; 

        for (fwd_codes_index=0; fwd_codes_index<4; fwd_codes_index++) {
            for (outlif_is_outrif_index = 0; outlif_is_outrif_index<2; outlif_is_outrif_index++) {
              SHR_BITCOPY_RANGE(pph_vsi_source,                                                                                                                 
                                SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_OFFSET_GET(fwd_codes_IPvX_XC[fwd_codes_index], 0, 1, boolean_values[outlif_is_outrif_index]), 
                                &value, 0, SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_PPH_VSI_OUTRIF_SOURCE_NOF_BITS);                                                                                                                  
            }
        }

        /* ----------------------------------------------------------------------------------- 
         * |  Forwarding code | stamp-     |  is-MC  |  OutLif-   ||  pph-vsi-outRif-Source  | 
         * |      (4b)        | native-vsi |         |  is-outRif ||        (3b)             | 
         * ----------------------------------------------------------------------------------- 
         * |  !(IPvX-XC)      |  0         |   X     |     X      ||      VSI                | 
         * ----------------------------------------------------------------------------------- */
        value = SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_PPH_VSI_OUTRIF_SOURCE_VSI; 


        for (fwd_code=0; fwd_code<ARAD_PP_FWD_CODE_NOF_FWD_CODE; fwd_code++) {
            if (fwd_code != ARAD_PP_FWD_CODE_IPV4_UC && fwd_code != ARAD_PP_FWD_CODE_IPV4_MC
                && fwd_code != ARAD_PP_FWD_CODE_IPV6_UC && fwd_code != ARAD_PP_FWD_CODE_IPV6_MC) {
               for (is_mc_index=0; is_mc_index<2; is_mc_index++) {
                   for (outlif_is_outrif_index=0; outlif_is_outrif_index<2; outlif_is_outrif_index++) {
                     SHR_BITCOPY_RANGE(pph_vsi_source,                                                                                                                 
                                       SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_OFFSET_GET(fwd_code, 0, boolean_values[is_mc_index], boolean_values[outlif_is_outrif_index]), 
                                       &value, 0, SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_PPH_VSI_OUTRIF_SOURCE_NOF_BITS);                                                                                                                  
                   }
               }
            }
        }
    } 

    /* 
     * system headers in arad mode:
     * 
     * ----------------------------------------------------------------------------------- 
     * |                         INDEX                        ||       VALUE             | 
     * ----------------------------------------------------------------------------------- 
     * |  Forwarding code | stamp-     |  is-MC  |  OutLif-   ||  pph-vsi-outRif-Source  | 
     * |      (4b)        | native-vsi |         |  is-outRif ||        (3b)             | 
     * ----------------------------------------------------------------------------------- 
     * |  !(IPvX-XC)      |  0         |   X     |     X      ||      VSI                | 
     * -----------------------------------------------------------------------------------
     * |                other cases                           ||      VRF                | 
     * -----------------------------------------------------------------------------------  
     */ 
    else if (system_header_mode == ARAD_PP_SYSTEM_HEADERS_MODE_ARAD) {

        /* ----------------------------------------------------------------------------------- 
         * |  Forwarding code | stamp-     |  is-MC  |  OutLif-   ||  pph-vsi-outRif-Source  | 
         * |      (4b)        | native-vsi |         |  is-outRif ||        (3b)             | 
         * ----------------------------------------------------------------------------------- 
         * |  !(IPvX-XC)      |  0         |   X     |     X      ||      VSI                | 
         * ----------------------------------------------------------------------------------- */

        value = SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_PPH_VSI_OUTRIF_SOURCE_VSI; 

        for (fwd_code=0; fwd_code<ARAD_PP_FWD_CODE_NOF_FWD_CODE; fwd_code++) {
            if (fwd_code != ARAD_PP_FWD_CODE_IPV4_UC && fwd_code != ARAD_PP_FWD_CODE_IPV4_MC
                && fwd_code != ARAD_PP_FWD_CODE_IPV6_UC && fwd_code != ARAD_PP_FWD_CODE_IPV6_MC) {
               for (is_mc_index=0; is_mc_index<2; is_mc_index++) {
                   for (outlif_is_outrif_index=0; outlif_is_outrif_index<2; outlif_is_outrif_index++) {
                     SHR_BITCOPY_RANGE(pph_vsi_source,                                                                                                                 
                                       SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_OFFSET_GET(fwd_code, 0, boolean_values[is_mc_index], boolean_values[outlif_is_outrif_index]), 
                                       &value, 0, SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_PPH_VSI_OUTRIF_SOURCE_NOF_BITS);                                                                                                                  
                   }
               }
            }
        }
        /*
         * ----------------------------------------------------------------------------------- 
         * |  Forwarding code | stamp-     |  is-MC  |  OutLif-   ||  pph-vsi-outRif-Source  | 
         * |      (4b)        | native-vsi |         |  is-outRif ||        (3b)             | 
         * ----------------------------------------------------------------------------------- 
         * |  !(IPvX-XC)      |  1         |   X     |     X      ||      VRF                | 
         * -----------------------------------------------------------------------------------
         */
        value = SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_PPH_VSI_OUTRIF_SOURCE_VRF; 

        for (fwd_code=0; fwd_code<ARAD_PP_FWD_CODE_NOF_FWD_CODE; fwd_code++) {
            if (fwd_code != ARAD_PP_FWD_CODE_IPV4_UC && fwd_code != ARAD_PP_FWD_CODE_IPV4_MC
                && fwd_code != ARAD_PP_FWD_CODE_IPV6_UC && fwd_code != ARAD_PP_FWD_CODE_IPV6_MC) {
               for (is_mc_index=0; is_mc_index<2; is_mc_index++) {
                   for (outlif_is_outrif_index=0; outlif_is_outrif_index<2; outlif_is_outrif_index++) {
                     SHR_BITCOPY_RANGE(pph_vsi_source,                                                                                                                 
                                       SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_OFFSET_GET(fwd_code, 1, boolean_values[is_mc_index], boolean_values[outlif_is_outrif_index]), 
                                       &value, 0, SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_PPH_VSI_OUTRIF_SOURCE_NOF_BITS);                                                                                                                  
                   }
               }
            }
        }

        /*
         * ----------------------------------------------------------------------------------- 
         * |  Forwarding code | stamp-     |  is-MC  |  OutLif-   ||  pph-vsi-outRif-Source  | 
         * |      (4b)        | native-vsi |         |  is-outRif ||        (3b)             | 
         * ----------------------------------------------------------------------------------- 
         * |  IPvX-XC         |  X         |   X     |     X      ||      VRF                | 
         * -----------------------------------------------------------------------------------
         */

        for (fwd_codes_index=0; fwd_codes_index<4; fwd_codes_index++) {
            for (stamp_native_vsi_index=0; stamp_native_vsi_index<2; stamp_native_vsi_index++) {
                for (is_mc_index=0; is_mc_index<2; is_mc_index++) {
                    for (outlif_is_outrif_index=0; outlif_is_outrif_index<2; outlif_is_outrif_index++) {
                      SHR_BITCOPY_RANGE(pph_vsi_source,                                                                       
                                        SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_OFFSET_GET(fwd_codes_IPvX_XC[fwd_codes_index],      
                                                                                     boolean_values[stamp_native_vsi_index],  
                                                                                     boolean_values[is_mc_index],             
                                                                                     boolean_values[outlif_is_outrif_index]), 
                                        &value, 0, SOC_JER_PP_LBP_INIT_PPH_VSI_SOURCE_PPH_VSI_OUTRIF_SOURCE_NOF_BITS);                                                                        
                    }
                }
            }
        }
    }

    /* set in HW configuration mapping table*/
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, WRITE_IHB_PPH_VSI_SOURCEr(unit, REG_PORT_ANY, pph_vsi_source)); 


    /* set outRif range configuration */
     
    /* set in HW */
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_IHB_OUT_RIF_RANGEr(unit, REG_PORT_ANY, SOC_DPP_CONFIG(unit)->l3.nof_rifs - 1)); 
    
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_jer_lpb_init()", 0, 0); 
 
}



/* init configuration mapping table: cfg-php-fhei-fwd-size.
*   format: 
* ------------------------------------------------------------------------------------------------------| 
* |                         INDEX                            ||       VALUE     |    Note               | 
* ------------------------------------------------------------------------------------------------------| 
* |   Forwarding-Offset   | inLif ==0  |  Forwarding code    ||  fhei size      |                       | 
* |   -Index (3b)         |    (1b)    |      (4b)           ||     (2b)        |                       | 
* ------------------------------------------------------------------------------------------------------| 
*/

#define SOC_JER_PP_LBP_INIT_PPH_FHEI_FWD_SIZE_INDEX_FWD_OFFSET_INDEX_NOF_BITS (3)  
#define SOC_JER_PP_LBP_INIT_PPH_FHEI_FWD_SIZE_INDEX_INLIF_EQUAL_0_NOF_BITS    (1)

#define SOC_JER_PP_LBP_INIT_PPH_FHEI_FWD_SIZE_INDEX_FWD_OFFSET_INDEX_MAX      ((1 << SOC_JER_PP_LBP_INIT_PPH_FHEI_FWD_SIZE_INDEX_FWD_OFFSET_INDEX_NOF_BITS) - 1) 

#define SOC_JER_PP_LBP_INIT_PPH_FHEI_FWD_SIZE_INDEX_FWD_CODE             (0)
#define SOC_JER_PP_LBP_INIT_PPH_FHEI_FWD_SIZE_INDEX_INLIF_EQUAL_0_LSB    (SOC_JER_PP_LBP_INIT_PPH_FHEI_FWD_SIZE_INDEX_FWD_CODE + 4)
#define SOC_JER_PP_LBP_INIT_PPH_FHEI_FWD_SIZE_INDEX_FWD_OFFSET_INDEX_LSB (SOC_JER_PP_LBP_INIT_PPH_FHEI_FWD_SIZE_INDEX_INLIF_EQUAL_0_LSB + 1)

/* get the index according to index fields */
#define SOC_JER_PP_LBP_INIT_PPH_FHEI_FWD_SIZE_INDEX_GET(fwd_offset_index, inLif_equal_0, fwd_code) \
    (((fwd_offset_index) << (SOC_JER_PP_LBP_INIT_PPH_FHEI_FWD_SIZE_INDEX_FWD_OFFSET_INDEX_LSB)) \
    | ((inLif_equal_0) << (SOC_JER_PP_LBP_INIT_PPH_FHEI_FWD_SIZE_INDEX_INLIF_EQUAL_0_LSB)) \
    | ((fwd_code) << (SOC_JER_PP_LBP_INIT_PPH_FHEI_FWD_SIZE_INDEX_FWD_CODE)))



/* get the offset of the register according to index */
#define SOC_JER_PP_LBP_INIT_PPH_FHEI_FWD_SIZE_OFFSET_GET(fwd_offset_index, inLif_equal_0, fwd_code) \
         (ARAD_IHB_PPH_FHEI_FWD_SIZE_NOF_BITS * SOC_JER_PP_LBP_INIT_PPH_FHEI_FWD_SIZE_INDEX_GET(fwd_offset_index, inLif_equal_0, fwd_code))

/* init configuration mapping table: cfg-php-fhei-fwd-size.
 * Purpose: HW selection of the appropriate value to set in PPH-base field: "FHEI size"
 */
soc_error_t
soc_jer_php_fhei_fwd_size_init(SOC_SAND_IN int unit) {
     uint32
        res = SOC_SAND_OK,
        system_header_mode; 

     /* register content */
     soc_reg_above_64_val_t pph_fhei_fwd_size; 

     /* variables for the index */
     uint32 fwd_code_index, inLif_equal_0_index, fwd_offset_index; 
     uint32 fwd_codes_Eth_IPvX_XC_Trill_Trap_Eth_After_Term[8] = {
         ARAD_PP_FWD_CODE_ETHERNET,ARAD_PP_FWD_CODE_IPV4_UC, ARAD_PP_FWD_CODE_IPV4_MC, ARAD_PP_FWD_CODE_IPV6_UC, 
         ARAD_PP_FWD_CODE_IPV6_MC, ARAD_PP_FWD_CODE_TRILL, ARAD_PP_FWD_CODE_CPU_TRAP, ARAD_PP_FWD_CODE_ETHERNET_AFTER_TERMINATION}; 
     uint32 fwd_codes_IPvX_MC[2] = {ARAD_PP_FWD_CODE_IPV4_MC, ARAD_PP_FWD_CODE_IPV6_MC}; 
     uint32 boolean_values[2] = {0,1}; 
     uint32 fwd_code; 

     uint32 value; 

     SOC_SAND_INIT_ERROR_DEFINITIONS(0); 

     SOC_REG_ABOVE_64_CLEAR(pph_fhei_fwd_size); 


     /* get system header mode */
     res = soc_reg_above_64_field32_read(unit, (SOC_IS_JERICHO(unit)? ECI_GLOBAL_SYS_HEADER_CFGr: ECI_GLOBALFr), REG_PORT_ANY, 0, SYSTEM_HEADERS_MODEf, &system_header_mode);
     SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

     /* system headers in arad mode */

     /* ------------------------------------------------------------------------------------------------------| 
      * |                        INDEX                             ||       VALUE     |    Note               | 
      * ------------------------------------------------------------------------------------------------------| 
      * | Forwarding-Offset   | inLif ==0  |  Forwarding code      ||  fhei size      |                       | 
      * | -Index (3b)         |    (1b)    |      (4b)             ||     (2b)        |                       | 
      * ------------------------------------------------------------------------------------------------------| 
      * |                     |            | Eth, IPvX-XC,         ||                 |                       |  
      * |        X            |  X         | Trill, CPU-trap,      ||      3B         | Arad value            | 
      * |                     |            | eth-after-termination ||                 |                       | 
      * ------------------------------------------------------------------------------------------------------|
      * |        X            |  X         | others                ||      0B         | Arad value            | 
      * ------------------------------------------------------------------------------------------------------| */
                                             

     /* ------------------------------------------------------------------------------------------------------| 
      * |                        INDEX                             ||       VALUE     |    Note               | 
      * ------------------------------------------------------------------------------------------------------| 
      * | Forwarding-Offset   | inLif ==0  |  Forwarding code      ||  fhei size      |                       | 
      * | -Index (3b)         |    (1b)    |      (4b)             ||     (2b)        |                       | 
      * ------------------------------------------------------------------------------------------------------| 
      * |                     |            | Eth, IPvX-XC,         ||                 |                       |  
      * |        X            |  X         | Trill, CPU-trap,      ||      3B         | Arad value            | 
      * |                     |            | eth-after-termination ||                 |                       | 
      * ------------------------------------------------------------------------------------------------------| */

     value = ARAD_IHB_PPH_FHEI_FWD_SIZE_3B; 

     for (fwd_code_index=0; fwd_code_index<8; fwd_code_index++) {
         for(inLif_equal_0_index = 0; inLif_equal_0_index < 2; inLif_equal_0_index++) {
             for (fwd_offset_index=0;fwd_offset_index <= SOC_JER_PP_LBP_INIT_PPH_FHEI_FWD_SIZE_INDEX_FWD_OFFSET_INDEX_MAX; fwd_offset_index++) {
                 SHR_BITCOPY_RANGE(pph_fhei_fwd_size, 
                                   SOC_JER_PP_LBP_INIT_PPH_FHEI_FWD_SIZE_OFFSET_GET(
                                        fwd_offset_index,
                                        boolean_values[inLif_equal_0_index], 
                                        fwd_codes_Eth_IPvX_XC_Trill_Trap_Eth_After_Term[fwd_code_index]), 
                                   &value, 0, ARAD_IHB_PPH_FHEI_FWD_SIZE_NOF_BITS); 
             }
         }
     }

     /* ------------------------------------------------------------------------------------------------------| 
      * |                         INDEX                            ||       VALUE     |    Note               | 
      * ------------------------------------------------------------------------------------------------------| 
      * |  Forwarding-Offset   | inLif ==0  |  Forwarding code     ||  fhei size      |                       | 
      * |  -Index (3b)         |    (1b)    |      (4b)            ||     (2b)        |                       | 
      * ------------------------------------------------------------------------------------------------------| 
      * |         X            |  X         | others               ||      0B         | Arad value            | 
      * ------------------------------------------------------------------------------------------------------| */
     value = ARAD_IHB_PPH_FHEI_FWD_SIZE_0B; 

     for (fwd_code=0; fwd_code<ARAD_PP_FWD_CODE_NOF_FWD_CODE; fwd_code++) {
         if ((fwd_code != ARAD_PP_FWD_CODE_ETHERNET) && (fwd_code != ARAD_PP_FWD_CODE_IPV4_UC) 
             && (fwd_code != ARAD_PP_FWD_CODE_IPV4_MC) && (fwd_code != ARAD_PP_FWD_CODE_IPV6_UC)
             && (fwd_code != ARAD_PP_FWD_CODE_IPV6_MC) && (fwd_code != ARAD_PP_FWD_CODE_TRILL)
             && (fwd_code != ARAD_PP_FWD_CODE_CPU_TRAP) && (fwd_code != ARAD_PP_FWD_CODE_ETHERNET_AFTER_TERMINATION)) {
            for(inLif_equal_0_index = 0; inLif_equal_0_index < 2; inLif_equal_0_index++) {
                for (fwd_offset_index=0;fwd_offset_index < SOC_JER_PP_LBP_INIT_PPH_FHEI_FWD_SIZE_INDEX_FWD_OFFSET_INDEX_MAX; fwd_offset_index++) {
                    SHR_BITCOPY_RANGE(pph_fhei_fwd_size, 
                                      SOC_JER_PP_LBP_INIT_PPH_FHEI_FWD_SIZE_OFFSET_GET(
                                         fwd_offset_index, boolean_values[inLif_equal_0_index], fwd_code), 
                                      &value, 0, ARAD_IHB_PPH_FHEI_FWD_SIZE_NOF_BITS); 
                }
            }
         }
     }

     /* for system header in jericho mode, juste like Arad mode. Only change for IPvx-MC, so we'll use as basis arad mode config
        and overwrite IPvX-MC cases */
     if (system_header_mode == ARAD_PP_SYSTEM_HEADERS_MODE_JERICHO) {

         /* 
          * system headers in jericho mode: 
          * ------------------------------------------------------------------------------------------------------------------|  
          * |                        INDEX                             ||       VALUE     |    Note                           | 
          * ------------------------------------------------------------------------------------------------------------------|  
          * |  Forwarding-Offset  | inLif ==0  |   Forwarding code     ||  fhei size      |                                   | 
          * |  -Index (3b)        |    (1b)    |       (4b)            ||     (2b)        |                                   | 
          * ------------------------------------------------------------------------------------------------------------------|   
          * |         2           |  1         |   IPvX-MC             ||      3B         | Routing over dot1q (like Arad)    | 
          * ------------------------------------------------------------------------------------------------------------------|    
          * |         3           |  X         |   IPvX-MC             ||      3B         | Routing out of tunnel (like Arad) | 
          * ------------------------------------------------------------------------------------------------------------------|  
          * |         4           |  X         |   IPvX-MC             ||      8B         | ROO                               | 
          * ------------------------------------------------------------------------------------------------------------------|  
          * |         2           |  0         |   IPvX-MC             ||      8B         | Routing over VPN                  | 
          * ------------------------------------------------------------------------------------------------------------------|    */

         for (fwd_code_index=0;fwd_code_index<2;fwd_code_index++) {
             
            /* ------------------------------------------------------------------------------------------------------| 
             * |                           INDEX                          ||       VALUE     |    Note               | 
             * ------------------------------------------------------------------------------------------------------| 
             * |    Forwarding-Offset   | inLif ==0  |  Forwarding code   ||  fhei size      |                       | 
             * |    -Index (3b)         |    (1b)    |      (4b)          ||     (2b)        |                       | 
             * ------------------------------------------------------------------------------------------------------| 
             * |           4            |  X         |  IPvX-MC           ||      8B         | ROO                   | 
             * ------------------------------------------------------------------------------------------------------| */

             value = ARAD_IHB_PPH_FHEI_FWD_SIZE_8B; 
             for (inLif_equal_0_index=0;inLif_equal_0_index<2;inLif_equal_0_index++) {
                 SHR_BITCOPY_RANGE(pph_fhei_fwd_size, 
                                   SOC_JER_PP_LBP_INIT_PPH_FHEI_FWD_SIZE_OFFSET_GET(
                                      4, boolean_values[inLif_equal_0_index], fwd_codes_IPvX_MC[fwd_code_index]), 
                                   &value, 0, ARAD_IHB_PPH_FHEI_FWD_SIZE_NOF_BITS); 
             }
            /* ------------------------------------------------------------------------------------------------------| 
             * |                        INDEX                             ||       VALUE     |    Note               | 
             * ------------------------------------------------------------------------------------------------------| 
             * | Forwarding-Offset   | inLif ==0  |  Forwarding code      ||  fhei size      |                       | 
             * | -Index (3b)         |    (1b)    |      (4b)             ||     (2b)        |                       | 
             * ------------------------------------------------------------------------------------------------------| 
             * |        2            |  0         |  IPvX-MC              ||      8B         | Routing over VPN      | 
             * ------------------------------------------------------------------------------------------------------| */
             SHR_BITCOPY_RANGE(pph_fhei_fwd_size, 
                                  SOC_JER_PP_LBP_INIT_PPH_FHEI_FWD_SIZE_OFFSET_GET(
                                  2, 0, fwd_codes_IPvX_MC[fwd_code_index]), 
                               &value, 0, ARAD_IHB_PPH_FHEI_FWD_SIZE_NOF_BITS); 
         }
     }

    /* set in HW configuration mapping table*/
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, WRITE_IHB_PPH_FHEI_FWD_SIZEr(unit, REG_PORT_ANY, pph_fhei_fwd_size));

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_jer_php_fhei_fwd_size_init()", 0, 0); 
}


/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>
