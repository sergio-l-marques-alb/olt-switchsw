/**
 * \file diag_sand_resource.c
 *
 * Framework for sand shell commands development
 *
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#include <sal/appl/sal.h>

#include <shared/dbx/dbx_file.h>

#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/sand/diag_sand_utils.h>

#define BSL_LOG_MODULE BSL_LS_APPL_SHELL

/*
 * Commonly used condition callbacks for shell commands
 * {
 */
#include <soc/drv.h>

shr_error_e
sh_cmd_is_dnx(
    int unit,
    rhlist_t * test_list)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!SOC_IS_DNX(unit))
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
    }

    SHR_FUNC_EXIT;
}

shr_error_e
sh_cmd_is_dnxf(
    int unit,
    rhlist_t * test_list)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!SOC_IS_DNXF(unit))
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
    }

    SHR_FUNC_EXIT;
}

shr_error_e
sh_cmd_is_device(
    int unit,
    rhlist_t * test_list)
{
    SHR_FUNC_INIT_VARS(unit);

#ifdef ADAPTER_SERVER_MODE
    SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
#endif

    SHR_FUNC_EXIT;
}
/*
 * }
 */
static sh_sand_enum_t sh_enum_table_core[] = {
    {"max", 0, "max core ID", "DNX_DATA.device.general.nof_cores-1"},
    {"all", _SHR_CORE_ALL, "all cores"},
    {NULL}
};

sh_sand_enum_t sh_enum_table_lag_pool[] = {
    { /** No need to define value if plugin_str was defined */
     .string = "max",
     .desc = "Number of lag pools supported",
     .plugin_str = "DNX_DATA.trunk.parameters.nof_pools-1"},
    {NULL}
};

/* *INDENT-OFF* */
sh_sand_man_t sh_sand_sys_arguments_man = {
    .brief = "Options available for all commands",
    .full = "This set of options is available for any leaf command."
            "Functionality of these options is universal, although it may not be relevant for all commands"
};

sh_sand_option_t sh_sand_sys_arguments[] = {
    {"columns", SAL_FIELD_TYPE_STR,  "Comma-separated list of PRT columns to show",           "all"},
    {"core",    SAL_FIELD_TYPE_INT32, "Core ID for multi-core devices",                       "all", (void *)sh_enum_table_core},
    {"file",    SAL_FIELD_TYPE_STR,  "File name where command output will be placed",         ""},
    {"gold",    SAL_FIELD_TYPE_BOOL, "Save PRT XML output file under the DB gold result folder", "No"},
    {"new",     SAL_FIELD_TYPE_BOOL, "Overwrite output file with the new one", "No"},
    {"xpath",   SAL_FIELD_TYPE_STR,  "XPath in output file, relative to top, where output will be added", ""},
    {"folder",  SAL_FIELD_TYPE_STR,  "Directory name where command output will be placed",    ""},
    {"silence", SAL_FIELD_TYPE_BOOL, "Deprecate certain info messages from tabular printing", "No"},
    {NULL}
};

/*
 * All command and option keywords should be present to below global keyword list.
 * Before adding new keywords to this list:
 *      Please verify that there are no already existing keywords that may fit your needs
 * Capital letters will represent shortcut that may be used instead of full keyword
 * List should be arranged in alphabet order, assuming all letters are 'small' case.
 * Note that the special sign '_' is placed before 'a' (or 'A')
 */
sh_sand_keyword_t sh_sand_keywords[] = {
    {"ACCess"},
    {"AccessId"},
    {"ACCessOnly"},
    {"ACE"},
    {"ACL"},
    {"AclConTeXt"},
    {"ACTion"},
    {"ACTIVe_actions"},
    {"ADaPTeR"},
    {"Add"},
    {"ADdRess"},
    {"ALGO"},
    {"AliAS"},
    {"ALL"},
    {"ALLocaTion"},
    {"ALLOW_wb_test"},
    {"APP_ID"},
    {"APPLication"},
    {"ARgS"},
    {"ArpID"},
    {"ARR"},
    {"ARray"},
    {"ATTach"},
    {"AUTOcredit"},
    {"AutoNeg"},
    {"AVS"},
    {"BaCKGRouNDThreads"},
    {"BandWidth"},
    {"BaNK"},
    {"BaseQueue"},
    {"BCM"},
    {"BER"},
    {"Ber_Scan_Mode"},
    {"BerProJ"},
    {"BFD"},
    {"BFDEndPoint"},
    {"BFR_Id"},
    {"BIER"},
    {"BIST"},
    {"BLock"},
    {"BUFfer"},
    {"BY_ConTeXt"},
    {"BY_Fg"},
    {"C45"},
    {"CacHe"},
    {"CallBack"},
    {"CaNDidate"},
    {"CFC"},
    {"CGM"},
    {"CHanGed"},
    {"CHaNnel"},
    {"CL72"},
    {"ClassiFier"},
    {"CLeaN"},
    {"CLear"},
    {"CLoSe"},
    {"CLUSTER"},
    {"CoLoR"},
    {"CoLuMN"},
    {"CoMBiNe"},
    {"Command_ID"},
    {"CoMmanD_Reg"},
    {"CoMmit"},
    {"CoMPare"},
    {"COMPensation"},
    {"CoMPoSe"},
    {"Compressed_Forward_Layer_Type"},
    {"CoNDITion"},
    {"ConFiG"},
    {"CoNGeSTion"},
    {"CONNection"},
    {"CONnectivity"},
    {"ConTeXt"},
    {"ContextSelect"},
    {"CORE"},
    {"CoreDiSPlay"},
    {"CouNT"},
    {"COUnter"},
    {"CouNTerSet"},
    {"CReate"},
    {"CRPS"},               /* CounteRProceSsor */
    {"CTest"},
    {"CURrent"},
    {"CuSToM"},
    {"Da_Is_Mc"},
    {"DaTa"},
    {"DaTaBaSe"},
    {"DaTaIN_Reg"},
    {"DaTaOUT_Reg"},
    {"DaTaTyPe"},
    {"DB"},
    {"DBaL"},
    {"DeBuG"},
    {"DeCoDe"},
    {"DeFaulT"},
    {"DEFine"},
    {"DeINiT"},
    {"DeLeTe"},
    {"DESC"},
    {"DeSTination"},
    {"DeSTRoY"},
    {"DETach"},
    {"DETAiled"},
    {"DEV_ADdr"},
    {"DeViCe"},
    {"Device_Data"},
    {"DEviceReset"},
    {"DIAG"},
    {"DIRECT"},
    {"DIRection"},
    {"DISaBle"},
    {"DIsplay"},
    {"DIVisoR"},
    {"DMAC"},
    {"DNX"},
    {"DNXF"},
    {"DRAM"},
    {"DSC"},
    {"DuMP"},
    {"DYNAMIC"},
    {"ECC"},
    {"ECMP"},
    {"EEDB"},
    {"EEDB_LL"},
    {"EFES"},
    {"EGQ"},
    {"EGress"},
    {"EgressVlanEditing"},
    {"EKLeaP"},
    {"EM"},
    {"ENAble"},
    {"Enabled_Only"},
    {"ENCap"},
    {"END"},
    {"ENGiNe"},
    {"ENTry"},
    {"EPEPort"},
    {"EPNI"},
    {"ERPP"},
    {"EthRif"},
    {"ETPP"},
    {"EXec"},
    {"EXPort"},
    {"EXTended_Vref_Range"},
    {"EYEScan"},
    {"Fabric"},
    {"FAILed"},
    {"FEATURE"},
    {"FEC"},
    {"FECALloC"},
    {"FECStat"},
    {"FEM"},
    {"FieLD"},
    {"FILE"},
    {"FILENaMe"},
    {"FIND"},
    {"FLaG"},
    {"FLOW"},
    {"Flow_Control"},
    {"FlowId"},
    {"FoLDeR"},
    {"FORCE"},
    {"FORCE_HW"},
    {"FoRMat"},
    {"ForwardDecisionTrace"},
    {"FRameWork"},
    {"FRom"},
    {"FULL"},
    {"Get"},
    {"GLIFID"},
    {"GLOBal"},
    {"GoLD"},
    {"GPM"}, /** GPort_Management */
    {"GPort"},
    {"GRaphical"},
    {"GRouP"},
    {"GRouP_Type"},
    {"HanDLe"},
    {"HanDLeR"},
    {"HeaDeR"},
    {"HeaDeR_APPend"},
    {"Help"},
    {"HEX"},
    {"HieRarChy"},
    {"Hist_Errcnt_Threshold"},
    {"HIT"},
    {"HIT_Clr"},
    {"HL"},
    {"HOST"},
    {"HW_RESources"},
    {"HWElement"},
    {"HwPortBitMap"},
    {"ID"},
    {"IGnore_Address"},
    {"IKLeaP"},
    {"ILKN"},
    {"ILKN_ID"},
    {"INDex"},
    {"INDirectACCess"},
    {"INFo"},
    {"INGress"},
    {"IngressVlanEditing"},
    {"Init_dnx"},
    {"INITial"},
    {"INJect"},
    {"InLif"},
    {"INPut_CORE"},
    {"INSTance"},
    {"INTerFace"},
    {"INTERNAL"},
    {"INTeRrupt"},
    {"InTerVal"},
    {"InVerT"},
    {"IP"},
    {"IP6Mask"},
    {"IPT"},
    {"IRE"},
    {"IS_Flow"},
    {"IVL"},
    {"KBP"},
    {"KEY"},
    {"Key_Only"},
    {"Key_SIZE"},
    {"LaBeL"},
    {"LAG"},
    {"LANE"},
    {"LANE_MAP"},
    {"LAST"},
    {"Last_lookUp"},
    {"Layer"},
    {"LeVeL"},
    {"LIF"},
    {"LIFALloC"},
    {"LiNK"},
#ifdef PHYMOD_LINKCAT_SUPPORT
    {"LiNKCat"},
#endif
    {"LiNKSCan"},
    {"LIST"},
    {"LOAD"},
    {"Load_Balancing"},
    {"Local_To_Sys"},
    {"LoGger"},
    {"LogicalTable"},
    {"LooKuP"},
    {"LooPBack"},
    {"LooPBack_Mode"},
    {"LPM"},
    {"MAC"},
    {"MaCRo"},
    {"MAINThread"},
    {"ManaGeMenT"},
    {"MANual"},
    {"MAPping"},
    {"MaRGin"},
    {"MaRK"},
    {"MAsK"},
    {"MaTuRity"},
    {"MAX"},
    {"Max_Err_Control"},
    {"MBIST"},
    {"MC_Group"},
    {"MC_IP"},
    {"MC_IP6Mask"},
    {"MC_Mask"},
    {"MDB"},
    {"MDL"},
    {"MeaSuRe"},
    {"MEMory"},
    {"MEMoryUSE"},
    {"MEMREG"},
    {"MESH"},
    {"Mesh_Topology"},
    {"METER"},
    {"METER_ID"},
    {"MidStack"},
    {"MoDe"},
    {"ModeRegisters"},
    {"MODID"},
    {"MODify"},
    {"MODULE"},
    {"MOST"},
    {"MPLS"},
    {"MPLS_ACTION"},
    {"MTA"},
    {"MTU"},
    {"MultiCast"},
    {"MYMAC"},
    {"NaMe"},
    {"NEW"},
    {"NIF"},
    {"NO"},
    {"No_Rates"},
    {"NO_WB_test"},
    {"NoDeinit"},
    
    {"NofSegments"},
    {"NoInit"},
    {"NON_empty"},
    {"NonZero"},
    {"Num_BIts"},
    {"Num_ENTries"},
    {"NUMber"},
    {"NUMERIC"},
    {"OAM"},
    {"OAM_ID"},
    {"OAMEndPoint"},
    {"OAMP_CounTeRS"},
    {"Oamp_Mep_DB"},
    {"OAMP_PE"},
    {"OCCupation"},
    {"OFF"},
    {"OFFSet"},
    {"ON"},
    {"OOB_InterFace"},
    {"OPcode"},
    {"OPeRation"},
    {"ORDER"},
    {"OVERwrite"},
    {"PacKeT"},
    {"PacketParsingInfo"},
    {"PacKeTTM"},
    {"PARAMeter"},
    {"PaRSer"},
    {"PARTition"},
    {"PATCH"},
    {"PATH"},
    {"PatTeRN"},
    {"PayLoad"},
    {"PayLoad_SIZE"},
    {"PBMP"},
    {"PEMLA"},
    {"PHY"},
    {"PHY_ID"},
    {"PhyDB"},
    {"PIPE"},
    {"Pll_INDeX"},
    {"PLOT"},
    {"PM_ID"},
    {"POLL_Delay"},
    {"POLL_NOF_times"},
    {"POLYnomial"},
    {"POOL"},
    {"Port"},
    {"PortBitmaP"},
    {"PoRtDrop"},
    {"PortInfo"},
    {"PortMod"},
    {"PortScheDuler"},
    {"POSITION"},
    {"PP"},
    {"PQP"},
    {"PRBS"},
    {"PRBSStat"},
    {"PRE"},
    {"PReDeFined"},
#ifdef PHYMOD_NEW_LINKCAT
    {"PRefix"},
#endif
    {"PRefix_LeNgth"},
    {"PRinT"},
    {"PRinT_Flow_and_Up"},
    {"PRinT_Status"},
    {"PRIOrity"},
    {"PRoFiLe"},
    {"PRoGram"},
    {"PRoPerty"},
    {"PRoPerty_MANual"},
    {"PROTEct"},
    {"PROTOcol"},
    {"PVT"},
    {"QUALifier"},
    {"Queue"},
    {"QuieT"},
    {"RaNDom"},
    {"RaNGe"},
    {"RATE"},
    {"Rate_Class"},
    {"RAW"},
    {"REACHability"},
    {"Read"},
    {"Read_WEIGHT"},
    {"REASSembly"},
    {"RedirectToOCB"},
    {"REFresh"},
    {"REG_ADdr"},
    {"REGister"},
    {"ReMoVe"},
    {"ReQueST"},
    {"ReSeT"},
    {"ReSouRCe"},
    {"ReSPoNSe"},
    {"ReSuLt"},
    {"ReSuMe"},
    {"RiF"},
    {"RoLLBaCK"},
    {"ROP"},
    {"ROP_ReaD"},
    {"ROP_TeST"},
    {"ROP_WRite"},
    {"ROUTE"},
    {"ROUTES_FILE"},
    {"ROW"},
    {"RQP"},
    {"RUN"},
    {"RX"},
    {"SaMe_ROW_CouNT"},
    {"SaMPle_Time"},
    {"SBUS"},
    {"SCHeduler"},
    {"SCheMe"},
    {"SCReeN"},
    {"SeaRCH"},
    {"SEC_GRouP"},
    {"SEED"},
    {"SEMantic"},
    {"SePaRaTe"},
    {"SER"},
    {"Set"},
    {"SeVeRity"},
    {"ShaDoW"},
    {"ShaPeR"},
    {"SHMoo_Type"},
    {"SHORT"},
    {"SHOW"},
    {"SIGnal"},
    {"SiLence"},
    {"SiZe"},
    {"SKiP"},
    {"SMAC"},
    {"SniF"},
    {"SOC"},
    {"SoFT"},
    {"SouRCe"},
    {"SQM"},
    {"SRv6"},
    {"STAGE"},
    {"START"},
    {"STat_PP"},
    {"STaTe"},
    {"STatiC"},
    {"STATS"},
    {"StaTuS"},
    {"STG"},
    {"STIF"},
    {"STOP"},
    {"Stop_On_Error"},
    {"STP"},
    {"STRUCTure"},
    {"SuSPenD"},
    {"SVL"},
    {"SW"},
    {"SW_STate_SiZe"},
    {"SWItch"},
    {"SwPortBitMap"},
    {"SWSTate"},
    {"SYMbol"},
    {"SYNC"},
    {"SYS_Header"},
    {"SYSTEM"},
    {"SYStem_PORT"},
    {"SYStem_RED"},
    {"TaBLe"},
    {"TABular"},
    {"TCAM"},
    {"TCL"},
    {"TEMPerature"},
    {"TeMPLate"},
    {"TERMination"},
    {"Test"},
    {"Test_MoDe"},
    {"TGID"},
    {"THreShold"},
    {"TimeAnalyzer"},
    {"Timer_ConTRoL"},
    {"TM"},
    {"TO"},
    {"Traffic_Profile"},
    {"TrafficClass"},
    {"TRANSACTion"},
    {"TRAP"},
    {"TRUNK"},
    {"TTL"},
    {"TUNe"},
    {"TUNNel"},
    
    {"TUNnelConFiG"},
    
    {"TUNnelConFiGEgress"},
    
    {"TUNnelConFiGLegacy"},
    {"TX"},
    {"TYpe"},
    {"UniCast"},
    {"UNIT"},
    {"UNMask"},
    {"UntagBitMap"},
    {"Update"},
    {"USaGe"},
    {"UsageTEST"},
    {"USeR"},
    {"V6"},
    {"ValiD"},
    {"VALue"},
    {"VendorInfo"},
    {"VERBose"},
    {"VERSion"},
    {"VID"},
    {"VISibility"},
    {"VLan"},
    {"VOLTage"},
    {"VRF"},
    {"VSI"},
    {"VSQ"},
    {"WarmBoot"},
    {"Write"},
    {"Write_Command"},
    {"Write_WEIGHT"},
    {"XPATH"},
    {"YDV"},
    {"YES"},
    {"Zero"},
    {NULL}
};
/* *INDENT-ON* */

shr_error_e
sh_sand_keyword_shortcut_get(
    char *keyword,
    char *short_key)
{
    int i_ch, i_sh = 0;
    int len = sal_strlen(keyword);

    SHR_FUNC_INIT_VARS(NO_UNIT);

    for (i_ch = 0; i_ch < len; i_ch++)
    {
        if (isupper(keyword[i_ch]) || isdigit(keyword[i_ch]))
        {
            short_key[i_sh++] = keyword[i_ch];
        }
        /*
         * Shortcut size includes string ending, so limit by size -1
         */
        if (i_sh >= SH_SAND_MAX_SHORTCUT_SIZE)
        {
            SHR_CLI_EXIT(_SHR_E_INIT, "Keyword:%s requires shortcut more than max %d characters,\n",
                         keyword, SH_SAND_MAX_SHORTCUT_SIZE - 1);
        }
    }
    /*
     * Null terminate the string. If no capital - no shortcut was defined, shortcut will be empty string
     */
    short_key[i_sh] = 0;
exit:
    SHR_FUNC_EXIT;

}

shr_error_e
sh_sand_keyword_list_init(
    int unit)
{
    int i_key;
    int len;
    char keyword_var_str[SH_SAND_MAX_RESOURCE_SIZE];
    rhhandle_t keyword_handle;

    SHR_FUNC_INIT_VARS(unit);

    for (i_key = 0; sh_sand_keywords[i_key].keyword != NULL; i_key++)
    {   /* Build shortcut */
        /*
         * Skip the entry if list was already initialized
         */
        if (sh_sand_keywords[i_key].var_list[unit] != NULL)
        {
            continue;
        }
        len = sal_strlen(sh_sand_keywords[i_key].keyword);
        /*
         * Verify max size - it also insures that plural will not go over its allocation size
         */
        if (len >= SH_SAND_MAX_KEYWORD_SIZE)
        {
            SHR_CLI_EXIT(_SHR_E_INIT, "Keyword:\"%s\" length:%d is longer than max(%d)\n",
                         sh_sand_keywords[i_key].keyword, len, SH_SAND_MAX_KEYWORD_SIZE - 1);
        }
#ifdef END_DIGIT_VERIFY
        if (isdigit(sh_sand_keywords[i_key].keyword[len - 1]))
        {
            SHR_CLI_EXIT(_SHR_E_INIT, "Instead of keyword:\"%s\" define one without digit at the end\n"
                         "Once defined any keyword with digits at the end may be used freely\n",
                         sh_sand_keywords[i_key].keyword);
        }
#endif
        SHR_IF_ERR_EXIT(sh_sand_keyword_shortcut_get
                        (sh_sand_keywords[i_key].keyword, sh_sand_keywords[i_key].short_key));
        /*
         * Create list for all keyword variations
         */
        if ((sh_sand_keywords[i_key].var_list[unit] = utilex_rhlist_create("KeywordVars",
                                                                           sizeof(sh_sand_keyword_var_t), 1)) == NULL)
        {
            SHR_CLI_EXIT(_SHR_E_MEMORY, "ERROR - No memory for vars list\n");
        }
        /*
         * Fill plural form and null terminate it, pay attention that allocation is 3 characters longer than keyword
         */
        sal_strncpy(keyword_var_str, sh_sand_keywords[i_key].keyword, len + 1);
        if (sh_sand_keywords[i_key].keyword[len - 1] == 'y')
        {
            sal_strncpy((keyword_var_str + (len - 1)), "ies", 4);
        }       /* s, x, z, ch, sh */
        else if ((sal_strcasecmp((sh_sand_keywords[i_key].keyword + len - 1), "s") == 0)
                 || (sal_strcasecmp((sh_sand_keywords[i_key].keyword + len - 1), "x") == 0)
                 || (sal_strcasecmp((sh_sand_keywords[i_key].keyword + len - 1), "z") == 0)
                 || (sal_strcasecmp((sh_sand_keywords[i_key].keyword + len - 2), "ch") == 0)
                 || (sal_strcasecmp((sh_sand_keywords[i_key].keyword + len - 2), "sh") == 0))
        {
            sal_strncpy((keyword_var_str + len), "es", 3);
        }
        else
        {
            sal_strncpy((keyword_var_str + len), "s", 2);
        }
        /*
         * Add plural to the list, shortcut is the same as for keyword itself
         */
        if (utilex_rhlist_entry_add_tail(sh_sand_keywords[i_key].var_list[unit],
                                         keyword_var_str, RHID_TO_BE_GENERATED, &keyword_handle) != _SHR_E_NONE)
        {
            SHR_CLI_EXIT(_SHR_E_MEMORY, "ERROR - Cannot add plural:%s to keyword variants list\n", keyword_var_str);
        }
        sal_strncpy(((sh_sand_keyword_var_t *) keyword_handle)->short_key, sh_sand_keywords[i_key].short_key,
                    SH_SAND_MAX_SHORTCUT_SIZE - 1);
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
sh_sand_keyword_list_deinit(
    int unit)
{
    int i_key;
    SHR_FUNC_INIT_VARS(unit);

    for (i_key = 0; sh_sand_keywords[i_key].keyword != NULL; i_key++)
    {   /* Build shortcut */
        if (sh_sand_keywords[i_key].var_list[unit] != NULL)
        {
            utilex_rhlist_free_all(sh_sand_keywords[i_key].var_list[unit]);
            sh_sand_keywords[i_key].var_list[unit] = NULL;
        }
        if (sh_sand_keywords[i_key].cmd_list[unit] != NULL)
        {
            utilex_rhlist_free_all(sh_sand_keywords[i_key].cmd_list[unit]);
            sh_sand_keywords[i_key].cmd_list[unit] = NULL;
        }
        sh_sand_keywords[i_key].count = 0;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
sh_sand_keyword_fetch(
    int unit,
    char *command,
    char *keyword_in,
    char **shortcut_out_p,
    char **keyword_out_p,
    int flags)
{
    int i_key;
    char *keyword = NULL, *shortcut = NULL;
    rhhandle_t keyword_handle;
    int i_ch, in_len, cur_len, short_len, suffix_len = 0;
    char keyword_temp[SH_SAND_MAX_RESOURCE_SIZE], *keyword_short;

    SHR_FUNC_INIT_VARS(unit);

    if ((in_len = sal_strlen(keyword_in)) >= SH_SAND_MAX_KEYWORD_SIZE)
    {
        if (!(flags & SH_CMD_VERIFY))
        {
            /*
             * Assign default values for keywords that do not require verification
             */
            keyword = keyword_in;
            shortcut = NULL;
            SHR_EXIT();
        }
        else
        {
            SHR_CLI_EXIT(_SHR_E_MEMORY, "ERROR - Keyword:%s length:%d is over the limit:%d\n",
                         keyword_in, in_len, SH_SAND_MAX_KEYWORD_SIZE);
        }
    }
    /*
     * If there are digits with or without underscore ('_'), strip it and search without
     * IF it is the case add keyword_in in modified form to the list to be have available pointers
     */
    for (i_ch = in_len - 1; i_ch >= 0; i_ch--)
    {
        if (!isdigit(keyword_in[i_ch]) && (keyword_in[i_ch] != '_'))
        {
            suffix_len = in_len - (i_ch + 1);
            break;
        }
    }
    /*
     * If digits discovered
     */
    if (suffix_len != 0)
    {
        sal_memcpy(keyword_temp, keyword_in, in_len - suffix_len);
        keyword_temp[in_len - suffix_len] = 0;
    }

    for (i_key = 0; sh_sand_keywords[i_key].keyword != NULL; i_key++)
    {
        cur_len = sal_strlen(sh_sand_keywords[i_key].keyword);
        /*
         * First compare input to head keyword
         */
        if (!sal_strcasecmp(sh_sand_keywords[i_key].keyword, keyword_in))
        {
            keyword = sh_sand_keywords[i_key].keyword;
            if (ISEMPTY(sh_sand_keywords[i_key].short_key))
            {
                /*
                 * IF there is no shortcut yes - try to fill it, assign short_key pointer anyway
                 * Useful in case where keyword_init_list was not called before keyword fetching
                 */
                sh_sand_keyword_shortcut_get(sh_sand_keywords[i_key].keyword, sh_sand_keywords[i_key].short_key);
            }
            shortcut = sh_sand_keywords[i_key].short_key;
            break;
        }
        /*
         * Now look for any existing variants already in the list
         * May be plurals or already registered numbered keyword
         */
        else if ((keyword_handle = utilex_rhlist_entry_get_by_name(sh_sand_keywords[i_key].var_list[unit], keyword_in))
                 != NULL)
        {
            keyword = RHNAME(keyword_handle);
            shortcut = ((sh_sand_keyword_var_t *) keyword_handle)->short_key;
            break;
        }
        /*
         * Now if it was numbered keyword, compare striped version and add to the list, if there was a match
         */
        else if ((suffix_len != 0) && !sal_strcasecmp(sh_sand_keywords[i_key].keyword, keyword_temp))
        {
            /*
             * First - overwrite keyword_temp with capitalized resource one
             */
            sal_strncpy(keyword_temp, sh_sand_keywords[i_key].keyword, SH_SAND_MAX_RESOURCE_SIZE - 1);
            /*
             * Add the rest of original keyword to capitalized version
             */
            sal_memcpy(keyword_temp + cur_len, keyword_in + cur_len, suffix_len);
            /*
             * End the string  in_len is a sum of cur_len and suffix_len
             */
            keyword_temp[in_len] = 0;
            if (utilex_rhlist_entry_add_tail(sh_sand_keywords[i_key].var_list[unit],
                                             keyword_temp, RHID_TO_BE_GENERATED, &keyword_handle) != _SHR_E_NONE)
            {
                SHR_CLI_EXIT(_SHR_E_MEMORY, "ERROR - Cannot add:%s to keyword variants list\n", keyword_temp);
            }
            /*
             * Now fill appropriate shortcut according to the same pattern as the keyword itself
             */
            /*
             * First - copy original shortcut
             */
            keyword_short = ((sh_sand_keyword_var_t *) keyword_handle)->short_key;
            sal_strncpy(keyword_short, sh_sand_keywords[i_key].short_key, SH_SAND_MAX_SHORTCUT_SIZE - 1);
            short_len = sal_strlen(sh_sand_keywords[i_key].short_key);
            /*
             * Add the rest of original keyword to capitalized version
             * Skip first "_" in appended number string
             */
            if (keyword_in[cur_len] == '_')
            {
                suffix_len--;
            }
            sal_memcpy(keyword_short + short_len, keyword_in + in_len - suffix_len, suffix_len);
            short_len += suffix_len;
            /*
             * End the string
             */
            keyword_short[short_len] = 0;
            /*
             * Now we can finally assign return values
             */
            keyword = RHNAME(keyword_handle);
            shortcut = ((sh_sand_keyword_var_t *) keyword_handle)->short_key;
            break;
        }
    }

    if (sh_sand_keywords[i_key].keyword == NULL)
    {
        if (!(flags & SH_CMD_VERIFY))
        {
            /*
             * We tried to find shortcut even for keyword without verification request, but no failure if not
             * Assign default values for keywords that do not require verification
             */
            keyword = keyword_in;
            shortcut = NULL;
            SHR_EXIT();
        }
        else
        {
            SHR_CLI_EXIT(_SHR_E_NOT_FOUND, "ERROR: Keyword:\"%s\" was not registered\n"
                         "\tTo register use sh_sand_keywords in diag_sand_resource.c\n", keyword_in);
        }
    }

        /** Update resource statistics only for requests with verification */
    sh_sand_keywords[i_key].count++;
    if (command != NULL)
    {
        if (sh_sand_keywords[i_key].cmd_list[unit] == NULL)
        {
            if ((sh_sand_keywords[i_key].cmd_list[unit] = utilex_rhlist_create("Commands",
                                                                               sizeof(rhentry_t), FALSE)) == NULL)
            {
                SHR_CLI_EXIT(_SHR_E_MEMORY, "ERROR - No memory for commands list\n");
            }
        }
        /*
         * Add command to the list
         */
        if (utilex_rhlist_entry_add_tail(sh_sand_keywords[i_key].cmd_list[unit],
                                         command, RHID_TO_BE_GENERATED, &keyword_handle) != _SHR_E_NONE)
        {
            SHR_CLI_EXIT(_SHR_E_MEMORY, "ERROR - Cannot add command:%s to keyword commands list\n", command);
        }
    }

exit:
    if (keyword_out_p != NULL)
    {
        *keyword_out_p = keyword;
    }
    if (shortcut_out_p != NULL)
    {
        *shortcut_out_p = shortcut;
    }
    SHR_FUNC_EXIT;
}

#ifndef NO_FILEIO
static sh_sand_man_t compare_man = {
    .brief = "Compare XML file with its golden analog",
    .examples = "list=simple"
};

static sh_sand_option_t compare_arguments[] = {
    {"list", SAL_FIELD_TYPE_STR, "Filter list name from XML file", ""},
    {NULL}
};

static shr_error_e
compare_filter_get_list(
    int unit,
    char *filter_n,
    rhlist_t ** token_list_p)
{
    xml_node curTop = NULL, curSubTop = NULL, curLine;
    char filename[RHSTRING_MAX_SIZE];

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(token_list_p, _SHR_E_PARAM, "token_list_p");

    if (ISEMPTY(filter_n))
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "Empty filter name\n");
    }
    if (((curTop = dbx_file_get_xml_top(unit, "DNX-Devices.xml", "top", 0)) == NULL) ||
        ((curSubTop = dbx_xml_child_get_first(curTop, "case-objects")) == NULL))
    {
        /*
         * No parsing objects - just leave
         */
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }

    RHDATA_ITERATOR(curLine, curSubTop, "include")
    {
        char type[RHNAME_MAX_SIZE];

        RHDATA_GET_STR_DEF_NULL(curLine, "type", type);
        if (!sal_strcasecmp(type, "filters"))
        {
            xml_node curFile, curSection, curFilter, curToken;
            int device_specific = 0;
            int flags = 0;

            RHDATA_GET_INT_DEF(curLine, "device_specific", device_specific, 0);
            if (device_specific == TRUE)
            {
                flags = CONF_OPEN_PER_DEVICE;
            }

            RHDATA_GET_STR_CONT(curLine, "file", filename);
            if ((curFile = dbx_file_get_xml_top(unit, filename, "top", flags)) == NULL)
                continue;

            /*
             * Go through packets and found the requested one
             */
            if ((curSection = dbx_xml_child_get_first(curFile, "filters")) == NULL)
            {
                dbx_xml_top_close(curFile);
                continue;
            }

            RHDATA_ITERATOR(curFilter, curSection, "filter")
            {
                char name[RHNAME_MAX_SIZE];
                RHDATA_GET_STR_DEF_NULL(curFilter, "name", name);
                if (!sal_strcasecmp(filter_n, name))
                {
                    rhhandle_t token;
                    rhlist_t *token_list;
                    if ((token_list = utilex_rhlist_create("FilterTokens", sizeof(rhentry_t), 1)) == NULL)
                        break;
                    RHDATA_ITERATOR(curToken, curFilter, "token")
                    {
                        char name[RHNAME_MAX_SIZE];

                        RHDATA_GET_STR_CONT(curToken, "name", name);
                        if (utilex_rhlist_entry_add_tail(token_list, name, RHID_TO_BE_GENERATED, &token) != _SHR_E_NONE)
                            break;
                    }
                    *token_list_p = token_list;
                    dbx_xml_top_close(curFile);
                    SHR_EXIT();
                }
            }
            dbx_xml_top_close(curFile);
        }
    }

exit:
    dbx_xml_top_close(curTop);
    SHR_FUNC_EXIT;
}

static shr_error_e
compare_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *filename;
    char gold_filename[RHFILE_MAX_SIZE];
    char filepath[RHFILE_MAX_SIZE];
    char line[SH_SAND_MAX_TOKEN_SIZE], gold_line[SH_SAND_MAX_TOKEN_SIZE];
    FILE *result = NULL, *gold_result = NULL;
    char *filter_n;
    rhlist_t *token_list = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR("file", filename);
    SH_SAND_GET_STR("list", filter_n);
    if (ISEMPTY(filename))
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "No input file for comparison\n");
    }
    /*
     * For ADAPTER generate file in sub-folder of gold, for device use the gold itself
     */
    if (sh_cmd_is_device(unit, NULL) == _SHR_E_NONE)
    {
        sal_snprintf(gold_filename, RHFILE_MAX_SIZE, "gold/%s", filename);
    }
    else
    {
        sal_snprintf(gold_filename, RHFILE_MAX_SIZE, "gold/adapter/%s", filename);
    }

    if (dbx_file_get_file_path(unit, gold_filename, CONF_OPEN_PER_DEVICE, filepath) != _SHR_E_NONE)
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "Gold result file:'%s' cannot be found\n", gold_filename);
    }

    if ((result = sal_fopen(filename, "r")) == NULL)
    {   /* Could not open the file, usually means that path was illegal */
        SHR_CLI_EXIT(_SHR_E_PARAM, "File to be compared:'%s' cannot be opened\n", filename);
    }

    if ((gold_result = sal_fopen(filepath, "r")) == NULL)
    {   /* Could not open the file, usually means that path was illegal */
        SHR_CLI_EXIT(_SHR_E_PARAM, "Gold result file:'%s' cannot be opened\n", filepath);
    }

    if (!ISEMPTY(filter_n))
    {
        SHR_IF_ERR_EXIT(compare_filter_get_list(unit, filter_n, &token_list));
    }

    while (NULL != sal_fgets(line, SH_SAND_MAX_TOKEN_SIZE - 1, result))
    {
        if (sal_fgets(gold_line, SH_SAND_MAX_TOKEN_SIZE - 1, gold_result) == NULL)
        {
            SHR_CLI_EXIT(_SHR_E_PARAM, "Line:'%s' from file:'%s' has no pair in gold result\n", line, filename);
        }
        if (sal_strcmp(line, gold_line))
        {
            rhentry_t *token_entry;
            /*
             * Skip line with timestamp and command itself and token from token_list
             */
            if (sal_strcasestr(gold_line, "timestamp") != NULL)
                continue;

            RHITERATOR(token_entry, token_list)
            {
                if (sal_strcasestr(gold_line, RHNAME(token_entry)) != NULL)
                    break;
            }
            /*
             * If we reached the end of the list - no token found matching
             */
            if (token_entry != NULL)
                continue;

            LOG_CLI((BSL_META("%s\n\t%s%s\n\t%s"), filename, line, gold_filename, gold_line));
            SHR_SET_CURRENT_ERR(_SHR_E_PARAM);
        }
    }

exit:
    if (result != NULL)
        dbx_file_close(result);
    if (gold_result != NULL)
        dbx_file_close(gold_result);

    SHR_FUNC_EXIT;
}

static sh_sand_man_t delete_man = {
    .brief = "Compare XML file with its golden analog",
    .examples = "file=1234.xml"
};

static shr_error_e
delete_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *filename;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR("file", filename);
    if (ISEMPTY(filename))
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "No input file for deletion\n");
    }
    SHR_CLI_EXIT_IF_ERR(dbx_file_remove(filename), "");

exit:
    SHR_FUNC_EXIT;
}
#endif

static sh_sand_man_t resource_man = {
    .brief = "Show all keywords",
    .full = "Print keywords, their plural forms and shortcuts, filtered by options",
    .synopsis = NULL,   /* synopsis will be autogenerated */
    .examples = "all\n" "name=size\n" "name=LIF count=2-8 full\n" "path=\"dbal table\" full",
};

static sh_sand_option_t resource_arguments[] = {
    {"name", SAL_FIELD_TYPE_STR, "String or substring to filter keywords upon", ""},
    {"path", SAL_FIELD_TYPE_STR, "String or substring to filter command path", ""},
    {"count", SAL_FIELD_TYPE_INT32, "Filter according the number of appearances", "-1"},
    {"full", SAL_FIELD_TYPE_BOOL, "Show commands requested this keyword", "NO"},
    {"all", SAL_FIELD_TYPE_BOOL, "Print all keywords", "no"},
    {NULL}
};

static shr_error_e
resource_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *match_n, *path_n;
    int all_flag;
    int i_key;
    int full_flag;
    int keyword_count = 0;
    int count_low, count_high;
    rhentry_t *cmd_entry;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("Registered Keywords");
    /*
     * Prepare header, pay attention to put header items and content in the same order
     */
    SH_SAND_GET_BOOL("all", all_flag);
    SH_SAND_GET_BOOL("full", full_flag);
    SH_SAND_GET_STR("name", match_n);
    SH_SAND_GET_STR("path", path_n);
    SH_SAND_GET_INT32_RANGE("count", count_low, count_high);

    PRT_COLUMN_ADD("Keyword");
    PRT_COLUMN_ADD("Shortcut");
    PRT_COLUMN_ADD("Count");
    if (full_flag == TRUE)
    {
        PRT_COLUMN_ADD("Command");
    }

    if (ISEMPTY(match_n) && ISEMPTY(path_n) && all_flag == FALSE)
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "Please specify filtering criteria or use \"all\" to show all keywords\n");
    }

    for (i_key = 0; sh_sand_keywords[i_key].keyword != NULL; i_key++)
    {
        if ((all_flag != TRUE) && (sal_strcasestr(sh_sand_keywords[i_key].keyword, match_n) == NULL))
            continue;

        if (!ISEMPTY(path_n))
        {
            RHITERATOR(cmd_entry, sh_sand_keywords[i_key].cmd_list[unit])
            {
                if (sal_strcasestr(RHNAME(cmd_entry), path_n) != NULL)
                    break;
            }
            /** Check if at least one command matched the criteria */
            if (cmd_entry == NULL)
                continue;
        }

        if ((count_low != -1) &&
            ((sh_sand_keywords[i_key].count < count_low) || (sh_sand_keywords[i_key].count > count_high)))
            continue;

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s", sh_sand_keywords[i_key].keyword);
        PRT_CELL_SET("%s", sh_sand_keywords[i_key].short_key);
        PRT_CELL_SET("%d", sh_sand_keywords[i_key].count);
        keyword_count++;
        if (full_flag == FALSE)
        {
            sh_sand_keyword_var_t *keyword_var;
            RHITERATOR(keyword_var, sh_sand_keywords[i_key].var_list[unit])
            {
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SET_SHIFT(1, "%s", RHNAME(keyword_var));
                PRT_CELL_SET("%s", keyword_var->short_key);
            }
        }
        else
        {
            int first = TRUE;
            RHITERATOR(cmd_entry, sh_sand_keywords[i_key].cmd_list[unit])
            {
                if (!ISEMPTY(path_n) && (sal_strcasestr(RHNAME(cmd_entry), path_n) == NULL))
                    continue;
                if (first == TRUE)
                {
                    first = FALSE;
                }
                else
                {
                    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                    PRT_CELL_SKIP(3);
                }
                PRT_CELL_SET("%s", RHNAME(cmd_entry));
            }
        }
    }

    PRT_INFO_ADD("%d keywords in the list", keyword_count);
    PRT_INFO_SET_MODE(PRT_ROW_SEP_UNDERSCORE_BEFORE);
    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

sh_sand_man_t sh_sand_shell_man = {
    .brief = "Shell management commands",
};

sh_sand_cmd_t sh_sand_shell_cmds[] = {
#ifndef NO_FILEIO
    {"compare", compare_cmd, NULL, compare_arguments, &compare_man, NULL, NULL, SH_CMD_SKIP_EXEC}
    ,
    {"delete", delete_cmd, NULL, NULL, &delete_man, NULL, NULL, SH_CMD_SKIP_EXEC}
    ,
#endif
    {"resource", resource_cmd, NULL, resource_arguments, &resource_man}
    ,
    {NULL}
};
