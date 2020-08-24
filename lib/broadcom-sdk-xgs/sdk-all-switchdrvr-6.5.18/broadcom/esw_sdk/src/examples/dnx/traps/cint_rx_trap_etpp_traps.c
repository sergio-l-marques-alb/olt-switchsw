/* $Id: 
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$ 
 *  
 * File: cint_rx_trap_etpp_traps.c
 * Purpose: Example for creating and setting an ETPP application Trap.
 *
 * cint ../../../../src/examples/dnx/traps/cint_rx_trap_etpp_traps.c
 *
 * Main Function:
 *      cint_rx_trap_etpp_traps_main(unit, appl_trap_type, is_pkt_drop, etpp_config_p);
 * Destroy Function:
 *      cint_rx_trap_etpp_traps_destroy(unit, appl_trap_type);
 *
 * Additional Notes: 
 * To configure snoop action need to update following fields:
 * cint_rx_trap_etpp_traps_config.snoop_strength - should be filled with value between 1-7
 * cint_rx_trap_etpp_traps_trap_id - should be filled with snoop profile retrieved from bcm_mirror_destination_create
 *
 * Example Config:
 * cint ../../../../src/examples/dnx/traps/cint_utils_rx.c
 * cint ../../../../src/examples/dnx/traps/cint_rx_trap_etpp_traps.c
 * cint
 * cint_rx_trap_etpp_traps_main(0,bcmRxTrapEgTxSplitHorizonFilter,TRUE,NULL);
 */

struct cint_rx_trap_etpp_traps_config_utils_s{
    /* Flags for ETPP can only be BCM_RX_TRAP_COPY_DROP */
    int flags;

    /* cpu trap gport - configure recycle cmd */
    bcm_gport_t cpu_trap_gport;

    /* recycle high priority indication */
    uint8 is_recycle_high_priority;
    
    /* crop recycle packet indication */
    uint8 is_recycle_crop_pkt;

    /* indication regarding appending the original FTMH to the recycle packet */
    uint8 is_recycle_append_ftmh;

    /** packet processing drop reason */
    int pp_drop_reason;

    /* forward trap strength */
    int trap_strength;                
    
    /* snoop strength */
    int snoop_strength;     
};

/* default configuration is drop */
cint_rx_trap_etpp_traps_config_utils_s cint_rx_trap_etpp_traps_config ={
    /* Flags for ETPP can only be BCM_RX_TRAP_COPY_DROP */
    0,

    /* cpu trap gport */
    0,

    /* recycle high priority indication */
    FALSE,
    
    /* crop recycle packet indication */
    TRUE,

    /* indication regarding appending the original FTMH to the recycle packet */
    TRUE,

    /** packet processing drop reason */
    0,    

    /* forward trap strength */
    7,                
    
    /* snoop strength */
    0      
};

int cint_rx_trap_etpp_traps_trap_id = BCM_RX_TRAP_EG_TX_TRAP_ID_DEFAULT;

/* Configure recycle command trap to redirect packet to CPU */
BCM_GPORT_TRAP_SET(cint_rx_trap_etpp_traps_config.cpu_trap_gport,0xb3,15,0);

 /**
 * \brief  
 *  Fill trap_config struct with etpp trap configuration
 * \par DIRECT INPUT: 
 *   \param [in] etpp_config   -  etpp trap configuration
 *   \param [out] trap_config  - generic trap configuration
 * \par INDIRECT INPUT: 
 *   * None
 * \par DIRECT OUTPUT: 
 *   int -  Error Type
 * \par INDIRECT OUTPUT: 
 *   * None
 * \remark 
 *   * None
 * \see
 *   * None
 */
int trap_config_from_etpp_config_fill(
cint_rx_trap_etpp_traps_config_utils_s* etpp_config_p,
bcm_rx_trap_config_t* trap_config_p)
{
    int rv = BCM_E_NONE;

    /* Check input */
    if(etpp_config_p == NULL || trap_config_p == NULL)
    {
        printf("Error,  NULL param \n");
        return BCM_E_PARAM;
    }

    bcm_rx_trap_config_t_init(trap_config_p);
    if(rv != BCM_E_NONE)
    {
        printf("Error initializing trap_config \n");
        return rv;
    }

    trap_config_p->flags = etpp_config_p->flags;
    trap_config_p->cpu_trap_gport = etpp_config_p->cpu_trap_gport;
    trap_config_p->is_recycle_high_priority = etpp_config_p->is_recycle_high_priority;
    trap_config_p->is_recycle_crop_pkt = etpp_config_p->is_recycle_crop_pkt;
    trap_config_p->is_recycle_append_ftmh = etpp_config_p->is_recycle_append_ftmh;
    trap_config_p->pp_drop_reason = etpp_config_p->pp_drop_reason;
    trap_config_p->trap_strength = etpp_config_p->trap_strength;
    trap_config_p->snoop_strength = etpp_config_p->snoop_strength;       

    return rv;
}

 /**
 * \brief  
 *  Main function for generic etpp trap configuration
 * \par DIRECT INPUT: 
 *   \param [in] unit      -  Unit Id
 *   \param [in] appl_trap_type   -  application trap type to configure
 *   \param [in] trap_config - etpp trap configuration
 * \par INDIRECT INPUT: 
 *   * None
 * \par DIRECT OUTPUT: 
 *   int -  Error Type
 * \par INDIRECT OUTPUT: 
 *   * None
 * \remark 
 *   * None
 * \see
 *   * None
 */
int cint_rx_trap_etpp_traps_main(
int unit,
bcm_rx_trap_t appl_trap_type,
int is_pkt_drop,
cint_rx_trap_etpp_traps_config_utils_s* etpp_config_p)
{
    int rv = BCM_E_NONE;
    bcm_rx_trap_config_t trap_config;

    if (etpp_config_p != NULL)
    {
        sal_memcpy(&cint_rx_trap_etpp_traps_config, etpp_config_p, sizeof(cint_rx_trap_etpp_traps_config));
    }

    if (is_pkt_drop)
    {
        rv = cint_utils_rx_trap_etpp_create_and_set_to_drop(unit, appl_trap_type, cint_rx_trap_etpp_traps_config.trap_strength, &cint_rx_trap_etpp_traps_trap_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in cint_utils_rx_trap_etpp_create_and_set \n");
            return rv;
        }        
    }
    else
    {   
        rv = trap_config_from_etpp_config_fill(&cint_rx_trap_etpp_traps_config, &trap_config);
        if(rv != BCM_E_NONE)
        {
            printf("Error, with config struct \n");
            return rv;
        }        
    
        /* Set the trap */
        rv = cint_utils_rx_trap_appl_config(unit, 0, FALSE, appl_trap_type, &trap_config, &cint_rx_trap_etpp_traps_trap_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in cint_utils_rx_trap_etpp_create_and_set \n");
            return rv;
        }
    }

    return rv;    
}

/**
* \brief  
*  Destroy etpp trap
* \par DIRECT INPUT: 
*   \param [in] unit      -  Unit Id
*   \param [in] appl_trap_type   -  application trap type to destroy
* \par INDIRECT INPUT: 
*   * None
* \par DIRECT OUTPUT: 
*   int -  Error Type
* \par INDIRECT OUTPUT: 
*   * None
* \remark 
*   * None
* \see
*   * None
*/
int cint_rx_trap_etpp_traps_destroy(
int unit,
bcm_rx_trap_t appl_trap_type)
{
    int rv = BCM_E_NONE;

    rv = cint_utils_rx_trap_appl_destroy(unit, appl_trap_type);
    if (rv != BCM_E_NONE)
    {
        printf("Error destroying etpp trap, in cint_utils_rx_trap_appl_destroy\n");
        return rv;
    }
    
    return rv;
}
