/*********************************************************************
* <pre>
* LL   VV  VV LL   7777777  (C) Copyright LVL7 Systems 2001-2007
* LL   VV  VV LL   7   77   All Rights Reserved.
* LL   VV  VV LL      77
* LL    VVVV  LL     77
* LLLLL  VV   LLLLL 77      Code classified LVL7 Confidential
* </pre>
**********************************************************************
*
* @filename  broad_poe.h
*
* @purpose   This file contains the POE code
*
* @component hapi
*
* @comments
*
* @create    10/2/2003
*
* @author    colinw
*
* @end
*
**********************************************************************/

#ifndef HPC_POE_PWRDSNE_H
#define HPC_POE_PWRDSNE_H

#define POE_TASK_MON_SLEEP 2

#define POE_CHANNEL_ALL                    128

/* PowerDsine Command/Response Codes */
#define POE_CODE_ACTIVECHANNELMATRIX       0x43
#define POE_CODE_ALL_CHANNELS              0x80
#define POE_CODE_BAT                       0x16
#define POE_CODE_CHANNEL                   0x05
#define POE_CODE_COMMAND                   0x00
#define POE_CODE_POWERGUARDBAND            0x13 /* 19 Watts */
#define POE_CODE_DETECTTEST                0x3C
#define POE_CODE_DEVICE_PARAMS             0x87
#define POE_CODE_E2                        0x06
#define POE_CODE_FLASH                     0xFF
#define POE_CODE_FORCEPOWER                0x51
#define POE_CODE_GLOBAL                    0x07
#define POE_CODE_INDIVIDUALMASK            0x56
#define POE_CODE_LATCH1                    0x3A
#define POE_CODE_LATCH2                    0x49
#define POE_CODE_MAIN                      0x17
#define POE_CODE_MASKZ                     0x2B
#define POE_CODE_MEASUREMENTZ              0x1A
#define POE_CODE_N                         0x4E
#define POE_CODE_ONOFF                     0x0C
#define POE_CODE_PARAMZ                    0x25
#define POE_CODE_POE_DEVICE_VERSION        0x5E
#define POE_CODE_PORTFULLINIT              0x4A
#define POE_CODE_PORTSPOWER1               0x4B
#define POE_CODE_PORTSPOWER2               0x4C
#define POE_CODE_PORTSPOWER3               0x4D
#define POE_CODE_PORTSPOWER4               0x4F
#define POE_CODE_PORTSPOWER5               0x50
#define POE_CODE_PORTSSTATUS1              0x31
#define POE_CODE_PORTSSTATUS2              0x32
#define POE_CODE_PORTSSTATUS3              0x33
#define POE_CODE_PORTSSTATUS4              0x47
#define POE_CODE_PORTSSTATUS5              0x48
#define POE_CODE_PORTSTATUS                0x0E
#define POE_CODE_POWERBUDGET               0x57
#define POE_CODE_POWERLOSS                 0x58
#define POE_CODE_POWERMANAGE_MODE          0x5F
#define POE_CODE_PRIORITY                  0x0A
#define POE_CODE_PRODUCTINFOZ              0x13
#define POE_CODE_PROGRAM                   0x01
#define POE_CODE_REPORT                    0x52
#define POE_CODE_REQUEST                   0x02
#define POE_CODE_RESET                     0x55
#define POE_CODE_RESTOREFACT               0x2D
#define POE_CODE_SAVECONFIG                0x0F
#define POE_CODE_SOFTWAREDOWNLOADTELEMETRY 0x54
#define POE_CODE_SOFTWAREVERSION           0x21
#define POE_CODE_SUPPLY                    0x0B
#define POE_CODE_SUPPLY1                   0x15
#define POE_CODE_SUPPLY2                   0x16
#define POE_CODE_SYSTEMSTATUS              0x3D
#define POE_CODE_TELEMETRY                 0x03
#define POE_CODE_TEMPORARY                 0x42
#define POE_CODE_TEMPORARYCHANNELMATRIX    0x43
#define POE_CODE_UDLCOUNTER                0x59
#define POE_CODE_UDLCOUNTER2               0x5A
#define POE_CODE_USERBYTE                  0x41
#define POE_CODE_VERSION_30K               0x1F
#define POE_CODE_VERSION_30KDATE           0x22
#define POE_CODE_VERSIONZ                  0x1E


#define S19_RECORD_TYPE_OFFSET 1
#define S19_RECORD_TYPE_HEADER '0'
#define S19_RECORD_TYPE_DATA   '1'
#define S19_RECORD_TYPE_EOF    '9'

#define POE_PROGRAM_MODE_CHAR_DELAY 10 /* ms */
#define POE_ERASE_TIMEOUT           5 /* seconds */
#define POE_KEY                0
#define POE_ECHO               1
#define POE_SUBJECT            2
#define POE_SUBJECT1           3
#define POE_SUBJECT2           4

#define POE_PROG_REQD          0x02
#define POE_PM_NO_PRIORITY     0x01
#define POE_PM_PRIORITY        0x00
#define POE_CD_RESCAP_MODE     0x02
#define POE_CD_RES_MODE        0x00


#define POE_CTLR_RSP_TPE     "TPE\r\n"
#define POE_CTLR_RSP_TOE     "TOE\r\n"
#define POE_CTLR_RSP_TE      "TE\r\n"
#define POE_CTLR_RSP_TOP     "TOP\r\n"
#define POE_CTLR_RSP_TSPLAT  "T*\r\n"
#define POE_CTLR_RSP_TP      "TP\r\n"

/* The following can be overriden in Platform specific include file */
#define POE_MIN_VOLTAGE           440
#define POE_MAX_VOLTAGE           570
#define POE_MIN_POWER              37

#define PD64008                     1
#define PD64012                     2

#define PD_SYSTEM PD64012

#endif  /* HPC_POE_PWRDSNE_H */
