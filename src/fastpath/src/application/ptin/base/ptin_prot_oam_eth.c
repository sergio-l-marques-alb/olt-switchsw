/**
 * ptin_prot_oam_eth.c 
 *  
 * Adapting ptin_oam.[ch] to Fastpath
 *
 * @author
 *  
 */

#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include <logger.h>
#include <ptin_cnfgr.h>
#include <ptin_intf.h>
#include <ethsrv_oam.h>

/* *******************************************************************************/
/*                                  GLOBAL VARS                                  */
/* *******************************************************************************/

static T_ETH_SRV_OAM oam;

/* *******************************************************************************/
/*                                   FUNCTIONS                                   */
/* *******************************************************************************/
void ethsrv_oam_register_mismerge(T_MEG_ID *meg_id, L7_uint16 mep_id, L7_uint16 mep_indx, L7_uint16 porta, L7_uint64 vid) {}
void ethsrv_oam_register_LVL(T_MEG_ID *meg_id, L7_uint16 mep_id, L7_uint16 mep_indx, L7_uint16 porta, L7_uint64 vid, L7_uint8 level) {}
void ethsrv_oam_register_T(T_MEG_ID *meg_id, L7_uint16 mep_id, L7_uint16 mep_indx, L7_uint16 porta, L7_uint64 vid, L7_uint8 period) {}
void ethsrv_oam_register_unexpected_MEP_potential_loop(T_MEG_ID *meg_id, L7_uint16 mep_id, L7_uint16 mep_indx, L7_uint16 porta, L7_uint64 vid) {}
void ethsrv_oam_register_unexpected_MEP_id(T_MEG_ID *meg_id, L7_uint16 mep_id, L7_uint16 mep_indx, L7_uint16 porta, L7_uint64 vid) {}
void ethsrv_oam_register_connection_restored(L7_uint8 *meg_id, L7_uint16 mep_id, L7_uint16 rmep_id, L7_uint16 port, L7_uint64 vid) {}
void ethsrv_oam_register_receiving_RDI(L7_uint8 *meg_id, L7_uint16 mep_id, L7_uint16 rmep_id, L7_uint16 port, L7_uint64 vid) {}
void ethsrv_oam_register_connection_loss(L7_uint8 *meg_id, L7_uint16 mep_id, L7_uint16 rmep_id, L7_uint16 port, L7_uint64 vid) {}




int send_eth_pckt(L7_uint16 port, L7_uint8 up1_down0,
                  L7_uint8 *buf, L7_ulong32 length, //ETH client buffer and length
                  L7_uint64 vid, L7_uint8 prior, L7_uint8 CoS, L7_uint8 color, L7_uint16 ETHtype, L7_uint8 *pDMAC) {
    L7_netBufHandle   bufHandle;
    L7_uchar8        *dataStart;
    L7_INTF_TYPES_t   sysIntfType;
    L7_uint32         intIfNum;//=port+1;
    DTL_CMD_TX_INFO_t dtlCmd;
    L7_uint8 buff[1600];//={1,0,0,0,2,3,   0,1,2,3,4,5,    0x88,9,   3,  16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,};
    //length=70;



    ptin_intf_port2intIfNum(port, &intIfNum);

    // If outgoing interface is CPU interface, don't send it
    if ( (nimGetIntfType(intIfNum, &sysIntfType) == L7_SUCCESS) &&
         (sysIntfType == L7_CPU_INTF) )     return 1;

    SYSAPI_NET_MBUF_GET(bufHandle);
    if (bufHandle == L7_NULL) {
      LOG_TRACE(LOG_CTX_OAM,"send_eth_pckt: System out of netbuffs");
      return 2;
    }

    if (NULL==pDMAC || NULL==buf) return 3;

    memcpy(buff, pDMAC, 6);
    //memcpy(buff, pSMAC, 6);
    if (vid<=4095) {
        buff[12]=0x81;
        buff[13]=0x00;
        buff[14]=prior<<5 | vid>>8;
        buff[15]=vid;
        buff[16]=ETHtype>>8;
        buff[17]=ETHtype;
        memcpy(&buff[18], buf, length);   
        if (length<64-18) {
              memset(&buff[length+18], 0, 64-18-length);
              length=64;
        }
    }
    else {
        buff[12]=ETHtype>>8;
        buff[13]=ETHtype;
        memcpy(&buff[14], buf, length);
        if (length<64-14) {
            memset(&buff[length+14], 0, 64-14-length);
            length=64;            
        }
    }

    SYSAPI_NET_MBUF_GET_DATASTART(bufHandle, dataStart);
    memcpy(dataStart, buff, length);
    SYSAPI_NET_MBUF_SET_DATALENGTH(bufHandle, length);

    {
        //DTL_CMD_TX_INFO_t  dtlCmd;

        memset((L7_uchar8 *)&dtlCmd, 0, sizeof(DTL_CMD_TX_INFO_t));

        dtlCmd.priority            = 1;
        dtlCmd.typeToSend          = DTL_L2RAW_UNICAST;
        dtlCmd.intfNum             = intIfNum;
        dtlPduTransmit (bufHandle, DTL_CMD_TX_L2, &dtlCmd);
    }
    printf("send_eth_pckt() port=%u\n\r", port);
    return 0;
}//send_eth_pckt








void tst_send(void) {
    {
        L7_uint8 buf[100]={1,0,0,0,2,3,   0,1,2,3,4,5,    0x88,9,   3,  
                           16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,};
        //length=70;
        send_eth_pckt(8, 0, &buf[15], 70, 
                      -1,
                      0,
                      0,
                      0,
                      0x8902,
                      (L7_uint8*)OAM_MC_MAC.byte);
    }
}









/****************************************************************************** 
 * Task Init
 ******************************************************************************/

L7_uint32 oam_eth_TaskId = L7_ERROR;



/**
 * Task with infinite loop
 */
void ptin_oam_eth_task(void)
{
  LOG_TRACE(LOG_CTX_OAM,"OAM ETH Task started");

  if (osapiTaskInitDone(L7_PTIN_OAM_ETH_TASK_SYNC)!=L7_SUCCESS) {
    LOG_FATAL(LOG_CTX_OAM, "Error syncing task");
    PTIN_CRASH();
  }

  LOG_TRACE(LOG_CTX_OAM,"OAM ETH task ready");


  /*
   * TEST
  */
  init_eth_srv_oam(&oam);

  /* Infinite Loop */
  while (1) {
      proc_ethsrv_oam(&oam, 10);

      sleep(10000);
  }
}




/**
 * Initializes OAM ETH module
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_oam_eth_init(void)
{
  oam_eth_TaskId = osapiTaskCreate("ptin_oam_eth_task", ptin_oam_eth_task, 0, 0,
                                L7_DEFAULT_STACK_SIZE,
                                L7_TASK_PRIORITY_LEVEL(L7_DEFAULT_TASK_PRIORITY),
                                L7_DEFAULT_TASK_SLICE);

  if (oam_eth_TaskId == L7_ERROR) {
    LOG_FATAL(LOG_CTX_PTIN_CNFGR, "Could not create task ptin_oam_eth_task");
    return L7_FAILURE;
  }
  LOG_TRACE(LOG_CTX_PTIN_CNFGR,"Task ptin_oam_eth_task created");

  if (osapiWaitForTaskInit (L7_PTIN_OAM_ETH_TASK_SYNC, L7_WAIT_FOREVER) != L7_SUCCESS) {
    LOG_FATAL(LOG_CTX_PTIN_CNFGR,"Unable to initialize ptin_oam_eth_task()\n");
    return(L7_FAILURE);
  }
  LOG_TRACE(LOG_CTX_PTIN_CNFGR,"Task ptin_oam_eth_task initialized");

  return L7_SUCCESS;
}

