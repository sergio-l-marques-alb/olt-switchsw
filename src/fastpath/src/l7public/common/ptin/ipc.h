/* ======================================================================
Projecto:   EmiloX16
Modulo:     Controlo
Copyright:  SIR1@PT Inovacao
Descricao:  Interface de utilizacao dos canais de comunicacao entre
			   processos.

Autor:      Vitor Mirones (VM)

Historico:  VM 2006.06.22 - Criacao do modulo V1.0.0.0

======================================================================= */
#ifndef EMILOX16_CONTROLO_IPC_H
#define EMILOX16_CONTROLO_IPC_H

#include "datatypes.h"
#include "globaldefs.h"
#include "ptin_globaldefs.h"
#include "ipc_lib.h"

#define TRAP_ARRANQUE       0x3001
#define TRAP_ALARME_INTF    0x3004
#define TRAP_ALARME_SWITCH  0x3007
#define TRAP_LINECARD_INTF  0x300f

#define TRAP_ALARME_ETH_OAM 0x5024
#define CHTRAP_ETHERNET_CFM_UNEXPECTEDMEP   TRAP_ALARME_ETH_OAM

#define TRAP_ALARM_LINK_DOWN_END                0x9001
#define TRAP_ALARM_LINK_DOWN_START              0x9002
#define TRAP_ALARM_LAG_INACTIVE_MEMBER_END      0x9003
#define TRAP_ALARM_LAG_INACTIVE_MEMBER_START    0x9004
//#define TRAP_ALARM_PORT_ENABLED                 0x9005
//#define TRAP_ALARM_PORT_DISABLED                0x9006
//#define TRAP_ALARM_MAC_DUPLICATED               0x9010
#define TRAP_ALARM_MAC_LIMIT                    0x9011
#define TRAP_LINECARD_TRAFFIC_RX                0x900F

#define TRAP_LC_TYPE_INTERFACE    4   // For alarmtype
#define TRAP_ALARM_TYPE_INTERFACE 3   // For alarmtype
#define TRAP_ALARM_STATUS_END     0   // For alarmstatus
#define TRAP_ALARM_STATUS_START   1   // For alarmstatus
#define TRAP_ALARM_STATUS_EVENT   2   // For alarmstatus

//#define IPC_LOCALHOST_IPADDR  0x7F000001  // Local Host
//#define IPC_SERVER_IPADDR     IPC_LOCALHOST_IPADDR

#define IPC_CHMSG_TRAP_PORT   5001
#define IPC_TRAP_LC_PORT      6102

   #define IPC_MAINTENANCE_PORT  5200
   #define IPC_AGENT_PORT        5100
   #define IPC_CLI_PORT          5101
   #define IPC_WEBTI_PORT        5102

   #define IPC_CCMSG_PORT        5000
   #define IPC_CHMSG_PORT        5001

   #define IPC_HARDWARE_PORT     6000
   #define IPC_HW_OSI_PORT       6030
   #define IPC_HW_FASTPATH_PORT  6006

   #define IPC_CC_TIMEOUT        30
   #define IPC_CH_TIMEOUT        20
   #define IPC_CM_TIMEOUT        10

   #define MAX_COUNTERVALUE      65530   

   typedef struct _Index
   {
      UINT     indexA;
      UINT     indexB;
      UINT     indexC;
   } st_Index;

/*****************************************************************************
 * Global variables
 *****************************************************************************/

/* Slot id */
extern uint8 ptin_board_slotId;

// ----------------------------------------------------
// Dependencias externas
// ----------------------------------------------------

   EXTERN_C int CCMessageHandler (ipc_msg *inbuffer, ipc_msg *outbuffer);

// ----------------------------------------------------
// Interface Publica
// ----------------------------------------------------

   EXTERN_C int  OpenIPC	(void);
   EXTERN_C void CloseIPC	(void);

   EXTERN_C int send_trap(int porto, int trap_type, int arg);
   EXTERN_C int send_trap_intf_alarm(unsigned char intfType, int porto, int code, int status, int param);
   EXTERN_C int send_trap_gen_alarm(unsigned char intfType, int porto, int code, int status, int param1, int param2);
   EXTERN_C int send_trap_switch_event(unsigned char intfType, int interface, int code, int status, int param);
   EXTERN_C int send_trap_to_linecard(unsigned char intfType, int porto, int code, int status, int param);
   EXTERN_C int send_trap_ETH_OAM(void *param, int param_size);
   EXTERN_C int send_ipc_message(int porto, uint32 ipaddr, int msg_id, char *request, char *answer, uint32 infoDim);

   EXTERN_C void EnableHandling        (BOOLEAN enable);
   EXTERN_C void SwapIPCHeader         (ipc_msg *inbuffer, ipc_msg *outbuffer);
   EXTERN_C void SetIPCNACK            (ipc_msg *outbuffer, int res);


   EXTERN_C int SendSetMessage         (UINT msg, void *data, int* num);
   EXTERN_C int SendGetStructMessage   (UINT msg, void *data, int  num);
   EXTERN_C int SendNotifyMessage      (UINT msg, void *data, int* num);
   EXTERN_C int SendHWGetMessage       (int index, UINT msg, void *data, size_t* num);
   EXTERN_C int SendHWGetStructMessage    (UINT msg, void *data, int num);
   EXTERN_C int SendHWGetStructMessageEx  (UINT msg, void *data, int *num);
   EXTERN_C int SendHwPortGetMessage      (UINT dest, UINT index, UINT msg, void *data, int* num);
   EXTERN_C int SendHWNotifyMessage    (UINT msg, void *data, int* num);
   //#define SendHWSetMessage(a,b,c)     SendHwSetMessage((a),(b),(size_t*)(c))
   EXTERN_C int SendHWSetMessage       (UINT msg, void *data, size_t *num);
   EXTERN_C int SendHWSetExMessage     (UINT msg, void *input, int num, int *output, int* outnum);

   //verificar se ainda e usado
   EXTERN_C int SendPortGetStructMessage (UINT dest, UINT msg, void *data, int num, int uid);
   EXTERN_C int SendHwPortNotifyMessage  (UINT dest, UINT msg, void *data, int* num);

   EXTERN_C HRES  SendHwStructMessage                          (void *input, size_t inSize, UINT msg, void *output, size_t *outSize);

// ----------------------------------------------------
// Interface Privada
// ----------------------------------------------------

   EXTERN_C int  GetMsgCounter     (void);

   // Set a new server ipaddr
   EXTERN_C void ipc_server_ipaddr_set(uint32 ipaddr);
#endif // EMILOX16_CONTROLO_IPC_H

