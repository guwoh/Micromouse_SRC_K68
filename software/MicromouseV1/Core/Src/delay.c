/*
 * delay.c
 *
 *  Created on: Mar 4, 2026
 *      Author: kinhk
 */


#include "delay.h"


volatile uint32_t Micros;
volatile uint32_t Millis;


void Systick_Configuration(void)
{
	SystemInit();
	SystemCoreClockUpdate();

	//systemFrequency = SystemCoreClock / 1000000;
	if (SysTick_Config (SystemCoreClock / 1000000)) //1ms per interrupt
		while (1);

	Millis = 0;//reset Millis
}

uint32_t micros(void)
{
	Micros = Millis*1000 + 1000 - SysTick->VAL/systemFrequency;//=Millis*1000+(SystemCoreClock/1000-SysTick->VAL)/168;
	return Micros;
}

uint32_t millis(void)
{
	return Millis;
}

void delay_ms(uint32_t nTime)
{
	uint32_t curTime = Millis;
	while((nTime-(Millis-curTime)) > 0);

}
void delay_us(uint32_t nTime)
{
	uint32_t curTime = micros();
	while((nTime-(micros()-curTime)) > 0);
}

void elapseMicros(uint32_t targetTime, uint32_t oldt)
{
	while((micros()-oldt)<targetTime);
}


void elapseMillis(uint32_t targetTime, uint32_t oldt)
{
	while((millis()-oldt)<targetTime);
}
