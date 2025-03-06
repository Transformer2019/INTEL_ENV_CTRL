#ifndef __FLASH_H
#define __FLASH_H

#include "stm32f10x.h"
#include "sys.h"


#define STARTADDR 0x0801E800 //风机数据的起始地址
#define STARTADDR_ALARM 0x0801E9E0 //报警数据的起始地址
#define STARTADDR_HZ 0x0801EA00 //变频数据的起始地址
// E9E0  EA00
extern volatile FLASH_Status FLASHStatus;


void WriteFlashOneWord(uint32_t WriteAddress, uint32_t WriteData);
//void WriteFlashData(uint32_t WriteAddress, uint16_t data[], uint32_t num);
void WriteFlashData(uint32_t WriteAddress, uint16_t data[], uint32_t num, uint16_t alarm_data[], uint32_t alarm_num, uint16_t hz_data[], uint32_t hz_num);
int ReadFlashNBtye(uint32_t ReadAddress, uint8_t *ReadBuf, int32_t ReadNum);
void ReadFlashData(uint32_t ReadAddress, uint8_t *dest_Data, uint32_t num);


#endif

