#pragma once

#include <stdint.h>

// returns 0 - fail, 1 - success
bool __attribute__((cdecl)) disk_reset(uint8_t drive);

// returns 0 - fail, 1 - success
bool __attribute__((cdecl)) disk_read(uint8_t drive, uint8_t sector,
                                      uint8_t sector_count, uint8_t cylinder,
                                      uint8_t head, uint16_t linear_segment,
                                      uint16_t linear_offset);
// returns 0 - fail, 1 - success
bool __attribute__((cdecl))
get_disk_params(uint8_t drive, uint8_t *drive_type_out, uint16_t *cylinders_out,
                uint16_t *sectors_out, uint16_t *heads_out);
