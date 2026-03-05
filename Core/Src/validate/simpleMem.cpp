#include "validate/simpleMem.hpp"

#include <cstdint>

#include "stdio.h"

bool simpleMem::validate(IProtocol* protocol) {
    // Just write 0x00 and 0xFF to all bytes. Check that it's there
    for (int i = 0; i < protocol->mem_size; i++) {
        uint8_t data;
        bool ret = protocol->writeByte(i, 0xFF);
        if (!ret) return false;
        ret = protocol->readByte(i, &data);
        if (!ret || data != 0xFF) return false;
        ret = protocol->writeByte(i, 0x00);
        if (!ret) return false;
        ret = protocol->readByte(i, &data);
        if (!ret || data != 0x00) return false;

        if (i % 512 == 0) printf("%#08x PASSED\r\n", i);
    }

    return true;
}
