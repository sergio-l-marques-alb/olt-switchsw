#include <stdio.h>
#include <unistd.h>
#include "pclib.h"
#include "memchk.h"
//#include "dbgprintf.h"

#define dbgtrue(val) 0
//*****************************************************************************

void pc_debug(pc_bymq *pc_id){

  printf("pc_debug:\n");
  printf("  mqTxId      = %d\n", pc_id->mqTxId);
  printf("  mqRxId      = %d\n", pc_id->mqRxId);
  printf("  rxHandlerId = %d\n", pc_id->rxHandlerId);
}

//*****************************************************************************
int pc_rx_handler(void *pc_id){
  int     dimdata, counter;
  mq_type mqbuf;

  while ( msgrcv(((pc_bymq *)pc_id)->mqRxId, &mqbuf, sizeof(msq_pc_type), 1, IPC_NOWAIT)>=0 );

  while (-1){
    do {
      dimdata = msgrcv(((pc_bymq *)pc_id)->mqRxId, &mqbuf, sizeof(msq_pc_type), 1, 0);
    } while ( (dimdata == -1) && (errno == EINTR) );

    if (dimdata > 0){
      if (dbgtrue(DEBUG_MSG)) {
        printf("[+]RX-MSG\n\r");
        printf("protocolID = %08xh\n\r",mqbuf.msg.protocolId);
        printf("srcId      = %08xh\n\r",mqbuf.msg.srcId);
        printf("dstId      = %08xh\n\r",mqbuf.msg.dstId);
        printf("flags      = %08xh\n\r",mqbuf.msg.flags);
        printf("counter    = %08xh\n\r",mqbuf.msg.counter);
        printf("msgId      = %08xh\n\r",mqbuf.msg.msgId);
        printf("infoDim    = %08xh",mqbuf.msg.infoDim);
        for (counter=0; counter < mqbuf.msg.infoDim; counter++) {
          if ((counter % 16)==0) printf("\n\r  info[%4d] : ", counter);
          printf("%02x ",mqbuf.msg.info[counter]);
        }
        printf("\n\r[-]RX-MSG\n\r");
      }

      if (mqbuf.msg.flags & 0x00000001) {
        memcpy(&(((pc_bymq *)pc_id)->msg), (void *)&mqbuf.msg, sizeof(msq_pc_type));
      }
      else {
        (((pc_bymq *)pc_id)->rxProcId)((pc_bymq *)pc_id, &mqbuf.msg);
      }
    }
    else {
      sleep(1);
    }
  }
  sleep(2);
  return(0);
}

//*****************************************************************************

int pc_open(pc_bymq *pc_id, key_t keyTx, key_t keyRx, void (*rxProcId)(pc_bymq *pc_id, msq_pc_type *msg)){

  memfill(pc_id->rxHandlerStk, RX_HANDLER_STK, "STACK_");

  pc_id->rxProcId = rxProcId;
  pc_id->mqTxId = msgget(keyTx, IPC_CREAT | 0666 );
  pc_id->mqRxId = msgget(keyRx, IPC_CREAT | 0666 );
  #ifdef PC_DEBUG
  printf("pc_open:\n");
  printf("  mqTxId = %d\n", pc_id->mqTxId);
  printf("  mqRxId = %d\n", pc_id->mqRxId);
  #endif
  if ((pc_id->mqRxId < 0) || (pc_id->mqTxId < 0)){
    if (pc_id->mqRxId >= 0) msgctl(pc_id->mqRxId, IPC_RMID, 0);
    if (pc_id->mqTxId >= 0) msgctl(pc_id->mqTxId, IPC_RMID, 0);
    if (pc_id->rxHandlerId > 0) kill(pc_id->rxHandlerId, SIGKILL);
    pc_id->mqTxId = -1;
    pc_id->mqRxId = -1;
    pc_id->rxHandlerId = -1;
    #ifdef PC_DEBUG
    printf("  Return FAIL\n");
    #endif
    return -1;
  }
  else {
    if (pc_id->rxHandlerId < 0)
      pc_id->rxHandlerId = clone(pc_rx_handler, (void *)&(pc_id->rxHandlerStk[RX_HANDLER_STK-1]), (CLONE_VM | CLONE_SIGHAND | CLONE_FS | CLONE_FILES), (void *)pc_id);
    #ifdef PC_DEBUG
    printf("  rxHandlerId = %d\n", pc_id->rxHandlerId);
    printf("  Return OK\n");
    #endif
    return 0;
  }
}

//*****************************************************************************

int pc_close(pc_bymq *pc_id){

  //if (pc_id->mqRxId >= 0) msgctl(pc_id->mqRxId, IPC_RMID, 0);
  //if (pc_id->mqTxId >= 0) msgctl(pc_id->mqTxId, IPC_RMID, 0);
  if (pc_id->rxHandlerId > 0) kill(pc_id->rxHandlerId, SIGKILL);
  pc_id->mqTxId = -1;
  pc_id->mqRxId = -1;
  pc_id->rxHandlerId = -1;
  #ifdef PC_DEBUG
  printf("pc_close:\n");
  printf("  Return OK\n");
  printf("Stack: used %d of %d\n\r", RX_HANDLER_STK-memchk(pc_id->rxHandlerStk, RX_HANDLER_STK, "STACK_"), RX_HANDLER_STK);
  #endif
  return 0;
}

//*****************************************************************************

int pc_tx(pc_bymq *pc_id, msq_pc_type *msg){
  int value;
  mq_type mqbuf;


  if (dbgtrue(DEBUG_MSG)) {
    printf("[+]TX-MSG\n\r");
    printf("protocolID = %08xh\n\r",msg->protocolId);
    printf("srcId      = %08xh\n\r",msg->srcId);
    printf("dstId      = %08xh\n\r",msg->dstId);
    printf("flags      = %08xh\n\r",msg->flags);
    printf("counter    = %08xh\n\r",msg->counter);
    printf("msgId      = %08xh\n\r",msg->msgId);
    printf("infoDim    = %08xh",msg->infoDim);
    for (value=0; value < msg->infoDim; value++) {
      if ((value % 16)==0) printf("\n\r  info[%4d] : ", value);
      printf("%02x ",msg->info[value]);
    }
    printf("\n\r[-]TX-MSG\n\r");
  }
  
  #ifdef PC_DEBUG
  printf("pc_tx:\n");
  #endif
  if (pc_id->mqTxId >= 0){
    mqbuf.mtype = 1;
    memcpy(&mqbuf.msg, msg, sizeof(msq_pc_type));
    value = msgsnd(pc_id->mqTxId, &mqbuf, sizeof(msq_pc_type), IPC_NOWAIT);
    #ifdef PC_DEBUG
    printf("  Return %s\n", (value<0)? "FAIL": "OK");
    #endif
    return value;
  }
  #ifdef PC_DEBUG
  printf("  Return FAIL\n");
  #endif
  return -1;
}

//*****************************************************************************

int pc_txwaitrx(pc_bymq *pc_id, int timeOut, msq_pc_type *msgtx, msq_pc_type *msgrx){
  int value;

  #ifdef PC_DEBUG
  printf("pc_txwaitrx:\n");
  #endif
  pc_id->msg.flags = 0x00000000;
  value = pc_tx(pc_id,msgtx);
  if (value >= 0) {
    value = -2;
    while (timeOut>0){
      if (pc_id->msg.flags & 0x00000001) {
        memcpy(msgrx, &(pc_id->msg), sizeof(msq_pc_type));
        timeOut = 0;
        value = 0;
      }
      else {
        timeOut--;
        usleep(100);
      }
    }
  }
  #ifdef PC_DEBUG
  switch (value){
    case -2:
      printf("  Return TIME-OUT\n");
      break;
    case 0:
      printf("  Return OK\n");
      break;
    default:
      printf("  Return FAIL\n");
      break;
  }
  #endif
  return value;
}

//*****************************************************************************

int pc_flushtx(pc_bymq *pc_id){
  int value;
  mq_type mqbuf;

  #ifdef PC_DEBUG
  printf("pc_flushtx\n");
  #endif
  if (pc_id->mqTxId >= 0){
    while ( (value = msgrcv(pc_id->mqTxId, &mqbuf, sizeof(msq_pc_type), 1, IPC_NOWAIT))>=0 );
    #ifdef PC_DEBUG
    printf("  Return OK\n");
    #endif
    return 0;
  }
  #ifdef PC_DEBUG
  printf("  Return FAIL\n");
  #endif
  return -1;
}

//*****************************************************************************

int pc_flushrx(pc_bymq *pc_id){
  int value;
  mq_type mqbuf;

  #ifdef PC_DEBUG
  printf("pc_flushrx\n");
  #endif
  if (pc_id->mqRxId >= 0){
    while ( (value = msgrcv(pc_id->mqRxId, &mqbuf, sizeof(msq_pc_type), 1, IPC_NOWAIT))>=0 );
    #ifdef PC_DEBUG
    printf("  Return OK\n");
    #endif
    return 0;
  }
  #ifdef PC_DEBUG
  printf("  Return FAIL\n");
  #endif
  return -1;
}

//*****************************************************************************

