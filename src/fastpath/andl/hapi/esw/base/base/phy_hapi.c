/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2001-2007
 *
 **********************************************************************
 *
 * @filename phy.c
 *
 * @purpose This file contains the code for phy link management
 *
 * @component broad
 *
 * @comments none
 *
 * @create 2/26/2001
 *
 * @author nsummers
 *
 * @end
 *
 **********************************************************************/

#include "l7_common.h"
#include "osapi.h"
#include "sysapi.h"

#include "cnfgr.h"
#include "default_cnfgr.h"

#include "broad_common.h"

#include "phy_hapi.h"
#include "log.h"

#include "registry.h"

#include "soc/types.h"
#include "soc/drv.h"
#include "bcm/port.h"
#include "bcm/link.h"
#include "bcm/stat.h"
#include "bcmx/port.h"
#include "bcmx/link.h"

#include "l7_usl_bcmx_port.h"

/*********************************************************************
 *
 * @purpose Check the link status on the USP, if the status has changed,
 *          a callback will be performed to the application.
 *
 * @param   L7_ulong32  numArgs  - number of arguments for the task
 * @param   L7_ulong   *argArray - arg array to be passed
 *
 * @returns L7_RC_t result
 *
 * @notes   none
 *
 * @end
 *
 *********************************************************************/
L7_RC_t hapiBroadLinkCheck(DAPI_USP_t *usp, DAPI_t *dapi_g)
{

	L7_RC_t  result = L7_SUCCESS;

	return result;

}



/*********************************************************************
 *
 * @purpose Poll for the phy link status
 *
 * @param   L7_ulong32  numArgs - number of arguments for the task
 * @param   DAPI_t     *dapi_g  - the driver object
 *
 * @returns L7_RC_t result
 *
 * @notes   Doesn't actually do the read
 *
 * @end
 *
 *********************************************************************/
L7_RC_t hapiBroadBC5218Task(L7_ulong32 numArgs, DAPI_t *dapi_g)
{

	L7_RC_t  result = L7_SUCCESS;

	return result ;

}



/*********************************************************************
 *
 * @purpose Handle BC5218 interrupts
 *
 * @param L7_ulong32 temp - not used
 *
 * @returns none
 *
 * @notes caution! stilleto specific
 *
 * @end
 *
 *********************************************************************/
void hapiBroadPhyIntrptHdlr(int temp)
{

}



/*********************************************************************
 *
 * @purpose Initialize the phy polling/interrupt
 *
 * @param   DAPI_t *dapi_g - the driver object
 *
 * @returns L7_RC_t result
 *
 * @notes   Creates a task for now.  Needs to change in the future to check
 *  		     whether or not to do polling or interrupt.
 *
 * @end
 *
 *********************************************************************/
L7_RC_t hapiBroadPhyInit(DAPI_t *dapi_g)
{
	/*static const char           *routine_name = "hapiBroadPhyInit()";*/
	L7_RC_t                result = L7_SUCCESS;

	return result;
}

/*********************************************************************
 *
 * @purpose Initializes the Phys and Grabs the inital state of Link Status
 *
 * @param   L7_ushort16  unitNum - the unit being initialized
 * @param   L7_ushort16  slotNum - the slot being initialized
 * @param   DAPI_t      *dapi_g  - the driver object
 *
 * @returns L7_RC_t result
 *
 * @notes   none
 *
 * @end
 *
 *********************************************************************/
L7_RC_t hapiBroadPortPhyInit(L7_ushort16 unitNum, L7_ushort16 slotNum, DAPI_t *dapi_g)
{
	/* static const char    *routine_name = "hapiBroadPortPhyInit()"; */
	L7_RC_t         result      = L7_SUCCESS;

	/* We don't need to do anything here for this platform.
	 */

	return result;

}

#if 0
/* Given a maximum speed, return the mask of bcm_port_ability_t speeds
 * while are less than or equal to the given speed. */
bcm_port_abil_t
static port_speed_max_mask(bcm_port_abil_t max_speed)
{
    bcm_port_abil_t speed_mask = 0;
    /* This is a giant fall through switch */
    switch (max_speed) {
        
    case 42000:
        speed_mask |= BCM_PORT_ABILITY_42GB;
    case 40000:
        speed_mask |= BCM_PORT_ABILITY_40GB;
    case 30000:
        speed_mask |= BCM_PORT_ABILITY_30GB;
    case 25000:
        speed_mask |= BCM_PORT_ABILITY_25GB;
    case 24000:
        speed_mask |= BCM_PORT_ABILITY_24GB;
    case 21000:
        speed_mask |= BCM_PORT_ABILITY_21GB;
    case 20000:
        speed_mask |= BCM_PORT_ABILITY_20GB;
    case 16000:
        speed_mask |= BCM_PORT_ABILITY_16GB;
    case 15000:
        speed_mask |= BCM_PORT_ABILITY_15GB;
    case 13000:
        speed_mask |= BCM_PORT_ABILITY_13GB;
    case 12500:
        speed_mask |= BCM_PORT_ABILITY_12P5GB;
    case 12000:
        speed_mask |= BCM_PORT_ABILITY_12GB;
    case 10000:
        speed_mask |= BCM_PORT_ABILITY_10GB;
    case 6000:
        speed_mask |= BCM_PORT_ABILITY_6000MB;
    case 5000:
        speed_mask |= BCM_PORT_ABILITY_5000MB;
    case 3000:
        speed_mask |= BCM_PORT_ABILITY_3000MB;
    case 2500:
        speed_mask |= BCM_PORT_ABILITY_2500MB;
    case 1000:
        speed_mask |= BCM_PORT_ABILITY_1000MB;
    case 100:
        speed_mask |= BCM_PORT_ABILITY_100MB;
    case 10:
        speed_mask |= BCM_PORT_ABILITY_10MB;
    default:
        break;
    }
    return speed_mask;
}
#endif

/*********************************************************************
 *
 * @purpose This routine set the speed and duplex for a phy
 *
 * @param   DAPI_USP_t         *usp    - needs to be a valid usp
 * @param   DAPI_PORT_SPEED_t   speed  - the speed you wish to set the usp to
 * @param   DAPI_PORT_DUPLEX_t  duplex - the duplex you wish to set the usp to
 * @param   DAPI_t             *dapi_g - the driver object
 *
 * @returns L7_RC_t result
 *
 * @notes   none
 *
 * @end
 *
 *********************************************************************/
L7_RC_t hapiBroadPhyModeSet(DAPI_USP_t *usp, DAPI_PORT_SPEED_t speed, DAPI_PORT_DUPLEX_t duplex, DAPI_t *dapi_g)
{
	/* static const char           *routine_name = "hapiBroadPhyModeSet()"; */
	L7_RC_t				result = L7_SUCCESS;
	DAPI_PORT_t 		*dapiPortPtr;
	BROAD_PORT_t		*hapiPortPtr;
	L7_int32			bcmSpeed=0, local_advert=0;
	L7_int32			rc=0;
	bcm_phy_config_t	fiber_config;
	bcm_phy_config_t	copper_config;
	L7_int32 			bcmDuplex=0, mac_pause_tx = 0, mac_pause_rx = 0;
	bcm_port_abil_t		local_ability_mask;
    //bcm_port_ability_t  port_local_ability, local_ability;
	int					jam = L7_FALSE;

	hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);
	dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);

	if (IS_PORT_TYPE_PHYSICAL(dapiPortPtr) == L7_FALSE)
	{
	        L7_LOG_ERROR(dapiPortPtr->type);
	}

	switch (speed)
	{
		case DAPI_PORT_SPEED_FE_10MBPS:
			bcmSpeed = 10;
            local_advert = BCM_PORT_ABIL_10MB_FD;       /* PTin added: autoneg */
			break;

		case DAPI_PORT_SPEED_FE_100MBPS:
			bcmSpeed = 100;
            local_advert = BCM_PORT_ABIL_100MB_FD;      /* PTin added: autoneg */
			break;

		case DAPI_PORT_SPEED_GE_1GBPS:
			bcmSpeed = 1000;
            local_advert = BCM_PORT_ABIL_1000MB_FD;     /* PTin added: autoneg */
			break;

        /* PTin added: Speed 2.5G */
        case DAPI_PORT_SPEED_GE_2G5BPS:
          bcmSpeed = 2500;
          local_advert = BCM_PORT_ABIL_2500MB_FD;       /* PTin added: autoneg */
          break;
        /* PTin end */

		case DAPI_PORT_SPEED_GE_10GBPS:
			bcmSpeed = 10000;
			break;

        /* PTin added: Speed 40G */
        case DAPI_PORT_SPEED_GE_40GBPS:
          bcmSpeed = 40000;
          local_advert = BCM_PORT_ABIL_1000MB_FD;     /* PTin added: autoneg */
          break;

        /* PTin added: Speed 100G */
        case DAPI_PORT_SPEED_GE_100GBPS:
          bcmSpeed = 100000;
          break;

        /* PTin end */

        case DAPI_PORT_AUTO_NEGOTIATE:
			break;
		default:
			return L7_FAILURE;
	}

	if (hapiPortPtr->hapiModeparm.physical.phySemaphore == L7_NULL)
	{
	        L7_LOG_ERROR(0);
	}
	if (osapiSemaTake(hapiPortPtr->hapiModeparm.physical.phySemaphore, L7_WAIT_FOREVER) != L7_SUCCESS)
	{
	        L7_LOG_ERROR(0);
	}

	if (speed != DAPI_PORT_AUTO_NEGOTIATE)
	{
		/* Previously, Fiber mode on combo PHYs was disabled as soon as auto-negotiate
		 * is turned off. This could be because PHY drivers didn't support fixed speeds
		 * on fiber ports. Now, some of the PHY driver do support fixed speeds on fiber
		 * ports too. For example, phy5482. There is no generic way of to find what speeds
		 * are supported in fiber mode.
		 */
		if ((dapiPortPtr->phyCapabilities & L7_PHY_CAP_PORTSPEED_SFP) &&
				(hapiPortPtr->hapiModeparm.physical.fiber_mode_disabled == L7_FALSE))
		{
			rc = bcmx_port_medium_config_get (hapiPortPtr->bcmx_lport,
					BCM_PORT_MEDIUM_FIBER, &fiber_config);
			if (rc == BCM_E_NONE)
			{
				fiber_config.autoneg_enable = 0;
				fiber_config.force_speed = bcmSpeed;
				fiber_config.force_duplex = (duplex == DAPI_PORT_DUPLEX_FULL)?
											BCM_PORT_DUPLEX_FULL : BCM_PORT_DUPLEX_HALF;

				rc = bcmx_port_medium_config_set (hapiPortPtr->bcmx_lport,
						BCM_PORT_MEDIUM_FIBER, &fiber_config);
				if (rc != BCM_E_NONE)
				{
					/* We failed to set fixed speed on Fiber. Disable it */
					fiber_config.autoneg_enable = 1;
					fiber_config.enable = 0;
					fiber_config.preferred = 0;
					rc = bcmx_port_medium_config_set (hapiPortPtr->bcmx_lport,
							BCM_PORT_MEDIUM_FIBER,
							&fiber_config);
					hapiPortPtr->hapiModeparm.physical.fiber_mode_disabled = L7_TRUE;
				}
			}
		}

		rc = bcmx_port_medium_config_get (hapiPortPtr->bcmx_lport,
				BCM_PORT_MEDIUM_COPPER, &copper_config);
		if (rc == BCM_E_NONE)
		{
			copper_config.autoneg_enable = 0;
			/* Prefer copper if fiber was disabled */
			if (hapiPortPtr->hapiModeparm.physical.fiber_mode_disabled == L7_TRUE)
				copper_config.preferred = 1;

			copper_config.force_duplex = (duplex == DAPI_PORT_DUPLEX_FULL)?
											BCM_PORT_DUPLEX_FULL : BCM_PORT_DUPLEX_HALF;

			/*If system Flowcontrol is enabled*/
			if (dapi_g->system->flowControlEnable)
			{
				if (duplex == DAPI_PORT_DUPLEX_FULL)
				{
					/* Enable PAUSE  & disable back pressure, for full duplex port */
					copper_config.autoneg_advert |= (SOC_PM_PAUSE_TX | SOC_PM_PAUSE_RX);
					mac_pause_tx = 1;
			  		mac_pause_rx = 1;
					jam = L7_FALSE;
				}
				else
				{
					/* Disable PAUSE  & enable back pressure, for half duplex port */
					copper_config.autoneg_advert &= ~(SOC_PM_PAUSE_TX | SOC_PM_PAUSE_RX);
					mac_pause_tx = 0;
				  	mac_pause_rx = 0;
					jam = L7_TRUE;
				}
			}
			else /* System flowcontrol is disabled */
			{
				/* Disable both PAUSE and back pressure for this port*/
				copper_config.autoneg_advert &= ~(SOC_PM_PAUSE_TX | SOC_PM_PAUSE_RX);
				mac_pause_tx = 0;
				mac_pause_rx = 0;
				jam = L7_FALSE;
			}

#if 1
			/* According to 802.3 we must disable cross-over detection when autonegotiation
			 ** is disabled, however this sometimes causes link not to come up due to
			 ** the way different PHYs handle the mode. It is safer to just use autodetect
			 ** all the time.
			 */
			copper_config.mdix = BCM_PORT_MDIX_FORCE_AUTO;
#else
			copper_config.mdix = BCM_PORT_MDIX_XOVER;
#endif
			copper_config.force_speed = bcmSpeed;

			rc = bcmx_port_pause_set(hapiPortPtr->bcmx_lport, mac_pause_tx, mac_pause_rx);
			if ((L7_BCMX_OK(rc) != L7_TRUE) && (rc != BCM_E_UNAVAIL))
			        L7_LOG_ERROR(rc);

                        bcmDuplex = (duplex == DAPI_PORT_DUPLEX_FULL) ?
                                     BCM_PORT_DUPLEX_FULL : BCM_PORT_DUPLEX_HALF;

                        /* Set the duplex setting too. The medium config set API
                         * configures the PHY, and not the MAC.
                         */
                        rc = bcmx_port_duplex_set(hapiPortPtr->bcmx_lport, bcmDuplex);
                        if ((L7_BCMX_OK(rc) != L7_TRUE) && (rc != BCM_E_UNAVAIL))
                                L7_LOG_ERROR(rc);

	                rc = bcmx_port_jam_set(hapiPortPtr->bcmx_lport, jam);
			if ((L7_BCMX_OK(rc) != L7_TRUE) && (rc != BCM_E_UNAVAIL))
			        L7_LOG_ERROR(rc);

			rc = usl_bcmx_port_medium_config_set (hapiPortPtr->bcmx_lport,
					BCM_PORT_MEDIUM_COPPER, &copper_config);
			if (L7_BCMX_OK(rc) != L7_TRUE)
			        L7_LOG_ERROR (rc);
		}
		else
		{
			if ((L7_BCMX_OK(rc) != L7_TRUE) && (rc != BCM_E_UNAVAIL))
			        L7_LOG_ERROR (rc);

			/* port-medium-config command is not supported on this PHY. Use different commands.
			 */

			rc = bcmx_port_speed_set(hapiPortPtr->bcmx_lport, bcmSpeed);
			if ((L7_BCMX_OK(rc) != L7_TRUE) && (rc != BCM_E_UNAVAIL))
			{
#ifdef LVL7_ALPHA8245
				/* Alpha tucanas have only SFP versions of 1G ports. And only 1000Mbps speed
				 * is supported
				 */
				if (dapiPortPtr->phyCapabilities & L7_PHY_CAP_PORTSPEED_SFP)
				{
					if (rc != BCM_E_CONFIG) /* Ignore this for SFP */
					{
					        L7_LOG_ERROR (rc);
					}
				}
				else
				{
				        L7_LOG_ERROR (rc);
				}
#else
			        L7_LOG_ERROR(rc);
#endif
			}

			bcmDuplex = (duplex == DAPI_PORT_DUPLEX_FULL) ?
							BCM_PORT_DUPLEX_FULL : BCM_PORT_DUPLEX_HALF;

			rc = bcmx_port_duplex_set(hapiPortPtr->bcmx_lport, bcmDuplex);
			if ((L7_BCMX_OK(rc) != L7_TRUE) && (rc != BCM_E_UNAVAIL))
			        L7_LOG_ERROR(rc);

			rc = bcmx_port_mdix_set(hapiPortPtr->bcmx_lport,BCM_PORT_MDIX_FORCE_AUTO);
			if ((L7_BCMX_OK(rc) != L7_TRUE) && (rc != BCM_E_UNAVAIL))
			        L7_LOG_ERROR(rc);

			rc = bcmx_port_ability_get(hapiPortPtr->bcmx_lport,&local_ability_mask);
			if (L7_BCMX_OK(rc) != L7_TRUE)
			        L7_LOG_ERROR(rc);

			local_ability_mask = (hapiPortPtr->autonegotiationAbilityMask);
			/* If System Flowcontrol is enabled */
			if (dapi_g->system->flowControlEnable)
			{
				if (duplex == DAPI_PORT_DUPLEX_FULL)
				{
					/* Disable PAUSE & enable pack pressure, if port is full duplex */
					local_ability_mask |= (SOC_PM_PAUSE_TX | SOC_PM_PAUSE_RX);
					mac_pause_tx = 1;
			  		mac_pause_rx = 1;
					jam = L7_FALSE;
				}
				else
				{
					/* Enable PAUSE & disable back pressure, if port is half duplex */
					local_ability_mask &= ~(SOC_PM_PAUSE_TX | SOC_PM_PAUSE_RX);
					mac_pause_tx = 0;
				  	mac_pause_rx = 0;
					jam = L7_TRUE;
				}
			}
			else /* System flow control is disabled */
			{
				/* Disable both PAUSE and back pressure for the port */
				local_ability_mask &= ~(SOC_PM_PAUSE_TX | SOC_PM_PAUSE_RX);
				mac_pause_tx = 0;
				mac_pause_rx = 0;
				jam = L7_FALSE;
			}

            /* PTin added: autoneg */
            #if 1
            if (local_ability_mask==0)   local_ability_mask |= local_advert;
            #endif

			rc = bcmx_port_advert_set(hapiPortPtr->bcmx_lport,local_ability_mask);
			if (L7_BCMX_OK(rc) != L7_TRUE)
			        L7_LOG_ERROR(rc);

			rc = bcmx_port_pause_set(hapiPortPtr->bcmx_lport, mac_pause_tx, mac_pause_rx);
			if ((L7_BCMX_OK(rc) != L7_TRUE) && (rc != BCM_E_UNAVAIL))
			        L7_LOG_ERROR(rc);

	        rc = bcmx_port_jam_set(hapiPortPtr->bcmx_lport, jam);
			if ((L7_BCMX_OK(rc) != L7_TRUE) && (rc != BCM_E_UNAVAIL))
			        L7_LOG_ERROR(rc);

			rc = bcmx_port_autoneg_set(hapiPortPtr->bcmx_lport, FALSE);
			if ((L7_BCMX_OK(rc) != L7_TRUE) && (rc != BCM_E_UNAVAIL))
			        L7_LOG_ERROR(rc);
		}
	}
	else  /* Autonegotiation enabled */
	{
		rc = bcmx_port_medium_config_get (hapiPortPtr->bcmx_lport,
				BCM_PORT_MEDIUM_COPPER, &copper_config);
		if (rc == BCM_E_NONE)
		{
			copper_config.autoneg_enable = 1;
			copper_config.mdix = BCM_PORT_MDIX_AUTO;
			/* Set copper preferred as false only if the port is fiber capable */
			copper_config.preferred = !(dapiPortPtr->phyCapabilities & L7_PHY_CAP_PORTSPEED_SFP);

			if(sal_config_get("copper_preffered")!= NULL)
			{
				copper_config.preferred = 1;
			}

			copper_config.autoneg_advert = (hapiPortPtr->autonegotiationAbilityMask);
			/* If system Flowcontrol is enabled */
			if (dapi_g->system->flowControlEnable)
			{
				/* Advertise pause capability in PHY. */
				copper_config.autoneg_advert |= (SOC_PM_PAUSE_TX | SOC_PM_PAUSE_RX);

				/* When port link is UP,
				* for FD port, resolve MAC PAUSE based on local and link partner advertisements
				*      OR
				* for HD port, enable back pressure*/
			}
			else
			{
				/* Clear pause advertisement in PHY.*/
				copper_config.autoneg_advert &= ~(SOC_PM_PAUSE_TX | SOC_PM_PAUSE_RX);				

				/* When port link is UP,
				* disable both PAUSE  & back pressure for this port */
			}
			/* When autonegotiation is enabled MAC pause and jam setting will be done after
			* link comes UP */

			rc = usl_bcmx_port_medium_config_set (hapiPortPtr->bcmx_lport,
					BCM_PORT_MEDIUM_COPPER, &copper_config);
			if (L7_BCMX_OK(rc) != L7_TRUE)
			        L7_LOG_ERROR (rc);
		} else
		{
			/* port-medium-config command is not supported on this PHY. Use different commands.
			 */
			if ((L7_BCMX_OK(rc) != L7_TRUE) && (rc != BCM_E_UNAVAIL))
			        L7_LOG_ERROR (rc);

			/* get the ability of the port */
			rc = bcmx_port_ability_get(hapiPortPtr->bcmx_lport,&local_ability_mask);
			if (L7_BCMX_OK(rc) != L7_TRUE)
			        L7_LOG_ERROR(rc);

			local_ability_mask = (hapiPortPtr->autonegotiationAbilityMask);

			/* If system Flowcontrol is enabled */
			if (dapi_g->system->flowControlEnable)
			{
				/* Advertise pause capability in PHY.*/
				local_ability_mask |= (SOC_PM_PAUSE_TX | SOC_PM_PAUSE_RX);

				/* When port link is UP,
				* for FD port, resolve MAC PAUSE based on local and link partner advertisements
				*      OR
				* for HD port, enable back pressure*/
			}
			else
			{
				/* Clear pause advertisement in PHY. */
				local_ability_mask &= ~(SOC_PM_PAUSE_TX | SOC_PM_PAUSE_RX);				
				/* When port link is UP,
				* disable both PAUSE  & back pressure for this port */
			}

			rc = bcmx_port_advert_set(hapiPortPtr->bcmx_lport,local_ability_mask);
			if (L7_BCMX_OK(rc) != L7_TRUE)
			        L7_LOG_ERROR(rc);

            /* PTin added: autoneg */
            #if 0
            /* get the ability of the port */
            rc = bcmx_port_ability_local_get(hapiPortPtr->bcmx_lport, &port_local_ability);
            if ((L7_BCMX_OK(rc) != L7_TRUE) && (rc != BCM_E_UNAVAIL))
                L7_LOG_ERROR(rc);

            memset(&local_ability,0x00,sizeof(local_ability));

            local_ability.speed_full_duplex  = port_local_ability.speed_full_duplex;
            local_ability.speed_full_duplex &= port_speed_max_mask(speed);
            local_ability.speed_half_duplex  = port_local_ability.speed_half_duplex;
            local_ability.speed_half_duplex &= port_speed_max_mask(speed);

            rc = bcmx_port_ability_advert_set(hapiPortPtr->bcmx_lport, &local_ability);
            if ((L7_BCMX_OK(rc) != L7_TRUE) && (rc != BCM_E_UNAVAIL))
                L7_LOG_ERROR(rc);
            #endif

			/*When autonegotiation is enabled MAC pause and jam setting will be done after
			* link comes UP*/

			rc = bcmx_port_mdix_set(hapiPortPtr->bcmx_lport,BCM_PORT_MDIX_AUTO);
			if ((L7_BCMX_OK(rc) != L7_TRUE) && (rc != BCM_E_UNAVAIL))
			        L7_LOG_ERROR(0);

			/* Enable autoneg on port */
			rc = bcmx_port_autoneg_set(hapiPortPtr->bcmx_lport, TRUE);
			if ((L7_BCMX_OK(rc) != L7_TRUE) && (rc != BCM_E_UNAVAIL))
			        L7_LOG_ERROR(rc);
		}

		/* Enable auto negotiation on Fiber ports too */
		if (dapiPortPtr->phyCapabilities & L7_PHY_CAP_PORTSPEED_SFP)
		{
			rc = bcmx_port_medium_config_get (hapiPortPtr->bcmx_lport,
					BCM_PORT_MEDIUM_FIBER,
					&fiber_config);
			if (rc == BCM_E_NONE)
			{
				/* If fiber mode is disabled on the PHY then re-enable it.*/
				if (hapiPortPtr->hapiModeparm.physical.fiber_mode_disabled == L7_TRUE)
				{
					fiber_config.enable = 1;
				}

				fiber_config.autoneg_enable = 1;
				fiber_config.preferred = 1;
				if(sal_config_get("copper_preffered")!= NULL)
				{
					/* Copper preffered - overwriting */
					fiber_config.preferred = 0;
				}
				fiber_config.force_speed = 1000;
				fiber_config.force_duplex = 1;
				rc = usl_bcmx_port_medium_config_set (hapiPortPtr->bcmx_lport,
						BCM_PORT_MEDIUM_FIBER,
						&fiber_config);
				if (rc == BCM_E_NONE)
				{
					hapiPortPtr->hapiModeparm.physical.fiber_mode_disabled = L7_FALSE;
				}
			}
		}

	}

	if (osapiSemaGive(hapiPortPtr->hapiModeparm.physical.phySemaphore) != L7_SUCCESS)
	{
	        L7_LOG_ERROR(0);
	}
	return result;
}

/*********************************************************************
 *
 * @purpose This routine get the speed and duplex for a phy
 *
 * @param   DAPI_USP_t         *usp    - needs to be a valid usp
 * @param   DAPI_PORT_SPEED_t   speed  - the speed to set the usp to
 * @param   DAPI_PORT_DUPLEX_t  duplex - the duplex to set the usp to
 * @param   DAPI_t             *dapi_g - the driver object
 *
 * @returns L7_RC_t result
 *
 * @notes
 *
 * @end
 *
 *********************************************************************/
L7_RC_t hapiBroadPhyModeGet(DAPI_USP_t *usp, DAPI_PORT_SPEED_t *speed, DAPI_PORT_DUPLEX_t *duplex,
		L7_BOOL *isLinkUp, L7_BOOL *isSfpLink,
		L7_BOOL *isTxPauseAgreed, L7_BOOL *isRxPauseAgreed, DAPI_t *dapi_g)
{
	/* static const char           *routine_name = "hapiBroadPhyModeGet()"; */
	L7_RC_t                      result = L7_SUCCESS;
	BROAD_PORT_t                *hapiPortPtr;
	DAPI_PORT_t                 *dapiPortPtr;
	bcmx_lport_t                 lport;
	bcm_port_info_t              lportInfo;

	memset (&lportInfo, 0, sizeof (lportInfo));

	dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);
	hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

	if (IS_PORT_TYPE_PHYSICAL(dapiPortPtr) == L7_FALSE)
		return L7_FAILURE;

	lport = hapiPortPtr->bcmx_lport;

	lportInfo.action_mask = BCM_PORT_ATTR_LINKSTAT_MASK | BCM_PORT_AN_ATTRS | BCM_PORT_ATTR_MEDIUM_MASK;

	bcmx_port_selective_get(lport, &lportInfo);

	/* update speed */
    /* PTin added: Speed 100G */
    if (lportInfo.speed == 100000)
      *speed = DAPI_PORT_SPEED_GE_100GBPS;
    /* PTin added: Speed 40G */
    else if (lportInfo.speed == 40000)
      *speed = DAPI_PORT_SPEED_GE_40GBPS;
    /* PTin end */
	else if (lportInfo.speed == 10000)
		*speed = DAPI_PORT_SPEED_GE_10GBPS;
    /* PTin added: Speed 2.5G */
    else if (lportInfo.speed == 2500)
      *speed = DAPI_PORT_SPEED_GE_2G5BPS;
    /* PTin end */
	else if (lportInfo.speed == 1000)
		*speed = DAPI_PORT_SPEED_GE_1GBPS;
	else if (lportInfo.speed == 100)
		*speed = DAPI_PORT_SPEED_FE_100MBPS;
	else
		*speed = DAPI_PORT_SPEED_FE_10MBPS;

	/* update duplex */
	if (lportInfo.duplex == BCM_PORT_DUPLEX_FULL)
		*duplex = DAPI_PORT_DUPLEX_FULL;
	else
		*duplex = DAPI_PORT_DUPLEX_HALF;

	*isLinkUp = lportInfo.linkstatus;
	/* update negotiated pause info */
	*isTxPauseAgreed = lportInfo.pause_tx;
	*isRxPauseAgreed = lportInfo.pause_rx;

	/* update selection of sfp */
	if (lportInfo.medium == BCM_PORT_MEDIUM_FIBER)
		*isSfpLink = L7_TRUE;
	else
		*isSfpLink = L7_FALSE;

	return result;
}



/*********************************************************************
 *
 * @purpose This routine set the speed and duplex for a phy
 *
 * @param   DAPI_USP_t  *usp    - needs to be a valid usp
 * @param   L7_BOOL  enable - not yet used in this function
 * @param   DAPI_t      *dapi_g - the driver object
 *
 * @returns L7_RC_t result
 *
 * @notes   none
 *
 * @end
 *
 *********************************************************************/
L7_RC_t hapiBroadPhyLoopbackSet(DAPI_USP_t *usp, L7_BOOL enable, DAPI_t *dapi_g)
{

	L7_RC_t  result = L7_SUCCESS;

	return result;


}



/*********************************************************************
 *
 * @purpose This routine acquires the MDIO interface
 *
 * @param   DAPI_USP_t *usp    - needs to be a valid usp
 * @param   DAPI_t     *dapi_g - the driver object
 *
 * @returns L7_RC_t result
 *
 * @notes   none
 *
 * @end
 *
 *********************************************************************/
L7_RC_t MDIO_RES_GET(DAPI_USP_t *usp, DAPI_t *dapi_g)
{

	L7_RC_t result = L7_SUCCESS;

	return result;


}



/*********************************************************************
 *
 * @purpose This routine release the MDIO interface
 *
 * @param   DAPI_USP_t *usp    - needs to be a valid usp
 * @param   DAPI_t     *dapi_g - the driver object
 *
 * @returns L7_RC_t result
 *
 * @notes   none
 *
 * @end
 *
 *********************************************************************/
L7_RC_t MDIO_RES_FREE(DAPI_USP_t *usp, DAPI_t *dapi_g)
{

	L7_RC_t result = L7_SUCCESS;

	return result;


}


int dump_xgxgs_txacontrol(int unit, int port)
{
	uint8	phy_addr;
	uint16 data;


	/* Setting for HG0 */
	phy_addr = PORT_TO_PHY_ADDR_INT(unit, port);


	SOC_IF_ERROR_RETURN (soc_miim_write(unit, phy_addr, 0x1f, 0x0006));
	SOC_IF_ERROR_RETURN (soc_miim_read(unit, phy_addr, 0x17, &data));
	printf("Block 6 data %x\n", data);

	SOC_IF_ERROR_RETURN (soc_miim_write(unit, phy_addr, 0x1f, 0x0007));
	SOC_IF_ERROR_RETURN (soc_miim_read(unit, phy_addr, 0x17, &data));
	printf("Block 7 data %x\n", data);

	SOC_IF_ERROR_RETURN (soc_miim_write(unit, phy_addr, 0x1f, 0x0008));
	SOC_IF_ERROR_RETURN (soc_miim_read(unit, phy_addr, 0x17, &data));
	printf("Block 8 data %x\n", data);

	SOC_IF_ERROR_RETURN (soc_miim_write(unit, phy_addr, 0x1f, 0x0009));
	SOC_IF_ERROR_RETURN (soc_miim_read(unit, phy_addr, 0x17, &data));
	printf("Block 9 data %x\n", data);

	SOC_IF_ERROR_RETURN (soc_miim_write(unit, phy_addr, 0x1f, 0x00a0));
	SOC_IF_ERROR_RETURN (soc_miim_read(unit, phy_addr, 0x17, &data));
	printf("Block A data %x\n", data);

	return 0;
}

/*********************************************************************
*
* @purpose This routine resolves the MAC pause from PHY pause advertisement
*           of its own and its link partner if link comes UP in full duplex and
*           system flow control is ON . If link comes UP in half duplex, this routine
*           sets the jam if system flow control is ON.
*           If system flow control is OFF, this routine turns off both the MAC pause and jam
*
* @param   DAPI_USP_t  *usp    - needs to be a valid usp
* @param   DAPI_t      *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadMacPauseResolveOnLinkUp(DAPI_USP_t *usp, DAPI_t *dapi_g)
{
	L7_RC_t			result = L7_SUCCESS;
	BROAD_PORT_t	*hapiPortPtr;
	DAPI_PORT_t		*dapiPortPtr;
	bcm_port_info_t	lportInfo;
	L7_int32		rc=0;
	int 			remote_ability_mask, remote_RX=0, remote_TX=0, local_RX=0, local_TX = 0;
	int				mac_pause_tx=0, mac_pause_rx=0, jam=L7_FALSE;
	int 			autoneg, duplex;

	memset (&lportInfo, 0, sizeof (lportInfo));

	dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);
	hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

	if (IS_PORT_TYPE_PHYSICAL(dapiPortPtr) == L7_FALSE)
		return L7_FAILURE;

	bcmx_port_autoneg_get(hapiPortPtr->bcmx_lport, &autoneg);
	bcmx_port_duplex_get(hapiPortPtr->bcmx_lport, &duplex);

	/* If  Autonegotiation Enabled and system flow control is ON.
	* When port link is UP,
	* If port is FD, resolve MAC PAUSE based on local and link partner advertisements
	*      OR
	* If port is HD, enable back pressure for port*/

	/* When link is UP,
	*  1. If system flowcontrol is OFF, disable both PAUSE and back pressure.
	*  2. If system flowcontrol is ON and port is full duplex then disable back pressure.
	*  2a. If port autoneg is ON then resolve PAUSE
	*  2b. If port autoneg is OFF then force PAUSE setting
	*  3. If system flowcontrol is ON and port is half duplex then disable PAUSE and enable back pressure.
	*/

	if (!dapi_g->system->flowControlEnable)
	{
		/* Disable both PAUSE and back pressure */
		mac_pause_tx = 0;
		mac_pause_rx = 0;
		jam = L7_FALSE;
	}
	else
	{
		if (duplex & BCM_PORT_DUPLEX_FULL)
		{
			if (autoneg)
			{
				/* Resolve MAC PAUSE settings from local & remote PHY's PAUSE advertisement*/
				/* local  PHY pause advertisement */
				local_RX = 1;
				local_TX = 1;

				/* get link partner PHY pause advertisement */
				bcmx_port_advert_remote_get(hapiPortPtr->bcmx_lport, &remote_ability_mask);
				remote_RX = (remote_ability_mask & SOC_PM_PAUSE_RX)? 1:0;
				remote_TX = (remote_ability_mask & SOC_PM_PAUSE_TX)? 1:0;

				/* Derive the MAC pause settings from local and remote pause advertisements
				*  based on IEEE 802.3 Table 28B-3. The pause resolution formula for Broadcom
				*  PHY drivers can be found in pause-resolution.txt, stored at esw_sdk/doc/ */
				mac_pause_tx = local_RX *(remote_RX +  remote_TX* !local_TX);
			  	mac_pause_rx = remote_RX *(local_RX + !remote_TX* local_TX);
				/* Disable back pressure */
				jam = L7_FALSE;
			}
			else
			{
				/* Force PAUSE for forced speed Full duplex port & disable back pressure */
				mac_pause_tx = 1;
				mac_pause_rx = 1;
				jam = L7_FALSE;
			}
		}
		else
		{
			/* Enable back pressure and disable PAUSE */
			jam = L7_TRUE;
			mac_pause_tx = 0;
			mac_pause_rx = 0;
		}
	}

	rc = bcmx_port_pause_set(hapiPortPtr->bcmx_lport, mac_pause_tx, mac_pause_rx);
	if ((L7_BCMX_OK(rc) != L7_TRUE) && (rc != BCM_E_UNAVAIL))
	        L7_LOG_ERROR(rc);

	rc = bcmx_port_jam_set(hapiPortPtr->bcmx_lport, jam);
	if ((L7_BCMX_OK(rc) != L7_TRUE) && (rc != BCM_E_UNAVAIL))
	        L7_LOG_ERROR(rc);

	return result;
}
