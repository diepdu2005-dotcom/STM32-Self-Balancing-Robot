#include "main.h"
#include <stdlib.h> // Cho hàm abs()

// CẤU HÌNH THAM SỐ HIỆU CHỈNH
#define CALIB_BUFFER_SIZE 1000   // Số mẫu đọc để tính trung bình
#define ACEL_DEADZONE     8      // Sai số chấp nhận được cho Accel
#define GYRO_DEADZONE     1      // Sai số chấp nhận được cho Gyro

// Biến toàn cục lưu Offset tìm được
int16_t ax_offset, ay_offset, az_offset;
int16_t gx_offset, gy_offset, gz_offset;

// Biến tạm để tính trung bình
int32_t mean_ax, mean_ay, mean_az;
int32_t mean_gx, mean_gy, mean_gz;

// ---------------------------------------------------------
// HÀM GIẢ LẬP (Bạn thay bằng hàm thực tế của thư viện bạn)
// ---------------------------------------------------------
extern I2C_HandleTypeDef hi2c1; // Giả sử dùng I2C1
//-------------------------------------------------------------------//
void MPU6050_GetRawData(int16_t* ax, int16_t* ay, int16_t* az,
                        int16_t* gx, int16_t* gy, int16_t* gz)
{
    uint8_t data[14];

    HAL_I2C_Mem_Read(&hi2c1, 0x68<<1, 0x3B, 1, data, 14, 100);

    *ax = (int16_t)(data[0] << 8 | data[1]);
    *ay = (int16_t)(data[2] << 8 | data[3]);
    *az = (int16_t)(data[4] << 8 | data[5]);

    *gx = (int16_t)(data[8] << 8 | data[9]);
    *gy = (int16_t)(data[10] << 8 | data[11]);
    *gz = (int16_t)(data[12] << 8 | data[13]);
}
//------------------------------------------------------------------//
void MPU6050_SetOffsets(int16_t ax_off, int16_t ay_off, int16_t az_off,
                        int16_t gx_off, int16_t gy_off, int16_t gz_off)
{
    uint8_t data[2];

    // ACCEL X
    data[0] = (uint8_t)(ax_off >> 8);
    data[1] = (uint8_t)(ax_off & 0xFF);
    HAL_I2C_Mem_Write(&hi2c1, 0x68<<1, 0x06, 1, data, 2, 100);

    // ACCEL Y
    data[0] = (uint8_t)(ay_off >> 8);
    data[1] = (uint8_t)(ay_off & 0xFF);
    HAL_I2C_Mem_Write(&hi2c1, 0x68<<1, 0x08, 1, data, 2, 100);

    // ACCEL Z
    data[0] = (uint8_t)(az_off >> 8);
    data[1] = (uint8_t)(az_off & 0xFF);
    HAL_I2C_Mem_Write(&hi2c1, 0x68<<1, 0x0A, 1, data, 2, 100);

    // GYRO X
    data[0] = (uint8_t)(gx_off >> 8);
    data[1] = (uint8_t)(gx_off & 0xFF);
    HAL_I2C_Mem_Write(&hi2c1, 0x68<<1, 0x13, 1, data, 2, 100);

    // GYRO Y
    data[0] = (uint8_t)(gy_off >> 8);
    data[1] = (uint8_t)(gy_off & 0xFF);
    HAL_I2C_Mem_Write(&hi2c1, 0x68<<1, 0x15, 1, data, 2, 100);

    // GYRO Z
    data[0] = (uint8_t)(gz_off >> 8);
    data[1] = (uint8_t)(gz_off & 0xFF);
    HAL_I2C_Mem_Write(&hi2c1, 0x68<<1, 0x17, 1, data, 2, 100);
}
// -------------------------------------------------------------//

// BƯỚC 1: HÀM TÍNH GIÁ TRỊ TRUNG BÌNH (MEAN)
void MPU_Calculate_Mean_Sensors() {
    int32_t i = 0;
    int32_t buff_ax=0, buff_ay=0, buff_az=0;
    int32_t buff_gx=0, buff_gy=0, buff_gz=0;
    int16_t raw_ax, raw_ay, raw_az, raw_gx, raw_gy, raw_gz;

    // Đọc BUFFER_SIZE mẫu
    while (i < (CALIB_BUFFER_SIZE + 100)) {
        MPU6050_Read_Gyro(&raw_gx, &raw_gy, &raw_gz);
        MPU6050_Read_Accel(&raw_ax, &raw_ay, &raw_az);
        // Bỏ qua 100 mẫu đầu để tín hiệu ổn định
        if (i >= 100) {
            buff_ax += raw_ax;
            buff_ay += raw_ay;
            buff_az += raw_az;
            buff_gx += raw_gx;
            buff_gy += raw_gy;
            buff_gz += raw_gz;
        }
        i++;
        HAL_Delay(2); // Chờ nhỏ giữa các lần đọc để tránh trùng lặp dữ liệu
    }

    // Tính trung bình
    mean_ax = buff_ax / CALIB_BUFFER_SIZE;
    mean_ay = buff_ay / CALIB_BUFFER_SIZE;
    mean_az = buff_az / CALIB_BUFFER_SIZE;
    mean_gx = buff_gx / CALIB_BUFFER_SIZE;
    mean_gy = buff_gy / CALIB_BUFFER_SIZE;
    mean_gz = buff_gz / CALIB_BUFFER_SIZE;
}

// BƯỚC 2: HÀM CALIBRATION CHÍNH
void MPU6050_Calibrate_Auto() {
    // 1. Đặt lại offset về 0 trước khi bắt đầu
    MPU6050_SetOffsets(0, 0, 0, 0, 0, 0);
    
    // 2. Đọc giá trị trung bình lần đầu
    MPU_Calculate_Mean_Sensors();
    
    // 3. Tính toán Offset ước lượng ban đầu (Công thức kinh nghiệm)
    ax_offset = -mean_ax / 8;
    ay_offset = -mean_ay / 8;
    az_offset = (16384 - mean_az) / 8; // Z axis cần bù để về 16384 (1G)

    gx_offset = -mean_gx / 4;
    gy_offset = -mean_gy / 4;
    gz_offset = -mean_gz / 4;

    // 4. Vòng lặp tinh chỉnh (Fine-tuning)
    while (1) {
        int ready = 0;
        
        // Ghi offset hiện tại vào cảm biến
        MPU6050_SetOffsets(ax_offset, ay_offset, az_offset, gx_offset, gy_offset, gz_offset);
        
        // Đọc lại giá trị trung bình mới
        MPU_Calculate_Mean_Sensors();
        
        // --- KIỂM TRA VÀ ĐIỀU CHỈNH ACCELEROMETER ---
        // X Axis
        if (abs(mean_ax) <= ACEL_DEADZONE) ready++;
        else ax_offset = ax_offset - mean_ax / ACEL_DEADZONE;

        // Y Axis
        if (abs(mean_ay) <= ACEL_DEADZONE) ready++;
        else ay_offset = ay_offset - mean_ay / ACEL_DEADZONE;

        // Z Axis (Mục tiêu là 16384)
        if (abs(16384 - mean_az) <= ACEL_DEADZONE) ready++;
        else az_offset = az_offset + (16384 - mean_az) / ACEL_DEADZONE;

        // --- KIỂM TRA VÀ ĐIỀU CHỈNH GYROSCOPE ---
        // X Axis
        if (abs(mean_gx) <= GYRO_DEADZONE) ready++;
        else gx_offset = gx_offset - mean_gx / (GYRO_DEADZONE + 1);

        // Y Axis
        if (abs(mean_gy) <= GYRO_DEADZONE) ready++;
        else gy_offset = gy_offset - mean_gy / (GYRO_DEADZONE + 1);

        // Z Axis
        if (abs(mean_gz) <= GYRO_DEADZONE) ready++;
        else gz_offset = gz_offset - mean_gz / (GYRO_DEADZONE + 1);

        // Nếu cả 6 trục đều đạt chuẩn (ready == 6) thì thoát
        if (ready == 6) break;
    }
}