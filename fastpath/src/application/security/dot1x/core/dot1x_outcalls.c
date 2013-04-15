/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename  dot1x_outcalls.c
*
* @purpose   dot1x outcalls file
*
* @component dot1x
*
* @comments  none
*
* @create    04/08/2003
*
* @author    mfiorito
*
* @end
*             
**********************************************************************/

#include "dot1x_include.h"

/**************************************************************************
* @purpose   Perform necessary outcall initialization
*
* @param     none
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments  Initialize hooks that are necessary for interactions with other
*            components or for platform-specific extensions to dot1x.
*         
* @end
*************************************************************************/
L7_RC_t dot1xOutcallsSysInit()
{
  return L7_SUCCESS;
}

/**************************************************************************
* @purpose   Send key to the Supplicant
*
* @param     intIfNum  @b{(input)} internal interface number
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments  This function is called by the Key Transmit State Machine
*            when a new key is available to be sent to the Supplicant.
*            Code can be added here to transmit key information on
*            platforms that support encryption between NAS (i.e. Access
*            Point) and client (Supplicant).  The key is transmitted to the
*            Supplicant in an EAPOL-Key packet.
* @comments  This code should be executed only when keyTxEnabled has been
*            enabled (set to L7_TRUE).  The event ktxKeyAvailable or
*            ktxKeyTxEnabledAndKeyAvailableAndPortAuthorized should be the
*            only events that trigger the key transmission.
*         
* @end
*************************************************************************/
L7_RC_t dot1xTxKey(L7_uint32 intIfNum)
{
  return L7_SUCCESS;
}

/**************************************************************************
* @purpose   Process a key received from the Supplicant
*
* @param     intIfNum  @b{(input)} internal interface number
*
* @returns   L7_SUCCESS
* @returns   L7_FAILURE
*
* @comments  This function is called by the Key Receive State Machine
*            when a new key is received from the Supplicant.  The key is
*            transmitted in an EAPOL-Key packet which is contained in the
*            bufHandle parameter to this function.  Code can be added here
*            to process key information on platforms that support encryption
*            between NAS (i.e. Access Point) and client (Supplicant).
* @comments  This krxRxKey event should be the only event that triggers
*            received key processing.
*         
* @end
*************************************************************************/
L7_RC_t dot1xProcessKey(L7_uint32 intIfNum, L7_netBufHandle bufHandle)
{
  return L7_SUCCESS;
}
