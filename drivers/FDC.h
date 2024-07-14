#pragma once
#include <stdint.h>

typedef enum {
  READ_TRACK = 2,
  SPECIFY = 3,
  SENSE_DRIVE_STATUS = 4,
  WRITE_DATA = 5,
  READ_DATA = 6,
  RECALIBRATE = 7,
  SENSE_INTERRUPT = 8,
  WRITE_DELETED_DATA = 9,
  READ_ID = 10,
  READ_DELETED_DATA = 12,
  FORMAT_TRACK = 13,
  DUMPREG = 14,
  SEEK = 15,
  VERSION = 16,
  SCAN_EQUAL = 17,
  PERPENDICULAR_MODE = 18,
  CONFIGURE = 19,
  LOCK = 20,
  VERIFY = 22,
  SCAN_LOW_OR_EQUAL = 25,
  SCAN_HIGH_OR_EQUAL = 29
} FloppyCommands;

typedef enum {
  FDC_CCR_500kbps = 0x00,
} CCRDataRates;

void send_fdc_command(FloppyCommands command);
uint8_t read_fdc_data();
uint8_t get_fdc_version();
void fdc_test();
