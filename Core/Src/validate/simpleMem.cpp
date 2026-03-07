#include "validate/simpleMem.hpp"

#include <cstdint>

#include "stdio.h"

bool simpleMem::validate(IProtocol* protocol)
{
    if (protocol->needsErase) {
        // Flash: test per-sector
        uint32_t sector_size = 4096;
        uint32_t num_sectors = protocol->mem_size / sector_size;

        for (uint32_t s = 0; s < num_sectors; s++) {
            uint32_t base = s * sector_size;

            // Erase sets everything to 0xFF
            if (!protocol->eraseSector(base)) {
                        printf("erase failed at %#08lx\r\n", base);
                        return false;
                    }

            // Verify first and last byte are 0xFF
            uint8_t data;
            if (!protocol->readByte(base, &data)) {
                        printf("read failed at %#08lx\r\n", base);
                        return false;
                    }
            if (data != 0xFF) {
                       printf("erase verify failed at %#08lx: got 0x%02X\r\n", base, data);
                       return false;
                   }
            if (!protocol->readByte(base + sector_size - 1, &data) || data != 0xFF) return false;

            // Write 0x00, verify (1→0, always works on flash)
            if (!protocol->writeByte(base, 0x00)) return false;
            if (!protocol->readByte(base, &data) || data != 0x00) return false;

            if (!protocol->writeByte(base + sector_size - 1, 0x55)) return false;
            if (!protocol->readByte(base + sector_size - 1, &data) || data != 0x55) return false;

            if (s % 16 == 0)
                printf("sector %lu/%lu OK\r\n", s, num_sectors);
        }
    } else {
        // EEPROM/SRAM: byte-by-byte 0xFF ↔ 0x00
        for (uint32_t i = 0; i < protocol->mem_size; i++) {
            uint8_t data;
            if (!protocol->writeByte(i, 0xFF)) return false;
            if (!protocol->readByte(i, &data) || data != 0xFF) return false;
            if (!protocol->writeByte(i, 0x00)) return false;
            if (!protocol->readByte(i, &data) || data != 0x00) return false;
            if (i % 512 == 0) printf("%#08x PASSED\r\n", i);
        }
    }
    // If we reached end, we good.
    return true;
}
