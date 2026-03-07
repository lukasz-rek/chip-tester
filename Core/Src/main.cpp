/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 STMicroelectronics.
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

#include "IProtocol.hpp"
#include "gpio.h"
#include "i2c.h"
#include "quadspi.h"
#include "spi.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <array>

#include "i2cProtocol.hpp"
#include "simpleMem.hpp"
#include "spiProtocol.hpp"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

COM_InitTypeDef BspCOMInit;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
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
    MX_QUADSPI_Init();
    MX_I2C1_Init();
    MX_SPI1_Init();
    /* USER CODE BEGIN 2 */
    i2c i2c;
    spi spi;
    simpleMem simpleMem;
    std::array<IProtocol*, 2> protocols = {&i2c, &spi};

    /* USER CODE END 2 */

    /* Initialize leds */
    BSP_LED_Init(LED_GREEN);
    BSP_LED_Init(LED_BLUE);
    BSP_LED_Init(LED_RED);

    /* Initialize USER push-button, will be used to trigger an interrupt each time it's pressed.*/
    BSP_PB_Init(BUTTON_USER, BUTTON_MODE_EXTI);

    /* Initialize COM1 port (115200, 8 bits (7-bit data + 1 stop bit), no parity */
    BspCOMInit.BaudRate = 115200;
    BspCOMInit.WordLength = COM_WORDLENGTH_8B;
    BspCOMInit.StopBits = COM_STOPBITS_1;
    BspCOMInit.Parity = COM_PARITY_NONE;
    BspCOMInit.HwFlowCtl = COM_HWCONTROL_NONE;
    if (BSP_COM_Init(COM1, &BspCOMInit) != BSP_ERROR_NONE) {
        Error_Handler();
    }

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1) {
        HAL_Delay(500);

        IProtocol* found = NULL;
        uint8_t ch;


        printf("=== SPI FULL DIAGNOSTIC v2 ===\r\n");

        auto read_sr = [&]() -> uint8_t {
            uint8_t tx[2] = {0x05, 0x00};
            uint8_t rx[2] = {0};
            HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_RESET);
            HAL_SPI_TransmitReceive(&hspi1, tx, rx, 2, 100);
            HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_SET);
            return rx[1];
        };

        auto send_wren = [&]() {
            uint8_t tx = 0x06;
            uint8_t rx = 0;
            HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_RESET);
            HAL_SPI_TransmitReceive(&hspi1, &tx, &rx, 1, 100);
            HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_SET);
        };

        // 1. Initial SR
        uint8_t sr = read_sr();
        printf("SR initial: 0x%02X\r\n", sr);

        // 2. WREN latch test
        send_wren();
        sr = read_sr();
        printf("SR after WREN: 0x%02X (WEL=%d, MUST be 1)\r\n", sr, (sr >> 1) & 1);

        if (!((sr >> 1) & 1)) {
            printf("WREN STILL NOT LATCHING — hardware issue\r\n");
            printf("=== END DIAGNOSTIC ===\r\n");
        }

        // 3. Erase sector 0
        // printf("Erasing sector 0...\r\n");
        // send_wren();
        // uint8_t etx[4] = {0x20, 0x00, 0x00, 0x00};
        // uint8_t erx[4] = {0};
        // HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_RESET);
        // HAL_SPI_TransmitReceive(&hspi1, etx, erx, 4, 100);
        // HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_SET);
        // HAL_Delay(500);
        // sr = read_sr();
        // printf("SR after erase: 0x%02X (WIP should be 0)\r\n", sr);

        // uint8_t val;
        // spi.readByte(0, &val);
        // printf("After erase: 0x%02X (expect 0xFF)\r\n", val);

        // // 4. Write 0xAB to addr 0
        // printf("Writing 0xAB...\r\n");
        // send_wren();
        // sr = read_sr();
        // printf("SR before write: 0x%02X (WEL=%d)\r\n", sr, (sr >> 1) & 1);

        // uint8_t wtx[5] = {0x02, 0x00, 0x00, 0x00, 0xAB};
        // uint8_t wrx[5] = {0};
        // HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_RESET);
        // HAL_StatusTypeDef ret = HAL_SPI_TransmitReceive(&hspi1, wtx, wrx, 5, 100);
        // HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_SET);
        // printf("TransmitReceive returned: %d\r\n", ret);

        // HAL_Delay(10);
        // sr = read_sr();
        // printf("SR after write: 0x%02X\r\n", sr);

        // spi.readByte(0, &val);
        // printf("Read back: 0x%02X (expect 0xAB)\r\n", val);

        // printf("=== END DIAGNOSTIC v2 ===\r\n");

        printf("Trying to detect components\r\n");

        while (true) {
            for (IProtocol* p : protocols) {
                p->enable();
                if (p->check()) {
                    char buffer[128];
                    p->getDeviceInfo(buffer);
                    printf("Found %s with protocol %s\r\n", buffer, p->getProtocolName());
                    found = p;
                    break;
                } else {
                    printf("Failed to find with protocol %s\r\n", p->getProtocolName());
                    p->disable();
                }
            }
            if (found == NULL) continue;
            printf("\r\nPress s to begin, any other to try selecting again...\r\n");
            HAL_UART_Receive(&hcom_uart[COM1], &ch, 1, HAL_MAX_DELAY);
            if (ch == 's') {
                break;
            }
            found->disable();
            found = NULL;
        }

        found->checkMemorySize();
        char buffer[128];
        found->getDeviceInfo(buffer);
        printf("%s\r\n", buffer);
        if (found->detectFlash()) {
            printf("Flash detected, will apply sector erase!\r\n");
        }

        printf("\r\nPress any key to begin tests...\r\n");
        HAL_UART_Receive(&hcom_uart[COM1], &ch, 1, HAL_MAX_DELAY);
        printf("Beginning tests\r\n");

        bool reto = simpleMem.validate(found);
        if (reto) {
            printf("\r\nTest passed\r\n");
        } else {
            printf("\r\nTest failed\r\n");
        }

        printf("\r\nPress any key to test next component...\r\n");
        found->disable();
        HAL_UART_Receive(&hcom_uart[COM1], &ch, 1, HAL_MAX_DELAY);
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
    }
    /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Supply configuration update enable
     */
    HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

    /** Configure the main internal regulator output voltage
     */
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

    while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {
    }

    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM = 4;
    RCC_OscInitStruct.PLL.PLLN = 9;
    RCC_OscInitStruct.PLL.PLLP = 2;
    RCC_OscInitStruct.PLL.PLLQ = 1;
    RCC_OscInitStruct.PLL.PLLR = 2;
    RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
    RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOMEDIUM;
    RCC_OscInitStruct.PLL.PLLFRACN = 3072;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 |
                                  RCC_CLOCKTYPE_PCLK2 | RCC_CLOCKTYPE_D3PCLK1 | RCC_CLOCKTYPE_D1PCLK1;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV1;
    RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK) {
        Error_Handler();
    }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1) {
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
void assert_failed(uint8_t* file, uint32_t line) {
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
