#include "IProtocol.hpp"
#include <cstdint>

class spi : public IProtocol {
    public:
    bool check();
    void identify(char * buffer);
    // uint8_t readByte(uint32_t addr);
    const char* getProtocolName() { return "SPI"; }
    private:
    uint32_t jedecId = 0;
};
