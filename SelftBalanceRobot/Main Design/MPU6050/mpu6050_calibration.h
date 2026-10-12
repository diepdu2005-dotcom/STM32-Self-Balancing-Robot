#ifndef __MPU6050_H
#define __MPU6050_H

#include "stm32f1xx_hal.h"

// Struct nếu cần
typedef struct {
    int16_t Accel_X_RAW;
    int16_t Accel_Y_RAW;
    int16_t Accel_Z_RAW;
    int16_t Gyro_X_RAW;
    int16_t Gyro_Y_RAW;
    int16_t Gyro_Z_RAW;
} MPU6050_Raw;

// Prototype hàm
void MPU6050_Init(void);
void MPU6050_GetRawData(int16_t* ax, int16_t* ay, int16_t* az,
                        int16_t* gx, int16_t* gy, int16_t* gz);
void MPU6050_Calibrate_Auto(void);

#endif
