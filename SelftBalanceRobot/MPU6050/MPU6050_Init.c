uint8_t MPU6050_Init(I2C_HandleTypeDef *I2Cx)
{
uint8_t Data;
// check device ID WHO_AM_I
HAL_I2C_Mem_Read(I2Cx, MPU6050_ADDR, WHO_AM_I_REG, 1, &check, 1, i2c_timeout);
if (check == 0x98) // 0x68 will be returned by the sensor if everything goes well
{
// power management register 0X6B we should write all 0's to wake the sensor up
Data = 0;
HAL_I2C_Mem_Write(I2Cx, MPU6050_ADDR, PWR_MGMT_1_REG, 1, &Data, 1, i2c_timeout);
// Set DATA RATE of 1KHz by writing SMPLRT_DIV register
Data = 0x07;
HAL_I2C_Mem_Write(I2Cx, MPU6050_ADDR, SMPLRT_DIV_REG, 1, &Data, 1, i2c_timeout);
// Set accelerometer configuration in ACCEL_CONFIG Register
// XA_ST=0,YA_ST=0,ZA_ST=0, FS_SEL=0 -> ? 2g
Data = 0x00;
HAL_I2C_Mem_Write(I2Cx, MPU6050_ADDR, ACCEL_CONFIG_REG, 1, &Data, 1, i2c_timeout);
// Set Gyroscopic configuration in GYRO_CONFIG Register
// XG_ST=0,YG_ST=0,ZG_ST=0, FS_SEL=0 -> ? 250 ?/s
Data = 0x00;
HAL_I2C_Mem_Write(I2Cx, MPU6050_ADDR, GYRO_CONFIG_REG, 1, &Data, 1, i2c_timeout);
// return 0;
}
return 1;
}