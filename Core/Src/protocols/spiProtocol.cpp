#include "spiProtocol.hpp"

#include <cstdint>

#include "main.h"
#include "spi.h"
#include "stm32h7xx_hal_spi.h"

bool spi::check() {
    uint8_t txBuf[4] = {0x9F, 0x00, 0x00, 0x00};
    uint8_t rxBuf[4] = {0};
    for (int attempt = 0; attempt < 5; attempt++) {
        HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_RESET);
        HAL_SPI_TransmitReceive(&hspi1, txBuf, rxBuf, 4, 10);
        HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_SET);
        uint8_t manuf = rxBuf[1];
        uint8_t memType = rxBuf[2];
        uint8_t capacity = rxBuf[3];
        if (manuf == 0x68 && memType == 0x10 && capacity == 0x15) {
            jedecId = (manuf << 16) | (memType << 8) | capacity;
            needsErase = true;  // <-- ADD THIS
            return true;
        }
        HAL_Delay(1);
    }
    return false;
}
void spi::getDeviceInfo(char* buffer) { sprintf(buffer, "SPI device, JEDEC ID: 0x%06lX, mem size: %d bytes", jedecId, mem_size); }

void spi::enable() {}
void spi::disable() {}

bool spi::readByte(uint32_t addr, uint8_t* data)
{
    uint8_t tx[5] = {0x03, (uint8_t)(addr >> 16), (uint8_t)(addr >> 8), (uint8_t)(addr & 0xFF), 0x00};
    uint8_t rx[5] = {0};

    HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_RESET);
    HAL_StatusTypeDef ret = HAL_SPI_TransmitReceive(&hspi1, tx, rx, 5, 100);
    HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_SET);

    *data = rx[4];
    return ret == HAL_OK;
}

bool spi::writeByte(uint32_t addr, uint8_t data)
{
    uint8_t wren = 0x06;
    HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, &wren, 1, 100);
    HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_SET);

    uint8_t tx[5] = {0x02, (uint8_t)(addr >> 16), (uint8_t)(addr >> 8), (uint8_t)(addr & 0xFF), data};
    HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_RESET);
    HAL_StatusTypeDef ret = HAL_SPI_Transmit(&hspi1, tx, 5, 100);  // <-- 5 NOT 4
    HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_SET);

    if (ret != HAL_OK) return false;

    uint32_t start = HAL_GetTick();
    while ((HAL_GetTick() - start) < 10) {
        uint8_t sr_tx[2] = {0x05, 0x00};
        uint8_t sr_rx[2] = {0};
        HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_RESET);
        HAL_SPI_TransmitReceive(&hspi1, sr_tx, sr_rx, 2, 100);
        HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_SET);
        if (!(sr_rx[1] & 0x01)) return true;
    }
    return false;
}

bool spi::eraseSector(uint32_t addr)
{
    uint8_t wren = 0x06;
    HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, &wren, 1, 100);
    HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_SET);

    uint8_t tx[4] = {0x20, (uint8_t)(addr >> 16), (uint8_t)(addr >> 8), (uint8_t)(addr & 0xFF)};
    HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, tx, 4, 100);
    HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_SET);

    // Sector erase takes up to 400ms on BY25Q16AW
    uint32_t start = HAL_GetTick();
    while ((HAL_GetTick() - start) < 500) {
        uint8_t sr_tx[2] = {0x05, 0x00};
        uint8_t sr_rx[2] = {0};
        HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_RESET);
        HAL_SPI_TransmitReceive(&hspi1, sr_tx, sr_rx, 2, 100);
        HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_SET);
        if (!(sr_rx[1] & 0x01)) return true;
    }
    return false;
}

bool spi::checkMemorySize() {
    // We don't need boundary checks for this bad boi
    uint8_t txBuf[4] = {0x9F, 0x00, 0x00, 0x00};
    uint8_t rxBuf[4] = {0};

    HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_RESET);  // CS LOW
    HAL_SPI_TransmitReceive(&hspi1, txBuf, rxBuf, 4, 10);
    HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_SET);  // CS HIGH


    mem_size = 1UL << rxBuf[3];
    return true;
}
