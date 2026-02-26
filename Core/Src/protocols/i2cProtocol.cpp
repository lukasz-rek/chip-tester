#include "i2cProtocol.hpp"
#include "i2c.h"

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
