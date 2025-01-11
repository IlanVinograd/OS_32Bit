#include "../Includes/file_system.h"

SuperBlock SP;

void init_fs() {
    printf("\nbegin",RED_ON_BLACK_WARNING);
    SuperBlock SP;
    SP.total_sectors = 8192;
    SP.sectors_per_cluster = 8;
    SP.bytes_per_sector = 512;
    SP.available_sectors = 8192;
    SP.total_direntries = 256;
    SP.available_direntries = 256;
    SP.fs_type = 0xFA;
    memset(SP.reserved, 0, sizeof(SP.reserved)); // Reserved bytes set to 0
    strncpy(SP.label, (const uint8_t *)"SFAT", sizeof(SP.label));
    
    // Convert the SuperBlock structure to a buffer for writing
    uint8_t buffer[512] = {0};
    memcpy(buffer, &SP, sizeof(SuperBlock));

    // Write the SuperBlock to disk at the filesystem start
    ata_identify(ATA_PRIMARY_IO, ATA_MASTER);
    ata_write(ATA_PRIMARY_IO, ATA_MASTER, START_FS, 1, buffer);
    printf("\nFinished",RED_ON_BLACK_WARNING);
}

void creat_file() {

}
