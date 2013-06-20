#ifndef _PTIN_PROT_OAM_ETH_
#define _PTIN_PROT_OAM_ETH_
#include <datatypes.h>
#include <ethsrv_oam.h>

extern T_ETH_SRV_OAM oam;

extern void tst_send(void);
extern void ptin_oam_eth_task(void);
extern L7_RC_t ptin_oam_eth_init(void);
#endif /*_PTIN_PROT_OAM_ETH_*/

