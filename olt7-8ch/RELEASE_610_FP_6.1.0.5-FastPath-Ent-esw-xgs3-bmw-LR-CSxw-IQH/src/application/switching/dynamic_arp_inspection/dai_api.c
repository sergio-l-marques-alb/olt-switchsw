
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
* @filename  dai_api.c
*
* @purpose   Dynamic ARP Inspection APIs
*
* @component Dynamic ARP Inspection
*
* @comments 
*
* @create 09/01/2007
*
* @author Kiran Kumar Kella
*
* @end
*             
**********************************************************************/

#include "l7_common.h"
#include "osapi.h"
#include "log.h"

#include "dai_cfg.h"
#include "dai_util.h"

#define MAC_STR_LEN 17

extern daiCfgData_t   *daiCfgData;
extern daiVlanInfo_t  *daiVlanInfo;
extern osapiRWLock_t  daiCfgRWLock;

/*********************************************************************
* @purpose  Get API for whether DAI verifies source MAC addresses.
*
* @param    verifySMac   @b((output)) Verify Source MAC option
*
* @returns  L7_SUCCESS, if mode was set
*
* @notes    When SMAC verify is enabled, check the source MAC address
*           in the Ethernet header against the sender MAC address in the
*           ARP body of both ARP requests and responses. Packets with
*           different MAC addresses are classified as invalid and are
*           dropped.
*
* @end
*********************************************************************/
L7_RC_t daiVerifySMacGet(L7_BOOL *verifySMac)
{
  if (osapiReadLockTake(daiCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS) 
    return L7_FAILURE;

  *verifySMac = daiCfgData->daiVerifySMac;

  osapiReadLockGive(daiCfgRWLock);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set API for whether DAI verifies source MAC addresses.
*
* @param    verifySMac   @b((input)) L7_TRUE or L7_FALSE
*
* @returns  L7_SUCCESS, if mode was set
* @returns  L7_FAILURE, if invalid mode was specified
*
* @notes    see daiVerifySMacGet().
*
* @end
*********************************************************************/
L7_RC_t daiVerifySMacSet(L7_BOOL verifySMac)
{
  if (osapiWriteLockTake(daiCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  if (verifySMac != daiCfgData->daiVerifySMac)
  {
    daiCfgData->daiVerifySMac = verifySMac;
    /* There is no apply function */
    daiConfigDataChange();
  }
  osapiWriteLockGive(daiCfgRWLock);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get API for whether DAI verifies destination MAC addresses.
*
* @param    verifyDMac   @b((output)) Verify destination MAC option
*
* @returns  L7_SUCCESS, if mode was set
*
* @notes    When DMAC verify is enabled, check the destination MAC address
*           in the Ethernet header against the target MAC address in the
*           ARP body of ARP responses. Packets with different MAC addresses
*           are classified as invalid and are dropped.
*
* @end
*********************************************************************/
L7_RC_t daiVerifyDMacGet(L7_BOOL *verifyDMac)
{
  if (osapiReadLockTake(daiCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS) 
    return L7_FAILURE;

  *verifyDMac = daiCfgData->daiVerifyDMac;

  osapiReadLockGive(daiCfgRWLock);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set API for whether DAI verifies destination MAC addresses.
*
* @param    verifyDMac   @b((input)) L7_TRUE or L7_FALSE
*
* @returns  L7_SUCCESS, if mode was set
* @returns  L7_FAILURE, if invalid mode was specified
*
* @notes    see daiVerifyDMacGet().
*
* @end
*********************************************************************/
L7_RC_t daiVerifyDMacSet(L7_BOOL verifyDMac)
{
  if (osapiWriteLockTake(daiCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  if (verifyDMac != daiCfgData->daiVerifyDMac)
  {
    daiCfgData->daiVerifyDMac = verifyDMac;
    /* There is no apply function */
    daiConfigDataChange();
  }
  osapiWriteLockGive(daiCfgRWLock);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get API for whether DAI verifies IP address.
*
* @param    verifyIP   @b((output)) Verify IP address
*
* @returns  L7_SUCCESS, if mode was set
*
* @notes    When IP verify is enabled, check the ARP body for invalid and
*           unexpected IP addresses. Addresses include 0.0.0.0, 255.255.255.255,
*           and all IP multicast addresses. Sender IP addresses are checked
*           in all ARP requests and responses, and target IP addresses are
*           checked only in ARP responses.
*
* @end
*********************************************************************/
L7_RC_t daiVerifyIPGet(L7_BOOL *verifyIP)
{
  if (osapiReadLockTake(daiCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS) 
    return L7_FAILURE;

  *verifyIP = daiCfgData->daiVerifyIP;

  osapiReadLockGive(daiCfgRWLock);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set API for whether DAI verifies IP address.
*
* @param    verifyIP   @b((input)) L7_TRUE or L7_FALSE
*
* @returns  L7_SUCCESS, if mode was set
* @returns  L7_FAILURE, if invalid mode was specified
*
* @notes    see daiVerifyIPGet().
*
* @end
*********************************************************************/
L7_RC_t daiVerifyIPSet(L7_BOOL verifyIP)
{
  if (osapiWriteLockTake(daiCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  if (verifyIP != daiCfgData->daiVerifyIP)
  {
    daiCfgData->daiVerifyIP = verifyIP;
    /* There is no apply function */
    daiConfigDataChange();
  }
  osapiWriteLockGive(daiCfgRWLock);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get API for whether Dynamic ARP Inspection is enabled
*           or disabled on a VLAN.
*
* @param    vlanId @b((input)) VLAN Id
* @param    val    @b((output)) L7_ENABLE if DAI is enabled on the VLAN
*
* @returns  L7_SUCCESS, if value successfully retrieved
*           L7_ERROR, if inputs invalid
*           L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t daiVlanEnableGet(L7_uint32 vlanId, L7_uint32 *val)
{
  if ((vlanId < 1) ||
      (vlanId > DAI_MAX_VLAN_COUNT))
    return L7_ERROR;

  if (osapiReadLockTake(daiCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS) 
    return L7_FAILURE;

  if (_daiVlanEnableGet(vlanId))
    *val = L7_ENABLE;
  else
    *val = L7_DISABLE;

  osapiReadLockGive(daiCfgRWLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set API for whether Dynamic ARP Inspection is enabled
*           or disabled on a VLAN.
*
* @param    vlanId @b((input)) VLAN Id
* @param    val    @b((input)) L7_ENABLE if DAI is enabled on the VLAN
*
* @returns  L7_SUCCESS, if value successfully retrieved
*           L7_ERROR, if inputs invalid
*           L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t daiVlanEnableSet(L7_uint32 vlanId, L7_uint32 val)
{
  if ((vlanId < 1) ||
      (vlanId > DAI_MAX_VLAN_COUNT))
    return L7_ERROR;

  if (osapiWriteLockTake(daiCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS) 
    return L7_FAILURE;

  if ((val == L7_ENABLE) && !_daiVlanEnableGet(vlanId))
  {
    _daiVlanEnable(vlanId);       /* set config */
    daiVlanEnableApply(vlanId, val);   /* react to change */
    daiConfigDataChange();
  }
  else if ((val == L7_DISABLE) && (_daiVlanEnableGet(vlanId)))
  {
    _daiVlanDisable(vlanId);
    daiVlanEnableApply(vlanId, val);
    daiConfigDataChange();
  }

  osapiWriteLockGive(daiCfgRWLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the next vlan id eligible for DAI configuration, given
*           the current vlan id
*
* @param    vlanId   @b((input)) VLAN Id
* @param    nextVlan @b((output)) next vlan id
*
* @returns  L7_SUCCESS, if value successfully retrieved
*           L7_ERROR, if inputs invalid
*           L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t daiVlanNextGet(L7_uint32 vlanId, L7_uint32 *nextVlan)
{
  if ((vlanId < 0) ||
      (vlanId > DAI_MAX_VLAN_COUNT))
    return L7_ERROR;

  if(vlanId+1 <= DAI_MAX_VLAN_COUNT)
  {
    *nextVlan = vlanId+1;
    return L7_SUCCESS;
  }

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Enable or disable Dynamic ARP Inspection on a
*           VLAN/VLAN range
*
* @param    vlanStart @b((input)) Low end of VLAN range
* @param    vlanEnd   @b((input)) High end of VLAN range
* @param    val       @b((input)) L7_ENABLE if DAI is enabled 
*                                 on all VLANs in the range
*
* @returns  L7_SUCCESS, if mode was set
*           L7_ERROR, if inputs invalid
*           L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t daiVlanEnableRangeSet(L7_uint32 vlanStart, L7_uint32 vlanEnd, L7_uint32 val)
{
  L7_uint32 i;
  if ((vlanStart > vlanEnd) || 
      ((val != L7_ENABLE) && (val != L7_DISABLE)))
  {
    return L7_ERROR;
  }

  if ((vlanStart == 0) || (vlanEnd > DAI_MAX_VLAN_COUNT))
    return L7_ERROR;

  if (osapiWriteLockTake(daiCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  for (i = vlanStart; i <= vlanEnd; i++)
  {
    if ((val == L7_ENABLE) && !_daiVlanEnableGet(i))
    {
      _daiVlanEnable(i);       /* set config */
      daiVlanEnableApply(i, val);   /* react to change */
      daiConfigDataChange();
    }
    else if ((val == L7_DISABLE) && (_daiVlanEnableGet(i)))
    {
      _daiVlanDisable(i);
      daiVlanEnableApply(i, val);
      daiConfigDataChange();
    }
  }

  osapiWriteLockGive(daiCfgRWLock);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Given a VLAN ID, return the endpoints of the next range of 
*           VLANs enabled for Dynamic ARP Inspection.
*
* @param    vlanStart @b((input/output)) Low end of VLAN range
* @param    vlanEnd   @b((input/output)) High end of VLAN range
*
* @returns  L7_SUCCESS, if mode was set
*           L7_ERROR, if inputs invalid
*           L7_FAILURE
*
* @notes    This API intended to be used by show run, so that show run
*           doesn't have to list each enabled VLAN individually. Search
*           for next range starts at vlanEnd + 1. 
*
*           If no VLANs are enabled for DAI beyond vlanIn, 
*           returns L7_SUCCESS with vlanStart = 0.
*
* @end
*********************************************************************/
L7_RC_t daiNextEnabledVlanRangeGet(L7_uint32 *vlanStart,
                                   L7_uint32 *vlanEnd)
{
  L7_uint32 i;

  if (*vlanEnd > DAI_MAX_VLAN_COUNT)
    return L7_ERROR;

  if (osapiReadLockTake(daiCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS) 
    return L7_FAILURE;

  for (i = *vlanEnd + 1; i <= DAI_MAX_VLAN_COUNT; i++)
  {
    if (_daiVlanEnableGet(i))
      break;
  }

  if (i > DAI_MAX_VLAN_COUNT)
  {
    /* No more VLANs enabled */
    *vlanStart = 0;
    osapiReadLockGive(daiCfgRWLock);
    return L7_SUCCESS;
  }

  *vlanStart = i;
  for (i = i + 1; i <= DAI_MAX_VLAN_COUNT; i++)
  {
    if (!_daiVlanEnableGet(i))
    {
      *vlanEnd = i - 1; 
      osapiReadLockGive(daiCfgRWLock);
      return L7_SUCCESS;
    }
  }
  *vlanEnd = DAI_MAX_VLAN_COUNT;
  osapiReadLockGive(daiCfgRWLock);
  return L7_SUCCESS;
} 

/*********************************************************************
* @purpose  Get API for whether DAI considers a port trusted.
*
* @param    intIfNum @b((input))  internal interface number
* @param    trust    @b((output)) L7_TRUE if port is trusted
*
* @returns  L7_SUCCESS
*           L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t daiIntfTrustGet(L7_uint32 intIfNum, L7_BOOL *trust)
{
  if (osapiReadLockTake(daiCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS) 
    return L7_FAILURE;

  *trust = _daiIntfTrustGet(intIfNum);

  osapiReadLockGive(daiCfgRWLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set API for whether DAI considers a port trusted.
*
* @param    intIfNum @b((input))  internal interface number
* @param    trust    @b((input))  L7_TRUE if port is trusted
*
* @returns  L7_SUCCESS, if mode was set
*           L7_NOT_SUPPORTED, if DHCP snooping is not supported on this 
*                             type of interface
*           L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t daiIntfTrustSet(L7_uint32 intIfNum, L7_BOOL trust)
{
  if (!daiIntfIsValid(intIfNum))
  {
    return L7_NOT_SUPPORTED;
  }

  if (osapiWriteLockTake(daiCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  if (_daiIntfTrustGet(intIfNum) != trust)
  {
    if (trust)
      _daiIntfTrust(intIfNum);
    else
      _daiIntfUntrust(intIfNum);

    daiIntfTrustApply(intIfNum, trust);

    daiConfigDataChange();
  }

  osapiWriteLockGive(daiCfgRWLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Determines whether DAI is enabled on a port.
*
* @param    intIfNum @b((input))  internal interface number
* @param    enabled  @b((output)) L7_TRUE if DAI is enabled on port
*
* @returns  L7_SUCCESS, if value successfully retrieved
*           L7_FAILURE
*
* @notes    DAI is considered enabled on a port if DAI
*           is enabled on one or more VLANs the port participates in.
*
* @end
*********************************************************************/
L7_RC_t daiPortEnabledGet(L7_uint32 intIfNum, L7_BOOL *enabled)
{
  if (osapiReadLockTake(daiCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  *enabled = daiIntfIsInspected(intIfNum);

  osapiReadLockGive(daiCfgRWLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get API for whether DAI logging is enabled
*           or disabled on a VLAN.
*
* @param    vlanId @b((input)) VLAN Id
* @param    val    @b((output)) L7_ENABLE if DAI logging is enabled on the VLAN
*
* @returns  L7_SUCCESS, if value successfully retrieved
*           L7_ERROR, if inputs invalid
*           L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t daiVlanLoggingEnableGet(L7_uint32 vlanId, L7_uint32 *val)
{
  if ((vlanId < 1) ||
      (vlanId > DAI_MAX_VLAN_COUNT))
    return L7_ERROR;

  if (osapiReadLockTake(daiCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS) 
    return L7_FAILURE;

  if (_daiVlanLogInvalidGet(vlanId))
    *val = L7_ENABLE;
  else
    *val = L7_DISABLE;

  osapiReadLockGive(daiCfgRWLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set API for whether DAI logging is enabled
*           or disabled on a VLAN.
*
* @param    vlanId @b((input)) VLAN Id
* @param    val    @b((input)) L7_ENABLE if DAI logging is enabled on the VLAN
*
* @returns  L7_SUCCESS
*           L7_ERROR, if inputs invalid
*           L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t daiVlanLoggingEnableSet(L7_uint32 vlanId, L7_uint32 val)
{
  if ((vlanId < 1) ||
      (vlanId > DAI_MAX_VLAN_COUNT))
    return L7_ERROR;

  if (osapiWriteLockTake(daiCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS) 
    return L7_FAILURE;

  if ((val == L7_ENABLE) && !_daiVlanLogInvalidGet(vlanId))
  {
    _daiVlanLogInvalid(vlanId);       /* set config */
    daiConfigDataChange();
  }
  else if ((val == L7_DISABLE) && (_daiVlanLogInvalidGet(vlanId)))
  {
    _daiVlanDontLogInvalid(vlanId);
    daiConfigDataChange();
  }

  osapiWriteLockGive(daiCfgRWLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Enable or disable DAI logging on a VLAN/VLAN range
*
* @param    vlanStart @b((input)) Low end of VLAN range
* @param    vlanEnd   @b((input)) High end of VLAN range
* @param    val       @b((input)) L7_ENABLE if DAI logging is enabled 
*                                 on all VLANs in the range
*
* @returns  L7_SUCCESS, if mode was set
*           L7_ERROR, if inputs invalid
*           L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t daiVlanLoggingEnableRangeSet(L7_uint32 vlanStart, L7_uint32 vlanEnd,
                                     L7_uint32 val)
{
  L7_uint32 i;
  if ((vlanStart > vlanEnd) || 
      ((val != L7_ENABLE) && (val != L7_DISABLE)))
  { 
    return L7_ERROR;
  }

  if ((vlanStart == 0) || (vlanEnd > DAI_MAX_VLAN_COUNT))
    return L7_ERROR;

  if (osapiWriteLockTake(daiCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  for (i = vlanStart; i <= vlanEnd; i++)
  {
    if ((val == L7_ENABLE) && !_daiVlanLogInvalidGet(i))
    {
      _daiVlanLogInvalid(i);       /* set config */
      daiConfigDataChange();
    }
    else if ((val == L7_DISABLE) && (_daiVlanLogInvalidGet(i)))
    {
      _daiVlanDontLogInvalid(i);
      daiConfigDataChange();
    }
  }

  osapiWriteLockGive(daiCfgRWLock);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Given a VLAN ID, return the endpoints of the next range of 
*           VLANs enabled for DAI logging.
*
* @param    vlanStart @b((input/output)) Low end of VLAN range
* @param    vlanEnd   @b((input/output)) High end of VLAN range
*
* @returns  L7_SUCCESS, if mode was set
*           L7_ERROR, if inputs invalid
*           L7_FAILURE
*
* @notes    This API intended to be used by show run, so that show run
*           doesn't have to list each enabled VLAN individually. Search
*           for next range starts at vlanEnd + 1. 
*
*           If no VLANs are enabled for DAI beyond vlanIn, 
*           returns L7_SUCCESS with vlanStart = 0.
*
* @end
*********************************************************************/
L7_RC_t daiVlanNextLoggingEnableRangeGet(L7_uint32 *vlanStart,
                                         L7_uint32 *vlanEnd)
{
  L7_uint32 i;

  if (*vlanEnd > DAI_MAX_VLAN_COUNT)
    return L7_ERROR;

  if (osapiReadLockTake(daiCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS) 
    return L7_FAILURE;

  for (i = *vlanEnd + 1; i <= DAI_MAX_VLAN_COUNT; i++)
  {
    if (_daiVlanLogInvalidGet(i))
      break;
  }

  if (i > DAI_MAX_VLAN_COUNT)
  {
    /* No more VLANs enabled */
    *vlanStart = 0;
    osapiReadLockGive(daiCfgRWLock);
    return L7_SUCCESS;
  }

  *vlanStart = i;
  for (i = i + 1; i <= DAI_MAX_VLAN_COUNT; i++)
  {
    if (!_daiVlanLogInvalidGet(i))
    {
      *vlanEnd = i - 1; 
      osapiReadLockGive(daiCfgRWLock);
      return L7_SUCCESS;
    }
  }
  *vlanEnd = DAI_MAX_VLAN_COUNT;
  osapiReadLockGive(daiCfgRWLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the ARP ACL Name for a vlan
*
* @param    vlanId    @b((input)) vlan Id
* @param    aclName   @b((output)) ARP ACL name for this VLAN
*
* @returns  L7_SUCCESS
*           L7_ERROR, if inputs invalid
*           L7_FAILURE
*
* @notes    This API is intended to be used by WEB/SNMP
*
*           aclName should have memory allocated by the caller already.
*
* @end
*********************************************************************/
L7_RC_t daiVlanArpAclGet(L7_uint32 vlanId, L7_uchar8 *aclName)
{
  if ((vlanId < 1) ||
      (vlanId > DAI_MAX_VLAN_COUNT))
    return L7_ERROR;

  if (osapiReadLockTake(daiCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS) 
    return L7_FAILURE;

  _daiVlanArpAclGet(vlanId, aclName);

  osapiReadLockGive(daiCfgRWLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the ARP ACL Name for a vlan
*
* @param    vlanId    @b((input)) vlan Id
* @param    aclName   @b((input)) ARP ACL name for this VLAN
*
* @returns  L7_SUCCESS
*           L7_ERROR, if inputs invalid
*           L7_FAILURE
*
* @notes    This API is intended to be used by WEB/SNMP
*
*           aclName should have memory allocated by the caller already.
*           We allow ARP ACL name as a string that can take
*           alphanumeric characters.
*
* @end
*********************************************************************/
L7_RC_t daiVlanArpAclSet(L7_uint32 vlanId, L7_uchar8 *aclName)
{
  L7_uchar8 aclTemp[L7_ARP_ACL_NAME_LEN_MAX+1];

  if ((vlanId < 1) || (vlanId > DAI_MAX_VLAN_COUNT) ||
      (!aclName || (strlen(aclName) < L7_ARP_ACL_NAME_LEN_MIN) ||
                   (strlen(aclName) > L7_ARP_ACL_NAME_LEN_MAX) ||
                   (daiAclNameStringCheck(aclName) != L7_SUCCESS)))
  {
    return L7_ERROR;
  }

  if (osapiWriteLockTake(daiCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  _daiVlanArpAclGet(vlanId, aclTemp);

  if (osapiStrncmp(aclTemp, aclName, L7_ARP_ACL_NAME_LEN_MAX+1) != 0)
  {
    _daiVlanArpAclSet(vlanId, aclName);
    daiConfigDataChange();
  }

  osapiWriteLockGive(daiCfgRWLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the ARP ACL static flag for a vlan
*
* @param    vlanId      @b((input))  vlan Id
* @param    staticFlag @b((output)) static flag for the VLAN
*
* @returns  L7_SUCCESS
*           L7_ERROR, if inputs invalid
*           L7_FAILURE
*
* @notes    This API is intended to be used by WEB/SNMP
*
* @end
*********************************************************************/
L7_RC_t daiVlanArpAclStaticFlagGet(L7_uint32 vlanId, L7_uint32 *staticFlag)
{
  if ((vlanId < 1) ||
      (vlanId > DAI_MAX_VLAN_COUNT))
    return L7_ERROR;

  if (osapiReadLockTake(daiCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS) 
    return L7_FAILURE;

  if(_daiVlanStaticFlagGet(vlanId))
    *staticFlag = L7_ENABLE;
  else
    *staticFlag = L7_DISABLE;

  osapiReadLockGive(daiCfgRWLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the ARP ACL static flag for the vlan
*
* @param    vlanId     @b((input)) vlan Id
* @param    staticFlag @b((input)) static flag for the VLAN
*
* @returns  L7_SUCCESS
*           L7_ERROR, if inputs invalid
*           L7_FAILURE
*
* @notes    This API is intended to be used by WEB/SNMP
*
* @end
*********************************************************************/
L7_RC_t daiVlanArpAclStaticFlagSet(L7_uint32 vlanId, L7_uint32 staticFlag)
{
  if ((vlanId < 1) ||
      (vlanId > DAI_MAX_VLAN_COUNT))
    return L7_ERROR;

  if (osapiWriteLockTake(daiCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS) 
    return L7_FAILURE;

  if ((staticFlag == L7_ENABLE) && !_daiVlanStaticFlagGet(vlanId))
  {
    _daiVlanStaticFlagSet(vlanId);       /* set config */
    daiConfigDataChange();
  }
  else if ((staticFlag == L7_DISABLE) && (_daiVlanStaticFlagGet(vlanId)))
  {
    _daiVlanStaticFlagClear(vlanId);
    daiConfigDataChange();
  }

  osapiWriteLockGive(daiCfgRWLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Given a VLAN ID, return the endpoints of the next range of 
*           VLANs enabled for ARP ACLs.
*
* @param    vlanStart @b((input/output)) Low end of VLAN range
* @param    vlanEnd   @b((input/output)) High end of VLAN range
* @param    aclName   @b((output)) ARP ACL name for this VLAN range
* @param    staticFlag @b((output)) static Flag for this ACL filter
*
* @returns  L7_SUCCESS, if mode was set
*           L7_ERROR, if inputs invalid
*           L7_FAILURE
*
* @notes    This API intended to be used by show run, so that show run
*           doesn't have to list each enabled VLAN individually. Search
*           for next range starts at vlanEnd + 1. 
*
*           If no VLANs are enabled for DAI beyond vlanIn, 
*           returns L7_SUCCESS with vlanStart = 0.
*
*           aclName should have memory allocated by the caller already.
*
* @end
*********************************************************************/
L7_RC_t daiVlanNextArpAclRangeGet(L7_uint32 *vlanStart,
                                  L7_uint32 *vlanEnd,
                                  L7_uchar8 *aclName,
                                  L7_uint32 *staticFlag)
{
  L7_uint32 i, staticTemp = L7_DISABLE;
  L7_uchar8 aclTemp[L7_ARP_ACL_NAME_LEN_MAX+1];

  if ((*vlanEnd > DAI_MAX_VLAN_COUNT) || (!aclName))
    return L7_ERROR;

  if (osapiReadLockTake(daiCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS) 
    return L7_FAILURE;

  *staticFlag = L7_DISABLE;
  for (i = *vlanEnd + 1; i <= DAI_MAX_VLAN_COUNT; i++)
  {
    if (_daiVlanArpAclGet(i, aclName))
    {
      if(_daiVlanStaticFlagGet(i))
      {
        *staticFlag = L7_ENABLE;
      }
      break;
    }
  }

  if (i > DAI_MAX_VLAN_COUNT)
  {
    /* No more VLANs enabled */
    *vlanStart = 0;
    osapiReadLockGive(daiCfgRWLock);
    return L7_SUCCESS;
  }

  *vlanStart = i;
  for (i = i + 1; i <= DAI_MAX_VLAN_COUNT; i++)
  {
    if(_daiVlanStaticFlagGet(i))
      staticTemp = L7_ENABLE;
    else
      staticTemp = L7_DISABLE;
    
    if ((!_daiVlanArpAclGet(i, aclTemp)) ||
        ((_daiVlanArpAclGet(i, aclTemp)) && 
         ((osapiStrncmp(aclName, aclTemp, L7_ARP_ACL_NAME_LEN_MAX+1) != 0) ||
          (staticTemp != *staticFlag))))
    {
      *vlanEnd = i - 1; 
      osapiReadLockGive(daiCfgRWLock);
      return L7_SUCCESS;
    }
  }
  *vlanEnd = DAI_MAX_VLAN_COUNT;
  osapiReadLockGive(daiCfgRWLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Apply the ARP ACL to a range of VLANs
*
* @param    aclName   @b((input))  ACL name
* @param    vlanStart @b((input)) Low end of VLAN range
* @param    vlanEnd   @b((input)) High end of VLAN range
* @param    val       @b((input)) L7_ENABLE if ARP ACL is enabled
*                                 on all VLANs in the range
* @param    staticFlag @b((input)) If static is enabled, packets that do
*                      not match any permit statements of ARP ACL are
*                      dropped without consulting the DHCP snooping bindings.
*
* @returns  L7_SUCCESS if entry added.
*
* @notes    We allow ARP ACL name as a string that can take
*           alphanumeric characters.
*
* @end
*********************************************************************/
L7_RC_t daiVlanARPAclRangeSet(L7_uchar8 *aclName, L7_uint32 vlanStart,
                              L7_uint32 vlanEnd, L7_uint32 val, L7_uint32 staticFlag)
{
  L7_uint32 i;
  L7_uchar8 aclTemp[L7_ARP_ACL_NAME_LEN_MAX+1];

  /* validate the ACL name */
  if (!aclName || (strlen(aclName) < L7_ARP_ACL_NAME_LEN_MIN) ||
                  (strlen(aclName) > L7_ARP_ACL_NAME_LEN_MAX) ||
                  (daiAclNameStringCheck(aclName) != L7_SUCCESS))
    return L7_ERROR;

  /* validate the vlanStart vlanEnd, val */
  if ((vlanStart > vlanEnd) || 
      ((val != L7_ENABLE) && (val != L7_DISABLE)))
  {
    return L7_ERROR;
  }

  if ((vlanStart == 0) || (vlanEnd > DAI_MAX_VLAN_COUNT))
  {
    return L7_ERROR;
  }

  if (osapiWriteLockTake(daiCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;

  for (i = vlanStart; i <= vlanEnd; i++)
  {
    if (val == L7_ENABLE)
    {
      _daiVlanArpAclSet(i, aclName);
      if(staticFlag == L7_ENABLE)
        _daiVlanStaticFlagSet(i);
      else
        _daiVlanStaticFlagClear(i);
    }
    else
    {
      _daiVlanArpAclGet(i, aclTemp);

      if(osapiStrncmp(aclTemp, aclName, L7_ARP_ACL_NAME_LEN_MAX+1) != 0)
      {
        /* acl name given while disabling is not matching the
         * acl name configured for this vlan. Return L7_FAILURE.
         * Those that matched for the vlans so far in this range
         * would be successfully disabled */
        osapiWriteLockGive(daiCfgRWLock);
        return L7_FAILURE;
      }
        
      _daiVlanArpAclClear(i);
      _daiVlanStaticFlagClear(i);
    }
    daiConfigDataChange();
  }

  osapiWriteLockGive(daiCfgRWLock);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  API to get the rate limit value of incoming ARP packets on
*           untrusted interfaces
*
* @param    intIfNum        @b((input)) internal interface number
* @param    rate            @b((output)) upper limit for number of
*                           incoming packets processed per second
*
* @returns  L7_SUCCESS, if value successfully updated
*           L7_ERROR, if inputs are invalid
*           L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t daiIntfRateLimitGet(L7_uint32 intIfNum, L7_int32 *rate)
{
  if((intIfNum <= 0) || (intIfNum >= platIntfMaxCountGet()))
  {
    return L7_ERROR;
  }

  if (osapiReadLockTake(daiCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  
  *rate           = daiCfgData->intfCfg[intIfNum].rate_limit;

  osapiReadLockGive(daiCfgRWLock);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  API to get the burst interval value of incoming ARP packets on
*           untrusted interfaces
*
* @param    intIfNum        @b((input)) internal interface number
* @param    burst_interval  @b((output)) the consecutive interval in
*                           seconds, over which the interface is
*                           monitored for a high rate of ARP packets
*
* @returns  L7_SUCCESS, if value successfully updated
*           L7_ERROR, if inputs are invalid
*           L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t daiIntfBurstIntervalGet(L7_uint32 intIfNum, L7_uint32 *burst_interval)
{
  if((intIfNum <= 0) || (intIfNum >= platIntfMaxCountGet()))
  {
    return L7_ERROR;
  }

  if (osapiReadLockTake(daiCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  
  *burst_interval = daiCfgData->intfCfg[intIfNum].burst_interval;

  osapiReadLockGive(daiCfgRWLock);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  API to set the rate limit value of incoming ARP packets on
*           untrusted interfaces
*
* @param    intIfNum        @b((input)) internal interface number
* @param    rate            @b((input)) upper limit for number of
*                           incoming packets processed per second
*                           If passed as -1, it means no upper limit
*                           is set for the rate.
*
* @returns  L7_SUCCESS, if value successfully updated
*           L7_ERROR, if inputs are invalid
*           L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t daiIntfRateLimitSet(L7_uint32 intIfNum, L7_int32 rate)
{
  if((intIfNum <= 0) || (intIfNum >= platIntfMaxCountGet()))
  {
    return L7_ERROR;
  }

  if ((rate != -1) && ((rate < L7_DAI_RATE_LIMIT_MIN) ||
                       (rate > L7_DAI_RATE_LIMIT_MAX)))
  {
    return L7_ERROR;
  }

  if (osapiWriteLockTake(daiCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;
  
  if(daiCfgData->intfCfg[intIfNum].rate_limit != rate)
  {
    daiCfgData->intfCfg[intIfNum].rate_limit = rate;

    daiIntfRateLimitApply(intIfNum);
  }

  osapiWriteLockGive(daiCfgRWLock);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  API to set the burst interval value of incoming ARP packets on
*           untrusted interfaces
*
* @param    intIfNum        @b((input)) internal interface number
* @param    burst_interval  @b((input)) the consecutive interval in
*                           seconds, over which the interface is
*                           monitored for a high rate of ARP packets
*
* @returns  L7_SUCCESS, if value successfully updated
*           L7_ERROR, if inputs are invalid
*           L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t daiIntfBurstIntervalSet(L7_uint32 intIfNum, L7_uint32 burst_interval)
{
  if((intIfNum <= 0) || (intIfNum >= platIntfMaxCountGet()))
  {
    return L7_ERROR;
  }

  if((burst_interval < L7_DAI_BURST_INTERVAL_MIN) ||
     (burst_interval > L7_DAI_BURST_INTERVAL_MAX))
  {
    return L7_ERROR;
  }

  if (osapiWriteLockTake(daiCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;
  
  /* Don't allow to set burst interval if rate limit is none */
  if (daiCfgData->intfCfg[intIfNum].rate_limit == -1)
  {
    osapiWriteLockGive(daiCfgRWLock);
    return L7_ERROR;
  }

  if(daiCfgData->intfCfg[intIfNum].burst_interval != burst_interval)
  {
    daiCfgData->intfCfg[intIfNum].burst_interval    = burst_interval;

    daiIntfRateLimitApply(intIfNum);
  }

  osapiWriteLockGive(daiCfgRWLock);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get DAI statistics on a VLAN
*
* @param    vlanId    @b((input))  VLAN Id
* @param    stats     @b((output)) stats for given Vlan 
*
* @returns  L7_SUCCESS
*           L7_FAILURE
*           L7_ERROR  if Vlan is invalid
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t daiVlanStatsGet(L7_uint32 vlanId, daiVlanStats_t *stats)
{
  if ((vlanId == 0) || (vlanId > DAI_MAX_VLAN_COUNT))
    return L7_ERROR;

  if (osapiReadLockTake(daiCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS) 
   return L7_FAILURE;

  stats->forwarded       = daiVlanInfo[vlanId].stats.forwarded;
  stats->dropped         = daiVlanInfo[vlanId].stats.dropped;
  stats->dhcpDrops       = daiVlanInfo[vlanId].stats.dhcpDrops;
  stats->dhcpPermits     = daiVlanInfo[vlanId].stats.dhcpPermits;
  stats->aclDrops        = daiVlanInfo[vlanId].stats.aclDrops;
  stats->aclPermits      = daiVlanInfo[vlanId].stats.aclPermits;
  stats->sMacFailures    = daiVlanInfo[vlanId].stats.sMacFailures;
  stats->dMacFailures    = daiVlanInfo[vlanId].stats.dMacFailures;
  stats->ipValidFailures = daiVlanInfo[vlanId].stats.ipValidFailures;

  osapiReadLockGive(daiCfgRWLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Clear DAI statistics on all vlans
*
* @returns  L7_SUCCESS
*           L7_FAILURE
*           L7_ERROR  if Vlan is invalid
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t daiVlanStatsClear(void)
{
  if (osapiWriteLockTake(daiCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS) 
   return L7_FAILURE;

  _daiVlanStatsClear();

  osapiWriteLockGive(daiCfgRWLock);
  return L7_SUCCESS;
}

/********************************************************************/
/*                   ARP ACL Related APIs                           */
/*                                                                  */
/* ARP ACLs are part of Dynamic ARP Inspection as they are used     */
/* only by DAI.                                                     */
/* Later when it becomes something common in Fastpath               */
/* we can move these APIs to different files                        */
/********************************************************************/

/*********************************************************************
* @purpose  API to create ARP ACL
*
* @param    aclName @b((input))  ACL Name
*
* @returns  L7_SUCCESS
*           L7_FAILURE if the max number of ARP ACLs reached already
*
* @notes    We allow ARP ACL name as a string that can take
*           alphanumeric characters.
*
* @end
*********************************************************************/
L7_RC_t arpAclCreate(L7_uchar8 *aclName)
{
  L7_RC_t rc;

  if (!aclName || (strlen(aclName) < L7_ARP_ACL_NAME_LEN_MIN) ||
                  (strlen(aclName) > L7_ARP_ACL_NAME_LEN_MAX) ||
                  (daiAclNameStringCheck(aclName) != L7_SUCCESS))
    return L7_ERROR;

  if (osapiStrncmp(aclName, ARP_ACL_NULL, L7_ARP_ACL_NAME_LEN_MAX+1) == 0)
    return L7_FAILURE;

  if (osapiWriteLockTake(daiCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;
  
  if((rc = _arpAclCreate(aclName)) == L7_SUCCESS)
  {
    daiConfigDataChange();
  }
  osapiWriteLockGive(daiCfgRWLock);
  return rc;
}

/*********************************************************************
* @purpose  API to delete ARP ACL
*
* @param    aclName @b((input))  ACL Name
*
* @returns  L7_SUCCESS
*           L7_FAILURE if no ARP ACL exists with that name
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t arpAclDelete(L7_uchar8 *aclName)
{
  L7_RC_t rc;

  if (!aclName || (strlen(aclName) < L7_ARP_ACL_NAME_LEN_MIN) ||
                  (strlen(aclName) > L7_ARP_ACL_NAME_LEN_MAX))
    return L7_ERROR;

  if (osapiStrncmp(aclName, ARP_ACL_NULL, L7_ARP_ACL_NAME_LEN_MAX+1) == 0)
    return L7_FAILURE;

  if (osapiWriteLockTake(daiCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;
  
  if((rc = _arpAclDelete(aclName)) == L7_SUCCESS)
  {
    daiConfigDataChange();
  }
  osapiWriteLockGive(daiCfgRWLock);
  return rc;
}

/*********************************************************************
* @purpose  API to Check if an ARP ACL exists
*
* @param    aclName @b((input))  ACL Name
*
* @returns  L7_SUCCESS
*           L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t arpAclGet(L7_uchar8 *aclName)
{
  L7_RC_t rc;

  if (!aclName || (strlen(aclName) > L7_ARP_ACL_NAME_LEN_MAX))
    return L7_ERROR;

  if (osapiStrncmp(aclName, ARP_ACL_NULL, L7_ARP_ACL_NAME_LEN_MAX+1) == 0)
    return L7_FAILURE;

  if (osapiReadLockTake(daiCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;
  
  rc = _arpAclGet(aclName);

  osapiReadLockGive(daiCfgRWLock);
  return rc;
}

/*********************************************************************
* @purpose  API to get the next ARP ACL entry
*
* @param    aclName     @b((input))  ACL Name
* @param    nextAclName @b((output)) Next ACL Name
*
* @returns  L7_SUCCESS
*           L7_FAILURE
*
* @notes   
*           memory for nextAclName should have been allocated by
*           the caller of this function
*
* @end
*********************************************************************/
L7_RC_t arpAclNextGet(L7_uchar8 *aclName, L7_uchar8 *nextAclName)
{
  L7_RC_t rc;

  if (!aclName)
    return L7_ERROR;

  if (osapiReadLockTake(daiCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;
  
  rc = _arpAclNextGet(aclName, nextAclName);

  osapiReadLockGive(daiCfgRWLock);
  return rc;
}

/*********************************************************************
* @purpose  API to add rule to ARP ACL
*
* @param    aclName @b((input))  ACL Name
* @param    ipAddr  @b((input))  Sender's IP address
* @param    macAddr @b((input))  Sender's MAC address in 
*                                xx:xx:xx:xx:xx:xx string format 
*
* @returns  L7_SUCCESS
*           L7_FAILURE if the max number of rules in ARP ACL have
*                      already reached
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t arpAclRuleAdd(L7_uchar8 *aclName, L7_uint32 ipAddr, L7_uchar8 *macAddr)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uchar8 macAddress[L7_ENET_MAC_ADDR_LEN];

  if (!aclName || (strlen(aclName) < L7_ARP_ACL_NAME_LEN_MIN) ||
                  (strlen(aclName) > L7_ARP_ACL_NAME_LEN_MAX))
    return L7_ERROR;

  /* Validate IP address. Don't allow 0, mcast or above, loopback. */
  if ((ipAddr == 0) ||
      (ipAddr >= (L7_uint32)L7_CLASS_D_ADDR_NETWORK) ||
      (((ipAddr & 0xff000000) >> 24) == 127))
    return L7_ERROR;

  memset(macAddress, 0, L7_ENET_MAC_ADDR_LEN);
  if(daiStringToMac(macAddr, (L7_uchar8 *)macAddress) == L7_SUCCESS)
  {
    /* don't allow multicast mac addresses as sender mac in a rule */
    if((macAddress[0] & 1) == 1)
      return L7_ERROR;

    if (osapiWriteLockTake(daiCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS)
      return L7_FAILURE;
  
    if((rc = _arpAclRuleAdd(aclName, ipAddr, macAddress)) == L7_SUCCESS)
    {
      daiConfigDataChange();
    }
    osapiWriteLockGive(daiCfgRWLock);
  }

  return rc;
}

/*********************************************************************
* @purpose  API to remove rule from ARP ACL
*
* @param    aclName @b((input))  ACL Name
* @param    ipAddr  @b((input))  Sender's IP address
* @param    macAddr @b((input))  Sender's MAC address in 
*                                xx:xx:xx:xx:xx:xx string format 
*
* @returns  L7_SUCCESS
*           L7_FAILURE if no ARP ACL exists or if the rule doesn't exist
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t arpAclRuleDelete(L7_uchar8 *aclName, L7_uint32 ipAddr, L7_uchar8 *macAddr)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uchar8 macAddress[L7_ENET_MAC_ADDR_LEN];

  if (!aclName || (strlen(aclName) < L7_ARP_ACL_NAME_LEN_MIN) ||
                  (strlen(aclName) > L7_ARP_ACL_NAME_LEN_MAX))
    return L7_ERROR;

  /* Don't allow a rule for NULL ipaddress */
  if (ipAddr == 0)
    return L7_FAILURE;

  if (osapiWriteLockTake(daiCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;
  
  memset(macAddress, 0, L7_ENET_MAC_ADDR_LEN);
  if(daiStringToMac(macAddr, (L7_uchar8 *)macAddress) == L7_SUCCESS)
  {
    if((rc = _arpAclRuleDelete(aclName, ipAddr, macAddress)) == L7_SUCCESS)
    {
      daiConfigDataChange();
    }
  }

  osapiWriteLockGive(daiCfgRWLock);
  return rc;
}

/*********************************************************************
* @purpose  API to Check if an ARP ACL Rule exists
*
* @param    aclName @b((input))  ACL Name
* @param    ipAddr  @b((input))  Sender's IP address
* @param    macAddr @b((input))  Sender's MAC address in 
*                                xx:xx:xx:xx:xx:xx string format 
*
* @returns  L7_SUCCESS
*           L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t arpAclRuleGet(L7_uchar8 *aclName, L7_uint32 ipAddr, L7_uchar8 *macAddr)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uchar8 macAddress[L7_ENET_MAC_ADDR_LEN];

  if (!aclName || (strlen(aclName) > L7_ARP_ACL_NAME_LEN_MAX))
    return L7_ERROR;

  if (osapiStrncmp(aclName, ARP_ACL_NULL, L7_ARP_ACL_NAME_LEN_MAX+1) == 0)
    return L7_FAILURE;

  /* Don't allow a rule for NULL ipaddress */
  if (ipAddr == 0)
    return L7_FAILURE;

  if (osapiReadLockTake(daiCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;
  
  memset(macAddress, 0, L7_ENET_MAC_ADDR_LEN);
  if(osapiStrncmp(macAddr, "", MAC_STR_LEN) != 0)
  {
    if(daiStringToMac(macAddr, (L7_uchar8 *)macAddress) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
  }

  rc = _arpAclRuleGet(aclName, ipAddr, macAddress);
  if(rc == L7_SUCCESS)
  {
    daiMacToString(macAddress, macAddr);
  }

  osapiReadLockGive(daiCfgRWLock);
  return rc;
}

/*********************************************************************
* @purpose  API to get the next ARP ACL Rule from all the ACL list
*
* @param    aclNameIn @b((input))  ACL Name
* @param    ipAddrIn  @b((input))  Sender's IP address
* @param    macAddrIn @b((input))  Sender's MAC address in
*                                  xx:xx:xx:xx:xx:xx string format 
* @param    aclNameOut @b((output))  ACL Name
* @param    ipAddrOut  @b((output))  Sender's IP address
* @param    macAddrOut @b((output))  Sender's MAC address in
*                                    xx:xx:xx:xx:xx:xx string format 
*
* @returns  L7_SUCCESS
*           L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t arpAclRuleNextGet(L7_uchar8 *aclNameIn, L7_uint32 ipAddrIn, L7_uchar8 *macAddrIn,
                          L7_uchar8 *aclNameOut, L7_uint32 *ipAddrOut, L7_uchar8 *macAddrOut)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uchar8 macAddressIn[L7_ENET_MAC_ADDR_LEN];
  L7_uchar8 macAddressOut[L7_ENET_MAC_ADDR_LEN];

  if (!aclNameIn || !aclNameOut || (strlen(aclNameIn) > L7_ARP_ACL_NAME_LEN_MAX) ||
      (strlen(aclNameOut) > L7_ARP_ACL_NAME_LEN_MAX))
    return L7_ERROR;


  memset(macAddressIn, 0, L7_ENET_MAC_ADDR_LEN);
  memset(macAddressOut, 0, L7_ENET_MAC_ADDR_LEN);

  /* aclNameIn would be ARP_ACL_NULL to get the first rule. We need not
   * convert MAC input string in such a case */
  if(osapiStrncmp(aclNameIn, ARP_ACL_NULL, L7_ARP_ACL_NAME_LEN_MAX+1) != 0)
  {
    if(osapiStrncmp(macAddrIn, "", MAC_STR_LEN) != 0)
    {
      if(daiStringToMac(macAddrIn, (L7_uchar8 *)macAddressIn) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }
    }
  }

  if (osapiReadLockTake(daiCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;
  
  rc = _arpAclRuleNextGet(aclNameIn, ipAddrIn, macAddressIn,
                          aclNameOut, ipAddrOut, macAddressOut);
  if(rc == L7_SUCCESS)
  {
    daiMacToString(macAddressOut, macAddrOut);
  }

  osapiReadLockGive(daiCfgRWLock);
  return rc;
}

/*********************************************************************
* @purpose  API to get the next ARP ACL Rule in the given ACL
*
* @param    aclNameIn @b((input))  ACL Name
* @param    ipAddrIn  @b((input))  Sender's IP address
* @param    macAddrIn @b((input))  Sender's MAC address in
*                                  xx:xx:xx:xx:xx:xx string format 
* @param    ipAddrOut  @b((output))  Sender's IP address
* @param    macAddrOut @b((output))  Sender's MAC address in
*                                    xx:xx:xx:xx:xx:xx string format 
*
* @returns  L7_SUCCESS
*           L7_FAILURE
*
* @notes    This API would be used by CLI and WEB
*
* @end
*********************************************************************/
L7_RC_t arpAclRuleInAclNextGet(L7_uchar8 *aclName, L7_uint32 ipAddrIn, L7_uchar8 *macAddrIn,
                               L7_uint32 *ipAddrOut, L7_uchar8 *macAddrOut)
{
  L7_RC_t rc = L7_FAILURE;
  L7_uchar8 macAddressIn[L7_ENET_MAC_ADDR_LEN];
  L7_uchar8 macAddressOut[L7_ENET_MAC_ADDR_LEN];

  if (!aclName || (strlen(aclName) > L7_ARP_ACL_NAME_LEN_MAX)) 
    return L7_ERROR;

#define MAC_STR_LEN 17

  memset(macAddressIn, 0, L7_ENET_MAC_ADDR_LEN);
  memset(macAddressOut, 0, L7_ENET_MAC_ADDR_LEN);

  if(osapiStrncmp(aclName, ARP_ACL_NULL, L7_ARP_ACL_NAME_LEN_MAX+1) == 0)
    return L7_FAILURE;

  if(osapiStrncmp(macAddrIn, "", MAC_STR_LEN) != 0)
  {
    if(daiStringToMac(macAddrIn, (L7_uchar8 *)macAddressIn) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
  }

  if (osapiReadLockTake(daiCfgRWLock, L7_WAIT_FOREVER) != L7_SUCCESS)
    return L7_FAILURE;
  
  rc = _arpAclRuleInAclNextGet(aclName, ipAddrIn, macAddressIn,
                               ipAddrOut, macAddressOut);
  if(rc == L7_SUCCESS)
  {
    daiMacToString(macAddressOut, macAddrOut);
  }

  osapiReadLockGive(daiCfgRWLock);
  return rc;
}
