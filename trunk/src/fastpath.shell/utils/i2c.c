/************************************************************************
*                                                                       *
* FileName:                                                             *
*            i2c.c                                                      *
*                                                                       *
* Projecto:                                                             *
*            PR710_SDH_RA                                               *
*                                                                       *
* Descrição: Este módulo possui as funções necessárias para controlo    *
*            de chips I2C                                               *
*                                                                       *
*                                                                       *
* Histórico:                                                            *
*            12 Fevereiro 2003 - primeira versão testada                *
*                                                                       *
*                                                                       *
* Autor: Celso Lemos                                                    *
************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "addrmap.h"
#include "i2c.h"
//ini claudia
#include "globals.h"
//fim claudia

/* delay implementado com ciclo for - depende da velocidade do processador */
static uint8 psleep;
#define SLEEP  {for(psleep=0;psleep<100;psleep++);}
/********************************************************************************/

/* definicao de macros que implementam os estimulos para a serial eprom      */
#define SCL_0 {SLEEP; *ctrl_reg &= 0x0E; SLEEP}
#define SCL_1 {SLEEP; *ctrl_reg |= 0x01; SLEEP}

#define SDA_0 {SLEEP; *ctrl_reg &= 0x0D; SLEEP}
#define SDA_1 {SLEEP; *ctrl_reg |= 0x02; SLEEP}

#define MOD_RD {SLEEP; *ctrl_reg  |= 0x08; SLEEP}
#define MOD_WR {SDA_1; SLEEP; *ctrl_reg  &= 0x07; SLEEP}

#define I2C_START {SCL_1; SDA_1; MOD_WR; SLEEP; SDA_0; SCL_0}

#define I2C_STOP  {SCL_0; SDA_0; SCL_1; SLEEP; SDA_1; SLEEP}
/********************************************************************************/



/* Escreve um byte no i2c */
int i2c_putbyte(volatile uint8 *ctrl_reg, uint8 val)
{
    int i;
    
    MOD_WR;
    for (i=7;i>=0;i--) 
    {
        if(val & (1<<i))
            SDA_1
        else
            SDA_0;
        SCL_1;
        SCL_0;
    }
    SDA_1;

    MOD_RD;
    SCL_1;
    val = *ctrl_reg & 0x04;
    SCL_0;
    if((val & 0x4)!=0) { 
        //printf("ACK_ERROR_PUT\n\r");
        return(-1);
    }
    MOD_WR;
    return(0);
}


/* le um byte no i2c */
uint8 i2c_getbyte(volatile uint8 *ctrl_reg, uint8 last)
{
    int i;
    uint8 c,b=0;

    SDA_1;
    MOD_RD;
    for (i=7;i>=0;i--) {

        SCL_1;

        c = (((*ctrl_reg) & 0x04)>>2);

        b<<=1;
        if(c) b |= 1;
        SCL_0;
    }

    MOD_WR;

    if(last==0)
        SDA_1
    else
        SDA_0;

    SCL_1;
    SCL_0;
    SDA_1;

    return(b);
}



/* mem reset */
void i2c_mem_reset(volatile uint8 *ctrl_reg)
{
    uint8 i;
    MOD_RD;
    SCL_0;
    for(i=0;i<9;i++){
        SCL_1;
        SCL_0;
    }

    MOD_WR;
    I2C_START;

/*

    MOD_RD;
    SCL_0;
    for(i=0;i<9;i++){
        SLEEP;
        SCL_1;
        aux = *ctrl_reg & 0x04;
        if(aux==0x04) {
            SDA_1;
            MOD_WR;
            I2C_START;
            return;
        }
        SCL_0;
        SLEEP;
    }
    printf("ERROR IN DS1848 RESET\n\r");
    MOD_WR;
    SCL_1;
    SDA_1;
    */
}


/****** FUNCOES PUBLICAS *******/
/*******************************/

/* init do i2c */
void i2c_init(volatile uint8 *ctrl_reg)
{    
    MOD_WR;
    SDA_1;
    SCL_1;
}

/* envia bytes para o 12c */
int i2c_send(volatile uint8 *ctrl_reg, uint8 device, uint8 sub_addr, uint8 length, uint8 *data)
{
    int ret=0;
    
    MOD_WR;
    
    I2C_START;                        //start condition
    ret |=i2c_putbyte(ctrl_reg,device);     //send device address
    ret |=i2c_putbyte(ctrl_reg,sub_addr);   //send subadress

    //send data
    while (length--)
        ret |=i2c_putbyte(ctrl_reg,*data++);

    SDA_0;
    I2C_STOP;

    if(ret==0)
        return(0);
    else 
        return(-1);
}



/* recebe bytes para o 12c */
int i2c_receive(volatile uint8 *ctrl_reg, uint8 device, uint8 sub_addr, uint8 length, uint8 *data)
{
    int ret=0;
    int j = length;
    uint8 *p = data;

    MOD_WR;
    
    I2C_START;                               //start condition
    ret |= i2c_putbyte(ctrl_reg,device);     //send device address
    
    if(sub_addr<0xFF) {    
        ret |= i2c_putbyte(ctrl_reg,sub_addr);    //send subadress
        SLEEP;
        SCL_1;
        I2C_START;
    }

    ret |= i2c_putbyte(ctrl_reg,device | 0x1);

    //receive data bytes
    while (j--)
        *p++ = i2c_getbyte(ctrl_reg,j);

    SDA_0;
    I2C_STOP;

    if(ret==0)
        return(0);
    else 
        return(-1);
}





/* envia 1 byte para o i2c */
int i2c_send_byte(volatile uint8 *ctrl_reg, uint8 device, uint8 sub_addr, uint8 data)
{
    int ret=0;
    MOD_WR;
    
    I2C_START;                        //start condition
    ret |= i2c_putbyte(ctrl_reg,device);     //send device address

    if(sub_addr<0xFF)
      ret |= i2c_putbyte(ctrl_reg,sub_addr);    //send subadress

    ret |= i2c_putbyte(ctrl_reg,data);

    SDA_0;
    I2C_STOP;
    
    if(ret==0)
        return(0);
    else 
        return(-1);

}



/* recebe bytes para o i2c */
uint8 i2c_receive_byte(volatile uint8 *ctrl_reg, uint8 device, uint8 sub_addr)
{

    uint8 data;

    MOD_WR;
    
    I2C_START;                        //start condition
    i2c_putbyte(ctrl_reg,device);     //send device address
    i2c_putbyte(ctrl_reg,sub_addr);   //send subadress
    SLEEP;
    SCL_1;
    I2C_START;

    i2c_putbyte(ctrl_reg,device | 0x1);

    //receive data bytes
    data=i2c_getbyte(ctrl_reg,0);

    SDA_0;
    I2C_STOP;

    return(data);
}






/*********************************************************************************
* Função:         write_byte                                                     *
*                                                                                *
* Descrição:      Escreve um byte na eprom nume determinado endereço             *
*                                                                                *
* Parametros:                                                                    *
*			  ctrl_reg: Addr do registo de controlo dos sinais para o chip       *
*             dev_addr:  Device adress da seriaç eprom                           *
*             data_addr: Endereço onde se pretendem escrever os dados            *
*             data:      Dados a escrever                                        *
*                                                                                *
* retorno:    sem retorno                                                        *
*                                                                                *
* historico:                                                                     *
*         12 Fevereiro de 2003                                                   *
*                                                                                *
* Autor: Celso Lemos                                                             *
*                                                                                *
*********************************************************************************/
void write_byte(volatile uint8 *ctrl_reg,uint8 dev_addr, uint8 data_addr, uint8 data)
{
    uint8 aux_dev_addr;
    uint8 aux_ack,i;

    aux_dev_addr = 0xA0 | ((dev_addr<<1) & 0xF);

    *ctrl_reg=0;
    MOD_WR;
    //Start
    *ctrl_reg  |= 0x03; //SCL=1,SDA=1
    SLEEP;
    SDA_0;
    SCL_0;
    //fim de start
    
    //Enviar o device address
    for(i=0;i<8;i++) {
      if(aux_dev_addr & 0x80)
          SDA_1
      else 
          SDA_0

      SCL_1
      SCL_0
      aux_dev_addr<<=1;
    }

    //Espera pelo ACK
    MOD_RD;
    aux_ack = *ctrl_reg & 0x04;
    SCL_1;
    SCL_0;
    MOD_WR;

    //Enviar o data address
    for(i=0;i<8;i++) {
      if(data_addr & 0x80)
          SDA_1
      else 
          SDA_0

      SCL_1
      SCL_0
      data_addr<<=1;
    }

    //Espera pelo ACK
    MOD_RD;
    aux_ack = *ctrl_reg & 0x04;
    //printf("ACK_W1=0x%.2x\n\r",aux_ack);
    SCL_1;
    SCL_0;
    MOD_WR;
    //Enviar os dados
    for(i=0;i<8;i++) {
      if(data & 0x80)
          SDA_1
      else 
          SDA_0

      SCL_1
      SCL_0
      data<<=1;
    }

    //Espera ACK
    MOD_RD;
    aux_ack = *ctrl_reg & 0x04;
    SCL_1;
    SCL_0;
    MOD_WR;

    //Stop sequence
    SDA_0;
    SCL_1;
    SDA_1;
    SDA_0;
    SCL_0;
    usleep(50000); //Espera 10ms
}




/*********************************************************************************
* Função:         Read_next_byte                                                 *
*                                                                                *
* Descrição:      Ler um byte da eprom. O endereço e o do endereço seguinte      *
*                 relativamente a ultima leitura. Deve ser usada para leituras   *
*                 sequenciais.                                                   *
*                                                                                *
* Parametros:                                                                    *
*			  ctrl_reg: Addr do registo de controlo dos sinais para o chip       *
*             dev_addr:  Device adress da seriaç eprom                           *
*                                                                                *
* retorno:                                                                       *
*                 Retorna o valor lido                                           *
*                                                                                *
* historico:                                                                     *
*         12 Fevereiro de 2003                                                   *
*                                                                                *
* Autor: Celso Lemos                                                             *
*                                                                                *
*********************************************************************************/
uint8 Read_next_byte(volatile uint8 *ctrl_reg, uint8 dev_addr)
{
    uint8 aux_dev_addr;
    uint8 aux_ack,i;
    uint8 retvalue;

    retvalue=0;
    aux_dev_addr = 0xA1 | ((dev_addr<<1) & 0xF);

    //Start
    *ctrl_reg  |= 0x03; //SCL=1,SDA=1
    SLEEP;
    SDA_0;
    SCL_0;
    //fim de start
    
    //Enviar o device address
    for(i=0;i<8;i++) {
      if(aux_dev_addr & 0x80)
          SDA_1
      else 
          SDA_0

      SCL_1
      SCL_0
      aux_dev_addr<<=1;
    }

    //Espera pelo ACK
    MOD_RD;
    aux_ack = *ctrl_reg & 0x04;
    //printf("ACK_3=0x%.2x\n\r",aux_ack);
    //SCL_1;
    //SCL_0;

    //Ler dados
    for (i=0;i<8;i++) {
       retvalue<<=1;
       SCL_1;
       SCL_0;   
       retvalue |=(((*ctrl_reg) & 0x04)>>2);
    }


    //Envia NACK
    MOD_RD;  //Actua o pull-up
    aux_ack = *ctrl_reg & 0x04;  //Nao era necessario
    SCL_1;
    SCL_0;
    MOD_WR;

    //Stop sequence
    SDA_0;
    SCL_1;
    SDA_1;
    SDA_0;
    SCL_0;
    return(retvalue);
}


/*********************************************************************************
* Função:         Random_read_byte                                               *
*                                                                                *
* Descrição:      Ler um byte da eprom de um detrminado endereço.                *
*                                                                                *
* Parametros:                                                                    *
*			  ctrl_reg: Addr do registo de controlo dos sinais para o chip       *
*             dev_addr:  Device adress da seriaç eprom                           *
*             data_addr: Endereço onde se pretendem escrever os dados            *
*                                                                                *
* retorno:                                                                       *
*                 Retorna o valor lido                                           *
*                                                                                *
* historico:                                                                     *
*         12 Fevereiro de 2003                                                   *
*                                                                                *
* Autor: Celso Lemos                                                             *
*                                                                                *
*********************************************************************************/
uint8 Random_read_byte(volatile uint8 *ctrl_reg, uint8 dev_addr, uint8 data_addr)
{
    uint8 aux_dev_addr;
    uint8 aux_ack,i;
    uint8 retvalue;

    retvalue=0;
    aux_dev_addr = 0xA0 | ((dev_addr<<1) & 0xF);

    //Start
    *ctrl_reg  |= 0x03; //SCL=1,SDA=1
    SLEEP;
    SDA_0;
    SCL_0;
    //fim de start
    
    //Enviar o device address
    for(i=0;i<8;i++) {
      if(aux_dev_addr & 0x80)
          SDA_1
      else 
          SDA_0

      SCL_1
      SCL_0
      aux_dev_addr<<=1;
    }

    //Espera pelo ACK
    MOD_RD;
    aux_ack = *ctrl_reg & 0x04;
    //printf("ACK_1=0x%.2x\n\r",aux_ack);
    SCL_1;
    SCL_0;
    MOD_WR;

    //Enviar o data address
    for(i=0;i<8;i++) {
      if(data_addr & 0x80)
          SDA_1
      else 
          SDA_0

      SCL_1
      SCL_0
      data_addr<<=1;
    }

    //Envia NACK
    MOD_RD;  //Actua o pull-up
    aux_ack = *ctrl_reg & 0x04; //Nao era necessario
    //printf("ACK_2=0x%.2x\n\r",aux_ack);
    SCL_1;
    SCL_0;
    MOD_WR;

    retvalue=Read_next_byte(ctrl_reg,dev_addr);
    return(retvalue);
}



uint8 _Random_read_byte(volatile uint8 *ctrl_reg, uint8 device, uint8 sub_addr)
{
    /*
    uint8 aux_dev_addr;
    uint8 aux_ack,i;
    uint8 retvalue;
    */
    uint8 data;

    MOD_WR;
    
    I2C_START;                        //start condition
    i2c_putbyte(ctrl_reg,device);     //send device address
    i2c_putbyte(ctrl_reg,sub_addr);   //send subadress
    SLEEP;
    SCL_1;
    I2C_START;

    i2c_putbyte(ctrl_reg,device | 0x1);

    //receive data bytes
    data=i2c_getbyte(ctrl_reg,0);

    SDA_0;
    I2C_STOP;

    return(data);
}



/*********************************************************************************
* Função:         mem_reset                                                      *
*                                                                                *
* Descrição:      Reset da memoria (nao apaga o conteudo).                       *
*                                                                                *
* Parametros:                                                                    *
*			  ctrl_reg: Addr do registo de controlo dos sinais para o chip       *
*                                                                                *
* retorno:    sem retorno                                                        *
*                                                                                *
* historico:                                                                     *
*         12 Fevereiro de 2003                                                   *
*                                                                                *
* Autor: Celso Lemos                                                             *
*                                                                                *
*********************************************************************************/
void mem_reset(volatile uint8 *ctrl_reg)
{
    uint8 i;
    MOD_RD;
    SCL_0;
    for(i=0;i<9;i++){
        SCL_1;
        SCL_0;
    }

    MOD_WR;
    //Start
    *ctrl_reg  |= 0x03; //SCL=1,SDA=1
    SLEEP;
    SDA_0;
    SCL_0;
    //fim de start
}


