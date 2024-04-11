#pragma once

#include <stdint.h>

// returns 0 - fail, 1 - success
bool __attribute__((cdecl)) x86_disk_reset(uint8_t drive);

// returns 0 - fail, 1 - success
bool __attribute__((cdecl)) x86_disk_read(uint8_t drive, uint8_t sector,
                                      uint8_t sector_count, uint8_t cylinder,
                                      uint8_t head, uint16_t segment,
                                      uint16_t offset);
// returns 0 - fail, 1 - success
bool __attribute__((cdecl))
get_disk_params(uint8_t drive, uint8_t *drive_type_out, uint16_t *cylinders_out,
                uint16_t *sectors_out, uint16_t *heads_out);

bool __attribute__((cdecl)) x86_get_advanced_disk_params(uint8_t drive, uint16_t segment, uint16_t offset);