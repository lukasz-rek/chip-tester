#include "IProtocol.hpp"
#include <cstdint>

class spi : public IProtocol {
    public:
    bool check();
    void identify(char * buffer);
    uint8_t readByte(uint32_t addr);
    bool writeByte(uint32_t addr, uint8_t data);
    const char* getProtocolName() { return "SPI"; }
    private:
    uint32_t jedecId = 0;
};
