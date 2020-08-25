/* General macros for signals accessing */
int MAX_NUM_OF_INTS_IN_SIG_GETTER = 2000;
int BITS_IN_WORD = 32;

/* Macros for GRE encapsulation protocol type field */
int GRE_PROTOCOL_TYPE_IPV4 = 0x0800;
int GRE_PROTOCOL_TYPE_IPV6 = 0x86DD;
int GRE_PROTOCOL_TYPE_MPLS = 0x8847;

int PETRA         = 0x88640;
int ARAD          = 0x88650;
int ARAD_PLUS     = 0x88660;
int ARDON         = 0x88202;
int JERICHO       = 0x88675;
int JERICHO_B0    = 0x1188675; /*11 is the revision*/
int QUMRAN_MX     = 0x88375;
int QUMRAN_MX_B0  = 0x1188375; /*11 is the revision*/
int JERICHO_PLUS  = 0x88680;
int QUMRAN_AX     = 0x88470;
int QUMRAN_AX_B0  = 0x1188470;   /*11 is the revision*/
int JERICHO2      = 0x88690;
int JERICHO2_B0   = 0x1188690;  /*11 is the revision*/
int QUMRAN_UX     = 0x88270;

/** General device list - devices must be chronologically ordered.
    New devices should be added at the end */

int DEVICE_LIST_SIZE = 14;
int DEVICE_LIST [DEVICE_LIST_SIZE] = {0};
DEVICE_LIST[0] = PETRA;
DEVICE_LIST[1] = ARAD;
DEVICE_LIST[2] = ARAD_PLUS;
DEVICE_LIST[3] = ARDON;
DEVICE_LIST[4] = JERICHO;
DEVICE_LIST[5] = QUMRAN_MX;
DEVICE_LIST[6] = JERICHO_B0;
DEVICE_LIST[7] = QUMRAN_MX_B0;
DEVICE_LIST[8] = JERICHO_PLUS;
DEVICE_LIST[9] = QUMRAN_AX;
DEVICE_LIST[10] = QUMRAN_AX_B0;
DEVICE_LIST[11] = QUMRAN_UX;
DEVICE_LIST[12] = JERICHO2;
DEVICE_LIST[13] = JERICHO2_B0;

/*Global variable for Section OAM */
/*AC LIF used for Section OAM */
bcm_gport_t section_oam_in_gport = 0;
/*Section OAM indication */
int is_section_oam = 0;

/** Returns TRUE if the device is Arad*/
int
is_arad_only(
    int unit,
    int *is_arad)
{
    bcm_info_t info;
    int rv = bcm_info_get(unit, &info);
    {
        printf("Error in bcm_info_get\n");
        return rv;
    }
    *is_arad = (info.device == 0x8650) ? 1 : 0;
    return rv;
}
/** Returns TRUE if the device is Arad A0*/
int
is_arad_a0_only(
    int unit,
    int *is_arad_a0)
{
    bcm_info_t info;
    int rv = bcm_info_get(unit, &info);
    {
        printf("Error in bcm_info_get\n");
        return rv;
    }
    *is_arad_a0 = (info.device == 0x8650 &
             info.revision == 0x0) ? 1 : 0;

    return rv;
}
/** Returns TRUE if the device is Arad B0*/
int
is_arad_b0_only(
    int unit,
    int *is_arad_b0)
{
    bcm_info_t info;
    int rv = bcm_info_get(unit, &info);
    {
        printf("Error in bcm_info_get\n");
        return rv;
    }
    *is_arad_b0 = (info.device == 0x8650 &
             info.revision == 0x11) ? 1 : 0;
    return rv;
}
/** Returns TRUE if the device is Arad B1*/
int
is_arad_b1_only(
    int unit,
    int *is_arad_b1)
{
    bcm_info_t info;
    int rv = bcm_info_get(unit, &info);
    {
        printf("Error in bcm_info_get\n");
        return rv;
    }
    *is_arad_b1 = (info.device == 0x8650 &
             info.revision == 0x12) ? 1 : 0;
    return rv;
}
/** Returns TRUE if the device is Arad Plus*/
int
is_arad_plus_only(
    int unit,
    int *is_arad_plus)
{
    bcm_info_t info;
    int rv = BCM_E_NONE;

    rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_info_get\n");
        print rv;
        return rv;
    }

    *is_arad_plus = (info.device == 0x8660) ? 1 : 0;
    return rv;
}
/** Returns TRUE if the device is QAX*/
int
is_device_qax_only(
    int unit,
    int *is_qax)
{
    bcm_info_t info;
    int rv;
    rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_info_get\n");
        return rv;
    }
    *is_qax = (info.device == 0x8470)? 1: 0;
    return rv;
}
/** Returns TRUE if the device is QAX or QUX*/
int
is_qax(
    int unit,
    int *is_qax)
{
    bcm_info_t info;
    int rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_info_get\n");
        return rv;
    }
    is_qax = (info.device == 0x8470 || info.device == 0x8270) ? 1 : 0;
    return rv;
}
/** Returns TRUE if the device is QUX*/
int
is_qux_only(
    int unit,
    int *is_qux)
{
    bcm_info_t info;
    int rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_info_get\n");
        return rv;
    }
    *is_qux = (info.device == 0x8270) ? 1 : 0;
    return rv;
}
/** Returns TRUE if the device is QMX*/
int
is_qmx_only(
    int unit,
    int *is_qmx)
{
    bcm_info_t info;
    int rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_info_get\n");
        return rv;
    }
    *is_qmx = (info.device == 0x8375) ? 1 : 0;
    return rv;
}
/** Returns TRUE if the device is QMX B0*/
int
is_qmx_b0_only(
    int unit,
    int *is_qmx_b0)
{
    bcm_info_t info;

    int rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_info_get\n");
        return rv;
    }
    *is_qmx_b0 = (info.device == 0x8375 &
             info.revision == 0x11) ? 1 : 0;

    return rv;
}
/** Returns TRUE if the device is Jericho*/
int
is_device_jericho_only(
    int unit,
    int *is_jer)
{
    bcm_info_t info;
    int rv;
    rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_info_get\n");
        return rv;
    }
    *is_jer = (info.device == 0x8675)? 1: 0;
    return rv;
}
/** Returns TRUE if the device is Jericho B0*/
int
is_jericho_b0_only(
    int unit,
    int *is_jericho_b0)
{
    bcm_info_t info;
    int rv = bcm_info_get(unit, &info);
    rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE)
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_info_get\n");
        return rv;
    }
    *is_jericho_b0 = (info.device == 0x8675 &
             info.revision == 0x11) ? 1 : 0;
    return rv;
}
/** Returns TRUE if the device is Jericho Plus*/
int
is_device_jericho_plus_only(
    int unit,
    int *is_jer_plus)
{
    bcm_info_t info;
    int rv;

    rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_info_get\n");
        return rv;
    }

    *is_jer_plus = (info.device == 0x8680)? 1: 0;
    return rv;
}
/** Returns TRUE if the device is Ramon*/
int
is_ramon_only(
    int unit,
    int *is_ramon)
{
    bcm_info_t info;
    int rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_info_get\n");
        return rv;
    }
    *is_ramon = (info.device == 0x8790) ? 1 : 0;
    return rv;
}
/** Returns TRUE if the device is Jericho 2*/
int
is_device_jericho2(
    int unit,
    int *is_jericho2)
{
    int rv = BCM_E_NONE;
    char *proc_name;

    proc_name = "is_device_jericho2";
    bcm_info_t info;
    rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error in bcm_info_get \n", proc_name);
        return rv;
    }
    *is_jericho2 = (info.device == 0x8690) ? 1 : 0;
    return rv;
}
/** Returns TRUE if the device is Jericho 2*/
int
is__jericho2_a0_only(
    int unit,
    int *is_jericho2_a0)
{
    int rv = BCM_E_NONE;
    char *proc_name;

    proc_name = "is_device_jericho2";
    bcm_info_t info;
    rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error in bcm_info_get \n", proc_name);
        return rv;
    }
    *is_jericho2_a0 = ((info.device == 0x8690) && (info.revision == 0x1))  ? 1 : 0;
    return rv;
    return rv;
}

int
is_device_or_above(
    int unit,
    int supported_device_id)
{
    bcm_info_t info;
    int i;
    int current_device_id;
    int current_device_index;
    int supported_device_index = DEVICE_LIST_SIZE;

    int rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_info_get\n");
        return rv;
    }
    current_device_id = info.device;
    for (i=0;i<DEVICE_LIST_SIZE;i++)
    {
        if (((DEVICE_LIST[i] & 0xffff) == current_device_id) && (info.revision >= (DEVICE_LIST[i] >> 20)))
        {
            current_device_index = i;
        }
        if (DEVICE_LIST[i] == supported_device_id)
        {
            supported_device_index = i;
        }
    }
    return current_device_index >= supported_device_index;
}
/*
* Utility function, which differentiate between FE3200/1600 and Ramon devices.
* Input parameters:
* unit - relevant unit;
* result - the returned result from the function
*/
int
is_device_ramon(
    int unit,
    int *result)
{
    int rv = BCM_E_NONE;
    char *proc_name;

    proc_name = "is_device_ramon";
    bcm_info_t info;

    *result = 0;

    rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error in bcm_info_get \n", proc_name);
        return rv;
    }

    if (info.device == 0x8790)
    {
        *result = 1;
    }

    return rv;
}

/** Returns TRUE if the device is JERICHO2 or bcm886xx_vlan_translate_mode is enabled.*/
int is_advanced_vlan_translation_mode(int unit)
{
    if (is_device_or_above(unit, JERICHO2)) {
        return TRUE;
    } else {
        return soc_property_get(unit, "bcm886xx_vlan_translate_mode", 0);
    }
}

/**
* Function: return 16 bits part of a sinal
* Purpose: return a signal (16 bits part of a signal), used for a TCL function that returns the full signal
* (as this is used by TCL the possible return value is int so only 16 bits)
*
* Params:
* unit      - Unit id
* core      - Core id
* block     - Block name
* from      - Stage/Memory signal originated from or passed through
* to        - Stage/Memory signal destined to
* name      - Signal`s name
* */
int
get_adapter_signal_values_16bit(
    int unit,
    int core,
    char *block,
    char *from,
    char *to,
    char *name,
    int pos_in_16)
{
    uint32 value[MAX_NUM_OF_INTS_IN_SIG_GETTER] = { 0 };
    int pos_in_32 = pos_in_16 / 2;

    /*
     * Get the signal from the adapter
     */
    dpp_dsig_read(unit, core, block, from, to, name, value, MAX_NUM_OF_INTS_IN_SIG_GETTER);

    return ((value[pos_in_32] >> ((pos_in_16 & 0x1) * 16)) & 0xFFFF);
}
/*
 * Return the max single length in bytes
 */
int
get_adapter_signal_max_length_in_byte()
{
    return MAX_NUM_OF_INTS_IN_SIG_GETTER;
}



int
compare_adapter_signal_values(
    int unit,
    int in_port,
    int out_port,
    char *block,
    char *from,
    char *to,
    char *name,
    char *sig_exp_value)
{
    char *proc_name;
    int ret;

    proc_name="compare_adapter_signal_values";
    printf("%s(): %s %s %s, name %s exp %s\r\n",proc_name,block,from,to,name,sig_exp_value);
    ret = compare_adapter_signal_values_inner(unit, in_port, out_port , block, from, to, name, sig_exp_value, 1);
    printf("%s(): ret %d\r\n",proc_name,ret);
    return (ret);
}

/*
 * Extract core from input local port. On fail, return '-1'
 */
int
get_core_and_tm_port_from_port(
    int unit,
    bcm_gport_t gport,
    int * core,
    int * tm_port)
{
    int rv = BCM_E_NONE;
    uint32 dummy_flags;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;

    rv = bcm_port_get(unit, gport, &dummy_flags, &interface_info, &mapping_info);
    if (BCM_FAILURE(rv))
    {
        printf("bcm_port_get failed: port %d\n", gport);
        return rv;
    }
    else
    {
        *core = mapping_info.core;
        *tm_port = mapping_info.tm_port;
    }

    return rv;
}

/** This function accept a unit, port, and pointer to int, and fills it with system port appropriate for the unit-port combination.*/
int
port_to_system_port(
    int unit,
    int port,
    int *sysport)
{
    int modid, gport, rv, tm_port, core;
    if (BCM_GPORT_IS_SYSTEM_PORT(port))
    {
        *sysport = port;
        return BCM_E_NONE;
    }
    rv = bcm_stk_modid_get(unit, &modid);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_stk_modid_get\n");
        return rv;
    }
    if (is_device_or_above(unit, JERICHO))
    {
        /* Core should also be considered */
        rv = get_core_and_tm_port_from_port(unit,port,&core,&tm_port);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in get_core_and_tm_port_from_port\n");
            return rv;
        }
    }
    else
    {
        core = 0;
        tm_port = port;
    }
    BCM_GPORT_MODPORT_SET(gport, modid + core, tm_port);
    /* On JER2, Getting system port from modport is not supported, so set system port directly */
    if (!is_device_or_above(unit, JERICHO2)) {
        rv = bcm_stk_gport_sysport_get(unit, gport, sysport);
        if (rv != BCM_E_NONE){
            printf("Error, in bcm_stk_gport_sysport_get\n");
            return rv;
        }
    } else {
        BCM_GPORT_SYSTEM_PORT_ID_SET(*sysport, port);
        rv = bcm_stk_sysport_gport_set(unit, *sysport, gport);
        if (rv != BCM_E_NONE){
            printf("Error, in bcm_stk_sysport_gport_set\n");
            return rv;
        }
    }

    return BCM_E_NONE;
}

/** This function accept a unit, system port, and two pointers to int, fills with local port and is local or not.*/
int
system_port_is_local(int unit, int sysport, int *local_port, int *is_local){
    int rv = BCM_E_NONE;
    int my_modid, gport_modid, num_modid;
    int gport;

    *is_local = 0;

    if (!BCM_GPORT_IS_SET(sysport) || BCM_GPORT_IS_TRUNK(sysport))
    {
        *local_port = sysport;
        *is_local = 1;
        return BCM_E_NONE;
    }

    rv = bcm_stk_sysport_gport_get(unit, sysport, &gport);
    if (rv != BCM_E_NONE){
        printf("Error, in bcm_stk_sysport_gport_get\n");
        return rv;
    }
    printf("Sysport's unit%d  sysport is :%d,gport is: 0x%x\n",unit, sysport, gport);
    gport_modid = BCM_GPORT_MODPORT_MODID_GET(gport);
    *local_port = BCM_GPORT_MODPORT_PORT_GET(gport);
    printf("Required modid is: 0x%x\n", gport_modid);
    printf("Sysport's local port is: 0x%x\n", *local_port);
    rv = bcm_stk_modid_count(unit, &num_modid);
    if (rv != BCM_E_NONE){
        printf("Error, in bcm_stk_modid_count\n");
    }
    rv = bcm_stk_modid_get(unit, &my_modid);
    if (rv != BCM_E_NONE){
        printf("Error, in bcm_stk_modid_get\n");
    }

    /* By default, modids configured on a device are always consecutive, and the modid returned from bcm_stk_modid_get is always the lowest. */
    if ((gport_modid >= my_modid) && (gport_modid < (my_modid + num_modid))){
        *is_local = 1;
        printf("sysport 0x%x is a local port for unit %d\n", sysport, unit);
    } else {
        *is_local = 0;
        printf("sysport 0x%x is not a local port for unit %d\n", sysport, unit);
    }

    return rv;
}
/** This function accept a unit, gport, and two pointers to int, fills with local port and is local or not.*/
int
gport_is_local(int unit, int gport, int *local_port, int *is_local){
    int rv = BCM_E_NONE;
    int my_modid, gport_modid, num_modid;

    *is_local = 0;
    
    gport_modid = BCM_GPORT_MODPORT_MODID_GET(gport);
    *local_port = BCM_GPORT_MODPORT_PORT_GET(gport);
    printf("Required modid is: 0x%x\n", gport_modid);
    printf("gport(0x%x)'s local port is: 0x%x\n", gport, *local_port);
    rv = bcm_stk_modid_count(unit, &num_modid);
    if (rv != BCM_E_NONE){
        printf("Error, in bcm_stk_modid_count\n");
    }
    rv = bcm_stk_modid_get(unit, &my_modid);
    if (rv != BCM_E_NONE){
        printf("Error, in bcm_stk_modid_get\n");
    }

    /* By default, modids configured on a device are always consecutive, and the modid returned from bcm_stk_modid_get is always the lowest. */
    if ((gport_modid >= my_modid) && (gport_modid < (my_modid + num_modid))){
        *is_local = 1;
        printf("gport 0x%x is a local port for unit %d\n", gport, unit);
    } else {
        *is_local = 0;
        printf("gport 0x%x is not a local port for unit %d\n", gport, unit);
    }

    return rv;
}

