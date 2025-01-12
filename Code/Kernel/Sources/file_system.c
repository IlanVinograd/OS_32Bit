#include "../Includes/file_system.h"

SuperBlock SB;
bool_t dir[MAX_DIR - 1] =  {false};

void init_fs() {
    uint8_t buffer[SECTOR_SIZE] = {0};
    uint8_t dir_buffer[MAX_DIR * 16] = {0};

    // Read the SuperBlock from disk at the filesystem start
    ata_identify(ATA_PRIMARY_IO, ATA_SLAVE);
    ata_read(ATA_PRIMARY_IO, ATA_SLAVE, START_FS, 1, buffer);
    memcpy(&SB, buffer, sizeof(SuperBlock));

    // Read the Dir Entries from disk at the filesystem start
    ata_read(ATA_PRIMARY_IO, ATA_SLAVE, START_DIR, MAX_DIR * 16 / 512, dir_buffer);
    
    for(int i = 0; i < MAX_DIR; i++) { // each dir is 16 byte
        dir[i] = (dir_buffer[i * 16] == 0 || dir_buffer[i * 16] == '?') ? 0 : 1; // read this byte if ? or 0 means dir free other wise is not.
    }

    printf("Filesystem initialized with the following parameters:\n",RED_ON_BLACK_WARNING);
    printf("Total sectors: %d\n",RED_ON_BLACK_WARNING, SB.total_sectors);
    printf("Sectors per cluster: %d\n",RED_ON_BLACK_WARNING,  SB.sectors_per_cluster);
    printf("Bytes per sector: %d\n",RED_ON_BLACK_WARNING, SB.bytes_per_sector);
    printf("Available sectors: %d\n",RED_ON_BLACK_WARNING, SB.available_sectors);
    printf("Filesystem label: %s\n", RED_ON_BLACK_WARNING, (const char *)SB.label);

    printf("Dir Status: ", RED_ON_BLACK_WARNING);
    for(int i = 0; i < MAX_DIR; i++){
        printf("%x", RED_ON_BLACK_WARNING, dir[i]);
    }
}

void create_file(const char* filename) {
    if (strlen(filename) > 10) {
        printf("Error: Filename too long. Maximum length is 10 characters.\n",RED_ON_BLACK_WARNING);
        return;
    }
}