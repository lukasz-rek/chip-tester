#include "i2cProtocol.hpp"
#include "i2c.h"
#include <cstdint>

bool i2c::check() {
    for (uint8_t addr = 1; addr < 128; addr++) {
        if (HAL_I2C_IsDeviceReady(&hi2c1, addr << 1, 2, 10) == HAL_OK) {
            i2c::address = addr;
            return true;
        }
    }
    return false;
}

void i2c::identify(char * buffer) {
    sprintf(buffer, "I2C device at address: 0x%02X", i2c::address);
}

uint8_t i2c::readByte(uint32_t addr) {
    uint8_t data;
    HAL_StatusTypeDef ret = HAL_I2C_Mem_Read(&hi2c1,
                     i2c::address << 1,
                     addr,
                     I2C_MEMADD_SIZE_16BIT,
                     &data,
                     1,
                     HAL_MAX_DELAY);
    if (ret != HAL_OK) {
        printf("Failed to read memory\r\n");
    }
    return data;
}

bool i2c::writeByte(uint32_t addr, uint8_t data) {
    HAL_StatusTypeDef ret = HAL_I2C_Mem_Write(&hi2c1,
                                               i2c::address << 1,
                                               addr,
                                               I2C_MEMADD_SIZE_16BIT,
                                               &data,
                                               1,
                                               HAL_MAX_DELAY);
    HAL_Delay(5);
    return ret == HAL_OK;
}
