/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename usmdb_spm_api.h
*
* @purpose USMDB Stack Port Manager
*
* @component SPM
*
* @comments none
*
* @create 07/28/2004
*
* @author Andrey Tsigler
*
* @end
*             
**********************************************************************/

#ifndef USMDB_SPM_API_H
#define USMDB_SPM_API_H

/* Port tag length.
*/
#define SPM_STACK_PORT_TAG_SIZE  16

/* Length of the free-form information field.
*/
#define SPM_FREE_FORM_INFO_SIZE   80

typedef struct 
{
  L7_uint32 config_mode; /* 1 - Stack, 0 - Ethernet */
  L7_uint32 runtime_mode; /* 1 - Stack, 0 - Ethernet */

  L7_uint32 qos_mode;  /* 1 - Special QOS handling enabled. */
                       /* 0 - Special QOS handling disabled. */
  

  L7_BOOL  link_status; /* 1 - Link Up, 0 - Link Down */
  L7_uint32 speed;      /* Speed in Gb/s */
  
  L7_uint32 tx_data_rate; /* Estimated data rate in Gb/s */
  L7_uint32 tx_error_rate; /* Estimated error rate in Gb/s */
  L7_uint32 tx_total_errors; /* Total number of errors since boot */

  L7_uint32 rx_data_rate; /* Estimated data rate in Gb/s */
  L7_uint32 rx_error_rate; /* Estimated error rate in Gb/s */
  L7_uint32 rx_total_errors; /* Total number of errors since boot */

  L7_uchar8 info1 [SPM_FREE_FORM_INFO_SIZE];
  L7_uchar8 info2 [SPM_FREE_FORM_INFO_SIZE];
  L7_uchar8 info3 [SPM_FREE_FORM_INFO_SIZE];

  L7_uint32 pad [16]; /* Future enhancement */


} SPM_STACK_PORT_INFO_t;

typedef struct 
{
  L7_uint32 snmp_index; /* SNMP Index uniquely identifying this port. */


  L7_uint32 unit;   /* Unit number of this port */

  /* Name of this port.
  */
  L7_uchar8 port_tag [SPM_STACK_PORT_TAG_SIZE];

  L7_BOOL   fps_port; /* L7_TRUE - Front panel stacking port */
                      /* L7_FALSE - Dedicated stacking port */

  L7_uint32 slot, port; /* Slot and port of FPS ports. */
                        /* 0,0 for dedicated stacking ports */

  SPM_STACK_PORT_INFO_t port_info;
} SPM_STACK_PORT_ENTRY_t;

/*********************************************************************
* @purpose  Find front-panel port that matches the specified USP.
*
* @param    unit - unit of the front-panel stack port.
* @param    slot - slot of the front-panel stack port.
* @param    port - port of the front-panel stack port.
* @param    index - Output: Index of the matching port.
*
* @returns  L7_SUCCESS - Entry is found.
*           L7_FAILURE - Stacking port with matching index is not found.
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t usmdbSpmFpsIndexFromUspGet (L7_uint32 unit,
                               L7_uint32 slot,
                               L7_uint32 port,
                               L7_uint32 * index);

/*********************************************************************
* @purpose  Get QOS configuration for all front-panel stacking ports.
*           stack port.
*
* @param    enable - 1 - Enable QOS Mode
*                    0 - Disable QOS mode.
*
* @returns  L7_SUCCESS - Entry is found.
*           L7_FAILURE - Stacking port with matching index is not found.
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t usmdbSpmFpsConfigQosModeGet (L7_uint32 * mode);

/*********************************************************************
* @purpose  Set QOS configuration for all front-panel stacking ports.
*           stack port.
*
* @param    enable - 1 - Enable QOS Mode
*                    0 - Disable QOS mode.
*
* @returns  L7_SUCCESS - Entry is found.
*           L7_FAILURE - Stacking port with matching index is not found.
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t usmdbSpmFpsConfigQosModeSet (L7_uint32 mode);

/*********************************************************************
* @purpose  Set stacking mode configuration for a front-panel 
*           stack port.
*
* @param    index - SNMP index for the stack port.
* @param    enable - 1 - Enable Stacking Mode
*                    0 - Disable Stacking mode.
*
* @returns  L7_SUCCESS - Entry is found.
*           L7_FAILURE - Stacking port with matching index is not found.
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t usmdbSpmFpsConfigStackingModeSet (L7_uint32 index,
                                    L7_uint32 mode);



/*********************************************************************
* @purpose  Retrieve stack port information for the specified index.
*
* @param    index  - SNMP index of the port.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t usmdbSpmStackingCapablePortGet (L7_uint32 index,
				SPM_STACK_PORT_ENTRY_t *port_entry);

/*********************************************************************
* @purpose  Retrieve stack port information for the next port.
*
* @param    port_info - In - Current port info.
* 			Out - Next Port Info.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t usmdbSpmStackingCapablePortNextGet (SPM_STACK_PORT_ENTRY_t *port_entry);

#endif

