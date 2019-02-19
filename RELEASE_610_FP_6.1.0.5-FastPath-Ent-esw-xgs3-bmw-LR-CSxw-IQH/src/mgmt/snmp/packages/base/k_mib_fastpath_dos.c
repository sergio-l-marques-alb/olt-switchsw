/*********************************************************************
*
* (C) Copyright Broadcom Corporation 1999-2007
*
**********************************************************************
*
* @filename   k_mib_fastpath_denialofservice.c
*
* @purpose    Denial of Service agent code 
*
* @component  SNMP
*
* @comments
*
* @create    09/1/2005
*
* @author     esmiley
* @end
*
**********************************************************************/
#include "k_private_base.h"
#include "k_mib_fastpath_dos_api.h"
#include "usmdb_common.h"
#include "doscontrol_exports.h"
#include "usmdb_util_api.h"

agentSwitchDenialOfServiceGroup_t *
k_agentSwitchDenialOfServiceGroup_get(int serialNum, ContextInfo *contextInfo,
                                      int nominator)
{
  static agentSwitchDenialOfServiceGroup_t agentSwitchDenialOfServiceGroupData;

  ZERO_VALID(agentSwitchDenialOfServiceGroupData.valid);

  /*
   * if ( nominator != -1 ) condition is added to all the case statements
   * for storing all the values to support the undo functionality.
   */

  switch (nominator)
  {
  case -1:
    if (nominator != -1) break;
    /* else pass through */

  case I_agentSwitchDenialOfServiceSIPDIPMode :
    CLR_VALID(I_agentSwitchDenialOfServiceSIPDIPMode, agentSwitchDenialOfServiceGroupData.valid);
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_SIPDIP_FEATURE_ID) == L7_TRUE)
    {
       if (snmpAgentSwitchDenialOfServiceSIPDIPModeGet(USMDB_UNIT_CURRENT,
                                               &agentSwitchDenialOfServiceGroupData.agentSwitchDenialOfServiceSIPDIPMode) == L7_SUCCESS)
         SET_VALID(I_agentSwitchDenialOfServiceSIPDIPMode, agentSwitchDenialOfServiceGroupData.valid);
    }
    if (nominator != -1) break;
    /* else pass through */

  case I_agentSwitchDenialOfServiceFirstFragMode :
    CLR_VALID(I_agentSwitchDenialOfServiceFirstFragMode, agentSwitchDenialOfServiceGroupData.valid);
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_FIRSTFRAG_FEATURE_ID) == L7_TRUE)
    {
       if (snmpAgentSwitchDenialOfServiceFirstFragModeGet(USMDB_UNIT_CURRENT,
                                               &agentSwitchDenialOfServiceGroupData.agentSwitchDenialOfServiceFirstFragMode) == L7_SUCCESS)
         SET_VALID(I_agentSwitchDenialOfServiceFirstFragMode, agentSwitchDenialOfServiceGroupData.valid);
    }
    if (nominator != -1) break;
    /* else pass through */

  case I_agentSwitchDenialOfServiceTCPHdrSize :
    CLR_VALID(I_agentSwitchDenialOfServiceTCPHdrSize, agentSwitchDenialOfServiceGroupData.valid);
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_FIRSTFRAG_FEATURE_ID) == L7_TRUE)
    {
       if (snmpAgentSwitchDenialOfServiceTCPHdrSizeGet(USMDB_UNIT_CURRENT,
                                               &agentSwitchDenialOfServiceGroupData.agentSwitchDenialOfServiceTCPHdrSize) == L7_SUCCESS)
         SET_VALID(I_agentSwitchDenialOfServiceTCPHdrSize, agentSwitchDenialOfServiceGroupData.valid);
    }
    if (nominator != -1) break;
    /* else pass through */

  case I_agentSwitchDenialOfServiceTCPFragMode :
    CLR_VALID(I_agentSwitchDenialOfServiceTCPFragMode, agentSwitchDenialOfServiceGroupData.valid);
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_TCPFRAG_FEATURE_ID) == L7_TRUE)
    {
       if (snmpAgentSwitchDenialOfServiceTCPFragModeGet(USMDB_UNIT_CURRENT,
                                               &agentSwitchDenialOfServiceGroupData.agentSwitchDenialOfServiceTCPFragMode) == L7_SUCCESS)
         SET_VALID(I_agentSwitchDenialOfServiceTCPFragMode, agentSwitchDenialOfServiceGroupData.valid);
    }
    if (nominator != -1) break;
    /* else pass through */

  case I_agentSwitchDenialOfServiceTCPFlagMode :
    CLR_VALID(I_agentSwitchDenialOfServiceTCPFlagMode, agentSwitchDenialOfServiceGroupData.valid);
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_TCPFLAG_FEATURE_ID) == L7_TRUE)
    {
       if (snmpAgentSwitchDenialOfServiceTCPFlagModeGet(USMDB_UNIT_CURRENT,
                                               &agentSwitchDenialOfServiceGroupData.agentSwitchDenialOfServiceTCPFlagMode) == L7_SUCCESS)
         SET_VALID(I_agentSwitchDenialOfServiceTCPFlagMode, agentSwitchDenialOfServiceGroupData.valid);
    }
    if (nominator != -1) break;
    /* else pass through */

  case I_agentSwitchDenialOfServiceL4PortMode :
    CLR_VALID(I_agentSwitchDenialOfServiceL4PortMode, agentSwitchDenialOfServiceGroupData.valid);
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_L4PORT_FEATURE_ID) == L7_TRUE)
    {
       if (snmpAgentSwitchDenialOfServiceL4PortModeGet(USMDB_UNIT_CURRENT,
                                               &agentSwitchDenialOfServiceGroupData.agentSwitchDenialOfServiceL4PortMode) == L7_SUCCESS)
         SET_VALID(I_agentSwitchDenialOfServiceL4PortMode, agentSwitchDenialOfServiceGroupData.valid);
    }
    if (nominator != -1) break;
    /* else pass through */  

  case I_agentSwitchDenialOfServiceICMPMode :
    CLR_VALID(I_agentSwitchDenialOfServiceICMPMode, agentSwitchDenialOfServiceGroupData.valid);
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_ICMP_FEATURE_ID) == L7_TRUE)
    {
       if (snmpAgentSwitchDenialOfServiceICMPModeGet(USMDB_UNIT_CURRENT,
                                               &agentSwitchDenialOfServiceGroupData.agentSwitchDenialOfServiceICMPMode) == L7_SUCCESS)
         SET_VALID(I_agentSwitchDenialOfServiceICMPMode, agentSwitchDenialOfServiceGroupData.valid);
       if (nominator != -1) break;
    /* else pass through */
    }

  case I_agentSwitchDenialOfServiceICMPSize :
    CLR_VALID(I_agentSwitchDenialOfServiceICMPSize, agentSwitchDenialOfServiceGroupData.valid);
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_ICMP_FEATURE_ID) == L7_TRUE)
    {
       if (snmpAgentSwitchDenialOfServiceICMPSizeGet(USMDB_UNIT_CURRENT,
                                               &agentSwitchDenialOfServiceGroupData.agentSwitchDenialOfServiceICMPSize) == L7_SUCCESS)
         SET_VALID(I_agentSwitchDenialOfServiceICMPSize, agentSwitchDenialOfServiceGroupData.valid);
    }
    break;

  case I_agentSwitchDenialOfServiceSMACDMACMode :
    CLR_VALID(I_agentSwitchDenialOfServiceSMACDMACMode, agentSwitchDenialOfServiceGroupData.valid);
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_ICMP_FEATURE_ID) == L7_TRUE)
    {
       if (snmpAgentSwitchDenialOfServiceSMACDMACModeGet(USMDB_UNIT_CURRENT,
                                               &agentSwitchDenialOfServiceGroupData.agentSwitchDenialOfServiceSMACDMACMode) == L7_SUCCESS)
         SET_VALID(I_agentSwitchDenialOfServiceSMACDMACMode, agentSwitchDenialOfServiceGroupData.valid);
    }
    break;

  case I_agentSwitchDenialOfServiceTCPOffsetMode :
    CLR_VALID(I_agentSwitchDenialOfServiceTCPOffsetMode, agentSwitchDenialOfServiceGroupData.valid);
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_ICMP_FEATURE_ID) == L7_TRUE)
    {
       if (snmpAgentSwitchDenialOfServiceTCPOffsetModeGet(USMDB_UNIT_CURRENT,
                                               &agentSwitchDenialOfServiceGroupData.agentSwitchDenialOfServiceTCPOffsetMode) == L7_SUCCESS)
         SET_VALID(I_agentSwitchDenialOfServiceTCPOffsetMode, agentSwitchDenialOfServiceGroupData.valid);
    }
    break;

  case I_agentSwitchDenialOfServiceTCPFlagSeqMode :
    CLR_VALID(I_agentSwitchDenialOfServiceTCPFlagSeqMode, agentSwitchDenialOfServiceGroupData.valid);
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_ICMP_FEATURE_ID) == L7_TRUE)
    {
       if (snmpAgentSwitchDenialOfServiceTCPFlagSeqModeGet(USMDB_UNIT_CURRENT,
                                               &agentSwitchDenialOfServiceGroupData.agentSwitchDenialOfServiceTCPFlagSeqMode) == L7_SUCCESS)
         SET_VALID(I_agentSwitchDenialOfServiceTCPFlagSeqMode, agentSwitchDenialOfServiceGroupData.valid);
    }
    break;

  case I_agentSwitchDenialOfServiceTCPPortMode :
    CLR_VALID(I_agentSwitchDenialOfServiceTCPPortMode, agentSwitchDenialOfServiceGroupData.valid);
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_ICMP_FEATURE_ID) == L7_TRUE)
    {
       if (snmpAgentSwitchDenialOfServiceTCPPortModeGet(USMDB_UNIT_CURRENT,
                                               &agentSwitchDenialOfServiceGroupData.agentSwitchDenialOfServiceTCPPortMode) == L7_SUCCESS)
         SET_VALID(I_agentSwitchDenialOfServiceTCPPortMode, agentSwitchDenialOfServiceGroupData.valid);
    }
    break;

  case I_agentSwitchDenialOfServiceUDPPortMode :
    CLR_VALID(I_agentSwitchDenialOfServiceUDPPortMode, agentSwitchDenialOfServiceGroupData.valid);
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_ICMP_FEATURE_ID) == L7_TRUE)
    {
       if (snmpAgentSwitchDenialOfServiceUDPPortModeGet(USMDB_UNIT_CURRENT,
                                               &agentSwitchDenialOfServiceGroupData.agentSwitchDenialOfServiceUDPPortMode) == L7_SUCCESS)
         SET_VALID(I_agentSwitchDenialOfServiceUDPPortMode, agentSwitchDenialOfServiceGroupData.valid);
    }
    break;

  case I_agentSwitchDenialOfServiceTCPSynMode :
    CLR_VALID(I_agentSwitchDenialOfServiceTCPSynMode, agentSwitchDenialOfServiceGroupData.valid);
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_ICMP_FEATURE_ID) == L7_TRUE)
    {
       if (snmpAgentSwitchDenialOfServiceTCPSynModeGet(USMDB_UNIT_CURRENT,
                                               &agentSwitchDenialOfServiceGroupData.agentSwitchDenialOfServiceTCPSynMode) == L7_SUCCESS)
         SET_VALID(I_agentSwitchDenialOfServiceTCPSynMode, agentSwitchDenialOfServiceGroupData.valid);
    }
    break;

  case I_agentSwitchDenialOfServiceTCPSynFinMode :
    CLR_VALID(I_agentSwitchDenialOfServiceTCPSynFinMode, agentSwitchDenialOfServiceGroupData.valid);
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_ICMP_FEATURE_ID) == L7_TRUE)
    {
       if (snmpAgentSwitchDenialOfServiceTCPSynFinModeGet(USMDB_UNIT_CURRENT,
                                               &agentSwitchDenialOfServiceGroupData.agentSwitchDenialOfServiceTCPSynFinMode) == L7_SUCCESS)
         SET_VALID(I_agentSwitchDenialOfServiceTCPSynFinMode, agentSwitchDenialOfServiceGroupData.valid);
    }
    break;

  case I_agentSwitchDenialOfServiceTCPFinUrgPshMode :
    CLR_VALID(I_agentSwitchDenialOfServiceTCPFinUrgPshMode, agentSwitchDenialOfServiceGroupData.valid);
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_ICMP_FEATURE_ID) == L7_TRUE)
    {
       if (snmpAgentSwitchDenialOfServiceTCPFinUrgPshModeGet(USMDB_UNIT_CURRENT,
                                               &agentSwitchDenialOfServiceGroupData.agentSwitchDenialOfServiceTCPFinUrgPshMode) == L7_SUCCESS)
         SET_VALID(I_agentSwitchDenialOfServiceTCPFinUrgPshMode, agentSwitchDenialOfServiceGroupData.valid);
    }
    break;

  case I_agentSwitchDenialOfServiceICMPv6Size :
    CLR_VALID(I_agentSwitchDenialOfServiceICMPv6Size, agentSwitchDenialOfServiceGroupData.valid);
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_ICMP_FEATURE_ID) == L7_TRUE)
    {
       if (snmpAgentSwitchDenialOfServiceICMPv6SizeGet(USMDB_UNIT_CURRENT,
                                               &agentSwitchDenialOfServiceGroupData.agentSwitchDenialOfServiceICMPv6Size) == L7_SUCCESS)
         SET_VALID(I_agentSwitchDenialOfServiceICMPv6Size, agentSwitchDenialOfServiceGroupData.valid);
    }
    break;

  case I_agentSwitchDenialOfServiceICMPFragMode :
    CLR_VALID(I_agentSwitchDenialOfServiceICMPFragMode, agentSwitchDenialOfServiceGroupData.valid);
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_ICMP_FEATURE_ID) == L7_TRUE)
    {
       if (snmpAgentSwitchDenialOfServiceICMPFragModeGet(USMDB_UNIT_CURRENT,
                                               &agentSwitchDenialOfServiceGroupData.agentSwitchDenialOfServiceICMPFragMode) == L7_SUCCESS)
         SET_VALID(I_agentSwitchDenialOfServiceICMPFragMode, agentSwitchDenialOfServiceGroupData.valid);
    }
    break;

  default:
    /* unknown nominator */
    return(NULL);
    break;
  }

  if ( nominator >= 0 && !VALID(nominator, agentSwitchDenialOfServiceGroupData.valid) )
    return(NULL);

  return(&agentSwitchDenialOfServiceGroupData);
}

#ifdef SETS
int
k_agentSwitchDenialOfServiceGroup_test(ObjectInfo *object, ObjectSyntax *value,
                              doList_t *dp, ContextInfo *contextInfo)
{
  ZERO_VALID(((agentSwitchDenialOfServiceGroup_t *) (dp->data))->valid);
  return NO_ERROR;
}

int
k_agentSwitchDenialOfServiceGroup_ready(ObjectInfo *object, ObjectSyntax *value,
                               doList_t *doHead, doList_t *dp)
{

  dp->state = SR_ADD_MODIFY;
  return NO_ERROR;
}

int
k_agentSwitchDenialOfServiceGroup_set(agentSwitchDenialOfServiceGroup_t *data,
                             ContextInfo *contextInfo, int function)
{
  /*
   * Defining temporary variable for storing the valid bits for the case when the
   * set request is only paritally successful
  */
  L7_char8 tempValid[sizeof(data->valid)];
  bzero(tempValid, sizeof(tempValid));

  if ( VALID(I_agentSwitchDenialOfServiceSIPDIPMode, data->valid))
  {
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_SIPDIP_FEATURE_ID) == L7_TRUE)
    {
      if( snmpAgentSwitchDenialOfServiceSIPDIPModeSet(USMDB_UNIT_CURRENT,
                                              data->agentSwitchDenialOfServiceSIPDIPMode) != L7_SUCCESS )
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return(COMMIT_FAILED_ERROR);
      }
      else
      {
        SET_VALID(I_agentSwitchDenialOfServiceSIPDIPMode, tempValid);
      }
    }
  }

  if ( VALID(I_agentSwitchDenialOfServiceFirstFragMode, data->valid))
  {
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_FIRSTFRAG_FEATURE_ID) == L7_TRUE)
    {
      if( snmpAgentSwitchDenialOfServiceFirstFragModeSet(USMDB_UNIT_CURRENT,
                                              data->agentSwitchDenialOfServiceFirstFragMode) != L7_SUCCESS )
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return(COMMIT_FAILED_ERROR);
      }
      else
      {
        SET_VALID(I_agentSwitchDenialOfServiceFirstFragMode, tempValid);
      }
    }
  }

  if ( VALID(I_agentSwitchDenialOfServiceTCPHdrSize, data->valid))
  {
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_FIRSTFRAG_FEATURE_ID) == L7_TRUE)
    {
      if( snmpAgentSwitchDenialOfServiceTCPHdrSizeSet(USMDB_UNIT_CURRENT,
                                              data->agentSwitchDenialOfServiceTCPHdrSize) != L7_SUCCESS )
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return(COMMIT_FAILED_ERROR);
      }
      else
      {
        SET_VALID(I_agentSwitchDenialOfServiceTCPHdrSize, tempValid);
      }
    }
  }

  if ( VALID(I_agentSwitchDenialOfServiceTCPFragMode, data->valid))
  {
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_TCPFRAG_FEATURE_ID) == L7_TRUE)
    {
      if( snmpAgentSwitchDenialOfServiceTCPFragModeSet(USMDB_UNIT_CURRENT,
                                              data->agentSwitchDenialOfServiceTCPFragMode) != L7_SUCCESS )
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return(COMMIT_FAILED_ERROR);
      }
      else
      {
        SET_VALID(I_agentSwitchDenialOfServiceTCPFragMode, tempValid);
      }
    }
  }

  if ( VALID(I_agentSwitchDenialOfServiceTCPFlagMode, data->valid))
  {
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_TCPFLAG_FEATURE_ID) == L7_TRUE)
    {
      if( snmpAgentSwitchDenialOfServiceTCPFlagModeSet(USMDB_UNIT_CURRENT,
                                              data->agentSwitchDenialOfServiceTCPFlagMode) != L7_SUCCESS )
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return(COMMIT_FAILED_ERROR);
      }
      else
      {
        SET_VALID(I_agentSwitchDenialOfServiceTCPFlagMode, tempValid);
      }
    }
  }

  if ( VALID(I_agentSwitchDenialOfServiceL4PortMode, data->valid))
  {
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_L4PORT_FEATURE_ID) == L7_TRUE)
    {
      if( snmpAgentSwitchDenialOfServiceL4PortModeSet(USMDB_UNIT_CURRENT,
                                              data->agentSwitchDenialOfServiceL4PortMode) != L7_SUCCESS )
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return(COMMIT_FAILED_ERROR);
      }
      else
      {
        SET_VALID(I_agentSwitchDenialOfServiceL4PortMode, tempValid);
      }
    }
  }

  if ( VALID(I_agentSwitchDenialOfServiceICMPMode, data->valid))
  {
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_ICMP_FEATURE_ID) == L7_TRUE)
    {
      if( snmpAgentSwitchDenialOfServiceICMPModeSet(USMDB_UNIT_CURRENT,
                                              data->agentSwitchDenialOfServiceICMPMode) != L7_SUCCESS )
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return(COMMIT_FAILED_ERROR);
      }
      else
      {
        SET_VALID(I_agentSwitchDenialOfServiceICMPMode, tempValid);
      }
    }
  }

  if ( VALID(I_agentSwitchDenialOfServiceICMPSize, data->valid))
  {
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_ICMP_FEATURE_ID) == L7_TRUE)
    {
      if( snmpAgentSwitchDenialOfServiceICMPSizeSet(USMDB_UNIT_CURRENT,
                                              data->agentSwitchDenialOfServiceICMPSize) != L7_SUCCESS )
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return(COMMIT_FAILED_ERROR);
      }
      else
      {
        SET_VALID(I_agentSwitchDenialOfServiceICMPSize, tempValid);
      }
    }
  }

  if ( VALID(I_agentSwitchDenialOfServiceSMACDMACMode, data->valid))
  {
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_ICMP_FEATURE_ID) == L7_TRUE)
    {
      if( snmpAgentSwitchDenialOfServiceSMACDMACModeSet(USMDB_UNIT_CURRENT,
                                              data->agentSwitchDenialOfServiceSMACDMACMode) != L7_SUCCESS )
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return(COMMIT_FAILED_ERROR);
      }
      else
      {
        SET_VALID(I_agentSwitchDenialOfServiceSMACDMACMode, tempValid);
      }
    }
  }

  if ( VALID(I_agentSwitchDenialOfServiceTCPOffsetMode, data->valid))
  {
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_ICMP_FEATURE_ID) == L7_TRUE)
    {
      if( snmpAgentSwitchDenialOfServiceTCPOffsetModeSet(USMDB_UNIT_CURRENT,
                                              data->agentSwitchDenialOfServiceTCPOffsetMode) != L7_SUCCESS )
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return(COMMIT_FAILED_ERROR);
      }
      else
      {
        SET_VALID(I_agentSwitchDenialOfServiceTCPOffsetMode, tempValid);
      }
    }
  }

  if ( VALID(I_agentSwitchDenialOfServiceTCPFlagSeqMode, data->valid))
  {
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_ICMP_FEATURE_ID) == L7_TRUE)
    {
      if( snmpAgentSwitchDenialOfServiceTCPFlagSeqModeSet(USMDB_UNIT_CURRENT,
                                              data->agentSwitchDenialOfServiceTCPFlagSeqMode) != L7_SUCCESS )
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return(COMMIT_FAILED_ERROR);
      }
      else
      {
        SET_VALID(I_agentSwitchDenialOfServiceTCPFlagSeqMode, tempValid);
      }
    }
  }

  if ( VALID(I_agentSwitchDenialOfServiceTCPPortMode, data->valid))
  {
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_ICMP_FEATURE_ID) == L7_TRUE)
    {
      if( snmpAgentSwitchDenialOfServiceTCPPortModeSet(USMDB_UNIT_CURRENT,
                                              data->agentSwitchDenialOfServiceTCPPortMode) != L7_SUCCESS )
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return(COMMIT_FAILED_ERROR);
      }
      else
      {
        SET_VALID(I_agentSwitchDenialOfServiceTCPPortMode, tempValid);
      }
    }
  }

  if ( VALID(I_agentSwitchDenialOfServiceUDPPortMode, data->valid))
  {
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_ICMP_FEATURE_ID) == L7_TRUE)
    {
      if( snmpAgentSwitchDenialOfServiceUDPPortModeSet(USMDB_UNIT_CURRENT,
                                              data->agentSwitchDenialOfServiceUDPPortMode) != L7_SUCCESS )
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return(COMMIT_FAILED_ERROR);
      }
      else
      {
        SET_VALID(I_agentSwitchDenialOfServiceUDPPortMode, tempValid);
      }
    }
  }

  if ( VALID(I_agentSwitchDenialOfServiceTCPSynMode, data->valid))
  {
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_ICMP_FEATURE_ID) == L7_TRUE)
    {
      if( snmpAgentSwitchDenialOfServiceTCPSynModeSet(USMDB_UNIT_CURRENT,
                                              data->agentSwitchDenialOfServiceTCPSynMode) != L7_SUCCESS )
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return(COMMIT_FAILED_ERROR);
      }
      else
      {
        SET_VALID(I_agentSwitchDenialOfServiceTCPSynMode, tempValid);
      }
    }
  }

  if ( VALID(I_agentSwitchDenialOfServiceTCPSynFinMode, data->valid))
  {
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_ICMP_FEATURE_ID) == L7_TRUE)
    {
      if( snmpAgentSwitchDenialOfServiceTCPSynFinModeSet(USMDB_UNIT_CURRENT,
                                              data->agentSwitchDenialOfServiceTCPSynFinMode) != L7_SUCCESS )
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return(COMMIT_FAILED_ERROR);
      }
      else
      {
        SET_VALID(I_agentSwitchDenialOfServiceTCPSynFinMode, tempValid);
      }
    }
  }

  if ( VALID(I_agentSwitchDenialOfServiceTCPFinUrgPshMode, data->valid))
  {
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_ICMP_FEATURE_ID) == L7_TRUE)
    {
      if( snmpAgentSwitchDenialOfServiceTCPFinUrgPshModeSet(USMDB_UNIT_CURRENT,
                                              data->agentSwitchDenialOfServiceTCPFinUrgPshMode) != L7_SUCCESS )
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return(COMMIT_FAILED_ERROR);
      }
      else
      {
        SET_VALID(I_agentSwitchDenialOfServiceTCPFinUrgPshMode, tempValid);
      }
    }
  }

  if ( VALID(I_agentSwitchDenialOfServiceICMPv6Size, data->valid))
  {
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_ICMP_FEATURE_ID) == L7_TRUE)
    {
      if( snmpAgentSwitchDenialOfServiceICMPv6SizeSet(USMDB_UNIT_CURRENT,
                                              data->agentSwitchDenialOfServiceICMPv6Size) != L7_SUCCESS )
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return(COMMIT_FAILED_ERROR);
      }
      else
      {
        SET_VALID(I_agentSwitchDenialOfServiceICMPv6Size, tempValid);
      }
    }
  }

  if ( VALID(I_agentSwitchDenialOfServiceICMPFragMode, data->valid))
  {
    if (usmDbFeaturePresentCheck(USMDB_UNIT_CURRENT, L7_DOSCONTROL_COMPONENT_ID, L7_DOSCONTROL_ICMP_FEATURE_ID) == L7_TRUE)
    {
      if( snmpAgentSwitchDenialOfServiceICMPFragModeSet(USMDB_UNIT_CURRENT,
                                              data->agentSwitchDenialOfServiceICMPFragMode) != L7_SUCCESS )
      {
        memcpy(data->valid, tempValid, sizeof(data->valid));
        return(COMMIT_FAILED_ERROR);
      }
      else
      {
        SET_VALID(I_agentSwitchDenialOfServiceICMPFragMode, tempValid);
      }
    }
  }

  return NO_ERROR;
}

  #ifdef SR_agentSwitchDenialOfServiceGroup_UNDO
/* add #define SR_agentSwitchDenialOfServiceGroup_UNDO in sitedefs.h to
 * include the undo routine for the agentSwitchDenialOfServiceGroup family.
 */
int
agentSwitchDenialOfServiceGroup_undo(doList_t *doHead, doList_t *doCur,
                            ContextInfo *contextInfo)
{
  agentSwitchDenialOfServiceGroup_t *data = (agentSwitchDenialOfServiceGroup_t *) doCur->data;
  agentSwitchDenialOfServiceGroup_t *undodata = (agentSwitchDenialOfServiceGroup_t *) doCur->undodata;
  agentSwitchDenialOfServiceGroup_t *setdata = NULL;
  L7_int32 function = SR_UNKNOWN;

  /*
   * Modifications for UNDO Feature
   * Setting valid bits of undodata same as that for data
  */
  if ( data == NULL || undodata == NULL )
    return UNDO_FAILED_ERROR;
  memcpy(undodata->valid,data->valid,sizeof(data->valid));

  /* undoing a modify, replace the original data */
  setdata = undodata;
  function = SR_ADD_MODIFY;

  /* use the set method for the undo */
  if ((setdata != NULL) && (k_agentSwitchDenialOfServiceGroup_set(setdata, contextInfo, function) == NO_ERROR))
    return NO_ERROR;

  return UNDO_FAILED_ERROR;
}
  #endif /* SR_agentSwitchDenialOfServiceGroup_UNDO */

#endif /* SETS */
