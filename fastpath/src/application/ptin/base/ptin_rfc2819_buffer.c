
//*****************************************************************************

#include <stdio.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h> 
#include "logger.h"
#include "ptin_rfc2819_buffer.h"


//*****************************************************************************
//* Vars
//*****************************************************************************

// BUFFERS
TBufferQualRFC2819 bufferQualRFC2819;

//*****************************************************************************
//* Code
//*****************************************************************************

/**
 * ----------------------------------------------------------------------
 * Function name: ptin_rfc2819_buffer_init
 * ----------------------------------------------------------------------
 * Description:     
 *   
 * ----------------------------------------------------------------------
 * 
 * @param flag: 
 * ----------------------------------------------------------------------
 * 
 * @return L7_int: 
 * ----------------------------------------------------------------------
 * @author clemos (2/23/2015)
 * ----------------------------------------------------------------------
 */
L7_int ptin_rfc2819_buffer_init(L7_int flag) 
{
    L7_int i,j;

    for ( i=0; i<MAX_QUAL_RFC2819_BUFFERS; i++ ) {
        bufferQualRFC2819.wrptr[i]      = 0;
        bufferQualRFC2819.bufferfull[i] = 0;

        for (j=0;j<MAX_QUAL_RFC2819_REG_NUM;j++) {
            bufferQualRFC2819.reg[i][j].index = j;
        }
    }

    return(sizeof(TBufferQualRFC2819));
}


/**
 * ----------------------------------------------------------------------
 * Function name: ptin_rfc2819_buffer_clear
 * ----------------------------------------------------------------------
 * Description:     
 *   
 * ----------------------------------------------------------------------
 * 
 * @param buffer_index: 
 * ----------------------------------------------------------------------
 * 
 * @return L7_RC_t: L7_SUCCESS, L7_FAILURE
 * ----------------------------------------------------------------------
 * @author clemos (2/23/2015)
 * ----------------------------------------------------------------------
 */
L7_RC_t ptin_rfc2819_buffer_clear(L7_int buffer_index) 
{
    int j;

    if (buffer_index>=MAX_QUAL_RFC2819_BUFFERS)
        return L7_FAILURE;

    bufferQualRFC2819.wrptr[buffer_index]      = 0;
    bufferQualRFC2819.bufferfull[buffer_index] = 0;

    for (j=0;j<MAX_QUAL_RFC2819_REG_NUM;j++) {
        bufferQualRFC2819.reg[buffer_index][j].index = j;
    }

    if(buffer_index==RFC2819_BUFFER_15MIN)
      LOG_TRACE(LOG_CTX_RFC2819, "rfc2819 15min buffer cleared");
    else
      LOG_TRACE(LOG_CTX_RFC2819, "rfc2819 24hours buffer cleared");

    return L7_SUCCESS;
}



/**
 * ----------------------------------------------------------------------
 * Function name: ptin_rfc2819_buffer_close
 * ----------------------------------------------------------------------
 * Description:     
 *   
 * ----------------------------------------------------------------------
 * ----------------------------------------------------------------------
 * 
 * @return L7_RC_t: L7_SUCCESS
 * ----------------------------------------------------------------------
 * @author clemos (2/23/2015)
 * ----------------------------------------------------------------------
 */
L7_RC_t ptin_rfc2819_buffer_close(void) 
{
    return L7_SUCCESS;
}



/**
 * ----------------------------------------------------------------------
 * Function name: ptin_rfc2819_buffer_write
 * ----------------------------------------------------------------------
 * Description:     
 *   
 * ----------------------------------------------------------------------
 * 
 * @param buffer_index: 
 * @param data: 
 * @param flag: 
 * ----------------------------------------------------------------------
 * 
 * @return L7_RC_t: L7_SUCCESS, L7_FAILURE
 * ----------------------------------------------------------------------
 * @author clemos (2/23/2015)
 * ----------------------------------------------------------------------
 */
L7_RC_t ptin_rfc2819_buffer_write(L7_int buffer_index, void *data, L7_int flag) 
{
    unsigned char *preg;

    preg = NULL;  

    if (buffer_index>=MAX_QUAL_RFC2819_BUFFERS)
        return L7_FAILURE;

    if (bufferQualRFC2819.wrptr[buffer_index] >= MAX_QUAL_RFC2819_REG_NUM) {
        ptin_rfc2819_buffer_init(0);
        return L7_FAILURE;
    }

    LOG_TRACE(LOG_CTX_RFC2819, "ptin_rfc2819_buffer_write(buffer_index=%d, wrptr=%d, flag=%d)", buffer_index, bufferQualRFC2819.wrptr[buffer_index], flag);

    preg  = (unsigned char *)&(bufferQualRFC2819.reg[buffer_index][bufferQualRFC2819.wrptr[buffer_index]]);

    memcpy(preg, (unsigned char *)data, sizeof(TBufferRegQualRFC2819));    

    bufferQualRFC2819.reg[buffer_index][bufferQualRFC2819.wrptr[buffer_index]].index = bufferQualRFC2819.wrptr[buffer_index];   

    bufferQualRFC2819.wrptr[buffer_index]++;
    if (bufferQualRFC2819.wrptr[buffer_index] >= MAX_QUAL_RFC2819_REG_NUM) {
        bufferQualRFC2819.wrptr[buffer_index]      = 0;
        bufferQualRFC2819.bufferfull[buffer_index] = 1; //O buffer circular esta cheio. A escrita passa a ser wrptr+1
    }

    return L7_SUCCESS;
}


/**
 * ----------------------------------------------------------------------
 * Function name: ptin_rfc2819_get_buffer_status
 * ----------------------------------------------------------------------
 * Description:   
 *   
 * ----------------------------------------------------------------------
 * 
 * @param buffer_type: 
 * @param max_regs: 
 * @param wrptr: 
 * @param bufferfull: 
 * ----------------------------------------------------------------------
 * 
 * @return L7_RC_t: 
 * ----------------------------------------------------------------------
 * @author clemos (2/27/2015)
 * ----------------------------------------------------------------------
 */
L7_RC_t ptin_rfc2819_get_buffer_status(L7_int buffer_type, L7_uint16 *max_regs,  L7_uint16 *wrptr, L7_uint16 *bufferfull)
{

  if (buffer_type>=MAX_QUAL_RFC2819_BUFFERS) {
    *max_regs     = 0;
    *wrptr        = 0;
    *bufferfull   = 0;
    return L7_FAILURE;
  }

  *max_regs   = MAX_QUAL_RFC2819_REG_NUM;
  *wrptr      = bufferQualRFC2819.wrptr[buffer_type];
  *bufferfull = bufferQualRFC2819.bufferfull[buffer_type];

  return L7_SUCCESS;
}



/**
 * ----------------------------------------------------------------------
 * Function name: ptin_rfc2819_buffer_print_ctrl
 * ----------------------------------------------------------------------
 * Description:     
 *   
 * ----------------------------------------------------------------------
 * ----------------------------------------------------------------------
 * ----------------------------------------------------------------------
 * @author clemos (2/23/2015)
 * ----------------------------------------------------------------------
 */
void ptin_rfc2819_buffer_print_ctrl(void)
{
    L7_int i,nregs;

    LOG_INFO(LOG_CTX_RFC2819, " Index  wrptr  rdflag   nreg");
    printf("+++++++++++++++++++++++++++++");

    for ( i=0; i<MAX_QUAL_RFC2819_BUFFERS; i++ ) {

        //calculo do numero de registos
        if (bufferQualRFC2819.bufferfull[i]==0)
            if (bufferQualRFC2819.wrptr[i]==0)
                nregs=0;
            else
                nregs=bufferQualRFC2819.wrptr[i];
        else
            nregs=MAX_QUAL_RFC2819_REG_NUM;        

        LOG_INFO(LOG_CTX_RFC2819, "%4d     %4d      %d   %4d",i,bufferQualRFC2819.wrptr[i], bufferQualRFC2819.bufferfull[i],nregs);
    }
    LOG_INFO(LOG_CTX_RFC2819, "+++++++++++++++++++++++++++++\n\r");
}


/**
 * ----------------------------------------------------------------------
 * Function name: ptin_rfc2819_buffer_get
 * ----------------------------------------------------------------------
 * Description:     
 *   
 * ----------------------------------------------------------------------
 * 
 * @param buffer_index: 
 * @param reg_index: 
 * @param reg: 
 * ----------------------------------------------------------------------
 * 
 * @return L7_int: 
 * ----------------------------------------------------------------------
 * @author clemos (2/23/2015)
 * ----------------------------------------------------------------------
 */
L7_int ptin_rfc2819_buffer_get(L7_int buffer_index, L7_int reg_index, TBufferRegQualRFC2819 *reg)
{
  LOG_TRACE(LOG_CTX_RFC2819, "buffer_index=%d, reg_index=%d, full=%d ",
            buffer_index,
            reg_index,
            bufferQualRFC2819.bufferfull[buffer_index]);

    if (buffer_index>=MAX_QUAL_RFC2819_BUFFERS) return(-1);

    if (reg_index==-1) {
        if (bufferQualRFC2819.bufferfull[buffer_index]==1)
            reg_index = bufferQualRFC2819.wrptr[buffer_index];
        else {
            if (bufferQualRFC2819.wrptr[buffer_index]==0)
                return(-1);
            else
                reg_index = 0;        
        }

    } else {
        if (bufferQualRFC2819.bufferfull[buffer_index]==1) {
            if (reg_index>=MAX_QUAL_RFC2819_REG_NUM) //testa valor fora de gama
                return(-1);

            if (reg_index==bufferQualRFC2819.wrptr[buffer_index]) //Testa fim de buffer
                return(-1);
        } else {
            if (reg_index>=bufferQualRFC2819.wrptr[buffer_index]) //Testa fim de buffer
                return(-1);
        }            
    }

    memcpy((unsigned char *)reg,
           (unsigned char *)&(bufferQualRFC2819.reg[buffer_index][reg_index]),
           sizeof(TBufferRegQualRFC2819));

    if (reg_index==MAX_QUAL_RFC2819_REG_NUM-1)
        reg_index = 0;
    else
        reg_index ++;

    return(reg_index);
}


/**
 * ----------------------------------------------------------------------
 * Function name: ptin_rfc2819_buffer_get_inv
 * ----------------------------------------------------------------------
 * Description:     
 *   
 * ----------------------------------------------------------------------
 * 
 * @param buffer_index: 
 * @param reg_index: 
 * @param reg: 
 * ----------------------------------------------------------------------
 * 
 * @return L7_int: 
 * ----------------------------------------------------------------------
 * @author clemos (2/23/2015)
 * ----------------------------------------------------------------------
 */
L7_int ptin_rfc2819_buffer_get_inv(L7_int buffer_index, L7_int reg_index, TBufferRegQualRFC2819 *reg)
{
    LOG_TRACE(LOG_CTX_RFC2819, "buffer_index=%d, reg_index=%d, full=%d ",
              buffer_index,
              reg_index,
              bufferQualRFC2819.bufferfull[buffer_index]);

    if (buffer_index>=MAX_QUAL_RFC2819_BUFFERS) return(-1);

    if (reg_index==-1) {
        if (bufferQualRFC2819.bufferfull[buffer_index]==1) {   //OK
            if (bufferQualRFC2819.wrptr[buffer_index]==0)
                reg_index = MAX_QUAL_RFC2819_REG_NUM-1;
            else
                reg_index = bufferQualRFC2819.wrptr[buffer_index]-1; 
        } else { //OK
            if (bufferQualRFC2819.wrptr[buffer_index]==0)
                return(-1);
            else
                reg_index = bufferQualRFC2819.wrptr[buffer_index]-1;   //Alterado
        }

    } else {
        if (bufferQualRFC2819.bufferfull[buffer_index]==1) {
            if (reg_index>=MAX_QUAL_RFC2819_REG_NUM) {              //testa valor fora de gama  OK
                return(-1);            
            }

            if (reg_index==bufferQualRFC2819.wrptr[buffer_index]) {//Testa fim de buffer OK
                return(-1);            
            }
        } else {
            if (reg_index>=bufferQualRFC2819.wrptr[buffer_index]) {//Testa fim de buffer OK
                return(-1);            
            }
        }
    }

    memcpy((unsigned char *)reg,
           (unsigned char *)&(bufferQualRFC2819.reg[buffer_index][reg_index]),
           sizeof(TBufferRegQualRFC2819));

    if (reg_index==0)
        reg_index = MAX_QUAL_RFC2819_REG_NUM-1;
    else
        reg_index --;

    return(reg_index);
}



/**
 * ----------------------------------------------------------------------
 * Function name: ptin_rfc2819_buffer_fill
 * ----------------------------------------------------------------------
 * Description:     
 *   
 * ----------------------------------------------------------------------
 * 
 * @param buffer: 
 * @param n_elementos: 
 * ----------------------------------------------------------------------
 * 
 * @return L7_RC_t: L7_SUCCESS
 * ----------------------------------------------------------------------
 * @author clemos (2/23/2015)
 * ----------------------------------------------------------------------
 */
L7_RC_t ptin_rfc2819_buffer_fill(L7_uint8 buffer, L7_int n_elementos) 
{
    L7_int j;
    static TBufferRegQualRFC2819 aux_buffer;


    if (buffer>=MAX_QUAL_RFC2819_BUFFERS) {
        LOG_TRACE(LOG_CTX_RFC2819, "buffer overflow (%d)",buffer);
    }


    for (j=0;j<n_elementos;j++) {
        aux_buffer.index = j;

        aux_buffer.time   = time(NULL);
        aux_buffer.time   = aux_buffer.time - (aux_buffer.time % 60);
        ptin_rfc2819_buffer_write(buffer,&aux_buffer,0);
    }
    
    return L7_SUCCESS;
}


