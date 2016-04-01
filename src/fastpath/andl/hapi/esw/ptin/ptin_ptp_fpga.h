#ifndef _PTIN_PTP_FPGA_H
#define _PTIN_PTP_FPGA_H

//http://wiki.ptin.corppt.com/pages/viewpage.action?pageId=730693695
//(Ata de 2015-11-27 - OLT1T0 - Y.1731 (CCMs) & IEEE1588)

//#include "ptin_hapi.h"
#include "l7_common.h"
//#include "ptin_utils.h"
#include <ptin_structs.h>
//typedef struct {
//    struct {
//        unsigned short  prt;
//        unsigned short  vid;
//    } __attribute__((packed)) key;
//
//    unsigned short  vid_prt;
//    unsigned short  vid_os;
//    int encap;
//#define TS_ENCAP_ETH_PTP            1
//#define TS_ENCAP_ETH_IPv4_PTP       2
//#define TS_ENCAP_ETH_IPv6_PTP       3
////#define TS_ENCAP_ETH_MPLS_IPv4_PTP  4
////#define TS_ENCAP_ETH_MPLS_IPv6_PTP  5
//    union {
//        u8 DMAC[6];                     //If all 0s, matches any DMAC
//        u8 dIP[16];                     //If all 0s, matches any IP     //big Endian
//    } __attribute__((packed)) ntw;
//} __attribute__((packed)) ptin_dtl_search_ptp_t;




//P T P / 1588***********************************************************************************
//ANDL/HAPI LAYER********************************************************************************
extern void ptin_hapi_ptp_table_init(void);
extern L7_RC_t ptin_hapi_ptp_entry_add(ptin_dapi_port_t *dapiPort, ptin_dtl_search_ptp_t *entry);
extern L7_RC_t ptin_hapi_ptp_entry_del(ptin_dapi_port_t *dapiPort, ptin_dtl_search_ptp_t *entry);
extern L7_RC_t ptin_hapi_ptp_dump(void);


//DTL/APP LAYER**********************************************************************************
extern void ptin_ptp_flows_init(void);
extern L7_RC_t ptin_ptp_fpga_entry(ptin_dtl_search_ptp_t *e, DAPI_CMD_GET_SET_t operation);




//O A M******************************************************************************************
//ANDL/HAPI LAYER********************************************************************************
extern void ptin_hapi_oam_table_init(void);
extern L7_RC_t ptin_hapi_oam_entry_add(ptin_dapi_port_t *dapiPort, ptin_dtl_search_oam_t *entry);
extern L7_RC_t ptin_hapi_oam_entry_del(ptin_dapi_port_t *dapiPort, ptin_dtl_search_oam_t *entry);
extern L7_RC_t ptin_hapi_oam_dump(void);


//DTL/APP LAYER**********************************************************************************
extern L7_RC_t ptin_oam_fpga_entry(ptin_dtl_search_oam_t *e, DAPI_CMD_GET_SET_t operation);
#endif /*_PTIN_PTP_FPGA_H*/

