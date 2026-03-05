#pragma once
#include <stdint.h>

enum ic_type_t { memory, clock, security, other };

class IProtocol {
   public:
    virtual ~IProtocol() = default;
    /// Checks if connection established
    virtual bool check() = 0;
    /// Return everything we know so far about device
    virtual void getDeviceInfo(char* buffer) = 0;
    /// Get name id'ing this protocol
    virtual const char* getProtocolName() = 0;

    /// Checks memory size, should store it internally
    virtual bool checkMemorySize();

    virtual bool readByte(uint32_t addr, uint8_t* data) = 0;
    virtual bool writeByte(uint32_t addr, uint8_t data) = 0;

    uint8_t address;
    uint32_t mem_size;
    ic_type_t ic_type;
};
