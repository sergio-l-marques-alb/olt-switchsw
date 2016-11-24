/************************************************************************
*                                                                       *
* FileName:                                                             *
*            i2c.h                                                      *
*                                                                       *
* Projecto:                                                             *
*            PR710_SDH_RA                                               *
*                                                                       *
* Descrição: Header file para o modulo i2c.c                            *
*                                                                       *
*                                                                       *
* Histórico:                                                            *
*            12 Fevereiro 2003 - primeira versão testada                *
*                                                                       *
*                                                                       *
* Autor: Celso Lemos                                                    *
************************************************************************/
#ifndef _I2C_H_
#define _I2C_H_

#ifdef __cplusplus
extern "C"
#else
extern
#endif
void i2c_mem_reset(volatile unsigned char *ctrl_reg);
/*********************************************************************************/

#ifdef __cplusplus
extern "C"
#else
extern
#endif
void i2c_init(volatile unsigned char *ctrl_reg);
/*********************************************************************************/

#ifdef __cplusplus
extern "C"
#else
extern
#endif
int i2c_send(volatile unsigned char *ctrl_reg, unsigned char device, unsigned char sub_addr, unsigned char length, unsigned char *data);
/*********************************************************************************/

#ifdef __cplusplus
extern "C"
#else
extern
#endif
int i2c_receive(volatile unsigned char *ctrl_reg, unsigned char device, unsigned char sub_addr, unsigned char length, unsigned char *data);
/*********************************************************************************/


#ifdef __cplusplus
extern "C"
#else
extern
#endif
int i2c_send_byte(volatile unsigned char *ctrl_reg, unsigned char device, unsigned char sub_addr, unsigned char data);
/*********************************************************************************/

#ifdef __cplusplus
extern "C"
#else
extern
#endif
unsigned char i2c_receive_byte(volatile unsigned char *ctrl_reg, unsigned char device, unsigned char sub_addr);
/*********************************************************************************/

#ifdef __cplusplus
extern "C"
#else
extern
#endif
void write_byte(volatile unsigned char *ctrl_reg,unsigned char dev_addr, unsigned char data_addr, unsigned char data);



#ifdef __cplusplus
extern "C"
#else
extern
#endif
unsigned char Random_read_byte(volatile unsigned char *ctrl_reg, unsigned char dev_addr, unsigned char data_addr);


#ifdef __cplusplus
extern "C"
#else
extern
#endif
void mem_reset(volatile unsigned char *ctrl_reg);

#endif //_I2C_H_
