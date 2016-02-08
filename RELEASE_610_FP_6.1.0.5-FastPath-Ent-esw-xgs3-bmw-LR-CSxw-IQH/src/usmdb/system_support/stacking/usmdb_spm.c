/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename usmdb_spm.c
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

#include "l7_common.h"
#include "usmdb_spm_api.h"
#include "spm_api.h"
#include <stdio.h>
#include <string.h>

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
                               L7_uint32 * index)
{

return spmFpsIndexFromUspGet (unit, slot, port, index);
}

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
L7_RC_t usmdbSpmFpsConfigQosModeGet (L7_uint32 * mode)
{
 return spmFpsConfigQosModeGet (mode);
}

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
L7_RC_t usmdbSpmFpsConfigQosModeSet (L7_uint32 mode)
{
   return spmFpsConfigQosModeSet (mode);
}

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
                                    L7_uint32 mode)
{
  return spmFpsConfigStackingModeSet (index, mode);
}



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
				SPM_STACK_PORT_ENTRY_t *port_entry)
{
  return spmStackingCapablePortGet (index, port_entry);
}

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
L7_RC_t usmdbSpmStackingCapablePortNextGet (SPM_STACK_PORT_ENTRY_t *port_entry)
{
  return spmStackingCapablePortNextGet (port_entry);
}

/**********************************************************************
**** Debug Functions.
**********************************************************************/

/*****************************************************
**
** Print information about the specified stack port.
**
*****************************************************/
static void usmdbDebugSpmPrint (SPM_STACK_PORT_ENTRY_t *port_entry)
{
  printf("SNMP Index:         %d\n", port_entry->snmp_index);
  printf("Unit:               %d\n", port_entry->unit);
  printf("FPS Flag:           %d\n", port_entry->fps_port);
  printf("   FPS Slot:        %d\n", port_entry->slot);
  printf("   FPS Port:        %d\n", port_entry->port);
  printf("Port Tag:           %s\n", port_entry->port_tag); 

  printf("\n");
  printf("Stack Config Mode:  %d\n", port_entry->port_info.config_mode);
  printf("Stack Runtime Mode: %d\n", port_entry->port_info.runtime_mode);
  printf("Stack QOS Mode:     %d\n", port_entry->port_info.qos_mode);
  printf("\n");
  printf("Link Status:        %d\n", port_entry->port_info.link_status);
  printf("Link Speed (Gb/s):  %d\n", port_entry->port_info.speed);
  printf("\n");
  printf("Tx Data Rate (Mb/s):      %d\n", port_entry->port_info.tx_data_rate);
  printf("Rx Data Rate (Mb/s):      %d\n", port_entry->port_info.rx_data_rate);
  printf("Tx Error Rate (Errors/s): %d\n", port_entry->port_info.tx_error_rate);
  printf("Rx Error Rate (Errors/s): %d\n", port_entry->port_info.rx_error_rate);
  printf("Tx Total Errors:          %d\n", port_entry->port_info.tx_total_errors);
  printf("Rx Total Errors:          %d\n", port_entry->port_info.rx_total_errors);
  printf("\n");
  printf("Diag Field 1: %s\n", port_entry->port_info.info1);
  printf("Diag Field 2: %s\n", port_entry->port_info.info2);
  printf("Diag Field 3: %s\n", port_entry->port_info.info3);
       
}

/************************************
**
** Get information about specific stack port.
**
************************************/
void usmdbDebugSpmGet (L7_uint32 index)
{
  L7_RC_t rc;
  SPM_STACK_PORT_ENTRY_t port_entry;

  memset (&port_entry, 0, sizeof (port_entry));

  rc = usmdbSpmStackingCapablePortGet (index, &port_entry);
  if (rc != L7_SUCCESS)
  {
    printf("Stack port %d doesn't exist.\n", index);
    return;
  }

  usmdbDebugSpmPrint (&port_entry);
}

/************************************
**
** List all stack ports in the system.
**
************************************/
void usmdbDebugSpmAllGet (void)
{
  L7_RC_t rc;
  SPM_STACK_PORT_ENTRY_t port_entry;
  L7_uint32 num_ports = 0;

  memset (&port_entry, 0, sizeof (port_entry));

  do
  {
    rc = usmdbSpmStackingCapablePortNextGet (&port_entry);
    if (rc != L7_SUCCESS)
    {
      printf("\n\nTotal Number of Entries: %d\n", num_ports);
      return;
    }

    num_ports++;
    printf("\n---------------------------\n");
    printf("Entry Number: %d\n", num_ports);
    printf("---------------------------\n");

    usmdbDebugSpmPrint (&port_entry);

  } while (rc == L7_SUCCESS);

}
