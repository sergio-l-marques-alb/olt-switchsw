#include "debug.h"

#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

 
int  debug_level;

#define MAX_DEBUG_LEVEL 5

T_DEBUG bd_debug;
T_DEBUG bk_debug;

int debug_init(void)
{
    bd_debug.start_up_debug    = 1;
    bd_debug.ins_modules_debug = 1;
    bd_debug.sinc_config       = 0;
    bd_debug.processes_debug   = 0;
    bd_debug.sincronismo_debug = 0;
    bd_debug.trmi_debug        = 0;
    bd_debug.trap_debug        = 1;
    bd_debug.sncp_debug        = 0;
    bd_debug.sncp_debug_sd     = 0;
    bd_debug.cruzamentos       = 0;
    bd_debug.protboard         = 0;
    bd_debug.xilinx            = 0;
    bd_debug.reles             = 0;
    bd_debug.i2c               = 0;
    bd_debug.sm_pboard         = 0;
    bd_debug.proc_sdegrade     = 0;
    bd_debug.als               = 0;
    bd_debug.alarme_externo    = 0;
    bd_debug.cwdm              = 0;
    bd_debug.atm_debug         = 0;
    bd_debug.trmi_bytes_debug  = 0;
    bd_debug.eth_debug         = 0;
    bd_debug.unicom_debug      = 0;
    return(0);
}


void err_prn(const char *fmt, va_list ap, char *logfile)
{
    int save_err;
    char buf[MAXLINELEN];
    FILE *plf;

    save_err =errno;
    vsprintf(buf,fmt,ap);
    if(logfile!=NULL)
        if((plf=fopen(logfile,"a"))!=NULL) {
            fputs(buf,plf);
            fclose(plf);
        } else
            fputs("failed to open log file\n",stderr);
    else
        fputs(buf,stderr);

    fflush(NULL);  /*flush everything */
    return;
}


int printf_debug(int level, const char *fmt,...)
{
    int save_err;
    char buf[MAXLINELEN];
    va_list ap;
    

    switch (level) {
        case DEBUG_START_UP:
            if(bd_debug.start_up_debug==0) return(-1);
            break;
        case DEBUG_INS_MODULES:
            if(bd_debug.ins_modules_debug==0) return(-1);
            break;
        case DEBUG_PROCESSES:
            if(bd_debug.processes_debug==0) return(-1);
            break;
        case DEBUG_BYTES_TRMI:
            if(bd_debug.trmi_bytes_debug==0) return(-1);
            break;
        case DEBUG_SINCRONISMO:
            if(bd_debug.trmi_debug==0) return(-1);
            break;
        case DEBUG_SINC_CONFIG:
            if(bd_debug.sinc_config==0) return(-1);
            break;
            
        case DEBUG_TRAP:
            if(bd_debug.trap_debug==0) return(-1);
            break;
            
        case DEBUG_SNCP:
            if(bd_debug.sncp_debug==0) return(-1);
            break;
        
        case DEBUG_SNCP_SD:
            if(bd_debug.sncp_debug_sd==0) return(-1);
            break;

        case DEBUG_CRUZAMENTOS:
            if(bd_debug.cruzamentos==0) return(-1);
            break;
        
        case DEBUG_PROTBOARD:
            if(bd_debug.protboard==0) return(-1);
            break;
        
        case DEBUG_XILINX:
            if(bd_debug.xilinx==0) return(-1);
            break;
        
        case DEBUG_RELES:
            if(bd_debug.reles==0) return(-1);
            break;
        
        case DEBUG_I2C:
            if(bd_debug.i2c==0) return(-1);
            break;

        case DEBUG_SM_PBOARD:
            if(bd_debug.sm_pboard==0) return(-1);
            break;

        case DEBUG_SDEGRADE:
            if(bd_debug.proc_sdegrade==0) return(-1);
            break;

        case DEBUG_ALS:
            if(bd_debug.als==0) return(-1);
            break;
        
        case DEBUG_ALARME_EXTERNO:
            if(bd_debug.alarme_externo==0) return(-1);
            break;

        case DEBUG_CWDM:
            if(bd_debug.cwdm==0) return(-1);
            break;
            
        case DEBUG_TRMI:
            if(bd_debug.trmi_debug==0) return(-1);
            break;
        
        case DEBUG_ATM:
            if(bd_debug.atm_debug==0) return(-1);
            break;
        
        case DEBUG_ETH:
            if(bd_debug.eth_debug==0) return(-1);
            break;

        case DEBUG_UNICOM:
            if(bd_debug.unicom_debug==0) return(-1);
            break;
            
    }

    va_start(ap,fmt);
    //err_prn(fmt,ap,NULL);

    save_err =errno;
    vsprintf(buf,fmt,ap);
    fputs(buf,stderr);

    fflush(NULL);  /*flush everything */
    
    va_end(ap);
    return(0);   
}


/*  
  [void debug_setlevel(int level)]
  Esta função retorna 0 caso a operação seja bem sucedida 
  e -1 em caso contrário  
*/


int debug_setlevel(int level)
{
    //if(level>MAX_DEBUG_LEVEL) return(-1);
    //debug_level = level;
    bd_debug.start_up_debug    = level & 0x000001;
    bd_debug.ins_modules_debug = level & 0x000002;
    bd_debug.sinc_config       = level & 0x000004;
    bd_debug.processes_debug   = level & 0x000008;
    bd_debug.sincronismo_debug = level & 0x000010;
    bd_debug.trmi_bytes_debug  = level & 0x000020;
    bd_debug.trap_debug        = level & 0x000040;
    bd_debug.sncp_debug        = level & 0x000080;
    bd_debug.sncp_debug_sd     = level & 0x000100;
    bd_debug.cruzamentos       = level & 0x000200;
    bd_debug.protboard         = level & 0x000400;
    bd_debug.xilinx            = level & 0x000800;
    bd_debug.reles             = level & 0x001000;
    bd_debug.i2c               = level & 0x002000;
    bd_debug.sm_pboard         = level & 0x004000;
    bd_debug.proc_sdegrade     = level & 0x008000;
    bd_debug.als               = level & 0x010000;
    bd_debug.alarme_externo    = level & 0x020000;
    bd_debug.cwdm              = level & 0x040000;
    bd_debug.trmi_debug        = level & 0x080000;
    bd_debug.atm_debug         = level & 0x100000;
    bd_debug.eth_debug         = level & 0x200000;
    bd_debug.unicom_debug      = level & 0x400000;

    printf("debug_setlevel 0x%.4x\n\r",level);
    return(0);
}



int debug_activo(int level)
{
    switch (level) {
        case DEBUG_START_UP:
            if(bd_debug.start_up_debug==0) return(-1);
            break;
        case DEBUG_INS_MODULES:
            if(bd_debug.ins_modules_debug==0) return(-1);
            break;
        case DEBUG_PROCESSES:
            if(bd_debug.processes_debug==0) return(-1);
            break;
        case DEBUG_BYTES_TRMI:
            if(bd_debug.trmi_bytes_debug==0) return(-1);
            break;
        case DEBUG_SINCRONISMO:
            if(bd_debug.trmi_debug==0) return(-1);
            break;
        case DEBUG_SINC_CONFIG:
            if(bd_debug.sinc_config==0) return(-1);
            break;
            
        case DEBUG_TRAP:
            if(bd_debug.trap_debug==0) return(-1);
            break;
            
        case DEBUG_SNCP:
            if(bd_debug.sncp_debug==0) return(-1);
            break;
        
        case DEBUG_SNCP_SD:
            if(bd_debug.sncp_debug_sd==0) return(-1);
            break;

        case DEBUG_CRUZAMENTOS:
            if(bd_debug.cruzamentos==0) return(-1);
            break;
        
        case DEBUG_PROTBOARD:
            if(bd_debug.protboard==0) return(-1);
            break;

        case DEBUG_XILINX:
            if(bd_debug.xilinx==0) return(-1);
            break;

        case DEBUG_RELES:
            if(bd_debug.reles==0) return(-1);
            break;

        case DEBUG_I2C:
            if(bd_debug.i2c==0) return(-1);
            break;

        case DEBUG_SM_PBOARD:
            if(bd_debug.sm_pboard==0) return(-1);
            break;

        case DEBUG_SDEGRADE:
            if(bd_debug.proc_sdegrade==0) return(-1);
            break;

        case DEBUG_ALS:
            if(bd_debug.als==0) return(-1);
            break;

        case DEBUG_ALARME_EXTERNO:
            if(bd_debug.alarme_externo==0) return(-1);
            break;

        case DEBUG_CWDM:
            if(bd_debug.cwdm==0) return(-1);
            break;

        case DEBUG_TRMI:
            if(bd_debug.trmi_debug==0) return(-1);
            break;

        case DEBUG_ATM:
            if(bd_debug.atm_debug==0) return(-1);
            break;

        case DEBUG_ETH:
            if(bd_debug.eth_debug==0) return(-1);
            break;

        case DEBUG_UNICOM:
            if(bd_debug.unicom_debug==0) return(-1);
            break;
            

        default:
            return(-1);
    }

    return(0);
}


/*
 [void debug_scr(int level, const char *fmt,...)]
 
 Envio de uma mensagem para o stdout. O envio ou não está dependente do nível 
 de debug atribuído à mensagem e ao nível de debug configurado
 
*/
int debug_scr(int level, const char *fmt,...)
{
    va_list ap;

    if(level>debug_level) return(-1);
    va_start(ap,fmt);
    err_prn(fmt,ap,NULL);
    va_end(ap);
    return(0);   
}


unsigned char sss[15];

void debug_api(void)
{   
    memcpy(&bk_debug, &bd_debug, sizeof(T_DEBUG));

    //memset(&bd_debug,0,sizeof(T_DEBUG));

    sleep(1);

    printf("**** ACTIVACAO DE DEBUG ****\n\r");
    printf("****++++++++++++++++++++****\n\r");

    if (bk_debug.start_up_debug == 0)    
        printf("1 - start_up_debug .... I\n\r"); 
    else 
        printf("1 - start_up_debug .... A\n\r");

    if (bk_debug.ins_modules_debug == 0) 
        printf("2 - ins_modules_debug . I\n\r"); 
    else 
        printf("2 - ins_modules_debug . A\n\r");

    if (bk_debug.sinc_config == 0)  
        printf("3 - sinc_config ....... I\n\r"); 
    else 
        printf("3 - sinc_config ....... A\n\r");

    if (bk_debug.processes_debug == 0) 
        printf("4 - processes_debug ... I\n\r"); 
    else         
        printf("4 - processes_debug ... A\n\r");

    if (bk_debug.sincronismo_debug == 0) 
        printf("5 - sincronismo_debug . I\n\r"); 
    else 
        printf("5 - sincronismo_debug . A\n\r");

    if (bk_debug.trmi_debug == 0) 
        printf("6 - trmi_debug ........ I\n\r"); 
    else 
        printf("6 - trmi_debug ........ A\n\r");

    if (bk_debug.trap_debug == 0) 
        printf("7 - trap_debug ........ I\n\r"); 
    else 
        printf("7 - trap_debug ........ A\n\r");

    if (bk_debug.sncp_debug == 0) 
        printf("8 - sncp_debug ........ I\n\r"); 
    else 
        printf("8 - sncp_debug ........ A\n\r");

    if (bk_debug.sncp_debug_sd == 0) 
        printf("9 - sncp_debug_sd ..... I\n\r"); 
    else 
        printf("9 - sncp_debug_sd ..... A\n\r");

    if (bk_debug.cruzamentos == 0) 
        printf("10- cruzamentos ....... I\n\r"); 
    else 
        printf("10- cruzamentos ....... A\n\r");

    if (bk_debug.protboard == 0) 
        printf("11- protboard ......... I\n\r"); 
    else 
        printf("11- protboard ......... A\n\r");

    if (bk_debug.xilinx == 0) 
        printf("12- xilinx ............ I\n\r"); 
    else 
        printf("12- xilinx ............ A\n\r");

    if (bk_debug.reles == 0)        
        printf("13- reles ............. I\n\r"); 
    else 
        printf("13- reles ............. A\n\r");
    
    if (bk_debug.i2c == 0) 
        printf("14- i2c ............... I\n\r"); 
    else 
        printf("14- i2c ............... A\n\r");

    if (bk_debug.sm_pboard == 0) 
        printf("15- sm_pboard ......... I\n\r"); 
    else 
        printf("15- sm_pboard ......... A\n\r");

    if (bk_debug.proc_sdegrade == 0) 
        printf("15- sdegrade .......... I\n\r"); 
    else 
        printf("15- sdegrade .......... A\n\r"); 

    if (bk_debug.als == 0) 
        printf("16- sdegrade .......... I\n\r"); 
    else 
        printf("16- sdegrade .......... A\n\r"); 

    printf("--------------------------\n\r");
    printf("ex: Para activar o debug dos reles: a13\n\r");
    printf("ex: Para inactivar o debug dos reles: i13\n\r");
    /*
    printf(">");
    gets(sss);
    if(strcmp(sss,"s1")==0) {
        printf("slfhlsalhf\n\r");
    }
    else
        printf("comando invalido\n\r");
        */
    //fscanf(stdin,"%5s",&sss[0]);
    //printf("%s\n\r",s);

    //printf("Carregue numa tecla para continuar\n\r");
    //getchar();
    //memcpy(&bd_debug, &bk_debug, sizeof(T_DEBUG));

}


int debug_print_levels(void)
{
    printf("start_up_debug    = 00 00 00 01\n\r");
    printf("ins_modules_debug = 00 00 00 02\n\r");
    printf("sinc_config       = 00 00 00 04\n\r");
    printf("processes_debug   = 00 00 00 08\n\r");
    printf("sincronismo_debug = 00 00 00 10\n\r");
    printf("trmi_bytes_debug  = 00 00 00 20\n\r");
    printf("trap_debug        = 00 00 00 40\n\r");
    printf("sncp_debug        = 00 00 00 80\n\r");
    printf("sncp_debug_sd     = 00 00 01 00\n\r");
    printf("cruzamentos       = 00 00 02 00\n\r");
    printf("protboard         = 00 00 04 00\n\r");
    printf("xilinx            = 00 00 08 00\n\r");
    printf("reles             = 00 00 10 00\n\r");
    printf("i2c               = 00 00 20 00\n\r");
    printf("sm_pboard         = 00 00 40 00\n\r");
    printf("proc_sdegrade     = 00 00 80 00\n\r");
    printf("als               = 00 01 00 00\n\r");
    printf("alarme_externo    = 00 02 00 00\n\r");
    printf("cwdm              = 00 04 00 00\n\r");
    printf("trmi_debug        = 00 08 00 00\n\r");
    printf("atm_debug         = 00 10 00 00\n\r");
    printf("eth_debug         = 00 20 00 00\n\r");
    printf("unicom_debug      = 00 40 00 00\n\r");
    return(0);
}



