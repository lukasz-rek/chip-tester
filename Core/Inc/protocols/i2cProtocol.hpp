#include "IProtocol.hpp"
#include <cstdint>

class i2c : public IProtocol {
    public:
    bool check();
    void identify(char * buffer);
    // uint8_t readByte(uint32_t addr);
    const char* getProtocolName() { return "I2C"; }
    private:
    uint8_t address;
};
