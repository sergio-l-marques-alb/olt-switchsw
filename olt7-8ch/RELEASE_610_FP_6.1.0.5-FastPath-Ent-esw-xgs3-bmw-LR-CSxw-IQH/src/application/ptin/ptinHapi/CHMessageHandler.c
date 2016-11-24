#include "ptinHapi_include.h"
#include "IPC.h"
#include "usmdb_1213_api.h"
#include "CHMessageHandler.h"
#include "dtlinclude.h"
#include "ptin_msg.h"
#include "tty_ptin.h"
#include "usmdb_mib_bridge_api.h"

#include "ptin/globaldefs.h"

int global_var_trap_sent=FALSE;
int global_var_system_ready=FALSE;

static L7_uint16 get_SIRerror(L7_RC_t error_code)
{
	switch ((int) error_code)
	{
	case L7_TABLE_IS_FULL:
		return ERROR_CODE_FULLTABLE;
		break;
    case L7_REQUEST_DENIED:
         return ERROR_CODE_CONFLICT_REQ;
         break;
	case L7_NOT_SUPPORTED:
		return ERROR_CODE_NOTSUPPORTED;
		break;
	}

	// Default error
	return ERROR_CODE_INVALIDPARAM;
}

int CHMessageHandler (ipc_msg *inbuffer, ipc_msg *outbuffer)
{
  L7_RC_t rc = L7_SUCCESS;
	int res = S_OK;
  (void)outbuffer;

  if (inbuffer==NULL) {
    DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_HAPI, TRACE_SEVERITY_WARNING,  "NULL message received.");
    return SIR_ERROR(ERROR_FAMILY_IPC, ERROR_SEVERITY_ERROR, ERROR_CODE_EMPTYMSG);
  }

  DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_HAPI, TRACE_SEVERITY_WARNING,  "IPC Message Received!!!");

  // Prepara o cabecalho da mensagem de resposta
  SwapIPCHeader (inbuffer,outbuffer);

  switch (inbuffer->msgId) {
  case CCMSG_DEBUG_SHOW:
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL,  "Recebeu a mensagem CCMSG_DEBUG_SHOW.");
    {
      st_DebugParams auxdebug;
      st_debug       *p;

      GetDebugSettings ((st_DebugParams*) &auxdebug);

      p = (st_debug*) outbuffer->info;
      p->mask        = auxdebug.mask;
      p->tracemode   = auxdebug.debugFormat;
      p->tracemask   = (auxdebug.debugModules & 0xFFFF0000) | (auxdebug.debugLayers & 0x0000FFFF);
      p->traceoutput = auxdebug.debugOutput;
      memcpy (p->tty, auxdebug.debugDevice, 50);
      p->tty[49] = 0;
      outbuffer->infoDim = sizeof (st_debug);
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL,  "Mensagem processada, resposta com %d bytes.", outbuffer->infoDim);

    }
    break; // CCMSG_DEBUG_SHOW (V2.9.2.060222)

  case CCMSG_DEBUG_CONFIG:
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL,  "Recebeu a mensagem CCMSG_DEBUG_CONFIG.");
    // Valida parametros de entrada
    if (inbuffer->infoDim!=sizeof(st_debug))
      SetIPCNACK (outbuffer, SIR_ERROR(ERROR_FAMILY_IPC, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE));
    else {  // Mensagem valida
      st_DebugParams auxdebug;
      st_debug       *p;
      p = (st_debug*) inbuffer->info;
      auxdebug.mask           = p->mask;
      auxdebug.debugFormat    = p->tracemode;
      auxdebug.debugModules   = (p->tracemask & 0xFFFF0000);
      auxdebug.debugLayers    = (p->tracemask & 0x0000FFFF);
      auxdebug.debugOutput    = p->traceoutput;
      memcpy (auxdebug.debugDevice, p->tty, 50);
      auxdebug.debugDevice[49] = 0;
      if ((res=ptinHapiconfigDebugTrace((st_DebugParams*) &auxdebug, 0))==L7_SUCCESS) {
        outbuffer->infoDim = sizeof (int);
        *(unsigned int*)outbuffer->info = 0;
        DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL,  "Mensagem processada, resposta com %d bytes.", outbuffer->infoDim);
      } else
        SetIPCNACK (outbuffer, res);

    }
    break; // CCMSG_DEBUG_CONFIG (V2.9.2.060222)


  case 0x1002:
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Show IP TTL");
    {
      if ( sizeof(UINT) != inbuffer->infoDim ) {
        SetIPCNACK(outbuffer, SIR_ERROR(ERROR_FAMILY_IPC, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE));
      } else {
        if ( L7_SUCCESS == (rc=usmDbIpDefaultTTLGet( (*(L7_uint32 *) inbuffer->info), (L7_uint32 *) outbuffer->info ))) {
          outbuffer->infoDim = sizeof(L7_uint32);
          DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Mensagem processada, resposta com %d bytes", outbuffer->infoDim);
        } else {
					res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, get_SIRerror(rc));
          SetIPCNACK(outbuffer, res);
        }
      }
    }
    break;

  case 0x1003:
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Set IP TTL %d ", ((L7_uint32 *) outbuffer->info)[1]);
    {
      if ( 2 * sizeof(UINT) != inbuffer->infoDim ) {
        SetIPCNACK(outbuffer, SIR_ERROR(ERROR_FAMILY_IPC, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE));
      } else {
        if (L7_SUCCESS == (rc=usmDbIpDefaultTTLSet( ((L7_uint32 *) inbuffer->info)[0], ((L7_uint32 *) inbuffer->info)[1] ))) {

          outbuffer->infoDim = sizeof(L7_uint32);
          *((UINT *) outbuffer->info) = 0;
          DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Mensagem processada, resposta com %d bytes", outbuffer->infoDim);
        } else {
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, get_SIRerror(rc));
          SetIPCNACK(outbuffer, res);
        }
      }
    }
    break;

  case 0x1004:
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Show IP TTL");
    {
      if ( sizeof(UINT) != inbuffer->infoDim ) {
        SetIPCNACK(outbuffer, SIR_ERROR(ERROR_FAMILY_IPC, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE));
      } else {
        if ( L7_SUCCESS == (rc = usmDbIpRtrAdminModeGet( (*(L7_uint32 *) inbuffer->info), (L7_uint32 *) outbuffer->info ))) {
          outbuffer->infoDim = sizeof(L7_uint32);
          DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Mensagem processada, resposta com %d bytes", outbuffer->infoDim);
        } else {
					res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, get_SIRerror(rc));
          SetIPCNACK(outbuffer, res);
        }
      }
    }
    break;

  case 0x1005:
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Set IP TTL %d ", ((L7_uint32 *) outbuffer->info)[1]);
    {
      if ( 2 * sizeof(UINT) != inbuffer->infoDim ) {
        SetIPCNACK(outbuffer, SIR_ERROR(ERROR_FAMILY_IPC, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE));
      } else {
        if (L7_SUCCESS == (rc=usmDbIpRtrAdminModeSet( ((L7_uint32 *) inbuffer->info)[0], ((L7_uint32 *) inbuffer->info)[1] ))) {
          outbuffer->infoDim = sizeof(L7_uint32);
          *((UINT *) outbuffer->info) = 0;
          DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Mensagem processada, resposta com %d bytes", outbuffer->infoDim);
        } else {
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, get_SIRerror(rc));
          SetIPCNACK(outbuffer, res);
        }
      }
    }
    break;

  case CCMSG_RESET_ALARMS:
    printf("Resetting Alarms\n");
    {
      // Validate message size
      if (inbuffer->infoDim<4)  {
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE);
        SetIPCNACK(outbuffer, res);
        break;
      }

      if (L7_SUCCESS==(rc=ptin_msg_reset_alarms())) {
        outbuffer->infoDim = sizeof(L7_uint32);
        *((UINT *) outbuffer->info) = 0;
        DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Mensagem processada, resposta com %d bytes", outbuffer->infoDim);
      } else {
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, get_SIRerror(rc));
        SetIPCNACK(outbuffer, res);
      }
    }
    break;

  case CCMSG_RESET_DEFAULTS:
    printf("Resetting dynamic services\n");
    {
      // Validate message size
      if (inbuffer->infoDim<4)  {
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE);
        SetIPCNACK(outbuffer, res);
        break;
      }

      if (L7_SUCCESS==(rc=ptin_msg_set_defaults( (uint32) *((uint32 *) &(inbuffer->info[0]) ) )) ) {
        outbuffer->infoDim = sizeof(L7_uint32);
        *((UINT *) outbuffer->info) = 0;
        DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Mensagem processada, resposta com %d bytes", outbuffer->infoDim);
      } else {
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, get_SIRerror(rc));
        SetIPCNACK(outbuffer, res);
      }
    }
    break;

  case CCMSG_BOARD_SHOW:
    printf("reading board state\n");
    {
      L7_HwCardInfo *po = (L7_HwCardInfo *) &outbuffer->info[0];

      if (global_var_system_ready && (L7_SUCCESS==dtlPtinGetBoardState(po))) {
        outbuffer->infoDim = sizeof(L7_HwCardInfo);
        DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Mensagem processada, resposta com %d bytes", outbuffer->infoDim);
        global_var_trap_sent = TRUE;
      } else {
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, get_SIRerror(rc) );
        SetIPCNACK(outbuffer, res);
      }
    }
    break;

  case CCMSG_APPLICATION_IS_ALIVE:
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "ping");
    outbuffer->infoDim = sizeof(L7_uint32);
    *((UINT *) outbuffer->info) = 0;
    DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "ping reply: resposta com %d bytes", outbuffer->infoDim);
    break;

  case CCMSG_APPLICATION_RESOURCES:
    printf("CCMSG_APPLICATION_RESOURCES\n");
    {
      st_msg_ptin_hw_resources *po = (st_msg_ptin_hw_resources *) &outbuffer->info[0];

      if (L7_SUCCESS==(rc=ptin_msg_HwResources(po))) {
        outbuffer->infoDim = sizeof(st_msg_ptin_hw_resources);
        DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Mensagem processada, resposta com %d bytes", outbuffer->infoDim);
      } else {
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, get_SIRerror(rc));
        SetIPCNACK(outbuffer, res);
      }
    }
    break;

  case CCMSG_ETH_SET_PHY_CONFIG:
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "config interface");
    printf("config interface\n");
    {
      uint8 i, n;
      L7_HWEthPhyConf *pi = (L7_HWEthPhyConf *) &inbuffer->info[0];

      if (inbuffer->infoDim<sizeof(L7_HWEthPhyConf) || (inbuffer->infoDim%sizeof(L7_HWEthPhyConf))!=0) {
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE);
          SetIPCNACK(outbuffer, res);
          break;
      }

      n = inbuffer->infoDim/sizeof(L7_HWEthPhyConf);
      if (n>L7_SYSTEM_N_PORTS)  n=L7_SYSTEM_N_PORTS;

      for (i=0; i<n; i++) {
        if (L7_SUCCESS != (rc=ptin_msg_setPhyConfig( &pi[i] )))  break;
      }
      if (i!=0 && i>=n) {
        outbuffer->infoDim = sizeof(L7_uint32);
        *((UINT *) outbuffer->info) = 0;
        DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Mensagem processada, resposta com %d bytes", outbuffer->infoDim);
      } else {
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, get_SIRerror(rc) );
        SetIPCNACK(outbuffer, res);
      }
    }
    break;

  case CCMSG_ETH_GET_PHY_CONFIG:
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "reading phy configurations");
    printf("reading phy configurations\n");
    {
      uint8 index;
      L7_HWEthPhyConf *pi = (L7_HWEthPhyConf *) &inbuffer->info[0];
      L7_HWEthPhyConf *po = (L7_HWEthPhyConf *) &outbuffer->info[0];

      if (inbuffer->infoDim<2) {
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE);
          SetIPCNACK(outbuffer, res);
          break;
      }

      if (pi->Port!=(uint8)-1) {
        memcpy(po,pi,sizeof(L7_HWEthPhyConf));
        if (L7_SUCCESS == (rc=ptin_msg_getPhyConfig( po ))) {
          outbuffer->infoDim = sizeof(L7_HWEthPhyConf);
          DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Mensagem processada, resposta com %d bytes", outbuffer->infoDim);
        } else {
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, get_SIRerror(rc));
          SetIPCNACK(outbuffer, res);
        }
      } else {
        for (index=0; index<L7_SYSTEM_N_PORTS; index++) {
          memcpy(&po[index],pi,sizeof(L7_HWEthPhyConf));
          po[index].Port=index;
          if (L7_SUCCESS != ptin_msg_getPhyConfig( &po[index] ))  break;
        }
        if (index>=L7_SYSTEM_N_PORTS) {
          outbuffer->infoDim = sizeof(L7_HWEthPhyConf)*L7_SYSTEM_N_PORTS;
          DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Mensagem processada, resposta com %d bytes", outbuffer->infoDim);
        }
        else {
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, get_SIRerror(rc));
          SetIPCNACK(outbuffer, res);
        }
      }
    }
    break;

  case CCMSG_ETH_GET_PHY_STATE:
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "reading phy states");
    printf("reading phy states\n");
    {
      uint8 index;
      L7_HWEthPhyState *pi = (L7_HWEthPhyState *) &inbuffer->info[0];
      L7_HWEthPhyState *po = (L7_HWEthPhyState *) &outbuffer->info[0];

      if (inbuffer->infoDim<2) {
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE);
          SetIPCNACK(outbuffer, res);
          break;
      }

      if (pi->Port!=(uint8)-1) {
        memcpy(po,pi,sizeof(L7_HWEthPhyState));  
        if (L7_SUCCESS == (rc=ptin_msg_getPhyState( po ))) {
          printf("Port=%u\n"
                 "Mask=%u\n"
                 "LinkUp=%u\n"
                 "Media=%u\n"
                 "Speed=%u\n"
                 "AutoNegComplete=%u\n"
                 "Duplex=%u\n"
                 "LOS=%u\n"
                 "RemoteFault=%u\n"
                 "Collisions=%u\n"
                 "RxActivity=%u\n"
                 "TxActivity=%u\n"
                 "MTU_mismatch=%u\n"
                 "Suported_MaxFrame=%u\n",
                 po->Port,
                 po->Mask,
                 po->LinkUp,
                 po->Media,
                 po->Speed,
                 po->AutoNegComplete,
                 po->Duplex,
                 po->LOS,
                 po->RemoteFault,
                 po->Collisions,
                 po->RxActivity,
                 po->TxActivity,
                 po->MTU_mismatch,
                 po->Suported_MaxFrame);
          outbuffer->infoDim = sizeof(L7_HWEthPhyState);
          DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Mensagem processada, resposta com %d bytes", outbuffer->infoDim);
        } else {
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, get_SIRerror(rc) );
          SetIPCNACK(outbuffer, res);
        }
      } else {
        for (index=0; index<L7_SYSTEM_N_PORTS; index++) {
          memcpy(&po[index],pi,sizeof(L7_HWEthPhyState));
          po[index].Port=index;
          if (L7_SUCCESS != (rc=ptin_msg_getPhyState( &po[index] )))  break;
        }
        if (index>=L7_SYSTEM_N_PORTS) {
          outbuffer->infoDim = sizeof(L7_HWEthPhyState)*L7_SYSTEM_N_PORTS;
          DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Mensagem processada, resposta com %d bytes", outbuffer->infoDim);
        }
        else {
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, get_SIRerror(rc) );
          SetIPCNACK(outbuffer, res);
        }
      }
    }
    break;

  case CCMSG_ETH_PORT_TYPE_SET:
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "config port type");
    printf("config port type\n");
    {
      outbuffer->infoDim = sizeof(L7_uint32);
      *((UINT *) outbuffer->info) = 0;
      DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Mensagem processada, resposta com %d bytes", outbuffer->infoDim);
    }
    break;

  case CCMSG_ETH_GET_VLAN_COUNTERS:
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "reading counters");
    printf("reading vlan counters\n");
    {
      L7_HWEth_VlanStatistics *ptr;
      L7_uint16 nClients;

      if (inbuffer->infoDim<3) {
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE);
          SetIPCNACK(outbuffer, res);
          break;
      }

      memcpy(outbuffer->info,inbuffer->info,sizeof(L7_HWEth_VlanStatistics));

      ptr = (L7_HWEth_VlanStatistics *) &outbuffer->info[0];
      if (L7_SUCCESS == (rc=ptin_msg_getVlanCounters( (L7_HWEth_VlanStatistics *) &outbuffer->info[0], &nClients))) {
        outbuffer->infoDim = sizeof(L7_HWEth_VlanStatistics)*nClients;
        DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Mensagem processada, resposta com %d bytes", outbuffer->infoDim);
        DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s(%d) Success\n", __FUNCTION__, __LINE__);
      } else {
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, get_SIRerror(rc) );
        SetIPCNACK(outbuffer, res);
      }
    }
    break;

  case CCMSG_ETH_GET_COUNTERS:
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "reading counters");
    printf("reading port counters\n");
    {
      L7_HWEthRFC2819_PortStatistics *ptr;

      if (inbuffer->infoDim<2) {
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE);
          SetIPCNACK(outbuffer, res);
          break;
      }

      memcpy(outbuffer->info,inbuffer->info,sizeof(L7_HWEthRFC2819_PortStatistics));

      ptr = (L7_HWEthRFC2819_PortStatistics *) &outbuffer->info[0];
      if (L7_SUCCESS == (rc=ptin_msg_getCounters( (L7_HWEthRFC2819_PortStatistics *) &outbuffer->info[0] ))) {
        outbuffer->infoDim = sizeof(L7_HWEthRFC2819_PortStatistics);
        DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Mensagem processada, resposta com %d bytes", outbuffer->infoDim);
        printf("Porta : %u\n",     ptr->Port);
        printf("Mask  : 0x%02x\n", ptr->Mask);
        printf("RxMask: 0x%04x\n", ptr->RxMask);
        printf("TxMask: 0x%04x\n", ptr->TxMask);
      } else {
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, get_SIRerror(rc) );
        SetIPCNACK(outbuffer, res);
      }
    }
    break;

  case CCMSG_ETH_CLEAR_COUNTERS:
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "clearing counters");
    printf("clearing counters\n");
    {
      if (inbuffer->infoDim<2) {
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE);
          SetIPCNACK(outbuffer, res);
          break;
      }

      if (L7_SUCCESS == (rc=ptin_msg_clearCounters((L7_HWEthRFC2819_PortStatistics *) &inbuffer->info[0])) ) {
        outbuffer->infoDim = sizeof(L7_uint32);
        *((UINT *) outbuffer->info) = 0;
        DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Mensagem processada, resposta com %d bytes", outbuffer->infoDim);
      } else {
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, get_SIRerror(rc) );
        SetIPCNACK(outbuffer, res);
      }
    }
    break;

  case CCMSG_ETH_GET_FLOW_COUNTERS_ABS:
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "CCMSG_ETH_GET_FLOW_COUNTERS_ABS");
    printf("CCMSG_ETH_GET_FLOW_COUNTERS\n");
    {
      st_ptin_flow_counters *ptr;

      if (inbuffer->infoDim!=sizeof(st_ptin_flow_counters)) {
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE);
          SetIPCNACK(outbuffer, res);
          break;
      }

      memcpy(outbuffer->info,inbuffer->info,sizeof(st_ptin_flow_counters));
      ptr = (st_ptin_flow_counters *) &outbuffer->info[0];

      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "EVC index      = %llu",ptr->index);
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Client/Channel = %lu",ptr->c_id.client_vlan);

      if (L7_SUCCESS == (rc=ptin_msg_getFlowCounters( ptr ))) {
        outbuffer->infoDim = sizeof(st_ptin_flow_counters);
/*        ptr->stats.eth_rx.pktTotal     = 100;
        ptr->stats.eth_rx.pktUnicast   = 10;
        ptr->stats.eth_rx.pktMulticast = 20;
        ptr->stats.eth_rx.pktBroadcast = 30;
        ptr->stats.eth_rx.pktDropped   = 40;

        ptr->stats.pon_rx.pktTotal     = 1000;
        ptr->stats.pon_rx.pktUnicast   = 100;
        ptr->stats.pon_rx.pktMulticast = 200;
        ptr->stats.pon_rx.pktBroadcast = 300;
        ptr->stats.pon_rx.pktDropped   = 400;
*/
        DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Mensagem processada, resposta com %d bytes", outbuffer->infoDim);
        DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s(%d) Success\n", __FUNCTION__, __LINE__);
      } else {
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, get_SIRerror(rc) );
        SetIPCNACK(outbuffer, res);
      }
    }
    break;

  case CCMSG_ETH_ADD_FLOW_COUNTERS:
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "CCMSG_ETH_ADD_FLOW_COUNTERS");
    {
      st_ptin_flow_counters *ptr;

      if (inbuffer->infoDim<sizeof(st_ptin_flow_counters)) {
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE);
          SetIPCNACK(outbuffer, res);
          break;
      }

      ptr = (st_ptin_flow_counters *) &inbuffer->info[0];

      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "EVC index      = %llu",ptr->index);
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Client/Channel = %lu",ptr->c_id.client_vlan);

      if (L7_SUCCESS == (rc=ptin_msg_addFlowCounters( ptr ))) {
        outbuffer->infoDim = sizeof(L7_uint32);
        *((UINT *) outbuffer->info) = 0;
        DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Mensagem processada, resposta com %d bytes", outbuffer->infoDim);
      } else {
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, get_SIRerror(rc) );
        SetIPCNACK(outbuffer, res);
      }
    }
    break;

  case CCMSG_ETH_REMOVE_FLOW_COUNTERS:
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "CCMSG_ETH_REMOVE_FLOW_COUNTERS");
    {
      st_ptin_flow_counters *ptr;

      if (inbuffer->infoDim<sizeof(st_ptin_flow_counters)) {
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE);
          SetIPCNACK(outbuffer, res);
          break;
      }

      ptr = (st_ptin_flow_counters *) &inbuffer->info[0];

      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "EVC index      = %llu",ptr->index);
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Client/Channel = %lu",ptr->c_id.client_vlan);

      if (L7_SUCCESS == (rc=ptin_msg_removeFlowCounters( ptr ))) {
        outbuffer->infoDim = sizeof(L7_uint32);
        *((UINT *) outbuffer->info) = 0;
        DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Mensagem processada, resposta com %d bytes", outbuffer->infoDim);
      } else {
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, get_SIRerror(rc) );
        SetIPCNACK(outbuffer, res);
      }
    }
    break;

  case CCMSG_ETH_EVC_READ:
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "read flow");
    printf("read flow\n");
    {
      if (inbuffer->infoDim!=sizeof(L7_HwEthernetMef10EvcBundling)) {
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE);
          SetIPCNACK(outbuffer, res);
          break;
      }

      memcpy(&outbuffer->info[0],&inbuffer->info[0],sizeof(L7_HwEthernetMef10EvcBundling));
      if (L7_SUCCESS == (rc=ptin_msg_EVCread( (L7_HwEthernetMef10EvcBundling *) &outbuffer->info[0] ))) {
        outbuffer->infoDim = sizeof(L7_HwEthernetMef10EvcBundling);
        DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Mensagem processada, resposta com %d bytes", outbuffer->infoDim);
      } else {
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, get_SIRerror(rc) );
        SetIPCNACK(outbuffer, res);
      }
    }
    break;

  case CCMSG_ETH_EVC_CREATE:
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "create flow");
    printf("create flow\n");
    {
      L7_HwEthernetMef10EvcBundling *ptr;

      if (inbuffer->infoDim!=sizeof(L7_HwEthernetMef10EvcBundling)) {
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE);
          SetIPCNACK(outbuffer, res);
          break;
      }

      ptr = (L7_HwEthernetMef10EvcBundling *) &inbuffer->info[0];

      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "EVC Index = 0x%llX",ptr->index);
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "EVC Id    = %llu",ptr->index & 0xFFFF);
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "EVC Admin = %u",ptr->admin);
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "EVC Type  = %u",ptr->type);
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "LAG id    = %u (0x%llX)",(L7_uint16) (ptr->index>>32) & 0xFF,ptr->index);
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "LAG vlan  = %u",ptr->outerTag);

      if (L7_SUCCESS == (rc=ptin_msg_EVCcreate( (L7_HwEthernetMef10EvcBundling *) &inbuffer->info[0] ))) {
        outbuffer->infoDim = sizeof(L7_uint32);
        *((UINT *) outbuffer->info) = 0;
        DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Mensagem processada, resposta com %d bytes", outbuffer->infoDim);
      } else {
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, get_SIRerror(rc) );
        SetIPCNACK(outbuffer, res);
      }
    }
    break;

  case CCMSG_ETH_EVC_DESTROY:
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "destroy flow");
    printf("destroy_flow\n");
    {
      if (inbuffer->infoDim!=sizeof(L7_HwEthernetMef10p1EvcRemove)) {
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE);
          SetIPCNACK(outbuffer, res);
          break;
      }

      if (L7_SUCCESS == (rc=ptin_msg_EVCdestroy( (L7_HwEthernetMef10p1EvcRemove *) &inbuffer->info[0] ))) {
        outbuffer->infoDim = sizeof(L7_uint32);
        *((UINT *) outbuffer->info) = 0;
        DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Mensagem processada, resposta com %d bytes", outbuffer->infoDim);
      } else {
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, get_SIRerror(rc) );
        SetIPCNACK(outbuffer, res);
      }
    }
    break;

  case CCMSG_ETH_PROFILE_GET:
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "get profile");
    printf("get profile\n");
    {
      L7_uint16 nProfiles;
      if (inbuffer->infoDim!=sizeof(L7_HwEthernetProfile)) {
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE);
          SetIPCNACK(outbuffer, res);
          break;
      }
      memcpy(&outbuffer->info[0],&inbuffer->info[0],sizeof(L7_HwEthernetProfile));
      if ((rc=ptin_msg_GetProfile((L7_HwEthernetProfile *) &outbuffer->info[0],&nProfiles))==L7_SUCCESS)  {
        outbuffer->infoDim = sizeof(L7_HwEthernetProfile)*nProfiles;
        DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Mensagem processada, resposta com %d bytes", outbuffer->infoDim);
      } else {
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, get_SIRerror(rc) );
        SetIPCNACK(outbuffer, res);
      }
    }
    break;

  case CCMSG_ETH_PROFILE_SET:
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "set profile");
    printf("set profile\n");
    {
      if (inbuffer->infoDim!=sizeof(L7_HwEthernetProfile)) {
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE);
          SetIPCNACK(outbuffer, res);
          break;
      }

      if ((rc=ptin_msg_SetProfile((L7_HwEthernetProfile *) &inbuffer->info[0]))==L7_SUCCESS)  {
        outbuffer->infoDim = sizeof(L7_uint32);
        *((UINT *) outbuffer->info) = 0;
        DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Mensagem processada, resposta com %d bytes", outbuffer->infoDim);
      } else {
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, get_SIRerror(rc) );
        SetIPCNACK(outbuffer, res);
      }
    }
    break;

  case CCMSG_ETH_PROFILE_REMOVE:
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "remove ETH profile");
    {
      if (inbuffer->infoDim!=sizeof(L7_HwEthernetProfile)) {
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE);
          SetIPCNACK(outbuffer, res);
          break;
      }

      if ((rc=ptin_msg_RemoveProfile((L7_HwEthernetProfile *) &inbuffer->info[0]))==L7_SUCCESS)  {
        outbuffer->infoDim = sizeof(L7_uint32);
        *((UINT *) outbuffer->info) = 0;
        DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Mensagem processada, resposta com %d bytes", outbuffer->infoDim);
      } else {
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, get_SIRerror(rc) );
        SetIPCNACK(outbuffer, res);
      }
    }
    break;

  case CCMSG_ETH_DHCP_PROFILE_GET:
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "read DHCP option 82 profile");
    printf("read DHCP option 82 profile\n");
    {
      if (inbuffer->infoDim!=sizeof(L7_HwEthernetDhcpOpt82Profile)) {
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE);
          SetIPCNACK(outbuffer, res);
          break;
      }

      memcpy(outbuffer->info, inbuffer->info, sizeof(L7_HwEthernetDhcpOpt82Profile));
      outbuffer->infoDim = sizeof(L7_HwEthernetDhcpOpt82Profile);

      if ((rc=ptin_msg_dhcpOpt82Get((L7_HwEthernetDhcpOpt82Profile *) &outbuffer->info[0]))==L7_SUCCESS)  {
        DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Mensagem processada, resposta com %d bytes", outbuffer->infoDim);
      } else {
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, get_SIRerror(rc) );
        SetIPCNACK(outbuffer, res);
      }
    }
    break;

  case CCMSG_ETH_DHCP_ACCESS_NODE_ID_SET:
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "set DHCP Access Node Identifier");
    printf("set DHCP option 82 Access Node Identifier\n");
    {
      if (inbuffer->infoDim!=sizeof(L7_HwEthernetAccessNodeId)) {
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE);
          SetIPCNACK(outbuffer, res);
          break;
      }

      if ((rc=ptin_msg_dhcpOpt82AccessNodeUpdate((L7_HwEthernetAccessNodeId *) &inbuffer->info[0]))==L7_SUCCESS)  {
        outbuffer->infoDim = sizeof(L7_uint32);
        *((UINT *) outbuffer->info) = 0;
        DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Mensagem processada, resposta com %d bytes", outbuffer->infoDim);
      } else {
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, get_SIRerror(rc) );
        SetIPCNACK(outbuffer, res);
      }
    }
    break;

  case CCMSG_ETH_DHCP_PROFILE_CONFIG:
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "set DHCP option 82 profile");
    printf("set DHCP option 82 profile\n");
    {
      if (inbuffer->infoDim!=sizeof(L7_HwEthernetDhcpOpt82Profile)) {
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE);
          SetIPCNACK(outbuffer, res);
          break;
      }

      if ((rc=ptin_msg_dhcpOpt82Config((L7_HwEthernetDhcpOpt82Profile *) &inbuffer->info[0]))==L7_SUCCESS)  {
        outbuffer->infoDim = sizeof(L7_uint32);
        *((UINT *) outbuffer->info) = 0;
        DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Mensagem processada, resposta com %d bytes", outbuffer->infoDim);
      } else {
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, get_SIRerror(rc) );
        SetIPCNACK(outbuffer, res);
      }
    }
    break;

  case CCMSG_ETH_DHCP_PROFILE_REMOVE:
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "remove DHCP Option82 profile");
    {
      if (inbuffer->infoDim!=sizeof(L7_HwEthernetDhcpOpt82Profile)) {
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE);
          SetIPCNACK(outbuffer, res);
          break;
      }

      if ((rc=ptin_msg_dhcpOpt82Remove((L7_HwEthernetDhcpOpt82Profile *) &inbuffer->info[0]))==L7_SUCCESS)  {
        outbuffer->infoDim = sizeof(L7_uint32);
        *((UINT *) outbuffer->info) = 0;
        DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Mensagem processada, resposta com %d bytes", outbuffer->infoDim);
      } else {
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, get_SIRerror(rc) );
        SetIPCNACK(outbuffer, res);
      }
    }
    break;

  case CCMSG_ETH_LACP_LAG_SHOW:
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "read LAG properties");
    printf("read LAG properties\n");
    {
      L7_LACPLagInfo *ptr;
      L7_uint8 nLags;

      if (inbuffer->infoDim!=sizeof(L7_LACPLagInfo)) {
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE);
          SetIPCNACK(outbuffer, res);
          break;
      }

      memcpy(outbuffer->info, inbuffer->info, sizeof(L7_LACPLagInfo));
      ptr = (L7_LACPLagInfo *) &outbuffer->info[0];

      if ((rc=ptin_msg_LAGGet( ptr, &nLags ))==L7_SUCCESS && nLags>0)  {
        outbuffer->infoDim = sizeof(L7_LACPLagInfo)*nLags;
        DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Mensagem processada, resposta com %d bytes", outbuffer->infoDim);
      } else {
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, get_SIRerror(rc) );
        SetIPCNACK(outbuffer, res);
      }
    }
    break;

  case CCMSG_ETH_LACP_LAG_CREATE:
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "LAG Create");
    {
      L7_LACPLagInfo *ptr;
  
      if (inbuffer->infoDim!=sizeof(L7_LACPLagInfo)) {
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_INVALIDPARAM);
          SetIPCNACK(outbuffer, res);
          break;
      }
  
      ptr = (L7_LACPLagInfo *) &inbuffer->info[0];
  
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "LAG Index = %llu",ptr->index);
  
      if (L7_SUCCESS == (rc=ptin_msg_LAGCreate( ptr ))) {
        outbuffer->infoDim = sizeof(L7_uint32);
        *((UINT *) outbuffer->info) = 0;
        DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Mensagem processada, resposta com %d bytes", outbuffer->infoDim);
      } else {
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, get_SIRerror(rc) );
        SetIPCNACK(outbuffer, res);
      }
    }
    break;

  case CCMSG_ETH_LACP_LAG_DESTROY:
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "LAG Destroy");
    {
      L7_LACPLagInfo *ptr;

      if (inbuffer->infoDim!=sizeof(L7_LACPLagInfo)) {
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE);
          SetIPCNACK(outbuffer, res);
          break;
      }

      ptr = (L7_LACPLagInfo *) &inbuffer->info[0];

      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "LAG Index = %llu",ptr->index);

      if (L7_SUCCESS == (rc=ptin_msg_LAGDestroy( ptr ))) {
        outbuffer->infoDim = sizeof(L7_uint32);
        *((UINT *) outbuffer->info) = 0;
        DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Mensagem processada, resposta com %d bytes", outbuffer->infoDim);
      } else {
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, get_SIRerror(rc) );
        SetIPCNACK(outbuffer, res);
      }
    }
    break;

  case CCMSG_ETH_LACP_LAG_STATUS:
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "LAG Status");
    {
      L7_LACPLagStatus *ptr;
      L7_uint8 nLags;

      if (inbuffer->infoDim!=sizeof(L7_LACPLagStatus)) {
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE);
          SetIPCNACK(outbuffer, res);
          break;
      }

      memcpy(outbuffer->info, inbuffer->info, sizeof(L7_LACPLagStatus));
      ptr = (L7_LACPLagStatus *) &outbuffer->info[0];

      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "LAG Index = %llu",ptr->index);

      if (L7_SUCCESS == (rc=ptin_msg_LAGStatus( ptr, &nLags )) && nLags>0) {
        outbuffer->infoDim = sizeof(L7_LACPLagStatus)*nLags;
        DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Mensagem processada, resposta com %d bytes", outbuffer->infoDim);
      } else {
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, get_SIRerror(rc) );
        SetIPCNACK(outbuffer, res);
      }
    }
    break;

  case CCMSG_ETH_LACP_ADMINSTATE_SET:
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "LAG AdminState Set");
    {
      L7_LACPAdminState *ptr;
      L7_uint8 nStructs;

      // Validate size
      if (inbuffer->infoDim<sizeof(L7_LACPAdminState) || (inbuffer->infoDim%sizeof(L7_LACPAdminState))!=0) {
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE);
          SetIPCNACK(outbuffer, res);
          break;
      }
      nStructs = inbuffer->infoDim/sizeof(L7_LACPAdminState);
      ptr = (L7_LACPAdminState *) &inbuffer->info[0];

      if ( L7_SUCCESS == (rc=ptin_msg_LAGAdminStateSet( &ptr[0], nStructs )) ) {
        outbuffer->infoDim = sizeof(L7_uint32);
        *((UINT *) outbuffer->info) = 0;
        DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Mensagem processada, resposta com %d bytes", outbuffer->infoDim);
      } else {
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, get_SIRerror(rc) );
        SetIPCNACK(outbuffer, res);
      }
    }
    break;

  case CCMSG_ETH_LACP_ADMINSTATE_GET:
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "LAG AdminState Get");
    {
      L7_LACPAdminState *pi, *po;
      L7_uint8 nStructs;

      // Validate size
      if (inbuffer->infoDim<sizeof(L7_LACPAdminState)) {
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE);
          SetIPCNACK(outbuffer, res);
          break;
      }
      pi = (L7_LACPAdminState *) &inbuffer->info[0];
      po = (L7_LACPAdminState *) &outbuffer->info[0];
      if (L7_SUCCESS == (rc=ptin_msg_LAGAdminStateGet( (L7_int) pi->index, &po[0], &nStructs )) && nStructs>0 )  {
        outbuffer->infoDim = sizeof(L7_LACPAdminState)*nStructs;
        DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Mensagem processada, resposta com %d bytes", outbuffer->infoDim);
      } else {
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, get_SIRerror(rc) );
        SetIPCNACK(outbuffer, res);
      }
    }
    break;

  case CCMSG_ETH_LACP_STATS_SHOW:
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "sending LACPdu stats");
    {
      L7_LACPStats *pi, *po;
      L7_uint8 nStructs;
      L7_int port;

      if (inbuffer->infoDim!=sizeof(L7_LACPStats)) {
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE);
          SetIPCNACK(outbuffer, res);
          break;
      }

      pi = (L7_LACPStats *) &inbuffer->info[0];
      po = (L7_LACPStats *) &outbuffer->info[0];
      port = ((pi->index & 0xFF)!=0xFF) ? (pi->index & 0xFF) : -1;
      memcpy(po,pi,sizeof(L7_LACPStats));

      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Port = %d",port);

      if (L7_SUCCESS == (rc=ptin_msg_LACPduStat_get( port, po, &nStructs )) && nStructs!=0) {
        DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Mensagem processada, resposta com %d bytes", outbuffer->infoDim);
        outbuffer->infoDim = sizeof(L7_LACPStats)*nStructs;
      } else {
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, get_SIRerror(rc) );
        SetIPCNACK(outbuffer, res);
      }
    }
    break;

  case CCMSG_ETH_LACP_STATS_CLEAR:
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Clearing LACPdu stats");
    {
      L7_LACPStats *ptr;

      if (inbuffer->infoDim!=sizeof(L7_LACPStats)) {
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE);
          SetIPCNACK(outbuffer, res);
          break;
      }

      ptr = (L7_LACPStats *) &inbuffer->info[0];

      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Port = %llu",ptr->index);

      if (L7_SUCCESS == (rc=ptin_msg_LACPduStat_clr(ptr))) {
        DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Mensagem processada, resposta com %d bytes", outbuffer->infoDim);
        outbuffer->infoDim = sizeof(L7_uint32);
        *((UINT *) outbuffer->info) = 0;
      } else {
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, get_SIRerror(rc) );
        SetIPCNACK(outbuffer, res);
      }
    }
    break;

  case CCMSG_ETH_NTW_CONNECTIVITY_SHOW:
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Network Connectivity show");
    {
      L7_NtwConnectivity *ptr;

      if (inbuffer->infoDim!=sizeof(L7_NtwConnectivity)) {
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE);
          SetIPCNACK(outbuffer, res);
          break;
      }

      memcpy(outbuffer->info, inbuffer->info, sizeof(L7_NtwConnectivity));
      ptr = (L7_NtwConnectivity *) &outbuffer->info[0];

      if (L7_SUCCESS == (rc=ptin_msg_showNtwConnectivity( ptr ))) {
        outbuffer->infoDim = sizeof(L7_NtwConnectivity);
        DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Mensagem processada, resposta com %d bytes", outbuffer->infoDim);
      } else {
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, get_SIRerror(rc) );
        SetIPCNACK(outbuffer, res);
      }
    }
    break;

  case CCMSG_ETH_NTW_CONNECTIVITY:
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Network Connectivity Configure");
    {
      L7_NtwConnectivity *ptr;

      if (inbuffer->infoDim!=sizeof(L7_NtwConnectivity)) {
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE);
          SetIPCNACK(outbuffer, res);
          break;
      }

      ptr = (L7_NtwConnectivity *) &inbuffer->info[0];

      if (L7_SUCCESS == (rc=ptin_msg_ntwConnectivity( ptr ))) {
        outbuffer->infoDim = sizeof(L7_uint32);
        *((UINT *) outbuffer->info) = 0;
        DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Mensagem processada, resposta com %d bytes", outbuffer->infoDim);
      } else {
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, get_SIRerror(rc) );
        SetIPCNACK(outbuffer, res);
      }
    }
    break;

  case CCMSG_ETH_ROUTE_CONNECTIVITY:
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Route Connectivity Configure");
    {
      L7_RouteConnectivity *ptr;

      if (inbuffer->infoDim!=sizeof(L7_RouteConnectivity)) {
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE);
          SetIPCNACK(outbuffer, res);
          break;
      }

      ptr = (L7_RouteConnectivity *) &inbuffer->info[0];

      if (L7_SUCCESS == (rc=ptin_msg_routeConnectivity( ptr ))) {
        outbuffer->infoDim = sizeof(L7_uint32);
        *((UINT *) outbuffer->info) = 0;
        DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Mensagem processada, resposta com %d bytes", outbuffer->infoDim);
      } else {
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, get_SIRerror(rc) );
        SetIPCNACK(outbuffer, res);
      }
    }
    break;

  case CCMSG_ETH_SHELL_CMD:

    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Executing drivShell command...");
    {
      if (inbuffer->infoDim==0) {
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE);
          SetIPCNACK(outbuffer, res);
          break;
      }

      if ((rc=ptin_msg_Shell_Command((L7_char8 *) &inbuffer->info[0]))==L7_SUCCESS)  {
        outbuffer->infoDim = sizeof(L7_uint32);
        *((UINT *) outbuffer->info) = 0;
        DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Mensagem processada, resposta com %d bytes", outbuffer->infoDim);
      } else {
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, get_SIRerror(rc) );
        SetIPCNACK(outbuffer, res);
      }
    }
    break;

  case CCMSG_ETH_MC_FLOW_ADD:
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "add multicast Flow");
    {
      L7_ClientIgmp *ptr;
      L7_uint16 number_of_flows;

      if (inbuffer->infoDim<sizeof(L7_ClientIgmp)) {
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE);
          SetIPCNACK(outbuffer, res);
          break;
      }

      number_of_flows = inbuffer->infoDim/sizeof(L7_ClientIgmp);

      ptr = (L7_ClientIgmp *) &inbuffer->info[0];

//    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "EVC Index = %llu",ptr->index);
//    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "CVid      = %u",ptr->cvid);
//    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Port      = %u",ptr->port);

      if (L7_SUCCESS == (rc=ptin_msg_IgmpFlowAdd( ptr, number_of_flows ))) {
        outbuffer->infoDim = sizeof(L7_uint32);
        *((UINT *) outbuffer->info) = 0;
        DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Mensagem processada, resposta com %d bytes", outbuffer->infoDim);
      } else {
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, get_SIRerror(rc) );
        SetIPCNACK(outbuffer, res);
      }
    }
    break;

  case CCMSG_ETH_MC_FLOW_REMOVE:
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "remove multicast Flow");
    {
      L7_ClientIgmp *ptr;
      L7_uint16 number_of_flows;

      if (inbuffer->infoDim<sizeof(L7_ClientIgmp)) {
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE);
          SetIPCNACK(outbuffer, res);
          break;
      }

      number_of_flows = inbuffer->infoDim/sizeof(L7_ClientIgmp);

      ptr = (L7_ClientIgmp *) &inbuffer->info[0];

//    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "EVC Index = %llu",ptr->index);
//    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "CVid      = %u",ptr->cvid);
//    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Port      = %u",ptr->port);

      if (L7_SUCCESS == (rc=ptin_msg_IgmpFlowRemove( ptr, number_of_flows ))) {
        outbuffer->infoDim = sizeof(L7_uint32);
        *((UINT *) outbuffer->info) = 0;
        DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Mensagem processada, resposta com %d bytes", outbuffer->infoDim);
      } else {
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, get_SIRerror(rc) );
        SetIPCNACK(outbuffer, res);
      }
    }
    break;

  case CCMSG_ETH_MC_CLIENT_ADD:
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "add multicast client");
    {
      L7_ClientIgmp *ptr;
      L7_uint16 number_of_clients;

      if (inbuffer->infoDim<sizeof(L7_ClientIgmp)) {
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE);
          SetIPCNACK(outbuffer, res);
          break;
      }
      number_of_clients = inbuffer->infoDim/sizeof(L7_ClientIgmp);

      ptr = (L7_ClientIgmp *) &inbuffer->info[0];

//    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "EVC Index = %llu",ptr->index);
//    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "CVid      = %u",ptr->cvid);
//    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Port      = %u",ptr->port);

      if (L7_SUCCESS == (rc=ptin_msg_IgmpClientAdd( ptr, number_of_clients ))) {
        outbuffer->infoDim = sizeof(L7_uint32);
        *((UINT *) outbuffer->info) = 0;
        DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Mensagem processada, resposta com %d bytes", outbuffer->infoDim);
      } else {
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, get_SIRerror(rc) );
        SetIPCNACK(outbuffer, res);
      }
    }
    break;

  case CCMSG_ETH_MC_CLIENT_REMOVE:
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "remove multicast client");
    {
      L7_ClientIgmp *ptr;
      L7_uint16 number_of_clients;

      if (inbuffer->infoDim<sizeof(L7_ClientIgmp)) {
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE);
          SetIPCNACK(outbuffer, res);
          break;
      }
      number_of_clients = inbuffer->infoDim/sizeof(L7_ClientIgmp);

      ptr = (L7_ClientIgmp *) &inbuffer->info[0];

//    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "EVC Index = %llu",ptr->index);
//    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "CVid      = %u",ptr->cvid);
//    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Port      = %u",ptr->port);

      if (L7_SUCCESS == (rc=ptin_msg_IgmpClientRemove( ptr, number_of_clients ))) {
        outbuffer->infoDim = sizeof(L7_uint32);
        *((UINT *) outbuffer->info) = 0;
        DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Mensagem processada, resposta com %d bytes", outbuffer->infoDim);
      } else {
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, get_SIRerror(rc) );
        SetIPCNACK(outbuffer, res);
      }
    }
    break;

  case CCMSG_ETH_MC_CLIENT_STATS_CLEAR:
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "clearing multicast client stats");
    {
      L7_ClientIgmpStatistics *ptr;

      if (inbuffer->infoDim==0) {
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE);
          SetIPCNACK(outbuffer, res);
          break;
      }

      ptr = (L7_ClientIgmpStatistics *) &inbuffer->info[0];

      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "EVC Index = %llu",ptr->index);
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "CVid      = %u",ptr->cvid);

      if (L7_SUCCESS == (rc=ptin_msg_IgmpClientStatsClear( ptr ))) {
        outbuffer->infoDim = sizeof(L7_uint32);
        *((UINT *) outbuffer->info) = 0;
        DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Mensagem processada, resposta com %d bytes", outbuffer->infoDim);
      } else {
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, get_SIRerror(rc) );
        SetIPCNACK(outbuffer, res);
      }
    }
    break;

  case CCMSG_ETH_MC_PORT_STATS_CLEAR:
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "clearing multicast port stats");
    {
      L7_ClientIgmpStatistics *ptr;

      if (inbuffer->infoDim==0) {
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE);
          SetIPCNACK(outbuffer, res);
          break;
      }

      ptr = (L7_ClientIgmpStatistics *) &inbuffer->info[0];

      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "EVC Index = %llu",ptr->index);
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Port      = %u",ptr->port);

      if (L7_SUCCESS == (rc=ptin_msg_IgmpPortStatsClear( ptr ))) {
        outbuffer->infoDim = sizeof(L7_uint32);
        *((UINT *) outbuffer->info) = 0;
        DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Mensagem processada, resposta com %d bytes", outbuffer->infoDim);
      } else {
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, get_SIRerror(rc) );
        SetIPCNACK(outbuffer, res);
      }
    }
    break;

  case CCMSG_ETH_MC_CLIENT_STATS_SHOW:
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "sending multicast client stats");
    {
      L7_ClientIgmpStatistics *ptr;

      if (inbuffer->infoDim!=sizeof(L7_ClientIgmpStatistics)) {
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE);
          SetIPCNACK(outbuffer, res);
          break;
      }

      memcpy(&outbuffer->info[0],&inbuffer->info[0],sizeof(L7_ClientIgmpStatistics));
      outbuffer->infoDim = sizeof(L7_ClientIgmpStatistics);

      ptr = (L7_ClientIgmpStatistics *) &outbuffer->info[0];

      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "EVC Index = %llu",ptr->index);
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "CVid      = %u",ptr->cvid);

      if (L7_SUCCESS == (rc=ptin_msg_IgmpClientStatsGet( ptr ))) {
        DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Mensagem processada, resposta com %d bytes", outbuffer->infoDim);
      } else {
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, get_SIRerror(rc) );
        SetIPCNACK(outbuffer, res);
      }
    }
    break;

  case CCMSG_ETH_MC_PORT_STATS_SHOW:
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "sending multicast port stats");
    {
      L7_ClientIgmpStatistics *ptr;

      if (inbuffer->infoDim!=sizeof(L7_ClientIgmpStatistics)) {
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE);
          SetIPCNACK(outbuffer, res);
          break;
      }

      memcpy(&outbuffer->info[0],&inbuffer->info[0],sizeof(L7_ClientIgmpStatistics));
      outbuffer->infoDim = sizeof(L7_ClientIgmpStatistics);

      ptr = (L7_ClientIgmpStatistics *) &outbuffer->info[0];

      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "EVC Index = %llu",ptr->index);
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Port      = %u",ptr->port);

      if (L7_SUCCESS == (rc=ptin_msg_IgmpPortStatsGet( ptr ))) {
        DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Mensagem processada, resposta com %d bytes", outbuffer->infoDim);
      } else {
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, get_SIRerror(rc) );
        SetIPCNACK(outbuffer, res);
      }
    }
    break;

  case CCMSG_ETH_DHCP_CLIENT_STATS_SHOW:
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "sending DHCP client stats");
    {
      L7_ClientDhcpStatistics *ptr;

      if (inbuffer->infoDim!=sizeof(L7_ClientDhcpStatistics)) {
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE);
          SetIPCNACK(outbuffer, res);
          break;
      }

      memcpy(&outbuffer->info[0],&inbuffer->info[0],sizeof(L7_ClientDhcpStatistics));
      outbuffer->infoDim = sizeof(L7_ClientDhcpStatistics);

      ptr = (L7_ClientDhcpStatistics *) &outbuffer->info[0];

      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "EVC Index = %llu",ptr->index);
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "CVid      = %u",ptr->cvid);

      if (L7_SUCCESS == (rc=ptin_msg_DhcpClientStatsGet( ptr ))) {
        DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Mensagem processada, resposta com %d bytes", outbuffer->infoDim);
      } else {
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, get_SIRerror(rc) );
        SetIPCNACK(outbuffer, res);
      }
    }
    break;

  case CCMSG_ETH_DHCP_PORT_STATS_SHOW:
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "sending multicast port stats");
    {
      L7_ClientDhcpStatistics *ptr;

      if (inbuffer->infoDim!=sizeof(L7_ClientDhcpStatistics)) {
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE);
          SetIPCNACK(outbuffer, res);
          break;
      }

      memcpy(&outbuffer->info[0],&inbuffer->info[0],sizeof(L7_ClientDhcpStatistics));
      outbuffer->infoDim = sizeof(L7_ClientDhcpStatistics);

      ptr = (L7_ClientDhcpStatistics *) &outbuffer->info[0];

      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "EVC Index = %llu",ptr->index);
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Port      = %u",ptr->port);

      if (L7_SUCCESS == (rc=ptin_msg_DhcpPortStatsGet( ptr ))) {
        DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Mensagem processada, resposta com %d bytes", outbuffer->infoDim);
      } else {
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, get_SIRerror(rc) );
        SetIPCNACK(outbuffer, res);
      }
    }
    break;

  case CCMSG_ETH_DHCP_CLIENT_STATS_CLEAR:
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "clearing DHCP client stats");
    {
      L7_ClientDhcpStatistics *ptr;

      if (inbuffer->infoDim==0) {
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE);
          SetIPCNACK(outbuffer, res);
          break;
      }

      ptr = (L7_ClientDhcpStatistics *) &inbuffer->info[0];

      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "EVC Index = %llu",ptr->index);
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "CVid      = %u",ptr->cvid);

      if (L7_SUCCESS == (rc=ptin_msg_DhcpClientStatsClear( ptr ))) {
        outbuffer->infoDim = sizeof(L7_uint32);
        *((UINT *) outbuffer->info) = 0;
        DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Mensagem processada, resposta com %d bytes", outbuffer->infoDim);
      } else {
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, get_SIRerror(rc) );
        SetIPCNACK(outbuffer, res);
      }
    }
    break;

  case CCMSG_ETH_DHCP_PORT_STATS_CLEAR:
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "clearing DHCP port stats");
    {
      L7_ClientDhcpStatistics *ptr;

      if (inbuffer->infoDim==0) {
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE);
          SetIPCNACK(outbuffer, res);
          break;
      }

      ptr = (L7_ClientDhcpStatistics *) &inbuffer->info[0];

      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "EVC Index = %llu",ptr->index);
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Port      = %u",ptr->port);

      if (L7_SUCCESS == (rc=ptin_msg_DhcpPortStatsClear( ptr ))) {
        outbuffer->infoDim = sizeof(L7_uint32);
        *((UINT *) outbuffer->info) = 0;
        DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Mensagem processada, resposta com %d bytes", outbuffer->infoDim);
      } else {
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, get_SIRerror(rc) );
        SetIPCNACK(outbuffer, res);
      }
    }
    break;

  case CCMSG_ETH_SNOOP_MODE:
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "CCHMSG_ETH_SNOOP_MODE message");
    {
      st_snooping *ptr;

      if (inbuffer->infoDim!=sizeof(st_snooping)) {
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE);
          SetIPCNACK(outbuffer, res);
          break;
      }

      ptr = (st_snooping *) &inbuffer->info[0];

      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Snoop_admin=%u",              ptr->snooping_admin);
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "querier_admin=%u",            ptr->querier_admin);
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "querier_ipaddress=%u",        ptr->querier_ipaddress);
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "querier_interval=%u",         ptr->querier_interval);
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "group_membership_interval=%u",ptr->group_membership_interval);
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "igmp_version_router=%u",      ptr->igmp_version_router);
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "igmp_version=%u",             ptr->igmp_version);

      if (L7_SUCCESS == (rc=ptin_msg_snooping_querier_config( ptr ))) {
        outbuffer->infoDim = sizeof(L7_uint32);
        *((UINT *) outbuffer->info) = 0;
        DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Mensagem processada, resposta com %d bytes", outbuffer->infoDim);
      } else {
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, get_SIRerror(rc) );
        SetIPCNACK(outbuffer, res);
      }
    }
    break;

  case CCMSG_ETH_SNOOP_INTF_MANGMT:
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "CCHMSG_ETH_SNOOP_INTF_MANGMT message");
    {
      st_snooping_intf *ptr;

      if (inbuffer->infoDim!=sizeof(st_snooping_intf)) {
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE);
          SetIPCNACK(outbuffer, res);
          break;
      }

      ptr = (st_snooping_intf *) &inbuffer->info[0];

      if (L7_SUCCESS == (rc=ptin_msg_snooping_intf_config( ptr ))) {
        DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s(%d) success\n", __FUNCTION__, __LINE__);
        outbuffer->infoDim = sizeof(L7_uint32);
        *((UINT *) outbuffer->info) = 0;
        DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Mensagem processada, resposta com %d bytes", outbuffer->infoDim);
      } else {
        DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) failure\n", __FUNCTION__, __LINE__);
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, get_SIRerror(rc) );
        SetIPCNACK(outbuffer, res);
      }
    }
    break;

  case CCMSG_ETH_SNOOP_VLAN_MANGMT:
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "CCHMSG_ETH_SNOOP_VLAN_MANGMT");
    {
      st_snooping_vlan *ptr;
      L7_uint16 number_of_cells;

      if (inbuffer->infoDim<sizeof(st_snooping_vlan)) {
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE);
          SetIPCNACK(outbuffer, res);
          break;
      }
      number_of_cells = inbuffer->infoDim/sizeof(st_snooping_vlan);

      ptr = (st_snooping_vlan *) &inbuffer->info[0];

      if (L7_SUCCESS == (rc=ptin_msg_snooping_vlan_config( ptr, number_of_cells ))) {
        outbuffer->infoDim = sizeof(L7_uint32);
        *((UINT *) outbuffer->info) = 0;
        DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Mensagem processada, resposta com %d bytes", outbuffer->infoDim);
      } else {
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, get_SIRerror(rc) );
        SetIPCNACK(outbuffer, res);
      }
    }
    break;

  case CCMSG_ETH_SNOOP_QUERIER_MANGMT:
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "CCHMSG_ETH_SNOOP_QUERIER_MANGMT");
    {
      st_snooping_querier_vlan *ptr;
      L7_uint16 number_of_cells;

      if (inbuffer->infoDim<sizeof(st_snooping_querier_vlan)) {
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE);
          SetIPCNACK(outbuffer, res);
          break;
      }
      number_of_cells = inbuffer->infoDim/sizeof(st_snooping_querier_vlan);

      ptr = (st_snooping_querier_vlan *) &inbuffer->info[0];

      if (L7_SUCCESS == (rc=ptin_msg_snooping_querier_vlan_config( ptr, number_of_cells ))) {
        outbuffer->infoDim = sizeof(L7_uint32);
        *((UINT *) outbuffer->info) = 0;
        DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Mensagem processada, resposta com %d bytes", outbuffer->infoDim);
      } else {
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, get_SIRerror(rc) );
        SetIPCNACK(outbuffer, res);
      }
    }
    break;

    case CCMSG_ETH_IGMP_CHANNEL_WLIST_ADD:
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "CCMSG_ETH_IGMP_CHANNEL_WLIST_ADD");
      {
        msg_MCChannelWList_t *ptr;
        L7_uint16 number_of_cells;

        if (inbuffer->infoDim==0 || (inbuffer->infoDim%sizeof(msg_MCChannelWList_t))!=0) {
            res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE);
            SetIPCNACK(outbuffer, res);
            break;
        }
        number_of_cells = inbuffer->infoDim/sizeof(msg_MCChannelWList_t);

        ptr = (msg_MCChannelWList_t *) &inbuffer->info[0];

        #ifdef IGMP_WHITELIST_FEATURE
        if (L7_SUCCESS == (rc=ptin_msg_IGMP_WList_add( ptr, number_of_cells )))
        {
          outbuffer->infoDim = sizeof(L7_uint32);
          *((UINT *) outbuffer->info) = 0;
          DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Mensagem processada, resposta com %d bytes", outbuffer->infoDim);
        }
        else
        #endif
        {
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, get_SIRerror(rc) );
          SetIPCNACK(outbuffer, res);
        }
      }
      break;

    case CCMSG_ETH_IGMP_CHANNEL_WLIST_REMOVE:
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "CCMSG_ETH_IGMP_CHANNEL_WLIST_REMOVE");
      {
        msg_MCChannelWList_t *ptr;
        L7_uint16 number_of_cells;

        if (inbuffer->infoDim==0 || (inbuffer->infoDim%sizeof(msg_MCChannelWList_t))!=0) {
            res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE);
            SetIPCNACK(outbuffer, res);
            break;
        }
        number_of_cells = inbuffer->infoDim/sizeof(msg_MCChannelWList_t);

        ptr = (msg_MCChannelWList_t *) &inbuffer->info[0];

        #ifdef IGMP_WHITELIST_FEATURE
        if (L7_SUCCESS == (rc=ptin_msg_IGMP_WList_remove( ptr, number_of_cells )))
        {
          outbuffer->infoDim = sizeof(L7_uint32);
          *((UINT *) outbuffer->info) = 0;
          DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Mensagem processada, resposta com %d bytes", outbuffer->infoDim);
        }
        else
        #endif
        {
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, get_SIRerror(rc) );
          SetIPCNACK(outbuffer, res);
        }
      }
      break;

  case CCMSG_ETH_MC_STATIC_CHANNEL_ADD:
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "CCMSG_ETH_MC_STATIC_CHANNEL_ADD");
    {
      L7_MCStaticChannel *ptr;
      L7_uint16 number_of_cells;

      if (inbuffer->infoDim==0 || (inbuffer->infoDim%sizeof(L7_MCStaticChannel))!=0) {
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE);
          SetIPCNACK(outbuffer, res);
          break;
      }
      number_of_cells = inbuffer->infoDim/sizeof(L7_MCStaticChannel);

      ptr = (L7_MCStaticChannel *) &inbuffer->info[0];

      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "EVC Index        = %llu",ptr->index);
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "ChannelIP        = %lu",ptr->channelIp);
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "ports_bmp        = %lu",ptr->port_bmp);
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "portsfwd_bmp     = %lu",ptr->portfwd_bmp);

      if (L7_SUCCESS == (rc=ptin_msg_MCStaticChannel_add( ptr, number_of_cells ))) {
        outbuffer->infoDim = sizeof(L7_uint32);
        *((UINT *) outbuffer->info) = 0;
        DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Mensagem processada, resposta com %d bytes", outbuffer->infoDim);
      } else {
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, get_SIRerror(rc) );
        SetIPCNACK(outbuffer, res);
      }
    }
    break;

  case CCMSG_ETH_MC_STATIC_CHANNEL_DEL:
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "CCMSG_ETH_MC_STATIC_CHANNEL_DEL");
    {
      L7_MCStaticChannel *ptr;
      L7_uint16 number_of_cells;

      if (inbuffer->infoDim==0 || (inbuffer->infoDim%sizeof(L7_MCStaticChannel))!=0) {
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE);
          SetIPCNACK(outbuffer, res);
          break;
      }
      number_of_cells = inbuffer->infoDim/sizeof(L7_MCStaticChannel);

      ptr = (L7_MCStaticChannel *) &inbuffer->info[0];

      if (L7_SUCCESS == (rc=ptin_msg_MCStaticChannel_del( ptr, number_of_cells ))) {
        outbuffer->infoDim = sizeof(L7_uint32);
        *((UINT *) outbuffer->info) = 0;
        DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Mensagem processada, resposta com %d bytes", outbuffer->infoDim);
      } else {
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, get_SIRerror(rc) );
        SetIPCNACK(outbuffer, res);
      }
    }
    break;

  case CCMSG_ETH_MC_ACTIVE_CHANNELS_GET:
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "CCMSG_ETH_MC_ACTIVE_CHANNELS_GET message");
    {
      L7_MCActiveChannels *ptr;

      if (inbuffer->infoDim!=sizeof(L7_MCActiveChannels)) {
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE);
          SetIPCNACK(outbuffer, res);
          break;
      }

      memcpy(&outbuffer->info[0],&inbuffer->info[0],sizeof(L7_MCActiveChannels));
      outbuffer->infoDim = sizeof(L7_MCActiveChannels);

      ptr = (L7_MCActiveChannels *) &outbuffer->info[0];

      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "EVC Index        = %llu",ptr->index);
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Total channels   = %u",ptr->n_channels_total);
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Total pages      = %u",ptr->n_pages_total);
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Message channels = %u",ptr->n_channels_msg);
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Page index       = %u",ptr->page_index);

      if (L7_SUCCESS == (rc=ptin_msg_MCActiveChannelsList_get( ptr ))) {
        DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Mensagem processada, resposta com %d bytes", outbuffer->infoDim);
      } else {
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, get_SIRerror(rc) );
        SetIPCNACK(outbuffer, res);
      }
    }
    break;

  case CCMSG_ETH_MC_CHANNEL_CLIENTS_GET:
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "CCMSG_ETH_MC_CHANNEL_CLIENTS_GET message");
    {
      L7_MCActiveChannelClients *ptr;

      if (inbuffer->infoDim!=sizeof(L7_MCActiveChannelClients)) {
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE);
          SetIPCNACK(outbuffer, res);
          break;
      }

      memcpy(&outbuffer->info[0],&inbuffer->info[0],sizeof(L7_MCActiveChannelClients));
      outbuffer->infoDim = sizeof(L7_MCActiveChannelClients);

      ptr = (L7_MCActiveChannelClients *) &outbuffer->info[0];

      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "EVC Index        = %llu",ptr->index);
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Channel IP       = 0x%08X",ptr->channelIp);
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Total clients    = %u",ptr->n_clients);

      if (L7_SUCCESS == (rc=ptin_msg_MCChannelClientsList_get( ptr ))) {
        DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Mensagem processada, resposta com %d bytes", outbuffer->infoDim);
      } else {
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, get_SIRerror(rc) );
        SetIPCNACK(outbuffer, res);
      }
    }
    break;

  case CCMSG_ETH_PORT_COS_CONFIG_SET:
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "CCMSG_ETH_PORT_COS_CONFIG_SET message");
    {
      L7_QoSConfiguration *ptr;
      L7_uint8 number_of_cells;

      if (inbuffer->infoDim==0 || (inbuffer->infoDim%sizeof(L7_QoSConfiguration))!=0) {
          DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Wrong structure size");
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE);
          SetIPCNACK(outbuffer, res);
          break;
      }

      ptr = (L7_QoSConfiguration *) &inbuffer->info[0];
      number_of_cells = inbuffer->infoDim/sizeof(L7_QoSConfiguration);

      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Index          = %llu",ptr->index);
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "General Mask   = 0x%02X",ptr->mask);
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Trust mode     = %u",ptr->trust_mode);
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Bandwidth unit = %u",ptr->bandwidth_unit);
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Shaping rate   = %u",ptr->shaping_rate);
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "pktprio.cos    = [ %Xh %Xh %Xh %Xh %Xh %Xh %Xh %Xh ]",
                  ptr->pktprio.cos[0],ptr->pktprio.cos[1],ptr->pktprio.cos[2],ptr->pktprio.cos[3],ptr->pktprio.cos[4],ptr->pktprio.cos[5],ptr->pktprio.cos[6],ptr->pktprio.cos[7]);
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Priority Mask  = 0x%02X",ptr->pktprio.mask);
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "CoS Mask       = 0x%02X",ptr->cos_config.mask);
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "CoS Spec. Mask = [ 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X ]",
                  ptr->cos_config.cos[0].mask,ptr->cos_config.cos[1].mask,ptr->cos_config.cos[2].mask,ptr->cos_config.cos[3].mask,ptr->cos_config.cos[4].mask,ptr->cos_config.cos[5].mask,ptr->cos_config.cos[6].mask,ptr->cos_config.cos[7].mask);
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Queue_scheduler = [ %u %u %u %u %u %u %u %u ]",
                  ptr->cos_config.cos[0].scheduler, ptr->cos_config.cos[1].scheduler, ptr->cos_config.cos[2].scheduler, ptr->cos_config.cos[3].scheduler, ptr->cos_config.cos[4].scheduler, ptr->cos_config.cos[5].scheduler, ptr->cos_config.cos[6].scheduler, ptr->cos_config.cos[7].scheduler);
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Queue_min_bw    = [ %u %u %u %u %u %u %u %u ]",
                  ptr->cos_config.cos[0].min_bandwidth, ptr->cos_config.cos[1].min_bandwidth, ptr->cos_config.cos[2].min_bandwidth, ptr->cos_config.cos[3].min_bandwidth, ptr->cos_config.cos[4].min_bandwidth, ptr->cos_config.cos[5].min_bandwidth, ptr->cos_config.cos[6].min_bandwidth, ptr->cos_config.cos[7].min_bandwidth);
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Queue_max_bw    = [ %u %u %u %u %u %u %u %u ]",
                  ptr->cos_config.cos[0].max_bandwidth, ptr->cos_config.cos[1].max_bandwidth, ptr->cos_config.cos[2].max_bandwidth, ptr->cos_config.cos[3].max_bandwidth, ptr->cos_config.cos[4].max_bandwidth, ptr->cos_config.cos[5].max_bandwidth, ptr->cos_config.cos[6].max_bandwidth, ptr->cos_config.cos[7].max_bandwidth);
      
      if (L7_SUCCESS == (rc=ptin_msg_qos_config_set( ptr, number_of_cells ))) {
        DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s(%d) success\n", __FUNCTION__, __LINE__);
        outbuffer->infoDim = sizeof(L7_uint32);
        *((UINT *) outbuffer->info) = 0;
        DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Mensagem processada, resposta com %d bytes", outbuffer->infoDim);
      } else {
        DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) failure\n", __FUNCTION__, __LINE__);
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, get_SIRerror(rc) );
        SetIPCNACK(outbuffer, res);
      }
    }
    break;

  case CCMSG_ETH_PORT_COS_CONFIG_GET:
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "CCMSG_ETH_PORT_COS_CONFIG_GET message");
    {
      L7_QoSConfiguration *ptr;
      L7_uint8 number_of_cells;

      if (inbuffer->infoDim!=sizeof(L7_QoSConfiguration)) {
          DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Wrong structure size");
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE);
          SetIPCNACK(outbuffer, res);
          break;
      }

      memcpy(&outbuffer->info[0],&inbuffer->info[0],sizeof(L7_QoSConfiguration));
      ptr = (L7_QoSConfiguration *) &outbuffer->info[0];

      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s(%d): Port=0x%llu\n", __FUNCTION__, __LINE__,ptr->index);

      if (L7_SUCCESS == (rc=ptin_msg_qos_config_get( ptr, &number_of_cells )) && number_of_cells>0) {
        outbuffer->infoDim = sizeof(L7_QoSConfiguration)*number_of_cells;
        DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Mensagem processada, resposta com %d bytes", outbuffer->infoDim);
      } else {
        DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Returned Error");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, get_SIRerror(rc) );
        SetIPCNACK(outbuffer, res);
      }
    }
    break;

  case CCMSG_ETH_MAC_DYNAMIC_TABLE:
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "CCMSG_ETH_MAC_DYNAMIC_TABLE message");
    {
      L7_switch_mac_table *ptr;

      if (inbuffer->infoDim<(sizeof(L7_int)+sizeof(L7_uint64))) {
          DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Wrong structure size");
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE);
          SetIPCNACK(outbuffer, res);
          break;
      }

      memcpy(&outbuffer->info[0],&inbuffer->info[0],sizeof(L7_switch_mac_table));
      ptr = (L7_switch_mac_table *) &outbuffer->info[0];

      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s(%d): Page Index=0x%llu\n", __FUNCTION__, __LINE__,ptr->index);

      if ( L7_SUCCESS == (rc=ptin_msg_mac_table_get( ptr, L7_FALSE )) ) {
        outbuffer->infoDim = sizeof(L7_switch_mac_table);
        DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Mensagem processada, resposta com %d bytes", outbuffer->infoDim);
      } else {
        DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Returned Error");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, get_SIRerror(rc) );
        SetIPCNACK(outbuffer, res);
      }
    }
    break;

  case CCMSG_ETH_MAC_STATIC_TABLE:
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "CCMSG_ETH_MAC_STATIC_TABLE message");
    {
      L7_switch_mac_table *ptr;

      if (inbuffer->infoDim<(sizeof(L7_int)+sizeof(L7_uint64))) {
          DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Wrong structure size");
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE);
          SetIPCNACK(outbuffer, res);
          break;
      }

      memcpy(&outbuffer->info[0],&inbuffer->info[0],sizeof(L7_switch_mac_table));
      ptr = (L7_switch_mac_table *) &outbuffer->info[0];

      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s(%d): Page Index=0x%llu\n", __FUNCTION__, __LINE__,ptr->index);

      if ( L7_SUCCESS == (rc=ptin_msg_mac_table_get( ptr, L7_TRUE )) ) {
        outbuffer->infoDim = sizeof(L7_switch_mac_table);
        DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Mensagem processada, resposta com %d bytes", outbuffer->infoDim);
      } else {
        DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Returned Error");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, get_SIRerror(rc) );
        SetIPCNACK(outbuffer, res);
      }
    }
    break;

  case CCMSG_ETH_MAC_ENTRY_REMOVE:
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "CCMSG_ETH_MAC_DYNAMIC_TABLE message");
    {
      L7_switch_mac_table *ptr;

      if (inbuffer->infoDim!=sizeof(L7_switch_mac_table)) {
          DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Wrong structure size");
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE);
          SetIPCNACK(outbuffer, res);
          break;
      }

      ptr = (L7_switch_mac_table *) &inbuffer->info[0];

      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s(%d): Page Index=0x%llu", __FUNCTION__, __LINE__,ptr->index);
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s(%d): Msg_size  =0x%u", __FUNCTION__, __LINE__,ptr->mac_table_msg_size);

      if ( L7_SUCCESS == (rc=ptin_msg_mac_table_entry_remove( ptr )) ) {
        outbuffer->infoDim = sizeof(L7_uint32);
        *((UINT *) outbuffer->info) = 0;
        DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Mensagem processada, resposta com %d bytes", outbuffer->infoDim);
      } else {
        DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Returned Error");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, get_SIRerror(rc) );
        SetIPCNACK(outbuffer, res);
      }
    }
    break;

	case CCMSG_ETH_MAC_ENTRY_ADD:
		DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "CCMSG_ETH_MAC_ENTRY_ADD message");
		{
      L7_switch_mac_operation *ptr;

			if (inbuffer->infoDim!=sizeof(L7_switch_mac_operation)) {
        DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Wrong structure size");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE);
        SetIPCNACK(outbuffer, res);
        break;
			}

			ptr = (L7_switch_mac_operation *) &inbuffer->info[0];

			if ( L7_SUCCESS == (rc=ptin_msg_mac_table_entry_add( ptr )) ) {
				outbuffer->infoDim = sizeof(L7_uint32);
				*((UINT *) outbuffer->info) = 0;
				DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Mensagem processada, resposta com %d bytes", outbuffer->infoDim);
			} else {
				DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Returned Error");
				res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, get_SIRerror(rc) );
				SetIPCNACK(outbuffer, res);
			}
		}
		break;

  case CCMSG_ETH_DHCP_BIND_TABLE_SHOW:
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "CCMSG_ETH_DHCP_BIND_TABLE_SHOW message");
    {
      L7_DHCP_bind_table *ptr;

      if (inbuffer->infoDim<(sizeof(L7_int)+sizeof(L7_uint64))) {
          DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Wrong structure size");
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE);
          SetIPCNACK(outbuffer, res);
          break;
      }

      memcpy(&outbuffer->info[0],&inbuffer->info[0],sizeof(L7_DHCP_bind_table));
      ptr = (L7_DHCP_bind_table *) &outbuffer->info[0];

      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s(%d): Page Index=0x%llu\n", __FUNCTION__, __LINE__,ptr->index);

      if ( L7_SUCCESS == (rc=ptin_msg_dhcp_bindtable_get( ptr )) ) {
        outbuffer->infoDim = sizeof(L7_DHCP_bind_table);
        DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Mensagem processada, resposta com %d bytes", outbuffer->infoDim);
      } else {
        DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Returned Error");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, get_SIRerror(rc) );
        SetIPCNACK(outbuffer, res);
      }
    }
    break;

  case CCMSG_ETH_DHCP_BIND_TABLE_ADD:
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "CCMSG_ETH_DHCP_BIND_TABLE_ADD message");
    {
      L7_DHCP_bind_table *ptr;

      if (inbuffer->infoDim<sizeof(L7_DHCP_bind_table)) {
          DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Wrong structure size");
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE);
          SetIPCNACK(outbuffer, res);
          break;
      }

      ptr = (L7_DHCP_bind_table *) &inbuffer->info[0];

      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s(%d): Page Index=0x%llu\n", __FUNCTION__, __LINE__,ptr->index);
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s(%d): Total ent =0x%u\n", __FUNCTION__, __LINE__,ptr->bind_table_total_entries);
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s(%d): Msg size  =0x%u\n", __FUNCTION__, __LINE__,ptr->bind_table_msg_size);

      //if ( L7_SUCCESS == (rc=ptin_msg_dhcp_bindtable_get( ptr )) ) {
        outbuffer->infoDim = sizeof(L7_DHCP_bind_table);
        DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Mensagem processada, resposta com %d bytes", outbuffer->infoDim);
//    } else {
//      DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Returned Error");
//      res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, get_SIRerror(rc) );
//      SetIPCNACK(outbuffer, res);
//    }
    }
    break;

  case CCMSG_ETH_DHCP_BIND_TABLE_REMOVE:
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "CCMSG_ETH_DHCP_BIND_TABLE_ADD message");
    {
      L7_DHCP_bind_table *ptr;

      if (inbuffer->infoDim<sizeof(L7_DHCP_bind_table)) {
          DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Wrong structure size");
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE);
          SetIPCNACK(outbuffer, res);
          break;
      }

      ptr = (L7_DHCP_bind_table *) &inbuffer->info[0];

      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s(%d): Page Index=0x%llu\n", __FUNCTION__, __LINE__,ptr->index);
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s(%d): Total ent =0x%u\n", __FUNCTION__, __LINE__,ptr->bind_table_total_entries);
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s(%d): Msg size  =0x%u\n", __FUNCTION__, __LINE__,ptr->bind_table_msg_size);

      if ( L7_SUCCESS == (rc=ptin_msg_dhcp_bindtable_remove( ptr )) ) {
        outbuffer->infoDim = sizeof(L7_DHCP_bind_table);
        DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Mensagem processada, resposta com %d bytes", outbuffer->infoDim);
      } else {
        DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Returned Error");
        res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, get_SIRerror(rc) );
        SetIPCNACK(outbuffer, res);
      }
    }
    break;

  case CCMSG_ETH_CHANGE_STDOUT:

    printf("Redirecting stdout...\n");
    {
      if (inbuffer->infoDim==0) {
          res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, ERROR_CODE_WRONGSIZE);
          SetIPCNACK(outbuffer, res);
          break;
      }

      // Change 
      ptin_PitHandler((char *) &inbuffer->info[0]);

      outbuffer->infoDim = sizeof(L7_uint32);
      *((UINT *) outbuffer->info) = 0;
      printf("...Stdout redirected to here\n");
    }
    break;

  case CCMSG_ETH_SWITCH_SET:
  {
    L7_FDB_TYPE_t fdb_type = L7_SVL;
    L7_switch_config *swcfg = (L7_switch_config *)inbuffer->info;

    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL,  "Recebeu a mensagem CCMSG_ETH_SWITCH_SET.");

    // Aging time set?
    if (swcfg->mask & 0x01) {

      usmDbFDBTypeOfVLGet(1, &fdb_type);

      if (fdb_type == L7_SVL)
      {
        rc = usmDbDot1dTpAgingTimeSet(1, swcfg->aging_time);   /* the 'unit' parameter is ignored */
      }
      else
      {
        rc = usmDbFDBAddressAgingTimeoutSet(1, FD_SIM_DEFAULT_MGMT_VLAN_ID, swcfg->aging_time);
      }

      if (rc != L7_SUCCESS) {
        DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Returned Error (error setting aging time)");
				res = SIR_ERROR(ERROR_FAMILY_HARDWARE, ERROR_SEVERITY_ERROR, get_SIRerror(rc) );
        SetIPCNACK(outbuffer, res);
        break;  /* early exit due to error! */
      }
    }

    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL,  "Mensagem processada. MAC Aging Time = %d sec.", swcfg->aging_time);
    break; // CCMSG_ETH_SWITCH_SET
  }

  default:
    DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL,  "Recebeu a mensagem %08X (user=%02X).", inbuffer->msgId, 0);
    SetIPCNACK (outbuffer, SIR_ERROR(ERROR_FAMILY_IPC, ERROR_SEVERITY_WARNING, ERROR_CODE_NOSUCHMSG));
    DEBUGTRACE (TRACE_MODULE_ALL | TRACE_LAYER_CAPI, TRACE_SEVERITY_WARNING,  "A mensagem recebida nao e' suportada pelo modulo de controlo.");
    break;
  }

  return S_OK;
}
