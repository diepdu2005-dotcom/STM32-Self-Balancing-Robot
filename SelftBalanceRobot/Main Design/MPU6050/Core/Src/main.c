/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "math.h"
#include <stdlib.h> // Cho hàm abs()
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define MPU6050_ADDR   0xD0
#define WHO_AM_I_REG   0x75
#define PI             3.141592

#define CALIB_BUFFER_SIZE 100   // Số mẫu đọc để tính trung bình
#define ACEL_DEADZONE     8      // Sai số chấp nhận được cho Accel
#define GYRO_DEADZONE     1      // Sai số chấp nhận được cho Gyro
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */
int16_t raw_ax=0, raw_ay=0, raw_az=0;
int16_t raw_gx=0, raw_gy=0, raw_gz=0;

float Ax, Ay, Az, Gx, Gy, Gz;
const float dt = 10000/1000000.0f;
float pitch = 0.0;
float roll  = 0.0;

int16_t ax_offset, ay_offset, az_offset;
int16_t gx_offset, gy_offset, gz_offset;

// Biến tạm để tính trung bình
int32_t mean_ax, mean_ay, mean_az;
int32_t mean_gx, mean_gy, mean_gz;
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//---------------------------------------------------------------------------//
void MPU6050_Init(void){
  uint8_t check, Data;
	// check device ID WHO_AM_I
	HAL_I2C_Mem_Read(&hi2c1, MPU6050_ADDR, 0x75, 1, &check, 1, 1000);
	if(check == 0x68)//if the device is present
	{ 
		Data = 0;
		HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, 0x6B, 1,&Data, 1, 1000);
		// Set DATA RATE of 1KHz by writing SMPLRT_DIV register
		Data = 0x07;
		HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, 0x19, 1, &Data, 1, 1000);
		
		Data = 0x00;
    HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, 0x1A, 1, &Data, 1, 1000);

		// Set Gyroscopic configuration in GYRO_CONFIG Register
		Data = 0x00;  // XG_ST=0,YG_ST=0,ZG_ST=0, FS_SEL=0 -> � 250 ?/s
		HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, 0x1B, 1, &Data, 1, 1000);

		// Set accelerometer configuration in ACCEL_CONFIG Register
		Data = 0x00;  // XA_ST=0,YA_ST=0,ZA_ST=0, FS_SEL=0 -> � 2g
		HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, 0x1C, 1, &Data, 1, 1000); 
	}
}
//---------------------------------------------------------------------//
void MPU6050_Read_Accel(void)
{
    uint8_t data[6];
    HAL_I2C_Mem_Read(&hi2c1, 0x68<<1, 0x3B, 1, data, 6, 100);

    raw_ax = (int16_t)(data[0] << 8 | data[1]);
    raw_ay = (int16_t)(data[2] << 8 | data[3]);
    raw_az = (int16_t)(data[4] << 8 | data[5]);
}
void MPU6050_Read_Gyro (void)
{
    uint8_t data[6];
    HAL_I2C_Mem_Read(&hi2c1, 0x68<<1, 0x43, 1, data, 6, 100);

    raw_gx = (int16_t)(data[0] << 8 | data[1]);
    raw_gy = (int16_t)(data[2] << 8 | data[3]);
    raw_gz = (int16_t)(data[4] << 8 | data[5]);
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
    long i=0,buff_ax=0,buff_ay=0,buff_az=0,buff_gx=0,buff_gy=0,buff_gz=0;
    // Đọc BUFFER_SIZE mẫu
     while (i<(CALIB_BUFFER_SIZE+11)) {
			 MPU6050_Read_Accel();
			 MPU6050_Read_Gyro();
        // Bỏ qua 100 mẫu đầu để tín hiệu ổn định
        if (i >= 10&& i<=(CALIB_BUFFER_SIZE+10)){
            buff_ax += raw_ax;
            buff_ay += raw_ay;
            buff_az += raw_az;
            buff_gx += raw_gx;
            buff_gy += raw_gy;
            buff_gz += raw_gz;
        }
				if (i==(CALIB_BUFFER_SIZE+10)){
            mean_ax=buff_ax/CALIB_BUFFER_SIZE;
            mean_ay=buff_ay/CALIB_BUFFER_SIZE;
            mean_az=buff_az/CALIB_BUFFER_SIZE;
            mean_gx=buff_gx/CALIB_BUFFER_SIZE;
            mean_gy=buff_gy/CALIB_BUFFER_SIZE;
            mean_gz=buff_gz/CALIB_BUFFER_SIZE;
        }
        i++;
        HAL_Delay(2); // Chờ nhỏ giữa các lần đọc để tránh trùng lặp dữ liệu
   }
}
//-------------------------------------------------------------------------//
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
//-----------------------------------------------------------------------//
void filter( float Ax, float Ay, float Az, float Gx, float Gy, float Gz){
	Ax = (float)(raw_ax)/16384.0;
	Ay = (float)(raw_ay)/16384.0;
	Az = (float)(raw_az-az_offset)/16384.0;
	
	Gx = (float)(raw_gx)/131.0;
	Gy = (float)(raw_gy)/131.0;
	Gz = (float)(raw_gz)/131.0;
	
	float pitchG = pitch + Gx*dt;
	float	rollG  = roll  + Gy*dt;
	
	float pitchA = atan2(Ay, sqrt(Ax*Ax + Az*Az))*(180/PI);
	float rollA  = atan2(Ax, sqrt(Ay*Ay + Az*Az))*(180/PI);
	
	pitch = 0.98*pitchG + 0.02*pitchA;
	roll  = 0.98*rollG  + 0.02*rollA;
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
 MPU6050_Init();
 	MPU6050_Calibrate_Auto();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    MPU6050_Read_Accel();
		MPU6050_Read_Gyro();
	
    filter(Ax, Ay, Az, Gx, Gy, Gz);
		HAL_Delay(100);
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
