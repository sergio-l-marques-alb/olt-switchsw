#ifndef _PTIN_PROT_OAM_ETH_
#define _PTIN_PROT_OAM_ETH_
#include <datatypes.h>
#include <ethsrv_oam.h>

extern T_ETH_SRV_OAM oam;

extern void dump_uplinkprot_traps(void);

extern int send_also_uplinkprot_traps(L7_uint8 set1_clr0_init2, L7_uint16 slot, L7_uint16 port, L7_uint64 vid);

extern void tst_send(void);

extern int MEP_is_in_LOC(L7_ulong32 i_mep, L7_ulong32 i_rmep, T_ETH_SRV_OAM *p);

extern void ptin_oam_eth_task(void);
extern L7_RC_t ptin_ccm_packet_trap(L7_uint16 prt, L7_uint16 vlanId, L7_uint16 oam_level, L7_BOOL enable);
extern void eth_srv_oam_msg_defaults_reset(void);
extern L7_RC_t ptin_oam_eth_init(void);


/************************************************************************************************************* 
  Cisco needs CCMs belonging to OLT-BNG control plane MEPs not to pass over ERPs' RPLs.
  This is the fix
*************************************************************************************************************/
extern
int dont_txrx_oam_criterion(unsigned char init0_setbmp1_clrbmp2_rd3, unsigned short prt, unsigned short iMEP,
                            unsigned short vid,      //just on read
                            unsigned char *vid_bmp   //always but on read
                               );
//PLAN A
//Only prt and vid parameters are valid

#endif /*_PTIN_PROT_OAM_ETH_*/

