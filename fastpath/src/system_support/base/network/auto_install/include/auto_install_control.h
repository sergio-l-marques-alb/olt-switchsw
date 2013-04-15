/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2007
*
**********************************************************************
* @filename  auto_istall_control.h
*
* @purpose   auto_install control header. Contain common component's
*            definitions
*
* @component auto_install
*
* @comments
*
* @create    28/12/2007
*
* @author    lkhedyk
*
* @end
*
**********************************************************************/
#ifndef AUTO_INSTALL_CONTROL_H
#define AUTO_INSTALL_CONTROL_H

#include "l7_cnfgr_api.h"
#include "l7_tftp.h"
#include "auto_install_api.h"

#define AUTO_INSTALL_HOST_FILENAME_LENGTH    37      /* 32 bytes for filename and 4 for ".cfg" extension 
                                                        1 byte for trailing zero */
#define AUTO_INSTALL_RESTART_TIMEOUT         600000  /* 600000 milliseconds in 10 minutes timeout */

#define AUTO_INSTALL_NUM_EVENT_REGISTRATIONS_MAX     5  /* This can be tuned to numberof components 
                                                           that would like receive notifications from
                                                           AutoInstall.*/

/* Time delays between downloading config file depend on conditions */
#define AUTO_INSTALL_FAILED_REQUEST_DELAY            1 /* second */

#define AUTO_INSTALL_PORT_DESCR_STR_LEN 32

/* auto-install state machine states */
typedef enum
{
  AUTO_INSTALL_START_STATE = 0,             /* 0 - Initial state of the auto-instal state machine */
  AUTO_INSTALL_DHCP_STATE,                  /* 1 - Processing DHCP/BOOTP options, checking preconditions */
  AUTO_INSTALL_GET_SPECIFIC_FILE_STATE,     /* 2 - Attempts to get <bootfile>.cfg via TFTP */
  AUTO_INSTALL_GET_PREDEFINED_FILE_STATE,   /* 3 - Attempts to get fp-net.cfg or router.cfg files */
  AUTO_INSTALL_GET_HOST_FILE_STATE,         /* 4 - Attempts to get <hostname>.cfg file */  
  AUTO_INSTALL_APPLY_STATE,                 /* 5 - Applying downloaded configuration */
  AUTO_INSTALL_WAIT_FOR_RESTART_STATE,      /* 6 - Failed to get any of the config files. Wait for restart timeout. */
  AUTO_INSTALL_AUTO_SAVE_STATE,             /* 7 - Saving the downloaded configuration.*/   
  AUTO_INSTALL_FINISHED_STATE,              /* 7 - The auto-install is finished */
  AUTO_INSTALL_STOP_STATE,                  /* 8 - Stopped by the user */
  AUTO_INSTALL_MAX_STATE                    /* 9 - Max state - new states should be added before */
} autoInstalState_t;

/* auto-install state machine events */
typedef enum
{
  AUTO_INSTALL_DHCP_MSG_EVENT = 0,            /*  0 - Recieved options from DHCP */
  AUTO_INSTALL_DHCP_DONE_EVENT,               /*  1 - Preconditions validated - starting the process */
  AUTO_INSTALL_EXECUTION_EVENT,               /*  2 - Configurator switched to the execution phase */
  AUTO_INSTALL_SPECIFIC_FILE_FAILED_EVENT,    /*  3 - Failed to download <bootfile>.cfg */
  AUTO_INSTALL_SPECIFIC_COUNT_MORE_6,         /*  4 - Made 6 attempts to get <bootfile>.cfg */
  AUTO_INSTALL_SPECIFIC_FILE_OK_EVENT,        /*  5 - <bootfile>.cfg is downloaded successfully */
  AUTO_INSTALL_PREDEFINED_FILE_FAILED_EVENT,  /*  6 - Failed to download any of predefined files */
  AUTO_INSTALL_PREDEFINED_COUNT_MORE_6,       /*  7 - Made 6 attempts to get predefined files */
  AUTO_INSTALL_PREDEFINED_FILE_OK_EVENT,      /*  8 - Predefined file is downloaded successfully */
  AUTO_INSTALL_HOSTNAME_RESOLVED_EVENT,       /*  9 - <hostname>.cfg filename resolved */
  AUTO_INSTALL_HOST_COUNT_MORE_6,             /* 10 - Made 6 attempts to get <hostname>.cfg */ 
  AUTO_INSTALL_HOST_FILE_OK_EVENT,            /* 11 - <hostname>.cfg file is downloaded successfully */
  AUTO_INSTALL_HOST_FILE_FAILED_EVENT,        /* 12 - Failed to download <hostname>.cfg file */
  AUTO_INSTALL_SAVE_CONFIG_EVENT,             /* 13 - Auto Save of configuration is enabled.*/
  AUTO_INSTALL_FINISHED_EVENT,                /* 14 - AutoInstall is finished. */
  AUTO_INSTALL_TIMER_EXPIRE_EVENT,            /* 15 - AutoInstall retry timer expired */
  AUTO_INSTALL_UI_STOP_EVENT,                 /* 16 - The user stopped the AutoInstall */
  AUTO_INSTALL_UI_START_EVENT,                /* 17 - The user started the AutoInstall */
  AUTO_INSTALL_UI_RETRY_INFINITE_ENABLE_EVENT,/* 18 - The user enabled the specific file retry infinitely. */
  AUTO_INSTALL_UI_RETRY_INFINITE_DISABLE_EVENT,/* 19 - The user disabled the specific file retry infinitely. */
  AUTO_INSTALL_UI_AUTO_SAVE_ENABLE_EVENT,        /* 20 - The user enabled the automatic downloaded configuration save.*/
  AUTO_INSTALL_AUTO_SAVE_DISABLE_EVENT,       /* 21 - The user disabled the automatic downloaded configuration save.*/
  AUTO_INSTALL_MAX_EVENT                      /* 22 - Max event - new events should be added before */
} autoInstalStateMachineEvent_t;

/* typedef for auto-install event handling function pointer */
typedef void (*autoInstallEventHandlerFnPtr)(autoInstalStateMachineEvent_t event);

/* typedef for auto-install state handling function pointer */
typedef void (*autoInstallStateHandlerFnPtr)(void);

/* typedef for auto-install state handling function pointer */
typedef void (*autoInstallTransferCompletionHandlerFnPtr)(void);

/* auto-install runtime variables/parameters */
typedef struct
{
  L7_BOOL                       startStop;                             /* auto-install start/stop mode */
  L7_BOOL                       autoSave;                              /* auto-save enable/disable mode */
  L7_uint32                     unicastRetryCnt;                       /* No. of times a failed TFTP request can be made.*/
  autoInstalState_t             currentState;                          /* curent state of the auto-install state machine */
  L7_BOOL                       currentEvents[AUTO_INSTALL_MAX_EVENT]; /* array holds current state machine events */
  autoInstallStateHandlerFnPtr  stateHandlers[AUTO_INSTALL_MAX_STATE]; /* state handler for each state */
  autoInstallEventHandlerFnPtr  eventHandlers[AUTO_INSTALL_MAX_STATE]; /* event handler for each state */
  L7_BOOL                       isFirstCycle;                          /* signals whether AutoInstall just started first cycle */
  L7_bootp_dhcp_t               bootConfigParams;                      /* boot options recieved from DHCP */
  L7_uint8                      specificFileRetryCounter;              /* counter of attempts to download <bootfile>.cfg */
  L7_uint8                      predefinedFileRetryCounter;            /* counter of attempts to download predefined files */
  L7_uint8                      hostFileRetryCounter;                  /* counter of attempts to download <hostname>.cfg file */
  L7_uint32                     tftpIpAddress;                         /* IP address of the TFTP server recieved from DHCP*/
  L7_uchar8                     bootFileName[BOOTP_FILE_LEN + 1];      /* bootfile name recieved from DHCP */
  L7_uchar8                     hostFileName[AUTO_INSTALL_HOST_FILENAME_LENGTH]; /* host filename resolved from fp-net.cfg file or via DNS */  
  L7_uint32                     currentIpAddress;                      /* TFTP server IP address the AutoInstall is currently working with */
  L7_uchar8                     currentFileName[BOOTP_FILE_LEN + 1];   /* bootfile name the AutoInstall is currently working with */
  eventNotifyList_t             notifyList[AUTO_INSTALL_NUM_EVENT_REGISTRATIONS_MAX];
  L7_TRANSFER_STATUS_t          transferStatus;                        /* TFTP transfer status */
  autoInstallTransferCompletionHandlerFnPtr  dlCompletionHandler; /* event handler for each state */
} autoInstallRunTimeData_t;

/* auto-install message types */
typedef enum 
{
  AUTO_INSTALL_CNFGR_EVENT,          /* 0 - Configurator's event */
  AUTO_INSTALL_DHCP_BOOTP_EVENT,     /* 1 - DHCP/BOOTP notification */
  AUTO_INSTALL_RESTART_TIMER_EVENT,  /* 2 - auto-install retry timer expired */
  AUTO_INSTALL_STATE_MACHINE_EVENT,  /* 3 - event generated by the state machine */
  AUTO_INSTALL_UI_EVENT,             /* 4 - the user stated/stopped auto-install from UI */
  AUTO_INSTALL_TRANSFER_STATUS_EVENT,/* 5 - Transfer completion status of download attempt */
  AUTO_INSTALL_LAST_EVENT            /* 5 - add new enum entries before AUTO_INSTALL_LAST_EVENT */
} autoInstallMgmtEvents_t;

typedef enum 
{
  AUTO_INSTALL_UI_START_STOP_EVENT,
  AUTO_INSTALL_UI_AUTO_SAVE_EVENT,
}autoInstallUIEvent_t;
/* auto-install UI message structure */
typedef struct autoInstallUIEventMsg_s
{
  autoInstallUIEvent_t uiEvent;
  union
  {
    L7_BOOL startStop;     /* Start/stop the auto-install from UI.*/
    L7_BOOL autoSave;      /* Enable/disable auto-save of installed configuration.*/
  }uiMsg;
}autoInstallUIEventMsg_t;
/* auto-install message structure */
typedef struct autoInstallMsg_s
{
  autoInstallMgmtEvents_t  event;
  union
  {
    L7_CNFGR_CMD_DATA_t           cnfgrData;  /* Configurator's data */
    L7_bootp_dhcp_t               bootParams; /* Boot options from DHCP/BOOTP */
    autoInstalStateMachineEvent_t smEvent;    /* auto-install state machine event */
    autoInstallUIEventMsg_t       uiEventMsg; /* Events from UI.*/
    L7_TRANSFER_STATUS_t          transferStatus;  /* TFTP transfer status */
  }msgData;

} autoInstallMsg_t;

typedef enum
{
  AUTO_INSTALL_PREDEFINED_FILE = 1,
  AUTO_INSTALL_SPECIFIED_FILE
} autoInstallConfigFileType;


/* Begin Function Prototypes */

void autoInstallTask();

void autoInstallSpecificFileStateComplete (void);

void autoInstallPredefinedFileStateComplete1 (void);

void autoInstallPredefinedFileStateComplete2 (void);

void autoInstallHostFileStateComplete(void);

L7_RC_t autoInstallNotificationCallback(L7_bootp_dhcp_t *pBootConfigParams);

L7_uint32 autoInstallSaveConfigCompleteCallback(L7_uint32 event);

void autoInstallStateMachineEventProcess (autoInstalStateMachineEvent_t event);

L7_RC_t autoInstallBootConfigProcess(L7_bootp_dhcp_t *pBootParams);

L7_BOOL autoInstallStartupConfigIsExist();

L7_RC_t autoInstallTftpSvrIpAddressResolve(L7_uint32 *pIpAddr);

L7_RC_t autoInstallConfigFileViaTFTPGet(L7_inet_addr_t *inet_addr,
                                                     L7_uchar8      *pConfigFileName);

void autoInstallConfigFileViaTFTPComplete(void);
                                                     
L7_int32 autoInstallTmpScriptFileCreate(L7_uchar8 *pFileName);

L7_RC_t autoInstallBootFileNameResolve(L7_char8 *configFileName);

void autoInstallRestartTimerAdd(void);

void autoInstallRestartTimerCallback();

void autoInstallTransferStatusCallback(L7_TRANSFER_STATUS_t status);

void autoInstallRestartTimerDelete();

L7_RC_t autoInstallConfigFileProcess(L7_inet_addr_t *inet_addr,
                                     L7_uchar8      *pConfigFileName);

L7_RC_t autoInstallPredefinedFilesProcess(L7_inet_addr_t *inet_addr);

L7_RC_t autoInstallConfigFileDownload(L7_inet_addr_t       *inet_address,
                                      L7_uchar8            *pConfigFileName,
                                      L7_TRANSFER_STATUS_t *pTransferStatus);

void autoInstallStateMachineEventsClear(void);

void autoInstallBindStateHandlers (void);

void autoInstallBindEventHandlers (void);

L7_RC_t autoInstallTftpFileNameParse(L7_char8 *incomingStr, L7_char8 *remoteFilePath, L7_char8 *remoteFileName);

/* End Function Prototypes */


#endif /* AUTO_INSTALL_CONTROL_H */
