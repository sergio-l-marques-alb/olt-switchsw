/*
 * Using this cint in "ctest field tx stage=ipmf1 class=metadata"
 * to parse some metadata from FER to PMFA through signals.
 * Like In_LIF_Profile, In_LIF, Forwarding info(dest, domain, trap_qual, trap_code).
 *
 */
int unit = 0;

l2_basic_bridge_main_run(unit, 200, 201, 202, 203);