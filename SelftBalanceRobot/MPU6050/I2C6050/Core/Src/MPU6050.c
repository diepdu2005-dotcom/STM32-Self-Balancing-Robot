/*
 * MPU6050.c
 *
 *  Created on: May 19, 2022
 *      Author: Mr Ruler
 */

#include "stm32f1xx_hal.h"
#include "MPU6050.h"
#include "math.h"

extern I2C_HandleTypeDef hi2c1;
extern TIM_HandleTypeDef htim1;

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

#define timeOut  100


uint16_t Accel_X_RAW = 0;
uint16_t Accel_Y_RAW = 0;
uint16_t Accel_Z_RAW = 0;

uint16_t Gyro_X_RAW = 0;
uint16_t Gyro_Y_RAW = 0;
uint16_t Gyro_Z_RAW = 0;

uint16_t timer = 0;
double dt;

int16_t samples = 0, gyro_x_offset, gyro_y_offset, gyro_z_offset;

void MPU6050_Init (void)
{
	uint8_t check;
	uint8_t Data;
	// check device ID WHO_AM_I
	HAL_I2C_Mem_Read (&hi2c1, MPU6050_ADDR,WHO_AM_I_REG,1, &check, 1, timeOut);
	if (check == 104)  // 0x68 will be returned by the sensor if everything goes well
	{
		// power management register 0X6B we should write all 0's to wake the sensor up
		Data = 0x01;//0x00
		HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, PWR_MGMT_1_REG, 1,&Data, 1, timeOut);

		// Set DATA RATE of 1KHz by writing SMPLRT_DIV register
		Data = 0x07;
		HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, SMPLRT_DIV_REG, 1, &Data, 1, timeOut);

		// Set accelerometer configuration in ACCEL_CONFIG Register
		// XA_ST=0,YA_ST=0,ZA_ST=0, FS_SEL=0 -> � 2g  ~  0000 0000  ~ 0x00 16,384
		// XA_ST=0,YA_ST=0,ZA_ST=0, FS_SEL=0 -> � 4g  ~  0000 1000  ~ 0x08 8192
		// XA_ST=0,YA_ST=0,ZA_ST=0, FS_SEL=0 -> � 8g  ~  0001 0000  ~ 0x10 4096
		// XA_ST=0,YA_ST=0,ZA_ST=0, FS_SEL=0 -> � 16g ~  0001 1000  ~ 0x18 2048
		Data = 0x08;
		HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, ACCEL_CONFIG_REG, 1, &Data, 1, timeOut);

		Data = 0x05;
		HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, ACCEL_CONFIG_REG_2, 1,&Data, 1, timeOut);

		// Set Gyroscopic configuration in GYRO_CONFIG Register
		// XG_ST=0,YG_ST=0,ZG_ST=0, FS_SEL=0 -> � 250 �/s      ~  0000 0000  ~ 0x00 131
		// XG_ST=0,YG_ST=0,ZG_ST=0, FS_SEL=1 -> � 500 �/s      ~  0000 1000  ~ 0x08 65.5
		// XG_ST=0,YG_ST=0,ZG_ST=0, FS_SEL=2 -> � 1000 �/s   ~  0001 0000  ~ 0x10 32.8
		// XG_ST=0,YG_ST=0,ZG_ST=0, FS_SEL=3 -> � 2000 �/s   ~  0001 1000  ~ 0x18 16.4
		Data = 0x08;
		HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, GYRO_CONFIG_REG, 1, &Data, 1, timeOut);
		Data = 0x05;
		HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, CONFIG_REG, 1,&Data, 1, timeOut);
	}

}
void MPU6050_Read_Data(MPU6050_Raw *Raw){

	uint8_t Rec_Data[14];
	// Read 6 BYTES of data starting from ACCEL_XOUT_H register
	HAL_I2C_Mem_Read (&hi2c1, MPU6050_ADDR, ACCEL_XOUT_H_REG, 1, Rec_Data, 14, timeOut);

	Raw->Accel_X_RAW = (int16_t)(Rec_Data[0]  << 8 | Rec_Data [1]);
	Raw->Accel_Y_RAW = (int16_t)(Rec_Data[2]  << 8 | Rec_Data [3]);
	Raw->Accel_Z_RAW = (int16_t)(Rec_Data[4]  << 8 | Rec_Data [5]);
	Raw->Temp        = (int16_t)(Rec_Data[6]  << 8 | Rec_Data [7]);
	Raw->Gyro_X_RAW  = (int16_t)(Rec_Data[8]  << 8 | Rec_Data [9]);
	Raw->Gyro_Y_RAW  = (int16_t)(Rec_Data[10] << 8 | Rec_Data [11]);
	Raw->Gyro_Z_RAW  = (int16_t)(Rec_Data[12] << 8 | Rec_Data [13]);

	/*** convert the RAW values into acceleration in 'g'
	     we have to divide according to the Full scale value set in FS_SEL
	     I have configured FS_SEL = 0. So I am dividing by 16384.0
	     for more details check ACCEL_CONFIG Register
		// XA_ST=0,YA_ST=0,ZA_ST=0, FS_SEL=0 -> � 2g  ~  0000 0000  ~ 0x00 16,384
		// XA_ST=0,YA_ST=0,ZA_ST=0, FS_SEL=0 -> � 4g  ~  0000 1000  ~ 0x08 8192
		// XA_ST=0,YA_ST=0,ZA_ST=0, FS_SEL=0 -> � 8g  ~  0001 0000  ~ 0x10 4096
		// XA_ST=0,YA_ST=0,ZA_ST=0, FS_SEL=0 -> � 16g ~  0001 1000  ~ 0x18 2048 		 * ****/
	Raw->Ax = Raw->Accel_X_RAW/8192.0; //16384.0;
	Raw->Ay = Raw->Accel_Y_RAW/8192.0;
	Raw->Az = Raw->Accel_Z_RAW/8192.0;

    /*** convert the RAW values into dps (�/s)
         we have to divide according to the Full scale value set in FS_SEL
         I have configured FS_SEL = 0. So I am dividing by 131.0
         for more details check GYRO_CONFIG Register
		// XG_ST=0,YG_ST=0,ZG_ST=0, FS_SEL=0 -> � 250 �/s      ~  0000 0000  ~ 0x00 131
		// XG_ST=0,YG_ST=0,ZG_ST=0, FS_SEL=1 -> � 500 �/s      ~  0000 1000  ~ 0x08 65.5
		// XG_ST=0,YG_ST=0,ZG_ST=0, FS_SEL=2 -> � 1000 �/s   ~  0001 0000  ~ 0x10 32.8
		// XG_ST=0,YG_ST=0,ZG_ST=0, FS_SEL=3 -> � 2000 �/s   ~  0001 1000  ~ 0x18 16.4  			****/
		if(samples<32) {
		  samples ++;
		  return ;
		} else if(samples <64) {
			gyro_x_offset += Raw->Gyro_X_RAW;
			gyro_y_offset += Raw->Gyro_Y_RAW;
			gyro_z_offset += Raw->Gyro_Z_RAW;
			samples++;
			return;
		} else if(samples==64) {
			gyro_x_offset /= 32;
			gyro_y_offset /= 32;
			gyro_z_offset /= 32;
			samples++;
		} else {
			Raw->Gyro_X_RAW -= gyro_x_offset;
			Raw->Gyro_Y_RAW -= gyro_y_offset;
			Raw->Gyro_Z_RAW -= gyro_z_offset;
		}
	Raw->Gx = Raw->Gyro_X_RAW/65.5;
	Raw->Gy = Raw->Gyro_Y_RAW/65.5;
	Raw->Gz = Raw->Gyro_Z_RAW/65.5;

}

void MPU6050_Read_Angle(MPU6050_Data *Angle){
	MPU6050_Raw Raw;
	MPU6050_Read_Data(&Raw);

	Angle->Pitch_Accel = -atan(Raw.Ax/(sqrt(pow(Raw.Ay,2) + pow(Raw.Az,2)))) * 180/3.145926;
	Angle->Row_Accel   =  atan(Raw.Ay/(sqrt(pow(Raw.Ax,2) + pow(Raw.Az,2)))) * 180/3.145926;

	dt = (double) (HAL_GetTick() - timer) / 1000;
//	if(__HAL_TIM_GET_COUNTER(&htim1) > timer){
//		dt = (__HAL_TIM_GET_COUNTER(&htim1) - timer) / 1000000;
//	}else{
//		dt = (20000 - timer + __HAL_TIM_GET_COUNTER(&htim1)) / 1000000;
//	}

	Angle->Pitch_Gyro = Angle->Pitch_Gyro + Raw.Gy*dt;
	Angle->Row_Gyro   = Angle->Row_Gyro   + Raw.Gx*dt;
	Angle->Yaw_Gyro   = Angle->Yaw_Gyro   + Raw.Gz*dt;
//	timer = __HAL_TIM_GET_COUNTER(&htim1);
	timer = HAL_GetTick();

	Angle->Pitch = Angle->Pitch_Gyro*0.95 + Angle->Pitch_Accel*0.05;
	Angle->Row   = Angle->Row_Gyro*0.95   + Angle->Row_Accel*0.05;
	Angle->Yaw   = Angle->Yaw_Gyro;

	Angle->TempdegC = (float) ((int16_t) Raw.Temp / (float) 340.0 + (float) 36.53);
}
