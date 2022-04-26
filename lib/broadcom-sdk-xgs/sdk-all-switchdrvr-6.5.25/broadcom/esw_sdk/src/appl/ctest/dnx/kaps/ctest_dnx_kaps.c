/** \file ctest_dnx_kaps.c
 *
 * Ctests for KAPS
 *
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_KAPSDNX

/**
* INCLUDE FILES:
* {
*/
#include <shared/shrextend/shrextend_debug.h>

#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/ctest/dnxc/ctest_dnxc_system.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_kaps.h>

#include <sal/core/boot.h>

#include <shared/utilex/utilex_framework.h>

#include "ctest_dnx_kaps.h"
#include <soc/dnx/mdb_internal_shared.h>

#include "ctest_dnx_dataset_file_parser.h"
#include "verifier/kaps_verifier.h"

kaps_status
ctest_kaps_lpm_search(
    void *xpt,
    uint8_t * key,
    enum kaps_search_interface search_interface,
    struct kaps_search_result *kaps_result)
{
    return mdb_lpm_xpt_search(xpt, key, search_interface, kaps_result);
}

kaps_status
ctest_kaps_lpm_register_read(
    void *xpt,
    uint32_t offset,
    uint32_t nbytes,
    uint8_t * bytes)
{
    kaps_status rv = KAPS_OK;

    rv = mdb_lpm_xpt_register_read(xpt, offset, nbytes, bytes);

    ((CTEST_KAPS_MDB_XPT *) xpt)->num_reads++;

    return rv;
}

kaps_status
ctest_kaps_lpm_command(
    void *xpt,
    enum kaps_cmd cmd,
    enum kaps_func func,
    uint32_t blk_nr,
    uint32_t row_nr,
    uint32_t nbytes,
    uint8_t * bytes)
{
    kaps_status rv = KAPS_OK;
    CTEST_KAPS_MDB_XPT *ctest_mdb_xpt = xpt;
    int8_t write_to_device = 1;

    /*
     * Until the device is locked, we need to send commands to the device even in blackhole mode, since 1. We need to
     * read the device register to find out which KAPS device it is 2. We need to read the profile of the KAPS device
     * So only after device lock, we are blocking the XPT transactions in blackhole mode 
     */
    if (ctest_mdb_xpt->info->is_blackhole_mode && ctest_mdb_xpt->info->is_device_locked)
        write_to_device = 0;

    if (write_to_device)
    {
        rv = mdb_lpm_xpt_command(xpt, cmd, func, blk_nr, row_nr, nbytes, bytes);

        ctest_mdb_xpt->num_writes++;
    }

    return rv;
}

kaps_status
ctest_kaps_lpm_register_write(
    void *xpt,
    uint32_t offset,
    uint32_t nbytes,
    uint8_t * bytes)
{
    kaps_status rv = KAPS_OK;

    rv = mdb_lpm_xpt_register_write(xpt, offset, nbytes, bytes);

    ((CTEST_KAPS_MDB_XPT *) xpt)->num_writes++;

    return rv;

}

kaps_status
ctest_kaps_lpm_hb_read(
    void *xpt,
    uint32_t blk_nr,
    uint32_t row_nr,
    uint8_t * bytes)
{
    kaps_status rv = KAPS_OK;

    rv = mdb_lpm_xpt_hb_read(xpt, blk_nr, row_nr, bytes);

    ((CTEST_KAPS_MDB_XPT *) xpt)->num_reads++;

    return rv;
}

kaps_status
ctest_kaps_lpm_hb_write(
    void *xpt,
    uint32_t blk_nr,
    uint32_t row_nr,
    uint8_t * bytes)
{
    kaps_status rv = KAPS_OK;

    rv = mdb_lpm_xpt_hb_write(xpt, blk_nr, row_nr, bytes);

    ((CTEST_KAPS_MDB_XPT *) xpt)->num_writes++;

    return rv;
}

static shr_error_e
ctest_mdb_xpt_initialize(
    int unit,
    struct kaps_demo_info *info,
    void **xpt)
{
    CTEST_KAPS_MDB_XPT *xpt_p;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC(*xpt, sizeof(CTEST_KAPS_MDB_XPT), "kaps_xpt", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    if (*xpt == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, " Error:  SHR_ALLOC for xpt Failed\n");
    }

    xpt_p = (CTEST_KAPS_MDB_XPT *) * xpt;

    xpt_p->mdb_lpm_xpt.device_type = KAPS_DEVICE_KAPS;

    xpt_p->mdb_lpm_xpt.kaps_search = ctest_kaps_lpm_search;
    xpt_p->mdb_lpm_xpt.kaps_register_read = ctest_kaps_lpm_register_read;
    xpt_p->mdb_lpm_xpt.kaps_command = ctest_kaps_lpm_command;
    xpt_p->mdb_lpm_xpt.kaps_register_write = ctest_kaps_lpm_register_write;
    xpt_p->mdb_lpm_xpt.kaps_hb_read = ctest_kaps_lpm_hb_read;
    xpt_p->mdb_lpm_xpt.kaps_hb_write = ctest_kaps_lpm_hb_write;

    xpt_p->unit = unit;
    xpt_p->num_writes = 0;
    xpt_p->num_reads = 0;
    xpt_p->info = info;

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
ctest_mdb_xpt_destroy(
    int unit,
    void *xpt)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_FREE(xpt);

    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_kaps_verifier_add_dyn_tests(
    int unit,
    char *input_xml_file_list[],
    int num_xml_files,
    char *device_xml_file_name,
    int ctest_tag,
    rhlist_t * test_list)
{
    char cmd_line_params[MAX_KAPS_CTEST_STRING_LEN + 1];
    int iter;

    SHR_FUNC_INIT_VARS(unit);

    for (iter = 0; iter < num_xml_files; ++iter)
    {
        sal_strncpy(cmd_line_params, "", MAX_KAPS_CTEST_STRING_LEN);

        if (device_xml_file_name)
        {
            sal_strncat_s(cmd_line_params, "device_xml=", sizeof(cmd_line_params));
            sal_strncat_s(cmd_line_params, device_xml_file_name, sizeof(cmd_line_params));
        }

        sal_strncat_s(cmd_line_params, " input_xml=", sizeof(cmd_line_params));

        sal_strncat_s(cmd_line_params, input_xml_file_list[iter], sizeof(cmd_line_params));

        sal_strncat_s(cmd_line_params, ".xml ", sizeof(cmd_line_params));

        SHR_CLI_EXIT_IF_ERR(sh_sand_test_add(unit, test_list,
                                             input_xml_file_list[iter], cmd_line_params, ctest_tag), "Add test failed");
    }

exit:
    SHR_FUNC_EXIT;
}


/* *INDENT-OFF* */
/*The XML file indicates the width of the databases and random number generator used
 * for generating the prefixes
 * For example, in kaps_jr2_160_160-0113-7420.xml, the widths of the two databases are
 * 160b and 160b. The random number used for database-0 is 0113 and the random number
 * used for database-1 is 7420
 * */
static char *ctest_dnx_kaps_regular_ad_xml_nightly_list[] = {
    "kaps_160_160-0113-7420",
    "kaps_160_160-0330-7203",
    "kaps_160_160-2210-5323",
    "kaps_160_160-2330-5203",
    "kaps_160_160-5413-2120",
    "kaps_160_160-7222-0311",
    "kaps_160_160-0332-7201",
    "kaps_160_160-0412-7121",
    "kaps_160_160-0413-7120",
    "kaps_160_160-0420-7113",
    "kaps_160_160-0422-7111",
    "kaps_160_160-0423-7110",
    "kaps_160_160-0430-7103",
    "kaps_160_160-0432-7101",
    "kaps_160_160-0510-7023",
    "kaps_160_160-0520-7013",
    "kaps_160_160-0522-7011",
    "kaps_160_160-0530-7003",
    "kaps_160_160-0533-7000",
    "kaps_160_160-2013-5520",
    "kaps_160_160-2030-5503",
    "kaps_160_160-2033-5500",
    "kaps_160_160-2112-5421",
    "kaps_160_160-0023-7510",
    "kaps_160_160-0032-7501",
    "kaps_160_160-0110-7423",
    "kaps_160_160-0112-7421",
    "kaps_160_160-0122-7411",
    "kaps_160_160-0132-7401",
    "kaps_160_160-0213-7320",
    "kaps_160_160-0223-7310",
    "kaps_160_160-0230-7303",
    "kaps_160_160-0232-7301",
    "kaps_160_160-0233-7300",
    "kaps_160_160-0312-7221",
    "kaps_160_160-0313-7220",
    "kaps_160_160-0320-7213",
    "kaps_160_160-0322-7211",
    "kaps_160_160-0323-7210",
    "kaps_56-0133-7400",
    "kaps_56_144-0410-7123",
    "kaps_56_144-0512-7021",
    "kaps_56_144-2010-5523",
    "kaps_56_144-2433-5100",
    "kaps_56_144-5030-2503",
    "kaps_56_144-5113-2420",
    "kaps_56_144-7110-0423",
    "kaps_56_144-7333-0200",
    "kaps_56_144-0010-7523",
    "kaps_56_144-0012-7521",
    "kaps_56_144-0020-7513",
    "kaps_56_144-0033-7500",
    "kaps_56_144-0123-7410",
    "kaps_56_144-0130-7403",
    "kaps_56_144-0210-7323",
    "kaps_56_144-0220-7313",
    "kaps_56_144-0222-7311",
    "kaps_56_144-0310-7223",
    "kaps_56_144-0333-7200",
    "kaps_56_144-0433-7100",
    "kaps_56_144-0523-7010",
    "kaps_56_144-0532-7001",
    "kaps_56_144-2020-5513",
    "kaps_56_144-2022-5511",
    "kaps_56_144-2113-5420",
    "kaps_56-5322-2211"
};

/*The XML file indicates the width of the databases and random number generator used
 * for generating the prefixes
 * For example, in kaps_jr2_160_160-0113-7420.xml, the widths of the two databases are
 * 160b and 160b. The random number used for database-0 is 0113 and the random number
 * used for database-1 is 7420
 * */
static char *ctest_dnx_kaps_regular_ad_weekend_xml_list[] = {
    "kaps_160_160-0013-7520",
    "kaps_160_160-0022-7511",
    "kaps_160_160-2122-5411",
    "kaps_160_160-2123-5410",
    "kaps_160_160-2132-5401",
    "kaps_160_160-2220-5313",
    "kaps_160_160-2223-5310",
    "kaps_160_160-2233-5300",
    "kaps_160_160-2312-5221",
    "kaps_160_160-2313-5220",
    "kaps_160_160-2323-5210",
    "kaps_160_160-2333-5200",
    "kaps_160_160-2410-5123",
    "kaps_160_160-2412-5121",
    "kaps_160_160-2423-5110",
    "kaps_160_160-2523-5010",
    "kaps_160_160-5012-2521",
    "kaps_160_160-5020-2513",
    "kaps_160_160-5023-2510",
    "kaps_160_160-5033-2500",
    "kaps_160_160-5212-2321",
    "kaps_160_160-5223-2310",
    "kaps_160_160-5230-2303",
    "kaps_160_160-5232-2301",
    "kaps_160_160-5233-2300",
    "kaps_160_160-5313-2220",
    "kaps_160_160-5320-2213",
    "kaps_160_160-5323-2210",
    "kaps_160_160-5330-2203",
    "kaps_160_160-5333-2200",
    "kaps_160_160-5420-2113",
    "kaps_160_160-5423-2110",
    "kaps_160_160-5432-2101",
    "kaps_160_160-5510-2023",
    "kaps_160_160-5522-2011",
    "kaps_160_160-7012-0521",
    "kaps_160_160-7013-0520",
    "kaps_160_160-7020-0513",
    "kaps_160_160-7030-0503",
    "kaps_160_160-7112-0421",
    "kaps_160_160-7122-0411",
    "kaps_160_160-7130-0403",
    "kaps_160_160-7212-0321",
    "kaps_160_160-7213-0320",
    "kaps_160_160-7223-0310",
    "kaps_160_160-7230-0303",
    "kaps_160_160-7232-0301",
    "kaps_160_160-7233-0300",
    "kaps_160_160-7312-0221",
    "kaps_160_160-7313-0220",
    "kaps_160_160-7332-0201",
    "kaps_160_160-7410-0123",
    "kaps_160_160-7412-0121",
    "kaps_160_160-7422-0111",
    "kaps_160_160-7433-0100",
    "kaps_160_160-7512-0021",
    "kaps_160_160-7523-0010",
    "kaps_56-0030-7503",
    "kaps_56-0120-7413",
    "kaps_56-0212-7321",
    "kaps_56-0513-7020",
    "kaps_56_144-2120-5413",
    "kaps_56_144-2130-5403",
    "kaps_56_144-2133-5400",
    "kaps_56_144-2212-5321",
    "kaps_56_144-2213-5320",
    "kaps_56_144-2230-5303",
    "kaps_56_144-2232-5301",
    "kaps_56_144-2310-5223",
    "kaps_56_144-2320-5213",
    "kaps_56_144-2413-5120",
    "kaps_56_144-2420-5113",
    "kaps_56_144-2422-5111",
    "kaps_56_144-2430-5103",
    "kaps_56_144-2432-5101",
    "kaps_56_144-2513-5020",
    "kaps_56_144-2520-5013",
    "kaps_56_144-2522-5011",
    "kaps_56_144-2530-5003",
    "kaps_56_144-2532-5001",
    "kaps_56_144-2533-5000",
    "kaps_56_144-5010-2523",
    "kaps_56_144-5032-2501",
    "kaps_56_144-5110-2423",
    "kaps_56_144-5112-2421",
    "kaps_56_144-5120-2413",
    "kaps_56_144-5122-2411",
    "kaps_56_144-5123-2410",
    "kaps_56_144-5132-2401",
    "kaps_56_144-5133-2400",
    "kaps_56_144-5210-2323",
    "kaps_56_144-5213-2320",
    "kaps_56_144-5220-2313",
    "kaps_56_144-5222-2311",
    "kaps_56_144-5310-2223",
    "kaps_56_144-5332-2201",
    "kaps_56_144-5410-2123",
    "kaps_56_144-5412-2121",
    "kaps_56_144-5422-2111",
    "kaps_56_144-5430-2103",
    "kaps_56_144-5513-2020",
    "kaps_56_144-5520-2013",
    "kaps_56_144-5523-2010",
    "kaps_56_144-5530-2003",
    "kaps_56_144-5532-2001",
    "kaps_56_144-7010-0523",
    "kaps_56_144-7023-0510",
    "kaps_56_144-7033-0500",
    "kaps_56_144-7120-0413",
    "kaps_56_144-7123-0410",
    "kaps_56_144-7133-0400",
    "kaps_56_144-7210-0323",
    "kaps_56_144-7220-0313",
    "kaps_56_144-7320-0213",
    "kaps_56_144-7322-0211",
    "kaps_56_144-7413-0120",
    "kaps_56_144-7420-0113",
    "kaps_56_144-7423-0110",
    "kaps_56_144-7430-0103",
    "kaps_56_144-7432-0101",
    "kaps_56_144-7510-0023",
    "kaps_56_144-7520-0013",
    "kaps_56_144-7522-0011",
    "kaps_56_144-7533-0000",
    "kaps_56-2012-5521",
    "kaps_56-2023-5510",
    "kaps_56-2032-5501",
    "kaps_56-2110-5423",
    "kaps_56-2222-5311",
    "kaps_56-2322-5211",
    "kaps_56-2332-5201",
    "kaps_56-2510-5023",
    "kaps_56-2512-5021",
    "kaps_56-5013-2520",
    "kaps_56-5022-2511",
    "kaps_56-5130-2403",
    "kaps_56-5312-2221",
    "kaps_56-5433-2100",
    "kaps_56-5512-2021",
    "kaps_56-5533-2000",
    "kaps_56-7022-0511",
    "kaps_56-7032-0501",
    "kaps_56-7113-0420",
    "kaps_56-7132-0401",
    "kaps_56-7310-0223",
    "kaps_56-7323-0210",
    "kaps_56-7330-0203",
    "kaps_56-7513-0020",
    "kaps_56-7530-0003",
    "kaps_56-7532-0001"
};

/*Tests to be run to make sure capacity is fine*/
static char *ctest_dnx_kaps_verifier_capacity_xml_list[] = {
    "kaps_rrc00_2db", /*RRC00 IPv4, 2 databases*/
    "kaps_rrc01_2db", /*RRC01 IPv4, 2 databases*/
    "kaps_rrc02_2db", /*RRC02 IPv4, 2 databases*/
    "kaps_rrc03_2db", /*RRC03 IPv4, 2 databases*/
    "kaps_rrc04_2db", /*RRC04 IPv4, 2 databases*/
    "kaps_rrc05_2db", /*RRC05 IPv4, 2 databases*/
    "kaps_rrc06_2db", /*RRC06 IPv4, 2 databases*/
    "kaps_rrc07_2db", /*RRC07 IPv4, 2 databases*/
    "kaps_rrc08_2db", /*RRC08 IPv4, 2 databases*/
    "kaps_rrc09_2db", /*RRC09 IPv4, 2 databases*/
    "kaps_rrc10_2db", /*RRC10 IPv4, 2 databases*/
    "kaps_rrc11_2db", /*RRC11 IPv4, 2 databases*/
    "kaps_rrc12_2db", /*RRC12 IPv4, 2 databases*/
    "kaps_rrc13_2db", /*RRC13 IPv4, 2 databases*/
    "kaps_rrc14_2db", /*RRC14 IPv4, 2 databases*/
    "kaps_rrc15_2db", /*RRC15 IPv4, 2 databases*/
    "kaps_rrc16_2db", /*RRC16 IPv4, 2 databases*/
    "kaps_rrc18_2db", /*RRC18 IPv4, 2 databases*/
    "kaps_rrc19_2db", /*RRC19 IPv4, 2 databases*/
    "kaps_rrc20_2db", /*RRC20 IPv4, 2 databases*/
    "kaps_tokyo_2db", /*Tokyo pattern, 2 databases*/
    "kaps_ipv4_sequential_2db", /*Sequential IPv4, 2 databases, 64 VRF*/
    "kaps_ipv4_sequential_1_vrf_2db", /*Sequential IPv4, 2 databases, 1 VRF*/
    "kaps_ipv4_awc_2db", /*IPv4 Application Worst Case, 2 databases*/
    "kaps_ipv4_pwc_2db", /*IPv4 Pathological Worst Case, 2 databases*/
    "kaps_rrc00_ipv6_only_2db", /*RRC00 IPv6, 2 databases*/
    "kaps_rrc01_ipv6_only_2db", /*RRC01 IPv6, 2 databases*/
    "kaps_rrc03_ipv6_only_2db", /*RRC03 IPv6, 2 databases*/
    "kaps_rrc04_ipv6_only_2db", /*RRC04 IPv6, 2 databases*/
    "kaps_rrc05_ipv6_only_2db", /*RRC05 IPv6, 2 databases*/
    "kaps_rrc06_ipv6_only_2db", /*RRC06 IPv6, 2 databases*/
    "kaps_rrc07_ipv6_only_2db", /*RRC07 IPv6, 2 databases*/
    "kaps_rrc10_ipv6_only_2db", /*RRC10 IPv6, 2 databases*/
    "kaps_rrc11_ipv6_only_2db", /*RRC11 IPv6, 2 databases*/
    "kaps_rrc12_ipv6_only_2db", /*RRC12 IPv6, 2 databases*/
    "kaps_rrc13_ipv6_only_2db", /*RRC13 IPv6, 2 databases*/
    "kaps_rrc14_ipv6_only_2db", /*RRC14 IPv6, 2 databases*/
    "kaps_rrc15_ipv6_only_2db", /*RRC15 IPv6, 2 databases*/
    "kaps_rrc16_ipv6_only_2db", /*RRC16 IPv6, 2 databases*/
    "kaps_rrc18_ipv6_only_2db", /*RRC18 IPv6, 2 databases*/
    "kaps_rrc19_ipv6_only_2db", /*RRC19 IPv6, 2 databases*/
    "kaps_rrc20_ipv6_only_2db", /*RRC20 IPv6, 2 databases*/
    "kaps_rrc21_ipv6_only_2db", /*RRC21 IPv6, 2 databases*/
    "kaps_ipv6_sequential_2db", /*Sequential IPv6, 2 databases*/
    "kaps_ipv6_bgptable_64_vrf_2db", /*Potaroo IPv6, 2 databases, 64 VRF*/
    "kaps_ipv6_awc_2db", /*Application Worst Case IPv6, 2 databases */
    "kaps_ipv6_pwc_hosts_2db" /*Pathological Worst Case IPv6, 2 databases*/
};



/* *INDENT-ON* */

static shr_error_e
dnx_kaps_verifier_add_regular_ad_dyn_tests(
    int unit,
    rhlist_t * test_list)
{
    int num_xml_files;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Add the tests to be run every night
     */
    num_xml_files = sizeof(ctest_dnx_kaps_regular_ad_xml_nightly_list) / sizeof(char *);

    SHR_IF_ERR_EXIT(dnx_kaps_verifier_add_dyn_tests(unit, ctest_dnx_kaps_regular_ad_xml_nightly_list,
                                                    num_xml_files, "KAPS_JR2_Big_Profile_3.xml", CTEST_POSTCOMMIT,
                                                    test_list));
    /*
     * Add the tests to be run every weekend
     */
    num_xml_files = sizeof(ctest_dnx_kaps_regular_ad_weekend_xml_list) / sizeof(char *);

    SHR_IF_ERR_EXIT(dnx_kaps_verifier_add_dyn_tests(unit, ctest_dnx_kaps_regular_ad_weekend_xml_list,
                                                    num_xml_files, "KAPS_JR2_Big_Profile_3.xml", CTEST_WEEKEND,
                                                    test_list));

exit:
    SHR_FUNC_EXIT;

}

static shr_error_e
dnx_kaps_verifier_add_capacity_dyn_tests(
    int unit,
    rhlist_t * test_list)
{
    int num_xml_files = sizeof(ctest_dnx_kaps_verifier_capacity_xml_list) / sizeof(char *);

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_kaps_verifier_add_dyn_tests(unit, ctest_dnx_kaps_verifier_capacity_xml_list,
                                                    num_xml_files, "KAPS_JR2_Big_Profile_3.xml", CTEST_POSTCOMMIT,
                                                    test_list));

exit:
    SHR_FUNC_EXIT;
}


/*
 * structs to support bcm shell command
 */
/* *INDENT-OFF* */
sh_sand_option_t dnx_kaps_default_test_options[] = {
    {"num_entries", SAL_FIELD_TYPE_UINT32, "Indicates Number of entries to be inserted", "10000"},
    {"hb", SAL_FIELD_TYPE_UINT32, "Indicates if hit bits are present 0/1", "1"},
    {"warmboot", SAL_FIELD_TYPE_UINT32, "Indicates if warmboot should be performed 0/1", "1"},
    {"search", SAL_FIELD_TYPE_UINT32, "Indicates if search should be performed 0/1", "1"},
    {"blackhole", SAL_FIELD_TYPE_UINT32, "Indicates if blackhole mode should be used 0/1", "0"},
    {"pfx_field_len", SAL_FIELD_TYPE_UINT32, "Indicates the length of prefix field in bits", "32"},
    {"pattern", SAL_FIELD_TYPE_UINT32, "Indicates the type of prefix pattern to be generated: 0 - Sequential, 1 - Random, 2 - File", "0"},
    {"filename", SAL_FIELD_TYPE_STR, "Indicates the file that contains the prefixes", ""},
    {"sw_model", SAL_FIELD_TYPE_UINT32, "Indicates if we should connect to C-Model ", "0" },
    {"sw_model_dev_id", SAL_FIELD_TYPE_UINT32, "Indicates the device type of the KAPS C-Model", "5"},
    {"sw_model_sub_type", SAL_FIELD_TYPE_UINT32, "Indicates the sub type of the KAPS C-Model", "1"},
    {"sw_model_profile", SAL_FIELD_TYPE_UINT32, "Indicates the profile for the KAPS C-Model", "0"},
    {NULL}
};

static sh_sand_enum_t kaps_verifier_coreloop_type_enum_table[] = {
    {"SEQUENTIAL", KAPS_VERIFIER_CORELOOP_TYPE_SEQUENTIAL, "Sequential Coreloop"},
    {"RANDOM", KAPS_VERIFIER_CORELOOP_TYPE_RANDOM, "Random Coreloop"},
    {"ADDREMOVE", KAPS_VERIFIER_CORELOOP_TYPE_ADDREMOVE, "AddRemove Coreloop"},
    {"USER", KAPS_VERIFIER_CORELOOP_TYPE_USER, "User Coreloop"},
    {NULL}
};

static sh_sand_enum_t kaps_verifier_warmboot_type_enum_table[] = {
    {"NO", KAPS_VERIFIER_WARMBOOT_TYPE_NO_WARMBOOT, "No warmboot"},
    {"INFREQ", KAPS_VERIFIER_WARMBOOT_TYPE_INFREQ, "Infrequent warmboot"},
    {"FREQ", KAPS_VERIFIER_WARMBOOT_TYPE_FREQ, "Frequent Warmboot"},
    {NULL}
};

static sh_sand_enum_t kaps_verifier_xpt_type_enum_table[] = {
    {"BLACKHOLE_XPT", KAPS_VERIFIER_BLACKHOLE_XPT, "Blackhole XPT"},
    {"CMODEL_XPT", KAPS_VERIFIER_KAPS_MODEL_XPT, "KAPS Model XPT"},
    {"MDB_XPT", KAPS_VERIFIER_MDB_XPT, "MDB XPT"},
    {NULL}
};

sh_sand_option_t dnx_kaps_xml_tests_options[] = {
    {"device_xml", SAL_FIELD_TYPE_STR, "device config XML file to be used", ""},
    {"input_xml", SAL_FIELD_TYPE_STR, "input XML file to be used", ""},
    {"coreloop_type", SAL_FIELD_TYPE_ENUM, "coreloop type", "RANDOM", (void *) kaps_verifier_coreloop_type_enum_table},
    {"warmboot_type", SAL_FIELD_TYPE_ENUM, "warmboot type", "FREQ", (void *) kaps_verifier_warmboot_type_enum_table},
    {"xpt_type", SAL_FIELD_TYPE_ENUM, "XPT type", "MDB_XPT", (void *) kaps_verifier_xpt_type_enum_table},
    {"iterations", SAL_FIELD_TYPE_UINT32, "Indicates number of iterations to do", "2000000"},
    {"sw_model_dev_id", SAL_FIELD_TYPE_UINT32, "Indicates the device type of the KAPS C-Model", "5"},
    {"sw_model_sub_type", SAL_FIELD_TYPE_UINT32, "Indicates the sub type of the KAPS C-Model", "1"},
    {"sw_model_profile", SAL_FIELD_TYPE_UINT32, "Indicates the profile for the KAPS C-Model", "0"},
    {"vector_file", SAL_FIELD_TYPE_STR, "Indicates the file name in which the vectors should be stored", ""},
    {NULL}
};

sh_sand_option_t dnx_kaps_xml_tests_capacity_options[] = {
    {"device_xml", SAL_FIELD_TYPE_STR, "device config XML file to be used", ""},
    {"input_xml", SAL_FIELD_TYPE_STR, "input XML file to be used", ""},
    {"coreloop_type", SAL_FIELD_TYPE_ENUM, "coreloop type", "SEQUENTIAL", (void *) kaps_verifier_coreloop_type_enum_table},
    {"warmboot_type", SAL_FIELD_TYPE_ENUM, "warmboot type", "NO", (void *) kaps_verifier_warmboot_type_enum_table},
    {"xpt_type", SAL_FIELD_TYPE_ENUM, "XPT type", "MDB_XPT", (void *) kaps_verifier_xpt_type_enum_table},
    {"iterations", SAL_FIELD_TYPE_UINT32, "Indicates number of iterations to do", "2000000"},
    {"sw_model_dev_id", SAL_FIELD_TYPE_UINT32, "Indicates the device type of the KAPS C-Model", "5"},
    {"sw_model_sub_type", SAL_FIELD_TYPE_UINT32, "Indicates the sub type of the KAPS C-Model", "1"},
    {"sw_model_profile", SAL_FIELD_TYPE_UINT32, "Indicates the profile for the KAPS C-Model", "0"},
    {"vector_file", SAL_FIELD_TYPE_STR, "Indicates the file name in which the vectors should be stored", ""},
    {NULL}
};







sh_sand_man_t dnx_kaps_default_test_man = {
    "Default Test for KAPS",
    "The test adds entries to JR2 KAPS device and performs searches using adapter"
};

sh_sand_man_t dnx_kaps_xml_verifier_man = {
    "main menu for KAPS XML verifier",
    "All the tests under this adds/deletes entries in different order based on the input parameters to JR2 KAPS device and performs searches using adapter"
};


/* *INDENT-ON* */

shr_error_e dnx_kaps_default_test(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

shr_error_e dnx_kaps_verifier(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control);

static shr_error_e
allocate_memory_for_entries(
    int unit,
    struct kaps_demo_info *info,
    int num_entries_to_add)
{
    struct kaps_allocator *alloc_p = info->ctest_alloc;

    SHR_FUNC_INIT_VARS(unit);

    info->lpm_records_p = alloc_p->xcalloc(alloc_p->cookie, 1, (num_entries_to_add * sizeof(struct lpm_record)));
    if (info->lpm_records_p == NULL)
        return _SHR_E_MEMORY;

    SHR_FUNC_EXIT;

}

static shr_error_e
deallocate_memory_for_entries(
    int unit,
    struct kaps_demo_info *info)
{
    struct kaps_allocator *alloc_p = info->ctest_alloc;

    SHR_FUNC_INIT_VARS(unit);

    if (info->lpm_records_p)
        alloc_p->xfree(alloc_p->cookie, info->lpm_records_p);

    SHR_FUNC_EXIT;
}

/* Generate the records [entries/prefixes] for the LPM databases */
static shr_error_e
database_generate_records(
    int unit,
    struct kaps_demo_info *info,
    enum pattern_type pattern,
    char *filename,
    int num_entries_to_add)
{
    uint32 iter, byte_nr, num_bytes;
    uint32 pfx_gen_seq_num = 0x30000000;
    struct ctest_kaps_parse_record *file_entries = NULL;
    uint32 num_lines_in_file = 0, num_entries_parsed_from_file = 0;
    struct ctest_kaps_parse_key *parse_key = NULL;

    SHR_FUNC_INIT_VARS(unit);

    if (pattern == FILE_PATTERN)
    {
        DNX_KAPS_TRY(ctest_dnx_parse_standard_dataset(unit, KAPS_DB_LPM, filename,
                                                      &file_entries, 1000000, &num_lines_in_file,
                                                      &num_entries_parsed_from_file, &parse_key));

        if (num_entries_parsed_from_file < num_entries_to_add)
            num_entries_to_add = num_entries_parsed_from_file;
    }

    for (iter = 0; iter < num_entries_to_add; iter++)
    {
        /*
         * All entries have VPN-ID = 1
         */
        info->lpm_records_p[iter].pfx_data[0] = 0;
        info->lpm_records_p[iter].pfx_data[1] = 1;

        if (pattern == SEQUENTIAL_PATTERN)
        {
            int index = info->total_db_width_in_bytes - 4;

            info->lpm_records_p[iter].pfx_data[index] = (pfx_gen_seq_num >> 24) & 0xFF;
            info->lpm_records_p[iter].pfx_data[index + 1] = (pfx_gen_seq_num >> 16) & 0xFF;
            info->lpm_records_p[iter].pfx_data[index + 2] = (pfx_gen_seq_num >> 8) & 0xFF;
            info->lpm_records_p[iter].pfx_data[index + 3] = (pfx_gen_seq_num) & 0xFF;

            info->lpm_records_p[iter].length = info->total_db_width_in_bits;

            pfx_gen_seq_num++;
        }
        else if (pattern == RANDOM_PATTERN)
        {
            for (byte_nr = 2; byte_nr < info->total_db_width_in_bytes; byte_nr++)
            {
                info->lpm_records_p[iter].pfx_data[byte_nr] = rand() % 256;
            }

            info->lpm_records_p[iter].length = info->total_db_width_in_bits;
        }
        else if (pattern == FILE_PATTERN)
        {
            num_bytes = (file_entries[iter].length + 7) / 8;
            kaps_memcpy(info->lpm_records_p[iter].pfx_data, file_entries[iter].data, num_bytes);
            info->lpm_records_p[iter].length = file_entries[iter].length;
        }
    }

    if (pattern == FILE_PATTERN && file_entries)
    {
        kaps_sysfree(file_entries);
    }

exit:
    SHR_FUNC_EXIT;
}

/* Insert/add the records [entries/prefixes] to the LPM databases */
static shr_error_e
database_populate_records(
    int unit,
    struct kaps_demo_info *info,
    uint32 num_entries_to_add)
{
    int32 iter, i;
    struct kaps_entry *dummy_entry;
    struct kaps_ad *ad_handle = NULL;
    struct kaps_hb *hb_handle = NULL;
    kaps_status status = KAPS_OK;

    SHR_FUNC_INIT_VARS(unit);

    for (iter = 0; iter < num_entries_to_add; iter++)
    {
        uint8_t ad_value[KAPS_DEMO_AD_WIDTH_8] = { 0 };

        /*
         * mask has value zero, all bits in the key will be matched
         */
        status = kaps_db_add_prefix
            (info->lpm_db_p, info->lpm_records_p[iter].pfx_data, info->lpm_records_p[iter].length, &dummy_entry);

        if (status != KAPS_OK)
        {
            if (status == KAPS_DUPLICATE)
            {
                continue;
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_FAIL, "kaps_db_add_prefix failed, status = %d \n", status);
            }
        }

        for (i = 0; i < KAPS_DEMO_AD_WIDTH_8; ++i)
        {
            ad_value[i] = rand() % 256;
        }
        ad_value[KAPS_DEMO_AD_WIDTH_8 - 1] &= 0XF0;     /* Only first four bits in the last byte are valid for the AD */

        DNX_KAPS_TRY(kaps_ad_db_add_entry(info->lpm_ad_db_p, ad_value, &ad_handle));

        DNX_KAPS_TRY(kaps_entry_add_ad(info->lpm_db_p, dummy_entry, ad_handle));

        if (info->lpm_hb_db_p)
        {
            DNX_KAPS_TRY(kaps_hb_db_add_entry(info->lpm_hb_db_p, &hb_handle));

            DNX_KAPS_TRY(kaps_entry_add_hb(info->lpm_db_p, dummy_entry, hb_handle));
        }

        /*
         * add the entry to the database
         */
        status = kaps_db_install(info->lpm_db_p);
        if (status != KAPS_OK)
        {
            if (status == KAPS_OUT_OF_UDA || status == KAPS_OUT_OF_DBA
                || status == KAPS_EXHAUSTED_PCM_RESOURCE || status == KAPS_OUT_OF_UIT)
            {
                DNX_KAPS_TRY(kaps_db_delete_entry(info->lpm_db_p, dummy_entry));

                LOG_CLI((BSL_META("Table is full \n")));
                break;
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_FAIL, "kaps_db_install failed, status = %d \n", status);
            }
        }

        info->num_entries_inserted++;

        if ((iter % LPM_PRINT_COUNT) == 0 && iter)
            LOG_CLI((BSL_META("\t  -> Inserted %d prefixes to LPM database #0 \n"), iter));

    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
verify_search_results(
    int unit,
    uint32 res_port,
    struct kaps_entry *entry,
    struct kaps_complete_search_result *cmp_rslt,
    uint8_t * ad_value)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * compare the two search results
     */
    if (kaps_memcmp(ad_value, cmp_rslt->assoc_data[res_port], KAPS_DEMO_AD_WIDTH_8) != 0)
    {
        uint32 iter;

        LOG_CLI((BSL_META("\n\n\t- Error !! Mismatch between AD returned in device and software for the search\n\n")));

        LOG_CLI((BSL_META("Expected: ")));
        for (iter = 0; iter < KAPS_DEMO_AD_WIDTH_8; ++iter)
        {
            LOG_CLI((BSL_META("%02x "), ad_value[iter]));
        }
        LOG_CLI((BSL_META("\n")));

        LOG_CLI((BSL_META("Got:")));
        for (iter = 0; iter < KAPS_DEMO_AD_WIDTH_8; ++iter)
        {
            LOG_CLI((BSL_META("%02x "), cmp_rslt->assoc_data[res_port][iter]));
        }
        LOG_CLI((BSL_META("\n")));

        SHR_ERR_EXIT(_SHR_E_FAIL, "Search failed \n");
    }

exit:
    SHR_FUNC_EXIT;
}

/* Search the records [entries/prefixes] which are added toLPM databases */
static shr_error_e
database_perform_search(
    int unit,
    struct kaps_demo_info *info)
{
    uint32 iter;
    uint8_t ad_value[KAPS_DEMO_AD_WIDTH_8] = { 0 };

    uint8 master_key[MAX_KAPS_KEY_LEN_IN_BITS];
    uint8 search_key[KAPS_DEMO_SEARCH_INTERFACE_WIDTH_8];
    int32 index0 = 0, prio = 0;
    struct kaps_entry *lpm_entry = NULL;
    struct kaps_ad *lpm_ad_handle = NULL;
    struct kaps_complete_search_result cmp_rslt;

    SHR_FUNC_INIT_VARS(unit);

    LOG_CLI((BSL_META("- Searching all the prefixes \n")));

    for (iter = 0; iter < info->num_entries_inserted; iter++)
    {
        kaps_memset(&cmp_rslt, 0, sizeof(struct kaps_complete_search_result));
        kaps_memset(master_key, 0, sizeof(master_key));
        kaps_memset(search_key, 0, sizeof(search_key));
        kaps_memset(ad_value, 0, sizeof(ad_value));

        /*
         * generate the master key for result port 0 [159:0]
         */
        kaps_memcpy(&master_key[0 * KAPS_DEMO_SEARCH_INTERFACE_WIDTH_8], info->lpm_records_p[iter].pfx_data, 20);

        /*
         * generate the search key for s/w search [159:0]
         */
        kaps_memcpy(search_key, info->lpm_records_p[iter].pfx_data, info->total_db_width_in_bytes);

        DNX_KAPS_TRY(kaps_db_search(info->lpm_db_p, search_key, &lpm_entry, &index0, &prio));

        if (lpm_entry)
        {
            DNX_KAPS_TRY(kaps_entry_get_ad(info->lpm_db_p, lpm_entry, &lpm_ad_handle));

            DNX_KAPS_TRY(kaps_ad_db_get(info->lpm_ad_db_p, lpm_ad_handle, ad_value));
        }

        /*
         * search in the device
         */
        DNX_KAPS_TRY(kaps_instruction_search(info->lpm_inst_p, master_key, 0, &cmp_rslt));

        SHR_IF_ERR_EXIT(verify_search_results(unit, 0, lpm_entry, &cmp_rslt, ad_value));

        if ((iter % LPM_PRINT_COUNT) == 0 && iter)
            LOG_CLI((BSL_META("\t  -> Searched %d prefixes from LPM database #0 \n"), iter));
    }

    LOG_CLI((BSL_META("- Completed searching \n\n")));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
database_create(
    int unit,
    struct kaps_demo_info *info,
    int is_hb_present)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Create the LPM database
     */
    DNX_KAPS_TRY(kaps_db_init(info->device_p, KAPS_DB_LPM, 0, LPM_CAPACITY, &info->lpm_db_p));

    DNX_KAPS_TRY(kaps_ad_db_init(info->device_p, 0, LPM_CAPACITY, 20, &info->lpm_ad_db_p));

    DNX_KAPS_TRY(kaps_db_set_ad(info->lpm_db_p, info->lpm_ad_db_p));

    if (is_hb_present)
    {
        DNX_KAPS_TRY(kaps_hb_db_init(info->device_p, 0, LPM_CAPACITY, &info->lpm_hb_db_p));

        DNX_KAPS_TRY(kaps_db_set_hb(info->lpm_db_p, info->lpm_hb_db_p));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
database_create_key_and_inst(
    int unit,
    struct kaps_demo_info *info)
{
    struct kaps_key *lpm_key;
    struct kaps_key *lpm_mkey;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Create the LPM key
     */
    DNX_KAPS_TRY(kaps_key_init(info->device_p, &lpm_key));

    /*
     * Key format: 48b -> VRF_id(16b) + sip(32b)
     */
    if (info->vrf_field_len_in_bits)
    {
        DNX_KAPS_TRY(kaps_key_add_field(lpm_key, "VRF_ID", info->vrf_field_len_in_bits, KAPS_KEY_FIELD_EM));
    }

    DNX_KAPS_TRY(kaps_key_add_field(lpm_key, "PREFIX", info->prefix_field_len_in_bits, KAPS_KEY_FIELD_PREFIX));

    /*
     * Add the key to the database
     */
    DNX_KAPS_TRY(kaps_db_set_key(info->lpm_db_p, lpm_key));

    /*
     * Create LPM DB instruction
     */
    DNX_KAPS_TRY(kaps_instruction_init(info->device_p, 0, 0, &info->lpm_inst_p));

    /*
     * Add the master key details
     */
    DNX_KAPS_TRY(kaps_key_init(info->device_p, &lpm_mkey));

    /*
     * Master Key
     */
    if (info->vrf_field_len_in_bits)
    {
        DNX_KAPS_TRY(kaps_key_add_field(lpm_mkey, "VRF_ID", info->vrf_field_len_in_bits, KAPS_KEY_FIELD_EM));
    }

    DNX_KAPS_TRY(kaps_key_add_field(lpm_mkey, "PREFIX", info->prefix_field_len_in_bits, KAPS_KEY_FIELD_PREFIX));

    DNX_KAPS_TRY(kaps_key_add_field(lpm_mkey, "ZERO_FILL", 160 - info->total_db_width_in_bits, KAPS_KEY_FIELD_EM));

    DNX_KAPS_TRY(kaps_instruction_set_key(info->lpm_inst_p, lpm_mkey));

    /*
     * Add the keys to the LPM inst, Set the KPU and Result Port for the LPM database
     */
    DNX_KAPS_TRY(kaps_instruction_add_db(info->lpm_inst_p, info->lpm_db_p, 0));

    DNX_KAPS_TRY(kaps_instruction_install(info->lpm_inst_p));

exit:
    SHR_FUNC_EXIT;
}

/* Warmboot callback functions for write */
static int32_t
wb_nv_write(
    void *handle,
    uint8_t * buffer,
    uint32_t size,
    uint32_t offset)
{
    uint8_t *tmp;
    struct kaps_demo_info *info = (struct kaps_demo_info *) handle;
    struct kaps_allocator *alloc = info->ctest_alloc;

    if (info->wb_memory)
    {
        if ((offset + size) > info->wb_size)
        {
            /*
             * Realloc NV memory
             */
            tmp = (uint8_t *) alloc->xcalloc(alloc->cookie, info->wb_size * 2, sizeof(uint8_t));
            if (!tmp)
            {
                LOG_CLI((BSL_META("NV memory alloc failed\n")));
                kaps_sassert(0);
            }

            kaps_memcpy(tmp, info->wb_memory, info->wb_size);

            alloc->xfree(alloc->cookie, info->wb_memory);

            info->wb_memory = tmp;
            info->wb_size *= 2;
        }

        kaps_memcpy(&info->wb_memory[offset], buffer, size);
    }

    return 0;
}

/* Warmboot callback functions for read */
static int32_t
wb_nv_read(
    void *handle,
    uint8_t * buffer,
    uint32_t size,
    uint32_t offset)
{
    struct kaps_demo_info *info = (struct kaps_demo_info *) handle;

    if (info->wb_memory)
    {
        if ((offset + size) > info->wb_size)
            return -1;

        kaps_memcpy(buffer, &info->wb_memory[offset], size);
    }

    return 0;
}

static shr_error_e
perform_warmboot(
    int unit,
    struct kaps_demo_info *info)
{
    struct kaps_allocator *alloc = info->ctest_alloc;

    SHR_FUNC_INIT_VARS(unit);

    LOG_CLI((BSL_META("- Performing warmboot \n")));

    info->wb_size = 80 * 1024 * 1024;
    info->wb_memory = alloc->xcalloc(alloc->cookie, info->wb_size, sizeof(uint8_t));

    if (!info->wb_memory)
    {
        info->wb_size = 0;
        SHR_ERR_EXIT(_SHR_E_MEMORY, " Error:  Warmboot memory allocation failed\n");
    }

    /*
     * Save the software state in warmboot memory
     */
    DNX_KAPS_TRY(kaps_device_save_state(info->device_p, wb_nv_read, wb_nv_write, (void *) info));

    /*
     * Destroy the KAPS device
     */
    DNX_KAPS_TRY(kaps_device_destroy(info->device_p));

    /*
     * Re-initialize the KAPS device.
     */
    DNX_KAPS_TRY(kaps_device_init(info->kaps_alloc, KAPS_DEVICE_KAPS,
                                  KAPS_DEVICE_SKIP_INIT, info->xpt_p, NULL, &info->device_p));

    /*
     * Restore the software state from the warmboot memory
     */
    DNX_KAPS_TRY(kaps_device_restore_state(info->device_p, wb_nv_read, wb_nv_write, (void *) info));

    /*
     * Refresh the AD DB pointers
     */
    if (info->lpm_ad_db_p)
    {
        DNX_KAPS_TRY(kaps_ad_db_refresh_handle(info->device_p, info->lpm_ad_db_p, &info->lpm_ad_db_p));
    }

    /*
     * Refresh the HB DB pointers
     */
    if (info->lpm_hb_db_p)
    {
        DNX_KAPS_TRY(kaps_hb_db_refresh_handle(info->device_p, info->lpm_hb_db_p, &info->lpm_hb_db_p));
    }

    /*
     * Refresh the LPM DB pointers
     */
    if (info->lpm_db_p)
    {
        DNX_KAPS_TRY(kaps_db_refresh_handle(info->device_p, info->lpm_db_p, &info->lpm_db_p));
    }

    /*
     * Refresh the instruction handles
     */
    if (info->lpm_inst_p)
    {
        DNX_KAPS_TRY(kaps_instruction_refresh_handle(info->device_p, info->lpm_inst_p, &info->lpm_inst_p));
    }

    /*
     * Lock the KAPS device
     */
    DNX_KAPS_TRY(kaps_device_lock(info->device_p));

    /*
     * Release the warm boot memory
     */
    if (info->wb_memory)
    {
        alloc->xfree(info->ctest_alloc->cookie, info->wb_memory);
    }

    LOG_CLI((BSL_META("- Completed warmboot \n\n")));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
kaps_workflow_fn(
    int unit,
    sh_sand_control_t * sand_control)
{
    struct kaps_demo_info info = { 0 };
    struct kaps_default_allocator_stats alloc_stats;

    enum pattern_type pattern = SEQUENTIAL_PATTERN;
    char *filename = "rrc00_ipv4_only.txt";
    uint32 num_entries_to_add = 10000;
    uint32 is_hb_present = 1;
    uint32 do_warmboot = 1;
    uint32 do_search = 1;
    uint32 vrf_field_len_in_bits = 16;
    uint32 prefix_field_len_in_bits = 32;
    uint32 num_reads = 0, num_writes = 0;
    char *timer_name = NULL;
    int is_active = 0;
    uint32 nof_hits = 0, total_time = 0;

    uint32 kaps_timers_group;

    PRT_INIT_VARS;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32("num_entries", num_entries_to_add);
    SH_SAND_GET_UINT32("hb", is_hb_present);
    SH_SAND_GET_UINT32("warmboot", do_warmboot);
    SH_SAND_GET_UINT32("search", do_search);
    SH_SAND_GET_UINT32("blackhole", info.is_blackhole_mode);
    SH_SAND_GET_UINT32("pfx_field_len", prefix_field_len_in_bits);
    SH_SAND_GET_UINT32("pattern", pattern);
    SH_SAND_GET_STR("filename", filename);
    SH_SAND_GET_UINT32("sw_model", info.connect_to_sw_model);
    SH_SAND_GET_UINT32("sw_model_dev_id", info.sw_model_dev_id);
    SH_SAND_GET_UINT32("sw_model_sub_type", info.sw_model_sub_type);
    SH_SAND_GET_UINT32("sw_model_profile", info.sw_model_profile);

    LOG_CLI((BSL_META("Number of entries to insert = %d \n"), num_entries_to_add));

    if (pattern == SEQUENTIAL_PATTERN)
    {
        LOG_CLI((BSL_META("Generating sequential prefixes \n")));
    }
    else if (pattern == RANDOM_PATTERN)
    {
        LOG_CLI((BSL_META("Generating random prefixes \n")));
    }
    else if (pattern == FILE_PATTERN)
    {
        LOG_CLI((BSL_META("Generating prefixes from file = %s \n"), filename));
    }

    if (is_hb_present)
        LOG_CLI((BSL_META("Hit Bits are present \n")));

    if (info.is_blackhole_mode)
        LOG_CLI((BSL_META("Running in Blackhole mode \n")));

    if (info.connect_to_sw_model)
        LOG_CLI((BSL_META("Connecting to KAPS Model \n")));

    info.connect_to_sw_model = 0;

    /*
     * Initialize the timer for measuring the update rate
     */
    utilex_ll_timer_group_allocate("kaps_lpm measurements", &kaps_timers_group);
    utilex_ll_timer_clear_all(kaps_timers_group);

    /*
     * Create the default allocator
     */
    DNX_KAPS_TRY(kaps_default_allocator_create(&info.kaps_alloc));

    DNX_KAPS_TRY(kaps_default_allocator_create(&info.ctest_alloc));

    if (info.is_blackhole_mode)
        do_search = 0;

    if (SAL_BOOT_PLISIM && info.connect_to_sw_model == 0)
        do_search = 0;

    /*
     * Initialize the XPT
     */
    if (info.connect_to_sw_model)
    {
    }
    else
    {
        SHR_IF_ERR_EXIT(ctest_mdb_xpt_initialize(unit, &info, &info.xpt_p));
    }

    info.vrf_field_len_in_bits = vrf_field_len_in_bits;
    info.prefix_field_len_in_bits = prefix_field_len_in_bits;
    info.total_db_width_in_bits = vrf_field_len_in_bits + prefix_field_len_in_bits;
    info.total_db_width_in_bytes = (info.total_db_width_in_bits + 7) / 8;

    /*
     * Initialize the device
     */
    DNX_KAPS_TRY(kaps_device_init
                 (info.kaps_alloc, KAPS_DEVICE_KAPS, KAPS_DEVICE_DEFAULT, info.xpt_p, NULL, &info.device_p));

    /*
     * Allocate memory for storing the entries for LPM databases
     */
    SHR_IF_ERR_EXIT(allocate_memory_for_entries(unit, &info, num_entries_to_add));

    /*
     * Create the LPM databases as required
     */
    SHR_IF_ERR_EXIT(database_create(unit, &info, is_hb_present));

    SHR_IF_ERR_EXIT(database_create_key_and_inst(unit, &info));

    DNX_KAPS_TRY(kaps_device_lock(info.device_p));

    info.is_device_locked = 1;

    /*
     * Generate records for all the databases
     */
    SHR_IF_ERR_EXIT(database_generate_records(unit, &info, pattern, filename, num_entries_to_add));

    /*
     * Insert the record to the databases
     */
    LOG_CLI((BSL_META("\n- Adding entries to the database \n")));
    utilex_ll_timer_set("kaps_entry_add", kaps_timers_group, 0);
    SHR_IF_ERR_EXIT(database_populate_records(unit, &info, num_entries_to_add));
    utilex_ll_timer_stop(kaps_timers_group, 0);

    LOG_CLI((BSL_META("    - Number of prefixes added to database = %d \n\n"), info.num_entries_inserted));
    /*
     * Perform all the warmboot related operations
     */
    if (do_warmboot)
    {
        SHR_IF_ERR_EXIT(perform_warmboot(unit, &info));
    }

    /*
     * Search the inserted prefixes in database
     */
    if (do_search)
    {
        SHR_IF_ERR_EXIT(database_perform_search(unit, &info));
    }

    kaps_default_allocator_get_stats(info.kaps_alloc, &alloc_stats);

    if (info.connect_to_sw_model)
    {
        /*
         * Do nothing for now with SW Model since we don't have additional stats
         */
    }
    else
    {
        num_writes = ((CTEST_KAPS_MDB_XPT *) info.xpt_p)->num_writes;
        num_reads = ((CTEST_KAPS_MDB_XPT *) info.xpt_p)->num_reads;
    }

    /*
     * Free memory for stored entries for LPM or both databases
     */
    SHR_IF_ERR_EXIT(deallocate_memory_for_entries(unit, &info));

    /*
     * Destroy the device handle
     */
    LOG_CLI((BSL_META("\n- Destroying the device \n")));
    DNX_KAPS_TRY(kaps_device_destroy(info.device_p));

    /*
     * Print the time taken to insert the entries
     */
    utilex_ll_timer_print_all(kaps_timers_group);

    utilex_ll_timer_get_info(kaps_timers_group, 0, &is_active, &timer_name, &nof_hits, &total_time);

    /*
     * Clear the timers and free them
     */
    utilex_ll_timer_clear_all(kaps_timers_group);
    utilex_ll_timer_group_free(kaps_timers_group);

    PRT_TITLE_SET("KAPS Stats");

    PRT_COLUMN_ADD("Stat Name");
    PRT_COLUMN_ADD("Value");
    PRT_ROW_ADD(PRT_ROW_SEP_NONE);

    PRT_CELL_SET("Update Rate");
    PRT_CELL_SET("%0.2f K prefixes/s", ((float) info.num_entries_inserted / total_time) * 1000);
    PRT_ROW_ADD(PRT_ROW_SEP_NONE);

    PRT_CELL_SET("Heap Peak Usage");
    PRT_CELL_SET("%0.2fMB", ((float) alloc_stats.peak_bytes) / (1024.0 * 1024.0));
    PRT_ROW_ADD(PRT_ROW_SEP_NONE);

    if (info.connect_to_sw_model)
    {
        /*
         * Do nothing for now with SW Model since we don't have additional stats
         */
    }
    else
    {
        PRT_CELL_SET("Total number of writes ");
        PRT_CELL_SET("%d", num_writes);
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);

        PRT_CELL_SET("Total number of reads ");
        PRT_CELL_SET("%d", num_reads);
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);

    }

    PRT_COMMITX;

    /*
     * Destroy the C-Model
     */
    if (info.connect_to_sw_model)
    {
    }
    else
    {
        ctest_mdb_xpt_destroy(unit, info.xpt_p);
    }

    /*
     * Destroy the default allocator
     */
    DNX_KAPS_TRY(kaps_default_allocator_destroy(info.kaps_alloc));

    DNX_KAPS_TRY(kaps_default_allocator_destroy(info.ctest_alloc));

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_kaps_default_test(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    shr_error_e shr_status = _SHR_E_NONE;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Running the test for J2 KAPS
     */
    shr_status = kaps_workflow_fn(unit, sand_control);

    if (shr_status != _SHR_E_NONE)
        SHR_ERR_EXIT(shr_status, "KAPS test failed with status = %d\n", shr_status);

    LOG_CLI((BSL_META("Test passed\n")));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_kaps_verifier(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    shr_error_e shr_status = _SHR_E_NONE;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Running the test for J2 KAPS
     */
    shr_status = kaps_verifier_main(unit, sand_control);

    if (shr_status != _SHR_E_NONE)
        SHR_ERR_EXIT(shr_status, "KAPS test failed with status = %d\n", shr_status);

    LOG_CLI((BSL_META("KAPS Main Test passed\n")));

exit:
    SHR_FUNC_EXIT;
}


/* *INDENT-OFF* */

/*List of XML Commands*/
sh_sand_cmd_t dnx_kaps_xml_test_cmds[] = {
   /*************************************************************************************************************************************
    *   CMD_NAME    *     CMD_ACTION            * Next                    *        Options                 *         MAN                *
    *               *                           * Level                   *                                *                            *
    *               *                           * CMD                     *                                *                            *
    *************************************************************************************************************************************/
    {"regular_ad", dnx_kaps_verifier, NULL, dnx_kaps_xml_tests_options, &dnx_kaps_xml_verifier_man, NULL, NULL, 0, dnx_kaps_verifier_add_regular_ad_dyn_tests},
    {"capacity", dnx_kaps_verifier, NULL, dnx_kaps_xml_tests_capacity_options, &dnx_kaps_xml_verifier_man, NULL, NULL, 0, dnx_kaps_verifier_add_capacity_dyn_tests, .label="kaps_all_profiles"},

   {NULL}
};

sh_sand_cmd_t dnx_kaps_test_cmds[] = {
   /*************************************************************************************************************************************
    *   CMD_NAME    *     CMD_ACTION            * Next                    *        Options                 *         MAN                *
    *               *                           * Level                   *                                *                            *
    *               *                           * CMD                     *                                *                            *
    *************************************************************************************************************************************/
    {"default", dnx_kaps_default_test, NULL, dnx_kaps_default_test_options, &dnx_kaps_default_test_man, NULL, NULL, CTEST_POSTCOMMIT},
    {"xml", NULL, dnx_kaps_xml_test_cmds, NULL, &dnx_kaps_xml_verifier_man, NULL, NULL},

   {NULL}
};

/* *INDENT-ON* */
