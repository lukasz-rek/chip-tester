#include "sramSpiProtcol.hpp"
#include <stdint.h>
#include <cstdint>
#include <cstdio>
#include "main.h"
#include "spi.h"
#include "stm32h7xx_hal_spi.h"

bool sramSpi::check()
{
    // Reset peripheral state left by previous protocol attempts or our own last run
    HAL_SPI_Abort(&hspi1);
    __HAL_SPI_CLEAR_OVRFLAG(&hspi1);
    HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_SET);
    HAL_Delay(5);

    // Force byte mode
    uint8_t wrsr[2] = {0x01, 0x00};
    HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, wrsr, 2, 10);
    HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_SET);
    HAL_Delay(5);

    const uint32_t testAddr = 0x0000;
    const uint8_t patterns[3] = {0xFF, 0xAB, 0x00};

    for (int attempt = 0; attempt < 3; attempt++) {
        bool ok = true;
        for (uint8_t pattern : patterns) {
            uint8_t readBack = ~pattern;
            if (!writeByte(testAddr, pattern) || !readByte(testAddr, &readBack) || readBack != pattern) {
                ok = false;
                break;
            }
        }
        if (ok) return true;
        HAL_Delay(5);
    }
    return false;
}

void sramSpi::getDeviceInfo(char* buffer)
{
    // uint8_t tx[2] = {0x05, 0x00};
    // uint8_t rx[2] = {0};
    // HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_RESET);
    // HAL_SPI_TransmitReceive(&hspi1, tx, rx, 2, 10);
    // HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_SET);

    sprintf(buffer, "Microchip 23K256 SPI SRAM | size: %lu bytes", mem_size);
}

bool sramSpi::readByte(uint32_t addr, uint8_t* data)
{
    uint8_t tx[4] = {0x03, (uint8_t)(addr >> 8), (uint8_t)(addr & 0xFF), 0x00};
    uint8_t rx[4] = {0};
    HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_RESET);
    HAL_StatusTypeDef ret = HAL_SPI_TransmitReceive(&hspi1, tx, rx, 4, 100);
    HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_SET);
    *data = rx[3];
    return ret == HAL_OK;
}

bool sramSpi::writeByte(uint32_t addr, uint8_t data)
{
    uint8_t tx[4] = {0x02, (uint8_t)(addr >> 8), (uint8_t)(addr & 0xFF), data};
    HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_RESET);
    HAL_StatusTypeDef ret = HAL_SPI_Transmit(&hspi1, tx, 4, 100);
    HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_SET);
    return ret == HAL_OK;
}

void sramSpi::enable()  {}
void sramSpi::disable() {}
