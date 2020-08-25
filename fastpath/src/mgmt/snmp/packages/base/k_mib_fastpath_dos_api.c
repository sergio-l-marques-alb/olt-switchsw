/*********************************************************************
*
* (C) Copyright Broadcom Corporation 1999-2007
*
**********************************************************************
*
* @filename   k_mib_fastpath_denialofservice_api.c
*
* @purpose    Wrapper functions for Fastpath Denial of Service MIB
*
* @component  SNMP
*
* @comments
*
* @create     09/1/2005
*
* @author     esmiley
* @end
*
**********************************************************************/
#include "k_private_base.h"

#include "k_mib_fastpath_dos_api.h"
#include "usmdb_dos_api.h"


/* Begin Function Definitions: k_mib_fastpath_denialofservice_api.c */

/* Denial Of Service Functions */

L7_RC_t
snmpAgentSwitchDenialOfServiceSIPDIPModeGet ( L7_uint32 UnitIndex, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmdbDoSSIPDIPModeGet ( &temp_val );

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_agentSwitchDenialOfServiceSIPDIPMode_enable;
      break;

    case L7_DISABLE:
      *val = D_agentSwitchDenialOfServiceSIPDIPMode_disable;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}


L7_RC_t
snmpAgentSwitchDenialOfServiceSIPDIPModeSet  ( L7_uint32 UnitIndex, L7_int32 val )
{
  L7_uint32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentSwitchDenialOfServiceSIPDIPMode_enable:
    temp_val = L7_ENABLE;
    break;

  case D_agentSwitchDenialOfServiceSIPDIPMode_disable:
    temp_val = L7_DISABLE;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmdbDoSSIPDIPSet ( UnitIndex, temp_val );
  }

  return rc;
}


L7_RC_t
snmpAgentSwitchDenialOfServiceSMACDMACModeGet ( L7_uint32 UnitIndex, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmdbDoSSMACDMACModeGet ( &temp_val );

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_agentSwitchDenialOfServiceSMACDMACMode_enable;
      break;

    case L7_DISABLE:
      *val = D_agentSwitchDenialOfServiceSMACDMACMode_disable;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}


L7_RC_t
snmpAgentSwitchDenialOfServiceSMACDMACModeSet  ( L7_uint32 UnitIndex, L7_int32 val )
{
  L7_uint32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentSwitchDenialOfServiceSMACDMACMode_enable:
    temp_val = L7_ENABLE;
    break;

  case D_agentSwitchDenialOfServiceSMACDMACMode_disable:
    temp_val = L7_DISABLE;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmdbDoSSMACDMACSet ( UnitIndex, temp_val );
  }

  return rc;
}

L7_RC_t
snmpAgentSwitchDenialOfServiceFirstFragModeGet ( L7_uint32 UnitIndex, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmdbDoSFirstFragModeGet ( &temp_val );

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_agentSwitchDenialOfServiceFirstFragMode_enable;
      break;

    case L7_DISABLE:
      *val = D_agentSwitchDenialOfServiceFirstFragMode_disable;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}


L7_RC_t
snmpAgentSwitchDenialOfServiceFirstFragModeSet  ( L7_uint32 UnitIndex, L7_int32 val )
{
  L7_uint32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentSwitchDenialOfServiceFirstFragMode_enable:
    temp_val = L7_ENABLE;
    break;

  case D_agentSwitchDenialOfServiceFirstFragMode_disable:
    temp_val = L7_DISABLE;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmdbDoSFirstFragModeSet ( UnitIndex, temp_val );
  }

  return rc;
}


L7_RC_t
snmpAgentSwitchDenialOfServiceTCPHdrSizeGet ( L7_uint32 UnitIndex, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmdbDoSFirstFragSizeGet ( &temp_val );

  if (rc == L7_SUCCESS)
  {
    *val = temp_val;
  }

  return rc;
}


L7_RC_t
snmpAgentSwitchDenialOfServiceTCPHdrSizeSet  ( L7_uint32 UnitIndex, L7_int32 val )
{
  L7_RC_t rc = L7_SUCCESS;

  rc = usmdbDoSFirstFragSizeSet ( UnitIndex, val );

  return rc;
}


L7_RC_t
snmpAgentSwitchDenialOfServiceTCPFragModeGet ( L7_uint32 UnitIndex, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmdbDoSTCPFragModeGet ( &temp_val );

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_agentSwitchDenialOfServiceTCPFragMode_enable;
      break;

    case L7_DISABLE:
      *val = D_agentSwitchDenialOfServiceTCPFragMode_disable;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}


L7_RC_t
snmpAgentSwitchDenialOfServiceTCPFragModeSet  ( L7_uint32 UnitIndex, L7_int32 val )
{
  L7_uint32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentSwitchDenialOfServiceTCPFragMode_enable:
    temp_val = L7_ENABLE;
    break;

  case D_agentSwitchDenialOfServiceTCPFragMode_disable:
    temp_val = L7_DISABLE;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmdbDoSTCPFragSet ( UnitIndex, temp_val );
  }

  return rc;
}


L7_RC_t
snmpAgentSwitchDenialOfServiceTCPFlagModeGet ( L7_uint32 UnitIndex, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmdbDoSTCPFlagModeGet ( &temp_val );

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_agentSwitchDenialOfServiceTCPFlagMode_enable;
      break;

    case L7_DISABLE:
      *val = D_agentSwitchDenialOfServiceTCPFlagMode_disable;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentSwitchDenialOfServiceTCPFlagModeSet  ( L7_uint32 UnitIndex, L7_int32 val )
{
  L7_uint32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentSwitchDenialOfServiceTCPFlagMode_enable:
    temp_val = L7_ENABLE;
    break;

  case D_agentSwitchDenialOfServiceTCPFlagMode_disable:
    temp_val = L7_DISABLE;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmdbDoSTCPFlagSet ( UnitIndex, temp_val );
  }

  return rc;
}

L7_RC_t
snmpAgentSwitchDenialOfServiceTCPFlagSeqModeGet ( L7_uint32 UnitIndex, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmdbDoSTCPFlagSeqModeGet ( &temp_val );

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_agentSwitchDenialOfServiceTCPFlagSeqMode_enable;
      break;

    case L7_DISABLE:
      *val = D_agentSwitchDenialOfServiceTCPFlagSeqMode_disable;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentSwitchDenialOfServiceTCPFlagSeqModeSet  ( L7_uint32 UnitIndex, L7_int32 val )
{
  L7_uint32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentSwitchDenialOfServiceTCPFlagSeqMode_enable:
    temp_val = L7_ENABLE;
    break;

  case D_agentSwitchDenialOfServiceTCPFlagSeqMode_disable:
    temp_val = L7_DISABLE;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmdbDoSTCPFlagSeqSet ( UnitIndex, temp_val );
  }

  return rc;
}

L7_RC_t
snmpAgentSwitchDenialOfServiceTCPOffsetModeGet ( L7_uint32 UnitIndex, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmdbDoSTCPOffsetModeGet ( &temp_val );

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_agentSwitchDenialOfServiceTCPOffsetMode_enable;
      break;

    case L7_DISABLE:
      *val = D_agentSwitchDenialOfServiceTCPOffsetMode_disable;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentSwitchDenialOfServiceTCPOffsetModeSet  ( L7_uint32 UnitIndex, L7_int32 val )
{
  L7_uint32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentSwitchDenialOfServiceTCPOffsetMode_enable:
    temp_val = L7_ENABLE;
    break;

  case D_agentSwitchDenialOfServiceTCPOffsetMode_disable:
    temp_val = L7_DISABLE;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmdbDoSTCPOffsetSet ( UnitIndex, temp_val );
  }

  return rc;
}

L7_RC_t
snmpAgentSwitchDenialOfServiceTCPSynModeGet ( L7_uint32 UnitIndex, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmdbDoSTCPSynModeGet ( &temp_val );

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_agentSwitchDenialOfServiceTCPSynMode_enable;
      break;

    case L7_DISABLE:
      *val = D_agentSwitchDenialOfServiceTCPSynMode_disable;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentSwitchDenialOfServiceTCPSynModeSet  ( L7_uint32 UnitIndex, L7_int32 val )
{
  L7_uint32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentSwitchDenialOfServiceTCPSynMode_enable:
    temp_val = L7_ENABLE;
    break;

  case D_agentSwitchDenialOfServiceTCPSynMode_disable:
    temp_val = L7_DISABLE;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmdbDoSTCPSynSet ( UnitIndex, temp_val );
  }

  return rc;
}

L7_RC_t
snmpAgentSwitchDenialOfServiceTCPSynFinModeGet ( L7_uint32 UnitIndex, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmdbDoSTCPSynFinModeGet ( &temp_val );

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_agentSwitchDenialOfServiceTCPSynFinMode_enable;
      break;

    case L7_DISABLE:
      *val = D_agentSwitchDenialOfServiceTCPSynFinMode_disable;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentSwitchDenialOfServiceTCPSynFinModeSet  ( L7_uint32 UnitIndex, L7_int32 val )
{
  L7_uint32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentSwitchDenialOfServiceTCPSynFinMode_enable:
    temp_val = L7_ENABLE;
    break;

  case D_agentSwitchDenialOfServiceTCPSynFinMode_disable:
    temp_val = L7_DISABLE;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmdbDoSTCPSynFinSet ( UnitIndex, temp_val );
  }

  return rc;
}

L7_RC_t
snmpAgentSwitchDenialOfServiceTCPFinUrgPshModeGet ( L7_uint32 UnitIndex, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmdbDoSTCPFinUrgPshModeGet ( &temp_val );

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_agentSwitchDenialOfServiceTCPFinUrgPshMode_enable;
      break;

    case L7_DISABLE:
      *val = D_agentSwitchDenialOfServiceTCPFinUrgPshMode_disable;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}

L7_RC_t
snmpAgentSwitchDenialOfServiceTCPFinUrgPshModeSet  ( L7_uint32 UnitIndex, L7_int32 val )
{
  L7_uint32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentSwitchDenialOfServiceTCPFinUrgPshMode_enable:
    temp_val = L7_ENABLE;
    break;

  case D_agentSwitchDenialOfServiceTCPFinUrgPshMode_disable:
    temp_val = L7_DISABLE;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmdbDoSTCPFinUrgPshSet ( UnitIndex, temp_val );
  }

  return rc;
}


L7_RC_t
snmpAgentSwitchDenialOfServiceL4PortModeGet ( L7_uint32 UnitIndex, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmdbDoSL4PortModeGet ( &temp_val );

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_agentSwitchDenialOfServiceL4PortMode_enable;
      break;

    case L7_DISABLE:
      *val = D_agentSwitchDenialOfServiceL4PortMode_disable;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}


L7_RC_t
snmpAgentSwitchDenialOfServiceL4PortModeSet  ( L7_uint32 UnitIndex, L7_int32 val )
{
  L7_uint32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentSwitchDenialOfServiceL4PortMode_enable:
    temp_val = L7_ENABLE;
    break;

  case D_agentSwitchDenialOfServiceL4PortMode_disable:
    temp_val = L7_DISABLE;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmdbDoSL4PortSet ( UnitIndex, temp_val );
  }

  return rc;
}

L7_RC_t
snmpAgentSwitchDenialOfServiceUDPPortModeGet ( L7_uint32 UnitIndex, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmdbDoSUDPPortModeGet ( &temp_val );

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_agentSwitchDenialOfServiceUDPPortMode_enable;
      break;

    case L7_DISABLE:
      *val = D_agentSwitchDenialOfServiceUDPPortMode_disable;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}


L7_RC_t
snmpAgentSwitchDenialOfServiceUDPPortModeSet  ( L7_uint32 UnitIndex, L7_int32 val )
{
  L7_uint32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentSwitchDenialOfServiceUDPPortMode_enable:
    temp_val = L7_ENABLE;
    break;

  case D_agentSwitchDenialOfServiceUDPPortMode_disable:
    temp_val = L7_DISABLE;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmdbDoSUDPPortSet ( UnitIndex, temp_val );
  }

  return rc;
}

L7_RC_t
snmpAgentSwitchDenialOfServiceTCPPortModeGet ( L7_uint32 UnitIndex, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmdbDoSTCPPortModeGet ( &temp_val );

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_agentSwitchDenialOfServiceTCPPortMode_enable;
      break;

    case L7_DISABLE:
      *val = D_agentSwitchDenialOfServiceTCPPortMode_disable;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}


L7_RC_t
snmpAgentSwitchDenialOfServiceTCPPortModeSet  ( L7_uint32 UnitIndex, L7_int32 val )
{
  L7_uint32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentSwitchDenialOfServiceTCPPortMode_enable:
    temp_val = L7_ENABLE;
    break;

  case D_agentSwitchDenialOfServiceTCPPortMode_disable:
    temp_val = L7_DISABLE;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmdbDoSTCPPortSet ( UnitIndex, temp_val );
  }

  return rc;
}


L7_RC_t
snmpAgentSwitchDenialOfServiceICMPModeGet ( L7_uint32 UnitIndex, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmdbDoSICMPModeGet ( &temp_val );

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_agentSwitchDenialOfServiceICMPMode_enable;
      break;

    case L7_DISABLE:
      *val = D_agentSwitchDenialOfServiceICMPMode_disable;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}


L7_RC_t
snmpAgentSwitchDenialOfServiceICMPModeSet  ( L7_uint32 UnitIndex, L7_int32 val )
{
  L7_uint32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentSwitchDenialOfServiceICMPMode_enable:
    temp_val = L7_ENABLE;
    break;

  case D_agentSwitchDenialOfServiceICMPMode_disable:
    temp_val = L7_DISABLE;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmdbDoSICMPModeSet ( UnitIndex, temp_val );
  }

  return rc;
}

L7_RC_t
snmpAgentSwitchDenialOfServiceICMPSizeGet ( L7_uint32 UnitIndex, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmdbDoSICMPSizeGet ( &temp_val );

  if (rc == L7_SUCCESS)
  {
    *val = temp_val;
  }

  return rc;
}


L7_RC_t
snmpAgentSwitchDenialOfServiceICMPSizeSet  ( L7_uint32 UnitIndex, L7_int32 val )
{
  L7_RC_t rc = L7_SUCCESS;

  rc = usmdbDoSICMPSizeSet ( UnitIndex, val );

  return rc;
}

L7_RC_t
snmpAgentSwitchDenialOfServiceICMPv6SizeGet ( L7_uint32 UnitIndex, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmdbDoSICMPv6SizeGet ( &temp_val );

  if (rc == L7_SUCCESS)
  {
    *val = temp_val;
  }

  return rc;
}


L7_RC_t
snmpAgentSwitchDenialOfServiceICMPv6SizeSet  ( L7_uint32 UnitIndex, L7_int32 val )
{
  L7_RC_t rc = L7_SUCCESS;

  rc = usmdbDoSICMPv6SizeSet ( UnitIndex, val );

  return rc;
}

L7_RC_t
snmpAgentSwitchDenialOfServiceICMPFragModeGet ( L7_uint32 UnitIndex, L7_int32 *val )
{
  L7_uint32 temp_val;
  L7_RC_t rc;

  rc = usmdbDoSICMPFragModeGet ( &temp_val );

  if (rc == L7_SUCCESS)
  {
    switch (temp_val)
    {
    case L7_ENABLE:
      *val = D_agentSwitchDenialOfServiceICMPFragMode_enable;
      break;

    case L7_DISABLE:
      *val = D_agentSwitchDenialOfServiceICMPFragMode_disable;
      break;

    default:
      *val = 0;
      rc = L7_FAILURE;
    }
  }

  return rc;
}


L7_RC_t
snmpAgentSwitchDenialOfServiceICMPFragModeSet  ( L7_uint32 UnitIndex, L7_int32 val )
{
  L7_uint32 temp_val = 0;
  L7_RC_t rc = L7_SUCCESS;

  switch (val)
  {
  case D_agentSwitchDenialOfServiceICMPFragMode_enable:
    temp_val = L7_ENABLE;
    break;

  case D_agentSwitchDenialOfServiceICMPFragMode_disable:
    temp_val = L7_DISABLE;
    break;

  default:
    rc = L7_FAILURE;
  }

  if (rc == L7_SUCCESS)
  {
    rc = usmdbDoSICMPFragModeSet ( UnitIndex, temp_val );
  }

  return rc;
}


/* END of Denial Of Service Functions */
/****************************************************************************************/

