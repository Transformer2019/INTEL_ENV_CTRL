#include "ntc.h"
#include "math.h"
#include "delay.h"

//u16 ADC_ConvertValue[3]={0,0,0};

void ADC_Configuration(void) {
    ADC_InitTypeDef ADC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    // 使能 ADC1 时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

    // 使能 GPIOA 时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // 配置 ADC 输入引脚
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 配置 ADC 参数
    //ADC_DeInit(ADC1);
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);

    //配置 ADC 通道
    //ADC_RegularChannelConfig(ADC1, ADC_CHANNEL, 1, ADC_SampleTime_55Cycles5);

    // 使能 ADC
    ADC_Cmd(ADC1, ENABLE);
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);//配置ADC时钟为PCLK2的8分频

    // 校准 ADC
    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1));
}


//void ADC_Configuration(void) {
//	
//	GPIO_InitTypeDef GPIO_InitStructure; //定义结构体变量	
//	ADC_InitTypeDef       ADC_InitStructure;
//	DMA_InitTypeDef DMA_InitStructure;
//	
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_ADC1,ENABLE);
//	// 打开DMA时钟
//	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

//	// 复位DMA控制器
//	DMA_DeInit(DMA1_Channel1);
//	
//	// 配置 DMA 初始化结构体
//	// 外设基址为：ADC 数据寄存器地址
//	DMA_InitStructure.DMA_PeripheralBaseAddr = ( u32 ) ( & ( ADC1->DR ) );
//	
//	// 存储器地址，实际上就是一个内部SRAM的变量
//	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)ADC_ConvertValue;
//	
//	// 数据源来自外设
//	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
//	
//	//数据数 
//	DMA_InitStructure.DMA_BufferSize = 3;
//	
//	// 外设寄存器只有一个，地址不用递增
//	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;

//	// 存储器地址固定
//	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; 
//	
//	// 外设数据大小为半字，即两个字节
//	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
//	
//	// 内存数据大小也为半字，跟外设数据大小相同
//	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
//	
//	// 循环传输模式
//	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;	

//	// DMA 传输通道优先级为高，当使用一个DMA通道时，优先级设置不影响
//	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
//	
//	// 禁止存储器到存储器模式，因为是从外设到存储器
//	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
//	
//	// 初始化DMA
//	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
//	
//	// 使能 DMA 通道
//	DMA_Cmd(DMA1_Channel1 , ENABLE);
//	
//	
//	RCC_ADCCLKConfig(RCC_PCLK2_Div6);//设置ADC分频因子6 72M/6=12,ADC最大时间不能超过14M
//	
//	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;//ADC
//	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AIN;	//模拟输入
//	GPIO_Init(GPIOA,&GPIO_InitStructure);
//	
//	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
//	ADC_InitStructure.ADC_ScanConvMode = ENABLE;//扫描模式	
//	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;//打开连续转换
//	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//禁止触发检测，使用软件触发
//	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//右对齐	
//	ADC_InitStructure.ADC_NbrOfChannel = 3;//1个转换在规则序列中 也就是只转换规则序列1 
//	ADC_Init(ADC1, &ADC_InitStructure);//ADC初始化
//	
//	ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 1, ADC_SampleTime_239Cycles5);	//ADC1,ADC通道,239.5个周期,提高采样时间可以提高精确度			    
//	ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 2, ADC_SampleTime_239Cycles5);	//ADC1,ADC通道,239.5个周期,提高采样时间可以提高精确度			    
//	ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 3, ADC_SampleTime_239Cycles5);

//	
//	ADC_Cmd(ADC1, ENABLE);//开启AD转换器
//	
//	// 使能ADC DMA 请求
//	ADC_DMACmd(ADC1, ENABLE);
//	
//	ADC_ResetCalibration(ADC1);//重置指定的ADC的校准寄存器
//	while(ADC_GetResetCalibrationStatus(ADC1));//获取ADC重置校准寄存器的状态
//	
//	ADC_StartCalibration(ADC1);//开始指定ADC的校准状态
//	while(ADC_GetCalibrationStatus(ADC1));//获取指定ADC的校准程序

//	ADC_SoftwareStartConvCmd(ADC1, ENABLE);//使能或者失能指定的ADC的软件转换启动功能

//}

float Read_Temperature(uint8_t channel){
	
	// 配置 ADC 通道
    ADC_RegularChannelConfig(ADC1, channel, 1, ADC_SampleTime_7Cycles5);
	
    // 启动 ADC 转换
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);

    // 等待转换完成
    while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));

    // 读取 ADC 值
    uint16_t adc_value = ADC_GetConversionValue(ADC1);

    // 计算电压值
    float voltage = (float)adc_value / 4095.0 * 3.3;

    // 计算NTC电阻值
    float ntc_resistance = (voltage * R1) / (3.3 - voltage);
    // 计算温度
    float temperature = 1.0 / ((1.0 / (ROOM_TEMPERATURE+273.5)) + (1.0 / B_VALUE) * log(ntc_resistance/REFERENCE_RESISTANCE));

    return temperature - 273.15; // 转换为摄氏温度


}
