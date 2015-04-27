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

#define HPC_POE_TASK_MON_SLEEP 2

#define POE_CHANNEL_ALL                    128
#define POE_CONTROLLER_ACCESS_TIMEOUT       1000

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
#define PORT_POE_ON  1
#define PORT_POE_OFF 0
#define POE_NA_FIELD_VALUE 0xFF

#define BSC_MSG_LEN         12 /* bytes  */
#define POE_CMD_FIELD       0
#define POE_SUB_CMD_FIELD   1
#define POE_CHKSM_FIELD     11

#define POE_MSG_TX_RETRY_COUNT 3
#define POE_MSG_RX_RETRY_COUNT 3
#define POE_MSG_RETRY_COUNT 3

#define POE_DRVR_COMMAND_MAX_RETRIES 3

#define POE_WAIT_TIME_TO_READ_RESPONSE           35 /* in milli seconds */
#define POE_WAIT_TIME_BEF_REQUEST                1 /* in milli seconds */

#define POE_DRVR_ERROR_DOWNLOAD_IN_PROGRESS      0x8F /* download in progress - 800 msecs */
#define POE_DRVR_ERROR_CHKSUM_CALCULATION_IN_PROGRESS 0x9F /* calculating checksum for downloaded code - 800 msecs */
#define POE_DRVR_ERROR_BOOT_CODE                 0xAF /* Checksum failed for downloaded code. Need new code */
#define POE_DRVR_ERROR_RESPONSE_NOT_READY        0xFF /* Host is expecting response too fast */
#define POE_DRVR_ERROR_BAD_REQUEST_CHKSM         0xFE /* Bad checksum in the requested frame */

/* PoE command codes */
#define POE_PORT_PSE_ENABLE_CMD                  0x00
#define POE_PORT_POWER_UP_CMD                    0x01
#define POE_LOGICAL_PORT_MAP_ENABLE_CMD          0x02
#define POE_PORT_RESET_CMD                       0x03
#define POE_RAPID_POWER_DOWN_ENABLE_CMD          0x04
#define POE_PORT_STATS_RESET_CMD                 0x05
#define POE_GLOBAL_PSE_ENABLE_CMD                0x06
#define POE_GLOBAL_ILIM_CMD                      0x07
#define POE_PORT_SWITCH_HIGH_POWER_CMD           0X08
#define POE_GLOBAL_RESET_CMD                     0x09
#define POE_DETECTION_TYPE_CONFIG_CMD            0x10
#define POE_PORT_CLASSIFICATION_TYPE_CONFIG_CMD  0x11
#define POE_PORT_AUTO_POWER_UP_CONFIG_CMD        0x12
#define POE_PORT_DISCONNECT_TYPE_CONFIG_CMD      0x13
#define POE_INTERRUPT_MASK_CONFIG_CMD            0x14
#define POE_PORT_POWER_THRESHOLD_TYPE_CONFIG_CMD 0x15
#define POE_PORT_MAX_POWER_THRESHOLD_CONFIG_CMD  0x16
#define POE_POWER_MGMT_MODE_CONFIG_CMD           0x17
#define POE_POWER_SOURCE_CONFIG_CMD              0x18
#define POE_PORT_PAIR_CONFIG_CMD                 0x19
#define POE_PORT_PRIORITY_CONFIG_CMD             0x1A
#define POE_PORT_POWER_UP_MODE_CONFIG_CMD        0x1C
#define POE_PORT_MAP_CONFIG_CMD                  0x1D
#define POE_SYSTEM_STATUS_QUERY_CMD              0x20
#define POE_PORT_STATUS_QUERY_CMD                0x21
#define POE_PORT_STATS_QUERY_CMD                 0x22
#define POE_TOTAL_POWER_ALLOC_QUERY_CMD          0x23
#define POE_INTERRUPT_STATUS_QUERY_CMD           0x24
#define POE_PORT_CONFIG_QUERY_CMD                0x25
#define POE_PORT_EXTEN_CONFIG_QUERY_CMD          0x26
#define POE_POWER_MGMT_CONFIG_QUERY_CMD          0x27
#define POE_MULTIPLE_PORT_STATUS_QUERY_CMD       0x28
#define POE_PORT_MEASUREMENT_QUERY_CMD           0x30

#define POE_EEPROM_CONFIG_CMD                    0xE0
/* Poe Sub Commands */
#define POE_EEPROM_CLEAR_CONFIG_SUB_CMD          0xE0
#define POE_EEPROM_CLEAR_APPLICATION_SUB_CMD     0xC0
#define POE_EEPROM_SAVE_CONFIG_SUB_CMD           0xF0
#define POE_EEPROM_SAVE_APPLICATION_SUB_CMD      0xD0
#define POE_EEPROM_IMAGE_DOWNLOAD_SUB_CMD        0x80
#define POE_EEPROM_IMAGE_FORCE_CRC_SUB_CMD       0x40

#define POE_PORT_DETECT_STAT_DELIVERING          2
#define POE_PORT_DETECT_STAT_REQUESTING          6

typedef enum{
    POE_E_NONE                 =  0, /* No Error */
    POE_E_INIT                 = -1, /* Initializaiton failure */
    POE_E_CODE                 = -2, /* POE Chip has bad code. Need new code */
    POE_E_MAX_RETRIES          = -3, /* Unable to communicate successfully in Max retries */
    POE_E_SEND                 = -4, /* Tx to POE chip is failed */
    POE_E_RECEIVE              = -5, /* Rx from POE chip is failed */
    POE_E_CHECKSUM             = -6, /* Checksum incorrect in the received packet */
  POE_E_ACK            = -7  /* No Acknowledgement */
}_poe_error_t;

#define L7_POE_OK(rc)     ((((rc) > 0)                 || \
                       ((rc) == POE_E_NONE)) ? TRUE : FALSE)

#define BCM5910X_PORT_POWER_IN_WATTS(x)   ((x) / 10)    /* In watts */
#define BCM5910X_PORT_POWER(x)            ((x) * 100)    /* In milli watts */
#define BCM5910X_PORT_VOLTAGE(x)          (((x) * 64.45)/1000)  /* In Volts */
/* Temp in Centigrade = (#### - 0x7F)*(-1.25) + 125) */
#define BCM5910X_PORT_TEMPERATURE(x)      (((x) - 0x7F)*(-1.25) + 125)
#define BCM5910X_PORT_CURRENT(x)          ((x))  /* In milli Amps  */

#endif  /* HPC_POE_PWRDSNE_H */
