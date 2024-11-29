#include "ntc.h"
#include "math.h"
#include "delay.h"

//u16 ADC_ConvertValue[3]={0,0,0};

void ADC_Configuration(void) {
    ADC_InitTypeDef ADC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    // ʹ�� ADC1 ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

    // ʹ�� GPIOA ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // ���� ADC ��������
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // ���� ADC ����
    //ADC_DeInit(ADC1);
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);

    //���� ADC ͨ��
    //ADC_RegularChannelConfig(ADC1, ADC_CHANNEL, 1, ADC_SampleTime_55Cycles5);

    // ʹ�� ADC
    ADC_Cmd(ADC1, ENABLE);
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);//����ADCʱ��ΪPCLK2��8��Ƶ

    // У׼ ADC
    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1));
}


//void ADC_Configuration(void) {
//	
//	GPIO_InitTypeDef GPIO_InitStructure; //����ṹ�����	
//	ADC_InitTypeDef       ADC_InitStructure;
//	DMA_InitTypeDef DMA_InitStructure;
//	
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_ADC1,ENABLE);
//	// ��DMAʱ��
//	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

//	// ��λDMA������
//	DMA_DeInit(DMA1_Channel1);
//	
//	// ���� DMA ��ʼ���ṹ��
//	// �����ַΪ��ADC ���ݼĴ�����ַ
//	DMA_InitStructure.DMA_PeripheralBaseAddr = ( u32 ) ( & ( ADC1->DR ) );
//	
//	// �洢����ַ��ʵ���Ͼ���һ���ڲ�SRAM�ı���
//	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)ADC_ConvertValue;
//	
//	// ����Դ��������
//	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
//	
//	//������ 
//	DMA_InitStructure.DMA_BufferSize = 3;
//	
//	// ����Ĵ���ֻ��һ������ַ���õ���
//	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;

//	// �洢����ַ�̶�
//	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; 
//	
//	// �������ݴ�СΪ���֣��������ֽ�
//	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
//	
//	// �ڴ����ݴ�СҲΪ���֣����������ݴ�С��ͬ
//	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
//	
//	// ѭ������ģʽ
//	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;	

//	// DMA ����ͨ�����ȼ�Ϊ�ߣ���ʹ��һ��DMAͨ��ʱ�����ȼ����ò�Ӱ��
//	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
//	
//	// ��ֹ�洢�����洢��ģʽ����Ϊ�Ǵ����赽�洢��
//	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
//	
//	// ��ʼ��DMA
//	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
//	
//	// ʹ�� DMA ͨ��
//	DMA_Cmd(DMA1_Channel1 , ENABLE);
//	
//	
//	RCC_ADCCLKConfig(RCC_PCLK2_Div6);//����ADC��Ƶ����6 72M/6=12,ADC���ʱ�䲻�ܳ���14M
//	
//	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;//ADC
//	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AIN;	//ģ������
//	GPIO_Init(GPIOA,&GPIO_InitStructure);
//	
//	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
//	ADC_InitStructure.ADC_ScanConvMode = ENABLE;//ɨ��ģʽ	
//	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;//������ת��
//	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//��ֹ������⣬ʹ���������
//	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//�Ҷ���	
//	ADC_InitStructure.ADC_NbrOfChannel = 3;//1��ת���ڹ��������� Ҳ����ֻת����������1 
//	ADC_Init(ADC1, &ADC_InitStructure);//ADC��ʼ��
//	
//	ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 1, ADC_SampleTime_239Cycles5);	//ADC1,ADCͨ��,239.5������,��߲���ʱ�������߾�ȷ��			    
//	ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 2, ADC_SampleTime_239Cycles5);	//ADC1,ADCͨ��,239.5������,��߲���ʱ�������߾�ȷ��			    
//	ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 3, ADC_SampleTime_239Cycles5);

//	
//	ADC_Cmd(ADC1, ENABLE);//����ADת����
//	
//	// ʹ��ADC DMA ����
//	ADC_DMACmd(ADC1, ENABLE);
//	
//	ADC_ResetCalibration(ADC1);//����ָ����ADC��У׼�Ĵ���
//	while(ADC_GetResetCalibrationStatus(ADC1));//��ȡADC����У׼�Ĵ�����״̬
//	
//	ADC_StartCalibration(ADC1);//��ʼָ��ADC��У׼״̬
//	while(ADC_GetCalibrationStatus(ADC1));//��ȡָ��ADC��У׼����

//	ADC_SoftwareStartConvCmd(ADC1, ENABLE);//ʹ�ܻ���ʧ��ָ����ADC�����ת����������

//}

float Read_Temperature(uint8_t channel){
	
	// ���� ADC ͨ��
    ADC_RegularChannelConfig(ADC1, channel, 1, ADC_SampleTime_7Cycles5);
	
    // ���� ADC ת��
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);

    // �ȴ�ת�����
    while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));

    // ��ȡ ADC ֵ
    uint16_t adc_value = ADC_GetConversionValue(ADC1);

    // �����ѹֵ
    float voltage = (float)adc_value / 4095.0 * 3.3;

    // ����NTC����ֵ
    float ntc_resistance = (voltage * R1) / (3.3 - voltage);
    // �����¶�
    float temperature = 1.0 / ((1.0 / (ROOM_TEMPERATURE+273.5)) + (1.0 / B_VALUE) * log(ntc_resistance/REFERENCE_RESISTANCE));

    return temperature - 273.15; // ת��Ϊ�����¶�


}
