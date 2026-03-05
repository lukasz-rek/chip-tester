#include "IProtocol.hpp"

#include "stdio.h"
bool IProtocol::checkMemorySize() {
    printf("Beginning mem tests\r\n");
    if (!writeByte(0, 0xAB)) {
        printf("Failed to write canary byte\r\n");
        return false;
    }

    int limit = -1;

    for (int i = 1; i < 16384; i++) {
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
    }
    mem_size = limit;
    printf("Limit found at %#08x\r\n", limit);
    return true;
}
