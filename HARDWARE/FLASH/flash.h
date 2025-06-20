#ifndef __FLASH_H
#define __FLASH_H

#include "stm32f10x.h"
#include "sys.h"


//#define STARTADDR 0x0801F000 //风机数据的起始地址
//#define STARTADDR_ALARM 0x0801F290 //报警数据的起始地址
//#define STARTADDR_HZ 0x0801F300 //变频数据的起始地址

// 调整后的地址（每类数据独占一页）
#define STARTADDR       0x0801F000  // 风机数据（第496页）
#define STARTADDR_ALARM 0x0801F400  // 报警数据（第497页）
#define STARTADDR_HZ    0x0801F800  // 变频数据（第498页）

// E9E0  EA00
extern volatile FLASH_Status FLASHStatus;


void WriteFlashOneWord(uint32_t WriteAddress, uint32_t WriteData);
//void WriteFlashData(uint32_t WriteAddress, uint16_t data[], uint32_t num);
void WriteFlashData(uint32_t WriteAddress, uint16_t data[], uint32_t num, uint16_t alarm_data[], uint32_t alarm_num, uint16_t hz_data[], uint32_t hz_num);
int ReadFlashNBtye(uint32_t ReadAddress, uint8_t *ReadBuf, int32_t ReadNum);
void ReadFlashData(uint32_t ReadAddress, uint8_t *dest_Data, uint32_t num);


void WriteFlashData_relay(uint32_t WriteAddress, uint16_t data[], uint32_t num);
void WriteFlashData_alarm(uint32_t WriteAddress, uint16_t alarm_data[], uint32_t alarm_num);
void WriteFlashData_hz(uint32_t WriteAddress, uint16_t hz_data[], uint32_t hz_num);


#endif

