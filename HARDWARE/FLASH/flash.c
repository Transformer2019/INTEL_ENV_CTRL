#include "flash.h"

volatile FLASH_Status FLASHStatus = FLASH_BUSY; //Flash操作状态变量


/*
 * Name:	    WriteFlashOneWord
 * Function:	向内部Flash写入32位数据
 * Input:	    WriteAddress：数据要写入的目标地址（偏移地址）
 *              WriteData：   写入的数据
 */
void WriteFlashOneWord(uint32_t WriteAddress, uint32_t WriteData)
{   
    FLASH_UnlockBank1();
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);

    FLASHStatus = 1;    //清空状态指示标志位
    FLASHStatus = FLASH_ErasePage(STARTADDR);  
    if(FLASHStatus == FLASH_COMPLETE)   
    {  
        FLASHStatus = 1;    //清空状态指示标志位
        FLASHStatus = FLASH_ProgramWord(STARTADDR+WriteAddress, WriteData); //flash.c 中API函数
    }
    
    FLASHStatus = 1;    //清空状态指示标志位
    FLASH_LockBank1();    
}

/*
 * Name:	    WriteFlashData
 * Function:	向内部Flash写入数据
 * Input:	    WriteAddress：数据要写入的目标地址（偏移地址）
 *             data[]：      写入的数据首地址
 *             num：         写入数据的个数
 */
void WriteFlashData(uint32_t WriteAddress, uint8_t data[], uint32_t num)
{
    uint32_t i = 0;
    uint16_t temp = 0;
    
	FLASH_UnlockBank1();    //解锁flash
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR); 
    
    FLASHStatus = 1;        //清空状态指示标志位
    FLASHStatus = FLASH_ErasePage(STARTADDR);//擦除整页
	if(FLASHStatus == FLASH_COMPLETE)//flash操作完成
	{
        FLASHStatus = 1;    //清空状态指示标志位
        for(i=0; i<num; i++)
        {
            temp = (uint16_t)data[i];
            FLASHStatus = FLASH_ProgramHalfWord(STARTADDR+WriteAddress+i*2, temp);//写入数据
        }
	}
    
    FLASHStatus = 1;    //清空状态指示标志位
    
	FLASH_LockBank1();  //锁定flash
} 



/*
 * Name:	    ReadFlashNBtye
 * Function:	从内部Flash读取N字节数据
 * Input:	    ReadAddress：数据地址（偏移地址）
 *              ReadBuf：读取到的数据存放位置指针
 *              ReadNum：读取字节个数
 * Output:      读取的字节数
 */
int ReadFlashNBtye(uint32_t ReadAddress, uint8_t *ReadBuf, int32_t ReadNum)
{   
    int DataNum = 0;
    
    ReadAddress = (uint32_t)STARTADDR + ReadAddress;  
    while(DataNum < ReadNum)   
    {        
        *(ReadBuf + DataNum) = *(__IO uint8_t*) ReadAddress++;  
        DataNum++;     
    }
    
    return DataNum;    
}

/*
 * Name:	    ReadFlashData
 * Function:	从内部Flash读取num字节数据
 * Input:	    ReadAddress：数据地址（偏移地址）
 *              dest_Data：  读取到的数据存放位置指针
 *              num：        读取字节个数
 */
void ReadFlashData(uint32_t ReadAddress, uint8_t *dest_Data, uint32_t num)
{
    int i = 0;
    ReadAddress = (uint32_t)STARTADDR + ReadAddress; 
    while(i < num) 
    {
        *(dest_Data+i) = *(__IO uint16_t*) ReadAddress;
        ReadAddress += 2;
        
        i++;
    }
}
