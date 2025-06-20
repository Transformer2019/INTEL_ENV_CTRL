#ifndef __FLASH_H
#define __FLASH_H

#include "stm32f10x.h"
#include "sys.h"


//#define STARTADDR 0x0801F000 //������ݵ���ʼ��ַ
//#define STARTADDR_ALARM 0x0801F290 //�������ݵ���ʼ��ַ
//#define STARTADDR_HZ 0x0801F300 //��Ƶ���ݵ���ʼ��ַ

// ������ĵ�ַ��ÿ�����ݶ�ռһҳ��
#define STARTADDR       0x0801F000  // ������ݣ���496ҳ��
#define STARTADDR_ALARM 0x0801F400  // �������ݣ���497ҳ��
#define STARTADDR_HZ    0x0801F800  // ��Ƶ���ݣ���498ҳ��

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

