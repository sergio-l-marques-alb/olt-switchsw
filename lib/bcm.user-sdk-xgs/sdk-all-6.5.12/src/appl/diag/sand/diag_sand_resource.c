/**
 * \file diag_sand_resource.c
 *
 * Framework for sand shell commands development
 *
 */
/*
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/sand/diag_sand_utils.h>
#include <shared/gport.h>

#define BSL_LOG_MODULE BSL_LS_APPL_SHELL

static sh_sand_enum_t core_enum_table[] = {
    {"all", _SHR_CORE_ALL},
    {NULL}
};

/* *INDENT-OFF* */
sh_sand_option_t sh_sand_sys_options[] = {
    {"columns", SAL_FIELD_TYPE_STR,  "Comma-separated list of PRT columns to show",        "all"},
    {"core",    SAL_FIELD_TYPE_INT32, "Core ID for multi-core devices",                     "all", (void *)core_enum_table},
    {"file",    SAL_FIELD_TYPE_STR,  "File name where command output will be placed",      ""} ,
    {"folder",  SAL_FIELD_TYPE_STR,  "Directory name where command output will be placed", ""},
    {"silence", SAL_FIELD_TYPE_BOOL, "Deprecate info messages from PRT",                   "No"},
    {NULL}
};

/*
 * All command and option keywords should be present to below global keyword list.
 * Before adding new keywords to this list:
 *      Please verify that there are no already existing keywords that may fit your needs
 * Capital letters will represent shortcut that may be used instead of full keyword
 * List should be arranged in alphabet order
 */
sh_sand_keyword_t sh_sand_keywords[] = {
    {"ACCess"}
    ,
    {"AccessId"}
    ,
    {"ACTion"}
    ,
    {"ADaPTeR"}
    ,
    {"Add"}
    ,
    {"ADdRess"}
    ,
    {"ALGO"}
    ,
    {"ALL"}
    ,
    {"ALLOCate"}
    ,
    {"APP_ID"}
    ,
    {"APPLication"}
    ,
    {"ARR"}
    ,
    {"ARray"}
    ,
    {"ATTach"}
    ,
    {"AUTOcredit"}
    ,
    {"AVS"}
    ,
    {"BCM"}
    ,
    {"BFD"}
    ,
    {"BitMaP"}
    ,
    {"BLock"}
    ,
    {"BUFfer"}
    ,
    {"CacHe"}
    ,
    {"CallBack"}
    ,
    {"CaNDidate"}
    ,
    {"CaScaDe"}
    ,
    {"CHanGed"}
    ,
    {"CLASS"}
    ,
    {"ClassiFier"}
    ,
    {"CLeaN"}
    ,
    {"CLear"}
    ,
    {"CLoSe"}
    ,
    {"CO_EXist"}
    ,
    {"CoLuMN"}
    ,
    {"CoMBiNe"}
    ,
    {"CoMmit"}
    ,
    {"CoMPoSe"}
    ,
    {"ConFiG"}
    ,
    {"CoNGeSTion"}
    ,
    {"CONnectivity"}
    ,
    {"Consistent"}
    ,
    {"Consistent_Hashing_Manager"}
    ,
    {"ConTeXt"}
    ,
    {"ConTeXt_Range"}
    ,
    {"ContextSelect"}
    ,
    {"COPY"}
    ,
    {"CORE"}
    ,
    {"CoreDiSPlay"}
    ,   /* Core DiSPlay */
    {"CouNT"}
    ,
    {"COUnter"}
    ,
    {"CouNTerSet"}
    ,
    {"CReate"}
    ,
    {"CRPS"}
    ,   /* CounteRProceSsor */
    {"CuSToM"}
    ,
    {"Da_Is_Mc"}
    ,
    {"DaTa"}
    ,
    {"DaTaBaSe"}
    ,
    {"DBaL"}
    ,
    {"DBaL_Ut"}
    ,
    {"DeCoDe"}
    ,
    {"DeFaulT"}
    ,
    {"DEFine"}
    ,
    {"DEFRAGmentedCHunk"}
    ,
    {"DeINiT"}
    ,
    {"DeLeTe"}
    ,
    {"DESC"}
    ,
    {"DeSTination"}
    ,
    {"DeSTRoY"}
    ,
    {"DETach"}
    ,
    {"DIAG"}
    ,
    {"Dir_Ext"}
    ,
    {"DIRECT"}
    ,
    {"DIRection"}
    ,
    {"DISaBle"}
    ,
    {"DIsplay_RaNge"}
    ,
    {"DIsplay_Single"}
    ,
    {"DNX"}
    ,
    {"DuMP"}
    ,
    {"DYNAMIC"}
    ,
    {"ECMP"}
    ,
    {"EEDB"}
    ,
    {"EGress"}
    ,
    {"EM"}
    ,
    {"ENAble"}
    ,
    {"END"}
    ,
    {"ENGiNe"}
    ,
    {"ENTry"}
    ,
    {"ENum"}
    ,
    {"ErDis"}
    ,
    {"ETHernet"}
    ,
    {"EXec"}
    ,
    {"EXPort"}
    ,
    {"Fabric"}
    ,
    {"FEATURE"}
    ,
    {"FieLD"}
    ,
    {"FILE"}
    ,
    {"FLaGs"}
    ,
    {"FoLDeR"}
    ,
    {"FRameWork"}
    ,
    {"FRom"}
    ,
    {"FULL"}
    ,
    {"Get"}
    ,
    {"GLOBal"}
    ,
    {"GRaphical"}
    ,
    {"GRouP"}
    ,
    {"GRouP_Add"}
    ,
    {"GRouP_Type_range"}
    ,
    {"HanDLe"}
    ,
    {"HashTaBle"}
    ,
    {"HeaDeR"}
    ,
    {"Help"}
    ,
    {"HEX"}
    ,
    {"HIT"}
    ,
    {"HL"}
    ,
    {"HWElement"}
    ,
    {"ID"}
    ,
    {"INDex"}
    ,
    {"INFo"}
    ,
    {"INGress"}
    ,
    {"INJect"}
    ,
    {"InLifProfile"}
    ,
    {"INPut"}
    ,
    {"InstruINT"}
    ,
    {"INTeger"}
    ,
    {"INTeRrupt"}
    ,
    {"InTerVal"}
    ,
    {"IP"}
    ,
    {"IP4"}
    ,
    {"IP6"}
    ,
    {"IS_EGRess"}
    ,
    {"IS_GLOBal"}
    ,
    {"ITMH"}
    ,
    {"JouRNaL"}
    ,
    {"KBR"}
    ,
    {"KEY"}
    ,
    {"Key_SIZE"}
    ,
    {"KLeaP"}
    ,
    {"L2"}
    ,
    {"L3"}
    ,
    {"LaBeL"}
    ,
    {"Last"}
    ,
    {"LAYER"}
    ,
    {"LIF"}
    ,
    {"LIF1"}
    ,
    {"LIF2"}
    ,
    {"LIF3"}
    ,
    {"LINK"}
    ,
    {"LinkedList"}
    ,
    {"LINKscan"}
    ,
    {"LIST"}
    ,
    {"Local_To_Sys"}
    ,
    {"LoGger"}
    ,
    {"LPM"}
    ,
    {"LTT"}
    ,
    {"Ltt_Full"}
    ,
    {"MAC"}
    ,
    {"MaCRo"}
    ,
    {"MAPping"}
    ,
    {"MaRGin"}
    ,
    {"MAsK"}
    ,
    {"MaTuRity"}
    ,
    {"MDB"}
    ,
    {"MDL"}
    ,
    {"MEMory"}
    ,
    {"Mesh_Topology"}
    ,
    {"METER"}
    ,
    {"METER_ID"}
    ,
    {"MGMT"}
    ,
    {"MoDe"}
    ,
    {"MODify"}
    ,
    {"MultiCast"}
    ,
    {"MultiSET2"}
    ,
    {"MultiSET3"}
    ,
    {"MultiSET4"}
    ,
    {"MuTeX"}
    ,
    {"MYMAC"}
    ,
    {"NaMe"}
    ,
    {"NIF"}
    ,
    {"NonZero"}
    ,
    {"Num_DB"}
    ,
    {"Num_ENTries"}
    ,
    {"Num_ITeRs"}
    ,
    {"NUMber"}
    ,
    {"NUMERIC"}
    ,
    {"OAM"}
    ,
    {"OCC_Bitmap1"}
    ,
    {"OCC_bItmap2"}
    ,
    {"OCC_biTmap3"}
    ,
    {"OCC_bitMap4"}
    ,
    {"OFF"}
    ,
    {"OFFSet"}
    ,
    {"ON"}
    ,
    {"OPcode"}
    ,
    {"ORDER"}
    ,
    {"OVERwrite"}
    ,
    {"PacKeT"}
    ,
    {"PARAMeter"}
    ,
    {"PaRSer"}
    ,
    {"PayLoad"}
    ,
    {"PayLoad_SIZE"}
    ,
    {"PFC"}
    ,
    {"PhyDB"}
    ,
    {"PIPE"}
    ,
    {"PLC"}
    ,
    {"PLuRal"}
    ,
    {"PmfPRog"}
    ,
    {"PoinTeR"}
    ,
    {"Port"}
	,
    {"PortBitmaP"}
    ,
    {"PP"}
    ,
    {"PRE"}
    ,
    {"PRefix_LeNgth"}
    ,
    {"PRinT"}
    ,
    {"PRIOrity"}
    ,
    {"PriorityDecoder"}
    ,
    {"PRoPerty"}
    ,
    {"PROTOcol"}
    ,
    {"PVT"}
    ,
    {"QUALifier"}
    ,
    {"Queue"}
    ,
    {"Queue_MAX"}
    ,
    {"Queue_MIN"}
    ,
    {"RaNGe"}
    ,
    {"RATE"}
    ,
    {"Rate_Class"}
    ,
    {"RAW"}
    ,
    {"REACHability"}
    ,
    {"Read"}
    ,
    {"REASSembly"}
    ,
    {"REGister"}
    ,
    {"ReMoVe"}
    ,
    {"ReSeT"}
    ,
    {"ReSouRCe"}
    ,
    {"ReSuMe"}
    ,
    {"RUN"}
    ,
    {"SaMe_PRIOrity"}
    ,
    {"SeaRCH"}
    ,
    {"SeGMent"}
    ,
    {"SEMantic"}
    ,
    {"SePaRaTe"}
    ,
    {"Set"}
    ,
    {"SeVeRity"}
    ,
    {"SHell"}
    ,
    {"SHOW"}
    ,
    {"SIGnal"}
    ,
    {"SiLence"}
    ,
    {"SiZe"}
    ,
    {"SOrted_Ll"}
    ,
    {"SouRCe"}
    ,
    {"STAGE"}
    ,
    {"STAGE_Range"}
    ,
    {"START"}
    ,
    {"STaTe"}
    ,
    {"STATS"}
    ,
    {"StaTuS"}
    ,
    {"STG"}
    ,
    {"STP"}
    ,
    {"STRUCTure"}
    ,
    {"SubResourceIndex"}
    ,
    {"SuSPenD"}
    ,
    {"SWSTate"}
    ,
    {"SYMbol"}
    ,
    {"TaBLe"}
    ,
    {"TABular"}
    ,
    {"TCam"}
    ,
    {"TeMPLate"}
    ,
    {"TeST"}
    ,
    {"THreShold"}
    ,
    {"TM"}
    ,
    {"TO"}
    ,
    {"Traffic_Profile"}
    ,
    {"TRAJectory"}
    ,
    {"TRAP"}
    ,
    {"TX"}
    ,
    {"TYpe"}
    ,
    {"UNMask"}
    ,
    {"Update"}
    ,
    {"USaGe"}
    ,
    {"ValiD"}
    ,
    {"VALue"}
    ,
    {"VARiable"}
    ,
    {"VID"}
    ,
    {"VLan"}
    ,
    {"VSQ"}
    ,
    {"WB"}
    ,
    {"With_ID"}
    ,
    {"Write"}
    ,
    {"YDV"}
    ,
    {NULL}
};
/* *INDENT-ON* */

shr_error_e
sh_sand_keyword_list_init(
    void)
{
    int i_key, i_ch, i_sh;
    int len;
    static int initialized = FALSE;

    SHR_FUNC_INIT_VARS(NO_UNIT);

    if (initialized == TRUE)
    {
        SHR_EXIT();
    }
    else
    {
        initialized = TRUE;
    }

    for (i_key = 0; sh_sand_keywords[i_key].keyword != NULL; i_key++)
    {   /* Build shortcut */
        len = sal_strlen(sh_sand_keywords[i_key].keyword);
        if (len >= SH_SAND_MAX_KEYWORD_SIZE)
        {
            SHR_CLI_EXIT(_SHR_E_INIT, "Keyword:\"%s\" length:%d is longer than max(%d)\n",
                         sh_sand_keywords[i_key].keyword, len, SH_SAND_MAX_KEYWORD_SIZE);
        }
        i_sh = 0;
        for (i_ch = 0; i_ch < len; i_ch++)
        {
            if (isupper(sh_sand_keywords[i_key].keyword[i_ch]) || isdigit(sh_sand_keywords[i_key].keyword[i_ch]))
            {
                sh_sand_keywords[i_key].short_key[i_sh++] = sh_sand_keywords[i_key].keyword[i_ch];
            }
        }
        /*
         * Null terminate the string. If no capital - no shortcut was defined
         */
        sh_sand_keywords[i_key].short_key[i_sh] = 0;
        /*
         * Fill plural form and null terminate it, pay attention that allocation is 3 characters longer than keyword
         */
        sal_strncpy(sh_sand_keywords[i_key].plural, sh_sand_keywords[i_key].keyword, len + 1);
        if (sh_sand_keywords[i_key].keyword[len - 1] == 'y')
        {
            sal_strncpy((sh_sand_keywords[i_key].plural + (len - 1)), "ies", 4);
        }       /* s, x, z, ch, sh */
        else if ((sal_strcasecmp((sh_sand_keywords[i_key].keyword + len - 1), "s") == 0)
                 || (sal_strcasecmp((sh_sand_keywords[i_key].keyword + len - 1), "x") == 0)
                 || (sal_strcasecmp((sh_sand_keywords[i_key].keyword + len - 1), "z") == 0)
                 || (sal_strcasecmp((sh_sand_keywords[i_key].keyword + len - 2), "ch") == 0)
                 || (sal_strcasecmp((sh_sand_keywords[i_key].keyword + len - 1), "sh") == 0))
        {
            sal_strncpy((sh_sand_keywords[i_key].plural + len), "es", 3);
        }
        else
        {
            sal_strncpy((sh_sand_keywords[i_key].plural + len), "s", 2);
        }
        sh_sand_keywords[i_key].plural[SH_SAND_MAX_RESOURCE_SIZE - 1] = 0;
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
sh_sand_keyword_fetch(
    char *keyword_in,
    char **shortcut_out_p,
    char **keyword_out_p,
    int flags)
{
    int i_key;
    char *keyword = NULL, *shortcut = NULL;

    SHR_FUNC_INIT_VARS(NO_UNIT);
    for (i_key = 0; sh_sand_keywords[i_key].keyword != NULL; i_key++)
    {
        if (!sal_strcasecmp(sh_sand_keywords[i_key].keyword, keyword_in))
        {
            keyword = sh_sand_keywords[i_key].keyword;
            shortcut = sh_sand_keywords[i_key].short_key;
            break;
        }
        /*
         * May be the last letter is 's', so look for singular form
         * or May be word ends with y, so plural will be "ies", we have plural form in place anyway
         */
        else if (!sal_strcasecmp(sh_sand_keywords[i_key].plural, keyword_in))
        {
            keyword = sh_sand_keywords[i_key].plural;
            shortcut = sh_sand_keywords[i_key].short_key;
            break;
        }
    }

    if (sh_sand_keywords[i_key].keyword == NULL)
    {
        if (flags & SH_SAND_VERIFY_KEYWORDS)
        {
            SHR_CLI_EXIT(_SHR_E_NOT_FOUND, "Keyword:\"%s\" was not registered\n", keyword_in);
        }
        else
        {
            /*
             * Assign default values
             */
            keyword = keyword_in;
            shortcut = NULL;
        }
    }
    else
    {   /* Keyword found, update usage count */
        sh_sand_keywords[i_key].count++;
    }

    if (keyword_out_p != NULL)
    {
        *keyword_out_p = keyword;
    }
    if (shortcut_out_p != NULL)
    {
        *shortcut_out_p = shortcut;
    }

exit:
    SHR_FUNC_EXIT;
}

static sh_sand_man_t resource_man = {
    "Show all keywords",
    "Print keywords, thier plural forms and shortcuts, filtered by options",
    "mgmt resource <name=[...]>",
    "mgmt resource all\n" "mgmt resource name=des",
};

static sh_sand_option_t resource_options[] = {
    {"name", SAL_FIELD_TYPE_STR, "String or substring to filter keywords upon", ""},
    {"all", SAL_FIELD_TYPE_BOOL, "Print all keywords", "no"},
    {NULL}
};

static shr_error_e
resource_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *match_n;
    int all_flag;
    int i_key;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("Registered Keywords");
    /*
     * Prepare header, pay attention to put header items and content in the same order
     */
    PRT_COLUMN_ADD("Keyword");
    PRT_COLUMN_ADD("Plural");
    PRT_COLUMN_ADD("Shortcut");
    PRT_COLUMN_ADD("Count");

    SH_SAND_GET_BOOL("all", all_flag);
    SH_SAND_GET_STR("name", match_n);

    if (ISEMPTY(match_n) && all_flag == FALSE)
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "Please specify filtering criteria or use \"all\" to show all keywords\n");
    }

    for (i_key = 0; sh_sand_keywords[i_key].keyword != NULL; i_key++)
    {
        if ((all_flag == TRUE) || (sal_strcasestr(sh_sand_keywords[i_key].keyword, match_n) != NULL))
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("%s", sh_sand_keywords[i_key].keyword);
            PRT_CELL_SET("%s", sh_sand_keywords[i_key].plural);
            PRT_CELL_SET("%s", sh_sand_keywords[i_key].short_key);
            PRT_CELL_SET("%d", sh_sand_keywords[i_key].count);
        }
    }

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

sh_sand_man_t sh_sand_shell_man = {
    "Shell management commands",
    NULL,
    NULL,
    NULL,
};

sh_sand_cmd_t sh_sand_shell_cmds[] = {
    {"resource", resource_cmd, NULL, resource_options, &resource_man, NULL, sh_sand_all_invokes}
    ,
    {NULL}
};
