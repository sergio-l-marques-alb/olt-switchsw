/**
 * \file ctest_dnx_root.h
 *
 * Inlcude file for globals of root ctests for DNXdevices
 *
 */

#ifndef _INCLUDE_CTEST_DNX_ROOT_H
#define _INCLUDE_CTEST_DNX_ROOT_H

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <shared/shrextend/shrextend_error.h>
#include <appl/diag/sand/diag_sand_framework.h>

extern sh_sand_man_t sh_dnx_ctest_man;
extern sh_sand_cmd_t sh_dnx_ctest_commands[];

/**
 * \brief Send the packet and verify the results of run, based on definitions in XML file.
 *    In DNX-Devices.xml should be following section
 *   <case-objects>
 *       <include file="TestCases.xml" device_specific="1" type="tests"/>
 *   </case-objects>
 *   You may add unlimited number of XML files in above format, using type="tests"
 *   If the test file is specific per device it should be placed under $DB/device and marked as device_specific=1
 *   If the test file is applicable for all, it should be under $DB and device_specific=0
 *   Use $SDK/tools/sand/db/jericho_2/TestCases.xml as primary file with examples inside,, do not delete default packets
 *   and signal set, thay are used for code verification
 * \param [in] unit - device id
 * \param [in] core - core  id
 * \param [in] packet_n - packet name for test verification, packet with this name should be present in packets section
 * \param [in] set_n - signal set for test verification, signal set with this name should be present in signals section
 *
 * \retval _SHR_E_NONE if test was found, performed and verified
 * \retval _SHR_E_NOT_FOUND if there was no such test
 * \retval _SHR_E_FAIL - if the test was found and performed but failed
 * \remark
 */
shr_error_e ctest_dnx_case_verify(
    int unit,
    int core,
    char *packet_n,
    char *set_n);

#endif /* _INCLUDE_CTEST_DNX_ROOT_H */
