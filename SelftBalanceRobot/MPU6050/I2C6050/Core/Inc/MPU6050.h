/*
 * MPU6050.h
 *
 *  Created on: May 19, 2022
 *      Author: Mr Ruler
 */

#ifndef INC_MPU6050_H_
#define INC_MPU6050_H_

#define MPU6050_ADDR 0xD0


#define SMPLRT_DIV_REG 0x19
#define GYRO_CONFIG_REG 0x1B
#define ACCEL_CONFIG_REG 0x1C
#define ACCEL_XOUT_H_REG 0x3B
#define TEMP_OUT_H_REG 0x41
#define GYRO_XOUT_H_REG 0x43
#define PWR_MGMT_1_REG 0x6B
#define WHO_AM_I_REG 0x75

#define ACCEL_CONFIG_REG_2 0x1D
#define CONFIG_REG 0x1A

typedef struct {
	int16_t Accel_X_RAW;
	int16_t Accel_Y_RAW;
	int16_t Accel_Z_RAW;

	float Ax;
	float Ay;
	float Az;

	int16_t Gyro_X_RAW;
	int16_t Gyro_Y_RAW;
	int16_t Gyro_Z_RAW;

	float Gx;
	float Gy;
	float Gz;

	float Temp;
} MPU6050_Raw;
typedef struct {
	float TempdegC;

	float Pitch_Accel;
	float Row_Accel;

	float Pitch_Gyro;
	float Row_Gyro;
	float Yaw_Gyro;

	float Pitch;
	float Row;
	float Yaw;
}MPU6050_Data;

void MPU6050_Init (void);
void MPU6050_Read_Data(MPU6050_Raw *Raw);
void MPU6050_Read_Angle(MPU6050_Data *Angle);


#endif /* INC_MPU6050_H_ */
