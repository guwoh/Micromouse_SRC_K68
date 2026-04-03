/*
 * controller.c
 *
 *  Created on: Mar 7, 2026
 *      Author: kinhk
 */

#include "main.h"

uint8_t controlFlag = 0;

uint16_t length = 76; //mm

float encoderAccumulate = 0;
int encoderFeedbackX = 0;
int encoderFeedbackW = 0;
int32_t targetSpeedX = 0;
float targetSpeedW = 0;
float curSpeedX = 0;
float curSpeedW = 0;
float accX; // mm\s\s
float decX = 860;
int accX_Turn = 800;
int decX_Turn = 800;
float accW = 180; //rad/s/s
float decW = 180;
float kpX = 2;
float kdX = 0.4;
float kpW = 4;
float kdW = 14;

float posErrorX = 0;
float posErrorW = 0;
float oldPosErrorX = 0;
float oldPosErrorW = 0;
int32_t posPwmX = 0;
int32_t posPwmW = 0;

float moveSpeedX = 400;
float stopSpeedX = 0;
int moveSpeedX_Turn = 200;
int stopSpeedX_Turn = 40;

uint8_t moveForwardFlag = 0;
uint8_t turnLeftFlag = 0;
uint8_t turnRightFlag = 0;

uint8_t sensorFlag = 0;
uint8_t gyroFlag = 0;

uint8_t sensorScale = 80;
uint8_t gyroScale;
float oldSensorFeedback = 0;
float kdSensor = 1;
float kpSensor = 0.08;

float integralTurnL = 0;
float integralTurnR = 0;

int16_t pwmBuff = 0;
int8_t accFlag = 0;


float angLeftW = 0;
float angRightW = 0;
float oldAngLeftW = 0;
float oldAngRightW = 0;

float debug1[4000];
float debug2[4000];
int tick = 0;

float angularVelocity = 7;
/*-------------------------------------------------------------*/

void initController() {
	set_mleft(0);
	set_mright(0);
	moveSpeedX = mm_to_counts(moveSpeedX);
	stopSpeedX = mm_to_counts(stopSpeedX);
	accX = returnAcc(moveSpeedX);
//	decX = returnAcc(moveSpeedX);
//	accX = mm_to_counts(2000);
	decX = mm_to_counts(decX);
	accX_Turn = mm_to_counts(accX_Turn);
	decX_Turn = mm_to_counts(decX_Turn);
	moveSpeedX_Turn = mm_to_counts(moveSpeedX_Turn);
	stopSpeedX_Turn = mm_to_counts(stopSpeedX_Turn);
}

void resetEverything() {
	angLeftW = 0;
	angRightW = 0;
	oldAngLeftW = 0;
	oldAngRightW = 0;
	integralTurnL = 0;
	integralTurnR = 0;
	encoderAccumulate = 0;
	encoderFeedbackX = 0;
	encoderFeedbackW = 0;
	targetSpeedX = 0;
	targetSpeedW = 0;
//	curSpeedX = 0;
//	curSpeedW = 0;
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
		TIM2->CCR2 = 0;
		TIM2->CCR1 = pwm;
	} else {
		TIM2->CCR2 = -pwm;
		TIM2->CCR1 = 0;
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

int returnAcc(int speed) {
	return (float) speed / 0.142;
}

int32_t mm_to_counts(float speed) {
	return (speed * 16384) / (29 * 3.1416);
}

float counts_to_mm(float count) {
	return (count * 29 * 3.1416) / 16384;
}

void update_speed(void) {
	if (moveForwardFlag) {
		if ((curSpeedX * 1000) < targetSpeedX) {
			curSpeedX += (float) accX / 1000000.0f;
			if ((curSpeedX * 1000) > targetSpeedX)
				curSpeedX = targetSpeedX / 1000;
		} else if ((curSpeedX * 1000) > targetSpeedX) {
			curSpeedX -= (float) accX / 1000000.0f;
			if ((curSpeedX * 1000) < targetSpeedX)
				curSpeedX = targetSpeedX / 1000;
		}
	} else if (turnLeftFlag || turnRightFlag) {
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

void update_angle_speed() {
	if (curSpeedW < targetSpeedW) {
		curSpeedW += accW / 1000.0f;
		if (curSpeedW > targetSpeedW)
			curSpeedW = targetSpeedW;
	}
	else if(curSpeedW > targetSpeedW) {
		curSpeedW -= decW / 1000.0f;
		if (curSpeedW < targetSpeedW)
			curSpeedW = targetSpeedW;
	}
}

void calculate_motor_pwm(void) {
	int gyroFeedback;
	int rotationalFeedback;
	int sensorFeedback;
	int feedForward;

	encoderFeedbackX = (encoderChangeLeft + encoderChangeRight);
	encoderFeedbackW = encoderChangeRight - encoderChangeLeft;

	encoderAccumulate += (float) encoderFeedbackW * 0.1;
	int rawSensorError = sensorError();
//	sensorFeedback = 0;
	sensorFeedback = kpSensor * rawSensorError;
	+kdSensor * (rawSensorError - oldSensorFeedback);
	debug1[0] = rawSensorError;
	oldSensorFeedback = rawSensorError;
	if (sensorFlag)
		rotationalFeedback = encoderFeedbackW - sensorFeedback;
	else if (gyroFlag)
		rotationalFeedback = encoderFeedbackW + gyroFeedback;
	else if (sensorFlag && gyroFlag)
		rotationalFeedback = encoderFeedbackW - sensorFeedback + gyroFeedback;
	else
		rotationalFeedback = encoderFeedbackW;

	posErrorX = (curSpeedX * 2 - encoderFeedbackX) / 2;

	posErrorW = curSpeedW - rotationalFeedback;

	posPwmX = kpX * posErrorX + kdX * (posErrorX - oldPosErrorX);
	posPwmW = kpW * posErrorW + kdW * (posErrorW - oldPosErrorW);

	oldPosErrorX = posErrorX;
	oldPosErrorW = posErrorW;
	if (accFlag)
		feedForward = 4.9 * curSpeedX + 16.5 * accX / 1000000 - 28;
	else
		feedForward = 4.9 * curSpeedX - 16.5 * accX / 1000000 + 28;

	int leftBaseSpeed = feedForward + posPwmX - posPwmW + pwmBuff;
	int rightBaseSpeed = feedForward + posPwmX + posPwmW + pwmBuff;

	if (leftBaseSpeed > 700)
		leftBaseSpeed = 700;
	else if (leftBaseSpeed < -700)
		leftBaseSpeed = -700;
	if (rightBaseSpeed > 700)
		rightBaseSpeed = 700;
	else if (rightBaseSpeed < -700)
		rightBaseSpeed = -700;

	if (abs(encoderAccumulate) > 1) {
		leftBaseSpeed += 30 * encoderAccumulate;
		encoderAccumulate = 0;
	}
	set_mleft(leftBaseSpeed);
	set_mright(rightBaseSpeed);

}

void calculate_motor_pwm_turn_right() {
	int feedForwardLeft;
	int feedForwardRight;

	float errorLeft;
	float errorRight;

	float omega = mm_to_counts(curSpeedW * length / 2000.0f);

	feedForwardLeft = 5.4 * (moveSpeedX / 1000.0f + omega) + 16 * accX / 1000000 - 28;
	feedForwardRight = 5.4 * (moveSpeedX / 1000.0f - omega) - 16 * accX / 1000000 - 28;

	errorLeft = (moveSpeedX / 1000.0f + omega - encoderChangeLeft);
	errorRight = (moveSpeedX /1000.0f - omega - encoderChangeRight);
	angLeftW = errorLeft * 20 + (errorLeft - oldAngLeftW) * 10;
	angRightW = errorRight * 20 + (errorRight - oldAngRightW) * 10;

	oldAngLeftW = errorLeft;
	oldAngRightW = errorRight;

	if (feedForwardLeft > 800)
		feedForwardLeft = 800;
	if (feedForwardRight < 0)
		feedForwardRight = 0;

	set_mleft(feedForwardLeft + angLeftW);
	set_mright(feedForwardRight + angRightW);
}

void calculate_motor_pwm_turn_left() {
	int feedForwardLeft;
	int feedForwardRight;

	float errorLeft;
	float errorRight;

	float omega = mm_to_counts(curSpeedW * length / 2000.0f);

	feedForwardLeft = 5.4 * (moveSpeedX / 1000.0f - omega) + 16 * accX / 1000000 - 28;
	feedForwardRight = 5.4 * (moveSpeedX / 1000.0f + omega) - 16 * accX / 1000000 - 28;

	errorLeft = (moveSpeedX / 1000.0f - omega - encoderChangeLeft);
	errorRight = (moveSpeedX /1000.0f + omega - encoderChangeRight);
	angLeftW = errorLeft * 20 + (errorLeft - oldAngLeftW) * 10;
	angRightW = errorRight * 20 + (errorRight - oldAngRightW) * 10;

	oldAngLeftW = errorLeft;
	oldAngRightW = errorRight;

	if (feedForwardRight > 800)
		feedForwardRight = 800;
	if (feedForwardLeft < 0)
		feedForwardLeft = 0;

	set_mleft(feedForwardLeft + angLeftW);
	set_mright(feedForwardRight + angRightW);
}

float need_to_decelerate(int32_t dist, float curSpd, float endSpd) {
	if (curSpd < 0)
		curSpd = -curSpd;
	if (endSpd < 0)
		endSpd = -endSpd;
	if (dist < 0)
		dist = 1;
	if (dist == 0)
		dist = 1;
	return (fabs(
			(float) ((curSpd * curSpd - endSpd * endSpd) * 2 * 100)
					/ (float) (dist * 2.0f)));
}

void speed_profile() {
	update_speed();
	calculate_motor_pwm();
}

int16_t pwm_buff() {
	if(distanceLeft < 200 && distanceLeft > 0) {
		pwmBuff += oneCellDistance * 2 - encoderCount + oldEncoderCount;
		return pwmBuff * 0.000001;
	}
	return 0;
}

void move_one_cell(ADC_HandleTypeDef *hadc1, I2C_HandleTypeDef *hi2c2,
		I2C_HandleTypeDef *hi2c3) {
	moveForwardFlag = 1;
	sensorFlag = 1;
	targetSpeedW = 0;
	targetSpeedX = moveSpeedX;
	pwmBuff = 0;
	do {

		if ((need_to_decelerate(distanceLeft, (moveSpeedX / 100),
				(stopSpeedX / 100)) * 100) < (decX)) {
			targetSpeedX = moveSpeedX;
			accFlag = 1;
		} else {
			accFlag = 0;
			R_LED_ON
			;
			targetSpeedX = stopSpeedX;
		}
		if (controlFlag) {
			controlFlag = 0;
			get_encoder_status();
//			printf("%ld  %f   %d \n", distanceLeft, curSpeedX, encoderChangeLeft);
//			read_sensor(hadc1);
//			pwm_buff();
			speed_profile();
			if (encoderCount > (oneCellDistance + 3000)
					&& encoderCount < (oneCellDistance + 6000)) {
				wall_detect2();
//				wall_L_sum += wall_L_Check2;
//				wall_R_sum += wall_R_Check2;
			}
		}
	} while ((encoderCount - oldEncoderCount) < (oneCellDistance * 2));
//			&& (LFSensor < LFThreshold1) && (RFSensor < RFThreshold1));
	oldEncoderCount = encoderCount;
	R_LED_OFF
	;
//	wall_L_sum /= (Millis - curt);
//	wall_R_sum /= (Millis - curt);
//	if (wall_L_sum > 0.6)
//		wall_L_Check2 = 1;
//	else
//		wall_L_Check2 = 0;
//	if (wall_R_sum > 0.6)
//		wall_R_Check2 = 1;
//	else
//		wall_R_Check2 = 0;

}

void turn_right(I2C_HandleTypeDef *hi2c2, I2C_HandleTypeDef *hi2c3) {
	turnRightFlag = 1;
	targetSpeedW = 0;
//	resetEverything();
	tick = 0;
	do {
		if (controlFlag) {
			if (tick < 290) {
				targetSpeedW = angularVelocity;
			}
			else if (tick >= 290) {
				targetSpeedW = 0;
			}
			controlFlag = 0;
			get_encoder_status();
			update_angle_speed();
			calculate_motor_pwm_turn_right();
			read_enc(hi2c2, hi2c3);
			tick++;
		}

	} while (tick < 300);
	oldEncoderCount = encoderCount;
//	resetEverything();

}

void turn_left(I2C_HandleTypeDef *hi2c2, I2C_HandleTypeDef *hi2c3) {
	turnLeftFlag = 1;
	targetSpeedW = 0;
//	resetEverything();
	tick = 0;
	do {
		if (controlFlag) {
			if (tick < 290) {
				targetSpeedW = angularVelocity;
			}
			else if (tick >= 290) {
				targetSpeedW = 0;
			}
			controlFlag = 0;
			get_encoder_status();
			update_angle_speed();
			calculate_motor_pwm_turn_left();
			read_enc(hi2c2, hi2c3);
			tick++;
		}

	} while (tick < 310);
	oldEncoderCount = encoderCount;
}

void wall_front_adjust(ADC_HandleTypeDef *hadc1) {
	uint32_t curt = Millis;
	uint32_t time = 500;
	while (Millis - curt < time) {
		if (controlFlag) {
			controlFlag = 0;
//			read_sensor(hadc1);
			float pwmLeft = (LFThreshold1 - LFSensor) * 0.1;
			float pwmRight = (RFThreshold1 - RFSensor) * 0.15;
			if (pwmLeft > 350)
				pwmLeft = 0;
			else if (pwmLeft < -300)
				pwmLeft = 0;
			if (pwmRight > 350)
				pwmRight = 0;
			else if (pwmRight < -300)
				pwmRight = 0;
			set_mleft(pwmLeft);
			set_mright(pwmRight);
		}
	}
	set_mleft(0);
	set_mright(0);
}

void turn_90left(I2C_HandleTypeDef *hi2c2, I2C_HandleTypeDef *hi2c3) {
	resetEverything();
	turnLeftFlag = 1;
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
			integralTurnL += errorL;
			integralTurnR += errorR;
			leftPwm = 0.058 * errorL + 10 * (errorL - oldErrorL)
					+ 0.0002 * integralTurnL;
			rightPwm = 0.058 * errorR + 10 * (errorR - oldErrorR)
					+ 0.00018 * integralTurnR;
			leftPwm += 275;
			rightPwm += 275;
			if (leftPwm > 1000)
				leftPwm = 1000;
			if (rightPwm > 1000)
				rightPwm = 1000;
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
	turnRightFlag = 1;
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
			integralTurnL += errorL;
			integralTurnR += errorR;
			leftPwm = 0.058 * errorL + 10 * (errorL - oldErrorL)
					+ 0.00018 * integralTurnL;
			rightPwm = 0.058 * errorR + 10 * (errorR - oldErrorR)
					+ 0.0002 * integralTurnR;
			leftPwm += 275;
			rightPwm += 265;
			if (leftPwm > 1000)
				leftPwm = 1000;
			if (rightPwm > 1000)
				rightPwm = 1000;
			set_mleft(leftPwm);
			set_mright(-rightPwm);
			read_enc(hi2c2, hi2c3);
			oldErrorL = errorL;
			oldErrorR = errorR;

		}
	} while (enc_cnt_left < (turnDistance) || -enc_cnt_right < (turnDistance));
	set_mleft(0);
	set_mright(0);
}
void turn_180(I2C_HandleTypeDef *hi2c2, I2C_HandleTypeDef *hi2c3) {
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
			leftPwm += 270;
			rightPwm += 270;
			if (leftPwm > 1000)
				leftPwm = 1000;
			if (rightPwm > 1000)
				rightPwm = 1000;
			set_mleft(-leftPwm);
			set_mright(rightPwm);
			read_enc(hi2c2, hi2c3);
			oldErrorL = errorL;
			oldErrorR = errorR;

		}
	} while (-enc_cnt_left < turnDistance * 2
			|| enc_cnt_right < turnDistance * 2);
	set_mleft(0);
	set_mright(0);
}
