/*
 *
 * Time - Broadcom StrataSwitch Time BroadSync API.
 */

#include <bcm/time.h>
#include <bcm/error.h>

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/time.h>
#include <bcm_int/esw_dispatch.h>


/****************************************************************************/
/*                      LOCAL VARIABLES DECLARATION                         */
/****************************************************************************/
static _bcm_time_config_p _bcm_time_config[BCM_MAX_NUM_UNITS] = {NULL};

static bcm_time_spec_t _bcm_time_accuracy_arr[TIME_ACCURACY_CLK_MAX] = {
      {0, 0, 25},        /* HW value = 32, accuracy up tp 25 nanosec */
      {0, 0, 100},       /* HW value = 33, accuracy up to 100 nanosec */
      {0, 0, 250},       /* HW value = 34, accuracy up to 250 nanosec */
      {0, 0, 1000},      /* HW value = 35, accuracy up to 1 microsec */
      {0, 0, 2500},      /* HW value = 36, accuracy up to 2.5 microsec */
      {0, 0, 10000},     /* HW value = 37, accuracy up to 10 microsec */
      {0, 0, 25000},     /* HW value = 38, accuracy up to 25 microsec */
      {0, 0, 100000},    /* HW value = 39, accuracy up to 100 microsec */
      {0, 0, 250000},    /* HW value = 40, accuracy up to 250 microsec */
      {0, 0, 1000000},   /* HW value = 41, accuracy up to 1 milisec */
      {0, 0, 2500000},   /* HW value = 42, accuracy up to 2.5 milisec */
      {0, 0, 10000000},  /* HW value = 43, accuracy up to 10 milisec */
      {0, 0, 25000000},  /* HW value = 44, accuracy up to 25 milisec */
      {0, 0, 100000000}, /* HW value = 45, accuracy up to 100 milisec */
      {0, 0, 250000000}, /* HW value = 46, accuracy up to 250 milisec */
      {0, 1, 0},         /* HW value = 47, accuracy up to 1 sec */
      {0, 10, 0},        /* HW value = 48, accuracy up to 10 sec */
      /* HW value = 49, accuracy greater than 10 sec */
      {0, TIME_ACCURACY_INFINITE, TIME_ACCURACY_INFINITE},         
      /* HW value = 254 accuracy unknown */
      {0, TIME_ACCURACY_UNKNOWN, TIME_ACCURACY_UNKNOWN}          
};

static uint32 _bcm_time_bs_supported_frequencies[TIME_BS_FREQUENCIES_NUM] = {
    TIME_BS_FREQUENCY_1000NS,
    TIME_BS_FREQUENCY_1024NS,
    TIME_BS_FREQUENCY_1544NS,
    TIME_BS_FREQUENCY_2000NS,
    TIME_BS_FREQUENCY_2048NS
};

/****************************************************************************/
/*                      Internal functions implmentation                    */
/****************************************************************************/
/*
 * Function:
 *	  _bcm_esw_time_hw_clear
 * Purpose:
 *	  Internal routine used to clear all HW registers and table to default values
 * Parameters:
 *    unit           - (IN) BCM device number.
 *    intf_id        - (IN) Time interface identifier
 * Returns:
 *	  BCM_E_XXX
 */
STATIC int 
_bcm_esw_time_hw_clear(int unit, bcm_time_if_t intf_id)
{
    uint32      regval;

    /* Reset Drift Rate  */
    READ_CMIC_BS_DRIFT_RATEr(unit, &regval);
    soc_reg_field_set(unit, CMIC_BS_DRIFT_RATEr, &regval, SIGNf, 0);
    soc_reg_field_set(unit, CMIC_BS_DRIFT_RATEr, &regval, FRAC_NSf, 0);
    WRITE_CMIC_BS_DRIFT_RATEr(unit, regval);

    /* Reset Offset Adjust */
    READ_CMIC_BS_OFFSET_ADJUST_0r(unit, &regval);
    soc_reg_field_set(unit, CMIC_BS_OFFSET_ADJUST_0r, &regval, SECONDf, 0);
    WRITE_CMIC_BS_OFFSET_ADJUST_0r(unit, regval);
    READ_CMIC_BS_OFFSET_ADJUST_1r(unit, &regval);
    soc_reg_field_set(unit, CMIC_BS_OFFSET_ADJUST_1r, &regval, SIGN_BITf, 0);
    soc_reg_field_set(unit, CMIC_BS_OFFSET_ADJUST_1r, &regval, NSf, 0);
    WRITE_CMIC_BS_OFFSET_ADJUST_1r(unit, regval);

    /* Reset Config register */
    READ_CMIC_BS_CONFIGr(unit, &regval);
    soc_reg_field_set(unit, CMIC_BS_CONFIGr, &regval, ENABLEf, 0);
    soc_reg_field_set(unit, CMIC_BS_CONFIGr, &regval, MODEf, TIME_MODE_OUTPUT);
    soc_reg_field_set(unit, CMIC_BS_CONFIGr, &regval, TIME_CODE_ENABLEf, 0);
    soc_reg_field_set(unit, CMIC_BS_CONFIGr, &regval, LOCKf, 0);
    soc_reg_field_set(unit, CMIC_BS_CONFIGr, &regval, ACCURACYf, 
                      TIME_ACCURACY_UNKNOWN_HW_VAL);
    soc_reg_field_set(unit, CMIC_BS_CONFIGr, &regval, EPOCHf, 0);
    WRITE_CMIC_BS_CONFIGr(unit, regval);

    /* Reset Clock Control */
    READ_CMIC_BS_CLK_CTRL_0r(unit, &regval);
    soc_reg_field_set(unit, CMIC_BS_CLK_CTRL_0r, &regval, ENABLEf, 0);
    soc_reg_field_set(unit, CMIC_BS_CLK_CTRL_0r, &regval, NSf, 0);
    WRITE_CMIC_BS_CLK_CTRL_0r(unit, regval);
    READ_CMIC_BS_CLK_CTRL_1r(unit, &regval);
    soc_reg_field_set(unit, CMIC_BS_CLK_CTRL_1r, &regval, FRAC_NSf, 0);
    WRITE_CMIC_BS_CLK_CTRL_1r(unit, regval);

    /* Reset Clock Toggle  */
    READ_CMIC_BS_CLK_TOGGLE_TIME_0r(unit, &regval);
    soc_reg_field_set(unit, CMIC_BS_CLK_TOGGLE_TIME_0r, &regval, SECf, 0);
    WRITE_CMIC_BS_CLK_TOGGLE_TIME_0r(unit, regval);
    READ_CMIC_BS_CLK_TOGGLE_TIME_1r(unit, &regval);
    soc_reg_field_set(unit, CMIC_BS_CLK_TOGGLE_TIME_1r, &regval, NSf, 0);
    WRITE_CMIC_BS_CLK_TOGGLE_TIME_1r(unit, regval);
    READ_CMIC_BS_CLK_TOGGLE_TIME_2r(unit, &regval);
    soc_reg_field_set(unit, CMIC_BS_CLK_TOGGLE_TIME_2r, &regval, FRAC_NSf, 0);
    WRITE_CMIC_BS_CLK_TOGGLE_TIME_2r(unit, regval);

    /* Reset HeartBeat */
    READ_CMIC_BS_HEARTBEAT_CTRLr(unit, &regval);
    soc_reg_field_set(unit, CMIC_BS_HEARTBEAT_CTRLr, &regval, ENABLEf, 0);
    soc_reg_field_set(unit, CMIC_BS_HEARTBEAT_CTRLr, &regval, THRESHOLDf, 0);
    WRITE_CMIC_BS_HEARTBEAT_CTRLr(unit, regval);

    /* Reset Capture */
    READ_CMIC_BS_CAPTURE_CTRLr(unit, &regval);
    soc_reg_field_set(unit, CMIC_BS_CAPTURE_CTRLr, &regval, INT_ENf, 0);
    soc_reg_field_set(unit, CMIC_BS_CAPTURE_CTRLr, &regval, TIME_CAPTURE_MODEf,
                      TIME_CAPTURE_MODE_DISABLE);
    WRITE_CMIC_BS_CAPTURE_CTRLr(unit, regval);

    return BCM_E_NONE;
}

/*
 * Function:
 *	  _bcm_esw_time_deinit
 * Purpose:
 *	  Internal routine used to free time software module
 *    control structures. 
 * Parameters:
 *    unit           - (IN) BCM device number.
 *    time_cfg_pptr  - (IN) Pointer to pointer to time config structure.
 * Returns:
 *	  BCM_E_XXX
 */
STATIC int
_bcm_esw_time_deinit(int unit, _bcm_time_config_p *time_cfg_pptr)
{

    int                 idx;
    _bcm_time_config_p  time_cfg_ptr;
    soc_control_t       *soc = SOC_CONTROL(unit);

    /* Sanity checks. */
    if (NULL == time_cfg_pptr) {
        return (BCM_E_PARAM);
    }

    time_cfg_ptr = *time_cfg_pptr;
    /* If time config was not allocated we are done. */
    if (NULL == time_cfg_ptr) {
        return (BCM_E_NONE);
    }

    /* Free time interface */
    if (NULL != time_cfg_ptr->intf_arr) {
        for (idx = 0; idx < NUM_TIME_INTERFACE(unit); idx++) {
            if (NULL !=  time_cfg_ptr->intf_arr[idx].user_cb) {
                sal_free(time_cfg_ptr->intf_arr[idx].user_cb);
            }
        }
        sal_free(time_cfg_ptr->intf_arr);
    }

    /* Destroy protection mutex. */
    if (NULL != time_cfg_ptr->mutex) {
        sal_mutex_destroy(time_cfg_ptr->mutex);
    }

    /* If any registered function - deregister */
    soc->time_call_ref_count = 0;
    soc->soc_time_callout = NULL;

    /* Free module configuration structue. */
    sal_free(time_cfg_ptr);
    *time_cfg_pptr = NULL;
    return (BCM_E_NONE);
}


/*
 * Function:
 *	  _bcm_esw_time_interface_id_validate
 * Purpose:
 *	  Internal routine used to validate interface identifier 
 * Parameters:
 *    unit           - (IN) BCM device number.
 *    id             - (IN) Interface id to validate
 * Returns:
 *	  BCM_E_XXX
 */
STATIC int 
_bcm_esw_time_interface_id_validate(int unit, bcm_time_if_t id)
{
    if (0 == TIME_INIT(unit)) {
        return (BCM_E_INIT);
    }
    if (id < 0 || id > TIME_INTERFACE_ID_MAX(unit)) {
        return (BCM_E_PARAM);
    }
    if (NULL == TIME_INTERFACE(unit, id)) {
        return (BCM_E_NOT_FOUND);
    }

    return (BCM_E_NONE);
}
/*
 * Function:
 *	  _bcm_esw_time_interface_input_validate
 * Purpose:
 *	  Internal routine used to validate interface input 
 * Parameters:
 *    unit           - (IN) BCM device number.
 *    intf           - (IN) Interface to validate
 * Returns:
 *	  BCM_E_XXX
 */
STATIC int 
_bcm_esw_time_interface_input_validate(int unit, bcm_time_interface_t *intf)
{
    /* Sanity checks. */
    if (NULL == intf) {
        return (BCM_E_PARAM);
    }
    if (intf->flags & BCM_TIME_WITH_ID) {
        if (intf->id < 0 || intf->id > TIME_INTERFACE_ID_MAX(unit) ) {
            return (BCM_E_PARAM);
        }
    }

    if (intf->flags & BCM_TIME_EPOCH) {
        if (intf->epoch.seconds > TIME_EPOCH_MAX - 1) {
            return (BCM_E_PARAM);
        }
        if (intf->epoch.nanoseconds > TIME_NANOSEC_MAX) {
            return BCM_E_PARAM;
        }
    }

    if (intf->flags & BCM_TIME_DRIFT) {
        if (intf->drift.nanoseconds > TIME_NANOSEC_MAX) {
            return BCM_E_PARAM;
        }
    }

    if (intf->flags & BCM_TIME_OFFSET) {
        if (intf->offset.nanoseconds > TIME_NANOSEC_MAX) {
            return BCM_E_PARAM;
        }
    }
    if (intf->flags & BCM_TIME_DRIFT) {
        if (intf->drift.nanoseconds > TIME_NANOSEC_MAX) {
            return BCM_E_PARAM;
        }
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *	  _bcm_esw_time_interface_allocate_id
 * Purpose:
 *	  Internal routine used to allocate time interface id 
 * Parameters:
 *    unit           - (IN) BCM device number.
 *    id             - (OUT) Interface id to be allocated
 * Returns:
 *	  BCM_E_XXX
 */
STATIC int 
_bcm_esw_time_interface_allocate_id(int unit, bcm_time_if_t *id)
{
    int                              idx;  /* Time interfaces iteration index.*/
    _bcm_time_interface_config_p     intf; /* Time interface description.     */

    /* Input parameters check. */
    if (NULL == id) {
        return (BCM_E_PARAM);
    }

    /* Find & allocate time interface. */
    for (idx = 0; idx < TIME_CONFIG(unit)->intf_count; idx++) {
        intf = TIME_CONFIG(unit)->intf_arr + idx;
        if (intf->ref_count) {  /* In use interface */
            continue;
        }
        intf->ref_count++;  /* If founf mark interface as in use */
        *id = intf->time_interface.id; /* Assign ID */
        return (BCM_E_NONE);
    }

    /* No available interfaces */
    return (BCM_E_FULL);
}


/*
 * Function:
 *	  _bcm_esw_time_interface_heartbeat_install
 * Purpose:
 *	  Internal routine used to install interface heartbeat rate into a HW 
 * Parameters:
 *    unit           - (IN) BCM device number.
 *    id             - (IN) Interface id to be installed into a HW
 * Returns:
 *	  BCM_E_XXX
 */
STATIC int 
_bcm_esw_time_interface_heartbeat_install(int unit, bcm_time_if_t id)
{
    uint32                  regval, best_modulo, modulo;
    uint32                  hb_hz;  /* Number of heartbeats in HZ */
    uint32                  ns_hb;  /* Number of nanoseconds per heartbeat */ 
    uint32                  ns_hp;  /* Number of nanosec per half clock cycle*/
    uint32                  threshold; /* Heartbeat threshold */
    bcm_time_interface_t    *intf;
    int                     i, best_modulo_idx;

    intf = TIME_INTERFACE(unit, id);

    hb_hz = (intf->heartbeat_hz > TIME_HEARTBEAT_HZ_MAX) ? 
        TIME_HEARTBEAT_HZ_MAX : intf->heartbeat_hz;
    hb_hz = (intf->heartbeat_hz < TIME_HEARTBEAT_HZ_MIN) ? 
        TIME_HEARTBEAT_HZ_MIN : hb_hz;

    /* Calculate how many nano-seconds in requested hz */
    ns_hb = TIME_HEARTBEAT_NS_HZ / hb_hz;

    /* Find the most accurate period of cycle of a bit clock */
    best_modulo = ns_hb % _bcm_time_bs_supported_frequencies[0];;
    best_modulo_idx = 0;
    for (i = 1; i < TIME_BS_FREQUENCIES_NUM; i++) {
        modulo = ns_hb % _bcm_time_bs_supported_frequencies[i];
        if (modulo < best_modulo) {
            best_modulo = modulo;
            best_modulo_idx = i;
        }
    }

    /* Calculate nanoseconds per half period of cycle */
    ns_hp = _bcm_time_bs_supported_frequencies[best_modulo_idx] / 2;

    /* program the half period of broadsync clock */
    READ_CMIC_BS_CLK_CTRL_0r(unit, &regval);
    soc_reg_field_set(unit, CMIC_BS_CLK_CTRL_0r, &regval, ENABLEf, 1);
    soc_reg_field_set(unit, CMIC_BS_CLK_CTRL_0r, &regval, NSf, ns_hp);
    WRITE_CMIC_BS_CLK_CTRL_0r(unit, regval);

    /* Calculate the threshold for heartbeat */
    threshold = (ns_hb / _bcm_time_bs_supported_frequencies[best_modulo_idx]);

    /* Install correct hw value */
    READ_CMIC_BS_HEARTBEAT_CTRLr(unit, &regval);
    soc_reg_field_set(unit, CMIC_BS_HEARTBEAT_CTRLr, &regval, ENABLEf, 1); 
    soc_reg_field_set(unit, CMIC_BS_HEARTBEAT_CTRLr, &regval, 
                      THRESHOLDf, threshold); 
    WRITE_CMIC_BS_HEARTBEAT_CTRLr(unit, regval);

    return (BCM_E_NONE);
}


/*
 * Function:
 *	  _bcm_esw_time_interface_accuracy_time2hw
 * Purpose:
 *	  Internal routine used to compute HW accuracy value from interface 
 * Parameters:
 *    unit           - (IN) BCM device number.
 *    id             - (IN) Interface id 
 *    accuracy       - (OUT) HW value to be programmed 
 * Returns:
 *	  BCM_E_XXX
 */
STATIC int 
_bcm_esw_time_interface_accuracy_time2hw(int unit, bcm_time_if_t id, 
                                         uint32 *accuracy)
{
    int                     idx;    /* accuracy itterator */
    bcm_time_interface_t    *intf;

    if (NULL == accuracy) {
        return BCM_E_PARAM;
    }

    intf = TIME_INTERFACE(unit, id);

    /* Find the right accuracy */
    for (idx = 0; idx < TIME_ACCURACY_CLK_MAX; idx++) {
        if (intf->accuracy.nanoseconds <= _bcm_time_accuracy_arr[idx].nanoseconds && 
            intf->accuracy.seconds <= _bcm_time_accuracy_arr[idx].seconds) {
            break;
        }
    }
    /* if no match - return error */
    if (idx == TIME_ACCURACY_CLK_MAX) {
        return BCM_E_NOT_FOUND;
    }

    /* Return the correct HW value */

    *accuracy = TIME_ACCURACY_SW_IDX_2_HW(idx);

    return (BCM_E_NONE);
}

/*
 * Function:
 *	  _bcm_esw_time_interface_drift_install
 * Purpose:
 *	  Internal routine used to install interface drift into a HW 
 * Parameters:
 *    unit           - (IN) BCM device number.
 *    id             - (IN) Interface id to be installed into a HW
 * Returns:
 *	  BCM_E_XXX
 */
STATIC int 
_bcm_esw_time_interface_drift_install(int unit, bcm_time_if_t id)
{
    uint32 regval, hw_val, sign;
    bcm_time_interface_t    *intf;

    intf = TIME_INTERFACE(unit, id);
    sign = intf->drift.isnegative;

    /* Requested drift value should not be more then 1/8 of drift denominator */
    if (intf->drift.nanoseconds < TIME_DRIFT_MAX) {
        hw_val = 8 * intf->drift.nanoseconds;
    } else {
        hw_val = 8 * (TIME_DRIFT_MAX);
    }
    READ_CMIC_BS_DRIFT_RATEr(unit, &regval);
    soc_reg_field_set(unit, CMIC_BS_DRIFT_RATEr, &regval, SIGNf, sign);
    soc_reg_field_set(unit, CMIC_BS_DRIFT_RATEr, &regval, FRAC_NSf, hw_val);
    WRITE_CMIC_BS_DRIFT_RATEr(unit, regval);

    return (BCM_E_NONE);
}

/*
 * Function:
 *	  _bcm_esw_time_interface_offset_install
 * Purpose:
 *	  Internal routine used to install interface offset into a HW 
 * Parameters:
 *    unit           - (IN) BCM device number.
 *    id             - (IN) Interface id to be installed into a HW
 * Returns:
 *	  BCM_E_XXX
 */
STATIC int 
_bcm_esw_time_interface_offset_install(int unit, bcm_time_if_t id)
{
    uint32 regval, sign;
    bcm_time_interface_t    *intf;

    intf = TIME_INTERFACE(unit, id);
    
    /* Negative value if local clock is faster and need negative adjustment */
    sign = intf->offset.isnegative;
    /* Write second's values into the HW */
    READ_CMIC_BS_OFFSET_ADJUST_0r(unit, &regval);
    soc_reg_field_set(unit, CMIC_BS_OFFSET_ADJUST_0r, &regval, SECONDf, 
                      intf->offset.seconds);
    WRITE_CMIC_BS_OFFSET_ADJUST_0r(unit, regval);
    /* Write sign and nansecond's values into the HW */
    READ_CMIC_BS_OFFSET_ADJUST_1r(unit, &regval);
    soc_reg_field_set(unit, CMIC_BS_OFFSET_ADJUST_1r, &regval, 
                      SIGN_BITf, sign);
    soc_reg_field_set(unit, CMIC_BS_OFFSET_ADJUST_1r, &regval, NSf, 
                      intf->offset.nanoseconds);
    WRITE_CMIC_BS_OFFSET_ADJUST_1r(unit, regval);

    return (BCM_E_NONE);
}

/*
 * Function:
 *	  _bcm_esw_time_interface_install
 * Purpose:
 *	  Internal routine used to install interface settings into a HW 
 * Parameters:
 *    unit           - (IN) BCM device number.
 *    intf_id        - (IN) Interface id to be installed into a HW
 * Returns:
 *	  BCM_E_XXX
 */
STATIC int 
_bcm_esw_time_interface_install(int unit, bcm_time_if_t intf_id)
{
    bcm_time_interface_t    *intf;  /* Time interface */
    uint32                  regval; /* For register read and write operations */
    uint32                  hw_val; /* Value to program into a HW */
    
    if (NULL == TIME_INTERFACE(unit, intf_id)) {
        return BCM_E_PARAM;
    }

    intf = TIME_INTERFACE(unit, intf_id);

    READ_CMIC_BS_CONFIGr(unit, &regval);

    if (intf->flags & BCM_TIME_ENABLE) {
        soc_reg_field_set(unit, CMIC_BS_CONFIGr, &regval, ENABLEf, 1);
    } else {
        soc_reg_field_set(unit, CMIC_BS_CONFIGr, &regval, ENABLEf, 0);
    }

    if (intf->flags & BCM_TIME_INPUT) {
        soc_reg_field_set(unit, CMIC_BS_CONFIGr, &regval, MODEf, 
                          TIME_MODE_INPUT);
    } else {
        soc_reg_field_set(unit, CMIC_BS_CONFIGr, &regval, MODEf, 
                          TIME_MODE_OUTPUT);
        if (intf->flags & BCM_TIME_ACCURACY) {
            BCM_IF_ERROR_RETURN(
                _bcm_esw_time_interface_accuracy_time2hw(unit, intf_id, &hw_val));
            soc_reg_field_set(unit, CMIC_BS_CONFIGr, &regval, ACCURACYf, hw_val);
        }
        if (intf->flags & BCM_TIME_HEARTBEAT) {
            BCM_IF_ERROR_RETURN(
                _bcm_esw_time_interface_heartbeat_install(unit, intf_id));
        }
    }
    if (intf->flags & BCM_TIME_EPOCH) {
        hw_val = intf->epoch.seconds + 
            TIME_NANOSEC_2_SEC_ROUND(intf->epoch.nanoseconds);
        soc_reg_field_set(unit, CMIC_BS_CONFIGr, &regval, EPOCHf, hw_val);
    }

    WRITE_CMIC_BS_CONFIGr(unit, regval);

    if (intf->flags & BCM_TIME_OFFSET) {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_time_interface_offset_install(unit, intf_id));
    }

    if (intf->flags & BCM_TIME_DRIFT) {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_time_interface_drift_install(unit, intf_id));
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_time_interface_free
 * Purpose:
 *     Free time interface.
 * Parameters:
 *      unit            - (IN) BCM device number. 
 *      intf_id         - (IN) time interface id.
 * Returns:
 *      BCM_X_XXX
 */

STATIC int 
_bcm_time_interface_free(int unit, bcm_time_if_t intf_id) 
{
    _bcm_time_interface_config_p  intf_cfg; /* Time interface config.*/

    intf_cfg = &TIME_INTERFACE_CONFIG(unit, intf_id); 

    if (intf_cfg->ref_count > 0) {
        intf_cfg->ref_count--;
    }

    if (0 == intf_cfg->ref_count) {
        sal_memset(&intf_cfg->time_interface, 0, sizeof(bcm_time_interface_t));
        intf_cfg->time_interface.id = intf_id;
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_esw_time_capture_counter_read
 * Purpose:
 *      Internal routine to read HW clocks
 * Parameters:
 *      unit    - (IN) StrataSwitch Unit #.
 *      id      - (IN) Time interface identifier
 *      time    - (OUT) Structure to contain HW clocks values
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 *
 */

STATIC int 
_bcm_esw_time_capture_counter_read(int unit, bcm_time_if_t id, 
                                   bcm_time_capture_t *time)
{
    uint32      regval;

    /* Read free running time capture */
    READ_CMIC_BS_CAPTURE_FREE_RUN_TIME_0r(unit, &regval);
    time->free.seconds = 
        soc_reg_field_get(unit, CMIC_BS_CAPTURE_FREE_RUN_TIME_0r, 
                          regval, SECONDf); 
    READ_CMIC_BS_CAPTURE_FREE_RUN_TIME_1r(unit, &regval);
    time->free.nanoseconds = 
        soc_reg_field_get(unit, CMIC_BS_CAPTURE_FREE_RUN_TIME_1r, regval, NSf); 
    /* Read syntonous time capture */
    READ_CMIC_BS_CAPTURE_SYNT_TIME_0r(unit, &regval);
    time->syntonous.seconds = 
        soc_reg_field_get(unit, CMIC_BS_CAPTURE_SYNT_TIME_0r, regval, SECONDf);
    READ_CMIC_BS_CAPTURE_SYNT_TIME_1r(unit, &regval);
    time->syntonous.nanoseconds = 
        soc_reg_field_get(unit, CMIC_BS_CAPTURE_SYNT_TIME_1r, regval, NSf);
    /* Read synchronous time capture */
    READ_CMIC_BS_CAPTURE_SYNC_TIME_0r(unit, &regval);
    time->synchronous.seconds = 
        soc_reg_field_get(unit, CMIC_BS_CAPTURE_SYNC_TIME_0r, regval, SECONDf);
    READ_CMIC_BS_CAPTURE_SYNC_TIME_1r(unit, &regval);
    time->synchronous.nanoseconds = 
        soc_reg_field_get(unit, CMIC_BS_CAPTURE_SYNC_TIME_1r, regval, NSf);

    time->free.isnegative = time->synchronous.isnegative = 
        time->syntonous.isnegative = 0;

    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_esw_time_capture_get
 * Purpose:
 *      Internal routine to read HW clocks
 * Parameters:
 *      unit    - (IN) StrataSwitch Unit #.
 *      id      - (IN) Time interface identifier
 *      time    - (OUT) Structure to contain HW clocks values
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 *
 */
STATIC int 
_bcm_esw_time_capture_get (int unit, bcm_time_if_t id, bcm_time_capture_t *time)
{
    uint32          regval, orgval; /* To keep register value */
    int             hw_complete;    /* HW read completion indicator*/
    soc_timeout_t   timeout;        /* Timeout in case of HW error */

    /* Read and remember original capture mode */
    READ_CMIC_BS_CAPTURE_CTRLr(unit, &regval);
    orgval = regval;

    /* Program HW to disable time capture */
    soc_reg_field_set(unit, CMIC_BS_CAPTURE_CTRLr, &regval, TIME_CAPTURE_MODEf,
                      TIME_CAPTURE_MODE_DISABLE);
    if (orgval != regval) {
        WRITE_CMIC_BS_CAPTURE_CTRLr(unit, regval);
    }
    
    /* Program HW to capture time immediately */
    soc_reg_field_set(unit, CMIC_BS_CAPTURE_CTRLr, &regval, TIME_CAPTURE_MODEf,
                      TIME_CAPTURE_MODE_IMMEDIATE);
    WRITE_CMIC_BS_CAPTURE_CTRLr(unit, regval);

    /* Wait for HW time capture completion */
    hw_complete = 0;
    soc_timeout_init(&timeout, 10, 0); /* FIXME: make a #deifine for 10 */

    while (!hw_complete) {
        READ_CMIC_BS_CAPTURE_STATUSr(unit, &regval);
        hw_complete = soc_reg_field_get(unit, CMIC_BS_CAPTURE_STATUSr, regval,
                                        TIME_CAPTURE_COMPLETEf); 
        if (soc_timeout_check(&timeout)) {
            return (BCM_E_TIMEOUT);
        }
    }
    /* Read the HW time */
    BCM_IF_ERROR_RETURN(
        _bcm_esw_time_capture_counter_read(unit, id, time));

    /* Program HW to original time capture value */
    WRITE_CMIC_BS_CAPTURE_CTRLr(unit, orgval);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_esw_time_interface_offset_get
 * Purpose:
 *      Internal routine to read HW offset value and convert it into 
 *      bcm_time_spec_t structure 
 * Parameters:
 *      unit    -  (IN) StrataSwitch Unit #.
 *      id      -  (IN) Time interface identifier
 *      offset  - (OUT) Time interface  offset 
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 *
 */
STATIC int 
_bcm_esw_time_interface_offset_get(int unit, bcm_time_if_t id, 
                                   bcm_time_spec_t *offset)
{
    uint32 regval;
    
    READ_CMIC_BS_OFFSET_ADJUST_0r(unit, &regval);
    offset->seconds = soc_reg_field_get(unit, CMIC_BS_OFFSET_ADJUST_0r,
                                        regval, SECONDf);
    READ_CMIC_BS_OFFSET_ADJUST_1r(unit, &regval);
    offset->nanoseconds= soc_reg_field_get(unit, CMIC_BS_OFFSET_ADJUST_1r,
                                           regval, NSf); 
    offset->isnegative = soc_reg_field_get(unit, CMIC_BS_OFFSET_ADJUST_1r, 
                                           regval, SIGN_BITf);
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_esw_time_interface_drift_get
 * Purpose:
 *      Internal routine to read HW drift value and convert it into 
 *      bcm_time_spec_t structure 
 * Parameters:
 *      unit    -  (IN) StrataSwitch Unit #.
 *      id      -  (IN) Time interface identifier
 *      drift   - (OUT) Time interface  drift 
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 *
 */
STATIC int 
_bcm_esw_time_interface_drift_get(int unit, bcm_time_if_t id,
                                  bcm_time_spec_t *drift)
{
    uint32 regval, val, ns;
    
    READ_CMIC_BS_DRIFT_RATEr(unit, &regval);
    val = soc_reg_field_get(unit, CMIC_BS_DRIFT_RATEr, regval, FRAC_NSf);

    ns = (val / 8);

    drift->nanoseconds = (ns > TIME_DRIFT_MAX) ? TIME_DRIFT_MAX : ns;

    drift->isnegative = soc_reg_field_get(unit, CMIC_BS_DRIFT_RATEr, 
                                          regval, SIGNf);
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_esw_time_interface_accuracy_get
 * Purpose:
 *      Internal routine to read HW accuracy value and convert it into 
 *      bcm_time_spec_t structure 
 * Parameters:
 *      unit        -  (IN) StrataSwitch Unit #.
 *      id          -  (IN) Time interface identifier
 *      accuracy    - (OUT) Time interface  accuracy 
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 *
 */
STATIC int 
_bcm_esw_time_interface_accuracy_get(int unit, bcm_time_if_t id,
                                     bcm_time_spec_t *accuracy)
{
    uint32  regval, val; 

    READ_CMIC_BS_CONFIGr(unit, &regval);
    val = soc_reg_field_get(unit, CMIC_BS_CONFIGr, regval, ACCURACYf);

    if (val < TIME_ACCURACY_LOW_HW_VAL || (val > TIME_ACCURACY_HIGH_HW_VAL && 
                                       val != TIME_ACCURACY_UNKNOWN_HW_VAL)) {
        return BCM_E_PARAM;
    }
    accuracy->isnegative = 
        _bcm_time_accuracy_arr[TIME_ACCURACY_HW_2_SW_IDX(val)].isnegative;
    accuracy->nanoseconds = 
        _bcm_time_accuracy_arr[TIME_ACCURACY_HW_2_SW_IDX(val)].nanoseconds;
    accuracy->seconds = 
        _bcm_time_accuracy_arr[TIME_ACCURACY_HW_2_SW_IDX(val)].seconds;
    accuracy->isnegative = 0;   /* Can't be negative */

    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_esw_time_interface_get
 * Purpose:
 *      Internal routine to get a time sync interface by id
 * Parameters:
 *      unit -  (IN) StrataSwitch Unit #.
 *      id   -  (IN) Time interface identifier
 *      intf - (IN/OUT) Time Sync Interface to get
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 *
 */
STATIC int 
_bcm_esw_time_interface_get(int unit, bcm_time_if_t id, bcm_time_interface_t *intf)
{
    uint32                  regval;
    bcm_time_interface_t    *intf_ptr;

    intf_ptr = TIME_INTERFACE(unit, id);
    intf_ptr->flags = intf->flags;
    intf_ptr->id = id;
    READ_CMIC_BS_CONFIGr(unit, &regval);

    /* Update output flags */
    if (TIME_MODE_INPUT == soc_reg_field_get(unit, CMIC_BS_CONFIGr, 
                                             regval, MODEf)) {
        intf_ptr->flags |= BCM_TIME_INPUT;
    } else {
        intf_ptr->flags &= ~BCM_TIME_INPUT;
    }
    if (soc_reg_field_get(unit, CMIC_BS_CONFIGr, regval, ENABLEf)) {
        intf_ptr->flags |= BCM_TIME_ENABLE;
    } else {
        intf_ptr->flags &= ~BCM_TIME_ENABLE;
    }

    /* Update fields according to input flags */
    if (intf_ptr->flags & BCM_TIME_EPOCH) {
        intf_ptr->epoch.seconds = soc_reg_field_get(unit, CMIC_BS_CONFIGr,
                                                     regval, EPOCHf);
    }

    if (intf->flags & BCM_TIME_ACCURACY) {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_time_interface_accuracy_get(unit, id, 
                                                 &(intf_ptr->accuracy)));
    }
    
    if (intf->flags & BCM_TIME_OFFSET) {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_time_interface_offset_get(unit, id, &(intf_ptr->offset)));
    }

    if (intf->flags & BCM_TIME_DRIFT) {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_time_interface_drift_get(unit, id, &(intf_ptr->drift)));
    }

    *intf = *(TIME_INTERFACE(unit, id));

    return (BCM_E_NONE);
}

/*
 * Function:    
 *      _bcm_esw_time_hw_interrupt
 * Purpose:     
 *      Handles broadsync heartbeat interrupt
 * Parameters:  
 *      unit - StrataSwitch unit #.
 * Returns:     
 *      Nothing
 */
STATIC void
_bcm_esw_time_hw_interrupt(int unit)
{
    void                    *u_data; 
    bcm_time_heartbeat_cb   u_cb;
    int                     idx;    /* interface itterator */
    bcm_time_capture_t      time;
    
    for (idx = 0; idx < NUM_TIME_INTERFACE(unit); idx++) {
        if (NULL != TIME_INTERFACE(unit, idx) &&
            NULL != TIME_INTERFACE_CONFIG(unit,idx).user_cb) {
            TIME_LOCK(unit);
            u_cb = TIME_INTERFACE_CONFIG(unit,idx).user_cb->heartbeat_cb;
            u_data = TIME_INTERFACE_CONFIG(unit,idx).user_cb->user_data;
            _bcm_esw_time_capture_counter_read(unit, idx, &time);
            TIME_UNLOCK(unit);
            if (NULL != u_cb) {
                u_cb(unit, idx, &time, u_data);
            }
        }
    }

    return;
}


/****************************************************************************/
/*                      API functions implmentation                         */
/****************************************************************************/

/*
 * Function:
 *      bcm_esw_time_init
 * Purpose:
 *      Initialize time module 
 * Parameters:
 *      unit - (IN) StrataSwitch Unit #.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 *
 */
int 
bcm_esw_time_init (int unit)
{
    _bcm_time_config_p      time_cfg_ptr;   /* Pointer to Time module config */     
    bcm_time_interface_t    *intf;          /* Time interfaces iterator.     */
    int                     alloc_sz;       /* Memory allocation size.       */
    int                     idx;            /* Time interface array iterator */
    int                     rv;             /* Return Value                  */
    soc_control_t *soc = SOC_CONTROL(unit); /* Soc control structure         */
    
    /* Chek if time feature is supported on a device */
    if (!soc_feature(unit, soc_feature_time_support)) {
        return (BCM_E_UNAVAIL);
    }

    /* If already initialized then deinitialize time module */
    if (TIME_INIT(unit)) {
        _bcm_esw_time_deinit(unit, &TIME_CONFIG(unit));
    }

    /* Allocate time config structure. */
    alloc_sz = sizeof(_bcm_time_config_t);
    time_cfg_ptr = sal_alloc(alloc_sz, "Time module");
    if (NULL == time_cfg_ptr) {
        return (BCM_E_MEMORY);
    }
    sal_memset(time_cfg_ptr, 0, alloc_sz);

    /* Currently only one interface per unit */
    time_cfg_ptr->intf_count = NUM_TIME_INTERFACE(unit); 

    /* Allocate memory for all time interfaces, supported */
    alloc_sz = time_cfg_ptr->intf_count * sizeof(_bcm_time_interface_config_t);
    time_cfg_ptr->intf_arr = sal_alloc(alloc_sz, "Time Interfaces");
    if (NULL == time_cfg_ptr->intf_arr) {
        _bcm_esw_time_deinit(unit, &time_cfg_ptr);
        return (BCM_E_MEMORY);
    }
    sal_memset(time_cfg_ptr->intf_arr, 0, alloc_sz);
    for (idx = 0; idx < time_cfg_ptr->intf_count; idx++) {
        intf = &time_cfg_ptr->intf_arr[idx].time_interface;
        intf->id = idx;
    }

    /* For each time interface allocate memory for tuser_cb */
    alloc_sz = sizeof(_bcm_time_user_cb_t);
    for (idx = 0; idx < NUM_TIME_INTERFACE(unit); idx++) {
        time_cfg_ptr->intf_arr[idx].user_cb = 
            sal_alloc(alloc_sz, "Time Interface User Callback");
        if (NULL == time_cfg_ptr->intf_arr[idx].user_cb) {
            _bcm_esw_time_deinit(unit,  &time_cfg_ptr);
            return (BCM_E_MEMORY);
        }
        sal_memset(time_cfg_ptr->intf_arr[idx].user_cb, 0, alloc_sz);
    }

    /* Interrupt handling function initialization */
    soc->time_call_ref_count = 0;
    soc->soc_time_callout = NULL;

    /* Create protection mutex. */
    time_cfg_ptr->mutex = sal_mutex_create("Time mutex");
    if (NULL == time_cfg_ptr->mutex) {
        _bcm_esw_time_deinit(unit, &time_cfg_ptr);
        return (BCM_E_MEMORY);
    } 

    sal_mutex_take(time_cfg_ptr->mutex, sal_mutex_FOREVER);

    TIME_CONFIG(unit) = time_cfg_ptr;

    /* Clear memories/registers. */
    for (idx = 0; idx < NUM_TIME_INTERFACE(unit); idx++) {
        rv  = _bcm_esw_time_hw_clear(unit, idx);
        if (BCM_FAILURE(rv)) {
            TIME_UNLOCK(unit);
            _bcm_esw_time_deinit(unit, &time_cfg_ptr);
            TIME_CONFIG(unit) = NULL;
            return (BCM_E_MEMORY);
        }
    }

    TIME_UNLOCK(unit);

    return (BCM_E_NONE);
}

/*
 * Function:
 *      bcm_esw_time_deinit
 * Purpose:
 *      Uninitialize time module 
 * Parameters:
 *      unit - (IN) StrataSwitch Unit #.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 *
 */
int 
bcm_esw_time_deinit (int unit)
{
    /* Chek if time feature is supported on a device */
    if (!soc_feature(unit, soc_feature_time_support)) {
        return (BCM_E_UNAVAIL);
    }

    if (0 == TIME_INIT(unit)) {
        return (BCM_E_INIT);
    }

    return _bcm_esw_time_deinit(unit, &TIME_CONFIG(unit));
}

/*
 * Function:
 *      bcm_esw_time_interface_add
 * Purpose:
 *      Adding a time sync interface to a specified unit 
 * Parameters:
 *      unit - (IN) StrataSwitch Unit #.
 *      intf - (IN/OUT) Time Sync Interface
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 *
 */
int 
bcm_esw_time_interface_add (int unit, bcm_time_interface_t *intf)
{
    int             rv;     /* Return value */

    /* Chek if time feature is supported on a device */
    if (!soc_feature(unit, soc_feature_time_support)) {
        return (BCM_E_UNAVAIL);
    }

    BCM_IF_ERROR_RETURN(
        _bcm_esw_time_interface_input_validate(unit, intf));

    TIME_LOCK(unit);
    if (intf->flags & BCM_TIME_WITH_ID) {
        /* If interface already been in use */
        if (0 != TIME_INTERFACE_CONFIG_REF_COUNT(unit, intf->id)) {
            if (0 == (intf->flags & BCM_TIME_REPLACE)) {
                return BCM_E_EXISTS;
            }
        } else {
            TIME_INTERFACE_CONFIG_REF_COUNT(unit, intf->id) = 1;
        }
    } else {
        rv = _bcm_esw_time_interface_allocate_id(unit, &(intf->id));
        if (BCM_FAILURE(rv)) {
            TIME_UNLOCK(unit);
            return rv;
        }
    }

    /* Set time interface configuration. */
    *(TIME_INTERFACE(unit, intf->id)) = *intf; 

    /* Install the interface into the HW */
    rv = _bcm_esw_time_interface_install(unit, intf->id);
    if (BCM_FAILURE(rv)) {
        TIME_UNLOCK(unit);
        return rv;
    }

    TIME_UNLOCK(unit);

    return (BCM_E_NONE);
}

/*
 * Function:
 *      bcm_esw_time_interface_delete
 * Purpose:
 *      Deleting a time sync interface from unit 
 * Parameters:
 *      unit    - (IN) StrataSwitch Unit #.
 *      intf_id - (IN) Time Sync Interface id to remove
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 *
 */
int 
bcm_esw_time_interface_delete (int unit, bcm_time_if_t intf_id)
{
    int rv;   /* Operation return status. */

    /* Chek if time feature is supported on a device */
    if (!soc_feature(unit, soc_feature_time_support)) {
        return (BCM_E_UNAVAIL);
    }

    /* Initialization check. */
    BCM_IF_ERROR_RETURN(
        _bcm_esw_time_interface_id_validate(unit, intf_id));

    TIME_LOCK(unit);

    /* If interface still in use return an Error */
    if (1 < TIME_INTERFACE_CONFIG_REF_COUNT(unit, intf_id)) {
        TIME_UNLOCK(unit);
        return (BCM_E_BUSY);
    }

    /* Free the interface */
    rv = _bcm_time_interface_free(unit, intf_id); 
    if (BCM_FAILURE(rv)) {
        TIME_UNLOCK(unit);
        return (rv);
    }

    rv = _bcm_esw_time_hw_clear(unit, intf_id);

    TIME_UNLOCK(unit);
    return (rv);
}

/*
 * Function:
 *      bcm_esw_time_interface_get
 * Purpose:
 *      Get a time sync interface on a specified unit 
 * Parameters:
 *      unit -  (IN) StrataSwitch Unit #.
 *      intf - (IN/OUT) Time Sync Interface to get
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 *
 */
int 
bcm_esw_time_interface_get (int unit, bcm_time_interface_t *intf)
{
    int rv; 

    /* Chek if time feature is supported on a device */
    if (!soc_feature(unit, soc_feature_time_support)) {
        return (BCM_E_UNAVAIL);
    }

    /* Validation checks */
    if (NULL == intf) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(
        _bcm_esw_time_interface_id_validate(unit, intf->id));

    TIME_LOCK(unit);
    rv = _bcm_esw_time_interface_get(unit, intf->id, intf);
    TIME_UNLOCK(unit);

    return (rv);
}

/*
 * Function:
 *      bcm_esw_time_interface_delete_all
 * Purpose:
 *      Deleting all time sync interfaces on a unit 
 * Parameters:
 *      unit    - (IN) StrataSwitch Unit #.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 *
 */
int 
bcm_esw_time_interface_delete_all (int unit)
{
    bcm_time_if_t   intf;

    /* Chek if time feature is supported on a device */
    if (!soc_feature(unit, soc_feature_time_support)) {
        return (BCM_E_UNAVAIL);
    }

    /* Initialization check */
    if (0 == TIME_INIT(unit)) {
        return BCM_E_INIT;
    }

    for (intf = 0; intf < NUM_TIME_INTERFACE(unit); intf++ ) {
        BCM_IF_ERROR_RETURN(
            bcm_esw_time_interface_delete(unit, intf));
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *      bcm_esw_time_interface_traverse
 * Purpose:
 *      Itterates over all time sync interfaces and calls given callback 
 * Parameters:
 *      unit        - (IN) StrataSwitch Unit #.
 *      cb          - (IN) Call back function
 *      user_data   - (IN) void pointer to store any user information
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 *
 */
int 
bcm_esw_time_interface_traverse (int unit, bcm_time_interface_traverse_cb cb, 
                                 void *user_data)
{
    bcm_time_if_t   intf;
    
    /* Chek if time feature is supported on a device */
    if (!soc_feature(unit, soc_feature_time_support)) {
        return (BCM_E_UNAVAIL);
    }

    /* Initialization check. */
    if (0 == TIME_INIT(unit)) {
        return BCM_E_INIT;
    }
    
    if (NULL == cb) {
        return (BCM_E_PARAM);
    }

    for (intf = 0; intf < NUM_TIME_INTERFACE(unit); intf++ ) {
        if (NULL != TIME_INTERFACE(unit, intf)) {
            BCM_IF_ERROR_RETURN(cb(unit, TIME_INTERFACE(unit, intf), user_data));
        }
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *      bcm_esw_time_capture_get
 * Purpose:
 *      Gets a time captured by HW clock
 * Parameters:
 *      unit    - (IN) StrataSwitch Unit #.
 *      id      - (IN) Time interface identifier 
 *      time    - (OUT) Structure to contain HW clocks values
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 *
 */
int 
bcm_esw_time_capture_get (int unit, bcm_time_if_t id, bcm_time_capture_t *time)
{

    int rv;   /* Operation return status. */

    /* Chek if time feature is supported on a device */
    if (!soc_feature(unit, soc_feature_time_support)) {
        return (BCM_E_UNAVAIL);
    }

    /* Initialization check. */
    BCM_IF_ERROR_RETURN(
        _bcm_esw_time_interface_id_validate(unit, id));

    if (NULL == time) {
        return (BCM_E_PARAM);
    }
    if (NULL == TIME_CAPTURE(unit, id)) {
        return (BCM_E_NOT_FOUND);
    }

    TIME_LOCK(unit);
    rv = _bcm_esw_time_capture_get(unit, id, TIME_CAPTURE(unit, id));
    if (BCM_FAILURE(rv)) {
        TIME_UNLOCK(unit);
        return (rv);
    }

    *time = *(TIME_CAPTURE(unit, id));

    TIME_UNLOCK(unit);

    return (BCM_E_NONE);
}

/*
 * Function:
 *      bcm_esw_time_heartbeat_enable_set
 * Purpose:
 *      Enables/Disables interrupt handling for each heartbeat provided by a 
 *      HW clock
 * Parameters:
 *      unit    - (IN) StrataSwitch Unit #.
 *      id      - (IN) Time Sync Interface Id
 *      enable  - (IN) Enable/Disable parameter
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 *
 */
int 
bcm_esw_time_heartbeat_enable_set (int unit, bcm_time_if_t id, int enable)
{
    uint32      regval;

    /* Chek if time feature is supported on a device */
    if (!soc_feature(unit, soc_feature_time_support)) {
        return (BCM_E_UNAVAIL);
    }

    /* Param validation check. */
    BCM_IF_ERROR_RETURN(
        _bcm_esw_time_interface_id_validate(unit, id));

    TIME_LOCK(unit);
    READ_CMIC_BS_CAPTURE_CTRLr(unit, &regval);
    /* Configure the HW to give interrupt on every heartbeat */
    soc_reg_field_set(unit, CMIC_BS_CAPTURE_CTRLr, &regval, INT_ENf, 
                      (enable) ? 1: 0);
    /* Configure the HW to capture time on every heartbeat */
    soc_reg_field_set(unit, CMIC_BS_CAPTURE_CTRLr, &regval, TIME_CAPTURE_MODEf,
                      (enable) ? TIME_CAPTURE_MODE_HEARTBEAT : 
                      TIME_CAPTURE_MODE_DISABLE);

    WRITE_CMIC_BS_CAPTURE_CTRLr(unit, regval);

    TIME_UNLOCK(unit);

    return (BCM_E_NONE);
}

/*
 * Function:
 *      bcm_esw_time_heartbeat_enable_get
 * Purpose:
 *      Gets interrupt handling status for each heartbeat provided by a 
 *      HW clock
 * Parameters:
 *      unit    - (IN) StrataSwitch Unit #.
 *      id      - (IN) Time Sync Interface Id
 *      enable  - (OUT) Enable status of interrupt handling
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 *
 */
int 
bcm_esw_time_heartbeat_enable_get (int unit, bcm_time_if_t id, int *enable)
{
    uint32      regval;

    /* Chek if time feature is supported on a device */
    if (!soc_feature(unit, soc_feature_time_support)) {
        return (BCM_E_UNAVAIL);
    }

    /* Param validation check. */
    BCM_IF_ERROR_RETURN(
        _bcm_esw_time_interface_id_validate(unit, id));

    /* Read HW Configuration */
    READ_CMIC_BS_CAPTURE_CTRLr(unit, &regval);
    *enable = soc_reg_field_get(unit, CMIC_BS_CAPTURE_CTRLr, regval, INT_ENf); 

    return (BCM_E_NONE);
}

/*
 * Function:
 *      bcm_esw_time_heartbeat_register
 * Purpose:
 *      Registers a call back function to be called on each heartbeat
 * Parameters:
 *      unit        - (IN) StrataSwitch Unit #.
 *      id          - (IN) Time Sync Interface Id
 *      f           - (IN) Function to register
 *      user_data   - (IN) void pointer to store any user information
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 *
 */
int 
bcm_esw_time_heartbeat_register (int unit, bcm_time_if_t id, bcm_time_heartbeat_cb f,
                                 void *user_data)
{
    soc_control_t	*soc = SOC_CONTROL(unit);

    /* Chek if time feature is supported on a device */
    if (!soc_feature(unit, soc_feature_time_support)) {
        return (BCM_E_UNAVAIL);
    }

    /* Param validation check. */
    BCM_IF_ERROR_RETURN(
        _bcm_esw_time_interface_id_validate(unit, id));
    if (NULL == f) {
        return (BCM_E_PARAM);
    }

    TIME_LOCK(unit);
    /* Register HW interrupt handler */
    soc->soc_time_callout = _bcm_esw_time_hw_interrupt;
    soc->time_call_ref_count ++;

    /* Register user call back */
    TIME_INTERFACE_CONFIG(unit, id).user_cb->heartbeat_cb = f;
    TIME_INTERFACE_CONFIG(unit, id).user_cb->user_data = user_data;

    TIME_UNLOCK(unit);
    
    return (BCM_E_NONE);
}

/*
 * Function:
 *      bcm_esw_time_heartbeat_unregister
 * Purpose:
 *      Unregisters a call back function to be called on each heartbeat
 * Parameters:
 *      unit        - (IN) StrataSwitch Unit #.
 *      id          - (IN) Time Sync Interface Id
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 *
 */
int 
bcm_esw_time_heartbeat_unregister (int unit, bcm_time_if_t id)
{
    soc_control_t	*soc = SOC_CONTROL(unit);

    /* Chek if time feature is supported on a device */
    if (!soc_feature(unit, soc_feature_time_support)) {
        return (BCM_E_UNAVAIL);
    }

    /* Param validation check. */
    BCM_IF_ERROR_RETURN(
        _bcm_esw_time_interface_id_validate(unit, id));

    TIME_LOCK(unit);

    if (--soc->time_call_ref_count <= 0) {
        soc->time_call_ref_count = 0;
        soc->soc_time_callout = NULL;
    }
    TIME_INTERFACE_CONFIG(unit, id).user_cb->heartbeat_cb = NULL;
    TIME_INTERFACE_CONFIG(unit, id).user_cb->user_data = NULL;

    TIME_UNLOCK(unit);
    
    return (BCM_E_NONE);
}


