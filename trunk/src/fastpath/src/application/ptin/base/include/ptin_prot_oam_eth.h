#ifndef _PTIN_PROT_OAM_ETH_
#define _PTIN_PROT_OAM_ETH_
#include <datatypes.h>
#include <ethsrv_oam.h>

extern T_ETH_SRV_OAM oam;

extern void tst_send(void);

extern int MEP_is_in_LOC(L7_ulong32 i_mep, L7_ulong32 i_rmep, T_ETH_SRV_OAM *p);

extern void ptin_oam_eth_task(void);
extern L7_RC_t ptin_ccm_packet_trap(L7_uint16 prt, L7_uint16 vlanId, L7_uint16 oam_level, L7_BOOL enable);
extern void eth_srv_oam_msg_defaults_reset(void);
extern L7_RC_t ptin_oam_eth_init(void);
#endif /*_PTIN_PROT_OAM_ETH_*/

