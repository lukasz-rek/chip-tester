#include "IProtocol.hpp"

#include <cstdint>

#include "stdio.h"
bool IProtocol::checkMemorySize() {
    printf("Beginning mem tests\r\n");
    if (!writeByte(0, 0xAB)) {
        printf("Failed to write canary byte\r\n");
        return false;
    }

    int limit = -1;
    long i = 1;

    while (i > 0) {
        uint8_t read;
        if (!readByte(i, &read)) {
            printf("Failed to read byte at %#08x\r\n", i);
            return false;
        }
        if (read == 0xAB) {
            // Potentially roll over, check
            writeByte(0, 0xCD);
            readByte(i, &read);
            if (read == 0xCD) {
                // Found it
                limit = i;
                break;
            } else {
                writeByte(i, 0x00);
                writeByte(0, 0xAB);
            }
        }
        if (i % 512 == 0) printf("%#08x PASSED\r\n", i);
        i++;
    }
    mem_size = limit;
    printf("Limit found at %#08x\r\n", limit);
    return true;
}

bool IProtocol::eraseSector(uint32_t sector) { return false; }


protocol_timing_t IProtocol::get_timings() {
    return recorded_timings;
}

void IProtocol::reset_timings() {
    recorded_timings = {};
    recorded_timings.min_cycles = INT32_MAX;
}

bool IProtocol::detectFlash() {
    // Write 0x00 to byte 0, then try writing 0xFF back
    writeByte(0, 0x00);
    writeByte(0, 0xFF);
    uint8_t read;
    readByte(0, &read);
    needsErase = (read == 0x00);  // couldn't go back to 0xFF → flash
    return needsErase;
}
