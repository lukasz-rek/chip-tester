#include "i2cProtocol.hpp"

#include <stdio.h>

#include <cstdint>

#include "i2c.h"
#include "stm32h7xx_hal_i2c.h"

bool i2c::check() {
    for (uint8_t addr = 1; addr < 128; addr++) {
        if (HAL_I2C_IsDeviceReady(&hi2c1, addr << 1, 2, 10) == HAL_OK) {
            i2c::address = addr;
            return true;
        }
    }
    return false;
}

void i2c::getDeviceInfo(char* buffer) {
    sprintf(buffer, "I2C device at address: 0x%02X with mem size %d bytes", i2c::address, i2c::mem_size);
}

void i2c::enable() { MX_I2C1_Init(); }

void i2c::disable() {
    __HAL_I2C_DISABLE(&hi2c1);
    HAL_I2C_DeInit(&hi2c1);
    hi2c1.State = HAL_I2C_STATE_RESET;
    hi2c1.Lock = HAL_UNLOCKED;
}

bool i2c::readByte(uint32_t addr, uint8_t* data) {
    HAL_StatusTypeDef ret =
        HAL_I2C_Mem_Read(&hi2c1, i2c::address << 1, addr, I2C_MEMADD_SIZE_16BIT, data, 1, HAL_MAX_DELAY);
    if (ret != HAL_OK) {
        uint32_t err = HAL_I2C_GetError(&hi2c1);
        printf("I2C read failed (HAL status=%d, ErrorCode=0x%08lX):", ret, err);
        if (err & HAL_I2C_ERROR_BERR) printf(" BUS_ERROR");
        if (err & HAL_I2C_ERROR_ARLO) printf(" ARBITRATION_LOST");
        if (err & HAL_I2C_ERROR_AF) printf(" ACK_FAILURE");  // most common: wrong address
        if (err & HAL_I2C_ERROR_OVR) printf(" OVERRUN");
        if (err & HAL_I2C_ERROR_DMA) printf(" DMA_ERROR");
        if (err & HAL_I2C_ERROR_TIMEOUT) printf(" TIMEOUT");
        printf("\r\n");
        return false;
    }
    return true;
}

bool i2c::writeByte(uint32_t addr, uint8_t data) {
    HAL_StatusTypeDef ret =
        HAL_I2C_Mem_Write(&hi2c1, i2c::address << 1, addr, I2C_MEMADD_SIZE_16BIT, &data, 1, HAL_MAX_DELAY);
    HAL_Delay(5);
    return ret == HAL_OK;
}
