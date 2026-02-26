#include "spiProtocol.hpp"
#include "spi.h"
#include "main.h"

bool spi::check() {
    uint8_t txBuf[4] = {0x9F, 0x00, 0x00, 0x00};
    uint8_t rxBuf[4] = {0};

    for (int attempt = 0; attempt < 5; attempt++) {
        HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_RESET); // CS LOW
        HAL_SPI_TransmitReceive(&hspi1, txBuf, rxBuf, 4, 10);
        HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_SET);   // CS HIGH

        uint8_t manuf    = rxBuf[1];
        uint8_t memType  = rxBuf[2];
        uint8_t capacity = rxBuf[3];

        if (manuf == 0x68 && memType == 0x10 && capacity == 0x15) {
            jedecId = (manuf << 16) | (memType << 8) | capacity;
            return true;
        }
        HAL_Delay(1);
    }
    return false;
}
void spi::identify(char* buffer) {
    sprintf(buffer, "SPI device, JEDEC ID: 0x%06lX", jedecId);
}
