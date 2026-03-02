#pragma once
#include <stdint.h>


class IProtocol {
    public:
    virtual ~IProtocol() = default;
    virtual bool check() = 0;
    virtual void identify(char * buffer) = 0;
    virtual const char* getProtocolName() = 0;
    virtual uint8_t readByte(uint32_t addr) = 0;
    virtual bool    writeByte(uint32_t addr, uint8_t data) = 0;
    // TODO: more coming
};
