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

bool i2c::eraseSector(uint32_t addr) { return false; }

void i2c::getDeviceInfo(char* buffer) {
    sprintf(buffer, "I2C device at address: 0x%02X with mem size %d bytes", i2c::address, i2c::mem_size);
}

void i2c::enable() {
    // Pull ups on
    HAL_GPIO_WritePin(GPIOF, GPIO_PIN_3, GPIO_PIN_SET);
    MX_I2C1_Init();
}

void i2c::disable() {
    // Pull ups off
    HAL_GPIO_WritePin(GPIOF, GPIO_PIN_3, GPIO_PIN_RESET);

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

    if (ret != HAL_OK) return false;

    uint32_t p0 = DWT->CYCCNT;
    while (HAL_I2C_IsDeviceReady(&hi2c1, i2c::address << 1, 1, 10) != HAL_OK);
    uint32_t write_cycles = DWT->CYCCNT - p0;

    recorded_timings.total_program_cycles += write_cycles;
    if (write_cycles < recorded_timings.min_cycles) recorded_timings.min_cycles = write_cycles;
    if (write_cycles > recorded_timings.max_cycles) recorded_timings.max_cycles = write_cycles;
    recorded_timings.writeByteTransactions++;

    return true;
}
