#ifndef _ETHSRV_OAM_H_
#define _ETHSRV_OAM_H_

typedef unsigned char   u8;
typedef unsigned short  u16;
typedef unsigned long   u32;
typedef unsigned long long u64;
#ifndef NLS
    //#define NLS "\n\r"
    #define NLS "\n\r"
#endif


#ifndef N_OAM_PRTS
#include <ptin_globaldefs.h>
    //#warning You must define constant N_OAM_PRTS (number of OAM MAC ports)!
    #define N_OAM_PRTS PTIN_SYSTEM_N_PORTS
#endif

#ifndef N_MEPs
    #define N_MEPs  512
    //#warning Definable: constant N_MEPs (number of Maintenance end Points)!
#endif

#ifndef N_MAX_MEs_PER_MEP
    #define N_MAX_MEs_PER_MEP   16
    //#warning Definable: constant N_MAX_MEs_PER_MEP (number of remote MEPs each MEP can talk to)!
#endif

#ifndef LOOKUP_MEP_BITS
    #define LOOKUP_MEP_BITS    10
    //#warning Definable: constant LOOKUP_MEP_BITS
    //#warning (defines N_MAX_LOOKUP_MEPs=2^LOOKUP_MEP_BITS, the number of entries in the lookup table)
#endif
#define N_MAX_LOOKUP_MEPs   ((1<<(LOOKUP_MEP_BITS))+1)

#ifndef N_MAX_LOOKUP_MEPs_PER_HASH
    #define N_MAX_LOOKUP_MEPs_PER_HASH  16
    //#warning Definable: constant N_MAX_LOOKUP_MEPs_PER_HASH
    //#warning (Nr of sequential lookup entries search after hash index calculation - reduce "for" cycles)
#endif

#ifndef MEP_MIN_T_ms
    #define MEP_MIN_T_ms    10
    //#warning You must define constant MEP_MIN_T_ms!
    //#warning (Minimum period/max frequency at which each MEP is processed in function "proc_ethsrv_oam")
#endif












#define MEP_BITS        13
#define HIGHEST_MEP     ((1<<(MEP_BITS))-1)
#define INVALID_MEP     ((u16)(-1))
#define valid_mep_id(a) ((a)<=HIGHEST_MEP)
#define valid_mep_index(a)  ((a)<(N_MEPs))
#define valid_rmep_index(a) ((a)<(N_MAX_MEs_PER_MEP))

#define N_OAM_LEVELS    8                       // Customer role: Levels [7..5]; Provider role: Levels[4..3]; Operator role: Levels [2..0]
#define lvlcmp(a,b)     ((int)(a)-(int)(b))
//  <0 - level "a" is more "operator" and "b" more "customer"
// ==0 - equal levels
//  >0 - level "a" is more "customer" and "b" more "operator"
#define valid_oam_level(a)  ((a)<N_OAM_LEVELS)



// IEEE 802.1ag and ITU Y.1731 define 3 bits -> 2^3=8 code values for timers from 3.3ms to 10min
#define N_OAM_TMR_VALUES    8                   
#define valid_oam_tmr(a)  ((a)<N_OAM_TMR_VALUES)
#define __OAM_TIMER_CODE_VALUES_DECLARATION__ \
const u32 OAM_TMR_CODE_TO_ms[N_OAM_TMR_VALUES] = {(0UL-1), 3, 10, 100, 1000, 10000, 60000, 600000UL};           // ITU Y.17ethoam May2005
//const u32 OAM_TMR_CODE_TO_ms[N_OAM_TMR_VALUES] = {3, 20, 100, 600, 3000, 18000, 100000UL, 600000UL};          // These values do a logarithmic map between codes and timer values in that range [T(ms)=3.3b^n(0:7)]
//const u32 OAM_TMR_CODE_TO_ms[N_OAM_TMR_VALUES] = {(0UL-1)/*no period*/, 1, 3, 10, 100, 1000, 10000, 60000};   //ITU Y.17ethoam Dec2005
extern const u32 OAM_TMR_CODE_TO_ms[];







typedef struct {
    u8  byte[6];
} __attribute__ ((packed)) T_ETH_OAM_MAC;

//typedef T_ETH_OAM_MAC  T_MEG_ID;
typedef struct {
    u8  byte[48];
} __attribute__ ((packed)) T_MEG_ID;

#define __OAM_MC_MAC_DECLARATION__ \
const T_ETH_OAM_MAC OAM_MC_MAC={{0x01, 0x80, 0xC2, 0x00, 0x00, 0x30}};
//const T_ETH_OAM_MAC OAM_MC_MAC={{0x01, 0x06, 0x91, 0x00, 0x00, 0x00}};
extern const T_ETH_OAM_MAC OAM_MC_MAC;






typedef struct {
    u16 mep_id;             //Monitored MEP. ">HIGHEST_MEP"   means empty entry
    u32 LOC_timer;          //RO    (ms)        RX; used to decide that a timeout ocurred
#define LOC(LOC_timer, CCM_period)  ((LOC_timer)*2 >= (CCM_period)*7)

    u8  RDI;                //RO
} __attribute__ ((packed)) T_ME;  //"Remote MEP"; MEP on the other side of a ME (relatively to our MEP)




#define _T_RMEP_HDR \
    T_MEG_ID meg_id;\
    u64 vid;\
    u16 mep_id;     /*0..2^13-1*/\
    u8  level;      /*0..N_OAM_LEVELS-1*/\
    u8  tmout;      /*0..N_OAM_TMR_VALUES-1     (code)*/\
    u16 prt;        /*port to Tx/Rx OAM frames*/


typedef struct {
    _T_RMEP_HDR
} __attribute__ ((packed)) T_RMEP;




//MEP - Maintenance End Point [IEEE802.1ag adds the word "Association" - MA].
//The following struct is the building block for MEPs belonging to this board, their relation to
//higher layer MEPs that also belong to this board and client layer.
//It also reflects links to remote MEPs on each ME (Management Entity) of the MEG (G=group) and
//links to an auxiliary MEP lookup table
#define _T_MEP_HDR \
    _T_RMEP_HDR \
    u8  prior;\
    u8  up1_down0;\
\
    u32 CCM_timer;              /*RO    (ms)        TX; used to decide when to send a CCM packet*/\
\
    u32 mismerge_timer, unxp_MEP_timer, unxp_lvl_timer, unxp_T_timer;   /*RO    (ms)*/\
    u32 c[2][2][2];     /*[t0 t][intrnal_counter packet_counter][Rx Tx]     [0 1]*/\
    u8  CoS, dummy_color;
#define MISMERGE(MSMRG_timer, CCM_period)   ((MSMRG_timer)*2 < (CCM_period)*7)
#define UNXP_MEP(UXMEP_timer, CCM_period)   ((UXMEP_timer)*2 < (CCM_period)*7)
#define UNXP_LVL(UXLVL_timer, CCM_period)   ((UXLVL_timer)*2 < (CCM_period)*7)
#define UNXP_T(UX_T_timer, CCM_period)      ((UX_T_timer)*2 < (CCM_period)*7)


typedef struct {
    _T_MEP_HDR
} __attribute__ ((packed)) T_MEP_HDR;

typedef struct {
    _T_MEP_HDR

    T_ME ME[N_MAX_MEs_PER_MEP];   //For now we'll keep this in an array; if "N_MAX_MEs_PER_MEP" becomes too big, we'll change to a linked list
} __attribute__ ((packed)) T_MEP;
#define EMPTY_T_MEP(mep)    (!valid_mep_id((mep).mep_id))
#define SET_T_MEP_EMPTY(mep)    {(mep).mep_id= INVALID_MEP;}














//typedef
//TYPE_SIMPLE_LNKD_LIST(T_MEP, N_MEPs)
//T_MEP_LNKD_LST;




typedef struct {
    //T_ETH_OAM_MAC  SMAC;
    //T_MEG_ID meg_id;
    u32 mep_index;          //RO    auxiliary index to the "T_MEP" table
#define iMEP_iRMEP_TO_MEP_INDEX(iMEP,iRMEP)     ((iMEP)<<16 |   (iRMEP))
#define MEP_INDEX_TO_iMEP(mep_index)            ((mep_index)>>16)
#define MEP_INDEX_TO_iRMEP(mep_index)           ((mep_index)    &   0xffff)
    u16 mep_id;             //MEP or Monitored MEP. ">HIGHEST_MEP"   means empty entry
    u16 prt;
    u64 vid;
    u8 level;
    u8 mep0_rmep1;
    //u8  CCM_tmout;
} __attribute__ ((packed)) T_LOOKUP_MEP;

#define valid_lookup_index(a) ((a)<(N_MAX_LOOKUP_MEPs))











// ------------------------------------------------------------------------------------------------------
// Y.1731
// 7.12 Ethernet Client Signal Fail (ETH-CSF)
// ------------------------------------------------------------------------------------------------------
// Specific configuration information required by a MEP to support ETH-CSF transmission is:
// Local MEG Level: MEG Level at which the source MEP operates
// ETH-CSF transmission period: Determines transmission periodicity of frames with ETH-CSF information.
// Priority: Identifies the priority of frames with ETH-CSF information.
// Drop Eligibility: Frames with ETH-CSF information are always marked as drop ineligible.
// Specific configuration information required by a MEP to support ETH-CSF reception is:
// Local MEG Level: MEG Level at which the sink MEP operates.
// ------------------------------------------------------------------------------------------------------

#define CSF_flags_TO_CSF_TYPE(CSF_flags)    ((CSF_flags)>>3 &0x07)
#define CSF_flags_TO_CSF_TMOUT(CSF_flags)   flags_TO_TMOUT(CSF_flags)
//#define CSF_flags_TO_CSF_TMOUT(CSF_flags)   ((CSF_flags) &0x07)
#define ASSEMBLE_CSF_flags(TYPE,TMOUT)      (((TYPE)<<3)    |  ((TMOUT) & 0x07))
#define CSF_TYPE_LOS        0   /*Client Loss of signal*/
#define CSF_TYPE_FDI_AIS    1   /*Client Frwrd Defect Indication*/
#define CSF_TYPE_RDI        2   /*Client Reverse Defect Indication*/
#define CSF_TYPE_DCI        3   /*Client Defect Clear Indication*/
#define CSF_TYPE_INVALID    0xFF

typedef struct {
    u8  en1dis0;            // Enable or disable CSF function

    u8  period;             // ETH-CSF transmission period
    u8  CSF_Tx_en_flags;    // Enable Tx CSF with those flags; When eq. to CSF_FLAGS_INVALID disable Tx

    u32 CSF_timer;          // RO    (ms)        TX; used to decide when to send a CSF packet
    u32 LOS_timer;          // RO    (ms)        RX; used to decide the end of LOS alarm; Equal to "0" means in C-LOS

} __attribute__ ((packed)) T_MEP_CSF;

// ------------------------------------------------------------------------------------------------------





















//THIS STRUCTURE AGGREGATES ALL - DECLARE ONE INSTANCE OF THIS TYPE*********************
typedef struct {
 u16            proc_i_mep;                 //index to mep being processed by
 T_MEP          mep_db[N_MEPs];             //MEP data base (linked list)
 T_MEP_CSF      mep_csf_db[N_MEPs];         //MEP CSF data base
 T_LOOKUP_MEP   mep_lut[N_MAX_LOOKUP_MEPs]; //MEP look up table
} T_ETH_SRV_OAM;    //__attribute__ ((packed)) T_ETH_SRV_OAM;


























// (Y_1731: 9.1 -> Common OAM information elements)
typedef struct {
//1st octet is the one after the OAM E-Type field in the ethernet packet, whose value is...
//#define OAM_ETH_TYPE  0xFFFF    //still undefined in the draft standard...
#define OAM_ETH_TYPE  0x8902

 u8  MAlevel_and_version;
#define MALEVEL_AND_VERSION_TO_MALEVEL(lvl_and_ver) ((lvl_and_ver)>>5)
#define MALEVEL_AND_VERSION_TO_VERSION(lvl_and_ver) (~(0xff<<5) & (lvl_and_ver))
#define ASSEMBLE_OAM_MALEVEL_AND_VERSION(lvl,ver)   ((lvl)<<5 | (ver))
#define OAM_PROTO_VERSION   0

 u8  opcode;
//Common with IEEE802.1
#define CCM_OPCODE  0x01
//#define LBR_OPCODE  0x02
//#define LBM_OPCODE  0x03
//#define LTR_OPCODE  0x04
//#define LTM_OPCODE  0x05
//Specific to Y.1731
#define AIS_OPCODE  33
#define CSF_OPCODE  52

 u8  flags;
 u8  TLV_offset;
 u8  info[1];
} __attribute__ ((packed)) ETH_SRV_OAM_DATAGRM;



// (Y_1731: 9.2 -> CCM PDU)
typedef struct {
 u8     MAlevel_and_version;    // MEG Level = [0..7] ; Version = 0
 u8     opcode;         	    // Identifies an OAM PDU type (Table 9-1); for this PDU type is CCM (1)
                                // =CCM_OPCODE 
 u8     flags;			        // bit8 -> RDI; bits7..4 -> Reserved = 0; bits3..1 -> Period

#define flags_TO_RDI(flags)             ((flags) & 0x80)	// 1MS bit (RDI=1 -> detection of a defect; RDI=0 -> no defect detection)
#define flags_TO_TMOUT(flags)           ((flags) & 0x07)	// 3LS bits:            000 -> Invalid value    
                                                            //                      001 -> 3.33ms   (300 frames per second)
                                                            //                      010 -> 10ms     (100 frames per second)
                                                            //                      011 -> 100ms    (10 frames per second)
                                                            //                      100 -> 1s       (1 frames per second)
                                                            //                      101 -> 10s      (6 frames per second)
                                                            //                      110 -> 1min     (1 frames per minute)
                                                            //                      111 -> 10min    (6 frames per hour)
#define ASSEMBLE_CCM_flags(RDI,TMOUT)   (((RDI)? 0x80: 0)    |  ((TMOUT) & 0x07))

 u8     TLV_offset;             // Set to 70     
                                // =(offsetof(ETH_SRV_OAM_CCM_DATAGRM,end_TLV) - offsetof(ETH_SRV_OAM_CCM_DATAGRM,TLV_offset)			            
 u32	SeqNumb;                // Set to all-ZEROes for this Recommendation
 u16    mep_id;		            // 13LS bits are used to identify the MEP transmitting the CCM frame, within the MEG; 3MS bits are not used and set to ZERO
 T_MEG_ID   meg_id;             // Annex A
 u32    TxFCf, RxFCb, TxFCb;	// Information elements carried in CCM to support dual-ended ETH-LM (set to all-ZEROes when not used)
 u32	reserved;               // Set to all ZEROes
 u8	end_TLV;                    // Set to all ZEROes
 //u8     TLV_type;
 //u16    TLV_len;
} __attribute__ ((packed)) ETH_SRV_OAM_CCM_DATAGRM;
#define MAX_LEN_CCM 128


#define AIS_flags_TO_AIS_TMOUT(AIS_flags)   flags_TO_TMOUT(CSF_flags)
#define ASSEMBLE_AIS_flags(TMOUT)           ((TMOUT) & 0x07)
typedef ETH_SRV_OAM_DATAGRM ETH_SRV_OAM_AIS_DATAGRM;

typedef ETH_SRV_OAM_DATAGRM ETH_SRV_OAM_CSF_DATAGRM;





































extern void init_mep_db(T_MEP *p_mep_db);
extern void init_mep_lookup_table(T_LOOKUP_MEP *p_mep_lut);
extern void init_mep_csf_db(T_MEP_CSF *p_mep_csf_db);
//returns indexes for MEP table (and its RMEP table) according to format iMEP_iRMEP_TO_MEP_INDEX(,)
u32 finger_lut_index(u8 find0_add1_del2,                                                                                    //operation
                     u16 prt, u64 vid, u8 level, u8 mep0_rmep1, u32 mep_id, u32 i_mep, u32 i_rmep, T_LOOKUP_MEP *p_mep_lut, //input     (i_mep, i_rmep used only when ADDING)
                     u8 *unxlvl0_msmrg1_unxmep2_unxmeppotentloop3_unxperiod4, u32 *alrm_index);                             //additional outputs


extern void init_eth_srv_oam(T_ETH_SRV_OAM *p);
extern int invalid_T_MEP_HDR(const T_MEP_HDR *p_mep);

//Return status:
//0 - OK
//1 - Invalid T_MEP
//2 - MEP table full
//3 - MEP Lookup table full (please try another MEP ID)
//4 - A remote MEP already is using this MEP ID (please try another MEP ID)
extern int wr_mep(u32 i_mep, T_MEP_HDR *p_mep, T_ETH_SRV_OAM *p_oam);

//Return status:
//0 - OK
//1 - Invalid index
extern int del_mep(u32 i_mep, T_ETH_SRV_OAM *p_oam);

//Return status:
//0 - OK
//1 - Invalid T_RMEP
//2 - Invalid T_MEP
//3 - MEP and remote MEP don't belong to the same MEG or MEG level
//4 - MEP Lookup table full or remote MEP ID already in use (please try another remote MEP ID)
//5 - MEP to which to attach the remote MEP doesn't exist
//6 - A remote MEP is using this MEP ID
//7 - Already reached maximum number of MEs in this MEP
extern int wr_rmep(u32 i_mep, u32 i_rmep, T_RMEP *p_rmep, const T_MEP_HDR *p_mep, T_ETH_SRV_OAM *p_oam);

//Return status:
//0 - OK
//1 - Invalid index
extern int del_rmep(u32 i_mep, u32 i_rmep, T_ETH_SRV_OAM *p_oam);

extern void proc_ethsrv_oam(T_ETH_SRV_OAM *p_oam, u32 T_ms);
extern int rx_oam_pckt(u16 oam_prt, u8 *pkt, u32 pkt_len, u64 vid, u8 *pSMAC, T_ETH_SRV_OAM *p_oam);





//FUNCTIONS TO BE DEFINED ELSEWHERE
//this function must fill in the MAC SA (7th to 12th bytes)
extern int send_eth_pckt(u16 _prt, u8 up1_down0, u8 *buf, u32 length, u64 vid, u8 prior, u8 CoS, u8 color, u16 ETHtype, u8 *pDMAC);//"length" doesn't account for "vid", "ETHtype" or "S/DMACs"

extern void ethsrv_oam_register_connection_loss(u8 *meg_id, u16 mep_id, u16 rmep_id, u16 port, u64 vid);
extern void ethsrv_oam_register_receiving_RDI(u8 *meg_id, u16 mep_id, u16 rmep_id, u16 port, u64 vid);
extern void ethsrv_oam_register_RDI_END(u8 *meg_id, u16 mep_id, u16 rmep_id, u16 port, u64 vid);
extern void ethsrv_oam_register_connection_restored(u8 *meg_id, u16 mep_id, u16 rmep_id, u16 port, u64 vid);
//extern void ethsrv_oam_register_RMEP_discovery(u8 *meg_id, u16 mep_id, u16 rmep_id, u16 port, u64 vid);

//extern void ethsrv_oam_register_unexpected_MEG_MEP_id(T_MEG_ID *meg_id, u16 mep_id, u16 mep_indx, u16 porta, u64 vid);
extern void ethsrv_oam_register_unexpected_MEP_id(T_MEG_ID *meg_id, u16 mep_id, u16 mep_indx, u16 porta, u64 vid);
extern void ethsrv_oam_register_unexpected_MEP_potential_loop(T_MEG_ID *meg_id, u16 mep_id, u16 mep_indx, u16 porta, u64 vid);
extern void ethsrv_oam_register_mismerge(T_MEG_ID *meg_id, u16 mep_id, u16 mep_indx, u16 porta, u64 vid);

extern void ethsrv_oam_register_LVL(T_MEG_ID *meg_id, u16 mep_id, u16 mep_indx, u16 porta, u64 vid, u8 level);
extern void ethsrv_oam_register_T(T_MEG_ID *meg_id, u16 mep_id, u16 mep_indx, u16 porta, u64 vid, u8 period);



























/**
 * Enable/disable CSF Function on this MEP
 * 
 * @author joaom (1/26/2012)
 * 
 * @param mep_idx 
 * @param en1dis0 
 * @param period 
 * 
 * @return int 
 */
extern int MEP_csf_admin(u16 mep_idx, u8 en1dis0, u8 period, T_ETH_SRV_OAM *p_oam);

// CSF Function and Link Fault Pass-through (LFP) integration
extern int MEP_enable_Tx_CSF(u16 mep_idx, u8 CSF_tx_flags, T_ETH_SRV_OAM *p_oam);
extern int MEP_is_CC_LOC_or_RDI(u16 mep_idx, T_ETH_SRV_OAM *p_oam);
extern int MEP_is_CSF_LOS(u16 mep_idx, T_ETH_SRV_OAM *p_oam);
#endif /*_ETHSRV_OAM_H_*/

