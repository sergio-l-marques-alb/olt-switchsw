/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * This File is Auto-generated - DO NOT TOUCH IT!!!!
 */

#if defined (BCM_DNX_SUPPORT)

#include <shared/shrextend/shrextend_debug.h>
#include <sal/core/libc.h>
#include <bcm_int/dnx/kbp/auto_generated/standard_1_dbx_pre_compiled_kbp_xml_parser.h>

#define STANDARD_1_DBX_PRE_COMPILED_MAX_STRING_SIZE 4000

/* *INDENT-OFF* */
const char *standard_1_kbp_opcodes_definition_0="<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\
<!--\n\
    Aligned to Pemla. Arch version: #***\n\
    Auto-Generted - DO NOT EDIT!!!!\n\
-->\n\
\n\
<KbpOpcodesDbCatalogue xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\"  xsi:noNamespaceSchemaLocation=\"../../kbp_opcodes_schema.xsd\">\n\
\n\
\n\
<Opcode Name=\"BRIDGE_SVL\" OpCode=\"11\">\n\
    <OneLineDescription>BRIDGE_SVL</OneLineDescription>\n\
    <Description>No Lookups!!!!Note!!!!\n\
    this is a not a real AppDB\n\
    this is KBP-Command (Master Key)\n\
    it is appear as AppDB in the device PP for the sake of completeness when transferring information to the SDK\n\
!!!!\n\
</Description>\n\
    <MasterKey/>\n\
    <FwdContexts>\n\
        <Context Name=\"BRIDGE___SVL\"/>\n\
    </FwdContexts>\n\
</Opcode>\n\
\n\
\n\
<Opcode Name=\"IPV4_MULTICAST_PRIVATE_W_UC_RPF\" OpCode=\"1\">\n\
    <OneLineDescription>Master Key for MultiCast</OneLineDescription>\n\
    <Description>4 lookups:\n\
                         (1)Forward: (a){VRF,G,S,In_LIF} (b){VRF,G,In_LIF,S}\n\
                         (2)RPF (a){VRF,SIP} (b){SIP}!!!!Note!!!!\n\
    this is a not a real AppDB\n\
    this is KBP-Command (Master Key)\n\
    it is appear as AppDB in the device PP for the sake of completeness when transferring information to the SDK\n\
!!!!\n\
</Description>\n\
    <AppDB Name=\"KBP_IPV4_MULTICAST_TCAM_FORWARD\" ResultLocation=\"FWD\" Segments=\"1, 2, 3, 4\"/>\n\
    <AppDB Name=\"KBP_IPV4_UNICAST_PRIVATE_LPM_RPF\" ResultLocation=\"RPF\" Segments=\"1, 3\"/>\n\
    <MasterKey>\n\
        <Field BitSize=\"16\" Name=\"ZERO_PADDING\" Segment=\"0\"/>\n\
        <Field BitSize=\"16\" Name=\"VRF\" Segment=\"1\"/>\n\
        <Field BitSize=\"32\" Name=\"IPV4_DIP\" Segment=\"2\"/>\n\
        <Field BitSize=\"32\" Name=\"IPV4_SIP\" Segment=\"3\"/>\n\
        <Field BitSize=\"24\" Name=\"GLOBAL_IN_LIF\" Segment=\"4\"/>\n\
    </MasterKey>\n\
    <FwdContexts>\n\
        <Context Name=\"IPV4___MC_PRIVATE_WO_BF_WO_F2B\"/>\n\
        <Context Name=\"IPV4___MC_PRIVATE_WO_BF_W_F2B\"/>\n\
        <Context Name=\"IPV4___MC_PRIVATE_W_BF_W_F2B\"/>\n\
    </FwdContexts>\n\
</Opcode>\n\
\n\
\n\
<Opcode Name=\"IPV4_UNICAST_PRIVATE_W_UC_RPF\" OpCode=\"3\">\n\
    <OneLineDescription>Master Key for Unicast IPv4</OneLineDescription>\n\
    <Description>4 lookups (1) Private and Public, (2) FWD and RFP, resulting in 2 results: FWD and RPF!!!!Note!!!!\n\
    this is a not a real AppDB\n\
    this is KBP-Command (Master Key)\n\
    it is appear as AppDB in the device PP for the sake of completeness when transferring information to the SDK\n\
!!!!\n\
</Description>\n\
    <AppDB Name=\"KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD\" ResultLocation=\"FWD\" Segments=\"0, 2\"/>\n\
    <AppDB Name=\"KBP_IPV4_UNICAST_PRIVATE_LPM_RPF\" ResultLocation=\"RPF\" Segments=\"0, 1\"/>\n\
    <MasterKey>\n\
        <Field BitSize=\"16\" Name=\"VRF\" Segment=\"0\"/>\n\
        <Field BitSize=\"32\" Name=\"IPV4_SIP\" Segment=\"1\"/>\n\
        <Field BitSize=\"32\" Name=\"IPV4_DIP\" Segment=\"2\"/>\n\
    </MasterKey>\n\
    <FwdContexts>\n\
        <Context Name=\"IPV4___PRIVATE_UC\"/>\n\
    </FwdContexts>\n\
</Opcode>\n\
\n\
\n\
<Opcode Name=\"IPV6_MULTICAST_PRIVATE_W_UC_RPF\" OpCode=\"2\">\n\
    <OneLineDescription>Master Key for MultiCast</OneLineDescription>\n\
    <Description>4 lookups:\n\
                         (1)Forward: (a){VRF,G,S,In_LIF} (b){VRF,G,In_LIF,S}\n\
                         (2)RPF (a){VRF,SIP} (b){SIP}!!!!Note!!!!\n\
    this is a not a real AppDB\n\
    this is KBP-Command (Master Key)\n\
    it is appear as AppDB in the device PP for the sake of completeness when transferring information to the SDK\n\
!!!!\n\
</Description>\n\
    <AppDB Name=\"KBP_IPV6_MULTICAST_TCAM_FORWARD\" ResultLocation=\"FWD\" Segments=\"1, 5, 2, 3, 6\"/>\n\
    <AppDB Name=\"KBP_IPV6_UNICAST_PRIVATE_LPM_RPF\" ResultLocation=\"RPF\" Segments=\"1, 2\"/>\n\
    <MasterKey>\n\
        <Field BitSize=\"8\" Name=\"ZERO_PADDING_4\" Segm";

const char *standard_1_kbp_opcodes_definition_1="ent=\"0\"/>\n\
        <Field BitSize=\"16\" Name=\"VRF\" Segment=\"1\"/>\n\
        <Field BitSize=\"128\" Name=\"IPV6_SIP\" Segment=\"2\"/>\n\
        <Field BitSize=\"8\" Name=\"GLOBAL_IN_LIF_21_16\" Segment=\"3\"/>\n\
        <Field BitSize=\"16\" Name=\"ZERO_PADDING_3\" Segment=\"4\"/>\n\
        <Field BitSize=\"128\" Name=\"IPV6_DIP\" Segment=\"5\"/>\n\
        <Field BitSize=\"16\" Name=\"GLOBAL_IN_LIF_15_00\" Segment=\"6\"/>\n\
    </MasterKey>\n\
    <FwdContexts>\n\
        <Context Name=\"IPV6___MC_PRIVATE_WO_BF_WO_F2B\"/>\n\
        <Context Name=\"IPV6___MC_PRIVATE_WO_BF_W_F2B\"/>\n\
        <Context Name=\"IPV6___MC_PRIVATE_W_BF_W_F2B\"/>\n\
    </FwdContexts>\n\
</Opcode>\n\
\n\
\n\
<Opcode Name=\"IPV6_UNICAST_PRIVATE_W_UC_RPF\" OpCode=\"4\">\n\
    <OneLineDescription>Master Key for Unicast IPv6</OneLineDescription>\n\
    <Description>4 lookups (1) Private and Public, (2) FWD and RFP, resulting in 2 results: FWD and RPF!!!!Note!!!!\n\
    this is a not a real AppDB\n\
    this is KBP-Command (Master Key)\n\
    it is appear as AppDB in the device PP for the sake of completeness when transferring information to the SDK\n\
!!!!\n\
</Description>\n\
    <AppDB Name=\"KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD\" ResultLocation=\"FWD\" Segments=\"1, 2\"/>\n\
    <AppDB Name=\"KBP_IPV6_UNICAST_PRIVATE_LPM_RPF\" ResultLocation=\"RPF\" Segments=\"1, 5\"/>\n\
    <MasterKey>\n\
        <Field BitSize=\"16\" Name=\"ZERO_PADDING\" Segment=\"0\"/>\n\
        <Field BitSize=\"16\" Name=\"VRF\" Segment=\"1\"/>\n\
        <Field BitSize=\"128\" Name=\"IPV6_DIP\" Segment=\"2\"/>\n\
        <Field BitSize=\"24\" Name=\"GLOBAL_IN_LIF\" Segment=\"3\"/>\n\
        <Field BitSize=\"8\" Name=\"ZERO_PADDING_2\" Segment=\"4\"/>\n\
        <Field BitSize=\"128\" Name=\"IPV6_SIP\" Segment=\"5\"/>\n\
    </MasterKey>\n\
    <FwdContexts>\n\
        <Context Name=\"IPV6___PRIVATE_UC\"/>\n\
        <Context Name=\"IPV6___PRIVATE_UC_BFD\"/>\n\
    </FwdContexts>\n\
</Opcode>\n\
\n\
\n\
<Opcode Name=\"MPLS_PER_PLATFORM\" OpCode=\"7\">\n\
    <OneLineDescription>Master Key for MPLS_PER_PLATFORM</OneLineDescription>\n\
    <Description>1 lookups (1) in KBP_MPLS_PER_PLATFORM!!!!Note!!!!\n\
    this is a not a real AppDB\n\
    this is KBP-Command (Master Key)\n\
    it is appear as AppDB in the device PP for the sake of completeness when transferring information to the SDK\n\
!!!!\n\
</Description>\n\
    <MasterKey/>\n\
    <FwdContexts>\n\
        <Context Name=\"MPLS___FWD\"/>\n\
    </FwdContexts>\n\
</Opcode>\n\
\n\
\n\
<Opcode Name=\"NOP\" OpCode=\"12\">\n\
    <OneLineDescription>NOP</OneLineDescription>\n\
    <Description>NOP!!!!Note!!!!\n\
    this is a not a real AppDB\n\
    this is KBP-Command (Master Key)\n\
    it is appear as AppDB in the device PP for the sake of completeness when transferring information to the SDK\n\
!!!!\n\
</Description>\n\
    <MasterKey/>\n\
    <FwdContexts>\n\
        <Context Name=\"GENERAL___NOP_CTX\"/>\n\
    </FwdContexts>\n\
</Opcode>\n\
\n\
</KbpOpcodesDbCatalogue>\n\
";


shr_error_e
pre_compiled_kbp_standard_1_xml_to_buf(
    char *filePath,
    char **buf,
    long int * size_p)
{
    char *file_buf;
    long int _internal_size_p;
    long int offset;

    SHR_FUNC_INIT_VARS(NO_UNIT);

    if(sal_strstr("/tools/sand/db/jericho2_a0/kbp/auto_generated/standard_1/standard_1_kbp_opcodes_definition.xml", filePath) != NULL)
    {
        _internal_size_p = 0;
        _internal_size_p += sal_strlen(standard_1_kbp_opcodes_definition_0);
        _internal_size_p += sal_strlen(standard_1_kbp_opcodes_definition_1);

        *size_p = _internal_size_p;
        offset = 0;
        file_buf = sal_alloc(*size_p, "buf");

        sal_strncpy(file_buf, standard_1_kbp_opcodes_definition_0, sal_strlen(standard_1_kbp_opcodes_definition_0));

        offset += sal_strlen(standard_1_kbp_opcodes_definition_0);
        sal_strncpy(file_buf + offset, standard_1_kbp_opcodes_definition_1, sal_strlen(standard_1_kbp_opcodes_definition_1));


        *buf = file_buf;
        SHR_EXIT();
    }
    /*
     * failure to find existing entry is error
     */
    SHR_SET_CURRENT_ERR(_SHR_E_EXISTS);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
/* *INDENT-ON* */
#else /* (BCM_DNX_SUPPORT) */

typedef int make_iso_compilers_happy;

#endif /* (BCM_DNX_SUPPORT) */
