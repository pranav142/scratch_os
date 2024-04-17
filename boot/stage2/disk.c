#include "disk.h"

void linear_to_segment_offset(uint32_t linear_address, uint16_t *segment_out,
                              uint16_t *offset_out) {
  *segment_out = linear_address >> 4;
  *offset_out = linear_address & 0x0F;
}

void lba_to_chs(uint32_t lba, uint8_t *cylinder_out, uint8_t *head_out,
                uint8_t *sector_out, uint8_t heads_per_cylinder,
                uint8_t sectors_per_track) {
  *cylinder_out = lba / (heads_per_cylinder * sectors_per_track);
  *head_out = (lba / sectors_per_track) % heads_per_cylinder;
  *sector_out = (lba % sectors_per_track) + 1;
}

bool disk_read(uint8_t drive, uint32_t lba, uint16_t num_sectors,
               uint32_t storage_address, uint8_t heads_per_cylinder,
               uint8_t sectors_per_track) {
  const int NUM_TRIES = 3;
  uint8_t cylinder, head, sector;
  uint16_t segment, offset;
  bool error;

  linear_to_segment_offset(storage_address, &segment, &offset);
  lba_to_chs(lba, &cylinder, &head, &sector, heads_per_cylinder,
             sectors_per_track);

  for (int i = 0; i < NUM_TRIES; i++) {
    error = x86_disk_read(drive, sector, num_sectors, cylinder, head, segment,
                          offset);
    if (!error) {
      return SUCCESSFUL;
    }
    x86_disk_reset(drive);
  }

  return FAILED;
}
