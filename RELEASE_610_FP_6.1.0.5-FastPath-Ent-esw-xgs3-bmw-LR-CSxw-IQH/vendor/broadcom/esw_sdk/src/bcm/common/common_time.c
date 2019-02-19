/*
 *
 * Time - Broadcom StrataSwitch Time BroadSync common API.
 */

#include <soc/drv.h>
#include <bcm/time.h>


/*
 * Function:
 *      bcm_time_spec_t_init
 * Purpose:
 *      Initialize bcm_time_spec_t structure 
 * Parameters:
 *      spec - (OUT) pointer to bcm_time_spec_t structure to initialize
 */
void 
bcm_time_spec_t_init(bcm_time_spec_t *spec)
{
    sal_memset(spec, 0, sizeof (bcm_time_spec_t));
}


/*
 * Function:
 *      bcm_time_interface_t_init
 * Purpose:
 *      Initialize bcm_time_interface_t structure 
 * Parameters:
 *      intf - (OUT) pointer to bcm_time_interface_t structure to initialize
 */
void 
bcm_time_interface_t_init(bcm_time_interface_t *intf)
{
    sal_memset(intf, 0, sizeof (bcm_time_interface_t));
}


/*
 * Function:
 *      bcm_time_capture_t_init
 * Purpose:
 *      Initialize bcm_time_capture_t structure 
 * Parameters:
 *      capture - (OUT) pointer to bcm_time_capture_t structure to initialize
 */
void 
bcm_time_capture_t_init(bcm_time_capture_t *capture)
{
    sal_memset(capture, 0, sizeof (bcm_time_capture_t));
}
