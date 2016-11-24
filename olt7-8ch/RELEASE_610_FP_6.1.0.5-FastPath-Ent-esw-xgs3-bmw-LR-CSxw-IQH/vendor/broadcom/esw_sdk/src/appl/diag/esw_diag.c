/*
 * $Id: esw_diag.c,v 1.1 2011/04/18 17:10:59 mruas Exp $
 * $Copyright: Copyright 2009 Broadcom Corporation.
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
 * ESW socdiag command list
 */
 
#include "appl/diag/diag.h"
       
char cmd_age_esw_usage[] =
    "Parameters:  [<seconds>]\n\t"
    "   Set the hardware age timer to the indicated number of seconds.\n\t"
    "   With no parameter, displays current value.\n\t"
    "   Setting to 0 disables hardware aging\n";

extern cmd_result_t cmd_esw_age(int unit, args_t *a);
char auth_esw_cmd_usage[] =
    "Usages:\n\t"
#ifdef COMPILER_STRING_CONST_LIMIT
    "auth <option> [args...]\n"
#else
    "auth init\n\t"
    "       - Init auth function\n\t"
    "auth detach\n\t"
    "       - Detach auth function\n\t"
    "auth mac init\n\t"
    "       - Add switch mac address from config to all ports\n\t"
    "auth mac add  [PortBitMap = <pbmp>] [MACaddress=<address>]\n\t"
    "       - Add switch mac address\n\t"
    "auth mac del  [PortBitMap = <pbmp>] [MACaddress=<address>]\n\t"
    "       - Delete switch mac address\n\t"
    "auth mac clear  [PortBitMap = <pbmp>]\n\t"
    "       - Clear all switch mac addresses\n\t"
    "auth block [PortBitMap = <pbmp>] [IngressOnly=true|false]\n\t"
    "       - Block traffic for all directions or ingress direction only\n\t"
    "auth unblock [PortBitMap = <pbmp>]\n\t"
    "       - All traffic allowed, ports in uncontrolled state\n\t"
    "auth enable [PortBitMap = <pbmp>] [LearnEnable=true|false]\n\t"
    "            [IgnorLink=true|false]\n\t"
    "       - Authorized ports to allow traffic\n\t"
    "auth disable [PortBitMap = <pbmp>]\n\t"
    "       - Put ports back in block state\n\t"
    "auth show\n\t"
    "       - Show ports access state\n"
#endif
    ;
extern cmd_result_t cmd_esw_auth(int unit, args_t *a);

char if_esw_bpdu_usage[] =
    "Usages:\n\t"
    "  bpdu add [Index=<n>] [MACaddress=<mac>]\n\t"
    "        - Add BPDU addresses by index\n\t"
    "  bpdu del [Index=<n>]\n\t"
    "        - Delete BPDU address by index\n\t"
    "  bpdu show \n\t"
    "        - Show BPDU addresses and index range\n";

extern cmd_result_t if_esw_bpdu(int unit, args_t *a);

char cmd_esw_cablediag_usage[] =
	"Run Cable Diagnostics on a set of ports.\n"
	"Parameter: <portbitmap>\n";

extern cmd_result_t cmd_esw_cablediag(int unit, args_t *a);

char mem_esw_cache_usage[] =
    "Parameters: [+<TABLE>[.COPYNO] | -<TABLE>[.COPYNO]] ...\n\t"
    "If no parameters are given, displays caching status.  Otherwise,\n\t"
    "turns software caching on (+) or off (-) for specified TABLE(s).\n\t"
    "Use \"listmem\" for a list of tables and cachability.\n";

extern cmd_result_t mem_esw_cache(int unit, args_t *a);

char cmd_esw_clear_usage[] =
    "clear counters [PBMP] | [ALL] <TABLE> ... | DEV | SA | stats [PBMP]\n\t"
    "counters - zeroes all or some internal packet counters\n\t"
    "<TABLE> - clears specified memory table\n\t"
    "ALL - when clearing a sorted table, overwrite all entries instead\n\t"
    "      of just occupied ones (i.e. 1 through software-entry-count).\n\t"
    "DEV - Call bcm_clear on the unit to reset to known state\n\t"
    "SA - clears software ARL shadow table\n";

extern cmd_result_t cmd_esw_clear(int unit, args_t *a);

char if_esw_combo_usage[] =
    "Parameters: <ports> [copper|fiber [<option>=<value>]]\n"
    "            <ports> watch [on|off]\n\t"
    "Display or update operating parameters of copper/fiber combo PHY.\n\t"
    "Note: the 'port' command operates on the currently active medium.\n\t"
    "Example: combo ge1 fiber autoneg_enable=1 autoneg_advert=1000,pause\n\t"
    "Watch subcommand enables/disables media change notifications\n";

extern cmd_result_t if_esw_combo(int unit, args_t *a);

char cmd_esw_cos_usage[] =
    "Usages:\n\t"
#ifdef COMPILER_STRING_CONST_LIMIT
    "cos <option> [args...]\n"
#else
    "cos clear                  - Reset COS configuration to default\n\t"
    "cos config [Numcos=#]      - Set number of active COS queues\n\t"
    "cos map [Pri=#] [Queue=#]  - Map a priority 0-7 to a COS queue\n\t"
    "cos strict                 - Set strict queue scheduling mode\n\t"
    "cos simple                 - Set round robin queue scheduling mode\n\t"
    "cos weight [W0=#] [W1=#] ... - Set weighted queue scheduling mode\n\t"
    "                             with the specified weights per queue\n\t"
    "cos fair [W0=#] [W1=#] ... - Set weighted fair queue scheduling mode\n\t"
    "                             with the specified weights per queue\n\t"
    "cos bounded [W0=#] [W1=#] ... [Delay=<usec>]\n\t"
    "                           - Set bounded queue scheduling with delay\n\t"
    "cos drr [W0=#] [W1=#] ...  - Set Deficit Round Robin scheduling mode\n\t"
    "                             with the specified weights per queue\n\t"
    "cos discard [Enable=true|false] [CapAvg=true|false]\n\t"
    "                           - Set discard (WRED) config\n\t"
    "cos discard_port [PortBitMap=<pbmp>] [Queue=#] [Color=green|yellow|red]\n\t"
    "                 [DropSTart=#] [DropSLope=#] [AvgTime=#] [Gport=<GPORT ID>]\n\t"
    "                           - Set port discard (WRED) config\n\t"
    "cos discard_show [Gport=<GPORT ID>] [Queue=#]\n\t"
    "                           - Show current discard (WRED) config\n\t"
    "cos discard_gport [Enable=true|false] [CapAvg=true|false]\n\t"
    "                  [Gport=<GPORT ID>] [Queue=#] [Color=green|yellow|red]\n\t"
    "                  [DropSTart=#] [DropEnd=#] [DropProbability=#] [GAin=#]\n\t"
    "                           - Set GPORT discard (WRED) config\n\t"
    "cos bandwidth [PortBitMap=<pbmp>] [Queue=#] [KbpsMIn=#]\n\t"
    "              [KbpsMAx=#] [Flags=#]\n\t"
    "                           - Set cos bandwidth\n\t"
    "cos bandwidth_show         - Show current COS bandwidth config\n\t"
    "cos show                   - Show current COS config\n"
    "\n\t"
    "cos port config [PortBitMap=<pbmp>] [Numcos=#] [Gport=<GPORT ID>]\n\t"
    "                           - Add and attach a COSQ GPORT\n\t"
    "cos port map [PortBitMap=<pbmp>] [Pri=#] [Queue=#]\n\t"
    "                           - Map a port's priority 0-7 to a COS queue\n\t"
    "cos port strict            - Set strict queue scheduling mode\n\t"
    "cos port simple            - Set round robin queue scheduling mode\n\t"
    "cos port weight [PortBitMap=<pbmp>] [Queue=#] [Weight=#] [Gport=<GPORT ID>]\n\t"
    "                           - Set weighted queue scheduling mode\n\t"
    "                             with the specified weight of queue\n\t"
    "cos port fair [PortBitMap=<pbmp>] [Queue=#] [Weight=#] [Gport=<GPORT ID>]\n\t"
    "                           - Set weighted fair queue scheduling mode\n\t"
    "                             with the specified weight of queue\n\t"
    "cos port bounded [PortBitMap=<pbmp>] [Queue=#] [Weight=#] [Gport=<GPORT ID>]\n\t"
    "                           - Set bounded queue scheduling mode\n\t"
    "                             with the specified weight of queue\n\t"
    "cos port drr [PortBitMap=<pbmp>] [Queue=#] [Weight=#] [Gport=<GPORT ID>]\n\t"
    "                           - Set Deficit Round Robin scheduling mode\n\t"
    "                             with the specified weight of queue\n\t"
    "cos port bandwidth [PortBitMap=<pbmp>] [Queue=#] [KbpsMIn=#]\n\t"
    "                   [KbpsMAx=#] [Flags=#] [Gport=<GPORT ID>]\n\t"
    "                           - Set port cos bandwidth\n\t"
    "cos port show [PortBitMap=<pbmp>]\n\t"
    "                           - Show current port COS config\n"
#endif
    ;
extern cmd_result_t cmd_esw_cos(int unit, args_t *a);

char cmd_esw_counter_usage[] =
    "\nParameters: [off] [sync] [Interval=<usec>]\n\t"
    "\t[PortBitMap=<pbm>] [DMA=<true|false>]\n\t"
    "Starts the counter collection task and/or DMA running every <usec>\n\t"
    "microseconds.  The task tallies software counters based on hardware\n\t"
    "values and must run often enough to avoid counter overflow.\n\t"
    "If <interval> is 0, stops the task.  If <interval> is omitted, prints\n\t"
    "the current INTERVAL.  sync reads in all the counters to synchronize\n\t"
    "'show counters' for the first time, and must be used alone.\n";

extern cmd_result_t cmd_esw_counter(int unit, args_t *a);

char cmd_esw_custom_stat_usage[] =
    "Usages:\n\t"
#ifdef COMPILER_STRING_CONST_LIMIT
    "CustomSTAT <option> [args...]\n"
#else
    "  cstat <pbmp> [z]\n\t"
    "        - Display all non-zero custom counters and their triggers\n\t"
    "            z  - include counters that are zero\n\t"
    "  cstat raw <pbmp> [rx|tx] <counter>\n\t"
    "        - Display value of counter in raw mode\n\t"
    "  cstat set <pbmp> [rx|tx] <counter> <triggers>\n\t"
    "        - Set triggers for given counter\n\t"
    "  cstat get <pbmp> [rx|tx] <counter>\n\t"
    "        - Get currently enabled triggers of counter\n\t"
    "  cstat ls [rx|tx]\n\t"
    "        - List available triggers\n\t" 
    "  cstat info [rx|tx]\n\t"
    "        - List available triggers with short description\n\t"
    "        Note: To specify all counters use 'all'\n\t"
    "Displays and configures programmable statistic counters\n\t"
    "RDBGC0,TDBGC0 etc.\n"
#endif
;

extern cmd_result_t cmd_esw_custom_stat(int unit, args_t *a);


char if_esw_dscp_usage[] =
    "Usages:\n"
    "  dscp pbmp [Mode=<mode>][source [mapped [prio [cng]]]]\n"
    "        - map source dscp to mapped\n";

extern cmd_result_t if_esw_dscp(int unit, args_t *a);

char if_esw_dtag_usage[] =
	"Usage:\n"
	"\tdtag show <pbmp>\n"
	"\tdtag mode <pbmp> none|internal|external\n"
	"\tdtag tpid <pbmp> hex-value\n";

extern cmd_result_t if_esw_dtag(int unit, args_t *a);

char cmd_esw_dump_usage[] =
    "Usages:\n\t"
#ifdef COMPILER_STRING_CONST_LIMIT
    "DUMP [options]\n"
#else
    "DUMP [File=<name>] [Append=true|false] [raw] [hex] [all] [chg]\n\t"
    "        <TABLE>[.<COPYNO>] [<INDEX>] [<COUNT>]\n\t"
    "        [-filter <FIELD>=<VALUE>[,...]]\n\t"
    "      If raw is specified, show raw memory words instead of fields.\n\t"
    "      If hex is specified, show hex data only (for Expect parsing).\n\t"
    "      If all is specified, show even empty or invalid entries\n\t"
    "      If chg is specified, show only fields changed from defaults\n\t"
    "      (Use \"listmem\" command to show a list of valid tables)\n\t"
    "DUMP PCIC                     (PCI config space)\n\t"
    "DUMP PCIM [<START> [<COUNT>]] (CMIC PCI registers)\n\t"
    "DUMP SOC [ADDR | RVAL | DIFF] (All SOC registers)\n\t"
    "      ADDR shows only addresses, doesn't actually load.\n\t"
    "      RVAL shows reset defaults, doesn't actually load.\n\t"
    "      DIFF shows only regs not equal to their reset defaults.\n\t"
    "DUMP SOCMEM [DIFF] (All SOC memories)\n\t"
    "      DIFF shows only memories not equal to their reset defaults.\n\t"
    "DUMP MW [<START> [<COUNT>]]   (System memory, 32 bits)\n\t"
    "DUMP MH [<START> [<COUNT>]]   (System memory, 16 bits)\n\t"
    "DUMP MB [<START> [<COUNT>]]   (System memory, 8 bits)\n\t"
    "DUMP SA                       (ARL shadow table)\n\t"
    "DUMP DV ADDR                  (DMA vector)\n\t"
    "DUMP PHY [<PHYID>]            See also, the 'PHY' command.\n"
#endif
    ;
extern cmd_result_t cmd_esw_dump(int unit, args_t *a);

char cmd_esw_reg_edit_usage[] =
    "Parameters: <REGISTER>\n\t"
    "Loads a register and displays each field, providing an opportunity\n\t"
    "to modify each field.\n";

extern cmd_result_t cmd_esw_reg_edit(int unit, args_t *a);

char if_esw_field_proc_usage[] =
#ifdef COMPILER_STRING_CONST_LIMIT
"Parameters: <cmd> <opt>\n"
#else /* !COMPILER_STRING_CONST_LIMIT */
" Where <cmd> is:\n"
"\taction <add|get|remove> <eid> [act] [p0] [p1]\n"
"\taction ports <add|get> <eid> <act> [<pbmp>]\n"
"\taction mac <add|get> <eid> <act> [<mac>]\n"
"\tcontrol <ctrl_num> [<status>]\n"
"\tdetach\n"
"\tentry create <gid> [<eid>]\n"
"\tentry copy <src_eid> [<dst_eid>]\n"
"\tentry install|reinstall|remove|destroy <eid>\n"
"\tentry prio <eid> [highest|lowest|dontcare|default|<priority>]\n"
"\tgroup create <pri> [<gid>] [mode] [pbmp]\n"
"\tgroup destroy|get|set <gid>\n"
"\tinit\n"
"\tlist actions|qualifiers\n"
"\trange create [<rid>] [<flags>] [<min>] [<max>]\n"
"\trange group create [<rid>] [<flags>] [<min>] [<max>] [<group>]\n"
"\trange get|destroy <rid>\n"
"\tresync\n"
"\tpolicer create PolId=<polcerid> mode=<mode> cbs=<cbs> \n"
"\t        cir=<cir> ebs=<ebs> eir=<eir> ColorBlind=<bool> \n" 
"\t        ColorMergeOr=<bool>\n"
"\tpolicer destroy [all] or PolId=<polcerid>] \n"
"\tpolicer attach entry=<eid> level=<level> PolId=<polcerid> \n"
"\tpolicer detach entry=<eid> level=<level> \n"
"\tstat create group=<group> type1=<stat_type1> type2=<stat_type2>\n"
"\t        type3=<stat_type3> typeX=<stat_typeX>\n"
"\tstat destroy StatId=<statid> \n"
"\tstat attach entry=<eid> StatId=<statid> \n"
"\tstat detach entry=<eid> StatId=<statid> \n"
"\tstat set StatId=<statid> type=<stat_type> val=<value>\n"
"\tstat get StatId=<statid> type=<stat_type>\n"
"\tdata create OffsetBase=<offset_base> offset=<offset> length=<length>\n"
"\tdata destroy [all] or QualId=<qualid> \n"
"\tdata format add QualId=<qualid> RelativeOffset=<relative_offset> \n"
"\t     L2=<l2> VlanTag=<tag_format> OuterIp=<outer_ip> InnerIp=<inner_ip>\n"
"\t     Tunnel=<tunnel> mpls=<mpls>\n"
"\tdata format delete QualId=<qualid> RelativeOffset=<relative_offset>\n"
"\t     L2=<l2> VlanTag=<tag_format> OuterIp=<outer_ip> InnerIp=<inner_ip> \n"
"\t     Tunnel=<tunnel> Mpls=<mpls>\n"
"\tdata ethertype add QualId=<qualid> RelativeOffset=<relative_offset>\n"
"\t     etype=<ethertype> VlanTag=<tag_format> L2=<l2>\n"
"\tdata ethertype delete QualId=<qualid> RelativeOffset=<relative_offset>\n"
"\t     etype=<ethertype> VlanTag=<tag_format> L2=<l2>\n"
"\tdata ipproto add QualId=<qualid> RelativeOffset=<relative_offset>\n"
"\t     protocol=<protocol> VlanTag=<tag_format> L2=<l2> IpVer=<ip_version>\n"
"\tdata ipproto delete QualId=<qualid> RelativeOffset=<relative_offset>\n"
"\t     protocol=<protocol> VlanTag=<tag_format> L2=<l2> IpVer=<ip_version>\n"
"\tmeter create|destroy <eid>\n"
"\tmeter share <src_eid> <dst_eid>\n"
"\tmeter getc|getp|setc|getp <eid> [<rate>] [<burst>]\n"
"\tcounter create|get|set <eid> [<0|1>] [<val>]\n"
"\tcounter share <src_eid> <dst_eid>\n"
"\tcounter destroy <eid>\n"
"\tqual <eid> <QUAL> [<udf_id*>/<data_qual_id>] data mask \n"
"\t        (*required for QUAL=UserDefined or Data)\n"
"\tqual <eid> delete <QUAL>\n"
"\tqual <eid> clear\n"
"\tqset add|clear|set|show [qualifier|qualifier list]\n"
"\tshow [group|entry] [id]\n"
"\tuser get|set|init|create [udf_id] [flags] [offset]\n"
"\tstatus\n"
#endif /* !COMPILER_STRING_CONST_LIMIT */
;
extern cmd_result_t if_esw_field_proc(int unit, args_t *a);

char cmd_esw_reg_get_usage[] =
    "Parameters: [hex|raw|chg] [<REGTYPE>] <REGISTER>\n\t"
    "If <REGTYPE> is not specified, it defaults to \"soc\".\n\t"
    "<REGISTER> designates offset, address, or symbolic name.\n\t"
    "If hex is specified, dumps only a hex value (for Expect parsing).\n\t"
    "If raw is specified, no field decoding is done.\n\t"
    "If chg is specified, show only fields/regs changed from defaults.\n\t"
    "For a list of register types, use \"dump\".\n";

extern cmd_result_t cmd_esw_reg_get(int unit, args_t *a);

char if_esw_ipg_usage[] =
    "Parameters: [PortBitMap=<pbmp>] [SPeed=10|100|1000|2500|10000]\n\t"
    "[FullDuplex=true|false] [Gap=<int>]\n"
    "Set the IPG register values to the specified values when the\n\t"
    "MAC is programmed after autonegotiation OR when the port is\n\t"
    "into the specific mode.  If no args are given, displays settings.\n";

extern cmd_result_t if_esw_ipg(int unit, args_t *a);

char if_esw_l2_usage[] =
    "Usages:\n\t"
#ifdef COMPILER_STRING_CONST_LIMIT
    "  l2 <option> [args...]\n"
#else
    "  l2 add [PortBitMap=<pbmp>] [MACaddress=<mac>] [Vlanid=<id>]\n\t"
    "         [Trunk=true|false] [TrunkGroupId=<id>] [STatic=true|false]\n\t"
    "         [SourceCosPriority=true|false] [DiscardSource=true|false]\n\t"
    "         [DiscardDest]=true|false] [L3=true|false] [HIT=true|false]\n\t"
    "         [Replace=true|false] [CPUmirror=true|false]\n\t"
    "         [ReplacePriority=true|false] [RemoteLookup=true|false]\n\t"
    "         [MacBlockPortBitMap=<pbmp>] [Group=<group>]\n\t"
    "        - Add incrementing L2 addresses associated with port(s)\n\t"
    "  l2_replace [Module=<n>] [Port=<port>] [MACaddress=<mac>]\n\t"
    "             [Vlanid=<id>] [Trunk=true|false] [TrunkGroupId=<id>]\n\t"
    "             [STatic=true|false] [Pending=true|false] \n\t"
    "             [NewModule=<n>] [NewPort=<port>] [NewTrunkGroupId=<id>]\n\t"
    "  l2 del [MACaddress=<mac>] [Count=<value>] [Vlanid=<id>]\n\t"
    "        - Delete incrementing L2 address(s)\n\t"
    "  l2 show [PortBitMap=<pbmp]\n\t"
    "        - Show L2 addresses associated with port(s)\n\t"
    "  l2 check [hit, valid, static]\n\t"
    "        - Scan Draco l2 tables for entries entries.\n\t"
    "          With no subcommand, checks full L2 table for valid entries\n\t"
    "  l2 clear [Module=<n>] [Port=<port>] [MACaddress=<mac>]\n\t"
    "           [Vlanid=<id>] [TrunkGroupID=<id>] [Static=true|false]\n\t"
    "        - Remove all L2 entries on the given module, module/port,\n\t"
    "           VLAN, or trunk group ID\n\t"
    "  l2 sanity [Start=<index>] [End=<index>]\n\t"
    "        - Check l2x tables for (san)ity over the selected range\n\t"
    "  l2 hash [MACaddress=<mac>] [Vlanid=<id>] [Hash=<hash_type>]\n\t"
    "        - Calculate L2 hash index\n\t"
    "  l2 conflict [MACaddress=<mac>] [Vlanid=<id>]\n\t"
    "        - Dump all conflicting L2 entries (same hash bucket)\n\t"
    "  l2 watch [start | stop]\n\t"
    "        - Watch dynamic address insertions/deletions\n"
#ifdef BCM_XGS3_SWITCH_SUPPORT
    "\t"
    "  l2 cache add [CacheIndex=<index>]\n\t"
    "         [MACaddress=<mac>] [MACaddressMask=<mac>]\n\t"
    "         [Vlanid=<id>] [VlanidMask=<mask>] [LookupClass=<class>]\n\t"
    "         [SourcePort=<id>] [SourcePortMask=<mask>]\n\t"
    "         [Module=<modid>] [Port=<port>] \n\t"
    "         [Trunk=true|false] [TrunkGroupId=<id>]\n\t"
    "         [SetPriority=true|false] [PRIOrity=<prio>]\n\t"
    "         [DiscardDest]=true|false] [CPUmirror=true|false]\n\t"
    "         [MIRror=true|false] [BPDU=true|false] [L3=true|false]\n\t"
    "         [ReplacePriority=true|false] [RemoteLookup=true|false]\n\t"
    "         [TerminateTunnel=true|false] [LearnDisable=true|false]\n\t"
    "        - Add L2 cache entry\n\t"
    "  l2 cache del CacheIndex=<index> [Count=<value>]\n\t"
    "        - Delete L2 cache entry\n\t"
    "  l2 cache show\n\t"
    "        - Show L2 cache entries\n\t"
    "  l2 cache clear [ReInit=true|false]\n\t"
    "        - Delete all L2 cache entries\n"
#endif /* BCM_XGS3_SWITCH_SUPPORT */
#endif
    ;
extern cmd_result_t if_esw_l2(int unit, args_t *a);

char if_esw_linkscan_usage[] =
    "Parameters: [SwPortBitMap=<pbmp>] [HwPortBitMap=<pbmp>]\n\t"
    "[Interval=<usec>] [FORCE=<pbmp>]\n"
#ifndef COMPILER_STRING_CONST_LIMIT
    "\tWith no arguments, displays the linkscan status for all ports.\n\t"
    "Enables software linkscan on ports specified by SwPortBitMap.\n\t"
    "Enables hardware linkscan on ports specified by HwPortBitMap.\n\t"
    "Disables linkscan on all other ports.\n\t"
    "Interval specifies non-default linkscan interval for software.\n\t"
    "Note: With linkscan disabled, autonegotiated ports will NOT have\n\t"
    "the MACs updated with speed/duplex..\n\t"
    "FORCE=<pbmp> requests linkscan to update ports once even if link\n\t"
    "status did not change.\n"
#endif
    ;
extern cmd_result_t if_esw_linkscan(int unit, args_t *a);

char cmd_esw_mem_list_usage[] =
    "Parameters: [<TABLE> [<DATA> ...]]\n\t"
    "If no parameters are given, displays a reference list of all\n\t"
    "memories and their attributes.\n\t"
    "If TABLE is given, displays the entry fields for that table.\n\t"
    "If DATA is given, decodes the data into entry fields.\n";

extern cmd_result_t cmd_esw_mem_list(int unit, args_t *a);

char cmd_esw_reg_list_usage[] =
"Usage: listreg [options] regname [value]\n"
"Options:\n"
"	-alias/-a	display aliases\n"
"	-summary/-s	display single line summary for each reg\n"
"	-counters/-c	display counters\n"
"	-ed/-e		display error/discard counters\n"
"	-type/-t	display registers grouped by block type\n"
"If regname is '*' or does not match a register name, a substring\n"
"match is performed.  [] indicates register array.\n"
"If regname is a numeric address, the register that resides at that\n"
"address is displayed.\n";
extern cmd_result_t cmd_esw_reg_list(int unit, args_t *a);

char if_esw_mcast_usage[] =
    "Usages:\n\t"
#ifdef COMPILER_STRING_CONST_LIMIT
    "  mcast <option> [args...]\n"
#else
    "For Switch Devices:\n\t"
    "  mcast add MACaddress=<val> Vlanid=<val> Cos=<val>\n\t"
    "            PortBitMap=<val> UntagBitMap=<val> Index=<val>\n\t"
    "  mcast delete MACaddress=<val> Vlanid=<val>\n\t"
    "  mcast join MACaddress=<val> Vlanid=<val> PortBitMap=<val>\n\t"
    "  mcast leave MACaddress=<val> Vlanid=<val> PortBitMap=<val>\n\t"
    "  mcast padd MACaddress=<val> Vlanid=<val> PortBitMap=<val>\n\t"
    "  mcast premove MACaddress=<val> Vlanid=<val> PortBitMap=<val>\n\t"
    "For Fabric Devices:\n\t"
    "  mcast bitmap max\n\t"
    "  mcast bitmap set Port=<val> PortBitMap=<val> Index=<val>\n\t"
    "  mcast bitmap del Port=<val> PortBitMap=<val> Index=<val>\n\t"
    "  mcast bitmap get Port=<val> Index=<val>\n"
#endif
    ;
extern cmd_result_t if_esw_mcast(int unit, args_t *a);

char if_esw_mirror_usage[] =
    "mirror [options...]\n"
    "Switch Device Options:\n"
	"\tMode=<Off|L2|L3>	- set the mirroring mode\n"
	"\tPort=<p>		- set the mirror-to destination port\n"
	"\tIngressBitMap=<pbmp>	- ports with ingress mirroring enabled\n"
	"\tEgressBitMap=<pbmp>	- ports with egress mirroring enabled\n"
	"\tPreserveForMat=<Off|On> - preserve format\n"
    "Fabric Device Options: (5670/5675)\n"
	"\tPort=<p>		- which port to set the bitmap\n"
	"\tBitMap=<pbmp>	- destination bitmap for mirroring\n";

extern cmd_result_t if_esw_mirror(int unit, args_t *a);

char memwatch_usage[] =
    "Parameters: [Mem=<Memory>] [write] [read] [off]\n\t"
    "Mem specifies which memory to watch\n\t"
    "Write - specify to start watching writing operations on memory\n\t"
    "Read - specify to start watching reading operations on memory\n\t"
    "Off - specify to stop watching all operations on memory\n";

extern cmd_result_t mem_watch(int unit, args_t *a);

char cmd_esw_mem_modify_usage[] =
    "Parameters: <TABLE>[.<COPY>] <ENTRY> <ENTRYCOUNT>\n\t"
    "        <FIELD>=<VALUE>[,...]\n\t"
    "Read/modify/write field(s) of a table entry(s).\n";

extern cmd_result_t cmd_esw_mem_modify(int unit, args_t *a);

char cmd_esw_reg_mod_usage[] =
    "Parameters: <REGISTER> <FIELD>=<VALUE>[,...]\n\t"
    "<REGISTER> is SOC register symbolic name.\n\t"
    "<FIELD>=<VALUE>[,...] is a list of fields to affect,\n\t"
    "for example: L3_ENA=0,CPU_PRI=1.\n\t"
    "Fields not specified in the list are left unaffected.\n\t"
    "For a list of register types, use \"dump\".\n";

extern cmd_result_t cmd_esw_reg_mod(int unit, args_t *a);

char cmd_esw_pbmp_usage[] =
    "Parameters: <pbmp>\n"
#ifndef COMPILER_STRING_CONST_LIMIT
    "\tConverts a pbmp string into a hardware port bitmap.  A pbmp string\n\t"
    "is a single port, or a group of ports specified in a list using ','\n\t"
    "to separate them and '-' for ranges, e.g. 1-8,25,cpu.  Ports may be\n\t"
    "specified using the one-based port number (1-29) or port type and\n\t"
    "zero-based number (fe0-fe23,ge0-ge7).  'cpu' is the CPU port,\n\t"
    "'fe' is all FE ports, 'ge' is all GE ports, 'e' is all ethernet\n\t"
    "ports, 'all' is all ports, and 'none' is no ports (0x0).\n\t"
    "A '~' may be used to exclude port previously given (e.g. e,~fe19)\n\t"
    "Acceptable strings and values also depend on the chip being used.\n\t"
    "A pbmp may also be given as a raw hex (0x) number, e.g. 0xbffffff.\n"
#endif
    ;
extern cmd_result_t cmd_esw_pbmp(int unit, args_t *a);

char if_esw_phy_usage[] =
    "Parameters: [int] <ports> [regnum [value/devad] [value]]\n"
    "            raw [c45] <mii-addr> [devad] <regnum> [<value>]\n"
    "            dumpall [c22|c45]\n"
    "            info\n"
    "            control <ports> [phy_control_type=value]\n" 
    "            firmware <port> set=<filename>\n"
    "            power   <ports> [mode=mode_type]\n"
#ifndef COMPILER_STRING_CONST_LIMIT
    "\tSet or display PHY registers.  If only <ports> is specified,\n\t"
    "then registers for those ports' PHYs are displayed. <ports> is a\n\t"
    "standard port bitmap, e.g. fe for all 10/100 ports, fe5-fe7 for\n\t"
    "three FE's, etc. (see \"help pbmp\").  If the int option is given,\n\t"
    "the intermediate PHY for the port is used instead of the outer PHY.\n\t"
    "In 'raw' mode, the direct mii-address can be specified, only\n\t"
    "'writing' is supported.\n\t"
    "dumpall [c22|c45] - Do a c22|c45 PHY scan of ID registers\n\t"
    "Info    - display PHY device ID and attached PHY drivers\n\t" 
    "firmware - program the firmware given by the filename to the PHY \n\t"
    "           device's non-volatile storage. The file must be in binary\n\t"
    "           format.\n\t" 
    "power    - set or display power mode for the PHY devices implemented\n\t"
    "           low power control. mode_type has these values: auto_low,\n\t"
    "           auto_off,low and full\n\t" 
    "control - set or display PHY settings such as WAN, Preemphasis, etc.,\n"
#endif
    ;
extern cmd_result_t if_esw_phy(int unit, args_t *a);

char if_esw_port_usage[] =
#ifdef COMPILER_STRING_CONST_LIMIT
    "Usage: port <option> [args...]\n"
#else
    "Parameters: <pbmp> [ENCap=IEEE|HIGIG|B5632|HIGIG2]\n\t"
    "[AutoNeg=on|off] [ADVert=<portmode>]\n\t"
    "[LinkScan=on|off|hw|sw] [SPeed=10|100|1000] [FullDuplex=true|false]\n\t"
    "[TxPAUse=on|off] [RxPAUse=on|off] [STationADdr=<macaddr>]\n\t"
    "[LeaRN=<learnmode>] [DIScard=none|untag|all] [VlanFilter=<value>]\n\t"
    "[PRIOrity=<0-7>] [PortFilterMode=<value>]\n\t"
    "[PHymaster=<Master|Slave|Auto|None>] [Enable=<true|false>]\n\t"
    "[FrameMax=<value>]\n\t"
    "[MDIX=Auto|ForcedAuto|ForcedNormal|ForcedXover]\n\t"
    "[LoopBack=NONE|MAC|PHY]\n\t"
    "If only <ports> is specified, characteristics for that port are\n\t"
    "displayed. <ports> is a standard port bitmap (see \"help pbmp\").\n\t"
    "If AutoNeg is on, SPeed and DUPlex are the ADVERTISED MAX values.\n\t"
    "If AutoNeg is off, SPeed and DUPlex are the FORCED values.\n\t"
    "SPeed of zero indicates maximum speed.\n\t"
    "LinkScan enables automatic scanning for link changes with updating\n\t"
    "of MAC registers, and EPC_LINK (or equivalent)\n\t"
    "PAUse enables send/receive of pause frames in full duplex mode.\n\t"
    "<learnmode> is a numeric value controlling source lookup failure\n\t"
    "packets; it may include bit 0 to enable hardware L2 learn, bit 1\n\t"
    "to copy SLF packets to CPU, bit 2 to forward SLF packets.\n\t"
    "VlanFilter drops input packets that not tagged with a valid VLAN\n\t"
    "that contains the port. For XGS3, VlanFilter takes a value 0/1/2/3 where\n\t"
    "bit 0 turns on/off ingress filter and bit 1 turns on/off egress filter.\n\t"
    "PRIOrity sets the priority for untagged packets coming on this port.\n\t"
    "PortFilterMode takes a value 0/1/2 for mode A/B/C (see register manual).\n"
#endif
    ;
extern cmd_result_t if_esw_port(int unit, args_t *a);

char if_esw_port_rate_usage[] =
    "Set/Display port rate metering characteristics.\n"
    "Parameters: <pbm> [ingress|egress|pause [arg1 arg2]]\n\t"
    "    For Ingress or Egress: arg1 is rate, arg2 is max_burst\n\t"
    "    For Pause: arg1 is pause_thresh, arg2 is resume_thresh\n\t"
    "    rate is in kilobits (1000 bits) per second\n\t"
    "    max_burst and xxx_thresh are in kilobits (1000 bits)\n";
extern cmd_result_t if_esw_port_rate(int unit, args_t *a);

char if_esw_port_stat_usage[] =
    "Display info about port status in table format.\n"
    "    Link scan modes:\n"
    "        SW = software\n"
    "        HW = hardware\n"
    "    Learn operations (source lookup failure control):\n"
    "        F = SLF packets are forwarded\n"
    "        C = SLF packets are sent to the CPU\n"
    "        A = SLF packets are learned in L2 table\n"
    "        D = SLF packets are discarded.\n"
    "    Pause:\n"
    "        TX = Switch will transmit pause packets\n"
    "        RX = Switch will obey pause packets\n";
extern cmd_result_t if_esw_port_stat(int unit, args_t *a);

char cmd_esw_reg_set_usage[] =
    "1. Parameters: [<REGTYPE>] <REGISTER> <VALUE>\n\t"
    "If <REGTYPE> is not specified, it defaults to \"soc\".\n\t"
    "<REGISTER> is offset, address, or symbolic name.\n"
    "2. Parameters: <REGISTER> <FIELD>=<VALUE>[,...]\n\t"
    "<REGISTER> is SOC register offset or symbolic name.\n\t"
    "<FIELD>=<VALUE>[,...] is a list of fields to affect,\n\t"
    "for example: L3_ENA=0,CPU_PRI=1.\n\t"
    "Fields not specified in the list are set to zero.\n\t"
    "For a list of register types, use \"help dump\".\n";
extern cmd_result_t cmd_esw_reg_set(int unit, args_t *a);

char if_esw_pvlan_usage[] =
    "Usages:\n\t"
    "  pvlan show <pbmp>\n\t"
    "        - Show PVLAN info for these ports.\n\t"
    "  pvlan set <pbmp> <vid>\n\t"
    "        - Set default VLAN tag for port(s)\n\t"
    "          Port bitmaps are read from the VTABLE entry for the VID.\n\t"
    "          <vid> must have been created and all ports in <pbmp> must\n\t"
    "          belong to that VLAN.\n";
extern cmd_result_t if_esw_pvlan(int unit, args_t *a);

char cmd_esw_rate_usage[] =
    "Parameters: [PortBitMap=<pbm>] [Limit=<limit>] [Bcast=true|false]\n\t"
    "  [Mcast=true|false] [Dlf=true|false]\n\t"
    "Enables the specified packet rate controls.\n\t"
    "  pbm       port(s) to set up or display\n\t"
    "  limit     packets per second\n\t"
    "  bcast     Enable broadcast rate control\n\t"
    "  mcast     Enable multicast rate control\n\t"
    "  dlf       Enable DLF flooding rate control\n\t"
    "If no flags are given or only <pbm> is given, displays the current\n\t"
    "rate settings.\n";
extern cmd_result_t cmd_esw_rate(int unit, args_t *a);

char cmd_esw_rx_cfg_usage[] =
    "rxcfg [<chan>] [options...]\n"
#ifndef COMPILER_STRING_CONST_LIMIT
    "    With no options, displays current configuration\n"
    "    Global options include:\n"
    "        SPPS=<n>            Set system-wide packet per second limit\n"
    "                            Other options combined this are ignored\n"
    "        GPPS=<n>            Set global (all COS) pkts per second limit\n"
    "        PKTSIZE=<n>         Set maximum receive packet size\n"
    "        PPC=<n>             Set the number of pkts/chain\n"
    "        BURST=<n>           Set global (all COS) packet burst size\n"
    "        COSPPS<n>=<r>       Set the per COS rate limiting\n"
    "        FREE=[T|F]          Should handler free buffers?\n"
    "    Channel specific options include:\n"
    "        CHAINS=<n>          Set the number of chains for the channel\n"
    "        PPS=<n>             Set packet per second for channel\n"
    "        COSBMP=<bmp>        COS bitmap to accept on channel\n"
    "    Global options can be given w/o a channel.  Channel options\n"
    "    require that a channel be specified.\n"
    "    The channel's burst rate is #chains * pkts/chain\n"
#endif
    ;
extern cmd_result_t cmd_esw_rx_cfg(int unit, args_t *a);

char cmd_esw_rx_init_usage[] =
    "RXInit <override-unit>\n"
    "    Call bcm_rx_init on the given override unit.  Ignores"
    "    the current unit.\n";
extern cmd_result_t cmd_esw_rx_init(int unit, args_t *a);

char cmd_esw_rx_mon_usage[] =
    "Parameters [init|start|stop|show|[-]enqueue [n]]\n"
#ifndef COMPILER_STRING_CONST_LIMIT
    "With no parameters, show whether or not active.\n"
    "    init:           Initialize the RX API, but don't register handler\n"
    "    start:          Call RX start with local pkt dump routine\n"
    "                    Modify the configuration with the rxcfg command\n"
    "    stop:           Call RX stop\n"
    "    [-]enqueue [n]: Enqueue packets to be freed later in thread\n"
    "                    If n > 0, enqueue at least n pkts before freeing\n"
    "    stop:           Call RX stop\n"
    "    show:           Call RX show\n"
#endif
    ;
extern cmd_result_t cmd_esw_rx_mon(int unit, args_t *a);

char cmd_esw_show_usage[] =
    "Usages:\n"
#ifdef COMPILER_STRING_CONST_LIMIT
    "  show <args>\n"
#else
    "  show Pci        - Probe and display function 0 of all busses/devices\n"
    "  show CHips      - Show all driver-supported device IDs\n"
    "  show Counters [Changed] [Same] [Z] [NZ] [Hex] [Raw] [<reg>][.][<pbmp>]\n"
    "\tDisplay all counters, or only specified regs and/or ports\n"
    "\t  Changed - include counters that changed\n"
    "\t  Same    - include counters that did not change\n"
    "\t  Z       - include counters that are zero\n"
    "\t  Nz      - include counters that are not zero\n"
    "\t  All     - same as: Changed Same Z Nz\n"
    "\t  Hex     - display counters as 64-bit hex values\n"
    "\t  Raw     - display raw 64-bit hex values, no register name(s)\n"
    "\t  ErDisc  - Only show those counters marked with Error/Discard\n"
    "\tNOTES: If neither Changed or Same is specified, Change is used.\n"
    "\t       If neither Z or Nz is specified, Nz is used.\n"
    "\t       Use All to display counters regardless of value.\n"
    "  show Statistics [pbm] [all] - SNMP accumulated statistics, all shows 0s\n"
    "  show <mem>             - memory info (\"listmem\" for list)\n"
    "  show sa                - software ARL table\n"
    "  show Errors            - logged error counts for certain errors\n"
    "  show Interrupts        - interrupt counts for most interrupt types\n"
    "  show params [<chip>]   - Chip parameters (chip id or current unit)\n"
    "  show unit [<unit>]     - Unit list or unit parameters\n"
    "  show features [all]    - Show enabled (or all) features for this unit\n"
#if defined(VXWORKS)
    "  show ip                - IP statistics\n"
    "  show icmp              - ICMP statistics\n"
    "  show arp               - ARP statistics\n"
    "  show udp               - UDP statistics\n"
    "  show tcp               - TCP statistics\n"
    "  show mux               - MUX protocol stack backplane\n"
    "  show routes            - IP routing table\n"
    "  show hosts             - IP host table\n"
#endif /* VXWORKS */
#endif
    ;
extern cmd_result_t cmd_esw_show(int unit, args_t *a);

char cmd_esw_soc_usage[] =
    "Parameters: [<unit #>] ... \n\t"
    "Print internal SOC driver control information IF compiled as a \n\t"
    "debug version. If not compiled as a debug version, a warning is\n\t"
    "printed and the command completes successfully with no further\n\t"
    "output\n";
extern cmd_result_t cmd_esw_soc(int unit, args_t *a);

char if_esw_stg_usage[] =
    "Usages:\n\t"
#ifdef COMPILER_STRING_CONST_LIMIT
    "  stg <option> [args...]\n"
#else
    "  stg create [<id>]            - Create a STG; optionally specify ID\n\t"
    "  stg destroy <id>             - Destroy a STG\n\t"
    "  stg show [<id>]              - List STG(s)\n\t"
    "  stg add <id> <vlan_id> [...]     - Add VLAN(s) to a STG\n\t"
    "  stg remove <id> <vlan_id> [...]  - Remove VLAN(s) from a STG\n\t"
    "  stg stp                      - Get span tree state, all ports/STGs\n\t"
    "  stg stp <id>                 - Get span tree state of ports in STG\n\t"
    "  stg stp <id> <pbmp> <state>  - Set span tree state of ports in STG\n\t"
    "                                 (disable/block/listen/learn/forward)\n\t"
    "  stg default [<id>]           - Show or set the default STG\n"
#endif
    ;
extern cmd_result_t if_esw_stg(int unit, args_t *a);

char if_esw_trunk_usage[] =
    "Usages:\n\t"
#ifdef COMPILER_STRING_CONST_LIMIT
    "  trunk <option> [args...]\n"
#else
    "  trunk init\n\t"
    "        - Initialize trunking function\n\t"
    "  trunk deinit\n\t"
    "        - Deinitialize trunking function\n\t"
    "  trunk add <Id=val> <Rtag=val> <Pbmp=val>\n\t"
    "        - Add ports to a trunk\n\t"
    "  trunk remove <Id=val> <Pbmp=val>\n\t"
    "        - Remove ports from a trunk\n\t"
    "  trunk show [<Id=val>]\n\t"
    "        - Display trunk information\n\t"
    "  trunk egress [<Id=val>] <Pbmp=val>\n\t"
    "        - Set egress ports for trunk\n\t"
    "  trunk mcast <Id=val> <Mac=val> <Vlan=val>\n\t"
    "        - Join multicast to a trunk\n\t"
#ifdef BCM_FIREBOLT_SUPPORT
    "  trunk hash set <Pbmp=val> <HashValue=val>\n\t"
    "        - Set ingress port hash value to select egress port of a trunk\n\t"
    "  trunk hash get <Pbmp=val>\n\t"
    "        - Get ingress port programmable hash value\n\t"
#endif /* BCM_FIREBOLT_SUPPORT */
    "  trunk psc <Id=val> <Rtag=val>\n\t"
    "        - Change Rtag (for testing ONLY)\n"
#endif
    ;
extern cmd_result_t if_esw_trunk(int unit, args_t *a);

char cmd_esw_tx_usage[] =
    "Parameters: <Count> [options]\n"
#ifndef COMPILER_STRING_CONST_LIMIT
    "  Transmit the specified number of packets, if the contents of the\n"
    "  packets is important, they may be specified using options.\n"
    "  Supported options are:\n"
    "      Untagged[yes/no]    - Specify packet should be sent untagged(XGS3)\n"
    "      TXUnit=<val>        - Transmit unit number\n"
    "      PortBitMap=<pbmp>   - Specify port bitmap packet is sent to.\n"
    "      UntagBitMap=<pbmp>  - Specify untag bitmap used for DMA.\n"
    "      File=<filename>     - Load hex packet data from file and ignore\n"
    "                            various other pattern parameters below.\n"
    "      Length=<value>      - Specify the total length of the packet,\n"
    "                            including header, possible tag, and CRC.\n"
    "      VLantag=<value>     - Specify the VLAN tag used, only the low\n"
    "                            order 16-bits are used (VLANID=0 for none)\n"
    "      VlanPrio            - VLAN Priority.\n"
    "      PrioInt             - Internal Priority.\n"
    "      Pattern=<value>     - Specify 32-bit data pattern used.\n"
    "      PatternInc=<value>  - Value by which each word of the data\n"
    "                            pattern is incremented\n"
    "      PatternRandom[yes/no] - Use Random data pattern\n"
    "      PerPortSrcMac=[0|1] - Associate specific (different) src macs\n"
    "                            with each source port.\n"
    "      SourceMac=<value>   - Source MAC address in packet\n"
    "      SourceMacInc=<val>  - Source MAC increment\n"
    "      DestMac=<value>     - Destination MAC address in packet.\n"
    "      DestMacInc=<value>  - Destination MAC increment.\n"
    "      PurGe[yes/no]       - Send packet with purge Indication(XGS3)\n"
    "\n"
    "  Header Format-specific options (see HeaderMode command):\n"
    "  - HiGig:\n"
    "      HGOpcode=<val>      - HiGig header opcode\n"
    "      HGSrcMod=<val>      - Source Module ID\n"
    "      HGSrcPort=<val>     - Source Port (numeric value only)\n"
    "      HGDestMod=<val>     - Destination Module ID\n"
    "      HGDestPort=<val>    - Destination Port (numeric value only)\n"
    "      PFM=<val>           - Port Filtering Mode (0, 1, 2)\n"
#ifdef BCM_HIGIG2_SUPPORT
    "  - HiGig2 only:\n"
    "      TrafficClass=<val>  - Traffic Class (0-15)\n"
    "      McastGroupID=<val>  - Multicast Group ID\n"
    "      LoadBalID=<value>   - Load Balancing ID\n"
    "      DropPrecedence=<c>  - Drop Precendence (green/red/yellow)\n"
#endif
#endif
    ;
extern cmd_result_t cmd_esw_tx(int unit, args_t *a);

char cmd_esw_tx_count_usage[] =
    "Parameters: None\n\t"
    "Print current request count and set count values for an active\n\t"
    "TXSTART command.\n";

extern cmd_result_t cmd_esw_tx_count(int unit, args_t *a);
extern cmd_result_t cmd_esw_tx_start(int unit, args_t *a);


char cmd_esw_tx_stop_usage[] =
    "Parameters: None\n\t"
    "Terminate a background TXSTART command that is currently running.\n\t"
    "This command only requests termination, the background thread\n\t"
    "looks for termination requests BETWEEN sending packets.\n";
extern cmd_result_t cmd_esw_tx_stop(int unit, args_t *a);


char if_esw_vlan_usage[] =
    "Usages:\n\t"
#ifdef COMPILER_STRING_CONST_LIMIT
    "  vlan <option> [args...]\n"
#else
    "  vlan create <id> [PortBitMap=<pbmp> UntagBitMap=<pbmp>]\n\t"
    "                                       - Create a VLAN\n\t"
    "  vlan destroy <id>                    - Destroy a VLAN\n\t"
    "  vlan clear                           - Destroy all VLANs\n\t"
    "  vlan add <id> [PortBitMap=<pbmp> UntagBitMap=<pbmp>\n\t"
    "                                       - Add port(s) to a VLAN\n\t"
    "  vlan remove <id> [PortBitMap=<pbmp>] - Remove ports from a VLAN\n\t"
    "  vlan MulticastFlood <id>  [Mode]     - Multicast flood setting\n\t"
    "  vlan show                            - Display all VLANs\n\t"
    "  vlan default [<id>]                  - Show or set the default VLAN\n\t"
    "  vlan protocol add PortBitMap=<pbmp> Frame=<N> Ether=<N>\n\t"
    "        VLan=<vlanid> Prio=<prio>\n\t"
    "  vlan protocol delete PortBitMap=<pbmp> Frame=<N> Ether=<N>\n\t"
    "  vlan protocol clear PortBitMap=<pbmp>\n\t"
    "  vlan mac add MACaddress=<address> VLan=<vlanid> Prio=<prio>\n\t"
    "        Cng=<cng>\n\t"
    "  vlan mac delete MACaddress=<address>\n\t"
    "  vlan mac clear\n\t"
    "  vlan translate [On|Off]\n\t"
    "  vlan translate add Port=<port> OldVLan=<vlanid> NewVLan=<vlanid>\n\t"
    "        Prio=<prio> Cng=<cng>\n\t"
    "  vlan translate get Port=<port> OldVLan=<vlanid> \n\t"
    "  vlan translate show              - Shows all vlan translations \n\t"
    "  vlan translate delete Port=<port> OldVLan=<vlanid>\n\t"
    "  vlan translate clear\n\t"
    "  vlan translate egress add Port=<port> OldVLan=<vlanid> NewVLan=<vlanid>\n\t"
    "        Prio=<prio> Cng=<cng>\n\t"
    "  vlan translate egress get Port=<port> OldVLan=<vlanid> \n\t"
    "  vlan translate egress show  - Shows all vlan egress translations \n\t"
    "  vlan translate egress delete Port=<port> OldVLan=<vlanid>\n\t"
    "  vlan translate egress clear\n\t"
    "  vlan translate dtag add Port=<port> OldVLan=<vlanid> NewVLan=<vlanid>\n\t"
    "        Prio=<prio> Cng=<cng>\n\t"
    "  vlan translate dtag get Port=<port> OldVLan=<vlanid> \n\t"
    "  vlan translate dtag show     - Shows all double tagged vlans \n\t"
    "  vlan translate range add Port=<port> OldVLanHi=<vlanid> \n\t"
    "        OldVlanLo=<vlanid> NewVLan=<vlanid> Prio=<prio> Cng=<cng>\n\t"
    "  vlan translate range get Port=<port> OldVLanHi=<vlanid>\n\t"
    "        OldVlanLo=<vlanid>\n\t"
    "  vlan translate range delete Port=<port> OldVLanHi=<vlanid>\n\t"
    "        OldVlanLo=<vlanid>\n\t"
    "  vlan translate range clear\n\t"
    "  vlan translate range show\n\t"
    "  vlan ip4 add IPaddr=<ip-address> NetMask=<ip-addr> VLan=<vlanid>\n\t"
    "        Prio=<prio> Cng=<cng>\n\t"
    "  vlan ip4 delete IPaddr=<ip-address> NetMask=<ip-addr>\n\t"
    "  vlan ip4 clear\n\t"
    "  vlan ip6 add IPaddr=<ip-address> prefiX=<prefix> VLan=<vlanid>\n\t"
    "        Prio=<prio>\n\t"
    "  vlan ip6 delete IPaddr=<ip-address> prefiX=<prefix>\n\t"
    "  vlan ip6 clear\n\t"
    "  vlan control <name> <value>\n\t"
    "  vlan port <port> <value>\n\t"
    "  vlan innertag <port> Priority=<priority> CFI=<cfi> VLan=<vlanid>\n\t"
    "  vlan <id> <name>=<vlaue>         - Set/Get per VLAN property\n\t"
    "  vlan action port default add Port=<port> OuterVlan=<vlanid>\n\t"
    "        InnerVlan=<vlanid> Prio=<prio> DtOuter=<action>\n\t"
    "        DtOuterPrio=<action> DtInner=<action> DtInnerPrio=<action>\n\t"
    "        OtOuter=<action> OtOuterPrio=<action> OtInner=<action>\n\t"
    "        OtOuter=<action> ItOuter=<action> ItInner=<action>\n\t"
    "        ItInnerPrio=<action> UtOuter=<action> UtInner=<action>\n\t"
    "        - Add port default VLAN tag with actions.\n\t"
    "          Valid actions are { Add, Delete, Replace, None }\n\t"
    "  vlan action port default get Port=<port>\n\t"
    "        - Get port default VLAN tag actions\n\t"
    "  vlan action port egress default add Port=<port> OuterVlan=<vlanid>\n\t"
    "        InnerVlan=<vlanid> Prio=<prio> DtOuter=<action>\n\t"
    "        DtOuterPrio=<action> DtInner=<action> DtInnerPrio=<action>\n\t"
    "        OtOuter=<action> OtOuterPrio=<action> OtInner=<action>\n\t"
    "        OtOuter=<action> ItOuter=<action> ItInner=<action>\n\t"
    "        ItInnerPrio=<action> UtOuter=<action> UtInner=<action>\n\t"
    "        - Add port egress VLAN tag with actions.\n\t"
    "          Valid actions are { Add, Delete, Replace, None }\n\t"
    "  vlan action port egress default get Port=<port>\n\t"
    "        - Get port egress VLAN tag actions\n\t"
    "  vlan action mac add MACaddress=<address> OuterVlan=<vlanid>\n\t"
    "        InnerVlan=<vlanid> Prio=<prio> DtOuter=<action>\n\t"
    "        DtOuterPrio=<action> DtInner=<action> DtInnerPrio=<action>\n\t"
    "        OtOuter=<action> OtOuterPrio=<action> OtInner=<action>\n\t"
    "        OtOuter=<action> ItOuter=<action> ItInner=<action>\n\t"
    "        ItInnerPrio=<action> UtOuter=<action> UtInner=<action>\n\t"
    "        - Add MAC-based VLAN tag with actions\n\t"
    "          Valid actions are { Add, Delete, Replace, None }\n\t"
    "  vlan action mac get MACaddress=<address>\n\t"
    "        - Get MAC-based VLAN tag actions\n\t"
    "  vlan action mac delete MACaddress=<address>\n\t"
    "        - Delete MAC-based VLAN tag actions\n\t"
    "  vlan action mac clear\n\t"
    "        - Delete all MAC-based VLAN tag actions\n\t"
    "  vlan action protocol add PortBitMap=<pbmp> Frame=<N> Ether=<N>\n\t"
    "        OuterVlan=<vlanid> InnerVlan=<vlanid> Prio=<prio> \n\t"
    "        DtOuter=<action> DtOuterPrio=<action> DtInner=<action> \n\t"
    "        DtInnerPrio=<action> OtOuter=<action> OtOuterPrio=<action>\n\t"
    "        OtInner=<action> OtOuter=<action> ItOuter=<action>\n\t"
    "        ItInner=<action> ItInnerPrio=<action> UtOuter=<action> \n\t"
    "        UtInner=<action>\n\t"
    "        - Add protocol-based VLAN tag with actions.\n\t"
    "          Valid actions are { Add, Delete, Replace, None }\n\t"
    "  vlan action ip4 add IPaddr=<ip-address> NetMask=<ip-addr>\n\t"
    "        OuterVlan=<vlanid> InnerVlan=<vlanid> Prio=<prio> \n\t"
    "        DtOuter=<action> DtOuterPrio=<action> DtInner=<action> \n\t"
    "        DtInnerPrio=<action> OtOuter=<action> OtOuterPrio=<action>\n\t"
    "        OtInner=<action> OtOuter=<action> ItOuter=<action>\n\t"
    "        ItInner=<action> ItInnerPrio=<action> UtOuter=<action> \n\t"
    "        UtInner=<action>\n\t"
    "        - Add IP4-based VLAN tag with actions.\n\t"
    "          Valid actions are { Add, Delete, Replace, None }\n\t"
    "  vlan action ip6 add IPaddr=<ip-address> prefiX=<prefix>\n\t"
    "        OuterVlan=<vlanid> InnerVlan=<vlanid> Prio=<prio> \n\t"
    "        DtOuter=<action> DtOuterPrio=<action> DtInner=<action> \n\t"
    "        DtInnerPrio=<action> OtOuter=<action> OtOuterPrio=<action>\n\t"
    "        OtInner=<action> OtOuter=<action> ItOuter=<action>\n\t"
    "        ItInner=<action> ItInnerPrio=<action> UtOuter=<action> \n\t"
    "        UtInner=<action>\n\t"
    "        - Add IP6-based VLAN tag with actions.\n\t"
    "          Valid actions are { Add, Delete, Replace, None }\n\t"
    "  vlan action translate add Port=<port> KeyType=<key> \n\t"
    "        OldOuterVlan=<vlanid> OldInnerVlan=<vlanid> DtOuter=<action>\n\t"
    "        OuterVlan=<vlanid> InnerVlan=<vlanid> Prio=<prio> \n\t"
    "        DtOuter=<action> DtOuterPrio=<action> DtInner=<action>\n\t"
    "        DtInnerPrio=<action> OtOuter=<action> OtOuterPrio=<action>\n\t"
    "        OtInner=<action> OtOuter=<action> ItOuter=<action>\n\t"
    "        ItInner=<action> ItInnerPrio=<action> UtOuter=<action>\n\t"
    "        UtInner=<action>\n\t"
    "        - Add VLAN tag translation with actions.\n\t"
    "          Valid actions are { Add, Delete, Replace, None }\n\t"
    "          Valid key types are { Double, Outer, Inner, OuterTag, \n\t"
    "          InnerTag, PortDouble, PortOuter }\n\t"
    "  vlan action translate delete Port=<port> KeyType=<key> \n\t"
    "        OldOuterVlan=<vlanid> OldInnerVlan=<vlanid>\n\t"
    "        - Delete VLAN tag translation actions.\n\t"
    "  vlan action translate get Port=<port> KeyType=<key> \n\t"
    "        OldOuterVlan=<vlanid> OldInnerVlan=<vlanid>\n\t"
    "        - Get VLAN tag translation actions.\n\t"
    "  vlan action translate show\n\t"
    "        - Show all VLAN tag translation actions.\n\t"
    "  vlan action translate range add Port=<port> \n\t"
    "        OuterVlanLo=<vlanid> OuterVlanHi=<vlanid> \n\t"
    "        InnerVlanLo=<vlanid> InnerVlanHi=<vlanid>\n\t"
    "        OuterVlan=<vlanid> InnerVlan=<vlanid> Prio=<prio> \n\t"
    "        DtOuter=<action> DtOuterPrio=<action> DtInner=<action>\n\t"
    "        DtInnerPrio=<action> OtOuter=<action> OtOuterPrio=<action>\n\t"
    "        OtInner=<action> OtOuter=<action> ItOuter=<action>\n\t"
    "        ItInner=<action> ItInnerPrio=<action> UtOuter=<action>\n\t"
    "        UtInner=<action>\n\t"
    "        - Add VLAN tag range translation with actions.\n\t"
    "          Valid actions are { Add, Delete, Replace, None }\n\t"
    "  vlan action translate range delete Port=<port> \n\t"
    "        OuterVlanLo=<vlanid> OuterVlanHi=<vlanid> \n\t"
    "        InnerVlanLo=<vlanid> InnerVlanHi=<vlanid>\n\t"
    "        - Delete VLAN tag translation range actions.\n\t"
    "  vlan action translate range get Port=<port> \n\t"
    "        OuterVlanLo=<vlanid> OuterVlanHi=<vlanid> \n\t"
    "        InnerVlanLo=<vlanid> InnerVlanHi=<vlanid>\n\t"
    "        - Get VLAN tag translation range actions.\n\t"
    "  vlan action translate range clear\n\t"
    "        - Delete all VLAN tag translation range actions.\n\t"
    "  vlan action translate range show\n\t"
    "        - Show all VLAN tag translation range actions.\n\t"
    "  vlan action translate egress add PortClass=<class> \n\t"
    "        OldOuterVlan=<vlanid> OldInnerVlan=<vlanid> DtOuter=<action>\n\t"
    "        OuterVlan=<vlanid> InnerVlan=<vlanid> Prio=<prio> \n\t"
    "        DtOuter=<action> DtOuterPrio=<action> DtInner=<action>\n\t"
    "        DtInnerPrio=<action> OtOuter=<action> OtOuterPrio=<action>\n\t"
    "        OtInner=<action> OtOuter=<action> ItOuter=<action>\n\t"
    "        ItInner=<action> ItInnerPrio=<action> UtOuter=<action>\n\t"
    "        UtInner=<action>\n\t"
    "        - Add VLAN tag egress translation with actions.\n\t"
    "          Valid actions are { Add, Delete, Replace, None }\n\t"
    "  vlan action translate egress delete PortClass=<class> \n\t"
    "        OldOuterVlan=<vlanid> OldInnerVlan=<vlanid>\n\t"
    "        - Delete VLAN tag egress translation actions.\n\t"
    "  vlan action translate egress get PortClass=<class> \n\t"
    "        OldOuterVlan=<vlanid> OldInnerVlan=<vlanid>\n\t"
    "        - Get VLAN tag egress translation actions.\n\t"
    "  vlan action translate egress show\n"
#endif
    ;
extern cmd_result_t if_esw_vlan(int unit, args_t *a);


char cmd_esw_mem_write_usage[] =
    "Parameters: <TABLE>[.<COPY>] <ENTRY> <ENTRYCOUNT>\n\t"
    "        { <DW0> .. <DWN> | <FIELD>=<VALUE>[,...] }\n\t"
    "Number of <DW> must be a multiple of table entry size.\n\t"
    "Writes entry(s) into table index(es).\n";
extern cmd_result_t cmd_esw_mem_write(int unit, args_t *a);

char cmd_esw_eav_usage[] =
#ifdef COMPILER_STRING_CONST_LIMIT
"Parameters: <cmd> <opt>\n"
#else /* !COMPILER_STRING_CONST_LIMIT */
" Where <cmd> is:\n"
"\tcontrol set|get <ctrl_type> <parameter>\n"
"\tmac get|set <mac value>\n"
"\ttypes. (Describe the types of Ethernet AV CLI commands)\n"
"\tinit\n"
"\tport enable|disable <port number>\n"
"\twatch start|stop\n"
"\tstatus\n"
"\ttimestamp <port number>\n"
"\ttx <pbmp> <vlanid>\n"
"\tsrp set|get <mac value> <ethertype>\n"
"\tbandwidth set|get <port number> <class> <bandwidth> <burstsize>\n"
"\tpcp set|get <class> <pcp> <remapped_pcp>\n"
#endif /* !COMPILER_STRING_CONST_LIMIT */
;
extern cmd_result_t cmd_esw_eav(int unit, args_t *a);



