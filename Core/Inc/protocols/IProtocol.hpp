#pragma once
#include <stdint.h>
#include <cstdint>

enum ic_type_t { memory, clock, security, other };

typedef struct ProtocolTiming {
    uint32_t total_program_cycles = 0;  // Avg. of this should degrade
    uint32_t min_cycles = UINT32_MAX, max_cycles = 0;


    uint32_t readByteTransactions = 0;
    uint32_t writeByteTransactions = 0;
    uint32_t eraseSectorTransactions = 0;

} protocol_timing_t;

class IProtocol {
   public:
    virtual ~IProtocol() = default;
    /// Checks if connection established
    virtual bool check() = 0;
    /// Return everything we know so far about device
    virtual void getDeviceInfo(char* buffer) = 0;
    /// Get name id'ing this protocol
    virtual const char* getProtocolName() = 0;
    virtual void enable() = 0;
    virtual void disable() = 0;

    // Timing read/reset
    virtual protocol_timing_t get_timings();
    virtual void reset_timings();

    /// Checks memory size, should store it internally
    virtual bool checkMemorySize();

    virtual bool detectFlash();
    virtual bool eraseSector(uint32_t addr);

    virtual bool readByte(uint32_t addr, uint8_t* data) = 0;
    virtual bool writeByte(uint32_t addr, uint8_t data) = 0;

    uint8_t address;
    uint32_t mem_size;
    bool needsErase = false;
    int32_t lastErasedSector = -1;
    protocol_timing_t recorded_timings;
    ic_type_t ic_type;
};
