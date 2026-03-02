#include "validate/simpleMem.hpp"
#include <cstdint>
#include <stdio.h>

bool simpleMem::validate(IProtocol* protocol) {
    // Read 10 bytes

    protocol->writeByte(0, 0xAB);
    int limit = -1;


    for(int i = 1; i < 16384; i++) {

        uint8_t read = protocol->readByte(i);
        if (read == 0xAB) {
            // Potentially roll over, check
            protocol->writeByte(0, 0xCD);
            read = protocol->readByte(i);
            if (read == 0xCD) {
                // Found it
                limit = i;
                break;
            } else {
                protocol->writeByte(i, 0x00);
                protocol->writeByte(0, 0xAB);
            }
        }
        printf("%#08x PASSED\r\n", i);

    }
    printf("Limit found at %#08x\r\n", limit);


    return true;
}
