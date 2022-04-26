/*
 * cint;
 * cint_reset();
 * exit;
 * cint ../../src/examples/dnx/field/cint_field_ctest_clear.c
 *
 * Using this cint in "ctest field tx stage=ipmf1 class=metadata"
 * to clear the whole CINT declarations and to allow the test
 * to be run more than once.
 *
 */

int unit = 0;

l2_basic_bridge_arp_traps_destroy(unit);

bcm_vlan_destroy_all(unit);

cint_reset();
