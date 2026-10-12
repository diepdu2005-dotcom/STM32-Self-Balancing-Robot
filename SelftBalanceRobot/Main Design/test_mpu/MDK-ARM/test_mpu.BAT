SET PATH=C:\Keil_v5\ARM\ARMCC\Bin;C:\Windows\system32;C:\Windows;C:\Windows\System32\Wbem;C:\Windows\System32\WindowsPowerShell\v1.0\;C:\Windows\System32\OpenSSH\;C:\Program Files (x86)\NVIDIA Corporation\PhysX\Common;C:\Program Files\NVIDIA Corporation\NVIDIA NvDLISR;C:\WINDOWS\system32;C:\WINDOWS;C:\WINDOWS\System32\Wbem;C:\WINDOWS\System32\WindowsPowerShell\v1.0\;C:\WINDOWS\System32\OpenSSH\;C:\texlive\2025\bin\windows;C:\Program Files\MATLAB\R2021a\runtime\win64;C:\Program Files\MATLAB\R2021a\bin;C:\Program Files (x86)\STMicroelectronics\STM32 ST-LINK Utility\ST-LINK Utility;C:\Users\ACER\AppData\Local\Microsoft\WindowsApps;;C:\modeltech64_10.5\win64
SET CPU_TYPE=STM32F103C8
SET CPU_VENDOR=STMicroelectronics
SET UV2_TARGET=test_mpu
SET CPU_CLOCK=0x007A1200
"C:\Keil_v5\ARM\ARMCC\Bin\ArmAsm" --Via ".\test_mpu\startup_stm32f103xb._ia"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\test_mpu\main.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\test_mpu\stm32f1xx_it.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\test_mpu\stm32f1xx_hal_msp.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\test_mpu\mpu6050.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\test_mpu\stm32f1xx_hal_gpio_ex.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\test_mpu\stm32f1xx_hal_i2c.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\test_mpu\stm32f1xx_hal.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\test_mpu\stm32f1xx_hal_rcc.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\test_mpu\stm32f1xx_hal_rcc_ex.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\test_mpu\stm32f1xx_hal_gpio.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\test_mpu\stm32f1xx_hal_dma.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\test_mpu\stm32f1xx_hal_cortex.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\test_mpu\stm32f1xx_hal_pwr.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\test_mpu\stm32f1xx_hal_flash.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\test_mpu\stm32f1xx_hal_flash_ex.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\test_mpu\stm32f1xx_hal_exti.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\test_mpu\stm32f1xx_hal_tim.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\test_mpu\stm32f1xx_hal_tim_ex.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\test_mpu\system_stm32f1xx.__i"
"C:\Keil_v5\ARM\ARMCC\Bin\ArmLink" --Via ".\test_mpu\test_mpu.lnp"
"C:\Keil_v5\ARM\ARMCC\Bin\fromelf.exe" ".\test_mpu\test_mpu.axf" --i32combined --output ".\test_mpu\test_mpu.hex"
