#include <cstdint>

#include "IProtocol.hpp"

class spi : public IProtocol {
   public:
    bool check();
    void getDeviceInfo(char* buffer);
    bool readByte(uint32_t addr, uint8_t* data);
    bool writeByte(uint32_t addr, uint8_t data);
    const char* getProtocolName() { return "SPI"; }

   private:
    uint32_t jedecId = 0;
};
