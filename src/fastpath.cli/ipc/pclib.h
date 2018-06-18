#ifndef __PCLIB_H__
#define __PCLIB_H__

#include <stdio.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sched.h>
#include <signal.h>
#include <errno.h>


//*****************************************************************************

#define msq_PC_MsgDimMax     1024

#define RX_HANDLER_STK   32*1024

//#define PC_DEBUG         //Comment to hide debug

//*****************************************************************************

typedef struct {               //Definido no ipc_lib.h
   unsigned int   protocolId;
   unsigned int   srcId;
   unsigned int   dstId;
   unsigned int   flags;
   unsigned int   counter;
   unsigned int   msgId;
   unsigned int   infoDim;
   unsigned char  info[msq_PC_MsgDimMax];
 } msq_pc_type;

typedef struct bymq_s{
  int     mqTxId;
  int     mqRxId;
  pid_t   rxHandlerId;
  msq_pc_type msg;
  char    rxHandlerStk[RX_HANDLER_STK];
  void    (*rxProcId)(struct bymq_s *pc_id, msq_pc_type *msg);
} pc_bymq;

typedef struct {
  long    mtype;
  msq_pc_type msg;
} mq_type;
//*****************************************************************************

#ifdef __cplusplus
extern "C"
#else
extern
#endif
void pc_debug(pc_bymq *pc_id);
//*****************************************************************************

#ifdef __cplusplus
extern "C"
#else
extern
#endif
int pc_rx_handler(void *pc_id);
//static?
//*****************************************************************************

#ifdef __cplusplus
extern "C"
#else
extern
#endif
int pc_open(pc_bymq *pc_id, key_t keyTx, key_t keyRx, void (*rxProcId)(pc_bymq *pc_id, msq_pc_type *msg));
//*****************************************************************************

#ifdef __cplusplus
extern "C"
#else
extern
#endif
int pc_close(pc_bymq *pc_id);
//*****************************************************************************

#ifdef __cplusplus
extern "C"
#else
extern
#endif
int pc_tx(pc_bymq *pc_id, msq_pc_type *msg);
//*****************************************************************************

#ifdef __cplusplus
extern "C"
#else
extern
#endif
int pc_txwaitrx(pc_bymq *pc_id, int timeOut, msq_pc_type *msgtx, msq_pc_type *msgrx);
//*****************************************************************************

#ifdef __cplusplus
extern "C"
#else
extern
#endif
int pc_flushtx(pc_bymq *pc_id);
//*****************************************************************************

#ifdef __cplusplus
extern "C"
#else
extern
#endif
int pc_flushrx(pc_bymq *pc_id);
//*****************************************************************************

#endif

