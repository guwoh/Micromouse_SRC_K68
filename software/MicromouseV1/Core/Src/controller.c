/*
 * controller.c
 *
 *  Created on: Mar 7, 2026
 *      Author: kinhk
 */

#include "main.h"

uint8_t controlFlag = 0;


float encoderAccumulate = 0;
int encoderFeedbackX = 0;
int encoderFeedbackW = 0;
int32_t targetSpeedX = 0;
float targetSpeedW = 0;
float curSpeedX = 0;
float curSpeedW = 0;
int accX = 1600; // mm\s\s
int decX = 1600;
int accX_Turn = 800;
int decX_Turn = 800;
int accW = 10; //mm\s\s
int decW = 10;
float kpX = 0.018;
float kdX = -0.04;
float kpW = 4;
float kdW = 14;

float posErrorX = 0;
float posErrorW = 0;
float oldPosErrorX = 0;
float oldPosErrorW = 0;
int32_t posPwmX = 0;
int32_t posPwmW = 0;

int moveSpeedX = 600;
int stopSpeedX = 80;
int moveSpeedX_Turn = 200;
int stopSpeedX_Turn = 40;

uint8_t moveForwardFlag = 0;
uint8_t turnLeftFlag = 0;
uint8_t turnRightFlag = 0;

uint8_t sensorFlag = 0;
uint8_t gyroFlag = 0;

uint8_t sensorScale = 50;
uint8_t gyroScale;

float debug1;
/*-------------------------------------------------------------*/

void initController() {
	set_mleft(0);
	set_mright(0);
	accX = mm_to_counts(accX);
	decX = mm_to_counts(decX);
	accX_Turn = mm_to_counts(accX_Turn);
	decX_Turn = mm_to_counts(decX_Turn);
	moveSpeedX = mm_to_counts(moveSpeedX);
	stopSpeedX = mm_to_counts(stopSpeedX);
	moveSpeedX_Turn = mm_to_counts(moveSpeedX_Turn);
	stopSpeedX_Turn = mm_to_counts(stopSpeedX_Turn);
}

void resetEverything() {
	encoderAccumulate = 0;
	encoderFeedbackX = 0;
	encoderFeedbackW = 0;
	targetSpeedX = 0;
	targetSpeedW = 0;
	curSpeedX = 0;
	curSpeedW = 0;
	posErrorX = 0;
	posErrorW = 0;
	oldPosErrorX = 0;
	oldPosErrorW = 0;
	posPwmX = 0;
	posPwmW = 0;
	distanceLeft = 0;
	encoderChange = 0;
	encoderCount = 0;
	oldEncoderCount = 0;
	i2c_busy_left = 0;
	i2c_error_left = 0;
	i2c_busy_right = 0;
	i2c_error_right = 0;
	encoderChangeLeft = 0;
	encoderChangeRight = 0;
	enc_cnt_left = 0;
	enc_cnt_right = 0;
	setup_flag = 0;
	gyroFlag = 0;
	sensorFlag = 0;
	moveForwardFlag = 0;
	turnLeftFlag = 0;
	turnRightFlag = 0;
	Millis = 0;
	Micros = 0;
}

void set_mleft(int16_t pwm) {
	if (pwm > 0) {
		TIM2->CCR2 = pwm;
		TIM2->CCR1 = 0;
	} else {
		TIM2->CCR2 = 0;
		TIM2->CCR1 = -pwm;
	}
}

void set_mright(int16_t pwm) {
	if (pwm > 0) {
		TIM2->CCR4 = pwm;
		TIM2->CCR3 = 0;
	} else {
		TIM2->CCR4 = 0;
		TIM2->CCR3 = -pwm;
	}
}

int32_t mm_to_counts(float speed) {
	return (speed * 16384) / (32 * 3.1416);
}

float counts_to_mm(float count) {
	return (count * 32 * 3.1416) / 16384;
}

void update_speed(void) {
	if (moveForwardFlag) {
		if ((curSpeedX * 1000) < targetSpeedX) {
			curSpeedX += (float) accX / 1000;
			if ((curSpeedX * 1000) > targetSpeedX)
				curSpeedX = targetSpeedX / 1000;
		} else if ((curSpeedX * 1000) > targetSpeedX) {
			curSpeedX -= (float) decX / 1000;
			if ((curSpeedX * 1000) < targetSpeedX)
				curSpeedX = targetSpeedX / 1000;
		}
	}
	else if (turnLeftFlag || turnRightFlag) {
		if ((curSpeedX * 1000) < targetSpeedX) {
			curSpeedX += (float) accX_Turn / 1000;
			if ((curSpeedX * 1000) > targetSpeedX)
				curSpeedX = targetSpeedX / 1000;
		} else if ((curSpeedX * 1000) > targetSpeedX) {
			curSpeedX -= (float) decX_Turn / 1000;
			if ((curSpeedX * 1000) < targetSpeedX)
				curSpeedX = targetSpeedX / 1000;
		}
	}
	if (curSpeedW < targetSpeedW) {
		curSpeedW += accW / 1000;
		if (curSpeedW > targetSpeedW) {
			curSpeedW = targetSpeedW;
		}
	} else if (curSpeedW > targetSpeedW) {
		curSpeedW -= accW / 1000;
		if (curSpeedW < targetSpeedW)
			curSpeedW = targetSpeedW;
	}

}

void calculate_motor_pwm(void) {
	int gyroFeedback;
	int rotationalFeedback;
	int sensorFeedback;

	if (moveForwardFlag){
		encoderFeedbackX = (encoderChangeLeft + encoderChangeRight);
		encoderFeedbackW = encoderChangeRight - encoderChangeLeft;
	}
	else if (turnLeftFlag) {
		encoderFeedbackX = (-encoderChangeLeft + encoderChangeRight) / 2;
		encoderFeedbackW = encoderChangeRight + encoderChangeLeft;
	}
	else if (turnRightFlag) {
		encoderFeedbackX = (encoderChangeLeft - encoderChangeRight) / 2;
		encoderFeedbackW = encoderChangeRight + encoderChangeLeft;
	}
	encoderAccumulate += (float) encoderFeedbackW  * 0.1;
	sensorFeedback = sensorError() / sensorScale;
	debug1 = sensorFeedback;
	if (sensorFlag)
		rotationalFeedback = encoderFeedbackW - sensorFeedback;
	else if (gyroFlag)
		rotationalFeedback = encoderFeedbackW + gyroFeedback;
	else if (sensorFlag && gyroFlag)
		rotationalFeedback = encoderFeedbackW - sensorFeedback + gyroFeedback;
	else
		rotationalFeedback = encoderFeedbackW;

	posErrorX += (curSpeedX * 2 - encoderFeedbackX) / 2;

	posErrorW = curSpeedW - rotationalFeedback;

	posPwmX = kpX * posErrorX + kdX * (posErrorX - oldPosErrorX);
	posPwmW = kpW * posErrorW + kdW * (posErrorW - oldPosErrorW);

	oldPosErrorX = posErrorX;
	oldPosErrorW = posErrorW;

	int leftBaseSpeed = posPwmX - posPwmW;
	int rightBaseSpeed = posPwmX + posPwmW;
	if (leftBaseSpeed > 600)
		leftBaseSpeed = 600;
	else if (leftBaseSpeed < -600)
		leftBaseSpeed = -600;
	if (rightBaseSpeed > 600)
		rightBaseSpeed = 600;
	else if (rightBaseSpeed < -600)
		rightBaseSpeed = -600;

	if (moveForwardFlag) {
		if (abs(encoderAccumulate) > 1) {
			rightBaseSpeed += 30;
			encoderAccumulate = 0;
		}
		set_mleft(leftBaseSpeed);
		set_mright(rightBaseSpeed);
	}
	else if (turnLeftFlag) {
		rightBaseSpeed += 275;
		leftBaseSpeed += 275;
		if (abs(encoderAccumulate) > 1) {
			rightBaseSpeed += 30 * encoderAccumulate;
			encoderAccumulate = 0;
		}
		set_mleft(-leftBaseSpeed);
		set_mright(rightBaseSpeed);
	} else if (turnRightFlag) {
		rightBaseSpeed += 265;
		leftBaseSpeed += 275;
		if (abs(encoderAccumulate) > 1) {
			rightBaseSpeed += 10 * encoderAccumulate;
			encoderAccumulate = 0;
		}
		set_mleft(leftBaseSpeed);
		set_mright(-rightBaseSpeed);
	}
}

float need_to_decelerate(int32_t dist, int16_t curSpd, int16_t endSpd) {
	if (curSpd < 0)
		curSpd = -curSpd;
	if (endSpd < 0)
		endSpd = -endSpd;
	if (dist < 0)
		dist = 1;
	if (dist == 0)
		dist = 1;

	return (fabs((float) ((curSpd * curSpd - endSpd * endSpd) * 2 * 100) / (dist * 2.0f)));
}

void speed_profile() {
	update_speed();
	calculate_motor_pwm();
}

void move_one_cell(ADC_HandleTypeDef *hadc1, I2C_HandleTypeDef *hi2c2, I2C_HandleTypeDef *hi2c3) {
	moveForwardFlag = 1;
	sensorFlag = 1;
	targetSpeedW = 0;
	targetSpeedX = moveSpeedX;
	do {

		if ((need_to_decelerate(distanceLeft * 2, (moveSpeedX / 100), (stopSpeedX / 100)) * 100) < decX) {
			targetSpeedX = moveSpeedX;

		} else {
			targetSpeedX = stopSpeedX;
		}
		if (controlFlag) {
			controlFlag = 0;
			get_encoder_status();
			read_sensor(hadc1);
			speed_profile();
			read_enc(hi2c2, hi2c3);
		}
	} while ((encoderCount - oldEncoderCount) < (oneCellDistance * 2) && (LFSensor < LFThreshold1)
			&& (RFSensor < RFThreshold1));
	oldEncoderCount = encoderCount;

}

void turn_right(I2C_HandleTypeDef *hi2c2, I2C_HandleTypeDef *hi2c3) {
	turnRightFlag = 1;
	targetSpeedW = 0;
	targetSpeedX = moveSpeedX_Turn;
	do {
		debug1 = (need_to_decelerate(distanceLeft * 2, (moveSpeedX_Turn / 100),
				(stopSpeedX_Turn / 100)) * 100);
		if ((need_to_decelerate(distanceLeft, (moveSpeedX_Turn / 100),
				(stopSpeedX_Turn / 100)) * 100) < decX_Turn) {
			targetSpeedX = moveSpeedX_Turn;

		}
		else {
			targetSpeedX = stopSpeedX_Turn;
		}
		if (controlFlag) {
			controlFlag = 0;
			get_encoder_status();
			speed_profile();
			read_enc(hi2c2, hi2c3);

		}

	} while ((enc_cnt_left - enc_cnt_right - oldEncoderCount * 2) < ((turnDistance - 1000) * 2));
	oldEncoderCount = enc_cnt_left - enc_cnt_right;
}

void turn_left(I2C_HandleTypeDef *hi2c2, I2C_HandleTypeDef *hi2c3) {
	turnLeftFlag = 1;
	targetSpeedW = 0;
	targetSpeedX = moveSpeedX_Turn;
	do {
		debug1 = (need_to_decelerate(distanceLeft * 2, (moveSpeedX_Turn / 100),
				(stopSpeedX_Turn / 100)) * 100);
		if ((need_to_decelerate(distanceLeft, (moveSpeedX_Turn / 100),
				(stopSpeedX_Turn / 100)) * 100) < decX_Turn) {
			targetSpeedX = moveSpeedX_Turn;

		}
		else {
			targetSpeedX = stopSpeedX_Turn;
		}
		if (controlFlag) {
			controlFlag = 0;
			get_encoder_status();
			speed_profile();
			read_enc(hi2c2, hi2c3);

		}

	} while ((-enc_cnt_left + enc_cnt_right - oldEncoderCount * 2) < (turnDistance * 2));
	oldEncoderCount = -enc_cnt_left + enc_cnt_right;
}

void wall_front_adjust(ADC_HandleTypeDef *hadc1) {
	uint32_t curt = Millis;
	uint32_t time = 500;
	if (LFThreshold1 - LFSensor > 200) {
		time = time * 3;
	}
	while (Millis - curt < time) {
		if (controlFlag) {
			controlFlag = 0;
			read_sensor(hadc1);
			int16_t pwmLeft = (LFThreshold1 - LFSensor);
			int16_t pwmRight = (RFThreshold1 - RFSensor);
			if (pwmLeft > 450) pwmLeft = 450;
			else if(pwmLeft < -450) pwmLeft = -450;
			if (pwmRight > 450) pwmRight = 450;
			else if(pwmRight < -450) pwmRight = -450;
			set_mleft(pwmLeft);
			set_mright(pwmRight);
		}
	}
	set_mleft(0);
	set_mright(0);
}

void turn_90left(I2C_HandleTypeDef *hi2c2, I2C_HandleTypeDef *hi2c3) {
	resetEverything();
	int16_t errorL;
	int16_t oldErrorL = 0;
	int16_t errorR;
	int16_t oldErrorR = 0;
	int16_t leftPwm;
	int16_t rightPwm;
	do {
		if (controlFlag) {
			controlFlag = 0;
			errorL = turnDistance + enc_cnt_left;
			errorR = turnDistance - enc_cnt_right;
			leftPwm = 0.054 * errorL + 10 * (errorL - oldErrorL);
			rightPwm = 0.054 * errorR + 10 * (errorR - oldErrorR);
			leftPwm += 225;
			rightPwm += 225;
			if (leftPwm > 1000) leftPwm = 1000;
			if (rightPwm > 1000) rightPwm = 1000;
			set_mleft(-leftPwm);
			set_mright(rightPwm);
			read_enc(hi2c2, hi2c3);
			oldErrorL = errorL;
			oldErrorR = errorR;

		}
	} while (-enc_cnt_left < turnDistance || enc_cnt_right < turnDistance);
	set_mleft(0);
	set_mright(0);
}

void turn_90right(I2C_HandleTypeDef *hi2c2, I2C_HandleTypeDef *hi2c3) {
	resetEverything();
	int16_t errorL;
	int16_t oldErrorL = 0;
	int16_t errorR;
	int16_t oldErrorR = 0;
	int16_t leftPwm;
	int16_t rightPwm;
	do {
		if (controlFlag) {
			controlFlag = 0;
			errorL = turnDistance - enc_cnt_left;
			errorR = turnDistance + enc_cnt_right;
			leftPwm = 0.054 * errorL + 10 * (errorL - oldErrorL);
			rightPwm = 0.05 * errorR + 10 * (errorR - oldErrorR);
			leftPwm += 220;
			rightPwm += 210;
			if (leftPwm > 1000) leftPwm = 1000;
			if (rightPwm > 1000) rightPwm = 1000;
			set_mleft(leftPwm);
			set_mright(-rightPwm);
			read_enc(hi2c2, hi2c3);
			oldErrorL = errorL;
			oldErrorR = errorR;

		}
	} while (enc_cnt_left < (turnDistance-400) || -enc_cnt_right < (turnDistance-400));
	set_mleft(0);
	set_mright(0);
}
