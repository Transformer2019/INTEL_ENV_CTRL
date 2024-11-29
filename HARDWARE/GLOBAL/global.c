#ifndef __GLOBAL_H
#define __GLOBAL_H
#include "global.h"



// 读取温度 (错误)
//这个错误通常是因为在初始化全局变量或常量时调用了一个函数。C 语言不允许在编译时初始化常量或全局变量时使用函数调用。你需要确保初始化时只使用常量值。
//解决方法
//1.检查全局变量初始化:
//确保所有全局变量或常量的初始化不包含函数调用。
//2.延迟初始化:
//如果需要使用函数的返回值来初始化变量，请在主函数或初始化函数中进行。

//// 错误示例
//int someValue = getInitialValue();  // 假设 getInitialValue() 是一个函数
//// 正确示例
//int someValue;  // 先声明变量
//void init() {
//    someValue = getInitialValue();  // 在函数中初始化
//}
//你可以在程序的初始化部分调用必要的函数来设置这些变量。这样就可以避免在编译时初始化它们。

//float temperature1 = Read_Temperature(ADC_Channel_3);
//float temperature2 = Read_Temperature(ADC_Channel_4);
//float temperature3 = Read_Temperature(ADC_Channel_2);
//float average_temp = (temperature1 + temperature2 + temperature3) / 3.0;

//float temperature1 = 0.0;
//float temperature2 = 0.0;
//float temperature3 = 0.0;
//float average_temp = 0.0;



#endif
