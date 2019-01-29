/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2002-2007
 *
 **********************************************************************
 *
 * @filename src/mgmt/cli/base/clicommands_lldp.h
 *
 * @purpose header for 802.1AB commands in clicommands_lldp.c
 *
 * @component user interface
 *
 * @comments none
 *
 * @create  18/02/2005
 *
 * @author  Rama Krishna Hazari
 * @end
 *
 ***********************************************************************/

#ifndef CLICOMMANDS_LLDP_H
#define CLICOMMANDS_LLDP_H

/*****************************************************************/

#define CLISYNTAX_LLDP_TRANSMIT           "\r\nError! Use \"lldp transmit\"\
 to advertise local LLDP data on the interface."

#define CLISYNTAX_LLDP_RECEIVE            "\r\nError! Use \"lldp receive\"\
 to receive remote LLDP data on the interface."

#define CLISYNTAX_LLDP_TXMGMT             "\r\nError! Use \"lldp transmit-mgmt\"\
 to transmit the LLDP management address TLV on the interface."

#define CLISYNTAX_LLDP_NOTIFICATION       "\r\nError! Use\"lldp notification\"\
 to enable remote data change notifications."

#define CLISYNTAX_LLDP_NOTIF_INTERVAL     "\r\nError! Use \"lldp notification-interval <interval> \"\
 to limit how frequently LLDP remote data change notifications are sent."

#define CLISYNTAX_LLDP_NOTIF_INTV_RANGE   "\r\nError! Enter a value\
 in the range <5 - 3600> seconds."

#define CLISYNTAX_LLDP_TRAN_TLV           "\r\nError! Use \"lldp transmit-tlv [sys-desc] [sys-name] [sys-cap] [port-desc] \"\
 to transmit LLDP optional TLV(s) on the interface."

#define CLISYNTAX_LLDP_TIMERS             "\r\nError! Use \"lldp timers\
 [interval <transmit-interval>] [hold <hold-multiplier>] [reinit <reinit-delay>]\"\
 to configure the LLDP global timer values."


/* Defining constants */
/* begin */
#define CLILLDP_NOTI_INTERVAL_MIN         5
#define CLILLDP_NOTI_INTERVAL_MAX         3600
#define CLILLDP_TIMER_INTERVAL_MIN        5
#define CLILLDP_TIMER_INTERVAL_MAX        32768
#define CLILLDP_TIMER_HOLD_MIN            2
#define CLILLDP_TIMER_HOLD_MAX            10
#define CLILLDP_TIMER_REINIT_MIN          1
#define CLILLDP_TIMER_REINIT_MAX          10

/* end */

/*****************************************************************/

/* BEGIN LLDP TREE COMMAND STRUCTURE */
extern void buildTreeGlobalConfigLldp(EwsCliCommandP depth1);
extern void buildTreeInterfaceConfigLldp(EwsCliCommandP depth1);
extern void buildTreeShowConfigLldp(EwsCliCommandP depth1);
extern void buildTreeGlobalLldpClear(EwsCliCommandP depth2);

/* BEGIN LLDP ACTION COMMANDS DECL */
extern const L7_char8 *commandLLDPTransmit(EwsContext ewsContext,
                                          L7_uint32 argc,
                                           const L7_char8 * * argv,
                                          L7_uint32 index);

extern const L7_char8 *commandLLDPReceive(EwsContext ewsContext,
                                         L7_uint32 argc,
                                          const L7_char8 * * argv,
                                         L7_uint32 index);

extern const L7_char8 *commandLLDPTransmitMgmt(EwsContext ewsContext,
                                              L7_uint32 argc,
                                               const L7_char8 * * argv,
                                              L7_uint32 index);

extern const L7_char8 *commandLLDPNotification(EwsContext ewsContext,
                                              L7_uint32 argc,
                                               const L7_char8 * * argv,
                                              L7_uint32 index);

extern const L7_char8 *commandLLDPNotificationInterval(EwsContext ewsContext,
                                                      L7_uint32 argc,
                                                       const L7_char8 * * argv,
                                                      L7_uint32 index);

extern const L7_char8 *commandLLDPTransmitTLV(EwsContext ewsContext,
                                              L7_uint32 argc,
                                              const L7_char8 * * argv,
                                              L7_uint32 index);

extern const L7_char8 *commandLLDPTimers(EwsContext ewsContext,
                                        L7_uint32 argc,
                                         const L7_char8 * * argv,
                                        L7_uint32 index);

extern const L7_char8 *commandLLDPClearStats(EwsContext ewsContext,
                                            L7_uint32 argc,
                                             const L7_char8 * * argv,
                                            L7_uint32 index);

extern const L7_char8 *commandLLDPClearRemoteData(EwsContext ewsContext,
                                                  L7_uint32 argc,
                                                  const L7_char8 * * argv,
                                                  L7_uint32 index);

extern const L7_char8 *commandLLDPShow(EwsContext ewsContext,
                                      L7_uint32 argc,
                                       const L7_char8 * * argv,
                                      L7_uint32 index);

extern const L7_char8 *commandLLDPShowInterface(EwsContext ewsContext,
                                                L7_uint32 argc,
                                                const L7_char8 * * argv,
                                                L7_uint32 index);

extern const L7_char8 *commandLLDPShowStatistics(EwsContext ewsContext,
                                                 L7_uint32 argc,
                                                 const L7_char8 * * argv,
                                                 L7_uint32 index);

extern const L7_char8 *commandLLDPShowRemoteDevice(EwsContext ewsContext,
                                                   L7_uint32 argc,
                                                   const L7_char8 * * argv,
                                                   L7_uint32 index);

extern const L7_char8 *commandLLDPShowRemoteDeviceDetail(EwsContext ewsContext,
                                                         L7_uint32 argc,
                                                         const L7_char8 * * argv,
                                                         L7_uint32 index);

extern const L7_char8 *commandLLDPShowLocalDevice(EwsContext ewsContext,
                                                  L7_uint32 argc,
                                                  const L7_char8 * * argv,
                                                  L7_uint32 index);

extern const L7_char8 *commandLLDPShowLocalDeviceDetail(EwsContext ewsContext,
                                                        L7_uint32 argc,
                                                        const L7_char8 * * argv,
                                                        L7_uint32 index);

extern const L7_char8 *commandLLDPMed(EwsContext ewsContext,
                                      L7_uint32 argc,
                                      const L7_char8 **argv,
                                      L7_uint32 index);

extern const L7_char8 *commandLLDPMedConfigNotification(EwsContext ewsContext,
                                                        L7_uint32 argc,
                                                        const L7_char8 **argv,
                                                        L7_uint32 index);

extern const L7_char8 *commandLLDPMedFastStartRepeatCount(EwsContext ewsContext,
                                                          L7_uint32 argc,
                                                          const L7_char8 **argv,
                                                          L7_uint32 index);

extern const L7_char8 *commandLLDPMedTransmitTlv(EwsContext ewsContext,
                                                 L7_uint32 argc,
                                                 const L7_char8 **argv,
                                                 L7_uint32 index);

extern const L7_char8 *commandLLDPMedShow(EwsContext ewsContext,
                                          L7_uint32 argc,
                                          const L7_char8 **argv,
                                          L7_uint32 index);

extern const L7_char8 *commandLLDPMedShowInterface(EwsContext ewsContext,
                                                   L7_uint32 argc,
                                                   const L7_char8 **argv,
                                                   L7_uint32 index);

extern const L7_char8 *commandLLDPMedShowLocalDeviceDetail(EwsContext ewsContext,
                                                           L7_uint32 argc,
                                                           const L7_char8 **argv,
                                                           L7_uint32 index);

extern const L7_char8 *commandLLDPMedShowRemoteDevice(EwsContext ewsContext,
                                                      L7_uint32 argc,
                                                      const L7_char8 **argv,
                                                      L7_uint32 index);

extern const L7_char8 *commandLLDPMedShowRemoteDeviceDetail(EwsContext ewsContext,
                                                            L7_uint32 argc,
                                                            const L7_char8 **argv,
                                                            L7_uint32 index);

extern L7_RC_t *usmDbRxAdminModeGet(L7_uint32 * mode);

extern const L7_char8 *commandLLDPMedAll(EwsContext ewsContext, L7_uint32 argc,
                                         const L7_char8 **argv, L7_uint32 index);

extern const L7_char8 *commandLLDPMedConfigNotificationAll(EwsContext ewsContext, L7_uint32 argc,
                                                           const L7_char8 **argv, L7_uint32 index);

extern const L7_char8  *commandLLDPMedTransmitTlvAll(EwsContext ewsContext, L7_uint32 argc,
                                                     const L7_char8 **argv, L7_uint32 index);

/* END LLDP ACTION COMMANDS DECL */
#endif
