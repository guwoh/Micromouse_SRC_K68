#include "Sensor.h"

IR_SENSOR IR_sensor;

int16_t debug_sensor;
float alpha = 0.1;

float WALL_L = 0;
float WALL_R = 0;
float WALL_FR = 0;
float WALL_FL = 0;

float IR_FL_raw = 0;
float IR_FR_raw = 0;
float IR_LL_raw = 0;
float IR_RR_raw = 0;

float IR_FL_raw2 = 0;
float IR_FR_raw2 = 0;
float IR_LL_raw2 = 0;
float IR_RR_raw2 = 0;

float IR_FL = 0;
float IR_FR = 0;
float IR_LL = 0;
float IR_RR = 0;

uint16_t IR_data[4];
uint8_t completeADC = 0;

void IR_init(ADC_HandleTypeDef *ADCx) {
	IR_sensor.ADCx = ADCx;
	IR_sensor.prev_ir_state_left = 1;
	IR_sensor.prev_ir_state_right = 1;
	IR_sensor.ir_state_left = 1;
	IR_sensor.ir_state_right = 1;
	IR_sensor.offset = 0;
	HAL_ADC_Start_DMA(IR_sensor.ADCx, (uint32_t*) IR_data, 4);
	RF_EM_ON;
	LF_EM_ON;
	L_EM_ON;
	R_EM_ON;
}

void read_ir_adc() {
	if(completeADC == 0) {
		uint32_t curt = Micros;
		RF_EM_ON;
		IR_FR_raw = IR_data[0];
//		IR_FR_raw2 = (float) IR_data[0] * (1 - alpha) + (float) alpha * IR_FR_raw;
		elapseMicros(100, curt);
		RF_EM_OFF;
		elapseMicros(250,curt);

		LF_EM_ON;
		IR_FL_raw = IR_data[3];
//		IR_FL_raw2 = (float) IR_data[3] * (1 - alpha) + (float) alpha * IR_FL_raw;
		elapseMicros(350,curt);
		LF_EM_OFF;
		elapseMicros(500,curt);

		L_EM_ON;
		R_EM_ON;
		IR_RR_raw = IR_data[1];
		IR_LL_raw = IR_data[2];
//		IR_RR_raw2 = (float) IR_data[1] * (1 - alpha) + (float) alpha * IR_RR_raw;
//		IR_LL_raw2 = (float) IR_data[2] * (1 - alpha) + (float) alpha * IR_LL_raw;
		elapseMicros(550,curt);
		L_EM_OFF;
		R_EM_OFF;
		elapseMicros(700, curt);

		if(WALL_L > 0 && WALL_R > 0) {
			IR_LL = (100 * logf(WALL_L)) / logf(IR_LL_raw);
			IR_RR = (100 * logf(WALL_R)) / logf(IR_RR_raw);
		}
		if(WALL_FR > 0 && WALL_FL > 0) {
			IR_FL = (100 * logf(WALL_FL)) / logf(IR_FL_raw);
			IR_FR = (100 * logf(WALL_FR)) / logf(IR_FR_raw);
		}

	}
	else {
		completeADC = 0;
		HAL_ADC_Start_DMA(IR_sensor.ADCx,(uint32_t*) IR_data, 4);
	}
//	delay_us(300);
}


void ir_calibration() {
	for (int i = 0; i < 4000; i++) {
		read_ir_adc();
		WALL_L += IR_LL_raw;
		WALL_R += IR_RR_raw;
	}
	WALL_L /= 4000;
	WALL_R /= 4000;
	IR_sensor.prev_ir_state_left = WALL_L;
	IR_sensor.prev_ir_state_right = WALL_R;
}

void wall_front_calibration() {
	for (int i = 0; i < 4000; i++) {
		read_ir_adc();
		WALL_FL += IR_FL_raw;
		WALL_FR += IR_FR_raw;
	}
	WALL_FL /= 4000;
	WALL_FR /= 4000;

}

float wall_side_calibration() {
	read_ir_adc();
	float error = 0;
	float left_error = 100 - IR_LL;
	float right_error = 100 - IR_RR;
	/*
	if (IR_LL > WALL_L && IR_RR < WALL_R) {
		error = WALL_L - IR_LL;
	}
	else if(IR_LL < WALL_L && IR_RR > WALL_R) {
		error = IR_RR - WALL_R;
	}
	else
		error = 0;
	return error;
	*/
	if(check_wall_left() && check_wall_right()) {
//		printf("%s     ", "both wall");
		error = left_error + right_error;
	}
	else if(check_wall_left()) {
//		printf("%s     ", "left wall");
		error = left_error * 2.0;
	}
	else if(check_wall_right()) {
//		printf("%s     ", "right wall");
		error = - right_error * 2.0;
	}
	return error - IR_sensor.offset;
}

int8_t check_wall_left() {
	if(IR_LL > 110 || IR_LL < 0)
		return 0;
	else
		return 1;
}

int8_t check_wall_right() {
	if(IR_RR > 118 || IR_RR < 0)
		return 0;
	else
		return 1;
}

int8_t check_wall_frontL() {
	if(IR_FL > 110)
		return 0;
	else
		return 1;
}

int8_t check_wall_frontR() {
	if(IR_FR_raw < WALL_FR - 300)
		return 0;
	else
		return 1;
}

void wall_calib_offset() {
	float rev = 0;
	for(uint16_t i = 0; i < 4000; i++) {
		rev += wall_side_calibration();
	}
	IR_sensor.offset = rev / 4000;
}
