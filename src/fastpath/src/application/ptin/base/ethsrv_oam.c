//**************************************************************************************
//
//ABSTRACT:
//Ethernet Service OAM types/functions/objects
//(based on ITU-T Y.1731 and IEEE 802.1ag)
//
//HISTORY:
//
//
//COPYRIGHT:
//PT Inova��o (a.k.a. PTIn, a.k.a. CET)
//
//
//CONTACT:
//joao-v-mateiro@ptinovacao.pt; rcosta@ptinovacao.pt
//
//**************************************************************************************

//EXTERNAL TYPES/VARIABLES/ROUTINES/OBJECTS / #includeS ********************************
#include <string.h>
#include <stdio.h>


#include <arpa/inet.h>
//extern unsigned short htons(unsigned short host_value); //Endianess compatibility
//extern unsigned short ntohs(unsigned short netw_value);
//extern unsigned long htonl(unsigned long host_value);
//extern unsigned long ntohl(unsigned long netw_value);
//these functions return argument as is in BIG, inverted in LITTLE ENDIAN machines

//OWN TYPES/VARIABLES/OBJECTS***********************************************************
#include <ethsrv_oam.h>
//PUBLIC VARIABLES/OBJECTS**************************************************************
__OAM_TIMER_CODE_VALUES_DECLARATION__
__OAM_MC_MAC_DECLARATION__
//OWN ROUTINES**************************************************************************
static int send_ccm(u16 oam_prt, T_MEP_HDR *p_mep, u8 RDI, u8 use_mcast_DMAC);
static int send_csf(u16 oam_prt, T_MEP_HDR *p_mep, u8 CSF_period, u8 CSF_flags);
static int send_lmm(u16 i_mep, T_MEP_HDR *p_mep, T_MEP_LM *p_lm);
static int rx_ccm(u16 oam_prt, u8 *pkt_ethtype, u32 pkt_len, u64 vid, u8 *pSMAC,
                    T_MEP_DB *p_mep_db, T_LOOKUP_MEP *p_mep_lut);
static int rx_csf(u16 oam_prt, u8 *pkt_ethtype, u32 pkt_len, u64 vid, u8 *pSMAC,
                    T_MEP_DB *p_mep_db, T_LOOKUP_MEP *p_mep_lut);
static int rx_lmm(u16 oam_prt, u8 *pkt_ethtype, u32 pkt_len, u64 vid, u8 *pSMAC,
                    T_MEP_DB *p_mep_db, T_LOOKUP_MEP *p_mep_lut, u64 RxFCl);
static int rx_lmr(u16 oam_prt, u8 *pkt_ethtype, u32 pkt_len, u64 vid, u8 *pSMAC,
                    T_MEP_DB *p_mep_db, T_LOOKUP_MEP *p_mep_lut, u64 RxFCl);
static int rx_lbm(u16 oam_prt, u8 *pkt_ethtype, u32 pkt_len, u64 vid, u8 *pSMAC,
                    T_MEP_DB *p_mep_db, T_LOOKUP_MEP *p_mep_lut);
static int rx_ltm(u16 oam_prt, u8 *pkt_ethtype, u32 pkt_len, u64 vid, u8 *pSMAC,
                    T_MEP_DB *p_mep_db, T_LOOKUP_MEP *p_mep_lut);
//PUBLIC ROUTINES***********************************************************************

static void init_mep(T_MEP *p_mep) {
u32 i;
    memset(p_mep, 0xff, sizeof(T_MEP));
    for (i=0; i<N_MAX_MEs_PER_MEP; i++) p_mep->ME[i].RDI=0;
}//init_mep











static void init_mep_csf(T_MEP_CSF *p_mep_csf) {
    memset(p_mep_csf, 0xff, sizeof(T_MEP_CSF));
    p_mep_csf->en1dis0 = 0;   
}











static void init_mep_lm(T_MEP_LM *p) {
    invalidate_T_MEP_LM(p)
}




void init_mep_db(T_MEP_DB *p_mep_db) {
u32 i;
T_MEP *_p_mep;

 //init_simple_lnkd_list(*p_mep_db, N_MEPs);
 for (i=0; i<N_MEPs; i++) {
     //set_active_to_(*p_mep_db,i);
     //_p_mep= pointer2active_node_info(*p_mep_db);
     _p_mep= &p_mep_db[i].mep;
     init_mep(_p_mep);
     init_mep_csf(&p_mep_db[i].mep_csf);
     init_mep_lm(&p_mep_db[i].lm);
 }//for
}//init_mep_db













static void init_mep_lookup(T_LOOKUP_MEP *p_mep_lut) {
 p_mep_lut->mep_id=     INVALID_MEP;
 p_mep_lut->mep_index=  0UL-1;
}//init_mep_lookup




void init_mep_lookup_table(T_LOOKUP_MEP *p_mep_lut) {
u32 i;
 for (i=0; i<N_MAX_LOOKUP_MEPs; i++) init_mep_lookup(&p_mep_lut[i]);
}//init_mep_lookup_table





















void init_eth_srv_oam(T_ETH_SRV_OAM *p) {
 init_mep_db(p->db);
 init_mep_lookup_table(p->mep_lut);
 p->proc_i_mep= 0;
}//init_eth_srv_oam

































static u64 hash(u64 in, short ibits, short obits) {
u64 out;
 for (out=0; ibits>0; ibits-=obits, in>>=obits) out^=   in;
 return out & ((1<<obits)-1);
}//hash




/*u32 hash_vidmac_to_mepid(u64 vid, T_ETH_OAM_MAC *p) {
u64 in;
 in=    ((u64)vid)<<48 | *((u32 *)&p->byte[0])<<16 | *((u16 *)&p->byte[4]);
 return hash(in, 48+12, MEP_BITS);
}//hash_vidmac_to_mepid




static u32 hash_vidmepid_to_lookup_index(u64 vid, u32 mep_id) {
u32 in;
 in=    mep_id<<12 | vid;
 return hash(in, MEP_BITS+12, LOOKUP_MEP_BITS);
}//hash_vidmepid_to_lookup_index
*/






//returns indexes for MEP table (and its RMEP table) according to format iMEP_iRMEP_TO_MEP_INDEX(,)
u32 finger_lut_index(u8 find0_add1_del2,                                                                                    //operation
                     u16 prt, u64 vid, u8 level, u8 mep0_rmep1, u32 mep_id, u32 i_mep, u32 i_rmep, T_LOOKUP_MEP *p_mep_lut, //input     (i_mep, i_rmep used only when ADDING)
                     u8 *unxlvl0_msmrg1_unxmep2_unxmeppotentloop3_unxperiod4, u32 *alrm_index) {                            //additional outputs

u32 h, i, j, _1st_unoccupied=N_MAX_LOOKUP_MEPs;
u32 unx_lvl=0UL-1, msmrg=0UL-1, unx_mep=0UL-1, unx_mep_potent_loop=0UL-1, unx_period=0UL-1;

 mep0_rmep1=    mep0_rmep1? 1:0;

 h= hash( (((level<<1 /*| mep0_rmep1*/)<<10 | prt)<<21 | vid)<<MEP_BITS /*| mep_id*/,   3+1+10+21+MEP_BITS, LOOKUP_MEP_BITS);

 for (i=0, j=h; i<N_MAX_LOOKUP_MEPs_PER_HASH; i++) {
     if (EMPTY_T_MEP(p_mep_lut[j])) {
         if (!valid_lookup_index(_1st_unoccupied))  _1st_unoccupied=j;
     }
     else {//Not empty
         if (p_mep_lut[j].prt==prt && p_mep_lut[j].vid==vid) {          //Check if entry already in lookup table...
             if (p_mep_lut[j].level==level) {
                 //if (MEG==MEG) {
                     if (p_mep_lut[j].mep_id==mep_id) {
                         if (p_mep_lut[j].mep0_rmep1==mep0_rmep1) {
                             unx_lvl=0UL-1, msmrg=0UL-1, unx_mep=0UL-1, unx_mep_potent_loop=0UL-1, unx_period=0UL-1;
                             goto _finger_lut_index;   //return j;
                         }
                         else {unx_mep=j;   unx_mep_potent_loop=j;}
                     }
                     else unx_mep=j;
                 //}
                 //else msmrg=j;
             }
             else unx_lvl=j;
         }
         else msmrg=j;//?
     }

     if (++j>=N_MAX_LOOKUP_MEPs) j=0;   //j= (h+i)%N_MAX_LOOKUP_MEPs;
 }//for

 j=_1st_unoccupied;

_finger_lut_index:
 //Filling "unxlvl0_msmrg1_unxmep2_unxmeppotentloop3_unxperiod4" and "alrm_index"
 if (NULL!=unxlvl0_msmrg1_unxmep2_unxmeppotentloop3_unxperiod4 && NULL!=alrm_index) {
     if (valid_lookup_index(unx_period)) {
         *unxlvl0_msmrg1_unxmep2_unxmeppotentloop3_unxperiod4=4;
         *alrm_index=   p_mep_lut[unx_period].mep_index;
     }
     else
     if (valid_lookup_index(unx_mep_potent_loop)) {
         *unxlvl0_msmrg1_unxmep2_unxmeppotentloop3_unxperiod4=3;
         *alrm_index=   p_mep_lut[unx_mep_potent_loop].mep_index;
     }
     else
     if (valid_lookup_index(unx_mep)) {
         *unxlvl0_msmrg1_unxmep2_unxmeppotentloop3_unxperiod4=2;
         *alrm_index=   p_mep_lut[unx_mep].mep_index;
     }
     else
     if (valid_lookup_index(msmrg)) {
         *unxlvl0_msmrg1_unxmep2_unxmeppotentloop3_unxperiod4=1;
         *alrm_index=   p_mep_lut[msmrg].mep_index;
     }
     else
     if (valid_lookup_index(unx_lvl)) {
         *unxlvl0_msmrg1_unxmep2_unxmeppotentloop3_unxperiod4=0;
         *alrm_index=   p_mep_lut[unx_lvl].mep_index;
     }
     else {
         *unxlvl0_msmrg1_unxmep2_unxmeppotentloop3_unxperiod4=-1;
         *alrm_index=   0UL-1;
     }
 }//if (NULL!=unxlvl0_msmrg1_unxmep2_unxmeppotentloop3_unxperiod4 && NULL!=alrm_index)

 //ETHSRV_OAM_LOG("h=%lu\tj=%lu\t_1st_unoccupied=%lu"NLS, h, j, _1st_unoccupied);
 if (!valid_lookup_index(j)) return 0UL-1;//iMEP_iRMEP_TO_MEP_INDEX(N_MEPs, N_MAX_MEs_PER_MEP);

 i= p_mep_lut[j].mep_index;

 switch(find0_add1_del2) {
 case 2:    init_mep_lookup(&p_mep_lut[j]); break;
 case 1:
     if (j!=_1st_unoccupied)    return 0UL-1;//break;//return 0UL-1;
     p_mep_lut[j].level=        level;
     p_mep_lut[j].mep0_rmep1=   mep0_rmep1;
     p_mep_lut[j].mep_id=       mep_id;
     p_mep_lut[j].mep_index=    i=  iMEP_iRMEP_TO_MEP_INDEX(i_mep, i_rmep);
     p_mep_lut[j].prt=          prt;
     p_mep_lut[j].vid=          vid;
     break;
 case 0:
     if (j==_1st_unoccupied)    return 0UL-1;
 }
 //ETHSRV_OAM_LOG("i=%lu\tj=%lu\t_1st_unoccupied=%lu"NLS, i, j, _1st_unoccupied);
 return i;
}//finger_lut_index



































int invalid_T_MEP_HDR(const T_MEP_HDR *p_mep) {
//u32 i, j;

 if (EMPTY_T_MEP(*p_mep)) return 0;

 if (!valid_oam_level(p_mep->level))    return 1;
 if (!valid_oam_tmr(p_mep->tmout))      return 1;
 //if (p_mep->vid>4095)                   return 1;

 if (p_mep->prt>=N_OAM_PRTS)            return 1;

 return 0;
}//invalid_T_MEP_HDR
















//Return status:
//0 - OK
//1 - Invalid T_MEP
//2 - MEP table full
//3 - MEP Lookup table full (please try another MEP ID)
//4 - A remote MEP already is using this MEP ID (please try another MEP ID)
int wr_mep(u32 i_mep, T_MEP_HDR *p_mep, T_ETH_SRV_OAM *p_oam) {
u32 i_look;
T_MEP_DB *p_mep_db;
T_LOOKUP_MEP *p_mep_lut;
T_MEP   *_p_mep;

 if (invalid_T_MEP_HDR(p_mep) || !valid_mep_index(i_mep))  return 1;

 p_mep_db=  p_oam->db;
 p_mep_lut= p_oam->mep_lut;

 //Check if entry already in lookup table...
 i_look=    finger_lut_index(1, p_mep->prt, p_mep->vid, p_mep->level, 0, p_mep->mep_id, i_mep, N_MAX_MEs_PER_MEP, p_mep_lut, NULL, NULL); //vidmepid_to_lookup_index(p_mep->vid, p_mep->mep_id, p_mep_lut);
 if (MEP_INDEX_TO_iMEP(i_look)!=i_mep)              return 3;
 if (MEP_INDEX_TO_iRMEP(i_look)!=N_MAX_MEs_PER_MEP) return 4;

 _p_mep= &p_mep_db[i_mep].mep;
 init_mep(_p_mep);
 _p_mep->meg_id=     p_mep->meg_id;
 _p_mep->mep_id=     p_mep->mep_id;
 _p_mep->level=      p_mep->level;
 _p_mep->tmout=      p_mep->tmout;
 _p_mep->vid=        p_mep->vid;
 _p_mep->prt=        p_mep->prt;
 _p_mep->prior=      p_mep->prior;
 _p_mep->up1_down0=  p_mep->up1_down0;
 _p_mep->CoS=        p_mep->CoS;

 return 0;
}//wr_mep




//Return status:
//0 - OK
//1 - Invalid index
int del_mep(u32 i_mep, T_ETH_SRV_OAM *p_oam) {
u32 i;
T_MEP       *_p_mep;
T_MEP_CSF   *_p_mep_csf;
T_ME        *_p_me;
T_MEP_DB    *p_mep_db;
T_LOOKUP_MEP *p_mep_lut;


 if (!valid_mep_index(i_mep))    return 1;

 p_mep_db=      p_oam->db;
 p_mep_lut=     p_oam->mep_lut;

 _p_mep= &p_mep_db[i_mep].mep;
 _p_mep_csf= &p_mep_db[i_mep].mep_csf;
 //Remove entry in lookup table...
 finger_lut_index(2, _p_mep->prt, _p_mep->vid, _p_mep->level, 0, _p_mep->mep_id, -1, -1, p_mep_lut, NULL, NULL);

 //Remove entry in T_MEP table (removing all related remote MEPs in all MEs, 1st)
 for (i=0; i<N_MAX_MEs_PER_MEP; i++) {
     _p_me=    &_p_mep->ME[i];
     if (EMPTY_T_MEP(*_p_me))  continue;
     finger_lut_index(2, _p_mep->prt, _p_mep->vid, _p_mep->level, 1, _p_me->mep_id, -1, -1, p_mep_lut, NULL, NULL);
     //memset(_p_me, 0xff, sizeof(T_ME));
 }
 init_mep(_p_mep);
 init_mep_csf(_p_mep_csf);
 init_mep_lm(&p_mep_db[i_mep].lm);

 return 0;
}//del_mep




































//Return status:
//0 - OK
//1 - Invalid T_RMEP
//2 - Invalid T_MEP
//3 - MEP and remote MEP don't belong to the same MEG or MEG level
//4 - MEP Lookup table full or remote MEP ID already in use (please try another remote MEP ID)
//5 - MEP to which to attach the remote MEP doesn't exist
//6 - A remote MEP is already using this MEP ID
//7 - Already reached maximum number of MEs in this MEP
int wr_rmep(u32 i_mep, u32 i_rmep, T_RMEP *p_rmep, const T_MEP_HDR *p_mep, T_ETH_SRV_OAM *p_oam) {
u32 i_look_r;
T_MEP   *_p_mep;
T_ME    *_p_me;
T_MEP_DB *p_mep_db;
T_LOOKUP_MEP *p_mep_lut;


 if (invalid_T_MEP_HDR((T_MEP_HDR *)p_rmep) || !valid_rmep_index(i_rmep))   return 1;
 if (invalid_T_MEP_HDR(p_mep)               || !valid_mep_index(i_mep))     return 2;

 if (0!=memcmp(&p_rmep->meg_id, &p_mep->meg_id, sizeof(T_MEG_ID)))  return 3;
 if (p_rmep->vid    !=  p_mep->vid)     return 3;
 if (p_rmep->level  !=  p_mep->level)   return 3;
 if (p_rmep->tmout  !=  p_mep->tmout)   return 3;
 if (p_rmep->prt    !=  p_mep->prt)     return 3;

 p_mep_db=  p_oam->db;
 p_mep_lut= p_oam->mep_lut;

 //Check RMEP entry in lookup table...
 i_look_r=  finger_lut_index(1, p_mep->prt, p_rmep->vid, p_rmep->level, 1, p_rmep->mep_id, i_mep, i_rmep, p_mep_lut, NULL, NULL);
 if (MEP_INDEX_TO_iMEP(i_look_r)!=i_mep)            return 4;
 if (MEP_INDEX_TO_iRMEP(i_look_r)!=i_rmep)          return 4;

 _p_mep= &p_mep_db[i_mep].mep;

 //for (i_rmep=0; i_rmep<N_MAX_MEs_PER_MEP; i_rmep++) {
     _p_me=   &_p_mep->ME[i_rmep];
 //    if (EMPTY_T_MEP(*_p_me)) break;
 //}
 //if (!valid_rmep_index(i_rmep)) return 7;

 //Fill T_MEP table...
 _p_mep->ME[i_rmep].mep_id=         p_rmep->mep_id;

 return 0;
}//wr_rmep




//Return status:
//0 - OK
//1 - Invalid index
int del_rmep(u32 i_mep, u32 i_rmep, T_ETH_SRV_OAM *p_oam) {
u32 i_look_r;
T_MEP   *_p_mep;
T_ME    *_p_me;
T_MEP_DB *p_mep_db;
T_LOOKUP_MEP *p_mep_lut;


 if (!valid_mep_index(i_mep) ||!valid_rmep_index(i_rmep))   return 1;

 p_mep_db=  p_oam->db;
 p_mep_lut= p_oam->mep_lut;

 _p_mep=&p_mep_db[i_mep].mep;
 _p_me= &_p_mep->ME[i_rmep];

 //Delete entry in lookup table...
 i_look_r=  finger_lut_index(2, _p_mep->prt, _p_mep->vid, _p_mep->level, 1, _p_me->mep_id, -1, -1, p_mep_lut, NULL, NULL);

 //Remove entry in T_MEP table
 memset(_p_me, 0xff, sizeof(T_ME));

 return 0;
}//del_rmep



































void proc_ethsrv_oam(T_ETH_SRV_OAM *p_oam, u32 T_ms) {
u8 timeout, time_2_send_ccms, time_2_send_csf;
u32 i, tmout, n_rmeps;
T_MEP       *_p_mep;
T_MEP_DB    *p_mep_db;
T_MEP_CSF   *_p_mep_csf;
T_MEP_LM    *_p_mep_lm;

u16 *proc_i_mep;
static u32 j, meps_procssd_per_function_call=0;


 p_mep_db=      p_oam->db;
 proc_i_mep=    &p_oam->proc_i_mep;

 if (0==meps_procssd_per_function_call) {
     meps_procssd_per_function_call=    N_MEPs*T_ms/MEP_MIN_T_ms;
     if (0 != N_MEPs*T_ms % MEP_MIN_T_ms)    meps_procssd_per_function_call++; //meps_procssd_per_function_call=ceil()...
    
     if (meps_procssd_per_function_call > N_MEPs)    meps_procssd_per_function_call=N_MEPs;
 }
 T_ms= T_ms*N_MEPs/meps_procssd_per_function_call;

 for (j=meps_procssd_per_function_call; j; j--) {
    if (++*proc_i_mep>=N_MEPs) *proc_i_mep=0;
    
    _p_mep= &p_mep_db[*proc_i_mep].mep;             //Get the pointer to this MEP,...
    
    //Check if it's time to send CCMs on this MEP...
    if (!valid_oam_tmr(_p_mep->tmout)) continue; //return;
    tmout= OAM_TMR_CODE_TO_ms[_p_mep->tmout];// % N_OAM_TMR_VALUES];
    _p_mep->CCM_timer += T_ms;
    if (_p_mep->CCM_timer+T_ms/2 > tmout) {time_2_send_ccms=1;   _p_mep->CCM_timer=0;} //if (_p_mep->CCM_timer >= tmout)
    else time_2_send_ccms=0;
    
    //Process this MEP's remote MEPs on every ME...
    for (i=0, n_rmeps=0, timeout=0; i<N_MAX_MEs_PER_MEP; i++) {
        if (!valid_mep_id(_p_mep->ME[i].mep_id))   continue;
        n_rmeps++;
    
        if (_p_mep->ME[i].LOC_timer*2 < tmout*7)   _p_mep->ME[i].LOC_timer += T_ms; //3.5*tmout
        else {//if (_p_mep->ME[i].LOC_timer*2 >= tmout*7) {
            if (_p_mep->ME[i].LOC_timer!=0L-1) {
                ethsrv_oam_register_connection_loss((u8*)&_p_mep->meg_id, _p_mep->mep_id, _p_mep->ME[i].mep_id, _p_mep->prt, _p_mep->vid);
                ETHSRV_OAM_LOG("Connectivity MEP %u to RMEP %u lost"NLS, _p_mep->mep_id, _p_mep->ME[i].mep_id);
                if (_p_mep->ME[i].RDI) {
                     ethsrv_oam_register_RDI_END((u8*)&_p_mep->meg_id, _p_mep->mep_id, _p_mep->ME[i].mep_id, _p_mep->prt, _p_mep->vid);
                     _p_mep->ME[i].RDI=0;
                }
            }
            _p_mep->ME[i].LOC_timer=   0L-1;
            timeout=1;
        }
    }//for

    if (MISMERGE(_p_mep->mismerge_timer,tmout)) _p_mep->mismerge_timer += T_ms;
    else _p_mep->mismerge_timer=    0L-1;

    if (UNXP_MEP(_p_mep->unxp_MEP_timer,tmout)) _p_mep->unxp_MEP_timer += T_ms;
    else _p_mep->unxp_MEP_timer=    0L-1;

    if (UNXP_LVL(_p_mep->unxp_lvl_timer,tmout)) _p_mep->unxp_lvl_timer += T_ms;
    else _p_mep->unxp_lvl_timer=    0L-1;

    if (UNXP_T(_p_mep->unxp_T_timer,tmout)) _p_mep->unxp_T_timer += T_ms;
    else _p_mep->unxp_T_timer=    0L-1;
    
    //RDI determination
    if (time_2_send_ccms)
        send_ccm(_p_mep->prt, (T_MEP_HDR *)_p_mep, timeout || !n_rmeps, 1);
    
    //Process AIS...
    //if (!timeout) continue; //return;








    // CSF Function --------------------------------------------------------------------
    _p_mep_csf =    &p_oam->db[*proc_i_mep].mep_csf; //Get the pointer to this MEP,...

    if (_p_mep_csf->en1dis0) {
        //Check if it's time to send CSFs on this MEP...
        if (!valid_oam_tmr(_p_mep_csf->period)) goto _proc_ethsrv_oam_CSF_function_end;
        tmout= OAM_TMR_CODE_TO_ms[_p_mep_csf->period];
        _p_mep_csf->CSF_timer += T_ms;
        if (_p_mep_csf->CSF_timer+T_ms/2 > tmout) {time_2_send_csf=1;   _p_mep_csf->CSF_timer=0;}
        else time_2_send_csf=0;

        if (time_2_send_csf)
            send_csf(_p_mep->prt, (T_MEP_HDR *)_p_mep, _p_mep_csf->period, _p_mep_csf->CSF_Tx_en_flags);

        // Check for end of C-LOS
        if (_p_mep_csf->LOS_timer*2 < tmout*7)   _p_mep_csf->LOS_timer += T_ms; //3.5*tmout
        else {
            if (_p_mep_csf->LOS_timer!=0L-1) {
                ETHSRV_OAM_LOG("MEP %u: End of C-LOS"NLS, _p_mep->mep_id);
            }
            _p_mep_csf->LOS_timer=   0L-1;
        }
    }
_proc_ethsrv_oam_CSF_function_end:
    // -------------------------------------------------------------------- CSF Function









    // LM Function --------------------------------------------------------------------
    _p_mep_lm =   &p_oam->db[*proc_i_mep].lm; //Get the pointer to this MEP,...

    if (1==_p_mep_lm->CCMs0_LMMR1) {
        //Check if it's time to send LMMs on this MEP...
        if (!valid_oam_tmr(_p_mep_lm->period)) goto _proc_ethsrv_oam_LM_function_end;
        tmout= OAM_TMR_CODE_TO_ms[_p_mep_lm->period];
        _p_mep_lm->LMM_timer += T_ms;
        if (_p_mep_lm->LMM_timer+T_ms/2 > tmout) {//time_2_send_lmm=1;
            _p_mep_lm->LMM_timer=0;
            send_lmm(*proc_i_mep, (T_MEP_HDR *)_p_mep, _p_mep_lm);
        }
    }
_proc_ethsrv_oam_LM_function_end:;
    // -------------------------------------------------------------------- LM Function
 }//for (j=meps_procssd_per_function_call; j; j--)
}//proc_ethsrv_oam







// u8  en1dis0;            // Enable or disable CSF function
// u8  period;             // ETH-CSF transmission period
int MEP_csf_admin(u16 mep_idx, u8 en1dis0, u8 period, T_ETH_SRV_OAM *p_oam)
{
    T_MEP_CSF   *_p_mep_csf;
    T_MEP_DB    *p_mep_csf_db;

    if (!valid_mep_index(mep_idx))          return 1;
    if (!valid_oam_tmr(period))             return 2;               //if (en1dis0 && period!=4 && period!=6)  return 2;   //Y.1731, 9.21, table 9-6

    p_mep_csf_db =  p_oam->db;
    _p_mep_csf =    &p_mep_csf_db[mep_idx].mep_csf; //Get the pointer to this MEP,...

    init_mep_csf(_p_mep_csf);

    _p_mep_csf->en1dis0 =   en1dis0;
    _p_mep_csf->period =    period;
    
    return 0;
}



// CSF Function and Link Fault Pass-through (LFP) integration
int MEP_enable_Tx_CSF(u16 mep_idx, u8 CSF_tx_flags, T_ETH_SRV_OAM *p_oam)
{
    T_MEP_CSF   *_p_mep_csf;
    T_MEP_DB    *p_mep_csf_db;

    p_mep_csf_db =  p_oam->db;
    _p_mep_csf =    &p_mep_csf_db[mep_idx].mep_csf; //Get the pointer to this MEP,...

    if (_p_mep_csf->en1dis0 && _p_mep_csf->CSF_Tx_en_flags != CSF_tx_flags) {
        _p_mep_csf->CSF_Tx_en_flags = CSF_tx_flags;
        _p_mep_csf->CSF_timer=0x3fffffffUL;         //Force a quick transition
        //"CSF_TYPE_INVALID" works like CSF_TYPE_DCI but suppresses sending CSF frames; for a quicker convergence, we should send a CSF_TYPE_DCI
        if (CSF_TYPE_INVALID==CSF_tx_flags) send_csf(p_oam->db[mep_idx].mep.prt, (T_MEP_HDR *)&p_oam->db[mep_idx].mep, _p_mep_csf->period, CSF_TYPE_DCI);
    }

    return 0;
}

int MEP_is_CC_LOC_or_RDI(u16 mep_idx, T_ETH_SRV_OAM *p_oam)
{
    T_MEP   *_p_mep;
    T_MEP_DB *p_mep_db;
    u32     i;

    p_mep_db =  p_oam->db;
    _p_mep =    &p_mep_db[mep_idx].mep; //Get the pointer to this MEP,...


    for 
        (i=0; i<N_MAX_MEs_PER_MEP; i++) {
        if (!valid_mep_id(_p_mep->ME[i].mep_id))   continue;

        if (LOC(_p_mep->ME[i].LOC_timer, OAM_TMR_CODE_TO_ms[_p_mep->tmout])) return 1;
        if (_p_mep->ME[i].RDI) return 1;
    }//for
    return 0;   // _p_mep->LOC
}

int MEP_is_CSF_LOS(u16 mep_idx, T_ETH_SRV_OAM *p_oam)
{
    T_MEP_CSF   *_p_mep_csf;
    T_MEP_DB    *p_mep_csf_db;

    p_mep_csf_db =  p_oam->db;
    _p_mep_csf =    &p_mep_csf_db[mep_idx].mep_csf; //Get the pointer to this MEP,...

    if (_p_mep_csf->en1dis0) {
        if (_p_mep_csf->LOS_timer == (0L-1))    return 0;   // OK
        else                                    return 1;   // in C-LOS
    }

    return 0;
}










static int send_ccm(u16 oam_prt, T_MEP_HDR *p_mep, u8 RDI, u8 use_mcast_DMAC) {
T_ETH_OAM_MAC DMAC;
ETH_SRV_OAM_CCM_DATAGRM ccm, *p_ccm;

 DMAC=  OAM_MC_MAC;
 //pkt[5] &=                      ~0x07;
 DMAC.byte[5] |=                p_mep->level;// & 0x07;

 p_ccm=                         &ccm;
 p_ccm->MAlevel_and_version=    ASSEMBLE_OAM_MALEVEL_AND_VERSION(p_mep->level, OAM_PROTO_VERSION);
 p_ccm->opcode=                 CCM_OPCODE;
 p_ccm->flags=                  ASSEMBLE_CCM_flags(RDI,p_mep->tmout);
 p_ccm->TLV_offset=             70; // (offsetof(ETH_SRV_OAM_CCM_DATAGRM,end_TLV) - (offsetof(ETH_SRV_OAM_CCM_DATAGRM,TLV_offset) + sizeof(p_ccm->TLV_offset)));
 p_ccm->meg_id=                 p_mep->meg_id;
 p_ccm->SeqNumb=                0;
 p_ccm->mep_id=                 htons(p_mep->mep_id);
 p_ccm->TxFCf=  p_ccm->TxFCb=   p_ccm->RxFCb=0;
 p_ccm->reserved=               0;
 p_ccm->end_TLV=                0;

 return send_eth_pckt(oam_prt, p_mep->up1_down0, (u8*)p_ccm, sizeof(ETH_SRV_OAM_CCM_DATAGRM), p_mep->vid, p_mep->prior, p_mep->CoS, 0, OAM_ETH_TYPE, DMAC.byte);
}//send_ccm




















static int send_csf(u16 oam_prt, T_MEP_HDR *p_mep, u8 CSF_period, u8 CSF_flags) {
T_ETH_OAM_MAC DMAC;
ETH_SRV_OAM_CSF_DATAGRM csf, *p_csf;

 if (CSF_flags == CSF_TYPE_INVALID) {
     return 0;
 }

 DMAC=  OAM_MC_MAC;
 //pkt[5] &=                      ~0x07;
 DMAC.byte[5] |=                p_mep->level;// & 0x07;

 p_csf=                         &csf;
 p_csf->MAlevel_and_version=    ASSEMBLE_OAM_MALEVEL_AND_VERSION(p_mep->level, OAM_PROTO_VERSION);
 p_csf->opcode=                 CSF_OPCODE;
 p_csf->flags=                  ASSEMBLE_CSF_flags(CSF_flags, CSF_period);
 p_csf->TLV_offset=             0;
 p_csf->info[0]=                0;//End TLV

 return send_eth_pckt(oam_prt, p_mep->up1_down0, (u8*)p_csf, sizeof(ETH_SRV_OAM_CSF_DATAGRM), p_mep->vid, p_mep->prior, p_mep->CoS, 0, OAM_ETH_TYPE, DMAC.byte);
}//send_csf




















static int send_lmm(u16 i_mep, T_MEP_HDR *p_mep, T_MEP_LM *p_lm) {
T_ETH_OAM_MAC DMAC;
ETH_LMM_OAM_DATAGRM lmm, *p_lmm;

 DMAC=  OAM_MC_MAC;
 //pkt[5] &=                      ~0x07;
 DMAC.byte[5] |=                p_mep->level;// & 0x07;

 p_lmm=                         &lmm;
 p_lmm->MAlevel_and_version=    ASSEMBLE_OAM_MALEVEL_AND_VERSION(p_mep->level, OAM_PROTO_VERSION);
 p_lmm->opcode=                 LMM_OPCODE;
 p_lmm->flags=                  0;
 p_lmm->TLV_offset=             12;
 //p_lmm->RxFCf= p_lmm->TxFCb=    0;
 p_lmm->end_TLV=                0;
 p_lmm->TxFCf=                  htonl(rd_TxFCl(i_mep));

 return send_eth_pckt(p_mep->prt, p_mep->up1_down0, (u8*)p_lmm, sizeof(ETH_LMM_OAM_DATAGRM), p_mep->vid, p_mep->prior, p_mep->CoS, 0, OAM_ETH_TYPE, DMAC.byte);
}//send_lmm




















static int send_lmr(u16 i_mep, T_MEP_HDR *p_mep, ETH_LMM_OAM_DATAGRM *p_lmm, u8 *pDMAC) {
ETH_LMR_OAM_DATAGRM lmr, *p_lmr;

 p_lmr=                         &lmr;
 p_lmr->MAlevel_and_version=    ASSEMBLE_OAM_MALEVEL_AND_VERSION(p_mep->level, OAM_PROTO_VERSION);
 p_lmr->opcode=                 LMR_OPCODE;
 p_lmr->flags=                  0;
 p_lmr->TLV_offset=             12;
 p_lmr->TxFCf=                  p_lmm->TxFCf;
 p_lmr->RxFCf=                  p_lmm->RxFCf;   //Our side set this on the PDU, @LMM reception
 p_lmr->end_TLV=                0;
 p_lmr->TxFCb=                  htonl(rd_TxFCl(i_mep));

 return send_eth_pckt(p_mep->prt, p_mep->up1_down0, (u8*)p_lmr, sizeof(ETH_LMR_OAM_DATAGRM), p_mep->vid, p_mep->prior, p_mep->CoS, 0, OAM_ETH_TYPE, pDMAC);
}//send_lmr


















//Uses (and changes both opcode, SMAC, DMAC values) of the buffer pointed by p_lbr
static int send_lbr(u16 oam_prt, T_MEP_HDR *p_mep, ETH_LBR_OAM_DATAGRM *p_lbr, u16 lbr_len, u8 *pDMAC) {
    p_lbr->opcode=LBR_OPCODE;
    return send_eth_pckt(oam_prt, p_mep->up1_down0, (u8*)p_lbr, lbr_len, p_mep->vid, p_mep->prior, p_mep->CoS, 0, OAM_ETH_TYPE, pDMAC);
}//send_lbr


















#define v2008(i_eg_id_tlv)  ((i_eg_id_tlv)<9600)
/*
static int send_ltr(u16 oam_prt, T_MEP_HDR *p_mep, ETH_LTM_OAM_DATAGRM *p_ltm, u16 ltr_len, u8 *pDMAC) {
//u32 i_eg_id_tlv;
ETH_LTR_OAM_DATAGRM ltr;
T_LTR_EG_ID_TLV     *p_eg_id;
T_RPL_IN_ID_TLV     *p_rpl_id;

 ltr.MAlevel_and_version=   ASSEMBLE_OAM_MALEVEL_AND_VERSION(p_mep->level, OAM_PROTO_VERSION);
 ltr.opcode=                LTR_OPCODE;
 ltr.flags=                 ASSEMBLE_LTR_flags(LTM_flags_TO_HWonly(p_ltm->flags),FwdYes,TerminalMEP);
 ltr.TLV_offset=            offsetof(ETH_LTR_OAM_DATAGRM,tlvs)-offsetof(ETH_LTR_OAM_DATAGRM,TLV_offset)-1;//6;
 ltr.transID=               p_ltm->transID;
 ltr.TTL=                   p_ltm->TTL-1;
 ltr.rel_act=               rel_act;

 if (v2008(i_eg_id_tlv)) {
     p_eg_id=                   ltr.tlvs.nxt_TLV;
     p_eg_id->type=             LTR_EG_ID_TLV_TYPE;
     p_eg_id->len=              htons(TLV_LENGTH(T_LTR_EG_ID_TLV));

     {
      T_LTM_EG_ID_TLV *ltm_eg_id;

         ltm_eg_id= (T_LTM_EG_ID_TLV *) &((u8*)p_ltm)[i_eg_id_tlv];
         memcpy(&p_eg_id->last, &ltm_eg_id->v, sizeof(ltm_eg_id->v));
     }

     p_rpl_id=                  p_eg_id+sizeof(T_LTR_EG_ID_TLV);
 }
 else { //v2006
     p_rpl_id=                  ltr.tlvs.nxt_TLV;
 }

 p_rpl_id->type=            REPLY_IN_TLV_TYPE   REPLY_EG_TLV_TYPE
 p_rpl_id->len=             htons(TLV_LENGTH(T_RPL_IN_ID_TLV));
 p_rpl_id-

 return 0;
}//send_ltr
*/



















int rx_oam_pckt(u16 oam_prt, u8 *pkt_ethtype, u32 pkt_len, u64 vid, u8 *pSMAC, T_ETH_SRV_OAM *p_oam, u64 RxFCl) {
ETH_SRV_OAM_DATAGRM *p_oam_d;
T_MEP_DB        *p_mep_db;
T_LOOKUP_MEP    *p_mep_lut;

 if (ntohs(*((u16 *) pkt_ethtype))    !=  OAM_ETH_TYPE)   return 1;

 if (oam_prt>=N_OAM_PRTS) return 2;

 p_oam_d= (ETH_SRV_OAM_DATAGRM *) &pkt_ethtype[2];

 if (OAM_PROTO_VERSION  !=  MALEVEL_AND_VERSION_TO_VERSION(p_oam_d->MAlevel_and_version)) {
     //Different version protocol
     return 3;
 }

 switch (p_oam_d->opcode) {
 default:           return 4;
 case CCM_OPCODE:
     if (pkt_len<sizeof(ETH_SRV_OAM_CCM_DATAGRM)+2)  return 5;
     p_mep_db=  p_oam->db;
     p_mep_lut= p_oam->mep_lut;
     //ETHSRV_OAM_LOG("rx_ccm=%d"NLS,
            rx_ccm(oam_prt, pkt_ethtype, pkt_len, vid, pSMAC, p_mep_db, p_mep_lut);
            //);
     return 0;
 case CSF_OPCODE:
     if (pkt_len<sizeof(ETH_SRV_OAM_DATAGRM)+2)  return 5;
     p_mep_db=  p_oam->db;
     p_mep_lut= p_oam->mep_lut;
     rx_csf(oam_prt, pkt_ethtype, pkt_len, vid, pSMAC, p_mep_db, p_mep_lut);
     return 0;
 case LMM_OPCODE:
     if (pkt_len<sizeof(ETH_LMM_OAM_DATAGRM)+2)  return 5;
     p_mep_db=  p_oam->db;
     p_mep_lut= p_oam->mep_lut;
     rx_lmm(oam_prt, pkt_ethtype, pkt_len, vid, pSMAC, p_mep_db, p_mep_lut, RxFCl);
     return 0;
 case LMR_OPCODE:
     if (pkt_len<sizeof(ETH_LMR_OAM_DATAGRM)+2)  return 5;
     p_mep_db=  p_oam->db;
     p_mep_lut= p_oam->mep_lut;
     rx_lmr(oam_prt, pkt_ethtype, pkt_len, vid, pSMAC, p_mep_db, p_mep_lut, RxFCl);
     return 0;
 case LBM_OPCODE:
     if (pkt_len<sizeof(ETH_LBM_OAM_DATAGRM)+2)  return 5;
     p_mep_db=  p_oam->db;
     p_mep_lut= p_oam->mep_lut;
     rx_lbm(oam_prt, pkt_ethtype, pkt_len, vid, pSMAC, p_mep_db, p_mep_lut);
     return 0;
 case LTM_OPCODE:
     if (pkt_len<sizeof(ETH_LTM_OAM_DATAGRM)+2)  return 5;
     p_mep_db=  p_oam->db;
     p_mep_lut= p_oam->mep_lut;
     rx_ltm(oam_prt, pkt_ethtype, pkt_len, vid, pSMAC, p_mep_db, p_mep_lut);
     return 0;

 //case LBR_OPCODE:
 //case LTR_OPCODE:
 }
}//rx_oam_pckt








static int rx_ccm(u16 oam_prt, u8 *pkt_ethtype, u32 pkt_len, u64 vid, u8 *pSMAC,
                    T_MEP_DB *p_mep_db, T_LOOKUP_MEP *p_mep_lut) {
ETH_SRV_OAM_CCM_DATAGRM *p_ccm;
T_MEP   *_p_mep;
u32 i_look_r, i_mep, i_rmep;
int i;
u8 unxlvl0_msmrg1_unxmep2_unxmeppotentloop3_unxperiod4; u32 alrm_index;
u16 ccm_mep_id;


 p_ccm= (ETH_SRV_OAM_CCM_DATAGRM *) &pkt_ethtype[2];
 ccm_mep_id= ntohs(p_ccm->mep_id);

 i_look_r=  finger_lut_index(0, oam_prt, vid, MALEVEL_AND_VERSION_TO_MALEVEL(p_ccm->MAlevel_and_version), 1, ccm_mep_id, -1 , -1, p_mep_lut, &unxlvl0_msmrg1_unxmep2_unxmeppotentloop3_unxperiod4, &alrm_index);

 i_mep= MEP_INDEX_TO_iMEP(i_look_r);
 i_rmep=MEP_INDEX_TO_iRMEP(i_look_r);
 
 if (!valid_mep_index(i_mep) || !valid_rmep_index(i_rmep)) {
     i_mep= MEP_INDEX_TO_iMEP(alrm_index);
     i_rmep=MEP_INDEX_TO_iRMEP(alrm_index);
     
     if (!valid_mep_index(i_mep)) {ethsrv_oam_register_mismerge(&p_ccm->meg_id, ccm_mep_id, 0xffff, oam_prt, vid);   return 1;}
 }

 _p_mep=    &p_mep_db[i_mep].mep;

 i= _p_mep->level - MALEVEL_AND_VERSION_TO_MALEVEL(p_ccm->MAlevel_and_version);
 if (/*0==unxlvl0_msmrg1_unxmep2_unxmeppotentloop3_unxperiod4 ||*/  i>0) {
     //REGISTER "UNEXPECTED MEG LEVEL"
     if (!UNXP_LVL(_p_mep->unxp_lvl_timer, OAM_TMR_CODE_TO_ms[_p_mep->tmout]))  {
         ethsrv_oam_register_LVL(&p_ccm->meg_id, ccm_mep_id, i_mep, oam_prt, vid, MALEVEL_AND_VERSION_TO_MALEVEL(p_ccm->MAlevel_and_version));
     }
     _p_mep->unxp_lvl_timer=0;
     return 2;
 }
 else
 if (i<0) {
     //FWD this packet to every other port on the same vlan
     return 3;
 }

 if (1==unxlvl0_msmrg1_unxmep2_unxmeppotentloop3_unxperiod4 ||  0!=memcmp(&_p_mep->meg_id, &p_ccm->meg_id, sizeof(_p_mep->meg_id))) {
     //REGISTER "MISMERGE"
     if (!MISMERGE(_p_mep->mismerge_timer, OAM_TMR_CODE_TO_ms[_p_mep->tmout]))  {
         ethsrv_oam_register_mismerge(&p_ccm->meg_id, ccm_mep_id, i_mep, oam_prt, vid);
     }
     _p_mep->mismerge_timer=0;
     //ETHSRV_OAM_LOG("REGISTER MISMERGE"NLS);
     return 4;
 }

 if (2==unxlvl0_msmrg1_unxmep2_unxmeppotentloop3_unxperiod4 /*|| _p_mep->mep_id!=ccm_mep_id*/) {
     if (!UNXP_MEP(_p_mep->unxp_MEP_timer, OAM_TMR_CODE_TO_ms[_p_mep->tmout]))  {
         ethsrv_oam_register_unexpected_MEP_id(&p_ccm->meg_id, ccm_mep_id, i_mep, oam_prt, vid);
     }
     _p_mep->unxp_MEP_timer=0;
     return 5;
 }

 if (3==unxlvl0_msmrg1_unxmep2_unxmeppotentloop3_unxperiod4) {
     ethsrv_oam_register_unexpected_MEP_potential_loop(&p_ccm->meg_id, ccm_mep_id, i_mep, oam_prt, vid);
     _p_mep->unxp_MEP_timer=0;
     return 5;
 }
 


 //...on a registered MEP

 //_p_mep=    &p_mep_db[i_mep];


 if (_p_mep->tmout != flags_TO_TMOUT(p_ccm->flags) || 4==unxlvl0_msmrg1_unxmep2_unxmeppotentloop3_unxperiod4) {
     //REGISTER "UNEXPECTED PERIOD"
     if (!UNXP_LVL(_p_mep->unxp_T_timer, OAM_TMR_CODE_TO_ms[_p_mep->tmout]))  {
         ethsrv_oam_register_T(&p_ccm->meg_id, ccm_mep_id, i_mep, oam_prt, vid, flags_TO_TMOUT(p_ccm->flags));
     }
     _p_mep->unxp_T_timer=0;
     return 6;
 }

 //if (_p_mep->prt!=oam_prt) return 0;  //if (!_p_mep->prt_msk[oam_prt]) return 0; //Should only be done if articulated with protection at reception

 if (!valid_rmep_index(i_rmep)) return 7;



 //Finally, we update our state variables
 if (_p_mep->ME[i_rmep].LOC_timer==0L-1) {
     if (flags_TO_RDI(p_ccm->flags)) {
         ethsrv_oam_register_receiving_RDI((u8*)&_p_mep->meg_id, _p_mep->mep_id, ccm_mep_id, oam_prt, _p_mep->vid);
         ETHSRV_OAM_LOG("MEP %u receiving RDI from RMEP %u"NLS, _p_mep->mep_id, ccm_mep_id);
     }
     else {
         ethsrv_oam_register_connection_restored((u8*)&_p_mep->meg_id, _p_mep->mep_id, ccm_mep_id, oam_prt, _p_mep->vid);
         ETHSRV_OAM_LOG("Connectivity MEP %u to RMEP %u restored"NLS, _p_mep->mep_id, ccm_mep_id);
     }
 }
 else
 if (!_p_mep->ME[i_rmep].RDI && flags_TO_RDI(p_ccm->flags)) {
     ethsrv_oam_register_receiving_RDI((u8*)&_p_mep->meg_id, _p_mep->mep_id, ccm_mep_id, oam_prt, _p_mep->vid);
     ETHSRV_OAM_LOG("MEP %u receiving RDI from RMEP %u"NLS, _p_mep->mep_id, ccm_mep_id);
 }
 else
 if (_p_mep->ME[i_rmep].RDI && !flags_TO_RDI(p_ccm->flags)) {
     ethsrv_oam_register_RDI_END((u8*)&_p_mep->meg_id, _p_mep->mep_id, ccm_mep_id, oam_prt, _p_mep->vid);
     ethsrv_oam_register_connection_restored((u8*)&_p_mep->meg_id, _p_mep->mep_id, ccm_mep_id, oam_prt, _p_mep->vid);
     ETHSRV_OAM_LOG("MEP %u stopped receiving RDI from RMEP %u"NLS, _p_mep->mep_id, ccm_mep_id);
 }

 _p_mep->ME[i_rmep].LOC_timer=0;
 _p_mep->ME[i_rmep].RDI= flags_TO_RDI(p_ccm->flags);

 //p_mep_lut[i_look_r].SMAC=  *((T_ETH_OAM_MAC *) pSMAC);
 return 0;
}//rx_ccm








static int rx_csf(u16 oam_prt, u8 *pkt_ethtype, u32 pkt_len, u64 vid, u8 *pSMAC,
                    T_MEP_DB *p_mep_db, T_LOOKUP_MEP *p_mep_lut) {
ETH_SRV_OAM_DATAGRM *p_csf;
T_MEP       *_p_mep;
T_MEP_CSF   *_p_mep_csf;
u32 i_look_r, i_mep;
int i;
u8 unxlvl0_msmrg1_unxmep2_unxmeppotentloop3_unxperiod4; u32 alrm_index;


 p_csf= (ETH_SRV_OAM_DATAGRM *) &pkt_ethtype[2];

 i_look_r=  finger_lut_index(0, oam_prt, vid, MALEVEL_AND_VERSION_TO_MALEVEL(p_csf->MAlevel_and_version), 0,
                             -1, //CSF packet doesn't bring any MEP_ID...
                             -1 , -1, p_mep_lut, &unxlvl0_msmrg1_unxmep2_unxmeppotentloop3_unxperiod4, &alrm_index);

 i_mep= MEP_INDEX_TO_iMEP(i_look_r);    //...so this MEP_ID (-1) won't be found...
 //i_rmep=MEP_INDEX_TO_iRMEP(i_look_r);
 
 if (!valid_mep_index(i_mep) /*|| !valid_rmep_index(i_rmep)*/) {
     i_mep= MEP_INDEX_TO_iMEP(alrm_index);      //...and we'll have to deal just with alarms: the MEP in a certain port, VID and level (alarm: unexpected MEP)
     //i_rmep=MEP_INDEX_TO_iRMEP(alrm_index);
     
     if (!valid_mep_index(i_mep)) return 1;

     _p_mep=        &p_mep_db[i_mep].mep;
     _p_mep_csf=    &p_mep_db[i_mep].mep_csf;

     i= _p_mep->level - MALEVEL_AND_VERSION_TO_MALEVEL(p_csf->MAlevel_and_version);
     if (/*0==unxlvl0_msmrg1_unxmep2_unxmeppotentloop3_unxperiod4 ||*/  i>0) {
         return 2;
     }
     else
     if (i<0) {
         //FWD this packet to every other port on the same vlan
         return 3;
     }

     if (2==unxlvl0_msmrg1_unxmep2_unxmeppotentloop3_unxperiod4) {
       if (_p_mep_csf->period != CSF_flags_TO_CSF_TMOUT(p_csf->flags)) {
         //REGISTER "UNEXPECTED PERIOD"
       }

       //Finally, we update our state variables
       switch (CSF_flags_TO_CSF_TYPE(p_csf->flags)) {
       case CSF_TYPE_LOS:       _p_mep_csf->LOS_timer=0;    break;
       //case CSF_TYPE_FDI_AIS:
       //case CSF_TYPE_RDI:
       case CSF_TYPE_DCI:       _p_mep_csf->LOS_timer=0x3fffffffUL; break;      //Force a quick transition
       }

       return 0;
     }
 }
 return 0;
}//rx_csf







static int rx_lmm(u16 oam_prt, u8 *pkt_ethtype, u32 pkt_len, u64 vid, u8 *pSMAC,
                    T_MEP_DB *p_mep_db, T_LOOKUP_MEP *p_mep_lut, u64 RxFCl) {
ETH_LMM_OAM_DATAGRM *p_lmm;
T_MEP       *_p_mep;
u32 i_look_r, i_mep;
int i;
u8 unxlvl0_msmrg1_unxmep2_unxmeppotentloop3_unxperiod4; u32 alrm_index;


 p_lmm= (ETH_LMM_OAM_DATAGRM *) &pkt_ethtype[2];

 i_look_r=  finger_lut_index(0, oam_prt, vid, MALEVEL_AND_VERSION_TO_MALEVEL(p_lmm->MAlevel_and_version), 0,
                             -1, //LMM packet doesn't bring any MEP_ID...
                             -1 , -1, p_mep_lut, &unxlvl0_msmrg1_unxmep2_unxmeppotentloop3_unxperiod4, &alrm_index);

 i_mep= MEP_INDEX_TO_iMEP(i_look_r);    //...so this MEP_ID (-1) won't be found...
 //i_rmep=MEP_INDEX_TO_iRMEP(i_look_r);
 
 if (!valid_mep_index(i_mep) /*|| !valid_rmep_index(i_rmep)*/) {
     i_mep= MEP_INDEX_TO_iMEP(alrm_index);      //...and we'll have to deal just with alarms: the MEP in a certain port, VID and level (alarm: unexpected MEP)
     //i_rmep=MEP_INDEX_TO_iRMEP(alrm_index);
     
     if (!valid_mep_index(i_mep)) return 1;

     _p_mep=        &p_mep_db[i_mep].mep;

     i= _p_mep->level - MALEVEL_AND_VERSION_TO_MALEVEL(p_lmm->MAlevel_and_version);
     if (/*0==unxlvl0_msmrg1_unxmep2_unxmeppotentloop3_unxperiod4 ||*/  i>0) {
         return 2;
     }
     else
     if (i<0) {
         //FWD this packet to every other port on the same vlan
         return 3;
     }

     if (2!=unxlvl0_msmrg1_unxmep2_unxmeppotentloop3_unxperiod4) return 0;


     //Response to an LMM
     p_lmm->RxFCf = htonl(RxFCl);
     send_lmr(i_mep, (T_MEP_HDR *)_p_mep, (ETH_LMM_OAM_DATAGRM *)p_lmm, pSMAC);
 }
 return 0;
}//rx_lmm







static int rx_lmr(u16 oam_prt, u8 *pkt_ethtype, u32 pkt_len, u64 vid, u8 *pSMAC,
                    T_MEP_DB *p_mep_db, T_LOOKUP_MEP *p_mep_lut, u64 RxFCl) {
ETH_LMR_OAM_DATAGRM *p_lmr;
T_MEP       *_p_mep;
T_MEP_LM    *_p_mep_lm;
u32 i_look_r, i_mep;
int i;
u8 unxlvl0_msmrg1_unxmep2_unxmeppotentloop3_unxperiod4; u32 alrm_index;


 p_lmr= (ETH_LMR_OAM_DATAGRM *) &pkt_ethtype[2];

 i_look_r=  finger_lut_index(0, oam_prt, vid, MALEVEL_AND_VERSION_TO_MALEVEL(p_lmr->MAlevel_and_version), 0,
                             -1, //LMR packet doesn't bring any MEP_ID...
                             -1 , -1, p_mep_lut, &unxlvl0_msmrg1_unxmep2_unxmeppotentloop3_unxperiod4, &alrm_index);

 i_mep= MEP_INDEX_TO_iMEP(i_look_r);    //...so this MEP_ID (-1) won't be found...
 //i_rmep=MEP_INDEX_TO_iRMEP(i_look_r);
 
 if (!valid_mep_index(i_mep) /*|| !valid_rmep_index(i_rmep)*/) {
     i_mep= MEP_INDEX_TO_iMEP(alrm_index);      //...and we'll have to deal just with alarms: the MEP in a certain port, VID and level (alarm: unexpected MEP)
     //i_rmep=MEP_INDEX_TO_iRMEP(alrm_index);
     
     if (!valid_mep_index(i_mep)) return 1;

     _p_mep=        &p_mep_db[i_mep].mep;

     i= _p_mep->level - MALEVEL_AND_VERSION_TO_MALEVEL(p_lmr->MAlevel_and_version);
     if (/*0==unxlvl0_msmrg1_unxmep2_unxmeppotentloop3_unxperiod4 ||*/  i>0) {
         return 2;
     }
     else
     if (i<0) {
         //FWD this packet to every other port on the same vlan
         return 3;
     }

     if (2!=unxlvl0_msmrg1_unxmep2_unxmeppotentloop3_unxperiod4) return 0;


     //LMR processing
     _p_mep_lm=     &p_mep_db[i_mep].lm;
     if (1!=_p_mep_lm->CCMs0_LMMR1) return 0;
     {
      T_LM *p;

        //last photo
        p = &_p_mep_lm->lm[iLMlast(++_p_mep_lm->tog_iLM)];
        p->tx      = ntohl(p_lmr->TxFCf);
        p->rx_peer = ntohl(p_lmr->RxFCf);
        p->tx_peer = ntohl(p_lmr->TxFCb);
        p->rx      = RxFCl;

        {//if iLM0 still unwritten (1st photo)...
         u8 *p2;
            p2 = (u8*)&_p_mep_lm->lm[iLM0];
            for (i=0; i<sizeof(T_LM); i++) if (T_LM_VIRGIN_PATTERN!=p2[i]) return 0;

            *(T_LM*)p2 = *p;
        }//...write it
     }
 }
 return 0;
}//rx_lmr














//Far End and/or Near End Frame loss ratio (expressed by dividend and divisor)
//If either NE_flr_Dividend or NE_flr_divisor are NULL pointers, nothing is done to *NE_flr_Dividend and *NE_flr_divisor
//The same applies to FE_flr_*
void LM_frame_loss_ratio(T_LM *lm, T_LM *lm0, u64 *NE_flr_Dividend, u64 *NE_flr_divisor, u64 *FE_flr_Dividend, u64 *FE_flr_divisor) {
u64 a, b;
 if (NULL!=NE_flr_Dividend && NULL!=NE_flr_divisor) {
     a = diff_LM_counters(lm->tx_peer, lm0->tx_peer);
     b = diff_LM_counters(lm->rx, lm0->rx);
     *NE_flr_divisor = a;
     *NE_flr_Dividend = diff_LM_counters(a,b);
 }

 if (NULL!=FE_flr_Dividend && NULL!=FE_flr_divisor) {
     a = diff_LM_counters(lm->tx, lm0->tx);
     b = diff_LM_counters(lm->rx_peer, lm0->rx_peer);
     *FE_flr_divisor = a;
     *FE_flr_Dividend = diff_LM_counters(a,b);
 }
}//LM_frame_loss_ratio




void LM_last_period(T_MEP_LM *p, u64 *NE_flr_Dividend, u64 *NE_flr_divisor, u64 *FE_flr_Dividend, u64 *FE_flr_divisor) {
 LM_frame_loss_ratio(&p->lm[iLMlast(p->tog_iLM)], &p->lm[iLMpenu(p->tog_iLM)], NE_flr_Dividend, NE_flr_divisor, FE_flr_Dividend, FE_flr_divisor);
}




void LM_medium(T_MEP_LM *p, u64 *NE_flr_Dividend, u64 *NE_flr_divisor, u64 *FE_flr_Dividend, u64 *FE_flr_divisor) {
 LM_frame_loss_ratio(&p->lm[iLMlast(p->tog_iLM)], &p->lm[iLM0], NE_flr_Dividend, NE_flr_divisor, FE_flr_Dividend, FE_flr_divisor);
}




int wr_mep_lm(u32 i_mep, T_MEP_LM *p_mep_lm, T_ETH_SRV_OAM *p_oam) {
T_MEP_LM *_p_mep_lm;

 if (!valid_mep_index(i_mep)) return 1;

 if (invalid_T_MEP_LM(p_mep_lm)) return del_mep_lm(i_mep,p_oam);

 if (1==p_mep_lm->CCMs0_LMMR1 && !valid_oam_tmr(p_mep_lm->period)) return 2;

 _p_mep_lm = &p_oam->db[i_mep].lm;
 init_mep_lm(_p_mep_lm);
 _p_mep_lm->CCMs0_LMMR1=    p_mep_lm->CCMs0_LMMR1;
 _p_mep_lm->period=         p_mep_lm->period;

 return 0;
}//wr_mep_lm




int del_mep_lm(u32 i_mep, T_ETH_SRV_OAM *p_oam) {
 if (!valid_mep_index(i_mep)) return 1;
 invalidate_T_MEP_LM(&p_oam->db[i_mep].lm);
 return 0;
}





























static int rx_lbm(u16 oam_prt, u8 *pkt_ethtype, u32 pkt_len, u64 vid, u8 *pSMAC,
                    T_MEP_DB *p_mep_db, T_LOOKUP_MEP *p_mep_lut) {
ETH_LBM_OAM_DATAGRM *p_lbm;
T_MEP       *_p_mep;
u32 i_look_r, i_mep;
int i;
u8 unxlvl0_msmrg1_unxmep2_unxmeppotentloop3_unxperiod4; u32 alrm_index;


 p_lbm= (ETH_LBM_OAM_DATAGRM *) &pkt_ethtype[2];

 i_look_r=  finger_lut_index(0, oam_prt, vid, MALEVEL_AND_VERSION_TO_MALEVEL(p_lbm->MAlevel_and_version), 0,
                             -1, //LBM packet doesn't bring any MEP_ID...
                             -1 , -1, p_mep_lut, &unxlvl0_msmrg1_unxmep2_unxmeppotentloop3_unxperiod4, &alrm_index);

 i_mep= MEP_INDEX_TO_iMEP(i_look_r);    //...so this MEP_ID (-1) won't be found...
 //i_rmep=MEP_INDEX_TO_iRMEP(i_look_r);
 
 if (!valid_mep_index(i_mep) /*|| !valid_rmep_index(i_rmep)*/) {
     i_mep= MEP_INDEX_TO_iMEP(alrm_index);      //...and we'll have to deal just with alarms: the MEP in a certain port, VID and level (alarm: unexpected MEP)
     //i_rmep=MEP_INDEX_TO_iRMEP(alrm_index);
     
     if (!valid_mep_index(i_mep)) return 1;

     _p_mep=        &p_mep_db[i_mep].mep;

     i= _p_mep->level - MALEVEL_AND_VERSION_TO_MALEVEL(p_lbm->MAlevel_and_version);
     if (/*0==unxlvl0_msmrg1_unxmep2_unxmeppotentloop3_unxperiod4 ||*/  i>0) {
         return 2;
     }
     else
     if (i<0) {
         //FWD this packet to every other port on the same vlan
         return 3;
     }

     if (2==unxlvl0_msmrg1_unxmep2_unxmeppotentloop3_unxperiod4) {
       //Response to an LBM
       send_lbr(oam_prt, (T_MEP_HDR *)_p_mep, (ETH_LBM_OAM_DATAGRM *)p_lbm, pkt_len-2, pSMAC);
     }
 }
 return 0;
}//rx_lbm







#define H (sizeof(((T_GEN_TLV *)0)->type) + sizeof(((T_GEN_TLV *)0)->len))
/*
static u32 iTLV_search(ETH_SRV_OAM_DATAGRM *p, u32 oam_len, u8 TLV_type) {
u32 i;
T_GEN_TLV *p2; 
u16 len; 

 //for (i=(u8)&p->TLV_offset-(u8)p +1+ p->TLV_offset; i<oam_len;) {
 for (i=offsetof(ETH_SRV_OAM_DATAGRM,TLV_offset) +1+ p->TLV_offset; i<oam_len;) {
     p2=(T_GEN_TLV*)    &((u8*)p)[i];
     len=ntohs(p2->len);
     if (TLV_type==p2->type) {  //found
         if (i+len+H>=oam_len) return 0UL-1;
         return i;
     }
     i+=len+H;
 }
 return 0UL-1;  //not found
}//iTLV
*/



static int rx_ltm(u16 oam_prt, u8 *pkt_ethtype, u32 pkt_len, u64 vid, u8 *pSMAC,
                    T_MEP_DB *p_mep_db, T_LOOKUP_MEP *p_mep_lut) {
//ETH_LTM_OAM_DATAGRM *p_ltm;
//T_MEP       *_p_mep;
//u32 i_look_r, i_mep;
//int i;
//u16 targ_prt;
//u8 unxlvl0_msmrg1_unxmep2_unxmeppotentloop3_unxperiod4; u32 alrm_index;
//
//
// p_ltm= (ETH_LTM_OAM_DATAGRM *) &pkt_ethtype[2];
//
// i_look_r=  finger_lut_index(0, oam_prt, vid, MALEVEL_AND_VERSION_TO_MALEVEL(p_ltm->MAlevel_and_version), 0,
//                             -1, //LTM packet doesn't bring any MEP_ID...
//                             -1 , -1, p_mep_lut, &unxlvl0_msmrg1_unxmep2_unxmeppotentloop3_unxperiod4, &alrm_index);
//
// i_mep= MEP_INDEX_TO_iMEP(i_look_r);    //...so this MEP_ID (-1) won't be found...
// //i_rmep=MEP_INDEX_TO_iRMEP(i_look_r);
// 
// if (!valid_mep_index(i_mep) /*|| !valid_rmep_index(i_rmep)*/) {
//     i_mep= MEP_INDEX_TO_iMEP(alrm_index);      //...and we'll have to deal just with alarms: the MEP in a certain port, VID and level (alarm: unexpected MEP)
//     //i_rmep=MEP_INDEX_TO_iRMEP(alrm_index);
//     
//     if (!valid_mep_index(i_mep)) return 1;
//
//     _p_mep=        &p_mep_db[i_mep];
//
//     i= _p_mep->level - MALEVEL_AND_VERSION_TO_MALEVEL(p_ltm->MAlevel_and_version);
//     if (/*0==unxlvl0_msmrg1_unxmep2_unxmeppotentloop3_unxperiod4 ||*/  i>0) {
//         return 2;
//     }
//     else
//     if (i<0) {
//         //FWD this packet to every other port on the same vlan
//         return 3;
//     }
//
//
//     if (!(2==unxlvl0_msmrg1_unxmep2_unxmeppotentloop3_unxperiod4)) return 0;//4;
//
//     //Response to an LTM
//     if (0==p_ltm->TTL) return 0;
//
//     if (1/*we're a MEP*/ && memcmp(p_ltm->targ_mac, this_prts_MAC(oam_prt), 6)) return 0;//5;
//
//     targ_prt=MAC_in_prt(p_ltm->targ_mac);
//     if (targ_prt>=N_OAM_PRTS) return 0;//6;  //NE unaware of this TargetMAC or associating it to more than a single egress port
//
//     i=iTLV_search((ETH_SRV_OAM_DATAGRM*)p_ltm, pkt_len-2, LTM_EG_ID_TLV_TYPE);
//
//     if (v2008(i)) {            //LTM egress identifier found (v2008)
//     }
//     else {                     //LTM egress identifier not found (v2006)
//     }
//     
//     send_ltr(oam_prt, (T_MEP_HDR *)_p_mep, (ETH_LBR_OAM_DATAGRM *)p_ltm, pkt_len-2, pSMAC);
//
//     if (0/*we're a MIP*/ && p_ltm->TTL>1 && memcmp(p_ltm->targ_mac, this_prts_MAC(oam_prt), 6)) {
//          //send_ltm(oam_prt, (T_MEP_HDR *)_p_mep, (ETH_LTM_OAM_DATAGRM *)p_ltm, pkt_len-2, pSMAC);
//     }
// }//if (!valid_mep_index(i_mep)

 return 0;
}//rx_ltm

