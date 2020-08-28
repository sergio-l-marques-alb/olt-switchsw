/* $Id: jer_parser.c
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
*/

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_HEADERS

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/mcm/memregs.h>
#include <soc/mem.h>
#include <shared/bsl.h>
#include <sal/compiler.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/ARAD/arad_framework.h>
#include <soc/dpp/ARAD/arad_parser.h>
#include <soc/dpp/JER/jer_parser.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/arad_reg_access.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_ptp.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>
/* } */

/**********************************
 * MACRO DATA & HANDLING ROUTINES *
 **********************************/
/* { */
DPP_MACRO_E   macro_map[SOC_MAX_NUM_DEVICES][JER_CUSTOM_MACRO_NOF];

static dpp_macro_t macros[] =
{
 /* SW Macro              String        HW Macro          TM Flag */
   {DPP_MACRO_ETH,        "Eth",        JER_MACRO_ETH,    FALSE},
   {DPP_MACRO_IPv4,       "IPv4",       JER_MACRO_IPv4,   FALSE},
   {DPP_MACRO_IPv6,       "IPv6",       JER_MACRO_IPv6,   FALSE},
   {DPP_MACRO_MPLS,       "MPLS",       JER_MACRO_MPLS,   FALSE},
   {DPP_MACRO_TRILL,      "TRILL",      JER_MACRO_NA,     FALSE},
   {DPP_MACRO_FCoE1,      "FCoE1",      JER_MACRO_NA,     FALSE},
   {DPP_MACRO_FCoE2,      "FCoE2",      JER_MACRO_NA,     FALSE},
   {DPP_MACRO_VxLAN,      "VxLAN",      JER_MACRO_NA,     FALSE},
   {DPP_MACRO_UDP,        "UDP",        JER_MACRO_NA,     FALSE},
   {DPP_MACRO_IPv6Ext1,   "IPv6_Ext1",  JER_MACRO_NA,     FALSE},
   {DPP_MACRO_IPv6Ext2,   "IPv6_Ext2",  JER_MACRO_NA,     FALSE},
   {DPP_MACRO_ADD_TPID,   "Add_TPID",   JER_MACRO_NA,     FALSE},
   {DPP_MACRO_RAW_MPLS,   "RawMPLS",    JER_MACRO_NA,     FALSE},
   {DPP_MACRO_MPLS_CW,    "MPLS_CW",    JER_MACRO_NA,     FALSE},
   {DPP_MACRO_EoE,        "EoE",        JER_MACRO_NA,     FALSE},
   {DPP_MACRO_GTP,        "GTP",        JER_MACRO_NA,     FALSE},
   {DPP_MACRO_UDP_TUNNEL, "UDP_Tunnel", JER_MACRO_NA,     FALSE},
   {DPP_MACRO_TM,         "TM",         JER_MACRO_TM,     TRUE},
   {DPP_MACRO_TM_EXT,     "TM_EXT",     JER_MACRO_TM_EXT, TRUE},
   {DPP_MACRO_PPPoES, 	  "PPPoES",	    JER_MACRO_NA, 	  FALSE},
   {DPP_MACRO_L2TP1,	  "L2TP1", 	    JER_MACRO_NA, 	  FALSE},
   {DPP_MACRO_L2TP2,	  "L2TP2", 	    JER_MACRO_NA, 	  FALSE}
};

static dpp_macro_t *jer_parser_macro_get(DPP_MACRO_E macro_id)
{
    int i_mcr;
    for(i_mcr = 0; i_mcr < (sizeof(macros) / sizeof(dpp_macro_t)); i_mcr++)
    {
        if(macros[i_mcr].id == macro_id)
            return &macros[i_mcr];
    }
    return NULL;
}

char *jer_parser_macro_string_by_sw(DPP_MACRO_E macro_id)
{
    dpp_macro_t *macro;
    if((macro = jer_parser_macro_get(macro_id)) == NULL)
    {
        LOG_ERROR(BSL_LS_SOC_PKT,(BSL_META_U(0, "Invalid MACRO ID:%d\n"), macro_id));
        return NULL;
    }
    return macro->name;
}

char *jer_parser_macro_string_by_hw(int unit, uint32 macro_sel, int tm_flag)
{
    dpp_macro_t *macro = NULL;

    /* First look in static list - HardCoded Macro or TM Macro, which are also statically allocated */
    if((tm_flag == TRUE) || (macro_sel >= JER_CUSTOM_MACRO_NOF))
    {
        int i_mcr;
        for(i_mcr = 0; i_mcr < (sizeof(macros) / sizeof(dpp_macro_t)); i_mcr++)
        {
            if(macros[i_mcr].static_macro_sel == macro_sel)
            {
                macro = &macros[i_mcr];
                break;
            }
        }
    }
    else /* Look in dynamic map for PP Macro */
    {
        macro = jer_parser_macro_get(macro_map[unit][macro_sel]);
    }

    if(macro == NULL)
    {
        LOG_VERBOSE(BSL_LS_SOC_PKT,(BSL_META_U(0, "Invalid Macro Selection:%d(%d)\n"), macro_sel, tm_flag));
        return NULL;
    }

    return macro->name;
}

static uint32 jer_parser_macro_sel_get(int unit, DPP_MACRO_E macro_id)
{
    dpp_macro_t *macro;
    uint32 macro_sel = JER_MACRO_NA;
    int i_ind;

    if((macro = jer_parser_macro_get(macro_id)) == NULL)
    {
        LOG_ERROR(BSL_LS_SOC_PKT,(BSL_META_U(0, "Invalid MACRO ID:%d\n"), macro_id));
        return JER_MACRO_NA;
    }

    if(macro->static_macro_sel != JER_MACRO_NA)
    {
        macro_sel = macro->static_macro_sel;
    }

    for(i_ind = 0; i_ind < JER_CUSTOM_MACRO_NOF; i_ind++)
    {
        if(macro_map[unit][i_ind] == macro_id)
        {
            macro_sel = i_ind;
            break;
        }
    }

    return macro_sel;
}

static int jer_parser_macro_sel_allocate(int unit, DPP_MACRO_E macro_id, uint32 *macro_sel_p)
{
    dpp_macro_t *macro;
    uint32 macro_sel = 0;

    SOCDNX_INIT_FUNC_DEFS;

    if((macro = jer_parser_macro_get(macro_id)) == NULL)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("Invalid Macro ID:%d\n"), macro_id));
    }

    if(macro->static_macro_sel != JER_MACRO_NA)
    {
        macro_sel = macro->static_macro_sel;
    }
    else
    {
        int i_ind;
        for(i_ind = 0; i_ind < JER_CUSTOM_MACRO_NOF; i_ind++)
        {
            if(macro_map[unit][i_ind] == DPP_MACRO_NA)
            {
                macro_map[unit][i_ind] = macro_id;
                macro_sel = i_ind;
                break;
            }
        }

        if(i_ind == JER_CUSTOM_MACRO_NOF)
        {
            SOCDNX_EXIT_WITH_ERR(SOC_E_RESOURCE, (_BSL_SOCDNX_MSG("Failed to allocate Macro:%s\n"), macro->name));
        }
    }

    if(macro_sel_p != NULL)
        *macro_sel_p = macro_sel;

exit:
    SOCDNX_FUNC_RETURN;
}
/* } End of MACRO Handling Routines */

/************************************************
 * Per protocol init and/or if defined routines *
 ************************************************/
/* { */
/*
 * Following macro serves as a gatekeeper for initialization routines below.
 * It allows performing it only once, then it will return status that will sign for calling routines, that
 * sequence was already successfully invoked
 */
static int jer_cb_map[DPP_CB_NOF];

#define DPP_LEAF_CB_GATE(cb_enum)                   \
        {                                           \
            if(jer_cb_map[cb_enum] == FALSE)        \
                jer_cb_map[cb_enum] = TRUE;         \
            else                                    \
                return SOC_E_EXISTS;                \
        }

static int jer_parser_add_tpid(int unit)
{
    ARAD_PP_IHP_PARSER_CUSTOM_MACRO_PARAMETERS_TBL_DATA ihp_parser_custom_macro_parameters_tbl_data;
    ARAD_PP_IHP_PARSER_ETH_PROTOCOLS_TBL_DATA           ihp_parser_eth_protocols_tbl_data;
    uint32                                              custom_macro;

    SOCDNX_INIT_FUNC_DEFS;

    /* Configure Additional TPID custom macros only if enabled */
    if(!soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "additional_tpid", 0))
    {
        return SOC_E_NOT_FOUND;
    }

    DPP_LEAF_CB_GATE(DPP_CB_ADD_TPID);

    SOCDNX_IF_ERR_EXIT(jer_parser_macro_sel_allocate(unit, DPP_MACRO_ADD_TPID, &custom_macro));

    /* Configure relevant Ethertype:
     * Need to configure ethertype 0x88a8 - actually this value is
     * an additional (global) TPID. In case the outer-tag equals
     * a global TPID, it is not recognized by the Ethernet macro.
     * The Ethernet macro will then parse it as an ethertype and
     * compare it to Ethernet protocols. Then it should hit the new
     * ethernet protocol we add. It's added as a user-defined
     * ethernet protocol, and if there's a match, the following
     * custom macro is ran, which sets the outer-tag to be TPID-2
     * and calculates the next protocol.
     *
     * Ethernet macro will stop after outer-tag, thinking that
     * it is ethertype:
     *
     *    +-----+------+------------+------+-------------+
     *    | DA  |  SA  | Outer-Tag  | VID  |  Ethertype  |
     *    +-----+------+------------+------+-------------+
     *
     * Note:
     * This macro supports SINGLE-TAG PACKETS ONLY.
     */
    /* 0x88a8 = Additional TPID value - not real protocol, used as WA */
    ihp_parser_eth_protocols_tbl_data.eth_type_protocol = ARAD_PARSER_ETHER_PROTO_4_ADD_TPID_TYPE;
    ihp_parser_eth_protocols_tbl_data.eth_sap_protocol = 0;
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihp_parser_eth_protocols_tbl_set_unsafe(unit, ARAD_PARSER_ETHER_PROTO_4_ADD_TPID,
                                                                                  &ihp_parser_eth_protocols_tbl_data));

    ihp_parser_custom_macro_parameters_tbl_data.cstm_word_select       = 0;           /* take next header */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_mask_left         = 16;          /* mask anything else */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_mask_right        = 0;           /* */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_comparator_mode   = 4;           /* Compare 16 lsb to Ethernet values */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_comparator_mask   = 0x7fff;      /* Compare all values */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_condition_select  = 0;           /* */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_shift_sel         = 0;           /* If condition holds use shift-A else shift-B */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_shift_a           = 4;           /* add 4 bytes to previous header size */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_shift_b           = 4;           /* add 4 bytes to previous header size */
    ihp_parser_custom_macro_parameters_tbl_data.augment_previous_stage = 1;           /* don't advance header size or header qualifier */
    ihp_parser_custom_macro_parameters_tbl_data.qualifier_shift        = 14;          /* change protocol to be new ethertype */
    ihp_parser_custom_macro_parameters_tbl_data.qualifier_or_mask      = 0x10;        /* set Outer-Tag bits 4:3 of the Ethernet-Tag-Format to 2 0b10 (TPID 2) */
    ihp_parser_custom_macro_parameters_tbl_data.qualifier_and_mask     = 0x7f7;       /* set Outer-Tag bits 4:3 of the Ethernet-Tag-Format to 2 0b10 (TPID 2) */
    ihp_parser_custom_macro_parameters_tbl_data.header_qualifier_mask  = 0x798;       /* Allow to change protocol (bits 10:7) and outer-tag (bits 4:3) */

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihp_parser_custom_macro_parameters_tbl_set_unsafe(unit, custom_macro,
                                                                        &ihp_parser_custom_macro_parameters_tbl_data));

exit:
    SOCDNX_FUNC_RETURN;
}


uint32 jer_parser_add_gtp_teid_word_map(int unit)
{
    uint32
    res = SOC_SAND_OK, tbl_data;
    int index;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    /* TEID exists if GTPv1 or GTPv2 TEID flag enabled
    meaning GTP.header.version=1 also can be writen GTP.header[0-8]= 001xxxxx (values in table 32 -63)
    and  (GTP.header.version=2 && GTP.header.t_f=1 )also can be writen GTP.header[0-8]= 010x1xxx 
    (values	72 - 79 and 88- 95)
    Therefore add value 2 to these entires	*/
   tbl_data = 2;
   index = 32;	
   while (index <= 63 )
   {
       SOC_SAND_SOC_IF_ERROR_RETURN(res, 32, exit, WRITE_IHP_PARSER_CUSTOM_MACRO_WORD_MAPm(unit, MEM_BLOCK_ANY, index, &tbl_data));
       index++;
   }
   index = 72;
   while (index <= 79 )
   {
       SOC_SAND_SOC_IF_ERROR_RETURN(res, 32, exit, WRITE_IHP_PARSER_CUSTOM_MACRO_WORD_MAPm(unit, MEM_BLOCK_ANY, index, &tbl_data));
       index++;
   }
   index = 88;
   while (index <= 95 )
   {
       SOC_SAND_SOC_IF_ERROR_RETURN(res, 32, exit, WRITE_IHP_PARSER_CUSTOM_MACRO_WORD_MAPm(unit, MEM_BLOCK_ANY, index, &tbl_data));
       index++;
   }
   
    exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in dpp_parser_add_gtp_teid_word_map()", 0, 0);

}




static soc_error_t jer_parser_eoe(SOC_SAND_IN int unit)
{
    ARAD_PP_IHP_PARSER_CUSTOM_MACRO_PARAMETERS_TBL_DATA ihp_parser_custom_macro_parameters_tbl_data;
    ARAD_PP_IHP_PARSER_ETH_PROTOCOLS_TBL_DATA           ihp_parser_eth_protocols_tbl_data;
    uint32                                              custom_macro;

    SOCDNX_INIT_FUNC_DEFS;

    if(SOC_DPP_CONFIG(unit)->pp.test2 != 1)
    {
        return SOC_E_NOT_FOUND;
    }

    DPP_LEAF_CB_GATE(DPP_CB_EoE);

    SOCDNX_IF_ERR_EXIT(jer_parser_macro_sel_allocate(unit, DPP_MACRO_EoE, &custom_macro));

    /* EoE */
    ihp_parser_eth_protocols_tbl_data.eth_type_protocol     = ARAD_PARSER_ETHER_PROTO_3_EoE_TPID_TYPE;
    ihp_parser_eth_protocols_tbl_data.eth_sap_protocol      = ARAD_PARSER_ETHER_PROTO_3_EoE_TPID_TYPE;
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihp_parser_eth_protocols_tbl_set_unsafe(unit, ARAD_PARSER_ETHER_PROTO_3_EoE,
                                                                                  &ihp_parser_eth_protocols_tbl_data));

    ihp_parser_eth_protocols_tbl_data.eth_type_protocol     = ARAD_PARSER_ETHER_PROTO_5_EoE_TPID_TYPE;
    ihp_parser_eth_protocols_tbl_data.eth_sap_protocol      = ARAD_PARSER_ETHER_PROTO_5_EoE_TPID_TYPE;
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihp_parser_eth_protocols_tbl_set_unsafe(unit, ARAD_PARSER_ETHER_PROTO_5_EoE,
                                                                                  &ihp_parser_eth_protocols_tbl_data));
    /* New custom macro to identify EoE format; EoE-TPID = 0xE0E0 and 0xE0EC as two protocols.
    A custom macro will then process the TTL and EID */

    ihp_parser_custom_macro_parameters_tbl_data.cstm_word_select       = 0;           /* */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_mask_left         = 0;           /* */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_mask_right        = 0;           /* */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_comparator_mode   = 0;           /* */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_comparator_mask   = 0;           /* */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_condition_select  = 0;           /* */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_shift_sel         = 0;           /* If condition holds use shift-A else shift-B */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_shift_a           = 2;           /* add 2 bytes to previous header size */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_shift_b           = 2;           /* add 2 bytes to previous header size */
    ihp_parser_custom_macro_parameters_tbl_data.augment_previous_stage = 1;           /* don't advance header size or header qualifier */
    ihp_parser_custom_macro_parameters_tbl_data.qualifier_shift        = 0;           /* */
    ihp_parser_custom_macro_parameters_tbl_data.qualifier_or_mask      = 0x480;       /* set packet format qualifier to mac in mac */
    ihp_parser_custom_macro_parameters_tbl_data.qualifier_and_mask     = 0x7f;       /* set packet format qualifier to mac in mac */
    ihp_parser_custom_macro_parameters_tbl_data.header_qualifier_mask  = 0x780;            /* */

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihp_parser_custom_macro_parameters_tbl_set_unsafe(unit, custom_macro,
                                                                        &ihp_parser_custom_macro_parameters_tbl_data));

exit:
    SOCDNX_FUNC_RETURN;
}

static soc_error_t jer_parser_pppoes(SOC_SAND_IN int unit)
{
    ARAD_PP_IHP_PARSER_ETH_PROTOCOLS_TBL_DATA           ihp_parser_eth_protocols_tbl_data;
    ARAD_PP_IHP_PARSER_CUSTOM_MACRO_PROTOCOLS_TBL_DATA  ihp_parser_custom_macro_protocols_tbl_data;
    ARAD_PP_IHP_PARSER_CUSTOM_MACRO_PARAMETERS_TBL_DATA ihp_parser_custom_macro_parameters_tbl_data;
    uint32                                              custom_macro;

    SOCDNX_INIT_FUNC_DEFS;

    if(!SOC_DPP_PPPOE_IS_ENABLE(unit))
    {
        return SOC_E_NOT_FOUND;
    }
    if (SOC_DPP_CONFIG(unit)->pp.udp_tunnel_enable)
    {
        LOG_ERROR(BSL_LS_SOC_PKT,(BSL_META_U(0, "PPPoE: protocol 13/14 is used by UDP tunnel\n")));
        /* bypass PPPoE parser */
        return SOC_E_NOT_FOUND;
    }
    else if (SOC_DPP_CONFIG(unit)->pp.limited_gtp_parsing || SOC_DPP_CONFIG(unit)->pp.gtp_teid_exists_parsing)
    {
        LOG_ERROR(BSL_LS_SOC_PKT,(BSL_META_U(0, "PPPoE: protocol 13/14 is used by limited GTP parsing\n")));
        return SOC_E_NOT_FOUND;
    }
    else if (SOC_DPP_CONFIG(unit)->pp.fcoe_enable)
    {
        LOG_ERROR(BSL_LS_SOC_PKT,(BSL_META_U(0, "PPPoE: protocol 14 is used by FCoE\n")));
        return SOC_E_NOT_FOUND;
    }
    else if (SOC_DPP_CONFIG(unit)->pp.test2)
    {
        LOG_ERROR(BSL_LS_SOC_PKT,(BSL_META_U(0, "PPPoE: ETH protocol 5 is used by EOE\n")));
        return SOC_E_NOT_FOUND;
    }

    DPP_LEAF_CB_GATE(DPP_CB_PPPoES);

    SOCDNX_IF_ERR_EXIT(jer_parser_macro_sel_allocate(unit, DPP_MACRO_PPPoES, &custom_macro));

    /* add user-defined ethernet protocol, if it's matched, following customer macro will be executed */
    ihp_parser_eth_protocols_tbl_data.eth_type_protocol     = ARAD_PARSER_ETHER_PROTO_2_PPPoES_ETHER_TYPE;
    ihp_parser_eth_protocols_tbl_data.eth_sap_protocol      = ARAD_PARSER_ETHER_PROTO_2_PPPoES_ETHER_TYPE;
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihp_parser_eth_protocols_tbl_set_unsafe(unit, ARAD_PARSER_ETHER_PROTO_2_PPPoES,
                                                                                   &ihp_parser_eth_protocols_tbl_data));

    /* add custom macro protocols */
    /* Protocol_ID == 0x40000000, dummy value, match all PPPoE session control packets */
    ihp_parser_custom_macro_protocols_tbl_data.cstm_protocol = JER_PARSER_L2TP_OPTION_L_BIT_MASK;
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihp_parser_custom_macro_protocols_tbl_set_unsafe(unit, ARAD_PARSER_CUSTOM_MACRO_PROTO_6,
                                                                                  &ihp_parser_custom_macro_protocols_tbl_data));

    /* Protocol_ID == 0x0021 'IPv4' Next-Protocol = 13 */
    ihp_parser_custom_macro_protocols_tbl_data.cstm_protocol = 0x21;
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihp_parser_custom_macro_protocols_tbl_set_unsafe(unit, ARAD_PARSER_CUSTOM_MACRO_PROTO_13,
                                                                                  &ihp_parser_custom_macro_protocols_tbl_data));

    /* Protocol_ID == 0x0057 'IPv6' Next-Protocol = 14 */
    ihp_parser_custom_macro_protocols_tbl_data.cstm_protocol = 0x57;
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihp_parser_custom_macro_protocols_tbl_set_unsafe(unit, ARAD_PARSER_CUSTOM_MACRO_PROTO_14,
                                                                                  &ihp_parser_custom_macro_protocols_tbl_data));

    /* New custom macro to identify protocol id after PPPoE */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_word_select       = 4;           /* take 2 LSB bytes of PPPoE header + 2 bytes of next protocol */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_mask_left         = 16;          /* zero 16 bits from msb */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_mask_right        = 0;
    ihp_parser_custom_macro_parameters_tbl_data.cstm_comparator_mode   = 3;
    ihp_parser_custom_macro_parameters_tbl_data.cstm_comparator_mask   = 0x3030;      /* Compare 5, 6, 13, 14 */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_condition_select  = 0;
    ihp_parser_custom_macro_parameters_tbl_data.cstm_shift_sel         = 0;
    ihp_parser_custom_macro_parameters_tbl_data.cstm_shift_a           = 8;           /* add 8 bytes to previous header size */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_shift_b           = 8;           /* add 8 bytes to previous header size */
    ihp_parser_custom_macro_parameters_tbl_data.augment_previous_stage = 1;           /* don't advance header size or header qualifier */
    ihp_parser_custom_macro_parameters_tbl_data.qualifier_shift        = 0xe;         /* Set 4 MSb as Next-Protocol */
    ihp_parser_custom_macro_parameters_tbl_data.qualifier_or_mask      = 0x40;        /* set MSb of Encapuslation as 1 to indicate PPPoE */
    ihp_parser_custom_macro_parameters_tbl_data.qualifier_and_mask     = 0x7c0;       /* update Next-Protocol */
    ihp_parser_custom_macro_parameters_tbl_data.header_qualifier_mask  = 0x7c0;       /* update Next-Protocol */

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihp_parser_custom_macro_parameters_tbl_set_unsafe(unit, custom_macro,
                                                                        &ihp_parser_custom_macro_parameters_tbl_data));

exit:
    SOCDNX_FUNC_RETURN;
}


static soc_error_t jer_parser_raw_mpls(int unit)
{
    ARAD_PP_IHP_PARSER_CUSTOM_MACRO_PARAMETERS_TBL_DATA ihp_parser_custom_macro_parameters_tbl_data;
    ARAD_PP_IHP_PARSER_ETH_PROTOCOLS_TBL_DATA           ihp_parser_eth_protocols_tbl_data;
    uint32                                              custom_macro;

    SOCDNX_INIT_FUNC_DEFS;

    if(!soc_property_get(unit, spn_PORT_RAW_MPLS_ENABLE, 0))
    {
        return SOC_E_NOT_FOUND;
    }

    DPP_LEAF_CB_GATE(DPP_CB_RAW_MPLS);

    SOCDNX_IF_ERR_EXIT(jer_parser_macro_sel_allocate(unit, DPP_MACRO_RAW_MPLS, &custom_macro));

    ihp_parser_eth_protocols_tbl_data.eth_type_protocol     = ARAD_PARSER_ETHER_PROTO_7_MPLS_MC_ETHER_TYPE;
    ihp_parser_eth_protocols_tbl_data.eth_sap_protocol      = ARAD_PARSER_ETHER_PROTO_7_MPLS_MC_ETHER_TYPE;
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihp_parser_eth_protocols_tbl_set_unsafe(unit, ARAD_PARSER_ETHER_PROTO_7_MPLS_MC,
                                                                                  &ihp_parser_eth_protocols_tbl_data));

    /* NULL custom macro - advance header counter, but keep the offset */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_word_select       = 0;           /*  */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_mask_left         = 0;           /*  */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_mask_right        = 0;           /*  */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_comparator_mode   = 0;           /*  */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_comparator_mask   = 0;           /*  */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_condition_select  = 0;           /*  */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_shift_sel         = 0;           /*  */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_shift_a           = 0;           /*  */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_shift_b           = 0;           /*  */
    ihp_parser_custom_macro_parameters_tbl_data.augment_previous_stage = 0;           /*  */
    ihp_parser_custom_macro_parameters_tbl_data.qualifier_shift        = 0;           /*  */
    ihp_parser_custom_macro_parameters_tbl_data.qualifier_or_mask      = 0x780;       /*  */
    ihp_parser_custom_macro_parameters_tbl_data.qualifier_and_mask     = 0;           /*  */
    ihp_parser_custom_macro_parameters_tbl_data.header_qualifier_mask  = 0;           /*  */

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihp_parser_custom_macro_parameters_tbl_set_unsafe(unit, custom_macro,
                                                                        &ihp_parser_custom_macro_parameters_tbl_data));

exit:
    SOCDNX_FUNC_RETURN;
}

static soc_error_t jer_parser_mpls_cw(int unit)
{
    ARAD_PP_IHP_PARSER_CUSTOM_MACRO_PARAMETERS_TBL_DATA ihp_parser_custom_macro_parameters_tbl_data;
    uint32                                              custom_macro;

    SOCDNX_INIT_FUNC_DEFS;

    if(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "mpls_cw_disable", 0))
    {
        return SOC_E_NOT_FOUND;
    }

    DPP_LEAF_CB_GATE(DPP_CB_RAW_MPLS);

    SOCDNX_IF_ERR_EXIT(jer_parser_macro_sel_allocate(unit, DPP_MACRO_MPLS_CW, &custom_macro));

    /* NULL custom macro - advance header counter, but keep the offset */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_word_select       = 0;           /*  */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_mask_left         = 0;           /*  */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_mask_right        = 0;           /*  */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_comparator_mode   = 0;           /*  */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_comparator_mask   = 0;           /*  */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_condition_select  = 0;           /*  */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_shift_sel         = 0;           /*  */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_shift_a           = 4;           /* Shift by CW size */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_shift_b           = 4;           /* Shift by CW size */
    ihp_parser_custom_macro_parameters_tbl_data.augment_previous_stage = 1;           /* invisible stage */
    ihp_parser_custom_macro_parameters_tbl_data.qualifier_shift        = 0;           /*  */
    ihp_parser_custom_macro_parameters_tbl_data.qualifier_or_mask      = 0;           /*  */
    ihp_parser_custom_macro_parameters_tbl_data.qualifier_and_mask     = 0;           /*  */
    ihp_parser_custom_macro_parameters_tbl_data.header_qualifier_mask  = 0;           /*  */

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihp_parser_custom_macro_parameters_tbl_set_unsafe(unit, custom_macro,
                                                                        &ihp_parser_custom_macro_parameters_tbl_data));

exit:
    SOCDNX_FUNC_RETURN;
}

static int jer_parser_minm(int unit)
{
    /* spn_BCM886XX_AUXILIARY_TABLE_MODE = 2 - Enable Mac-in-Mac on device */
    if(soc_property_get(unit, spn_BCM886XX_AUXILIARY_TABLE_MODE, 0) != 2)
    {
        return SOC_E_NOT_FOUND;
    }
    return SOC_E_NONE;
}


static int jer_parser_bfd_ipv4(int unit)
{
    ARAD_PP_IHP_PARSER_CUSTOM_MACRO_PROTOCOLS_TBL_DATA  ihp_parser_custom_macro_protocols_tbl_data;

    SOCDNX_INIT_FUNC_DEFS;

    if((!SOC_DPP_CONFIG(unit)->pp.bfd_ipv4_single_hop)
        && (!soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "multi_hop_bfd_extra_dip_support", 0)))
    {
        return SOC_E_NOT_FOUND;
    }

    DPP_LEAF_CB_GATE(DPP_CB_BFD_IPv4);

    ihp_parser_custom_macro_protocols_tbl_data.cstm_protocol = ARAD_PP_BFD_SINGLE_HOP_UDP_PORT;
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihp_parser_custom_macro_protocols_tbl_set_unsafe(
            unit,
            ARAD_PARSER_CUSTOM_MACRO_PROTO_7,
            &ihp_parser_custom_macro_protocols_tbl_data
          ));

    ihp_parser_custom_macro_protocols_tbl_data.cstm_protocol = ARAD_PP_BFD_MICRO_UDP_PORT;
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihp_parser_custom_macro_protocols_tbl_set_unsafe(
            unit,
            ARAD_PARSER_CUSTOM_MACRO_PROTO_9,
            &ihp_parser_custom_macro_protocols_tbl_data
          ));

    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "multi_hop_bfd_extra_dip_support", 0)) {
        ihp_parser_custom_macro_protocols_tbl_data.cstm_protocol = ARAD_PP_BFD_MULTI_HOP_UDP_PORT;
        SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihp_parser_custom_macro_protocols_tbl_set_unsafe(
                unit,
                ARAD_PARSER_CUSTOM_MACRO_PROTO_15,
                &ihp_parser_custom_macro_protocols_tbl_data
              ));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

static int jer_parser_bfd_ipv4_transparent(int unit)
{
    SOCDNX_INIT_FUNC_DEFS;

    /* First verify that bfd ipv4 is enabled */
    _rv = jer_parser_bfd_ipv4(unit);
    if((_rv == SOC_E_EXISTS) || (_rv == SOC_E_NONE))
    {   /* If BFD is enabled, check custom feature,
         * If feature is enabled, entries associated with this custom feature will be inserted, thus masking following real BFD entries
         * Otherwise return NOT_FOUND and entry will not be inserted
         */
        if(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "non_bfd_forwarding", 0)
           || (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "multi_hop_bfd_extra_dip_support", 0)))
        {
            _rv = SOC_E_NOT_FOUND;
        }
    }
    /* If there was error on jer_parser_bfd_ipv4 (other than EXISTS) it should be returned to caller as is */

    SOCDNX_FUNC_RETURN;
}

static int jer_parser_udp_ipv4_protos(int unit)
{
    if(!SOC_DPP_CONFIG(unit)->pp.bfd_ipv4_single_hop && !SOC_DPP_CONFIG(unit)->pp.limited_gtp_parsing)
    {
        return SOC_E_NOT_FOUND;
    }
    return SOC_E_NONE;
}

static int jer_parser_udp_ipv4_gtp_protos(int unit)
{
    if(!SOC_DPP_CONFIG(unit)->pp.gtp_teid_exists_parsing)
    {
        return SOC_E_NOT_FOUND;
    }
    return SOC_E_NONE;
}


static int jer_parser_bfd_ipv6(int unit)
{
    ARAD_PP_IHP_PARSER_CUSTOM_MACRO_PROTOCOLS_TBL_DATA  ihp_parser_custom_macro_protocols_tbl_data;

    SOCDNX_INIT_FUNC_DEFS;

    if(SOC_DPP_CONFIG(unit)->pp.bfd_ipv6_enable == SOC_DPP_ARAD_BFD_IPV6_SUPPORT_NONE)
    {
        return SOC_E_NOT_FOUND;
    }

    DPP_LEAF_CB_GATE(DPP_CB_BFD_IPv6);

    ihp_parser_custom_macro_protocols_tbl_data.cstm_protocol = ARAD_PP_BFD_SINGLE_HOP_UDP_PORT;
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihp_parser_custom_macro_protocols_tbl_set_unsafe(
            unit,
            ARAD_PARSER_CUSTOM_MACRO_PROTO_7,
            &ihp_parser_custom_macro_protocols_tbl_data
          ));

    ihp_parser_custom_macro_protocols_tbl_data.cstm_protocol = ARAD_PP_BFD_MICRO_UDP_PORT;
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihp_parser_custom_macro_protocols_tbl_set_unsafe(
            unit,
            ARAD_PARSER_CUSTOM_MACRO_PROTO_9,
            &ihp_parser_custom_macro_protocols_tbl_data
          ));

    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "multi_hop_bfd_extra_dip_support", 0)) {
        ihp_parser_custom_macro_protocols_tbl_data.cstm_protocol = ARAD_PP_BFD_MULTI_HOP_UDP_PORT;
        SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihp_parser_custom_macro_protocols_tbl_set_unsafe(
                unit,
                ARAD_PARSER_CUSTOM_MACRO_PROTO_15,
                &ihp_parser_custom_macro_protocols_tbl_data
              ));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

static int jer_parser_bfd_ipv6_transparent(int unit)
{
    SOCDNX_INIT_FUNC_DEFS;

    /* First verify that bfd ipv4 is enabled */
    _rv = jer_parser_bfd_ipv6(unit);
    if((_rv == SOC_E_EXISTS) || (_rv == SOC_E_NONE))
    {   /* If BFD is enabled, check custom feature,
         * If feature is enabled, entries associated with this custom feature will be inserted, thus masking following real BFD entries
         * Otherwise return NOT_FOUND and entry will not be inserted
         */
        if(!soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "non_bfd_forwarding", 0))
        {
            _rv = SOC_E_NOT_FOUND;
        }
    }
    /* If there was error on jer_parser_bfd_ipv6 (other than EXISTS) it should be returned to caller as is */

    SOCDNX_FUNC_RETURN;
}

/* It looks exactly like the previous but we need placeholder for all udp protocols under IPv6 to enable segment creation */
static int jer_parser_udp_ipv6_protos(int unit)
{
    if(SOC_DPP_CONFIG(unit)->pp.bfd_ipv6_enable == SOC_DPP_ARAD_BFD_IPV6_SUPPORT_NONE && !SOC_DPP_CONFIG(unit)->pp.gtp_teid_exists_parsing )
    {
        return SOC_E_NOT_FOUND;
    }
    return SOC_E_NONE;
}


static int jer_parser_mpls_ipv4(int unit)
{
    if(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "mpls_ipv4_parse_disable", 0))
    {
        return SOC_E_NOT_FOUND;
    }
    return SOC_E_NONE;
}

static int jer_parser_udp(int unit)
{
    ARAD_PP_IHP_PARSER_CUSTOM_MACRO_PARAMETERS_TBL_DATA ihp_parser_custom_macro_parameters_tbl_data;
    ARAD_PP_IHP_PARSER_CUSTOM_MACRO_PROTOCOLS_TBL_DATA  ihp_parser_custom_macro_protocols_tbl_data;
    uint32                                              custom_macro;

    SOCDNX_INIT_FUNC_DEFS;

    if(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "udp_parse_disable", 0))
    {
        return SOC_E_NOT_FOUND;
    }

    DPP_LEAF_CB_GATE(DPP_CB_UDP);

    /* Configure PTP (1588) udp ports */
    ihp_parser_custom_macro_protocols_tbl_data.cstm_protocol = ARAD_PP_PTP_UDP_ENCAP_PORT1;
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihp_parser_custom_macro_protocols_tbl_set_unsafe(
            unit,
            ARAD_PARSER_CUSTOM_MACRO_PROTO_10,
            &ihp_parser_custom_macro_protocols_tbl_data
          ));

    ihp_parser_custom_macro_protocols_tbl_data.cstm_protocol = ARAD_PP_PTP_UDP_ENCAP_PORT2;
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihp_parser_custom_macro_protocols_tbl_set_unsafe(
            unit,
            ARAD_PARSER_CUSTOM_MACRO_PROTO_11,
            &ihp_parser_custom_macro_protocols_tbl_data
          ));

    SOCDNX_IF_ERR_EXIT(jer_parser_macro_sel_allocate(unit, DPP_MACRO_UDP, &custom_macro));

    ihp_parser_custom_macro_parameters_tbl_data.cstm_word_select       = 0;           /* take VID + eth-type (TPID is eth-type of previous header) */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_mask_left         = 16;          /* mask VID data */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_mask_right        = 0;           /*  */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_comparator_mode   = 1;           /* Compare 16 lsb of selected data to 16 lsb of comparator values */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_comparator_mask   = 0;
    ihp_parser_custom_macro_parameters_tbl_data.cstm_condition_select  = 0;
    ihp_parser_custom_macro_parameters_tbl_data.cstm_shift_sel         = 0;
    ihp_parser_custom_macro_parameters_tbl_data.cstm_shift_a           = 8;           /* UDP size is 8B */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_shift_b           = 8;           /* UDP size is 8B */
    ihp_parser_custom_macro_parameters_tbl_data.augment_previous_stage = 0;           /* visible stage */
    ihp_parser_custom_macro_parameters_tbl_data.qualifier_shift        = 0xe;
    ihp_parser_custom_macro_parameters_tbl_data.qualifier_or_mask      = 0;
    ihp_parser_custom_macro_parameters_tbl_data.qualifier_and_mask     = 0x780;
    ihp_parser_custom_macro_parameters_tbl_data.header_qualifier_mask  = 0x780;
    
    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ptp_udp_parser_enable", 0)) {
        ihp_parser_custom_macro_parameters_tbl_data.cstm_comparator_mask   |= (1 << ARAD_PARSER_CUSTOM_MACRO_PROTO_10) |
                                                                              (1 << ARAD_PARSER_CUSTOM_MACRO_PROTO_11); /* compare to PTP entries 10 and 10 */
    }       

    if ((SOC_DPP_CONFIG(unit)->pp.bfd_ipv4_single_hop)
                            || (SOC_DPP_CONFIG(unit)->pp.bfd_ipv6_enable == SOC_DPP_ARAD_BFD_IPV6_SUPPORT_WITH_LEM)) {
        ihp_parser_custom_macro_parameters_tbl_data.cstm_comparator_mask   |= (1 << ARAD_PARSER_CUSTOM_MACRO_PROTO_7);       /* add bfd ipv4 single hop macro protocol */
        ihp_parser_custom_macro_parameters_tbl_data.cstm_comparator_mask   |= (1 << ARAD_PARSER_CUSTOM_MACRO_PROTO_9);       /* add micro bfd macro protocol */
    }

    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "multi_hop_bfd_extra_dip_support", 0)) {
        ihp_parser_custom_macro_parameters_tbl_data.cstm_comparator_mask   |= (1 << ARAD_PARSER_CUSTOM_MACRO_PROTO_15);      /* bfd multi hop protocol */
    }

    if (SOC_DPP_CONFIG(unit)->pp.limited_gtp_parsing || SOC_DPP_CONFIG(unit)->pp.gtp_teid_exists_parsing) {
        ihp_parser_custom_macro_parameters_tbl_data.cstm_comparator_mask   |= (1 << ARAD_PARSER_CUSTOM_MACRO_PROTO_14);
        ihp_parser_custom_macro_parameters_tbl_data.cstm_comparator_mask   |= (1 << ARAD_PARSER_CUSTOM_MACRO_PROTO_15);
    }

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihp_parser_custom_macro_parameters_tbl_set_unsafe(unit, custom_macro,
                                                                        &ihp_parser_custom_macro_parameters_tbl_data));

exit:
    SOCDNX_FUNC_RETURN;
}

static int jer_parser_udp_mpls(int unit)
{
    SOCDNX_INIT_FUNC_DEFS;
    /*
     * Leafs of UDPoIPv4oMPLS are added for PLC of BFD Multi-Hop, when BFD Multi-Hop parser is not enabled, bypass these leafs to save segment resource.
     */
    if(!soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "multi_hop_bfd_extra_dip_support", 0))
    {
        return SOC_E_NOT_FOUND;
    }
    /*
     * Bypass Leafs of UDPoIPv4oMPLS when parser to UDP tunnel is enabled due to segment limit.
     */
    if(SOC_DPP_CONFIG(unit)->pp.udp_tunnel_enable)
    {
        LOG_ERROR(BSL_LS_SOC_PKT,(BSL_META_U(0, "BFDoUDPoIPv4oMPLS Leafs are not allocated when UDP tunnel is enabled\n")));
        return SOC_E_NOT_FOUND;
    }
    /*
     * Bypass Leafs of UDPoIPv4oMPLS when parser to IPv6 extension header is enabled due to segment limit.
     */
    if(SOC_DPP_CONFIG(unit)->pp.ipv6_ext_header)
    {
        LOG_ERROR(BSL_LS_SOC_PKT,(BSL_META_U(0, "BFDoUDPoIPv4oMPLS Leafs are not allocated when parser to IPv6 extension is enabled\n")));
        return SOC_E_NOT_FOUND;
    }
    /*
     * Bypass Leafs of UDPoIPv4oMPLS when parser to VxLANoIPv6 is enabled due to segment limit.
     */
    if(SOC_DPP_CONFIG(unit)->pp.ipv6_tunnel_term_bitmap_enable & SOC_DPP_IP_TUNNEL_TERM_DB_VXLAN)
    {
        LOG_ERROR(BSL_LS_SOC_PKT,(BSL_META_U(0, "BFDoUDPoIPv4oMPLS Leafs are not allocated when VxLAN over IPv6 is enabled\n")));
        return SOC_E_NOT_FOUND;
    }
    /*
     * Init Leafs of UDPoIPv4oMPLS, the following leaf is bypassed.
     */
    _rv = jer_parser_udp(unit);

    SOCDNX_FUNC_RETURN;
}

static soc_error_t jer_parser_vxlan(int unit)
{
    ARAD_PP_IHP_PARSER_CUSTOM_MACRO_PARAMETERS_TBL_DATA ihp_parser_custom_macro_parameters_tbl_data;
    ARAD_PP_IHP_PARSER_CUSTOM_MACRO_PROTOCOLS_TBL_DATA  ihp_parser_custom_macro_protocols_tbl_data;
    uint32                                              custom_macro;

    SOCDNX_INIT_FUNC_DEFS;

    if(!(SOC_DPP_CONFIG(unit)->pp.ipv4_tunnel_term_bitmap_enable & SOC_DPP_IP_TUNNEL_TERM_DB_VXLAN)
       &&!(SOC_DPP_CONFIG(unit)->pp.ipv6_tunnel_term_bitmap_enable & SOC_DPP_IP_TUNNEL_TERM_DB_VXLAN)
       &&!SOC_DPP_CONFIG(unit)->pp.custom_feature_vxlan_port_termination)
    {
        return SOC_E_NOT_FOUND;
    }

    DPP_LEAF_CB_GATE(DPP_CB_VxLAN);

    SOCDNX_IF_ERR_EXIT(jer_parser_macro_sel_allocate(unit, DPP_MACRO_VxLAN, &custom_macro));

    /* M3 */
    /* udp_invisible for VXLAN */
    ihp_parser_custom_macro_protocols_tbl_data.cstm_protocol = SOC_DPP_CONFIG(unit)->pp.vxlan_udp_dest_port;
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihp_parser_custom_macro_protocols_tbl_set_unsafe(unit, ARAD_PARSER_CUSTOM_MACRO_PROTO_8,
                                                                                   &ihp_parser_custom_macro_protocols_tbl_data));
    ihp_parser_custom_macro_parameters_tbl_data.cstm_word_select = 0;                 /* from start of UDP */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_mask_left = 16;                  /* mask UDP Source port */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_mask_right = 0;                  /* look at UDP dest port */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_comparator_mode = 1;             /* Compare 16 lsb of selected data to 16 lsb of comparator values */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_comparator_mask = (1 << ARAD_PARSER_CUSTOM_MACRO_PROTO_8);      /* compare to entry 7 */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_condition_select = 16; /* (7+1)*2 */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_shift_sel = 0;                   /* jump A if match */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_shift_a = 16;                  /* UDP+VXLAN */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_shift_b = 0;                  /* nothing */
    ihp_parser_custom_macro_parameters_tbl_data.augment_previous_stage = 1;           /* invisible stage */
    ihp_parser_custom_macro_parameters_tbl_data.qualifier_shift = 0xf;                /*we need Protocol value in bits 10:7 of the qualfier to set new next-protocol . */                                                                          
    ihp_parser_custom_macro_parameters_tbl_data.qualifier_or_mask = 0x80;            
    ihp_parser_custom_macro_parameters_tbl_data.qualifier_and_mask = 0x400;
    ihp_parser_custom_macro_parameters_tbl_data.header_qualifier_mask = 0x780;        /* update Next-Protocol */

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihp_parser_custom_macro_parameters_tbl_set_unsafe(unit, custom_macro,
                                                                        &ihp_parser_custom_macro_parameters_tbl_data));
exit:
    SOCDNX_FUNC_RETURN;
}

static soc_error_t jer_parser_l2tp1(int unit)
{
    ARAD_PP_IHP_PARSER_CUSTOM_MACRO_PARAMETERS_TBL_DATA ihp_parser_custom_macro_parameters_tbl_data;
    ARAD_PP_IHP_PARSER_CUSTOM_MACRO_PROTOCOLS_TBL_DATA  ihp_parser_custom_macro_protocols_tbl_data;
    uint32                                              custom_macro;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(jer_parser_macro_sel_allocate(unit, DPP_MACRO_L2TP1, &custom_macro));

    /* UDP dest_port for L2TP */
    ihp_parser_custom_macro_protocols_tbl_data.cstm_protocol = ARAD_PP_L2TP_ENCAP_PORT;
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihp_parser_custom_macro_protocols_tbl_set_unsafe(unit, ARAD_PARSER_CUSTOM_MACRO_PROTO_12,
                                                                                   &ihp_parser_custom_macro_protocols_tbl_data));
	
    ihp_parser_custom_macro_parameters_tbl_data.cstm_word_select = 0;                 /* from start of UDP */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_mask_left = 16;                  /* mask UDP Source port */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_mask_right = 0;                  /* look at UDP dest port */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_comparator_mode = 1;             /* Compare 16 lsb of selected data to 16 lsb of comparator values */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_comparator_mask = (1 << ARAD_PARSER_CUSTOM_MACRO_PROTO_12);
    ihp_parser_custom_macro_parameters_tbl_data.cstm_condition_select = (ARAD_PARSER_CUSTOM_MACRO_PROTO_12 + 1) * 2;
    ihp_parser_custom_macro_parameters_tbl_data.cstm_shift_sel = 0;                   /* jump A if match */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_shift_a = 8;                     /* UDP header */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_shift_b = 0;                     /* nothing */
    ihp_parser_custom_macro_parameters_tbl_data.augment_previous_stage = 1;           /* invisible stage */
    ihp_parser_custom_macro_parameters_tbl_data.qualifier_shift = 0xf;                /* range bit is on the msb of the next-protocol bits */
    ihp_parser_custom_macro_parameters_tbl_data.qualifier_or_mask = 0x00;            
    ihp_parser_custom_macro_parameters_tbl_data.qualifier_and_mask = 0x400;			  /* mask all bits except range bit */
    ihp_parser_custom_macro_parameters_tbl_data.header_qualifier_mask = 0x400;        /* update Next-Protocol - setting the msb. since udp is 9 match will change it to 1 or it will stay 9*/

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihp_parser_custom_macro_parameters_tbl_set_unsafe(unit, custom_macro,
                                                                        &ihp_parser_custom_macro_parameters_tbl_data));
exit:
    SOCDNX_FUNC_RETURN;
}

static soc_error_t jer_parser_l2tp2(int unit)
{
    ARAD_PP_IHP_PARSER_CUSTOM_MACRO_PARAMETERS_TBL_DATA ihp_parser_custom_macro_parameters_tbl_data;
    ARAD_PP_IHP_PARSER_CUSTOM_MACRO_PROTOCOLS_TBL_DATA  ihp_parser_custom_macro_protocols_tbl_data;
    uint32                                              custom_macro;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(jer_parser_macro_sel_allocate(unit, DPP_MACRO_L2TP2, &custom_macro));

	
    /* L2TP L-Bit option */
    ihp_parser_custom_macro_protocols_tbl_data.cstm_protocol = JER_PARSER_L2TP_OPTION_L_BIT_MASK;
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihp_parser_custom_macro_protocols_tbl_set_unsafe(unit, ARAD_PARSER_CUSTOM_MACRO_PROTO_6,
                                                                                   &ihp_parser_custom_macro_protocols_tbl_data));
	
    ihp_parser_custom_macro_parameters_tbl_data.cstm_word_select = 0;                 /* from start of UDP */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_mask_left = 1;                   /* mask off the T-bit */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_mask_right = 30;                 /* mask off everything after the L-bit */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_comparator_mode = 3;             /* Compare 32 bits selected data to 32 bits of comperator values */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_comparator_mask = (1 << ARAD_PARSER_CUSTOM_MACRO_PROTO_6);
    ihp_parser_custom_macro_parameters_tbl_data.cstm_condition_select = (ARAD_PARSER_CUSTOM_MACRO_PROTO_6 + 1) * 2;
    ihp_parser_custom_macro_parameters_tbl_data.cstm_shift_sel = 0;                   /* jump A if match, B if not match */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_shift_a = 12;                    /* L2TP-base(6) + L2TP-Length(2) + PPP-0xFF03_prefix(2) + PPP_Protocol(2) */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_shift_b = 10;                    /* L2TP-base(6) + PPP-0xFF03_prefix(2) + PPP_Protocol(2) */
    ihp_parser_custom_macro_parameters_tbl_data.augment_previous_stage = 1;           /* invisible stage */
    ihp_parser_custom_macro_parameters_tbl_data.qualifier_shift = 0;                  /* don't care */
    ihp_parser_custom_macro_parameters_tbl_data.qualifier_or_mask = 0x000;            /* don't care */
    ihp_parser_custom_macro_parameters_tbl_data.qualifier_and_mask = 0x000;		      /* don't care */
    ihp_parser_custom_macro_parameters_tbl_data.header_qualifier_mask = 0x000;        /* don't change anything from previous stage */

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihp_parser_custom_macro_parameters_tbl_set_unsafe(unit, custom_macro,
                                                                        &ihp_parser_custom_macro_parameters_tbl_data));
exit:
    SOCDNX_FUNC_RETURN;
}

static soc_error_t jer_parser_l2tp(int unit)
{
    SOCDNX_INIT_FUNC_DEFS;

	if(!SOC_DPP_L2TP_IS_ENABLE(unit)) 
	{
        return SOC_E_NOT_FOUND;
    }

	if (SOC_DPP_CONFIG(unit)->pp.limited_gtp_parsing)
    {
        LOG_ERROR(BSL_LS_SOC_PKT,(BSL_META_U(0, "L2TP: protocol 12 is used by limited GTP parsing\n")));
        return SOC_E_NOT_FOUND;
    }

    DPP_LEAF_CB_GATE(DPP_CB_L2TP);
	
    SOCDNX_IF_ERR_EXIT(jer_parser_l2tp1(unit));
    SOCDNX_IF_ERR_EXIT(jer_parser_l2tp2(unit));

exit:
	SOCDNX_FUNC_RETURN;
}

static soc_error_t jer_parser_vxlan6(int unit)
{
    SOCDNX_INIT_FUNC_DEFS;

    if(!(SOC_DPP_CONFIG(unit)->pp.ipv6_tunnel_term_bitmap_enable & SOC_DPP_IP_TUNNEL_TERM_DB_VXLAN))
    {
        return SOC_E_NOT_FOUND;
    }
    _rv = jer_parser_vxlan(unit);
    

    SOCDNX_FUNC_RETURN;
}

static soc_error_t jer_parser_udp_tunnel(int unit)
{
    ARAD_PP_IHP_PARSER_CUSTOM_MACRO_PARAMETERS_TBL_DATA ihp_parser_custom_macro_parameters_tbl_data;
    ARAD_PP_IHP_PARSER_CUSTOM_MACRO_PROTOCOLS_TBL_DATA  ihp_parser_custom_macro_protocols_tbl_data;
    uint32                                              custom_macro;

    SOCDNX_INIT_FUNC_DEFS;

    if(!SOC_DPP_CONFIG(unit)->pp.udp_tunnel_enable)
    {
        return SOC_E_NOT_FOUND;
    }

    DPP_LEAF_CB_GATE(DPP_CB_UDP_TUNNEL);

    SOCDNX_IF_ERR_EXIT(jer_parser_macro_sel_allocate(unit, DPP_MACRO_UDP_TUNNEL, &custom_macro));

    ihp_parser_custom_macro_protocols_tbl_data.cstm_protocol = ARAD_PP_UDP_DUMMY_DST_PORT;    /* indicates non udp tunnel */
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihp_parser_custom_macro_protocols_tbl_set_unsafe(unit, ARAD_PARSER_CUSTOM_MACRO_PROTO_10,
                                                                                   &ihp_parser_custom_macro_protocols_tbl_data));

    ihp_parser_custom_macro_protocols_tbl_data.cstm_protocol = ARAD_PP_UDP_IPV4_DST_PORT;
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihp_parser_custom_macro_protocols_tbl_set_unsafe(unit, ARAD_PARSER_CUSTOM_MACRO_PROTO_13,
                                                                                   &ihp_parser_custom_macro_protocols_tbl_data));

    ihp_parser_custom_macro_protocols_tbl_data.cstm_protocol = ARAD_PP_UDP_IPV6_DST_PORT;
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihp_parser_custom_macro_protocols_tbl_set_unsafe(unit, ARAD_PARSER_CUSTOM_MACRO_PROTO_14,
                                                                                   &ihp_parser_custom_macro_protocols_tbl_data));

    ihp_parser_custom_macro_protocols_tbl_data.cstm_protocol = ARAD_PP_UDP_MPLS_DST_PORT;
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihp_parser_custom_macro_protocols_tbl_set_unsafe(unit, ARAD_PARSER_CUSTOM_MACRO_PROTO_15,
                                                                                   &ihp_parser_custom_macro_protocols_tbl_data));

    ihp_parser_custom_macro_parameters_tbl_data.cstm_word_select = 0;             /* from start of UDP */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_mask_left = 16;              /* mask UDP Source port */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_mask_right = 0;              /* look at UDP dest port */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_comparator_mode = 3;         /* Compare 32 lsb of selected data to 32 lsb of comparator values */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_comparator_mask = (1 << ARAD_PARSER_CUSTOM_MACRO_PROTO_10) |
                                                                       (1 << ARAD_PARSER_CUSTOM_MACRO_PROTO_13) |
                                                                       (1 << ARAD_PARSER_CUSTOM_MACRO_PROTO_14) |
                                                                       (1 << ARAD_PARSER_CUSTOM_MACRO_PROTO_15);  /* compare to entry 15, 14, 13 and 10 */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_condition_select = 19;       /* compare to not udp */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_shift_sel = 0;               /* jump A if match */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_shift_a = 0;                 /* not UDP tunnel */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_shift_b = 8;                 /* UDP Tunnel */
    ihp_parser_custom_macro_parameters_tbl_data.augment_previous_stage = 1;       /* invisible stage */
    ihp_parser_custom_macro_parameters_tbl_data.qualifier_shift = 0xe;            /* take range-or-protocol */
    ihp_parser_custom_macro_parameters_tbl_data.qualifier_or_mask = 0;
    ihp_parser_custom_macro_parameters_tbl_data.qualifier_and_mask = 0x780;        /* change only the next-protocol values */
    ihp_parser_custom_macro_parameters_tbl_data.header_qualifier_mask = 0x780;     /* change only the next-protocol values */

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihp_parser_custom_macro_parameters_tbl_set_unsafe(unit, custom_macro,
        &ihp_parser_custom_macro_parameters_tbl_data));
exit:
    SOCDNX_FUNC_RETURN;
}

static soc_error_t jer_parser_ipv6_extension_header_ip_protocol(int unit)
{
    uint32 entry_offset, entry_offset_start, entry_offset_max;
    ARAD_PP_IHP_PARSER_IP_PROTOCOLS_TBL_DATA ihp_parser_ip_protocols_tbl_data[ARAD_PARSER_IP_PROTO_NOF_ENTRIES];

    SOCDNX_INIT_FUNC_DEFS;

    sal_memset(ihp_parser_ip_protocols_tbl_data, 0x0, ARAD_PARSER_IP_PROTO_NOF_ENTRIES * sizeof(ARAD_PP_IHP_PARSER_IP_PROTOCOLS_TBL_DATA));

        /* For QAX and Jericho-Plus, the IPv6 macro supports parsing of up to one (1) IPv6-Extension-headers.
         * In order to enable that, the IP protocols should be configured with the relevant protocols.
         * The macro compares the IP-Protocol in IPv6 header, and if matches to one of IPv6-Ext protocols
         * it parses the extension header after the IPv6 header.
         * The parsing of the extension header compares the next protocol to IP-Protocol table and updates
         * IPv6 qualifier accordingly.
         * Also, it calculates the size according to field hdr-ext-length. The length is in 8-octet units,
         * not including the first 8 octets (e.g. if hdr-ext-len==1, then extension-header size is
         * 2 * 8-Octets = 16 bytes). The size of the extension header is added to IPv6 header length.
         * Extension header is not considered a separate header in header offsets.
         *
         * Most of Ipv6 extension header protocols have the above format but not all. Only extensions with
         * that format will be supported.
         *
         * According to http://www.iana.org/assignments/ipv6-parameters/ipv6-parameters.xhtml the following
         * protocols are supported: Hop-by-Hop (0), Destination-Options (60), Routing (43), Fragment (44),
         * Mobility (135), Host-Identity-Protocol (HIP) (139), Shim6 (140).
         *
         * IPv6 Extension headers that are NOT supported are: Encapsulating-Security-Payload (50),
         * Authentication-Header (51), and two more (253) and (254) Use for experimentation and testing.
         *
         */

        entry_offset_start = 0;
        entry_offset_max   = 6;

        /* Hop-by-Hop (0) */
        ihp_parser_ip_protocols_tbl_data[0].ip_protocol     = ARAD_PARSER_IP_PROTO_IPV6_EXT_HOP_BY_HOP;
        ihp_parser_ip_protocols_tbl_data[0].extension_valid = 1;

        /* Destination-Options (60) */
        ihp_parser_ip_protocols_tbl_data[1].ip_protocol     = ARAD_PARSER_IP_PROTO_IPV6_EXT_DEST_OPTIONS;
        ihp_parser_ip_protocols_tbl_data[1].extension_valid = 1;

        /* Routing (43) */
        ihp_parser_ip_protocols_tbl_data[2].ip_protocol     = ARAD_PARSER_IP_PROTO_IPV6_EXT_ROUTING;
        ihp_parser_ip_protocols_tbl_data[2].extension_valid = 1;

        /* Fragment (44) */
        ihp_parser_ip_protocols_tbl_data[3].ip_protocol     = ARAD_PARSER_IP_PROTO_IPV6_EXT_FRAGMENT;
        ihp_parser_ip_protocols_tbl_data[3].extension_valid = 1;

        /* Mobility (135) */
        ihp_parser_ip_protocols_tbl_data[4].ip_protocol     = ARAD_PARSER_IP_PROTO_IPV6_EXT_MOBILITY;
        ihp_parser_ip_protocols_tbl_data[4].extension_valid = 1;

        /* Host-Identity-Protocol (HIP) (139) */
        ihp_parser_ip_protocols_tbl_data[5].ip_protocol     = ARAD_PARSER_IP_PROTO_IPV6_EXT_HIP;
        ihp_parser_ip_protocols_tbl_data[5].extension_valid = 1;

        /* Shim6 (140) */
        ihp_parser_ip_protocols_tbl_data[6].ip_protocol     = ARAD_PARSER_IP_PROTO_IPV6_EXT_SHIM6;
        ihp_parser_ip_protocols_tbl_data[6].extension_valid = 1;

    for(entry_offset = entry_offset_start; entry_offset <= entry_offset_max; entry_offset++)
    {
        SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihp_parser_ip_protocols_tbl_set_unsafe(unit, entry_offset,
                                                    &(ihp_parser_ip_protocols_tbl_data[entry_offset])));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

static int jer_parser_ipv6_ext(int unit)
{
    ARAD_PP_IHP_PARSER_CUSTOM_MACRO_PARAMETERS_TBL_DATA ihp_parser_custom_macro_parameters_tbl_data;
    ARAD_PP_IHP_PARSER_CUSTOM_MACRO_PROTOCOLS_TBL_DATA  ihp_parser_custom_macro_protocols_tbl_data;
    uint32 custom_macro;
    uint32 cstm_comparator_mask = 0;

    SOCDNX_INIT_FUNC_DEFS;

    if(!SOC_DPP_CONFIG(unit)->pp.ipv6_ext_header)
    {
        return SOC_E_NOT_FOUND;
    }
    /* Configure relevant IP Protocols. */
    SOCDNX_IF_ERR_EXIT(jer_parser_ipv6_extension_header_ip_protocol(unit));

    if (SOC_IS_JERICHO_PLUS(unit))
    {   /* For jericho+ we need this callback only to configure IP protocols once, from Program Memory POV not enabled */
        return SOC_E_NOT_FOUND;
    }

    /*
     * Only one layer of Extension header are supported
     * Next Header field of Extension header can be following values:
     * Next Header == 0x6 'TCP' Next-Protocol = 8
     * Next Header == 0x11 'UDP' Next-Protocol = 9
     * Next Header == 0x2 'IGMP' Next-Protocol = 10
     * Next Header == 0x1 'ICMP' Next-Protocol = 11
     * Next Header == 0x3a 'ICMPv6' Next-Protocol = 12
     * Next Header == 0x4 'IPv4' Next-Protocol = 14
     * Next Header == 0x29 'IPv6' Next-Protocol = 16
     * Next-Protocol is 0 for all other unknown protocol types
     * packet-format-qualifier.has-options is set to 1
     */

    /*
     * No check to ARAD_PARSER_CUSTOM_MACRO_PROTO_1
     * It should always be set for either Jericho ITMH mode(0x100) or Arad ITMH mode(0x80000)
     * Here it's used to match all unknow protocols to be next_protocol 0
     */
    cstm_comparator_mask = (1 << ARAD_PARSER_CUSTOM_MACRO_PROTO_1);

    /* Check conflict of Marco Protocol 8 for TCP*/
    if (SOC_DPP_CONFIG(unit)->pp.ipv4_tunnel_term_bitmap_enable & SOC_DPP_IP_TUNNEL_TERM_DB_VXLAN || SOC_DPP_CONFIG(unit)->pp.custom_feature_vxlan_port_termination)
    {
        LOG_ERROR(BSL_LS_SOC_PKT,(BSL_META_U(unit, "EHoIPv6: protocol 8 is used by VxLAN\n")));
    }
    else
    {
        /* Next Header == 0x6 'TCP' Next-Protocol = 0x8 */
        ihp_parser_custom_macro_protocols_tbl_data.cstm_protocol = 0x6;
        SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihp_parser_custom_macro_protocols_tbl_set_unsafe(unit, ARAD_PARSER_CUSTOM_MACRO_PROTO_8,
                                                                                      &ihp_parser_custom_macro_protocols_tbl_data));

        cstm_comparator_mask |= (1 << ARAD_PARSER_CUSTOM_MACRO_PROTO_8);
    }

    /* Check conflict of Marco Protocol 9 for UDP */
    if ((SOC_DPP_CONFIG(unit)->pp.bfd_ipv4_single_hop) || (SOC_DPP_CONFIG(unit)->pp.bfd_ipv6_enable == SOC_DPP_ARAD_BFD_IPV6_SUPPORT_WITH_LEM))
    {
        LOG_ERROR(BSL_LS_SOC_PKT,(BSL_META_U(unit, "EHoIPv6: protocol 9 is used by bfdoIPvX\n")));
    }
    else
    {
        /* Next Header == 0x11 'UDP' Next-Protocol = 9 */
        ihp_parser_custom_macro_protocols_tbl_data.cstm_protocol = 0x11;
        SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihp_parser_custom_macro_protocols_tbl_set_unsafe(unit, ARAD_PARSER_CUSTOM_MACRO_PROTO_9,
                                                                                      &ihp_parser_custom_macro_protocols_tbl_data));
        cstm_comparator_mask |= (1 << ARAD_PARSER_CUSTOM_MACRO_PROTO_9);
    }

    /* Check conflict of Marco Protocol 10 for IGMP */
    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ptp_udp_parser_enable", 0))
    {
        LOG_ERROR(BSL_LS_SOC_PKT,(BSL_META_U(unit, "EHoIPv6: protocol 10 are used by 1588oUDP\n")));
    }
    else if (SOC_DPP_CONFIG(unit)->pp.udp_tunnel_enable)
    {
        LOG_ERROR(BSL_LS_SOC_PKT,(BSL_META_U(unit, "EHoIPv6: protocol 10 are used by UDP tunnel\n")));
    }
    else
    {
        /* Next Header == 0x2 'IGMP' Next-Protocol = 10 */
        ihp_parser_custom_macro_protocols_tbl_data.cstm_protocol = 0x2;
        SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihp_parser_custom_macro_protocols_tbl_set_unsafe(unit, ARAD_PARSER_CUSTOM_MACRO_PROTO_10,
                                                                                      &ihp_parser_custom_macro_protocols_tbl_data));
        cstm_comparator_mask |= (1 << ARAD_PARSER_CUSTOM_MACRO_PROTO_10);
    }

    /* Check conflict of Marco Protocol 11 for ICMP */
    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ptp_udp_parser_enable", 0))
    {
        LOG_ERROR(BSL_LS_SOC_PKT,(BSL_META_U(unit, "EHoIPv6: protocol 11 are used by 1588oUDP\n")));
    }
    else if (SOC_DPP_CONFIG(unit)->pp.udp_tunnel_enable)
    {
        LOG_ERROR(BSL_LS_SOC_PKT,(BSL_META_U(unit, "EHoIPv6: protocol 11 are used by UDP tunnel\n")));
    }
    else
    {
        /* Next Header == 0x1 'ICMP' Next-Protocol = 11 */
        ihp_parser_custom_macro_protocols_tbl_data.cstm_protocol = 0x1;
        SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihp_parser_custom_macro_protocols_tbl_set_unsafe(unit, ARAD_PARSER_CUSTOM_MACRO_PROTO_11,
                                                                                      &ihp_parser_custom_macro_protocols_tbl_data));
        cstm_comparator_mask |= (1 << ARAD_PARSER_CUSTOM_MACRO_PROTO_11);
    }

	if(SOC_DPP_L2TP_IS_ENABLE(unit)) 
	{
        LOG_ERROR(BSL_LS_SOC_PKT,(BSL_META_U(0, "EHoIPv6: protocol 12 is used by L2TP parsing\n")));
        return SOC_E_NOT_FOUND;
    }

    /* Check conflict of Marco Protocol 12 for ICMPv6 */
    if (SOC_DPP_CONFIG(unit)->pp.limited_gtp_parsing || SOC_DPP_CONFIG(unit)->pp.gtp_teid_exists_parsing)
    {
        LOG_ERROR(BSL_LS_SOC_PKT,(BSL_META_U(unit, "EHoIPv6: protocol 12 is used by limited GTP parsing\n")));
    }
    else
    {
        /* Next Header == 0x3a 'ICMPv6' Next-Protocol = 0xc */
        ihp_parser_custom_macro_protocols_tbl_data.cstm_protocol = 0x3a;
        SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihp_parser_custom_macro_protocols_tbl_set_unsafe(unit, ARAD_PARSER_CUSTOM_MACRO_PROTO_12,
                                                                                      &ihp_parser_custom_macro_protocols_tbl_data));
        cstm_comparator_mask |= (1 << ARAD_PARSER_CUSTOM_MACRO_PROTO_12);
    }

    /* Check conflict of Marco Protocol 13 for IPv4 */
    if (SOC_DPP_CONFIG(unit)->pp.limited_gtp_parsing || SOC_DPP_CONFIG(unit)->pp.gtp_teid_exists_parsing)
    {
        LOG_ERROR(BSL_LS_SOC_PKT,(BSL_META_U(unit, "EHoIPv6: protocol 13 is used by limited GTP parsing\n")));
    }
    else if (SOC_DPP_CONFIG(unit)->pp.udp_tunnel_enable)
    {
        LOG_ERROR(BSL_LS_SOC_PKT,(BSL_META_U(unit, "EHoIPv6: protocol 13 are used by UDP tunnel\n")));
    }
    else if (SOC_IS_JERICHO(unit) && soc_property_get(unit, spn_PPPOE_MODE, 0))
    {
        LOG_ERROR(BSL_LS_SOC_PKT,(BSL_META_U(unit, "EHoIPv6: protocol 13 are used by PPPoE\n")));
    }
    else
    {
        /* Next Header == 0x4 'IPv4' Next-Protocol = 0xd */
        ihp_parser_custom_macro_protocols_tbl_data.cstm_protocol = 0x4;
        SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihp_parser_custom_macro_protocols_tbl_set_unsafe(unit, ARAD_PARSER_CUSTOM_MACRO_PROTO_13,
                                                                                      &ihp_parser_custom_macro_protocols_tbl_data));
        cstm_comparator_mask |= (1 << ARAD_PARSER_CUSTOM_MACRO_PROTO_13);
    }

    /* Check conflict of Marco Protocol 14 for IPv6 */
    if (SOC_DPP_CONFIG(unit)->pp.limited_gtp_parsing || SOC_DPP_CONFIG(unit)->pp.gtp_teid_exists_parsing)
    {
        LOG_ERROR(BSL_LS_SOC_PKT,(BSL_META_U(unit, "EHoIPv6: protocol 14 is used by limited GTP parsing\n")));
    }
    else if (SOC_DPP_CONFIG(unit)->pp.udp_tunnel_enable)
    {
        LOG_ERROR(BSL_LS_SOC_PKT,(BSL_META_U(unit, "EHoIPv6: protocol 14 are used by UDP tunnel\n")));
    }
    else if (SOC_DPP_CONFIG(unit)->pp.fcoe_enable)
    {
        LOG_ERROR(BSL_LS_SOC_PKT,(BSL_META_U(unit, "EHoIPv6: protocol 14 are used by FCOE\n")));
    }
    else if (SOC_IS_JERICHO(unit) && soc_property_get(unit, spn_PPPOE_MODE, 0))
    {
        LOG_ERROR(BSL_LS_SOC_PKT,(BSL_META_U(unit, "EHoIPv6: protocol 14 are used by PPPoE\n")));
    }
    else
    {
        /* Next Header == 0x29 'IPv6' Next-Protocol = 0xe */
        ihp_parser_custom_macro_protocols_tbl_data.cstm_protocol = 0x29;
        SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihp_parser_custom_macro_protocols_tbl_set_unsafe(unit, ARAD_PARSER_CUSTOM_MACRO_PROTO_14,
                                                                                      &ihp_parser_custom_macro_protocols_tbl_data));
        cstm_comparator_mask |= (1 << ARAD_PARSER_CUSTOM_MACRO_PROTO_14);
    }

    DPP_LEAF_CB_GATE(DPP_CB_IPv6Ext);

    SOCDNX_IF_ERR_EXIT(jer_parser_macro_sel_allocate(unit, DPP_MACRO_IPv6Ext1, &custom_macro));

    ihp_parser_custom_macro_parameters_tbl_data.cstm_word_select       = 29;          /* take Next_Header from EH */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_mask_left         = 24;          /* mask anything else */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_mask_right        = 0;
    ihp_parser_custom_macro_parameters_tbl_data.cstm_comparator_mode   = 3;           /* Compare all 32 bits */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_comparator_mask   = cstm_comparator_mask;
    ihp_parser_custom_macro_parameters_tbl_data.cstm_condition_select  = 0;
    ihp_parser_custom_macro_parameters_tbl_data.cstm_shift_sel         = 0;
    ihp_parser_custom_macro_parameters_tbl_data.cstm_shift_a           = 0;
    ihp_parser_custom_macro_parameters_tbl_data.cstm_shift_b           = 0;
    ihp_parser_custom_macro_parameters_tbl_data.augment_previous_stage = 1;           /* Don't advance */
    ihp_parser_custom_macro_parameters_tbl_data.qualifier_shift        = 0xe;         /* msb 4 bits is Range-or-Protocol (4) */
    ihp_parser_custom_macro_parameters_tbl_data.qualifier_or_mask      = 0x10;        /* Set Has-Options to be 1*/
    ihp_parser_custom_macro_parameters_tbl_data.qualifier_and_mask     = 0x780;       /* Keep msb 4 bits */
    ihp_parser_custom_macro_parameters_tbl_data.header_qualifier_mask  = 0x790;       /* Update header qualifier */

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihp_parser_custom_macro_parameters_tbl_set_unsafe(unit, custom_macro,
                                                                        &ihp_parser_custom_macro_parameters_tbl_data));

    SOCDNX_IF_ERR_EXIT(jer_parser_macro_sel_allocate(unit, DPP_MACRO_IPv6Ext2, &custom_macro));

    ihp_parser_custom_macro_parameters_tbl_data.cstm_word_select       = 30;          /* take Hdr_Ext_Len EH */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_mask_left         = 24;          /* mask anything else */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_mask_right        = 0;
    ihp_parser_custom_macro_parameters_tbl_data.cstm_comparator_mode   = 3;           /* Compare all 32 bits */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_comparator_mask   = 0;           /* Compare none */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_condition_select  = 0;
    ihp_parser_custom_macro_parameters_tbl_data.cstm_shift_sel         = 1;           /* Header-Size = Masked-Word-Shifted + Size-B */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_shift_a           = 3;           /* Size-A[4] = 0, Masked-Word-Shifted = Masked-Word << Size-A[3:0]*/
    ihp_parser_custom_macro_parameters_tbl_data.cstm_shift_b           = 8;           /* constant of first 8 bytes*/
    ihp_parser_custom_macro_parameters_tbl_data.augment_previous_stage = 1;           /* Don't advance */
    ihp_parser_custom_macro_parameters_tbl_data.qualifier_shift        = 0;           /* No upate to qualifier */
    ihp_parser_custom_macro_parameters_tbl_data.qualifier_or_mask      = 0;
    ihp_parser_custom_macro_parameters_tbl_data.qualifier_and_mask     = 0;
    ihp_parser_custom_macro_parameters_tbl_data.header_qualifier_mask  = 0;

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihp_parser_custom_macro_parameters_tbl_set_unsafe(unit, custom_macro,
                                                                        &ihp_parser_custom_macro_parameters_tbl_data));
exit:
    SOCDNX_FUNC_RETURN;
}

static soc_error_t jer_parser_trill(int unit)
{
    ARAD_PP_IHP_PARSER_CUSTOM_MACRO_PARAMETERS_TBL_DATA ihp_parser_custom_macro_parameters_tbl_data;
    uint32                                              custom_macro;

    SOCDNX_INIT_FUNC_DEFS;

    if(!SOC_DPP_CONFIG(unit)->trill.mode)
    {
        return SOC_E_NOT_FOUND;
    }

    DPP_LEAF_CB_GATE(DPP_CB_TRILL);

    SOCDNX_IF_ERR_EXIT(jer_parser_macro_sel_allocate(unit, DPP_MACRO_TRILL, &custom_macro));

    /*
     *  Configure the a macro to identify TRILL present
     *  (macro 3). only the shift matters.
     */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_shift_a = 0x14; /* bit 4 is set */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_shift_b = 6; /* we don't care. size is always 4B */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_comparator_mask = 0x7fff; /* compare all values */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_comparator_mode = 4; /* compare to eth values */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_condition_select = 0; /* we don't care, size is always 4B */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_mask_right = 6;
    ihp_parser_custom_macro_parameters_tbl_data.cstm_mask_left = 21;
    ihp_parser_custom_macro_parameters_tbl_data.cstm_word_select = 30; /* Go back 2 bytes */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_shift_sel = 1; /* we don't care, size is always 4B */
    ihp_parser_custom_macro_parameters_tbl_data.header_qualifier_mask = 0x780; /* change only the next-protocol values */
    ihp_parser_custom_macro_parameters_tbl_data.augment_previous_stage = 0; /* Invisible stage */
    ihp_parser_custom_macro_parameters_tbl_data.qualifier_shift = 14; /* we need protocol value in bits 10:7 of the qualifier to set new next-protocol */
    ihp_parser_custom_macro_parameters_tbl_data.qualifier_or_mask = 0;
    ihp_parser_custom_macro_parameters_tbl_data.qualifier_and_mask = 0x780; /* reset all but next-protocol */

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihp_parser_custom_macro_parameters_tbl_set_unsafe(unit, custom_macro,
                                                                        &ihp_parser_custom_macro_parameters_tbl_data));

exit:
    SOCDNX_FUNC_RETURN;
}

static soc_error_t jer_parser_fcoe(int unit)
{
    ARAD_PP_IHP_PARSER_CUSTOM_MACRO_PARAMETERS_TBL_DATA ihp_parser_custom_macro_parameters_tbl_data;
    ARAD_PP_IHP_PARSER_CUSTOM_MACRO_PROTOCOLS_TBL_DATA  ihp_parser_custom_macro_protocols_tbl_data;
    uint32         tbl_data,        custom_macro;

    SOCDNX_INIT_FUNC_DEFS;

    if(!SOC_DPP_CONFIG(unit)->pp.fcoe_enable)
    {
        return SOC_E_NOT_FOUND;
    }

    DPP_LEAF_CB_GATE(DPP_CB_FCoE);

    SOCDNX_IF_ERR_EXIT(jer_parser_macro_sel_allocate(unit, DPP_MACRO_FCoE1, &custom_macro));

    /* fcoe 1st macro */                                                                                /*comments (taken from verilog file): */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_word_select       = 0;                             /*start from vft (if exist)                                                                                                                                                                                        */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_mask_left         = 0;                             /*                                                                                                                                                                                                                 */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_mask_right        = 24;                            /*compare 8 msb                                                                                                                                                                                                    */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_comparator_mode   = 3;                             /*compare all 32 bits                                                                                                                                                                                              */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_comparator_mask   = (1 << ARAD_PARSER_CUSTOM_MACRO_PROTO_13) |
                                                                         (1 << ARAD_PARSER_CUSTOM_MACRO_PROTO_14); /*compare entries 13,14                                                                      */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_condition_select  = 28/* b11100 */;                /*compare to next address offset bits 4:0                                                                                                                                                                          */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_shift_sel         = 0;                             /*if (par_custom_macro_param_entry.Data.CstmConditionSelect == next_address_offset[4:0]) macro_size = par_custom_macro_param_entry.Data.CstmShiftA; else macro_size = par_custom_macro_param_entry.Data.CstmShiftB;*/
    ihp_parser_custom_macro_parameters_tbl_data.cstm_shift_a           = 8;                             /*VFT size                                                                                                                                                                                                         */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_shift_b           = 24;                            /*Standard size 24B                                                                                                                                                                                                */
    ihp_parser_custom_macro_parameters_tbl_data.augment_previous_stage = 0;                             /*                                                                                                                                                                                                                 */
    ihp_parser_custom_macro_parameters_tbl_data.qualifier_shift        = 15;                            /*we need Protocol value in bits 10:7 of the qualfier to set new next-protocol                                                                                                                                     */
    ihp_parser_custom_macro_parameters_tbl_data.qualifier_or_mask      = 0;                             /*Not relevant                                                                                                                                                                                                     */
    ihp_parser_custom_macro_parameters_tbl_data.qualifier_and_mask     = 0x18/* {4'h0, 2'h3, 3'h0} */;  /*Not relevant                                                                                                                                                                                                     */
    ihp_parser_custom_macro_parameters_tbl_data.header_qualifier_mask  = 0x7ff;                         /*Not relevant                                                                                                                                                                                                     */

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihp_parser_custom_macro_parameters_tbl_set_unsafe(unit, custom_macro,
                                                                        &ihp_parser_custom_macro_parameters_tbl_data));

    SOCDNX_IF_ERR_EXIT(jer_parser_macro_sel_allocate(unit, DPP_MACRO_FCoE2, &custom_macro));

    /* fcoe 2nd macro */                                                                  /*comments (taken from verilog file): */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_word_select       = 1;               /*start after version + reserved                                                                                      */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_mask_left         = 0;               /*                                                                                                                    */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_mask_right        = 0;               /*                                                                                                                    */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_comparator_mode   = 4;               /*compare to eth values                                                                                               */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_comparator_mask   = (1 << ARAD_PARSER_CUSTOM_MACRO_PROTO_13) |
                                                                         (1 << ARAD_PARSER_CUSTOM_MACRO_PROTO_14); /*compare entries 13,14                                                                      */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_condition_select  = 0;               /*we don't care, size is always 4B                                                                                    */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_shift_sel         = 1;               /*Size is data4B << par_custom_macro_param_entry.Data.CstmShiftA[3:0]) + par_custom_macro_param_entry.Data.CstmShiftB */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_shift_a           = 16 /* b10000 */; /*bit 4 is set                                                                                                        */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_shift_b           = 14;              /*size is always 14B                                                                                                  */
    ihp_parser_custom_macro_parameters_tbl_data.augment_previous_stage = 1;               /*invisible stage!                                                                                                    */
    ihp_parser_custom_macro_parameters_tbl_data.qualifier_shift        = 14;              /*we need Protocol value in bits 10:7 of the qualfier to set new next-protocol                                        */
    ihp_parser_custom_macro_parameters_tbl_data.qualifier_or_mask      = 0;               /*                                                                                                                    */
    ihp_parser_custom_macro_parameters_tbl_data.qualifier_and_mask     = 0;               /*reset all                                                                                                           */
    ihp_parser_custom_macro_parameters_tbl_data.header_qualifier_mask  = 0;               /*don't change values of qualifier                                                                                    */

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihp_parser_custom_macro_parameters_tbl_set_unsafe(unit, custom_macro,
                                                                        &ihp_parser_custom_macro_parameters_tbl_data));

    /* protocols for FCoe */

    /* VFT */
    ihp_parser_custom_macro_protocols_tbl_data.cstm_protocol = 0x50000000;
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihp_parser_custom_macro_protocols_tbl_set_unsafe(unit, ARAD_PARSER_CUSTOM_MACRO_PROTO_14,
                                                                                   &ihp_parser_custom_macro_protocols_tbl_data));

    /* encapsulation */
    ihp_parser_custom_macro_protocols_tbl_data.cstm_protocol = 0x52000000;
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihp_parser_custom_macro_protocols_tbl_set_unsafe(unit, ARAD_PARSER_CUSTOM_MACRO_PROTO_15,
                                                                                   &ihp_parser_custom_macro_protocols_tbl_data));

    /* fill memory with the entry in data */
    tbl_data = 1;
    SOCDNX_SAND_IF_ERR_EXIT(WRITE_IHP_PARSER_CUSTOM_MACRO_WORD_MAPm(unit, MEM_BLOCK_ANY,0x50, &tbl_data));

exit:
    SOCDNX_FUNC_RETURN;
}

static soc_error_t jer_parser_gtp(int unit)
{
    ARAD_PP_IHP_PARSER_CUSTOM_MACRO_PARAMETERS_TBL_DATA ihp_parser_custom_macro_parameters_tbl_data;
    ARAD_PP_IHP_PARSER_CUSTOM_MACRO_PROTOCOLS_TBL_DATA  ihp_parser_custom_macro_protocols_tbl_data;
    uint32 custom_macro;

    SOCDNX_INIT_FUNC_DEFS;
    if(!SOC_DPP_CONFIG(unit)->pp.limited_gtp_parsing && !SOC_DPP_CONFIG(unit)->pp.gtp_teid_exists_parsing)
    {
        return SOC_E_NOT_FOUND;
    }

	if(SOC_DPP_L2TP_IS_ENABLE(unit)) 
	{
        LOG_ERROR(BSL_LS_SOC_PKT,(BSL_META_U(0, "GTPoUDPoIPv4: protocol 12 is used by L2TP parsing\n")));
        return SOC_E_NOT_FOUND;
    }

    DPP_LEAF_CB_GATE(DPP_CB_GTP);

    ihp_parser_custom_macro_protocols_tbl_data.cstm_protocol = ARAD_PP_GTP_U_UDP_PORT;
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihp_parser_custom_macro_protocols_tbl_set_unsafe(unit, ARAD_PARSER_CUSTOM_MACRO_PROTO_14,
                                                                                   &ihp_parser_custom_macro_protocols_tbl_data));

    ihp_parser_custom_macro_protocols_tbl_data.cstm_protocol = ARAD_PP_GTP_C_UDP_PORT;
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihp_parser_custom_macro_protocols_tbl_set_unsafe(unit, ARAD_PARSER_CUSTOM_MACRO_PROTO_15,
                                                                                   &ihp_parser_custom_macro_protocols_tbl_data));
    
    if (SOC_DPP_CONFIG(unit)->pp.limited_gtp_parsing) {
    ihp_parser_custom_macro_protocols_tbl_data.cstm_protocol = 0x40000000; /* speculative parsing of IPv4 */
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihp_parser_custom_macro_protocols_tbl_set_unsafe(unit, ARAD_PARSER_CUSTOM_MACRO_PROTO_12,
                                                                                   &ihp_parser_custom_macro_protocols_tbl_data));

    ihp_parser_custom_macro_protocols_tbl_data.cstm_protocol = 0x60000000; /* speculative parsing of IPv4 */
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihp_parser_custom_macro_protocols_tbl_set_unsafe(unit, ARAD_PARSER_CUSTOM_MACRO_PROTO_13,
                                                                                   &ihp_parser_custom_macro_protocols_tbl_data));
    }
   /*For cases 001xxxxx and 010x1xxx in start of GTP packet
    Packet-Format-Qualifier. Next-protocol = 2 */
  if (SOC_DPP_CONFIG(unit)->pp.gtp_teid_exists_parsing) {

      SOCDNX_IF_ERR_EXIT(jer_parser_macro_sel_allocate(unit, DPP_MACRO_GTP, &custom_macro));

      ihp_parser_custom_macro_parameters_tbl_data.cstm_word_select       = 0;  /* 12 bytes size of GTP header, speculate according to first nibble after */      
      ihp_parser_custom_macro_parameters_tbl_data.cstm_mask_left         = 0;   /* take 4 msbs */         
      ihp_parser_custom_macro_parameters_tbl_data.cstm_mask_right        = 0;  /* zero 28 lsbs */        
      ihp_parser_custom_macro_parameters_tbl_data.cstm_comparator_mode   = 3;   /* compare all 32 bits */       
      ihp_parser_custom_macro_parameters_tbl_data.cstm_comparator_mask   = 0;
      ihp_parser_custom_macro_parameters_tbl_data.cstm_condition_select  = 0;
      ihp_parser_custom_macro_parameters_tbl_data.cstm_shift_sel         = 0;
      ihp_parser_custom_macro_parameters_tbl_data.cstm_shift_a           = 12;
      ihp_parser_custom_macro_parameters_tbl_data.cstm_shift_b           = 12;
      ihp_parser_custom_macro_parameters_tbl_data.augment_previous_stage = 1;
      ihp_parser_custom_macro_parameters_tbl_data.qualifier_shift        = 0xc;
      ihp_parser_custom_macro_parameters_tbl_data.qualifier_or_mask      = 0;
      ihp_parser_custom_macro_parameters_tbl_data.qualifier_and_mask     = 0x100;
      ihp_parser_custom_macro_parameters_tbl_data.header_qualifier_mask  = 0x1ff;


    SOCDNX_SAND_IF_ERR_EXIT(jer_parser_add_gtp_teid_word_map(unit));
		    
    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihp_parser_custom_macro_parameters_tbl_set_unsafe(unit, custom_macro,
                                                                        &ihp_parser_custom_macro_parameters_tbl_data));	  
		  
   } else if (SOC_DPP_CONFIG(unit)->pp.limited_gtp_parsing) {
    SOCDNX_IF_ERR_EXIT(jer_parser_macro_sel_allocate(unit, DPP_MACRO_GTP, &custom_macro));

    ihp_parser_custom_macro_parameters_tbl_data.cstm_word_select       = 12;  /* 12 bytes size of GTP header, speculate according to first nibble after */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_mask_left         = 0;   /* take 4 msbs */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_mask_right        = 28;  /* zero 28 lsbs */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_comparator_mode   = 3;   /* compare all 32 bits */
    ihp_parser_custom_macro_parameters_tbl_data.cstm_comparator_mask   = 0x0;
    ihp_parser_custom_macro_parameters_tbl_data.cstm_condition_select  = 0;
    ihp_parser_custom_macro_parameters_tbl_data.cstm_shift_sel         = 0;
    ihp_parser_custom_macro_parameters_tbl_data.cstm_shift_a           = 12;
    ihp_parser_custom_macro_parameters_tbl_data.cstm_shift_b           = 12;
    ihp_parser_custom_macro_parameters_tbl_data.augment_previous_stage = 0;
    ihp_parser_custom_macro_parameters_tbl_data.qualifier_shift        = 0xf;
    ihp_parser_custom_macro_parameters_tbl_data.qualifier_or_mask      = 0;
    ihp_parser_custom_macro_parameters_tbl_data.qualifier_and_mask     = 0x780;
    ihp_parser_custom_macro_parameters_tbl_data.header_qualifier_mask  = 0x780;


    ihp_parser_custom_macro_parameters_tbl_data.cstm_comparator_mask   |= (1 << ARAD_PARSER_CUSTOM_MACRO_PROTO_12);
    ihp_parser_custom_macro_parameters_tbl_data.cstm_comparator_mask   |= (1 << ARAD_PARSER_CUSTOM_MACRO_PROTO_13);

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihp_parser_custom_macro_parameters_tbl_set_unsafe(unit, custom_macro,
                                                                        &ihp_parser_custom_macro_parameters_tbl_data));
   }

exit:
    SOCDNX_FUNC_RETURN;
}

static soc_error_t jer_parser_tm(int unit)
{
    uint32 res;
    ARAD_PP_IHP_PARSER_CUSTOM_MACRO_PARAMETERS_TBL_DATA ihp_parser_custom_macro_parameters_tbl_data;
    ARAD_PP_IHP_PARSER_CUSTOM_MACRO_PROTOCOLS_TBL_DATA  ihp_parser_custom_macro_protocols_tbl_data;
    uint32 custom_macro;

    SOCDNX_INIT_FUNC_DEFS;

    DPP_LEAF_CB_GATE(DPP_CB_TM);

    if(!soc_property_get(unit, spn_ITMH_ARAD_MODE_ENABLE, 0)
       && !soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "itmh_jericho_parse_disable", 0))
    {
        /* Jericho mode */
        /*
         * Configure custom protocols that will be used by custom macro
         */

        /* Extension + Outlif */
        ihp_parser_custom_macro_protocols_tbl_data.cstm_protocol = 0x8 << 5;
        SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihp_parser_custom_macro_protocols_tbl_set_unsafe(unit, ARAD_PARSER_CUSTOM_MACRO_PROTO_1,
                                                                                       &ihp_parser_custom_macro_protocols_tbl_data));

        /* Extension + Multicast-ID */
        ihp_parser_custom_macro_protocols_tbl_data.cstm_protocol = 0x9 << 5;
        SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihp_parser_custom_macro_protocols_tbl_set_unsafe(unit, ARAD_PARSER_CUSTOM_MACRO_PROTO_2,
                                                                                       &ihp_parser_custom_macro_protocols_tbl_data));

        /* Extension + ISQ-Flow-ID */
        ihp_parser_custom_macro_protocols_tbl_data.cstm_protocol = 0xa << 5;
        SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihp_parser_custom_macro_protocols_tbl_set_unsafe(unit, ARAD_PARSER_CUSTOM_MACRO_PROTO_3,
                                                                                       &ihp_parser_custom_macro_protocols_tbl_data));

        /* Extension + Default */
        ihp_parser_custom_macro_protocols_tbl_data.cstm_protocol = 0xf << 5;
        SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihp_parser_custom_macro_protocols_tbl_set_unsafe(unit, ARAD_PARSER_CUSTOM_MACRO_PROTO_4,
                                                                                       &ihp_parser_custom_macro_protocols_tbl_data));

        /*
         *  Configure the macro (macro 0) to identify whether the field ITMH_BASE_ EXTENSION_EXISTS is set.
         *  This is done by comparing the ITMH LSB bit 1.
         */
        ihp_parser_custom_macro_parameters_tbl_data.cstm_word_select = 1;         /* take 3 LSB bytes of ITMH header + 1 MSB of next header*/
        ihp_parser_custom_macro_parameters_tbl_data.cstm_mask_left = 23;          /* mask all bits except extension bit indication and type of extension if exists */
        ihp_parser_custom_macro_parameters_tbl_data.cstm_mask_right = 5;          /* none */
        ihp_parser_custom_macro_parameters_tbl_data.cstm_comparator_mode = 0x3;   /* Compare 32 bits */
        ihp_parser_custom_macro_parameters_tbl_data.cstm_comparator_mask = 0xf;   /* Compare to protocol 1,2,3,4 */
        ihp_parser_custom_macro_parameters_tbl_data.cstm_condition_select = 0;    /* Doesn't matter, size is always 4 bytes */
        ihp_parser_custom_macro_parameters_tbl_data.cstm_shift_sel = 0;           /* Take compare result */
        ihp_parser_custom_macro_parameters_tbl_data.cstm_shift_a = 4;             /* Size is 4 bytes if compare is true */
        ihp_parser_custom_macro_parameters_tbl_data.cstm_shift_b = 4;             /* Size is 4 bytes if compare is false */
        ihp_parser_custom_macro_parameters_tbl_data.augment_previous_stage = 0;   /* separate header */
        ihp_parser_custom_macro_parameters_tbl_data.qualifier_or_mask = 0;        /*  */
        ihp_parser_custom_macro_parameters_tbl_data.qualifier_and_mask = 0;       /*  */
        ihp_parser_custom_macro_parameters_tbl_data.qualifier_shift =  0x1F/*-1*/;/*  */
        ihp_parser_custom_macro_parameters_tbl_data.header_qualifier_mask = 0;    /*  */
    }
    else
    {
      /* ARAD mode */
      /*
       *  Configure the macro to identify whether the destination is of type with destination info ext.
       *  (macro 0). This is done by comparing the ITMH destination (including the destination type) to
       *  to custom protocols 0, 1, 3, 4, 5, 6, which are set according to the destination info ranges.
       */
      /* OUTLIF */
      ihp_parser_custom_macro_protocols_tbl_data.cstm_protocol = ((ARAD_ITMH_DEST_OUT_LIF_ID_PREFIX << 17) | 0x0);
      SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihp_parser_custom_macro_protocols_tbl_set_unsafe(unit, ARAD_PARSER_CUSTOM_MACRO_PROTO_3,
                                                                                    &ihp_parser_custom_macro_protocols_tbl_data));
      ihp_parser_custom_macro_protocols_tbl_data.cstm_protocol = ((ARAD_ITMH_DEST_OUT_LIF_ID_PREFIX << 17) | 0x1ffff);
      SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihp_parser_custom_macro_protocols_tbl_set_unsafe(unit, ARAD_PARSER_CUSTOM_MACRO_PROTO_4,
                                                                                    &ihp_parser_custom_macro_protocols_tbl_data));
      
      /* MC-FLOW-ID */
      ihp_parser_custom_macro_protocols_tbl_data.cstm_protocol = ((ARAD_ITMH_DEST_MC_FLOW_ID_PREFIX << 16) | 0x0);
      SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihp_parser_custom_macro_protocols_tbl_set_unsafe(unit, ARAD_PARSER_CUSTOM_MACRO_PROTO_5,
                                                                                    &ihp_parser_custom_macro_protocols_tbl_data));
      ihp_parser_custom_macro_protocols_tbl_data.cstm_protocol = ((ARAD_ITMH_DEST_MC_FLOW_ID_PREFIX << 16) | 0xffff);
      SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihp_parser_custom_macro_protocols_tbl_set_unsafe(unit, ARAD_PARSER_CUSTOM_MACRO_PROTO_6,
                                                                                    &ihp_parser_custom_macro_protocols_tbl_data));
      
      /* In case of Arad, the Ingress Shaping always enabled */
      res = arad_parser_ingress_shape_state_set(
              unit,
              TRUE,
              0, /* q_low */
              0x1FFFF /*q_high*/
          );
      SOCDNX_SAND_IF_ERR_EXIT(res);
        
      ihp_parser_custom_macro_parameters_tbl_data.cstm_word_select = 0;
      ihp_parser_custom_macro_parameters_tbl_data.cstm_mask_left = 12; /* all bits upto destination*/
      ihp_parser_custom_macro_parameters_tbl_data.cstm_mask_right = 0;
      ihp_parser_custom_macro_parameters_tbl_data.cstm_comparator_mode = 0x3; /* Compare 32 bits */
      ihp_parser_custom_macro_parameters_tbl_data.cstm_comparator_mask = 0x3F; /* Use protocols 0,1,2,3,4,5 */
      ihp_parser_custom_macro_parameters_tbl_data.cstm_condition_select = 3; /* Between protocol 1 and 2 */
      ihp_parser_custom_macro_parameters_tbl_data.cstm_shift_sel = 0; /* take compare result */
      ihp_parser_custom_macro_parameters_tbl_data.cstm_shift_a = 4;
      ihp_parser_custom_macro_parameters_tbl_data.cstm_shift_b = 4;
      ihp_parser_custom_macro_parameters_tbl_data.augment_previous_stage = 0;
      ihp_parser_custom_macro_parameters_tbl_data.qualifier_or_mask = 0;
      ihp_parser_custom_macro_parameters_tbl_data.qualifier_and_mask = 0;
      ihp_parser_custom_macro_parameters_tbl_data.qualifier_shift =  0x1F/*-1*/;
      ihp_parser_custom_macro_parameters_tbl_data.header_qualifier_mask = 0;
    }

    SOCDNX_IF_ERR_EXIT(jer_parser_macro_sel_allocate(unit, DPP_MACRO_TM, &custom_macro));

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihp_parser_custom_macro_parameters_tbl_set_unsafe(unit,
            ARAD_PARSER_MACRO_SEL_TM_BASE + custom_macro, &ihp_parser_custom_macro_parameters_tbl_data));

  /*
   * Macro1 is used to set destination info extension offset size
   * , relevant only if destination info extension exists.
   */
  ihp_parser_custom_macro_parameters_tbl_data.cstm_mask_right = 0;
  ihp_parser_custom_macro_parameters_tbl_data.cstm_mask_left = 0;
  ihp_parser_custom_macro_parameters_tbl_data.cstm_word_select = 0;
  ihp_parser_custom_macro_parameters_tbl_data.cstm_comparator_mask = 0x0;
  ihp_parser_custom_macro_parameters_tbl_data.cstm_comparator_mode = 0x0;
  ihp_parser_custom_macro_parameters_tbl_data.cstm_condition_select = 3; /* Between protocol 1 and 2 */
  ihp_parser_custom_macro_parameters_tbl_data.cstm_shift_sel = 0; /* take compare result */
  ihp_parser_custom_macro_parameters_tbl_data.cstm_shift_a = 3;
  ihp_parser_custom_macro_parameters_tbl_data.cstm_shift_b = 3;
  ihp_parser_custom_macro_parameters_tbl_data.augment_previous_stage = 0;
  ihp_parser_custom_macro_parameters_tbl_data.qualifier_or_mask = 0;
  ihp_parser_custom_macro_parameters_tbl_data.qualifier_and_mask = 0;
  ihp_parser_custom_macro_parameters_tbl_data.qualifier_shift =  0x1F/*-1*/;
  ihp_parser_custom_macro_parameters_tbl_data.header_qualifier_mask = 0;

  SOCDNX_IF_ERR_EXIT(jer_parser_macro_sel_allocate(unit, DPP_MACRO_TM_EXT, &custom_macro));

  SOCDNX_SAND_IF_ERR_EXIT(arad_pp_ihp_parser_custom_macro_parameters_tbl_set_unsafe(unit,
            ARAD_PARSER_MACRO_SEL_TM_BASE + custom_macro, &ihp_parser_custom_macro_parameters_tbl_data));

exit:
  SOCDNX_FUNC_RETURN;
}
/* } End of per protocol init */

/************************************
 * SEGMENT DATA & HANDLING ROUTINES *
 ************************************/
/* { */
DPP_SEGMENT_E segment_map[SOC_MAX_NUM_DEVICES][JER_SEGMENT_NOF];
DPP_SEGMENT_E segment_complex_map[SOC_MAX_NUM_DEVICES][JER_SEGMENT_NOF][JER_SEGMENT_COMPLEX_NOF];

static dpp_leaf_t Base_leafs[] =
{
  /* Offsets
   *      Macro                 Next Segment                PFC                         PLC                 Callback */
  {{{ARAD_PARSER_PROG_RAW_ADDR_START, 1}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_RAW_AND_FTMH,       DPP_PLC_RAW},
  {{{ARAD_PARSER_PROG_ETH_ADDR_START, 1}},
          DPP_MACRO_ETH,        DPP_SEGMENT_ETH,            DPP_PFC_ETH,                DPP_PLC_PP},
  {{{ARAD_PARSER_PROG_TM_ADDR_START, 1}},
          DPP_MACRO_TM,         DPP_SEGMENT_TM,             DPP_PFC_TM,                 DPP_PLC_TM,         jer_parser_tm},
  {{{ARAD_PARSER_PROG_FTMH_ADDR_START, 1}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_RAW_AND_FTMH,       DPP_PLC_FTMH},
  {{{ARAD_PARSER_PROG_RAW_MPLS_ADDR_START, 1}},
          DPP_MACRO_RAW_MPLS,   DPP_SEGMENT_RAW_MPLS,       DPP_PFC_MPLS1_ETH,          DPP_PLC_PP,         jer_parser_raw_mpls},
  {{{0, 16}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_ETH,                DPP_PLC_PP}
};

static dpp_leaf_t TM_leafs[] =
{
  {{{2, 2}},
          DPP_MACRO_TM_EXT,     DPP_SEGMENT_FIN,            DPP_PFC_TM_OUT_LIF,         DPP_PLC_TM_OUT_LIF},
  {{{4, 2}},
          DPP_MACRO_TM_EXT,     DPP_SEGMENT_FIN,            DPP_PFC_TM_MC_FLOW,         DPP_PLC_TM_MC_FLOW},
  {{{6, 2}},
          DPP_MACRO_TM_EXT,     DPP_SEGMENT_FIN,            DPP_PFC_TM_IS,              DPP_PLC_TM_IS},
  {{{8, 1}},
          DPP_MACRO_TM_EXT,     DPP_SEGMENT_FIN,            DPP_PFC_TM,                 DPP_PLC_TM},
  {{{0, 16}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_TM,                 DPP_PLC_TM}
};

static dpp_leaf_t TM_leafs_arad[] =
{
  {{{2, 3}},
          DPP_MACRO_TM_EXT,     DPP_SEGMENT_FIN,            DPP_PFC_TM_IS,              DPP_PLC_TM_IS},
  {{{6, 3}},
          DPP_MACRO_TM_EXT,     DPP_SEGMENT_FIN,            DPP_PFC_TM_OUT_LIF,         DPP_PLC_TM_OUT_LIF},
  {{{10, 3}},
          DPP_MACRO_TM_EXT,     DPP_SEGMENT_FIN,            DPP_PFC_TM_MC_FLOW,         DPP_PLC_TM_MC_FLOW},
  {{{14, 1}},
          DPP_MACRO_TM_EXT,     DPP_SEGMENT_FIN,            DPP_PFC_TM,                 DPP_PLC_TM},
  {{{0, 16}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_TM,                 DPP_PLC_TM}
};

static dpp_leaf_t ETH_leafs[] =
{
  /* Offsets
   *      Macro                 Next Segment                PFC                         PLC                 Callback */
  {{{((ARAD_PARSER_ETHER_PROTO_2_PPPoES + 1) << 2), 4}},
          DPP_MACRO_PPPoES,     DPP_SEGMENT_PPPoE_SESSION,  DPP_PFC_ETH,                DPP_PLC_PP,         jer_parser_pppoes},
  {{{(ARAD_PARSER_ETHER_PROTO_4_ADD_TPID << 2), 1}},
          DPP_MACRO_ADD_TPID,   DPP_SEGMENT_ADD_TPID,       DPP_PFC_ETH,                DPP_PLC_PP,         jer_parser_add_tpid},
  {{{(ARAD_PARSER_ETHER_PROTO_3_EoE << 2), 4},
    {(ARAD_PARSER_ETHER_PROTO_5_EoE << 2), 4}},
          DPP_MACRO_EoE,        DPP_SEGMENT_FIN,            DPP_PFC_ETH,                DPP_PLC_PP,         jer_parser_eoe},
  {{{(DPP_PARSER_ETH_NEXT_PROTOCOL_IPv4 << 2), 4}},
          DPP_MACRO_IPv4,       DPP_SEGMENT_IPv4,           DPP_PFC_ETH,                DPP_PLC_PP},
  {{{(DPP_PARSER_ETH_NEXT_PROTOCOL_IPv6 << 2), 4}},
          DPP_MACRO_IPv6,       DPP_SEGMENT_IPv6,           DPP_PFC_ETH,                DPP_PLC_PP},
  {{{(DPP_PARSER_ETH_NEXT_PROTOCOL_MPLS << 2), 4},
    {((ARAD_PARSER_ETHER_PROTO_7_MPLS_MC + 1) << 2), 4}},
          DPP_MACRO_MPLS,       DPP_SEGMENT_MPLS,           DPP_PFC_ETH,                DPP_PLC_PP},
  {{{(DPP_PARSER_ETH_NEXT_PROTOCOL_TRILL << 2), 4}},
          DPP_MACRO_TRILL,      DPP_SEGMENT_TRILL,          DPP_PFC_ETH,                DPP_PLC_PP,         jer_parser_trill},
  {{{(DPP_PARSER_ETH_NEXT_PROTOCOL_FCoE << 2), 4}},
          DPP_MACRO_FCoE2,      DPP_SEGMENT_FCoE1,          DPP_PFC_ETH,                DPP_PLC_PP,         jer_parser_fcoe},
  {{{(DPP_PARSER_ETH_NEXT_PROTOCOL_MinM << 2), 2, 2}},
          DPP_MACRO_ETH,        DPP_SEGMENT_FIN,            DPP_PFC_ETH,                DPP_PLC_PP},
  {{{(DPP_PARSER_ETH_NEXT_PROTOCOL_MinM << 2) + 1, 2, 2}},
          DPP_MACRO_ETH,        DPP_SEGMENT_MinM,           DPP_PFC_ETH_ETH,            DPP_PLC_PP,         jer_parser_minm},
  {{{0, 64}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_ETH,                DPP_PLC_PP}
};

static dpp_leaf_t MinM_leafs[] =
{
  {{{(DPP_PARSER_ETH_NEXT_PROTOCOL_IPv4 << 2), 4}},
          DPP_MACRO_IPv4,       DPP_SEGMENT_FIN,            DPP_PFC_ETH_ETH,            DPP_PLC_PP},
  {{{(DPP_PARSER_ETH_NEXT_PROTOCOL_IPv6 << 2), 4}},
          DPP_MACRO_IPv6,       DPP_SEGMENT_FIN,            DPP_PFC_ETH_ETH,            DPP_PLC_PP},
  {{{0, 64}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_ETH_ETH,            DPP_PLC_PP},
};

static dpp_leaf_t IPv4_leafs[] =
{
  /* Offsets
   *      Macro                 Next Segment                PFC                         PLC                 Callback */
  {{{4, 2}},
          DPP_MACRO_ETH,        DPP_SEGMENT_FIN,            DPP_PFC_ETH_IPV4_ETH,       DPP_PLC_PP},
  {{{(DPP_PARSER_IP_NEXT_PROTOCOL_IPv4 << 1), 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_IPv4 << 1) + JER_IP_MC, 2}},
          DPP_MACRO_IPv4,       DPP_SEGMENT_IPv4oIPv4,      DPP_PFC_IPV4_IPV4_ETH,      DPP_PLC_PP},
  {{{(DPP_PARSER_IP_NEXT_PROTOCOL_IPv6 << 1), 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_IPv6 << 1) + JER_IP_MC, 2}},
          DPP_MACRO_IPv6,       DPP_SEGMENT_FIN,            DPP_PFC_IPV6_IPV4_ETH,      DPP_PLC_PP},
  {{{(DPP_PARSER_IP_NEXT_PROTOCOL_MPLS << 1), 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_MPLS << 1) + JER_IP_MC, 2}},
          DPP_MACRO_MPLS,       DPP_SEGMENT_FIN,            DPP_PFC_ETH_IPV4_ETH,       DPP_PLC_PP,         jer_parser_mpls_ipv4},
  /* L2TP Takes precedence */
  {{{(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1), 2},
	{(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1) + JER_IP_MC, 2}},
		  DPP_MACRO_L2TP1,		DPP_SEGMENT_L2TP,		DPP_PFC_IPV4_ETH,			DPP_PLC_PP_L4,		jer_parser_l2tp},
  /* VxLAN after L2TP */
  {{{(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1), 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1) + JER_IP_MC, 2}},
          DPP_MACRO_VxLAN,      DPP_SEGMENT_UDPoIPv4,       DPP_PFC_IPV4_ETH,           DPP_PLC_PP_L4,      jer_parser_vxlan},
  /* UDP Tunnel */
  {{{(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1), 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1) + JER_IP_MC, 2}},
          DPP_MACRO_UDP_TUNNEL,DPP_SEGMENT_UDPoIPv4,        DPP_PFC_IPV4_ETH,           DPP_PLC_PP_L4,      jer_parser_udp_tunnel},
  /* UDP Parsing if underlying protocol parsing (GTP/BFD) is required */
  {{{(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1), 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1) + JER_IP_MC, 2}},
          DPP_MACRO_UDP,        DPP_SEGMENT_UDPoIPv4,       DPP_PFC_IPV4_ETH,           DPP_PLC_PP_L4,      jer_parser_udp_ipv4_protos},
    {{{(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1), 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1) + JER_IP_MC, 2}},
          DPP_MACRO_UDP,        DPP_SEGMENT_UDPoIPv4_gtp,       DPP_PFC_IPV4_ETH,           DPP_PLC_PP_L4,      jer_parser_udp_ipv4_gtp_protos},
  /* Default UDP Parsing */
  {{{(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1), 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1) + JER_IP_MC, 2}},
          DPP_MACRO_UDP,        DPP_SEGMENT_FIN,            DPP_PFC_IPV4_ETH,           DPP_PLC_PP_L4,      jer_parser_udp},
  /* For TCP always for UDP default, when UDP parsing is disabled */
  {{{(DPP_PARSER_IP_NEXT_PROTOCOL_TCP << 1), 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_TCP << 1) + JER_IP_MC, 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1), 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1) + JER_IP_MC, 2}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_IPV4_ETH,           DPP_PLC_PP_L4},
  {{{0, 64}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_IPV4_ETH,           DPP_PLC_PP}
};

static dpp_leaf_t L2TP_leafs[] =
{
  {{{(ARAD_PARSER_CUSTOM_MACRO_PROTO_12 + 1) << 1, 1}},
      DPP_MACRO_L2TP2,      DPP_SEGMENT_FIN,            DPP_PFC_IPV4_ETH,           DPP_PLC_PP_L4,		jer_parser_l2tp},
  {{{0, 32}},
	  DPP_MACRO_VxLAN,		DPP_SEGMENT_UDPoIPv4,		DPP_PFC_IPV4_ETH,			DPP_PLC_PP_L4,		jer_parser_vxlan},
  {{{0, 32}},
	  DPP_MACRO_UDP_TUNNEL,DPP_SEGMENT_UDPoIPv4,		DPP_PFC_IPV4_ETH,			DPP_PLC_PP_L4,		jer_parser_udp_tunnel},
  {{{0, 32}},
	  DPP_MACRO_UDP,		DPP_SEGMENT_UDPoIPv4,		DPP_PFC_IPV4_ETH,			DPP_PLC_PP_L4,		jer_parser_udp_ipv4_protos},
  {{{0, 32}},
	  DPP_MACRO_UDP,		DPP_SEGMENT_FIN,			DPP_PFC_IPV4_ETH,			DPP_PLC_PP_L4,		jer_parser_udp},
  {{{0, 32}},
	  DPP_MACRO_NA, 		DPP_SEGMENT_END,			DPP_PFC_IPV4_ETH,			DPP_PLC_PP_L4},
};

static dpp_leaf_t IPv6_leafs[] =
{
  {{{2, 14},
    {2 + JER_IP_MC, 14}},
          DPP_MACRO_IPv6Ext1,   DPP_SEGMENT_IPv6Ext1,       DPP_PFC_IPV6_ETH,           DPP_PLC_PP,         jer_parser_ipv6_ext},
  {{{(DPP_PARSER_IP_NEXT_PROTOCOL_IPv4 << 1), 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_IPv4 << 1) + JER_IP_MC, 2}},
          DPP_MACRO_IPv4,       DPP_SEGMENT_FIN,            DPP_PFC_IPV4_IPV6_ETH,      DPP_PLC_PP},
  {{{(DPP_PARSER_IP_NEXT_PROTOCOL_IPv6 << 1), 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_IPv6 << 1) + JER_IP_MC, 2}},
          DPP_MACRO_IPv6,       DPP_SEGMENT_FIN,            DPP_PFC_IPV6_IPV6_ETH,      DPP_PLC_PP},
  /* VxLAN Takes precedence*/
  {{{(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1), 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1) + JER_IP_MC, 2}},
          DPP_MACRO_VxLAN,      DPP_SEGMENT_UDPoIPv6,       DPP_PFC_IPV6_ETH,           DPP_PLC_PP_L4,      jer_parser_vxlan6},
  /* UDP Parsing if underlying protocol parsing (GTP/BFD) is required */
  {{{(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1), 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1) + JER_IP_MC, 2}},
          DPP_MACRO_UDP,        DPP_SEGMENT_UDPoIPv6,       DPP_PFC_IPV6_ETH,           DPP_PLC_PP_L4,      jer_parser_udp_ipv6_protos},
  /* Basic UDP Parsing */
  {{{(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1), 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1) + JER_IP_MC, 2}},
          DPP_MACRO_UDP,        DPP_SEGMENT_FIN,            DPP_PFC_IPV6_ETH,           DPP_PLC_PP_L4,      jer_parser_udp},
  /* For TCP always for UDP default, when UDP parsing is disabled */
  {{{(DPP_PARSER_IP_NEXT_PROTOCOL_TCP << 1), 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_TCP << 1) + JER_IP_MC, 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1), 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1) + JER_IP_MC, 2}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_IPV6_ETH,           DPP_PLC_PP_L4},
  {{{0, 64}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_IPV6_ETH,           DPP_PLC_PP}
};

static dpp_leaf_t IPv6Ext1_leafs[] =
{
  /* unknown protocols Parsing */
  {{{0, 2},
    {JER_IP_MC, 2}},
          DPP_MACRO_IPv6Ext2,   DPP_SEGMENT_FIN,            DPP_PFC_IPV6_ETH,           DPP_PLC_PP},
  /* TCP Parsing */
  {{{(DPP_PARSER_IP_NEXT_PROTOCOL_TCP << 1), 1},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_TCP << 1) + JER_IP_MC, 1}},
          DPP_MACRO_IPv6Ext2,   DPP_SEGMENT_FIN,            DPP_PFC_IPV6_ETH,           DPP_PLC_PP},
  /* UDP Parsing */
  {{{(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1), 1},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1) + JER_IP_MC, 1}},
          DPP_MACRO_IPv6Ext2,   DPP_SEGMENT_IPv6Ext2_UDP,   DPP_PFC_IPV6_ETH,           DPP_PLC_PP},
  /* IGMP Parsing */
  {{{(DPP_PARSER_IP_NEXT_PROTOCOL_IGMP << 1), 1},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_IGMP << 1) + JER_IP_MC, 1}},
          DPP_MACRO_IPv6Ext2,   DPP_SEGMENT_FIN,            DPP_PFC_IPV6_ETH,           DPP_PLC_PP},
  /* ICMP Parsing */
  {{{(DPP_PARSER_IP_NEXT_PROTOCOL_ICMP << 1), 1},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_ICMP << 1) + JER_IP_MC, 1}},
          DPP_MACRO_IPv6Ext2,   DPP_SEGMENT_FIN,            DPP_PFC_IPV6_ETH,           DPP_PLC_PP},
  /* ICMPv6 Parsing */
  {{{(DPP_PARSER_IP_NEXT_PROTOCOL_ICMPv6 << 1), 1},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_ICMPv6 << 1) + JER_IP_MC, 1}},
          DPP_MACRO_IPv6Ext2,   DPP_SEGMENT_FIN,            DPP_PFC_IPV6_ETH,           DPP_PLC_PP},
  /* IPv4 Parsing */
  {{{(DPP_PARSER_IP_NEXT_PROTOCOL_IPv4 << 1), 1},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_IPv4 << 1) + JER_IP_MC, 1}},
          DPP_MACRO_IPv6Ext2,   DPP_SEGMENT_IPv6Ext2_IPv4,  DPP_PFC_IPV6_ETH,           DPP_PLC_PP},
  /* IPv6 Parsing */
  {{{(DPP_PARSER_IP_NEXT_PROTOCOL_IPv6 << 1), 1},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_IPv6 << 1) + JER_IP_MC, 1}},
          DPP_MACRO_IPv6Ext2,   DPP_SEGMENT_IPv6Ext2_IPv6,  DPP_PFC_IPV6_ETH,           DPP_PLC_PP},
  {{{0, 64}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_IPV6_ETH,           DPP_PLC_PP}
};

static dpp_leaf_t IPv6Ext2_UDP_leafs[] =
{
  {{{0, 16}},
          DPP_MACRO_UDP,        DPP_SEGMENT_FIN,            DPP_PFC_IPV6_ETH,           DPP_PLC_PP_L4,      jer_parser_udp},
  {{{0, 16}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_IPV6_ETH,           DPP_PLC_PP_L4}
};

static dpp_leaf_t IPv6Ext2_IPv4_leafs[] =
{
  {{{0, 16}},
          DPP_MACRO_IPv4,       DPP_SEGMENT_FIN,            DPP_PFC_IPV6_ETH,           DPP_PLC_PP}
};

static dpp_leaf_t IPv6Ext2_IPv6_leafs[] =
{
  {{{0, 16}},
          DPP_MACRO_IPv6,       DPP_SEGMENT_FIN,            DPP_PFC_IPV6_ETH,           DPP_PLC_PP}
};

static dpp_leaf_t MPLS_leafs[] =
{
  /* Offset    Macro            Next Segment                PFC                         PLC                 Init Callback */
  {{{(MPLS_SPECULATE_NEXT_PROTOCOL_NDX_ETH << 2) + 0, 1}},
          DPP_MACRO_ETH,        DPP_SEGMENT_ETHoMPLS1,      DPP_PFC_MPLS1_ETH,          DPP_PLC_ETHisH3},
  {{{(MPLS_SPECULATE_NEXT_PROTOCOL_NDX_ETH << 2) + 1, 1}},
          DPP_MACRO_ETH,        DPP_SEGMENT_ETHoMPLS2,      DPP_PFC_MPLS2_ETH,          DPP_PLC_ETHisH3},
  {{{(MPLS_SPECULATE_NEXT_PROTOCOL_NDX_ETH << 2) + 2, 1}},
          DPP_MACRO_ETH,        DPP_SEGMENT_ETHoMPLS3,      DPP_PFC_MPLS3_ETH,          DPP_PLC_ETHisH3},
  {{{(MPLS_SPECULATE_NEXT_PROTOCOL_NDX_IPV4 << 2) + 0, 1}},
          DPP_MACRO_IPv4,       DPP_SEGMENT_IPv4oMPLS1,     DPP_PFC_MPLS1_ETH,          DPP_PLC_IPv4isH3},
  {{{(MPLS_SPECULATE_NEXT_PROTOCOL_NDX_IPV4 << 2) + 1, 1}},
          DPP_MACRO_IPv4,       DPP_SEGMENT_IPv4oMPLS2,     DPP_PFC_MPLS2_ETH,          DPP_PLC_IPv4isH3},
  {{{(MPLS_SPECULATE_NEXT_PROTOCOL_NDX_IPV4 << 2) + 2, 1}},
          DPP_MACRO_IPv4,       DPP_SEGMENT_IPv4oMPLS3,     DPP_PFC_MPLS3_ETH,          DPP_PLC_IPv4isH3},
  {{{(MPLS_SPECULATE_NEXT_PROTOCOL_NDX_IPV6 << 2) + 0, 1}},
          DPP_MACRO_IPv6,       DPP_SEGMENT_IPv6oMPLS1,     DPP_PFC_MPLS1_ETH,          DPP_PLC_IPv6isH3},
  {{{(MPLS_SPECULATE_NEXT_PROTOCOL_NDX_IPV6 << 2) + 1, 1}},
          DPP_MACRO_IPv6,       DPP_SEGMENT_IPv6oMPLS2,     DPP_PFC_MPLS2_ETH,          DPP_PLC_IPv6isH3},
  {{{(MPLS_SPECULATE_NEXT_PROTOCOL_NDX_IPV6 << 2) + 2, 1}},
          DPP_MACRO_IPv6,       DPP_SEGMENT_IPv6oMPLS3,     DPP_PFC_MPLS3_ETH,          DPP_PLC_IPv6isH3},
  /* In case CW parsing is enabled */
  {{{(MPLS_SPECULATE_NEXT_PROTOCOL_NDX_CW << 2) + 0, 1}},
          DPP_MACRO_MPLS_CW,    DPP_SEGMENT_CWoMPLS1,      DPP_PFC_MPLS1_ETH,          DPP_PLC_ETHisH3, jer_parser_mpls_cw},
  {{{(MPLS_SPECULATE_NEXT_PROTOCOL_NDX_CW << 2) + 1, 1}},
          DPP_MACRO_MPLS_CW,    DPP_SEGMENT_CWoMPLS2,      DPP_PFC_MPLS2_ETH,          DPP_PLC_ETHisH3, jer_parser_mpls_cw},
  {{{(MPLS_SPECULATE_NEXT_PROTOCOL_NDX_CW << 2) + 2, 1}},
          DPP_MACRO_MPLS_CW,    DPP_SEGMENT_CWoMPLS3,      DPP_PFC_MPLS3_ETH,          DPP_PLC_ETHisH3, jer_parser_mpls_cw},
  /* In case CW parsing is disabled */
  {{{(MPLS_SPECULATE_NEXT_PROTOCOL_NDX_CW << 2) + 0, 1}},
          DPP_MACRO_MPLS_CW,    DPP_SEGMENT_END,           DPP_PFC_MPLS1_ETH,          DPP_PLC_PP},
  {{{(MPLS_SPECULATE_NEXT_PROTOCOL_NDX_CW << 2) + 1, 1}},
          DPP_MACRO_MPLS_CW,    DPP_SEGMENT_END,           DPP_PFC_MPLS2_ETH,          DPP_PLC_PP},
  {{{(MPLS_SPECULATE_NEXT_PROTOCOL_NDX_CW << 2) + 2, 1}},
          DPP_MACRO_MPLS_CW,    DPP_SEGMENT_END,           DPP_PFC_MPLS3_ETH,          DPP_PLC_PP},
  /* More than 3 labels always going for further parsing */
  {{{0, 16, 4}},
          DPP_MACRO_ETH,        DPP_SEGMENT_FIN,            DPP_PFC_MPLS1_ETH,          DPP_PLC_PP},
  {{{1, 16, 4}},
          DPP_MACRO_ETH,        DPP_SEGMENT_FIN,            DPP_PFC_MPLS2_ETH,          DPP_PLC_PP},
  {{{2, 16, 4}},
          DPP_MACRO_ETH,        DPP_SEGMENT_FIN,            DPP_PFC_MPLS3_ETH,          DPP_PLC_PP},
  {{{3, 16, 4}},
          DPP_MACRO_MPLS,       DPP_SEGMENT_MPLSoMPLS3,     DPP_PFC_MPLS3_ETH,          DPP_PLC_PP},
};

static dpp_leaf_t UDPoIPv4_leafs[] =
{
  {{{((ARAD_PARSER_CUSTOM_MACRO_PROTO_7 + 1) << 1), 1},
    {((ARAD_PARSER_CUSTOM_MACRO_PROTO_9 + 1) << 1), 1}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_IPV4_ETH,           DPP_PLC_PP_L4, jer_parser_bfd_ipv4_transparent},
  {{{((ARAD_PARSER_CUSTOM_MACRO_PROTO_7 + 1) << 1), 1},
    {((ARAD_PARSER_CUSTOM_MACRO_PROTO_9 + 1) << 1), 1},
    {((ARAD_PARSER_CUSTOM_MACRO_PROTO_15 + 1) << 1),1}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_IPV4_ETH,           DPP_PLC_BFD_SINGLE_HOP, jer_parser_bfd_ipv4},
  {{{((ARAD_PARSER_CUSTOM_MACRO_PROTO_8 + 1) << 1), 1},
    {((ARAD_PARSER_CUSTOM_MACRO_PROTO_8 + 1) << 1) + JER_IP_MC, 1}},
          DPP_MACRO_ETH,        DPP_SEGMENT_VxLAN,          DPP_PFC_IPV4_ETH,           DPP_PLC_PP,             jer_parser_vxlan},
  {{{((ARAD_PARSER_CUSTOM_MACRO_PROTO_13 + 1) << 1), 1}},
          DPP_MACRO_IPv4,       DPP_SEGMENT_END,            DPP_PFC_IPV4_ETH,           DPP_PLC_PP,             jer_parser_udp_tunnel},
  {{{((ARAD_PARSER_CUSTOM_MACRO_PROTO_14 + 1) << 1), 1}},
          DPP_MACRO_IPv6,       DPP_SEGMENT_END,            DPP_PFC_IPV4_ETH,           DPP_PLC_PP,             jer_parser_udp_tunnel},
  {{{((ARAD_PARSER_CUSTOM_MACRO_PROTO_15 + 1) << 1), 1}},
          DPP_MACRO_MPLS,       DPP_SEGMENT_END,            DPP_PFC_IPV4_ETH,           DPP_PLC_PP,             jer_parser_udp_tunnel},
    {{{((ARAD_PARSER_CUSTOM_MACRO_PROTO_14 + 1) << 1), 1},
    {((ARAD_PARSER_CUSTOM_MACRO_PROTO_15 + 1) << 1), 1}},
          DPP_MACRO_GTP,        DPP_SEGMENT_GTPoUDPoIPv4,   DPP_PFC_IPV4_ETH,           DPP_PLC_PP,             jer_parser_gtp},
  /* Ranges that allow to catch BFD when udp-tunnel is enabled
   * When UDP Destination Port is NULL, UDP Tunnel macro has match to null entry in segment, it also updates IPv4 header offset
   */
  {{{0, 1},
   {ARAD_PARSER_CUSTOM_MACRO_PROTO_RANGE_9_10, 1}},
          DPP_MACRO_UDP,        DPP_SEGMENT_UDPoIPv4_2,     DPP_PFC_IPV4_ETH,           DPP_PLC_PP_L4,          jer_parser_udp_tunnel},
  /* Ranges that allow to catch BFD when vxlan is enabled */
  {{{ARAD_PARSER_CUSTOM_MACRO_PROTO_RANGE_7_8, 1},
    {ARAD_PARSER_CUSTOM_MACRO_PROTO_RANGE_AFT_15, 1}},
          DPP_MACRO_UDP,        DPP_SEGMENT_UDPoIPv4_2,     DPP_PFC_IPV4_ETH,           DPP_PLC_PP_L4,          jer_parser_vxlan},
  {{{0, 32}},
          DPP_MACRO_UDP,        DPP_SEGMENT_FIN,            DPP_PFC_IPV4_ETH,           DPP_PLC_PP_L4}
};


static dpp_leaf_t UDPoIPv4_gtp_leafs[] =
{
  {{{((ARAD_PARSER_CUSTOM_MACRO_PROTO_7 + 1) << 1), 1},
    {((ARAD_PARSER_CUSTOM_MACRO_PROTO_9 + 1) << 1), 1}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_IPV4_ETH,           DPP_PLC_PP_L4, jer_parser_bfd_ipv4_transparent},
  {{{((ARAD_PARSER_CUSTOM_MACRO_PROTO_7 + 1) << 1), 1},
    {((ARAD_PARSER_CUSTOM_MACRO_PROTO_9 + 1) << 1), 1},
    {((ARAD_PARSER_CUSTOM_MACRO_PROTO_15 + 1) << 1),1}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_IPV4_ETH,           DPP_PLC_BFD_SINGLE_HOP, jer_parser_bfd_ipv4},
  {{{((ARAD_PARSER_CUSTOM_MACRO_PROTO_8 + 1) << 1), 1},
    {((ARAD_PARSER_CUSTOM_MACRO_PROTO_8 + 1) << 1) + JER_IP_MC, 1}},
          DPP_MACRO_ETH,        DPP_SEGMENT_VxLAN,          DPP_PFC_IPV4_ETH,           DPP_PLC_PP,             jer_parser_vxlan},
  {{{((ARAD_PARSER_CUSTOM_MACRO_PROTO_13 + 1) << 1), 1}},
          DPP_MACRO_IPv4,       DPP_SEGMENT_END,            DPP_PFC_IPV4_ETH,           DPP_PLC_PP,             jer_parser_udp_tunnel},
  {{{((ARAD_PARSER_CUSTOM_MACRO_PROTO_14 + 1) << 1), 1}},
          DPP_MACRO_IPv6,       DPP_SEGMENT_END,            DPP_PFC_IPV4_ETH,           DPP_PLC_PP,             jer_parser_udp_tunnel},
  {{{((ARAD_PARSER_CUSTOM_MACRO_PROTO_15 + 1) << 1), 1}},
          DPP_MACRO_MPLS,       DPP_SEGMENT_END,            DPP_PFC_IPV4_ETH,           DPP_PLC_PP,             jer_parser_udp_tunnel},
    {{{((ARAD_PARSER_CUSTOM_MACRO_PROTO_14 + 1) << 1), 1},
    {((ARAD_PARSER_CUSTOM_MACRO_PROTO_15 + 1) << 1), 1}},
          DPP_MACRO_GTP,        DPP_SEGMENT_GTPoUDPoIPv4,   DPP_PFC_IPV4_ETH,           DPP_PLC_PP,             jer_parser_gtp},
  /* Ranges that allow to catch BFD when udp-tunnel is enabled
   * When UDP Destination Port is NULL, UDP Tunnel macro has match to null entry in segment, it also updates IPv4 header offset
   */
  {{{0, 1},
   {ARAD_PARSER_CUSTOM_MACRO_PROTO_RANGE_9_10, 1}},
          DPP_MACRO_UDP,        DPP_SEGMENT_UDPoIPv4_2,     DPP_PFC_IPV4_ETH,           DPP_PLC_PP_L4,          jer_parser_udp_tunnel},
  /* Ranges that allow to catch BFD when vxlan is enabled */
  {{{ARAD_PARSER_CUSTOM_MACRO_PROTO_RANGE_7_8, 1},
    {ARAD_PARSER_CUSTOM_MACRO_PROTO_RANGE_AFT_15, 1}},
          DPP_MACRO_UDP,        DPP_SEGMENT_UDPoIPv4_2,     DPP_PFC_IPV4_ETH,           DPP_PLC_PP_L4,          jer_parser_vxlan},
  {{{0, 32}},
          DPP_MACRO_NA,        DPP_SEGMENT_END,            DPP_PFC_IPV4_ETH,           DPP_PLC_PP_L4}
};

static dpp_leaf_t GTPoUDPoIPv4_leafs[] =
{
  {{{((ARAD_PARSER_CUSTOM_MACRO_PROTO_12 + 1) << 1), 1}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_IPV4_ETH,           DPP_PLC_IP_UDP_GTP1},
  {{{((ARAD_PARSER_CUSTOM_MACRO_PROTO_13 + 1) << 1), 1}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_IPV4_ETH,           DPP_PLC_IP_UDP_GTP2},
  {{{0, 32}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_IPV4_ETH,           DPP_PLC_PP_L4}
};

static dpp_leaf_t UDPoIPv4_2_leafs[] =
{
  {{{((ARAD_PARSER_CUSTOM_MACRO_PROTO_7 + 1) << 1), 1},
    {((ARAD_PARSER_CUSTOM_MACRO_PROTO_9 + 1) << 1), 1}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_IPV4_ETH,           DPP_PLC_PP_L4, jer_parser_bfd_ipv4_transparent},
  {{{((ARAD_PARSER_CUSTOM_MACRO_PROTO_7 + 1) << 1), 1},
    {((ARAD_PARSER_CUSTOM_MACRO_PROTO_9 + 1) << 1), 1}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_IPV4_ETH,           DPP_PLC_BFD_SINGLE_HOP, jer_parser_bfd_ipv4},
  {{{0, 32}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_IPV4_ETH,           DPP_PLC_PP_L4}
};

static dpp_leaf_t VxLAN_leafs[] =
{
  {{{(DPP_PARSER_ETH_NEXT_PROTOCOL_IPv4 << 2), 4}},
          DPP_MACRO_IPv4,       DPP_SEGMENT_FIN,            DPP_PFC_ETH_IPV4_ETH,       DPP_PLC_PP_L4},
  {{{(DPP_PARSER_ETH_NEXT_PROTOCOL_IPv6 << 2), 4}},
          DPP_MACRO_IPv6,       DPP_SEGMENT_FIN,            DPP_PFC_ETH_IPV4_ETH,       DPP_PLC_PP_L4},
  {{{(DPP_PARSER_ETH_NEXT_PROTOCOL_MPLS << 2), 4}},
          DPP_MACRO_MPLS,       DPP_SEGMENT_FIN,            DPP_PFC_ETH_IPV4_ETH,       DPP_PLC_PP_L4},
  {{{0, 64}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_ETH_IPV4_ETH,       DPP_PLC_PP_L4}
};

static dpp_leaf_t VxLAN6_leafs[] =
{
  {{{(DPP_PARSER_ETH_NEXT_PROTOCOL_IPv4 << 2), 4}},
          DPP_MACRO_IPv4,       DPP_SEGMENT_FIN,            DPP_PFC_ETH_IPV6_ETH,       DPP_PLC_PP_L4},
  {{{(DPP_PARSER_ETH_NEXT_PROTOCOL_IPv6 << 2), 4}},
          DPP_MACRO_IPv6,       DPP_SEGMENT_FIN,            DPP_PFC_ETH_IPV6_ETH,       DPP_PLC_PP_L4},
  {{{(DPP_PARSER_ETH_NEXT_PROTOCOL_MPLS << 2), 4}},
          DPP_MACRO_MPLS,       DPP_SEGMENT_FIN,            DPP_PFC_ETH_IPV6_ETH,       DPP_PLC_PP_L4},
  {{{0, 64}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_ETH_IPV6_ETH,       DPP_PLC_PP_L4}
};

static dpp_leaf_t UDPoIPv6_leafs[] =
{
  {{{((ARAD_PARSER_CUSTOM_MACRO_PROTO_7 + 1) << 1), 1},
    {((ARAD_PARSER_CUSTOM_MACRO_PROTO_9 + 1) << 1), 1}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_IPV6_ETH,           DPP_PLC_PP_L4, jer_parser_bfd_ipv6_transparent},
  {{{((ARAD_PARSER_CUSTOM_MACRO_PROTO_7 + 1) << 1), 1},
    {((ARAD_PARSER_CUSTOM_MACRO_PROTO_9 + 1) << 1), 1},
    {((ARAD_PARSER_CUSTOM_MACRO_PROTO_15 + 1) << 1),1}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_IPV6_ETH,           DPP_PLC_BFD_SINGLE_HOP, jer_parser_bfd_ipv6},
  {{{((ARAD_PARSER_CUSTOM_MACRO_PROTO_8 + 1) << 1), 1},
    {((ARAD_PARSER_CUSTOM_MACRO_PROTO_8 + 1) << 1) + JER_IP_MC, 1}},
          DPP_MACRO_ETH,        DPP_SEGMENT_VxLAN6,         DPP_PFC_IPV6_ETH,           DPP_PLC_PP,             jer_parser_vxlan6},
  {{{((ARAD_PARSER_CUSTOM_MACRO_PROTO_14 + 1) << 1), 1},
    {((ARAD_PARSER_CUSTOM_MACRO_PROTO_15 + 1) << 1), 1}},
          DPP_MACRO_GTP,        DPP_SEGMENT_GTPoUDPoIPv6,   DPP_PFC_IPV6_ETH,           DPP_PLC_PP,             jer_parser_gtp},
  {{{0, 32}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_IPV6_ETH,           DPP_PLC_PP_L4}
};

static dpp_leaf_t GTPoUDPoIPv6_leafs[] =
{
  {{{((ARAD_PARSER_CUSTOM_MACRO_PROTO_12 + 1) << 1), 1}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_IPV6_ETH,           DPP_PLC_IP_UDP_GTP1},
  {{{((ARAD_PARSER_CUSTOM_MACRO_PROTO_13 + 1) << 1), 1}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_IPV6_ETH,           DPP_PLC_IP_UDP_GTP2},
  {{{0, 32}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_IPV6_ETH,           DPP_PLC_PP_L4}
};

static dpp_leaf_t TRILL_leafs[] =
{
  {{{0, 64}},
          DPP_MACRO_ETH,        DPP_SEGMENT_EoTRILL,        DPP_PFC_ETH,                DPP_PLC_PP}
};

static dpp_leaf_t EoTRILL_leafs[] =
{
  {{{0, 64}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_ETH_TRILL_ETH,      DPP_PLC_PP}
};

static dpp_leaf_t FCoE1_leafs[] =
{
  {{{0, 64}},
          DPP_MACRO_FCoE1,      DPP_SEGMENT_FCoE2,          DPP_PFC_FCOE_STD_ETH,       DPP_PLC_FCOE}
};

static dpp_leaf_t FCoE2_leafs[] =
{
  {{{30, 1}},
          DPP_MACRO_FCoE1,      DPP_SEGMENT_FCoE3,          DPP_PFC_FCOE_STD_ETH,       DPP_PLC_FCOE},
  {{{60, 1}},
          DPP_MACRO_FCoE1,      DPP_SEGMENT_FIN,            DPP_PFC_FCOE_STD_ETH,       DPP_PLC_FCOE_VFT},
  {{{0, 64}},
          DPP_MACRO_FCoE1,      DPP_SEGMENT_END,            DPP_PFC_FCOE_STD_ETH,       DPP_PLC_FCOE}
};

static dpp_leaf_t FCoE3_leafs[] =
{
  {{{60, 1}},
          DPP_MACRO_FCoE1,      DPP_SEGMENT_FIN,            DPP_PFC_FCOE_ENCAP_ETH,     DPP_PLC_FCOE_VFT},
  {{{0, 64}},
          DPP_MACRO_FCoE1,      DPP_SEGMENT_END,            DPP_PFC_FCOE_ENCAP_ETH,     DPP_PLC_FCOE}
};

static dpp_leaf_t raw_mpls_leafs[] =
{
  {{{0, 16, 4}},
          DPP_MACRO_MPLS,       DPP_SEGMENT_MPLS,           DPP_PFC_MPLS1_ETH,          DPP_PLC_PP},
  {{{1, 16, 4}},
          DPP_MACRO_MPLS,       DPP_SEGMENT_MPLS,           DPP_PFC_MPLS2_ETH,          DPP_PLC_PP},
  {{{2, 16, 4},
    {3, 16, 4}},
          DPP_MACRO_MPLS,       DPP_SEGMENT_MPLS,           DPP_PFC_MPLS3_ETH,          DPP_PLC_PP}
};

static dpp_leaf_t MPLSoMPLS3_leafs[] =
{
  {{{3, 16, 4}},
          DPP_MACRO_MPLS,       DPP_SEGMENT_FIN,            DPP_PFC_MPLS4P_ETH,         DPP_PLC_PP},
  {{{(MPLS_SPECULATE_NEXT_PROTOCOL_NDX_ETH << 2), 3}},
          DPP_MACRO_ETH,        DPP_SEGMENT_ETHoMPLS4P,     DPP_PFC_MPLS4P_ETH,         DPP_PLC_ETHisH4},
  {{{(MPLS_SPECULATE_NEXT_PROTOCOL_NDX_IPV4 << 2), 3}},
          DPP_MACRO_IPv4,       DPP_SEGMENT_IPv4oMPLS4P,    DPP_PFC_MPLS4P_ETH,         DPP_PLC_IPv4isH4},
  {{{(MPLS_SPECULATE_NEXT_PROTOCOL_NDX_IPV6 << 2), 3}},
          DPP_MACRO_IPv6,       DPP_SEGMENT_IPv6oMPLS4P,    DPP_PFC_MPLS4P_ETH,         DPP_PLC_IPv6isH4},
  /* In case CW parsing is enabled */
  {{{(MPLS_SPECULATE_NEXT_PROTOCOL_NDX_CW << 2), 3}},
          DPP_MACRO_MPLS_CW,    DPP_SEGMENT_CWoMPLS4P,      DPP_PFC_MPLS4P_ETH,         DPP_PLC_ETHisH4, jer_parser_mpls_cw},
  /* In case CW parsing is disabled */
  {{{(MPLS_SPECULATE_NEXT_PROTOCOL_NDX_CW << 2), 3}},
          DPP_MACRO_MPLS_CW,    DPP_SEGMENT_END,            DPP_PFC_MPLS4P_ETH,         DPP_PLC_PP},
  {{{0, 64}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_MPLS4P_ETH,         DPP_PLC_ETHisH4},
};

static dpp_leaf_t IPv4oMPLS1_leafs[] =
{
  {{{(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1), 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1) + JER_IP_MC, 2}},
          DPP_MACRO_UDP,        DPP_SEGMENT_UDPoIPv4oMPLS1, DPP_PFC_MPLS1_ETH,          DPP_PLC_L4_IPv4isH3,      jer_parser_udp_mpls},
  /* Basic UDP Parsing */
  {{{(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1), 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1) + JER_IP_MC, 2}},
          DPP_MACRO_UDP,        DPP_SEGMENT_FIN,            DPP_PFC_MPLS1_ETH,          DPP_PLC_L4_IPv4isH3,      jer_parser_udp},
  {{{(DPP_PARSER_IP_NEXT_PROTOCOL_TCP << 1), 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_TCP << 1) + JER_IP_MC, 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1), 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1) + JER_IP_MC, 2}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_MPLS1_ETH,          DPP_PLC_L4_IPv4isH3},
  {{{0, 64}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_MPLS1_ETH,          DPP_PLC_IPv4isH3}
};

static dpp_leaf_t IPv4oMPLS2_leafs[] =
{
  {{{(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1), 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1) + JER_IP_MC, 2}},
          DPP_MACRO_UDP,        DPP_SEGMENT_UDPoIPv4oMPLS1, DPP_PFC_MPLS1_ETH,          DPP_PLC_L4_IPv4isH3,      jer_parser_udp_mpls},
  /* Basic UDP Parsing */
  {{{(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1), 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1) + JER_IP_MC, 2}},
          DPP_MACRO_UDP,        DPP_SEGMENT_FIN,            DPP_PFC_MPLS2_ETH,          DPP_PLC_L4_IPv4isH3,      jer_parser_udp},
  {{{(DPP_PARSER_IP_NEXT_PROTOCOL_TCP << 1), 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_TCP << 1) + JER_IP_MC, 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1), 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1) + JER_IP_MC, 2}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_MPLS2_ETH,          DPP_PLC_L4_IPv4isH3},
  {{{0, 64}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_MPLS2_ETH,          DPP_PLC_IPv4isH3}
};

static dpp_leaf_t IPv4oMPLS3_leafs[] =
{
  {{{(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1), 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1) + JER_IP_MC, 2}},
          DPP_MACRO_UDP,        DPP_SEGMENT_UDPoIPv4oMPLS1, DPP_PFC_MPLS1_ETH,          DPP_PLC_L4_IPv4isH3,      jer_parser_udp_mpls},
  /* Basic UDP Parsing */
  {{{(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1), 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1) + JER_IP_MC, 2}},
          DPP_MACRO_UDP,        DPP_SEGMENT_FIN,            DPP_PFC_MPLS3_ETH,          DPP_PLC_L4_IPv4isH3,      jer_parser_udp},
  {{{(DPP_PARSER_IP_NEXT_PROTOCOL_TCP << 1), 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_TCP << 1) + JER_IP_MC, 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1), 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1) + JER_IP_MC, 2}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_MPLS3_ETH,          DPP_PLC_L4_IPv4isH3},
  {{{0, 64}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_MPLS3_ETH,          DPP_PLC_IPv4isH3}
};

static dpp_leaf_t IPv4oMPLS4P_leafs[] =
{
  /* Basic UDP Parsing */
  {{{(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1), 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1) + JER_IP_MC, 2}},
          DPP_MACRO_UDP,        DPP_SEGMENT_FIN,            DPP_PFC_MPLS4P_ETH,         DPP_PLC_L4_IPv4isH4,      jer_parser_udp},
  {{{(DPP_PARSER_IP_NEXT_PROTOCOL_TCP << 1), 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_TCP << 1) + JER_IP_MC, 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1), 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1) + JER_IP_MC, 2}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_MPLS4P_ETH,         DPP_PLC_L4_IPv4isH4},
  {{{0, 64}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_MPLS4P_ETH,         DPP_PLC_IPv4isH4}
};

static dpp_leaf_t UDPoIPv4oMPLS1_leafs[] =
{
  {{{((ARAD_PARSER_CUSTOM_MACRO_PROTO_15 + 1) << 1),1}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_MPLS1_ETH,           DPP_PLC_BFD_SINGLE_HOP, jer_parser_bfd_ipv4},
  {{{0, 32}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_MPLS1_ETH,           DPP_PLC_IPv4isH3}
};

static dpp_leaf_t UDPoIPv4oMPLS2_leafs[] =
{
  {{{((ARAD_PARSER_CUSTOM_MACRO_PROTO_15 + 1) << 1),1}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_MPLS2_ETH,           DPP_PLC_BFD_SINGLE_HOP, jer_parser_bfd_ipv4},
  {{{0, 32}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_MPLS2_ETH,           DPP_PLC_IPv4isH3}
};

static dpp_leaf_t UDPoIPv4oMPLS3_leafs[] =
{
  {{{((ARAD_PARSER_CUSTOM_MACRO_PROTO_15 + 1) << 1),1}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_MPLS3_ETH,           DPP_PLC_BFD_SINGLE_HOP, jer_parser_bfd_ipv4},
  {{{0, 32}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_MPLS3_ETH,           DPP_PLC_IPv4isH3}
};

static dpp_leaf_t IPv6oMPLS1_leafs[] =
{
  /* Basic UDP Parsing */
  {{{(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1), 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1) + JER_IP_MC, 2}},
          DPP_MACRO_UDP,        DPP_SEGMENT_FIN,            DPP_PFC_MPLS1_ETH,          DPP_PLC_L4_IPv6isH3,      jer_parser_udp},
  {{{(DPP_PARSER_IP_NEXT_PROTOCOL_TCP << 1), 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_TCP << 1) + JER_IP_MC, 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1), 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1) + JER_IP_MC, 2}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_MPLS1_ETH,          DPP_PLC_L4_IPv6isH3},
  {{{0, 64}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_MPLS1_ETH,          DPP_PLC_IPv6isH3}
};

static dpp_leaf_t IPv6oMPLS2_leafs[] =
{
  /* Basic UDP Parsing */
  {{{(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1), 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1) + JER_IP_MC, 2}},
          DPP_MACRO_UDP,        DPP_SEGMENT_FIN,            DPP_PFC_MPLS2_ETH,          DPP_PLC_L4_IPv6isH3,      jer_parser_udp},
  {{{(DPP_PARSER_IP_NEXT_PROTOCOL_TCP << 1), 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_TCP << 1) + JER_IP_MC, 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1), 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1) + JER_IP_MC, 2}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_MPLS2_ETH,          DPP_PLC_L4_IPv6isH3},
  {{{0, 64}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_MPLS2_ETH,          DPP_PLC_IPv6isH3}
};

static dpp_leaf_t IPv6oMPLS3_leafs[] =
{
  /* Basic UDP Parsing */
  {{{(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1), 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1) + JER_IP_MC, 2}},
          DPP_MACRO_UDP,        DPP_SEGMENT_FIN,            DPP_PFC_MPLS3_ETH,          DPP_PLC_L4_IPv6isH3,      jer_parser_udp},
  {{{(DPP_PARSER_IP_NEXT_PROTOCOL_TCP << 1), 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_TCP << 1) + JER_IP_MC, 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1), 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1) + JER_IP_MC, 2}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_MPLS3_ETH,          DPP_PLC_L4_IPv6isH3},
  {{{0, 64}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_MPLS3_ETH,          DPP_PLC_IPv6isH3}
};

static dpp_leaf_t IPv6oMPLS4P_leafs[] =
{
  /* Basic UDP Parsing */
  {{{(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1), 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1) + JER_IP_MC, 2}},
          DPP_MACRO_UDP,        DPP_SEGMENT_FIN,            DPP_PFC_MPLS4P_ETH,         DPP_PLC_L4_IPv6isH4,      jer_parser_udp},
  {{{(DPP_PARSER_IP_NEXT_PROTOCOL_TCP << 1), 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_TCP << 1) + JER_IP_MC, 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1), 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1) + JER_IP_MC, 2}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_MPLS4P_ETH,         DPP_PLC_L4_IPv6isH4},
  {{{0, 64}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_MPLS4P_ETH,         DPP_PLC_IPv6isH4}
};

static dpp_leaf_t CWoMPLS1_leafs[] =
{
  /* Offsets
   *      Macro                 Next Segment                PFC                         PLC            */
  {{{0, 32}},
          DPP_MACRO_ETH,        DPP_SEGMENT_ETHoMPLS1,      DPP_PFC_MPLS1_ETH,         DPP_PLC_ETHisH4}
};

static dpp_leaf_t CWoMPLS2_leafs[] =
{
  /* Offsets
   *      Macro                 Next Segment                PFC                         PLC            */
  {{{0, 32}},
          DPP_MACRO_ETH,        DPP_SEGMENT_ETHoMPLS2,      DPP_PFC_MPLS2_ETH,         DPP_PLC_ETHisH4}
};

static dpp_leaf_t CWoMPLS3_leafs[] =
{
  /* Offsets
   *      Macro                 Next Segment                PFC                         PLC            */
  {{{0, 32}},
          DPP_MACRO_ETH,        DPP_SEGMENT_ETHoMPLS3,      DPP_PFC_MPLS3_ETH,         DPP_PLC_ETHisH4}
};

static dpp_leaf_t CWoMPLS4P_leafs[] =
{
  /* Offsets
   *      Macro                 Next Segment                PFC                         PLC            */
  {{{0, 32}},
          DPP_MACRO_ETH,        DPP_SEGMENT_ETHoMPLS4P,      DPP_PFC_MPLS4P_ETH,         DPP_PLC_ETHisH4}
};

static dpp_leaf_t ETHoMPLS1_leafs[] =
{
  /* Offsets
   *      Macro                 Next Segment                PFC                         PLC            */
  {{{(MPLS_SPECULATE_NEXT_PROTOCOL_NDX_IPV4 << 2), 4}},
          DPP_MACRO_IPv4,       DPP_SEGMENT_IPv4oETHoMPLS1, DPP_PFC_MPLS1_ETH,          DPP_PLC_ETHisH3},
  {{{(MPLS_SPECULATE_NEXT_PROTOCOL_NDX_IPV6 << 2), 4}},
          DPP_MACRO_IPv6,       DPP_SEGMENT_IPv6oETHoMPLS1, DPP_PFC_MPLS1_ETH,          DPP_PLC_ETHisH3},
  {{{0, 64}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_MPLS1_ETH,          DPP_PLC_ETHisH3}
};

static dpp_leaf_t ETHoMPLS2_leafs[] =
{
  /* Offsets
   *      Macro                 Next Segment                PFC                         PLC            */
  {{{(MPLS_SPECULATE_NEXT_PROTOCOL_NDX_IPV4 << 2), 4}},
          DPP_MACRO_IPv4,       DPP_SEGMENT_IPv4oETHoMPLS2, DPP_PFC_MPLS2_ETH,          DPP_PLC_ETHisH3},
  {{{(MPLS_SPECULATE_NEXT_PROTOCOL_NDX_IPV6 << 2), 4}},
          DPP_MACRO_IPv6,       DPP_SEGMENT_IPv6oETHoMPLS2, DPP_PFC_MPLS2_ETH,          DPP_PLC_ETHisH3},
  {{{0, 64}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_MPLS2_ETH,          DPP_PLC_ETHisH3}
};

static dpp_leaf_t ETHoMPLS3_leafs[] =
{
  /* Offsets
   *      Macro                 Next Segment                PFC                         PLC            */
  {{{(MPLS_SPECULATE_NEXT_PROTOCOL_NDX_IPV4 << 2), 4}},
          DPP_MACRO_IPv4,       DPP_SEGMENT_IPv4oETHoMPLS3, DPP_PFC_MPLS3_ETH,          DPP_PLC_ETHisH3},
  {{{(MPLS_SPECULATE_NEXT_PROTOCOL_NDX_IPV6 << 2), 4}},
          DPP_MACRO_IPv6,       DPP_SEGMENT_IPv6oETHoMPLS3, DPP_PFC_MPLS3_ETH,          DPP_PLC_ETHisH3},
  {{{0, 64}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_MPLS3_ETH,          DPP_PLC_ETHisH3}
};

static dpp_leaf_t ETHoMPLS4P_leafs[] =
{
  /* Offsets
   *      Macro                 Next Segment                PFC                         PLC            */
  {{{(MPLS_SPECULATE_NEXT_PROTOCOL_NDX_IPV4 << 2), 4}},
          DPP_MACRO_IPv4,       DPP_SEGMENT_IPv4oETHoMPLS4P,DPP_PFC_MPLS4P_ETH,         DPP_PLC_ETHisH4},
  {{{(MPLS_SPECULATE_NEXT_PROTOCOL_NDX_IPV6 << 2), 4}},
          DPP_MACRO_IPv6,       DPP_SEGMENT_IPv6oETHoMPLS4P,DPP_PFC_MPLS4P_ETH,         DPP_PLC_ETHisH4},
  {{{0, 64}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_MPLS4P_ETH,         DPP_PLC_ETHisH4}
};

static dpp_leaf_t IPv4oETHoMPLS1_leafs[] =
{
  /* Basic UDP Parsing */
  {{{(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1), 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1) + JER_IP_MC, 2}},
          DPP_MACRO_UDP,        DPP_SEGMENT_FIN,            DPP_PFC_MPLS1_ETH,          DPP_PLC_L4_IPv4isH4,      jer_parser_udp},
  {{{(DPP_PARSER_IP_NEXT_PROTOCOL_TCP << 1), 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_TCP << 1) + JER_IP_MC, 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1), 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1) + JER_IP_MC, 2}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_MPLS1_ETH,          DPP_PLC_L4_IPv4isH4},
  {{{0, 64}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_MPLS1_ETH,          DPP_PLC_IPv4isH4}
};

static dpp_leaf_t IPv4oETHoMPLS2_leafs[] =
{
  /* Basic UDP Parsing */
  {{{(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1), 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1) + JER_IP_MC, 2}},
          DPP_MACRO_UDP,        DPP_SEGMENT_FIN,            DPP_PFC_MPLS2_ETH,          DPP_PLC_L4_IPv4isH4,      jer_parser_udp},
  {{{(DPP_PARSER_IP_NEXT_PROTOCOL_TCP << 1), 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_TCP << 1) + JER_IP_MC, 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1), 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1) + JER_IP_MC, 2}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_MPLS2_ETH,          DPP_PLC_L4_IPv4isH4},
  {{{0, 64}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_MPLS2_ETH,          DPP_PLC_IPv4isH4}
};

static dpp_leaf_t IPv4oETHoMPLS3_leafs[] =
{
  /* Basic UDP Parsing */
  {{{(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1), 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1) + JER_IP_MC, 2}},
          DPP_MACRO_UDP,        DPP_SEGMENT_FIN,            DPP_PFC_MPLS3_ETH,          DPP_PLC_L4_IPv4isH4,      jer_parser_udp},
  {{{(DPP_PARSER_IP_NEXT_PROTOCOL_TCP << 1), 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_TCP << 1) + JER_IP_MC, 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1), 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1) + JER_IP_MC, 2}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_MPLS3_ETH,          DPP_PLC_L4_IPv4isH4},
  {{{0, 64}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_MPLS3_ETH,          DPP_PLC_IPv4isH4}
};

static dpp_leaf_t IPv4oETHoMPLS4P_leafs[] =
{
  /* Basic UDP Parsing */
  {{{(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1), 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1) + JER_IP_MC, 2}},
          DPP_MACRO_UDP,        DPP_SEGMENT_FIN,            DPP_PFC_MPLS4P_ETH,         DPP_PLC_L4_IPv4isH5,      jer_parser_udp},
  {{{(DPP_PARSER_IP_NEXT_PROTOCOL_TCP << 1), 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_TCP << 1) + JER_IP_MC, 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1), 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1) + JER_IP_MC, 2}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_MPLS4P_ETH,         DPP_PLC_L4_IPv4isH5},
  {{{0, 64}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_MPLS4P_ETH,         DPP_PLC_IPv4isH5}
};

static dpp_leaf_t IPv6oETHoMPLS1_leafs[] =
{
  /* Basic UDP Parsing */
  {{{(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1), 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1) + JER_IP_MC, 2}},
          DPP_MACRO_UDP,        DPP_SEGMENT_FIN,            DPP_PFC_MPLS1_ETH,          DPP_PLC_L4_IPv6isH4,      jer_parser_udp},
  {{{(DPP_PARSER_IP_NEXT_PROTOCOL_TCP << 1), 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_TCP << 1) + JER_IP_MC, 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1), 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1) + JER_IP_MC, 2}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_MPLS1_ETH,          DPP_PLC_L4_IPv6isH4},
  {{{0, 64}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_MPLS1_ETH,          DPP_PLC_IPv6isH4}
};

static dpp_leaf_t IPv6oETHoMPLS2_leafs[] =
{
  /* Basic UDP Parsing */
  {{{(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1), 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1) + JER_IP_MC, 2}},
          DPP_MACRO_UDP,        DPP_SEGMENT_FIN,            DPP_PFC_MPLS2_ETH,          DPP_PLC_L4_IPv6isH4,      jer_parser_udp},
  {{{(DPP_PARSER_IP_NEXT_PROTOCOL_TCP << 1), 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_TCP << 1) + JER_IP_MC, 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1), 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1) + JER_IP_MC, 2}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_MPLS2_ETH,          DPP_PLC_L4_IPv6isH4},
  {{{0, 64}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_MPLS2_ETH,          DPP_PLC_IPv6isH4}
};

static dpp_leaf_t IPv6oETHoMPLS3_leafs[] =
{
  /* Basic UDP Parsing */
  {{{(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1), 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1) + JER_IP_MC, 2}},
          DPP_MACRO_UDP,        DPP_SEGMENT_FIN,            DPP_PFC_MPLS3_ETH,          DPP_PLC_L4_IPv6isH4,      jer_parser_udp},
  {{{(DPP_PARSER_IP_NEXT_PROTOCOL_TCP << 1), 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_TCP << 1) + JER_IP_MC, 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1), 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1) + JER_IP_MC, 2}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_MPLS3_ETH,          DPP_PLC_L4_IPv6isH4},
  {{{0, 64}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_MPLS3_ETH,          DPP_PLC_IPv6isH4}
};

static dpp_leaf_t IPv6oETHoMPLS4P_leafs[] =
{
  /* Basic UDP Parsing */
  {{{(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1), 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1) + JER_IP_MC, 2}},
          DPP_MACRO_UDP,        DPP_SEGMENT_FIN,            DPP_PFC_MPLS4P_ETH,         DPP_PLC_L4_IPv6isH5,      jer_parser_udp},
  {{{(DPP_PARSER_IP_NEXT_PROTOCOL_TCP << 1), 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_TCP << 1) + JER_IP_MC, 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1), 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1) + JER_IP_MC, 2}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_MPLS4P_ETH,         DPP_PLC_L4_IPv6isH5},
  {{{0, 64}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_MPLS4P_ETH,         DPP_PLC_IPv6isH5}
};

static dpp_leaf_t add_tpid_leafs[] =
{
  {{{(DPP_PARSER_ETH_NEXT_PROTOCOL_MinM << 1), 1}},
          DPP_MACRO_ETH,        DPP_SEGMENT_FIN,            DPP_PFC_ETH_ETH,            DPP_PLC_PP},
  {{{(DPP_PARSER_ETH_NEXT_PROTOCOL_IPv4 << 1), 1}},
          DPP_MACRO_IPv4,       DPP_SEGMENT_IPv4,           DPP_PFC_ETH,                DPP_PLC_PP},
  {{{(DPP_PARSER_ETH_NEXT_PROTOCOL_IPv6 << 1), 1}},
          DPP_MACRO_IPv6,       DPP_SEGMENT_IPv6,           DPP_PFC_ETH,                DPP_PLC_PP},
  {{{(DPP_PARSER_ETH_NEXT_PROTOCOL_MPLS << 1), 1},
    {((ARAD_PARSER_ETHER_PROTO_7_MPLS_MC + 1) << 1), 1}},
          DPP_MACRO_MPLS,       DPP_SEGMENT_MPLS,           DPP_PFC_ETH,                DPP_PLC_PP},
  {{{(DPP_PARSER_ETH_NEXT_PROTOCOL_TRILL << 1), 1}},
          DPP_MACRO_TRILL,      DPP_SEGMENT_TRILL,          DPP_PFC_ETH,                DPP_PLC_PP,         jer_parser_trill},
  {{{(DPP_PARSER_ETH_NEXT_PROTOCOL_FCoE << 1), 1}},
          DPP_MACRO_FCoE2,      DPP_SEGMENT_FCoE1,          DPP_PFC_ETH,                DPP_PLC_PP,         jer_parser_fcoe},
  {{{0, 64}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_ETH,                DPP_PLC_PP}
};

static dpp_leaf_t IPv4oIPv4_leafs[] =
{
  /* Basic UDP Parsing */
  {{{(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1), 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1) + JER_IP_MC, 2}},
          DPP_MACRO_UDP,        DPP_SEGMENT_FIN,            DPP_PFC_IPV4_IPV4_ETH,       DPP_PLC_PP_L4,      jer_parser_udp},
  {{{(DPP_PARSER_IP_NEXT_PROTOCOL_TCP << 1), 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_TCP << 1) + JER_IP_MC, 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1), 2},
    {(DPP_PARSER_IP_NEXT_PROTOCOL_UDP << 1) + JER_IP_MC, 2}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_IPV4_IPV4_ETH,       DPP_PLC_PP_L4},
  {{{0, 64}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_IPV4_IPV4_ETH,       DPP_PLC_PP}
};

static dpp_leaf_t PPPoE_session_leafs[] =
{
  {{{(ARAD_PARSER_ETHER_PROTO_5_PPPoES_CONTROL<< 1), 1}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_ETH,                DPP_PLC_PP},
  {{{(DPP_PARSER_ETH_NEXT_PROTOCOL_IPv4 << 1), 1}},
          DPP_MACRO_IPv4,       DPP_SEGMENT_IPv4,           DPP_PFC_ETH,                DPP_PLC_PP},
  {{{(DPP_PARSER_ETH_NEXT_PROTOCOL_IPv6 << 1), 1}},
          DPP_MACRO_IPv6,       DPP_SEGMENT_IPv6,           DPP_PFC_ETH,                DPP_PLC_PP},
  {{{0, 64}},
          DPP_MACRO_NA,         DPP_SEGMENT_END,            DPP_PFC_ETH,                DPP_PLC_PP}
};

/* Macro allowing to obtain number of Leafs in array */
#define NOF_LEAFS(array) sizeof(array) / sizeof(dpp_leaf_t)

static dpp_segment_t jer_segments[] =
{
  {DPP_SEGMENT_FIN,            "FIN"},
  {DPP_SEGMENT_BASE,           "Base",           Base_leafs,            NOF_LEAFS(Base_leafs)           , JER_SEGMENT_SMALL_SIZE},
  {DPP_SEGMENT_TM,             "TM",             TM_leafs,              NOF_LEAFS(TM_leafs)             , JER_SEGMENT_SMALL_SIZE},
  {DPP_SEGMENT_RAW_MPLS,       "RawMPLS",        raw_mpls_leafs,        NOF_LEAFS(raw_mpls_leafs)       , JER_SEGMENT_FULL_SIZE},
  {DPP_SEGMENT_ETH,            "ETH",            ETH_leafs,             NOF_LEAFS(ETH_leafs)            , JER_SEGMENT_FULL_SIZE},
  {DPP_SEGMENT_IPv4,           "IPv4",           IPv4_leafs,            NOF_LEAFS(IPv4_leafs)           , JER_SEGMENT_FULL_SIZE},
  {DPP_SEGMENT_IPv6,           "IPv6",           IPv6_leafs,            NOF_LEAFS(IPv6_leafs)           , JER_SEGMENT_FULL_SIZE},
  {DPP_SEGMENT_MPLS,           "MPLS",           MPLS_leafs,            NOF_LEAFS(MPLS_leafs)           , JER_SEGMENT_FULL_SIZE},
  {DPP_SEGMENT_TRILL,          "TRILL",          TRILL_leafs,           NOF_LEAFS(TRILL_leafs)          , JER_SEGMENT_FULL_SIZE},
  {DPP_SEGMENT_EoTRILL,        "EoTRILL",        EoTRILL_leafs,         NOF_LEAFS(EoTRILL_leafs)        , JER_SEGMENT_FULL_SIZE},
  {DPP_SEGMENT_FCoE1,          "FCoE1",          FCoE1_leafs,           NOF_LEAFS(FCoE1_leafs)          , JER_SEGMENT_FULL_SIZE},
  {DPP_SEGMENT_FCoE2,          "FCoE2",          FCoE2_leafs,           NOF_LEAFS(FCoE2_leafs)          , JER_SEGMENT_FULL_SIZE},
  {DPP_SEGMENT_FCoE3,          "FCoE2",          FCoE3_leafs,           NOF_LEAFS(FCoE3_leafs)          , JER_SEGMENT_FULL_SIZE},
  {DPP_SEGMENT_MinM,           "MinM",           MinM_leafs,            NOF_LEAFS(MinM_leafs)           , JER_SEGMENT_FULL_SIZE},
  {DPP_SEGMENT_MPLSoMPLS3,     "MPLSoMPLS3",     MPLSoMPLS3_leafs,      NOF_LEAFS(MPLSoMPLS3_leafs)     , JER_SEGMENT_FULL_SIZE},
  {DPP_SEGMENT_IPv4oMPLS1,     "IPv4oMPLS1",     IPv4oMPLS1_leafs,      NOF_LEAFS(IPv4oMPLS1_leafs)     , JER_SEGMENT_FULL_SIZE},
  {DPP_SEGMENT_IPv4oMPLS2,     "IPv4oMPLS2",     IPv4oMPLS2_leafs,      NOF_LEAFS(IPv4oMPLS2_leafs)     , JER_SEGMENT_FULL_SIZE},
  {DPP_SEGMENT_IPv4oMPLS3,     "IPv4oMPLS3",     IPv4oMPLS3_leafs,      NOF_LEAFS(IPv4oMPLS3_leafs)     , JER_SEGMENT_FULL_SIZE},
  {DPP_SEGMENT_IPv4oMPLS4P,    "IPv4oMPLS4P",    IPv4oMPLS4P_leafs,     NOF_LEAFS(IPv4oMPLS4P_leafs)    , JER_SEGMENT_FULL_SIZE},
  {DPP_SEGMENT_UDPoIPv4oMPLS1, "UDPoIPv4oMPLS1", UDPoIPv4oMPLS1_leafs,  NOF_LEAFS(UDPoIPv4oMPLS1_leafs) , JER_SEGMENT_HALF_SIZE},
  {DPP_SEGMENT_UDPoIPv4oMPLS2, "UDPoIPv4oMPLS2", UDPoIPv4oMPLS2_leafs,  NOF_LEAFS(UDPoIPv4oMPLS2_leafs) , JER_SEGMENT_HALF_SIZE},
  {DPP_SEGMENT_UDPoIPv4oMPLS3, "UDPoIPv4oMPLS3", UDPoIPv4oMPLS3_leafs,  NOF_LEAFS(UDPoIPv4oMPLS3_leafs) , JER_SEGMENT_HALF_SIZE},
  {DPP_SEGMENT_IPv6oMPLS1,     "IPv6oMPLS1",     IPv6oMPLS1_leafs,      NOF_LEAFS(IPv6oMPLS1_leafs)     , JER_SEGMENT_FULL_SIZE},
  {DPP_SEGMENT_IPv6oMPLS2,     "IPv6oMPLS2",     IPv6oMPLS2_leafs,      NOF_LEAFS(IPv6oMPLS2_leafs)     , JER_SEGMENT_FULL_SIZE},
  {DPP_SEGMENT_IPv6oMPLS3,     "IPv6oMPLS3",     IPv6oMPLS3_leafs,      NOF_LEAFS(IPv6oMPLS3_leafs)     , JER_SEGMENT_FULL_SIZE},
  {DPP_SEGMENT_IPv6oMPLS4P,    "IPv6oMPLS4P",    IPv6oMPLS4P_leafs,     NOF_LEAFS(IPv6oMPLS4P_leafs)    , JER_SEGMENT_FULL_SIZE},
  {DPP_SEGMENT_CWoMPLS1,       "CWoMPLS1",       CWoMPLS1_leafs,        NOF_LEAFS(CWoMPLS1_leafs)       , JER_SEGMENT_HALF_SIZE},
  {DPP_SEGMENT_CWoMPLS2,       "CWoMPLS2",       CWoMPLS2_leafs,        NOF_LEAFS(CWoMPLS2_leafs)       , JER_SEGMENT_HALF_SIZE},
  {DPP_SEGMENT_CWoMPLS3,       "CWoMPLS3",       CWoMPLS3_leafs,        NOF_LEAFS(CWoMPLS3_leafs)       , JER_SEGMENT_HALF_SIZE},
  {DPP_SEGMENT_CWoMPLS4P,      "CWoMPLS4P",      CWoMPLS4P_leafs,       NOF_LEAFS(CWoMPLS4P_leafs)      , JER_SEGMENT_HALF_SIZE},
  {DPP_SEGMENT_ETHoMPLS1,      "ETHoMPLS1",      ETHoMPLS1_leafs,       NOF_LEAFS(ETHoMPLS1_leafs)      , JER_SEGMENT_FULL_SIZE},
  {DPP_SEGMENT_ETHoMPLS2,      "ETHoMPLS2",      ETHoMPLS2_leafs,       NOF_LEAFS(ETHoMPLS2_leafs)      , JER_SEGMENT_FULL_SIZE},
  {DPP_SEGMENT_ETHoMPLS3,      "ETHoMPLS3",      ETHoMPLS3_leafs,       NOF_LEAFS(ETHoMPLS3_leafs)      , JER_SEGMENT_FULL_SIZE},
  {DPP_SEGMENT_ETHoMPLS4P,     "ETHoMPLS4P",     ETHoMPLS4P_leafs,      NOF_LEAFS(ETHoMPLS4P_leafs)     , JER_SEGMENT_FULL_SIZE},
  {DPP_SEGMENT_IPv4oETHoMPLS1, "IPv4oETHoMPLS1", IPv4oETHoMPLS1_leafs,  NOF_LEAFS(IPv4oETHoMPLS1_leafs) , JER_SEGMENT_FULL_SIZE},
  {DPP_SEGMENT_IPv4oETHoMPLS2, "IPv4oETHoMPLS2", IPv4oETHoMPLS2_leafs,  NOF_LEAFS(IPv4oETHoMPLS2_leafs) , JER_SEGMENT_FULL_SIZE},
  {DPP_SEGMENT_IPv4oETHoMPLS3, "IPv4oETHoMPLS3", IPv4oETHoMPLS3_leafs,  NOF_LEAFS(IPv4oETHoMPLS3_leafs) , JER_SEGMENT_FULL_SIZE},
  {DPP_SEGMENT_IPv4oETHoMPLS4P,"IPv4oETHoMPLS4P",IPv4oETHoMPLS4P_leafs, NOF_LEAFS(IPv4oETHoMPLS4P_leafs), JER_SEGMENT_FULL_SIZE},
  {DPP_SEGMENT_IPv6oETHoMPLS1, "IPv6oETHoMPLS1", IPv6oETHoMPLS1_leafs,  NOF_LEAFS(IPv6oETHoMPLS1_leafs) , JER_SEGMENT_FULL_SIZE},
  {DPP_SEGMENT_IPv6oETHoMPLS2, "IPv6oETHoMPLS2", IPv6oETHoMPLS2_leafs,  NOF_LEAFS(IPv6oETHoMPLS2_leafs) , JER_SEGMENT_FULL_SIZE},
  {DPP_SEGMENT_IPv6oETHoMPLS3, "IPv6oETHoMPLS3", IPv6oETHoMPLS3_leafs,  NOF_LEAFS(IPv6oETHoMPLS3_leafs) , JER_SEGMENT_FULL_SIZE},
  {DPP_SEGMENT_IPv6oETHoMPLS4P,"IPv6oETHoMPLS4P",IPv6oETHoMPLS4P_leafs, NOF_LEAFS(IPv6oETHoMPLS4P_leafs), JER_SEGMENT_FULL_SIZE},
  {DPP_SEGMENT_IPv4oIPv4,      "IPv4oIPv4",      IPv4oIPv4_leafs,       NOF_LEAFS(IPv4oIPv4_leafs)      , JER_SEGMENT_FULL_SIZE},
  {DPP_SEGMENT_UDPoIPv4,       "UDPoIPv4",       UDPoIPv4_leafs,        NOF_LEAFS(UDPoIPv4_leafs)       , JER_SEGMENT_HALF_SIZE},
  {DPP_SEGMENT_UDPoIPv4_gtp,   "UDPoIPv4_gtp",   UDPoIPv4_gtp_leafs,    NOF_LEAFS(UDPoIPv4_gtp_leafs)   , JER_SEGMENT_HALF_SIZE},
  {DPP_SEGMENT_GTPoUDPoIPv4,   "GTPoUDPoIPv4",   GTPoUDPoIPv4_leafs,    NOF_LEAFS(GTPoUDPoIPv4_leafs)   , JER_SEGMENT_HALF_SIZE},
  {DPP_SEGMENT_UDPoIPv4_2,     "UDPoIPv4_2",     UDPoIPv4_2_leafs,      NOF_LEAFS(UDPoIPv4_2_leafs)     , JER_SEGMENT_HALF_SIZE},
  {DPP_SEGMENT_VxLAN,          "VxLAN",          VxLAN_leafs,           NOF_LEAFS(VxLAN_leafs)          , JER_SEGMENT_FULL_SIZE},
  {DPP_SEGMENT_UDPoIPv6,       "UDPoIPv6",       UDPoIPv6_leafs,        NOF_LEAFS(UDPoIPv6_leafs)       , JER_SEGMENT_HALF_SIZE},
  {DPP_SEGMENT_GTPoUDPoIPv6,   "GTPoUDPoIPv6",   GTPoUDPoIPv6_leafs,    NOF_LEAFS(GTPoUDPoIPv6_leafs)   , JER_SEGMENT_HALF_SIZE},
  {DPP_SEGMENT_ADD_TPID,       "AddTPID",        add_tpid_leafs,        NOF_LEAFS(add_tpid_leafs)       , JER_SEGMENT_FULL_SIZE},
  {DPP_SEGMENT_IPv6Ext1,       "IPv6Ext1",       IPv6Ext1_leafs,        NOF_LEAFS(IPv6Ext1_leafs)       , JER_SEGMENT_FULL_SIZE},
  {DPP_SEGMENT_IPv6Ext2_UDP,   "IPv6Ext2_UDP",   IPv6Ext2_UDP_leafs,    NOF_LEAFS(IPv6Ext2_UDP_leafs)   , JER_SEGMENT_SMALL_SIZE},
  {DPP_SEGMENT_IPv6Ext2_IPv4,  "IPv6Ext2_IPv4",  IPv6Ext2_IPv4_leafs,   NOF_LEAFS(IPv6Ext2_IPv4_leafs)  , JER_SEGMENT_SMALL_SIZE},
  {DPP_SEGMENT_IPv6Ext2_IPv6,  "IPv6Ext2_IPv6",  IPv6Ext2_IPv6_leafs,   NOF_LEAFS(IPv6Ext2_IPv6_leafs)  , JER_SEGMENT_SMALL_SIZE},
  {DPP_SEGMENT_PPPoE_SESSION,  "PPPoES",         PPPoE_session_leafs,   NOF_LEAFS(PPPoE_session_leafs)  , JER_SEGMENT_HALF_SIZE},
  {DPP_SEGMENT_L2TP,           "L2TP",		     L2TP_leafs,	        NOF_LEAFS(L2TP_leafs)	        , JER_SEGMENT_HALF_SIZE},
  {DPP_SEGMENT_VxLAN6,         "VxLAN6",         VxLAN6_leafs,          NOF_LEAFS(VxLAN6_leafs)         , JER_SEGMENT_FULL_SIZE},
  {DPP_SEGMENT_COMPLEX,        "COMPLEX"},
  {DPP_SEGMENT_END,            "END"}
};

static dpp_segment_t *jer_parser_segment_get(DPP_SEGMENT_E segment_id)
{
    int i_seg;
    for(i_seg = 0; i_seg < (sizeof(jer_segments) / sizeof(dpp_segment_t)); i_seg++)
    {
        if(jer_segments[i_seg].id == segment_id)
            return &jer_segments[i_seg];
    }
    return NULL;
}

char *jer_parser_segment_string_by_sw(DPP_SEGMENT_E segment_id)
{
    dpp_segment_t *segment;
    if((segment = jer_parser_segment_get(segment_id)) == NULL)
    {
        LOG_ERROR(BSL_LS_SOC_PKT,(BSL_META_U(0, "Invalid Segment ID:%d\n"), segment_id));
        return NULL;
    }
    return segment->name;
}

int jer_parser_segment_size_by_sw(DPP_SEGMENT_E segment_id)
{
    dpp_segment_t *segment;
    if((segment = jer_parser_segment_get(segment_id)) == NULL)
    {
        LOG_ERROR(BSL_LS_SOC_PKT,(BSL_META_U(0, "Invalid Segment ID:%d\n"), segment_id));
        return 0;
    }
    return segment->size;
}

char *jer_parser_segment_string_by_base(int unit, uint32 base)
{
    DPP_SEGMENT_E segment_id;

    if((segment_id = jer_parser_segment_id_by_base(unit, base)) == DPP_SEGMENT_NA)
    {
        return "Not Allocated";
    }

    return jer_parser_segment_string_by_sw(segment_id);
}

DPP_SEGMENT_E jer_parser_segment_id_by_base(int unit, uint32 base)
{
    DPP_SEGMENT_E segment_id;

    if(base > JER_SEGMENT_END)
        segment_id = DPP_SEGMENT_NA;
    else if(base == JER_SEGMENT_FIN)
        segment_id = DPP_SEGMENT_FIN;
    else if(base == JER_SEGMENT_END)
        segment_id = DPP_SEGMENT_END;
    else
    {
        segment_id = segment_map[unit][base >> 2];
        if(segment_id == DPP_SEGMENT_COMPLEX)
        {
            segment_id = segment_complex_map[unit][base >> 2][base & JER_SEGMENT_COMPLEX_MASK];
        }
    }

    return segment_id;
}

soc_error_t
jer_parser_segment_alloc(int unit, dpp_segment_t *segment)
{
    soc_error_t ret = SOC_E_RESOURCE;
    int i_ind;

    for(i_ind = 0; i_ind < JER_SEGMENT_NOF; i_ind++)
    {
        if(segment->size == JER_SEGMENT_FULL_SIZE)
        {
            if(segment_map[unit][i_ind] == DPP_SEGMENT_NA)
            {
                segment_map[unit][i_ind] = segment->id;
                segment->address = i_ind << 2;
                ret = SOC_E_NONE;
                goto exit;
            }
        }
        else /* Small or Half Segment */
        {
            if((segment_map[unit][i_ind] == DPP_SEGMENT_NA) || (segment_map[unit][i_ind] == DPP_SEGMENT_COMPLEX))
            {
                int k_ind;
                for(k_ind = 0; k_ind < JER_SEGMENT_COMPLEX_NOF; k_ind++)
                {
                    /* HALF size will not match last fragment of the segment */
                    if((segment->size == JER_SEGMENT_HALF_SIZE) && (k_ind == JER_SEGMENT_COMPLEX_NOF - 1))
                        break;
                    if(segment_complex_map[unit][i_ind][k_ind] == DPP_SEGMENT_NA)
                    {
                        segment_complex_map[unit][i_ind][k_ind] = segment->id;
                        if(segment->size == JER_SEGMENT_HALF_SIZE)
                        {   /* Half segment catches 2 fragments and mark in the map second fragment as busy by NOF */
                            segment_complex_map[unit][i_ind][k_ind + 1] = DPP_SEGMENT_NOF;
                        }
                        segment_map[unit][i_ind] = DPP_SEGMENT_COMPLEX;
                        segment->address = (i_ind << 2) | k_ind;
                        ret = SOC_E_NONE;
                        goto exit;
                    }
                }
            }
        }
    }

exit:
    if(segment->address == -1)
    {
        LOG_ERROR(BSL_LS_SOC_PKT,(BSL_META_U(0, "Failed to allocate Segment:%s\n"), segment->name));
    }
    return ret;
}

static int jer_parser_segment_add(int unit, DPP_SEGMENT_E segment_id)
{
    uint32 res = SOC_SAND_OK;
    dpp_segment_t *segment;
    int i_ind;
    dpp_leaf_t *leaf;
    uint32 instr_addr;

    ARAD_PP_IHP_PARSER_PROGRAM_TBL_DATA program_data[JER_SEGMENT_FULL_SIZE];
    ARAD_PP_IHP_PACKET_FORMAT_TABLE_TBL_DATA format_data[JER_SEGMENT_FULL_SIZE];

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    /* Step 1 - Obtain pointer to segment info */
    if((segment = jer_parser_segment_get(segment_id)) == NULL)
        return -1;

    /* Step 2 - Leave with success is Segment was already initialized */
    if(segment->address != -1)
        goto exit;

    /* Step 3 - Allocate segment in program space. Address will be reported to caller */
    if(jer_parser_segment_alloc(unit, segment) != SOC_E_NONE)
    {
        LOG_ERROR(BSL_LS_SOC_PKT,(BSL_META_U(0, "Failed to allocate Segment:%s\n"), segment->name));
        goto exit;
    }

    /* Step 4 - Sanity, verify that everything is Ok with this segment */
    if((segment->leafs == NULL) || (segment->nof_leafs == 0))
    {
        LOG_ERROR(BSL_LS_SOC_PKT,(BSL_META_U(0, "No leafs on Segment:%s\n"), segment->name));
        goto exit;
    }

    /* Step 5 - Mark all leafs as not initialized, use macro as such mark - nothing to do with real macro */
    for(i_ind = 0; i_ind < segment->size; i_ind++)
    {
        program_data[i_ind].macro_sel       = JER_MACRO_NA;
        program_data[i_ind].next_addr_base  = 0;
    }

    /* Step 6 - Initialize All leaf of the segment */
    for(i_ind = 0; i_ind < segment->nof_leafs; i_ind++)
    {
        soc_error_t ret = SOC_E_NONE;
        int i_range;
        leaf = &segment->leafs[i_ind];
        /* Step 6.1 - Check if we have initialization callback and call it */
        if(leaf->parser_cb)
            ret = leaf->parser_cb(unit);

        if(ret == SOC_E_NOT_FOUND)
        { /* It means that this entry will not be initialized and the leaf in segment will be initialized by later ones(default) */
            continue;
        }
        else if(ret == SOC_E_EXISTS)
        {
            LOG_VERBOSE(BSL_LS_SOC_PKT,(BSL_META_U(0, "Init already invoked on Leaf:%d Segment:%s - we may continue\n"), i_ind, segment->name));
        }
        else if(ret != SOC_E_NONE)
        { /* Any other error means real error, leave */
            LOG_ERROR(BSL_LS_SOC_PKT,(BSL_META_U(0, "Failed to initialize Leaf:%d Segment:%s\n"), i_ind, segment->name));
            goto exit;
        }

        /* Step 6.2 - Obtain values for HW parameters */
        for(i_range = 0; (i_range < JER_SEGMENT_RANGE_MAX) && (leaf->offset_range[i_range].num != 0); i_range++)
        {
            int i_leaf;
            int offset, num, step;
            uint32 pfc_hw = 0, plc_hw = 0, macro_sel = 0, base = 0;

            /* For FIN or END it will just return value to put in next_addr_base,
             * For already allocated jer_segments return its address and try to allocate for non-allocated ones  */
            if((base = jer_parser_segment_add(unit, leaf->segment_id)) == -1)
            {
                LOG_ERROR(BSL_LS_SOC_PKT,(BSL_META_U(0, "Failed to add segment:%s\n"), jer_parser_segment_string_by_sw(leaf->segment_id)));
                goto exit;
            }

/*            if((leaf->segment_id == DPP_SEGMENT_END) || (leaf->segment_id == DPP_SEGMENT_FIN)) */
            {   /* We need to assign PFC/PLC anyway for the case of macro error, even if next segment defined */
                SOC_SAND_SOC_IF_ERROR_RETURN(res, 200, exit, dpp_parser_pfc_activate_hw_by_sw(unit, leaf->pfc_sw, &pfc_hw));
                SOC_SAND_SOC_IF_ERROR_RETURN(res, 200, exit, dpp_parser_plc_hw_by_sw(unit, leaf->pfc_sw, leaf->plc_sw, &plc_hw, NULL));
            }

            if(leaf->segment_id != DPP_SEGMENT_END)
            {   /* We need to obtain MACRO, no allocation here, allocation should have been done in init callback */
                if((macro_sel = jer_parser_macro_sel_get(unit, leaf->macro_id)) == JER_MACRO_NA)
                {
                    LOG_ERROR(BSL_LS_SOC_PKT,(BSL_META_U(0, "Custom Macro:%d was not allocated\n"), leaf->macro_id));
                    goto exit;
                }
            }

            offset = leaf->offset_range[i_range].offset;
            num = leaf->offset_range[i_range].num;
            step = leaf->offset_range[i_range].step != 0 ? leaf->offset_range[i_range].step : 1;
            for(i_leaf = offset; i_leaf < offset + num * step; i_leaf += step)
            {
                if(program_data[i_leaf].macro_sel != JER_MACRO_NA)
                { /* Leaf was already initialized */
                    LOG_VERBOSE(BSL_LS_SOC_PKT,(BSL_META_U(0, "Entry:%d in Segment:%s already initiated\n"), i_leaf, segment->name));
                    continue;
                }
                format_data[i_leaf].packet_format_code   = pfc_hw;
                format_data[i_leaf].parser_leaf_context  = plc_hw;
                program_data[i_leaf].macro_sel           = macro_sel;
                program_data[i_leaf].next_addr_base      = base;
            }
        }
    }

    /* Step 7 - Initialize HW */
    instr_addr = segment->address * JER_SEGMENT_SMALL_SIZE;
    for(i_ind = 0; i_ind < segment->size; i_ind++)
    {
        int array_index;
        if(program_data[i_ind].macro_sel == JER_MACRO_NA)
        {
            LOG_ERROR(BSL_LS_SOC_PKT,(BSL_META_U(0, "Entry:%d in Segment:%s not initialized\n"), i_ind, segment->name));
            goto exit;
        }

        for (array_index = 0; array_index < ARAD_PARSER_NOF_PARSER_PROGRAMS_REPLICATIONS; ++array_index)
        {
          /* Parser program Replications */
          res = arad_pp_ihp_parser_program_tbl_set_unsafe(
                  unit,
                  array_index,
                  instr_addr + i_ind,
                  &program_data[i_ind]
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
        }

        res = arad_pp_ihp_packet_format_table_tbl_set_unsafe(
                unit,
                instr_addr + i_ind,
                &format_data[i_ind]
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    }

exit:
    return segment->address;
}

static void jer_parser_static_init(
        int unit)
{
    int i_ind, k_ind;

    /* Reconfigure the TM_leafs when Jericho work in ARAD mode */
    if(soc_property_get(unit, spn_ITMH_ARAD_MODE_ENABLE, 0)
       || soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "itmh_jericho_parse_disable", 0)) {      
        memcpy(&TM_leafs, &TM_leafs_arad, sizeof(TM_leafs_arad));
    }

    for(i_ind = 0; i_ind < DPP_CB_NOF; i_ind++)
        jer_cb_map[i_ind] = FALSE;

    for(i_ind = 0; i_ind < (sizeof(jer_segments) / sizeof(dpp_segment_t)); i_ind++)
    {
        if(jer_segments[i_ind].id == DPP_SEGMENT_FIN)
            jer_segments[i_ind].address = JER_SEGMENT_FIN;
        else if(jer_segments[i_ind].id == DPP_SEGMENT_END)
            jer_segments[i_ind].address = JER_SEGMENT_END;
        else
            jer_segments[i_ind].address = -1;
    }

    for(i_ind = 0; i_ind < JER_SEGMENT_NOF; i_ind++)
    {
        segment_map[unit][i_ind] = DPP_SEGMENT_NA;
        for(k_ind = 0; k_ind < JER_SEGMENT_COMPLEX_NOF; k_ind++)
            segment_complex_map[unit][i_ind][k_ind] = DPP_SEGMENT_NA;
    }

    for(i_ind = 0; i_ind < JER_CUSTOM_MACRO_NOF; i_ind++)
        macro_map[unit][i_ind] = DPP_MACRO_NA;

    return;
}
/* } */

/* Configure Second stage parsing according to advanced scheme */
static
uint32
jer_parser_second_stage_parser_init(
   SOC_SAND_IN  int                                 unit
   ) {
    uint32
       res = SOC_SAND_OK,
        tbl_data,
       reg_val;
    uint32 next_protocol, is_ipv4, entry;
    DPP_PFC_E sw_pfc;
    DPP_PLC_E sw_plc;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    for (next_protocol = 0; next_protocol < DPP_PARSER_NEXT_PROTOCOL_NOF; next_protocol++) {

        /* IPv4/6 Next protocol size */
        for (is_ipv4 = 0; is_ipv4 <= 1; is_ipv4++) {
            entry = (is_ipv4 << 4) | next_protocol;

            SOC_SAND_SOC_IF_ERROR_RETURN(res, 230, exit, READ_IHP_VTT_IP_PACKET_FORMAT_ATTRIBUTES_TABLEm(unit, MEM_BLOCK_ANY, entry, &tbl_data));
            if (next_protocol == DPP_PARSER_IP_NEXT_PROTOCOL_TCP || next_protocol == DPP_PARSER_IP_NEXT_PROTOCOL_UDP) {
                reg_val = 8; /* Size of TCP and UDP headers */
            } else {
                reg_val = 0; /* Not supported */
            }
            soc_IHP_VTT_IP_PACKET_FORMAT_ATTRIBUTES_TABLEm_field_set(unit, &tbl_data, NEXT_PROTOCOL_LENGTHf, &reg_val);

            if (next_protocol == DPP_PARSER_IP_NEXT_PROTOCOL_TCP || next_protocol == DPP_PARSER_IP_NEXT_PROTOCOL_UDP) {
                sw_pfc = is_ipv4 ? DPP_PFC_L4_IPV4isH3 : DPP_PFC_L4_IPV6isH3;
                sw_plc = is_ipv4 ? DPP_PLC_L4_IPv4isH3 : DPP_PLC_L4_IPv6isH3;
            } else {
                sw_pfc = is_ipv4 ? DPP_PFC_IPV4isH3 : DPP_PFC_IPV6isH3;
                sw_plc = is_ipv4 ? DPP_PLC_IPv4isH3 : DPP_PLC_IPv6isH3;
            }

            SOC_SAND_SOC_IF_ERROR_RETURN(res, 230, exit, dpp_parser_pfc_activate_hw_by_sw(unit, sw_pfc, &reg_val));
            soc_IHP_VTT_IP_PACKET_FORMAT_ATTRIBUTES_TABLEm_field_set(unit, &tbl_data, PACKET_FORMAT_CODEf, &reg_val);
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 230, exit, dpp_parser_plc_hw_by_sw(unit, sw_pfc, sw_plc, &reg_val, NULL));
            soc_IHP_VTT_IP_PACKET_FORMAT_ATTRIBUTES_TABLEm_field_set(unit, &tbl_data, PARSER_LEAF_CONTEXTf, &reg_val);

            SOC_SAND_SOC_IF_ERROR_RETURN(res, 230, exit, WRITE_IHP_VTT_IP_PACKET_FORMAT_ATTRIBUTES_TABLEm(unit, MEM_BLOCK_ANY, entry, &tbl_data));
        }
        /* Ethernet Next protocol size */
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 230, exit, READ_IHP_VTT_ETH_PACKET_FORMAT_ATTRIBUTES_TABLEm(unit, MEM_BLOCK_ANY, next_protocol, &tbl_data));
        if (next_protocol == DPP_PARSER_ETH_NEXT_PROTOCOL_IPv4) {
            sw_pfc = DPP_PFC_IPV4isH4_ETHisH3;
            sw_plc = DPP_PLC_IPv4isH4;
        } else if (next_protocol == DPP_PARSER_ETH_NEXT_PROTOCOL_IPv6) {
            sw_pfc = DPP_PFC_IPV6isH4_ETHisH3;
            sw_plc = DPP_PLC_IPv6isH4;
        } else {
            sw_pfc = DPP_PFC_ETHisH3;
            sw_plc = DPP_PLC_ETHisH3;
        }
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 230, exit, dpp_parser_pfc_activate_hw_by_sw(unit, sw_pfc, &reg_val));
        soc_IHP_VTT_ETH_PACKET_FORMAT_ATTRIBUTES_TABLEm_field_set(unit, &tbl_data, PACKET_FORMAT_CODEf, &reg_val);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 230, exit, dpp_parser_plc_hw_by_sw(unit, sw_pfc, sw_plc, &reg_val, NULL));
        soc_IHP_VTT_ETH_PACKET_FORMAT_ATTRIBUTES_TABLEm_field_set(unit, &tbl_data, PARSER_LEAF_CONTEXTf, &reg_val);
        reg_val = 0;
        soc_IHP_VTT_ETH_PACKET_FORMAT_ATTRIBUTES_TABLEm_field_set(unit, &tbl_data, NEXT_PROTOCOL_LENGTHf, &reg_val);
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 230, exit, WRITE_IHP_VTT_ETH_PACKET_FORMAT_ATTRIBUTES_TABLEm(unit, MEM_BLOCK_ANY, next_protocol, &tbl_data));
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in jer_parser_second_stage_parser_init()", 0, 0);
}

uint32
  jer_parser_init(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32  res = SOC_SAND_OK;
  ARAD_PP_IHP_PARSER_ETH_PROTOCOLS_TBL_DATA ihp_parser_eth_protocols_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PARSER_INIT);

  jer_parser_static_init(unit);

  ihp_parser_eth_protocols_tbl_data.eth_type_protocol     = ARAD_PARSER_ETHER_PROTO_6_1588_ETHER_TYPE;
  ihp_parser_eth_protocols_tbl_data.eth_sap_protocol      = ARAD_PARSER_ETHER_PROTO_6_1588_ETHER_TYPE;
  res = arad_pp_ihp_parser_eth_protocols_tbl_set_unsafe(
            unit,
            ARAD_PARSER_ETHER_PROTO_6_1588,
            &ihp_parser_eth_protocols_tbl_data
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = jer_parser_segment_add(unit, DPP_SEGMENT_BASE);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = jer_parser_second_stage_parser_init(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  /*
   * In Jericho, initialize the indirections per Packet-Format-code:
   */
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 144, exit, dpp_parser_pfc_map_init(unit));

  /*
   * Init the PFC info table:
   * indicate the location of the L4 header according to the Packet Format Code
   * Init all the values according to dpp_parser_pfc_info table in arad_parser.c
   * Header-4 (e.g., for IPoMPLSoEth). Exceptions:
   * Header-3 for IPoEth, but not IPoIPoEth
   */
  res = dpp_parser_pfc_l4_location_init(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in jer_parser_init()", 0, 0);
}
