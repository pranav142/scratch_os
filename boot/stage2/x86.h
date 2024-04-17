#pragma once

#include <stdbool.h>
#include <stdint.h>

// returns 1 - fail, 0 - success

bool __attribute__((cdecl)) x86_disk_reset(uint8_t drive);

bool __attribute__((cdecl)) x86_disk_read(uint8_t drive, uint8_t sector,
                                          uint8_t sector_count,
                                          uint8_t cylinder, uint8_t head,
                                          uint16_t segment, uint16_t offset);
bool __attribute__((cdecl)) x86_get_disk_params(uint8_t drive,
                                                uint8_t *drive_type_out,
                                                uint16_t *cylinders_out,
                                                uint16_t *sectors_out,
                                                uint16_t *heads_out);
