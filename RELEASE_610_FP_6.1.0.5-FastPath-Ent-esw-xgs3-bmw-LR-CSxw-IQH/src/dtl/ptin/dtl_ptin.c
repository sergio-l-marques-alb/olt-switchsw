#include "dtlinclude.h"
#include "IPC.h"
#include "usmdb_dot3ad_api.h"

L7_RC_t dtlPtinInit(void)
{
  DAPI_USP_t ddUsp;
  nimUSP_t usp;
  L7_RC_t dr;

  if (nimGetUnitSlotPort(1, &usp) != L7_SUCCESS)
  {
    printf("error with nimGetUnitSlotPort\n");
    return L7_FAILURE;
  }
  else
  {
    ddUsp.unit = usp.unit;
    ddUsp.slot = usp.slot;
    ddUsp.port = usp.port - 1;
  }

  dr = dapiCtl(&ddUsp, DAPI_CMD_PTIN_INIT, NULL);
  if (dr == L7_SUCCESS)  {
    return L7_SUCCESS;
  } else {
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

L7_RC_t dtlPtinPhySet(L7_uint32 intIfNum, L7_uint8 tx_disable)
{
  DAPI_USP_t ddUsp;
  nimUSP_t usp;
  L7_RC_t dr;

  if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
  {
    printf("error with nimGetUnitSlotPort\n");
    return L7_FAILURE;
  }
  else
  {
    ddUsp.unit = usp.unit;
    ddUsp.slot = usp.slot;
    ddUsp.port = usp.port - 1;
  }

  dr = dapiCtl(&ddUsp, DAPI_CMD_PTIN_HW_PHY_SET, (void *) &tx_disable);
  if (dr == L7_SUCCESS)  {
    return L7_SUCCESS;
  } else {
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

L7_RC_t dtlPtinHwResources(st_ptin_hw_resources *resources)
{
  DAPI_USP_t ddUsp;
  L7_RC_t dr;

  ddUsp.unit = -1;
  ddUsp.slot = -1;
  ddUsp.port = -1;

  dr = dapiCtl(&ddUsp, DAPI_CMD_PTIN_HW_RESOURCES, (void *) resources);
  if (dr == L7_SUCCESS)  {
    return L7_SUCCESS;
  } else {
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

L7_RC_t dtlPtinVlanDefs( L7_vlan_defs *data )
{
  DAPI_USP_t ddUsp;
  nimUSP_t usp;
  L7_RC_t dr;

  if (nimGetUnitSlotPort(1, &usp) != L7_SUCCESS)
  {
    printf("error with nimGetUnitSlotPort\n");
    return L7_FAILURE;
  }
  else
  {
    ddUsp.unit = usp.unit;
    ddUsp.slot = usp.slot;
    ddUsp.port = usp.port - 1;
  }

  dr = dapiCtl(&ddUsp, DAPI_CMD_PTIN_VLAN_DEFS, (void *) data);
  if (dr == L7_SUCCESS)  {
    return L7_SUCCESS;
  } else {
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

L7_RC_t dtlPtinVlanXlate( L7_vlan_translation *data )
{
  DAPI_USP_t ddUsp;
  nimUSP_t usp;

  if (nimGetUnitSlotPort(1, &usp) != L7_SUCCESS)
  {
    printf("error with nimGetUnitSlotPort\n");
    return L7_FAILURE;
  }
  else
  {
    ddUsp.unit = usp.unit;
    ddUsp.slot = usp.slot;
    ddUsp.port = usp.port - 1;
  }

  return dapiCtl(&ddUsp, DAPI_CMD_PTIN_VLAN_XLATE, (void *) data);
}

L7_RC_t dtlPtinFPentry( L7_fpentry_oper *data )
{
  DAPI_USP_t ddUsp;
  nimUSP_t usp;

  if (nimGetUnitSlotPort(1, &usp) != L7_SUCCESS)
  {
    printf("error with nimGetUnitSlotPort\n");
    return L7_FAILURE;
  }
  else
  {
    ddUsp.unit = usp.unit;
    ddUsp.slot = usp.slot;
    ddUsp.port = usp.port - 1;
  }

  return dapiCtl(&ddUsp, DAPI_CMD_PTIN_FPENTRY, (void *) data );
}


L7_RC_t dtlPtinFlowCounters( st_ptin_fcounters_oper *data )
{
  DAPI_USP_t ddUsp;
  nimUSP_t usp;

  if (nimGetUnitSlotPort(1, &usp) != L7_SUCCESS)
  {
    printf("error with nimGetUnitSlotPort\n");
    return L7_FAILURE;
  }
  else
  {
    ddUsp.unit = usp.unit;
    ddUsp.slot = usp.slot;
    ddUsp.port = usp.port - 1;
  }

  return dapiCtl(&ddUsp, DAPI_CMD_PTIN_FLOW_COUNTERS, (void *) data );
}


L7_RC_t dtlPtinBcastPktLimit(L7_uint32 admin, L7_uint32 intIfNum, L7_uint32 vlanId)
{
  DAPI_USP_t ddUsp;
  nimUSP_t usp;
  L7_RC_t dr;
  L7_uint32 nPorts=L7_SYSTEM_N_PORTS, i;
  L7_uint32 member_list[L7_SYSTEM_N_PORTS];
  st_ptin_args args;

  printf("%s(%d) admin=%u, intIfNum=%u, vlanId=%u\r\n",__FUNCTION__,__LINE__,admin,intIfNum,vlanId);

  // Check if is a lag interface
  if (usmDbDot3adValidIntfCheck(1,intIfNum))
  {
    // Get list of ports, of this LAG
    nPorts = L7_SYSTEM_N_PORTS;
    if (usmDbDot3adMemberListGet(1,intIfNum,&nPorts,member_list)!=L7_SUCCESS || nPorts==0)
    {
      return L7_FAILURE;
    }
  }
  else
  {
    nPorts = 1;
    member_list[0] = intIfNum;
  }

  for (i=0; i<nPorts; i++)
  {
    if (member_list[i]==0 || member_list[i]==L7_ALL_INTERFACES ||
        nimGetUnitSlotPort(member_list[i], &usp) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }
    else
    {
      ddUsp.unit = usp.unit;
      ddUsp.slot = usp.slot;
      ddUsp.port = usp.port - 1;
    }

    args.admin  = admin & 1;
    args.index  = 0;
    args.vlanId = vlanId;

    dr = dapiCtl(&ddUsp, DAPI_CMD_PTIN_BCASTPKT_LIMIT, (void *) &args );
    if (dr != L7_SUCCESS)
      return dr;
  }
  return L7_SUCCESS;
}

L7_RC_t dtlPtinBitStreamUpFwd(L7_uint32 intIfNum, L7_uint32 admin, L7_uint16 index, L7_uint32 vlanId)
{
  DAPI_USP_t ddUsp;
  nimUSP_t   usp;
  L7_RC_t dr;
  st_ptin_args args;

  if (nimGetUnitSlotPort(intIfNum,&usp)==L7_SUCCESS)
  {
    ddUsp.unit = usp.unit;
    ddUsp.slot = usp.slot;
    ddUsp.port = usp.port-1;
  }
  else
  {
    return L7_FAILURE;
  }

  args.admin        = admin & 1;
  args.index        = index;
  args.vlanId       = vlanId;

  dr = dapiCtl(&ddUsp, DAPI_CMD_PTIN_BITSTREAM_UPFWD, (void *) &args );
  if (dr != L7_SUCCESS)
    return dr;

  return L7_SUCCESS;
}

L7_RC_t dtlPtinBitStreamLagRecheck(L7_uint32 intIfNum, L7_uint32 admin)
{
  DAPI_USP_t ddUsp;
  nimUSP_t   usp;
  L7_RC_t dr;
  st_ptin_args args;

  if (nimGetUnitSlotPort(intIfNum,&usp)==L7_SUCCESS)
  {
    ddUsp.unit = usp.unit;
    ddUsp.slot = usp.slot;
    ddUsp.port = usp.port-1;
  }
  else
  {
    return L7_FAILURE;
  }

  args.admin = admin & 1;

  dr = dapiCtl(&ddUsp, DAPI_CMD_PTIN_BITSTREAM_LAG_RECHECK, (void *) &args );
  if (dr != L7_SUCCESS)
    return dr;

  return L7_SUCCESS;
}

L7_RC_t dtlPtinGetBoardState(L7_HwCardInfo *data)
{
  nimUSP_t usp;

  if (nimGetUnitSlotPort(1, &usp) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  memset(data,0x00,sizeof(L7_HwCardInfo));

  data->BoardPresent = 1;
  data->BoardSerialNumber[ 0]='F';
  data->BoardSerialNumber[ 1]='A';
  data->BoardSerialNumber[ 2]='S';
  data->BoardSerialNumber[ 3]='T';
  data->BoardSerialNumber[ 4]='P';
  data->BoardSerialNumber[ 5]='A';
  data->BoardSerialNumber[ 6]='T';
  data->BoardSerialNumber[ 7]='H';
  data->BoardSerialNumber[ 8]=' ';
  data->BoardSerialNumber[ 9]='6';
  data->BoardSerialNumber[10]='.';
  data->BoardSerialNumber[11]='1';
  data->BoardSerialNumber[12]='0';
  data->BoardSerialNumber[13]='5';
  data->BoardSerialNumber[14]='\0';
  data->BoardSerialNumber[15]='\0';
  data->BoardSerialNumber[16]='\0';
  data->BoardSerialNumber[17]='\0';
  data->BoardSerialNumber[18]='\0';
  data->BoardSerialNumber[19]='\0';

  return L7_SUCCESS;
}


L7_RC_t dtlPtinGetCounters(L7_HWEthRFC2819_PortStatistics *data)
{
  /* If interface is AAL5, no call to dapi needed */
  DAPI_USP_t ddUsp;
  nimUSP_t usp;
  L7_RC_t dr;

  if (nimGetUnitSlotPort(1, &usp) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  else
  {
    ddUsp.unit = usp.unit;
    ddUsp.slot = usp.slot;
    ddUsp.port = usp.port - 1;
  }

  dr = dapiCtl(&ddUsp, DAPI_CMD_PTIN_GET_COUNTERS, (void *) data);
  if (dr == L7_SUCCESS)
    return L7_SUCCESS;
  else
    return L7_FAILURE;
}


L7_RC_t dtlPtinClearCounters(L7_HWEthRFC2819_PortStatistics *data)
{
  /* If interface is AAL5, no call to dapi needed */
  DAPI_USP_t ddUsp;
  nimUSP_t usp;
  L7_RC_t dr;

  if (nimGetUnitSlotPort(1, &usp) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  else
  {
    ddUsp.unit = usp.unit;
    ddUsp.slot = usp.slot;
    ddUsp.port = usp.port - 1;
  }

  dr = dapiCtl(&ddUsp, DAPI_CMD_PTIN_CLEAR_COUNTERS, (void *) data);
  if (dr == L7_SUCCESS)
    return L7_SUCCESS;
  else
    return L7_FAILURE;
}

L7_RC_t dtlPtinGetCountersState(L7_HWEth_Statistics_State *data)
{
  /* If interface is AAL5, no call to dapi needed */
  DAPI_USP_t ddUsp;
  nimUSP_t usp;
  L7_RC_t dr;

  if (nimGetUnitSlotPort(1, &usp) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  else
  {
    ddUsp.unit = usp.unit;
    ddUsp.slot = usp.slot;
    ddUsp.port = usp.port - 1;
  }

  dr = dapiCtl(&ddUsp, DAPI_CMD_PTIN_GET_COUNTERS_STATE, (void *) data);
  if (dr == L7_SUCCESS)
    return L7_SUCCESS;
  else
    return L7_FAILURE;
}

