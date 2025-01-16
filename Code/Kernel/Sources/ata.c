#include "../Includes/ata.h"

void ata_initialize(uint16_t io_base, uint8_t drive) {
    outPort(io_base + 6, drive);
    outPort(io_base + 2, 0);
    outPort(io_base + 3, 0);
    outPort(io_base + 4, 0);
    outPort(io_base + 5, 0);
    outPort(io_base + 7, 0xEC);

    uint8_t status = inPort(io_base + 7);

    // Wait for BSY to clear
    while ((status & 0x80) != 0) {
        status = inPort(io_base + 7);
    }
}

static void ata_wait_bsy(uint16_t io_base) {
    while (inPort(io_base + 7) & 0x80); // Wait for BSY to clear
}

static void ata_wait_drq(uint16_t io_base) {
    while (!(inPort(io_base + 7) & 0x08)); // Wait for DRQ to set
}

void ata_read(uint16_t io_base, uint8_t drive, uint32_t lba, uint8_t sectors, uint8_t* buffer) {
    if (!buffer || sectors == 0) {
        return;
    }

    if (sectors > 256) {
        return;
    }

    // Select drive and LBA
    outPort(io_base + 6, 0xE0 | (drive << 4) | ((lba >> 24) & 0x0F));
    outPort(io_base + 2, sectors);
    outPort(io_base + 3, (uint8_t)(lba));
    outPort(io_base + 4, (uint8_t)(lba >> 8));
    outPort(io_base + 5, (uint8_t)(lba >> 16));
    outPort(io_base + 7, 0x20); // Read command

    for (uint8_t i = 0; i < sectors; i++) {
        ata_wait_bsy(io_base);
        if (inPort(io_base + 7) & 0x01) { // Check for error
            printf("ATA read error on sector %u.\n",RED_ON_BLACK_WARNING, lba + i);
            return;
        }

        ata_wait_drq(io_base);

        for (uint16_t j = 0; j < ATA_SECTOR_SIZE / 2; j++) {
            ((uint16_t*)buffer)[j] = inPort16(io_base);
        }
        buffer += ATA_SECTOR_SIZE;
    }
}

void ata_write(uint16_t io_base, uint8_t drive, uint32_t lba, uint8_t sectors, const uint8_t* buffer) {
    outPort(io_base + 6, 0xE0 | (drive << 4) | ((lba >> 24) & 0x0F));
    outPort(io_base + 2, sectors);
    outPort(io_base + 3, (uint8_t)(lba));
    outPort(io_base + 4, (uint8_t)(lba >> 8));
    outPort(io_base + 5, (uint8_t)(lba >> 16));
    outPort(io_base + 7, 0x30);

    for (uint8_t i = 0; i < sectors; i++) {
        ata_wait_bsy(io_base);
        ata_wait_drq(io_base);

        for (uint16_t j = 0; j < ATA_SECTOR_SIZE / 2; j++) {
            outPort16(io_base, ((uint16_t*)buffer)[j]);
        }
        buffer += ATA_SECTOR_SIZE;
    }

    ata_wait_bsy(io_base);
    outPort(io_base + 7, 0xE7);
    ata_wait_bsy(io_base);
}

void ata_identify(uint16_t io_base, uint8_t drive) {
    outPort(io_base + 6, drive);
    outPort(io_base + 2, 0);
    outPort(io_base + 3, 0);
    outPort(io_base + 4, 0);
    outPort(io_base + 5, 0);
    outPort(io_base + 7, 0xEC);

    ata_wait_bsy(io_base);

    uint8_t status = inPort(io_base + 7);
    if (status == 0) {
        printf("No ATA device detected.\n", COLOR_BLACK_ON_WHITE);
        return;
    }

    uint16_t buffer[256];
    for (uint16_t i = 0; i < 256; i++) {
        buffer[i] = inPort16(io_base);
    }
}