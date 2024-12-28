#ifndef ATA_H
#define ATA_H

#include <stdint.h>
#include <io.h>
#include <string.h>
#include <vga.h>

#define ATA_PRIMARY_IO 0x1F0
#define ATA_SECONDARY_IO 0x170
#define ATA_MASTER 0xA0
#define ATA_SLAVE  0xB0
#define ATA_SECTOR_SIZE 512

void ata_identify(uint16_t io_base, uint8_t drive);
void ata_initialize(uint16_t io_base, uint8_t drive);
void ata_read(uint16_t io_base, uint8_t drive, uint32_t lba, uint8_t sectors, uint8_t* buffer);
void ata_write(uint16_t io_base, uint8_t drive, uint32_t lba, uint8_t sectors, const uint8_t* buffer);

#endif // ATA_H