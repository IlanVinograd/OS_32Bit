#include "../Includes/file_system.h"

SuperBlock SB;
unsigned short FAT[MAX_FAT_ENTRIES] = {0};

void init_fs() {
    uint8_t buffer[512] = {0};

    // Read the SuperBlock from disk at the filesystem start
    ata_identify(ATA_PRIMARY_IO, ATA_SLAVE);
    ata_read(ATA_PRIMARY_IO, ATA_SLAVE, START_FS, 1, buffer);

    // Populate the SuperBlock structure from the buffer
    memcpy(&SB, buffer, sizeof(SuperBlock));

    printf("Filesystem initialized with the following parameters:\n",RED_ON_BLACK_WARNING);
    printf("Total sectors: %d\n",RED_ON_BLACK_WARNING, SB.total_sectors);
    printf("Sectors per cluster: %d\n",RED_ON_BLACK_WARNING,  SB.sectors_per_cluster);
    printf("Bytes per sector: %d\n",RED_ON_BLACK_WARNING, SB.bytes_per_sector);
    printf("Available sectors: %d\n",RED_ON_BLACK_WARNING, SB.available_sectors);
    printf("Filesystem label: %s\n", RED_ON_BLACK_WARNING, (const char *)SB.label);
}

void create_file(const char* filename) {
    if (strlen(filename) > 10) {
        printf("Error: Filename too long. Maximum length is 10 characters.\n",RED_ON_BLACK_WARNING);
        return;
    }
}