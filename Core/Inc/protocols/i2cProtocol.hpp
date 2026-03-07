#include <cstdint>

#include "IProtocol.hpp"

class i2c : public IProtocol {
   public:
    bool check() override;
    void getDeviceInfo(char* buffer) override;
    bool readByte(uint32_t addr, uint8_t* data) override;
    bool writeByte(uint32_t addr, uint8_t data) override;
    void enable() override;
    void disable() override;
    const char* getProtocolName() override { return "I2C"; }

   private:
};
