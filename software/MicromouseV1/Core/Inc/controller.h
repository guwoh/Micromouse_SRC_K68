/*
 * controller.h
 *
 *  Created on: Mar 7, 2026
 *      Author: kinhk
 */

#ifndef INC_CONTROLLER_H_
#define INC_CONTROLLER_H_

extern uint8_t controlFlag;

extern float encoderAccumulate;
extern int encoderFeedbackX;
extern int encoderFeedbackW;
extern int32_t targetSpeedX;
extern float targetSpeedW;
extern float curSpeedX;
extern float curSpeedW;
extern int accX; // mm\s\s
extern int decX;
extern int accW; //mm\s\s
extern int decW;
extern int accX_Turn;
extern int decX_Turn;
extern float kpX;
extern float kdX;
extern float kpW;
extern float kdW;

extern float posErrorX;
extern float posErrorW;
extern float oldPosErrorX;
extern float oldPosErrorW;
extern int32_t posPwmX;
extern int32_t posPwmW;

extern int moveSpeedX;
extern int stopSpeedX;
extern int moveSpeedX_Turn;
extern int stopSpeedX_Turn;

extern uint8_t moveForwardFlag;
extern uint8_t turnLeftFlag;
extern uint8_t turnRightFlag;

extern uint8_t sensorFlag;
extern uint8_t gyroFlag;

extern uint8_t sensorScale;
extern uint8_t gyroScale;
extern float oldSensorFeedback;
extern float kpSensor;
extern float kdSensor;

extern float debug1;
/*----------------------------------------------------------------------*/

void initController(void);
void resetEverything();
void set_mleft(int16_t pwm);
void set_mright(int16_t pwm);
int32_t mm_to_counts(float speed);
float counts_to_mm(float count);
void update_speed(void);
void calculate_motor_pwm(void);
void move_one_cell();
void speed_profile();
float need_to_decelerate(int32_t dist, int16_t curSpd, int16_t endSpd);
void turn_right(I2C_HandleTypeDef *hi2c2, I2C_HandleTypeDef *hi2c3);
void turn_left(I2C_HandleTypeDef *hi2c2, I2C_HandleTypeDef *hi2c3);
void wall_front_adjust();
void turn_90left(I2C_HandleTypeDef *hi2c2, I2C_HandleTypeDef *hi2c3);
void turn_90right(I2C_HandleTypeDef *hi2c2, I2C_HandleTypeDef *hi2c3);
void turn_180(I2C_HandleTypeDef *hi2c2, I2C_HandleTypeDef *hi2c3);
#endif /* INC_CONTROLLER_H_ */
