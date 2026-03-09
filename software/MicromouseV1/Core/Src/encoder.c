/*
 * encoder.c
 *
 *  Created on: Mar 7, 2026
 *      Author: kinhk
 */


#include "main.h"

int32_t oneCellDistance = 29335;
int32_t turnDistance = 10500;

volatile uint8_t i2c_busy_left = 0;
volatile uint8_t i2c_error_left = 0;
volatile uint8_t i2c_busy_right = 0;
volatile uint8_t i2c_error_right = 0;
volatile int16_t encoderChangeLeft = 0;
volatile int16_t encoderChangeRight = 0;

int32_t distanceLeft = 0;
int encoderChange;
int32_t encoderCount = 0;
int32_t oldEncoderCount = 0;
uint8_t encoder_raw_left[2];
uint16_t encoder_left;
uint8_t encoder_raw_right[2];
uint16_t encoder_right;
uint16_t old_enc_cnt_left;
uint16_t old_enc_cnt_right;
int32_t enc_cnt_left = 0;
int32_t enc_cnt_right = 0;

uint8_t setup_flag = 0;


void init_encoder(I2C_HandleTypeDef *hi2c2, I2C_HandleTypeDef *hi2c3) {
	uint32_t curt = Millis;
	while (Millis - curt < 1000) {
		if (controlFlag){
			read_enc(hi2c2, hi2c3);
			controlFlag = 0;
		}
	}
	old_enc_cnt_left = encoder_left;
	old_enc_cnt_right = encoder_right;
	enc_cnt_left = 0;
	enc_cnt_right = 0;
}
void get_encoder_status() {
	encoderCount += (encoderChangeRight + encoderChangeLeft);
	encoderChange = (encoderChangeRight + encoderChangeLeft);
	distanceLeft -= encoderChange;
}

void read_enc(I2C_HandleTypeDef *hi2c2, I2C_HandleTypeDef *hi2c3) {
	if (i2c_busy_left == 0) {
		i2c_busy_left = 1;
		HAL_I2C_Mem_Read_IT(hi2c3, MT6701_ADD << 1, FIRST_REG, 1,
				encoder_raw_left, 2);
	}

	if (i2c_busy_right == 0) {
		i2c_busy_right = 1;
		HAL_I2C_Mem_Read_IT(hi2c2, MT6701_ADD << 1, FIRST_REG, 1,
				encoder_raw_right, 2);
	}
}
