#include "../Includes/ata.h"

void ata_initialize(uint16_t io_base, uint8_t drive) {
    outPort(io_base + 6, drive);
    outPort(io_base + 2, 0);
    outPort(io_base + 3, 0);
    outPort(io_base + 4, 0);
    outPort(io_base + 5, 0);
    outPort(io_base + 7, 0xEC);

    uint8_t status = inPort(io_base + 7);
    if (status == 0) {
        printf("No ATA device detected.\n", COLOR_BLACK_ON_WHITE);
        return;
    }

    while ((status & 0x80) != 0) {
        status = inPort(io_base + 7);
    }
}

void ata_read(uint16_t io_base, uint8_t drive, uint32_t lba, uint8_t sectors, uint8_t* buffer) {
    outPort(io_base + 6, drive | ((lba >> 24) & 0x0F));
    outPort(io_base + 2, sectors);
    outPort(io_base + 3, (uint8_t)(lba));
    outPort(io_base + 4, (uint8_t)(lba >> 8));
    outPort(io_base + 5, (uint8_t)(lba >> 16));
    outPort(io_base + 7, 0x20);

    for (uint8_t i = 0; i < sectors; i++) {
        int timeout = 100000;
        uint8_t status = inPort(io_base + 7);
        while (!(status & 0x08) && --timeout) {
            status = inPort(io_base + 7);
        }

        if (timeout == 0) {
            printf("Read timeout! Status: 0x%x\n", COLOR_BLACK_ON_WHITE, status);
            return;
        }

        for (uint16_t j = 0; j < ATA_SECTOR_SIZE / 2; j++) {
            ((uint16_t*)buffer)[j] = inPort(io_base);
        }
        buffer += ATA_SECTOR_SIZE;
    }

    printf("ATA read completed successfully.\n", COLOR_BLACK_ON_WHITE);
}

void ata_write(uint16_t io_base, uint8_t drive, uint32_t lba, uint8_t sectors, const uint8_t* buffer) {
    outPort(io_base + 6, drive | ((lba >> 24) & 0x0F));
    outPort(io_base + 2, sectors);
    outPort(io_base + 3, (uint8_t)(lba));
    outPort(io_base + 4, (uint8_t)(lba >> 8));
    outPort(io_base + 5, (uint8_t)(lba >> 16));
    outPort(io_base + 7, 0x30);

    for (uint8_t i = 0; i < sectors; i++) {
        int timeout = 100000;
        uint8_t status = inPort(io_base + 7);
        while (!(status & 0x08) && --timeout) {
            status = inPort(io_base + 7);
        }

        if (timeout == 0) {
            printf("Write timeout! Status: 0x%x\n", COLOR_BLACK_ON_WHITE, status);
            return;
        }

        for (uint16_t j = 0; j < ATA_SECTOR_SIZE / 2; j++) {
            outPort(io_base, ((uint16_t*)buffer)[j]);
        }
        buffer += ATA_SECTOR_SIZE;
    }

    uint8_t final_status = inPort(io_base + 7);
    if (final_status & 0x01) {
        printf("ATA write failed. Final status: 0x%x\n", COLOR_BLACK_ON_WHITE, final_status);
    } else {
        printf("ATA write completed successfully.\n", COLOR_BLACK_ON_WHITE);
    }
}

void ata_identify(uint16_t io_base, uint8_t drive) {
    outPort(io_base + 6, drive);
    outPort(io_base + 2, 0);
    outPort(io_base + 3, 0);
    outPort(io_base + 4, 0);
    outPort(io_base + 5, 0);
    outPort(io_base + 7, 0xEC);

    uint8_t status = inPort(io_base + 7);
    int timeout = 100000;
    while ((status & 0x80) && --timeout) {
        status = inPort(io_base + 7);
    }

    if (timeout == 0 || status == 0) {
        printf("No ATA device detected.\n", COLOR_BLACK_ON_WHITE);
        return;
    }

    printf("ATA device detected.\n", COLOR_BLACK_ON_WHITE);
}