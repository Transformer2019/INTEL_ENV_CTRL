#ifndef __FLASH_H
#define __FLASH_H

#include "stm32f10x.h"
#include "sys.h"


#define STARTADDR 0x0801FC00 //STM32F103C8T6  ”√

extern volatile FLASH_Status FLASHStatus;


void WriteFlashOneWord(uint32_t WriteAddress, uint32_t WriteData);
void WriteFlashData(uint32_t WriteAddress, uint8_t data[], uint32_t num);
int ReadFlashNBtye(uint32_t ReadAddress, uint8_t *ReadBuf, int32_t ReadNum);
void ReadFlashData(uint32_t ReadAddress, uint8_t *dest_Data, uint32_t num);


#endif

