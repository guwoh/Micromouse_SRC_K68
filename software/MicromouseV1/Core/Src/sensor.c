/*
 * sensor.c
 *
 *  Created on: Mar 7, 2026
 *      Author: kinhk
 */

#include "main.h"

uint16_t sensorData[4];
float RFSensor;
float LFSensor;
float RSensor;
float LSensor;
int16_t RFSensor_raw;
int16_t LFSensor_raw;
int16_t RSensor_raw;
int16_t LSensor_raw;

int16_t RThreshold  = 700;
int16_t LThreshold = 700;
int16_t RFThreshold1 = 1500;
int16_t LFThreshold1 = 1500;
int16_t RFThreshold2 = 280;
int16_t LFThreshold2 = 390;
float gainSensor;

uint8_t wall_R_Check = 0;
uint8_t wall_L_Check = 0;
uint8_t wall_RF_Check = 0;
uint8_t wall_LF_Check = 0;

volatile uint8_t wall_R_Check2 = 1;
volatile uint8_t wall_L_Check2 = 1;
volatile uint8_t wall_RF_Check2 = 0;
volatile uint8_t wall_LF_Check2 = 0;

float wall_L_sum = 0;
float wall_R_sum = 0;

void init_sensor(ADC_HandleTypeDef *hadc) {
	LF_EM_OFF;
	RF_EM_OFF;
	SIDE_EM_OFF;
//	HAL_ADC_Start_DMA(hadc, (uint32_t*) sensorData, 4);
	read_sensor(hadc);
	controlFlag = 0;
	gainSensor = (float) RSensor_raw / (float) LSensor_raw;
	LSensor = gainSensor * LSensor_raw;
	RThreshold = (uint16_t) RSensor;
	LThreshold = (uint16_t) LSensor;
}

void read_sensor(ADC_HandleTypeDef *hadc) {
	micros();
	uint32_t curt = Micros;

	//right front sensor
	HAL_ADC_Start_DMA(hadc, (uint32_t*) sensorData, 4);
//	elapseMicros(10, curt);
	RF_EM_ON
	;
	elapseMicros(60, curt);
	RFSensor_raw = sensorData[2];
	RF_EM_OFF
	;

	elapseMicros(140, curt);
	//left front sensor
	LF_EM_ON
	;
	elapseMicros(200, curt);
	LFSensor_raw = sensorData[1];
	LF_EM_OFF
	;

	elapseMicros(280, curt);
	//diagonal sensors
	SIDE_EM_ON
	;
	elapseMicros(340, curt);
	LSensor_raw = sensorData[0];
	RSensor_raw = sensorData[3];
	SIDE_EM_OFF
	;

	HAL_ADC_Stop_DMA(hadc);
	RFSensor = RFSensor_raw;
	LFSensor = LFSensor_raw;
	LSensor = (float) LSensor_raw * gainSensor;
	RSensor = RSensor_raw;
}

void wall_detect() {
	if (RSensor > (RThreshold - 500)) {
		wall_R_Check = 1;
	}
	else{
		wall_R_Check = 0;

	}

	if (LSensor > (LThreshold - 500)) {
		wall_L_Check = 1;

	}
	else{
		wall_L_Check = 0;

	}
}

void wall_front_detect() {
	if (RFSensor > RFThreshold2 - 100) {
		wall_RF_Check = 1;

	}
	else {
		wall_RF_Check = 0;


	}
	if (LFSensor > LFThreshold2) {

		wall_LF_Check = 1;

	}
	else {

		wall_LF_Check = 0;

	}
}

int16_t sensorError() {
	wall_detect();
	if (wall_R_Check && wall_L_Check) {
		return RSensor - LSensor;
	}
	else if (wall_L_Check) {
		return LThreshold - LSensor;
	}
	else if (wall_R_Check) {
		return -RThreshold + RSensor;
	}
	else
		return 0;
}

void wall_detect2() {
	if (RFSensor > (RFThreshold2 - 50)) {
		wall_RF_Check2 = 1;
	}
	else {
		wall_RF_Check2 = 0;
	}

	if (LFSensor > (LFThreshold2 - 50)) {
		wall_LF_Check2 = 1;
	}
	else {
		wall_LF_Check2 = 0;
	}
	if (RSensor > (RThreshold - 400)) {
		wall_R_Check2 = 1;
	}
	else{
		wall_R_Check2 = 0;

	}
	if (LSensor > (LThreshold - 500)) {
		wall_L_Check2 = 1;
	}
	else{
		wall_L_Check2 = 0;
	}
}
