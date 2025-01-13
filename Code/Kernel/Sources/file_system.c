#include "../Includes/file_system.h"

SuperBlock SB;
bool_t dir[MAX_DIR - 1] =  {false};

void init_fs() {
    uint8_t buffer[SECTOR_SIZE] = {0};
    uint8_t dir_buffer[MAX_DIR * 16] = {0};

    // Read the SuperBlock from disk at the filesystem start
    ata_identify(ATA_PRIMARY_IO, ATA_MASTER);
    ata_read(ATA_PRIMARY_IO, ATA_MASTER, START_FS, 1, buffer);
    memcpy(&SB, buffer, sizeof(SuperBlock));

    // Read the Dir Entries from disk at the filesystem start
    ata_read(ATA_PRIMARY_IO, ATA_MASTER, START_DIR, MAX_DIR * 16 / 512, dir_buffer);
    
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

void create_file(char* filename) {
    if (strlen(filename) > 10) {
        printf("Error: Filename too long. Maximum length is 10 characters.\n",RED_ON_BLACK_WARNING);
        return;
    }

    if(!updateSB()) {
        printf("Error: updateSB.\n", RED_ON_BLACK_WARNING);
        return;
    }

    if(!updateDir(filename)) {
        printf("Error: updateDir.\n", RED_ON_BLACK_WARNING);
        return;
    }
}

bool_t updateSB() {
    uint32_t sectors = 1;
    if(!isAvaDir()) {
        printf("No Available Dir", YELLOW_ON_BLACK_CAUTION);
        return false;
    }

    if(!isAvaSec(sectors)) {
        printf("No Available Sec", YELLOW_ON_BLACK_CAUTION);
        return false;
    }
    updateDirAndSec(sectors);
    return true;
}

bool_t updateDir(char* filename) {
    DirEntry entry = {0}; // Zero-initialize the struct
    strncpy((char *)entry.name, filename, sizeof(entry.name) - 1);
    entry.name[sizeof(entry.name) - 1] = '\0';
    entry.size = 0;

    int pos = -1;

    // Find the first available directory slot
    for (int i = 0; i < MAX_DIR; i++) {
        if (dir[i] == 0) { // Check if the directory slot is free
            dir[i] = 1;    // Mark it as used
            entry.fat_entry = i; // Set the FAT entry to the index
            pos = i;       // Record the position
            break;
        }
    }

    if (pos == -1) {
        printf("Error: No free directory entry found\n", YELLOW_ON_BLACK_CAUTION);
        return false;
    }

    // Read the current directory entries from the disk
    
    uint8_t dir_buffer[MAX_DIR * 16] = {0};
    
    ata_identify(ATA_PRIMARY_IO, ATA_MASTER);
    ata_read(ATA_PRIMARY_IO, ATA_MASTER, START_DIR, MAX_DIR * 16 / 512, dir_buffer);
    
    // Copy the DirEntry struct into the directory buffer at the correct position
    memcpy(&dir_buffer[pos * 16], &entry, sizeof(DirEntry));

    // Write the updated directory entries back to the disk
    ata_identify(ATA_PRIMARY_IO, ATA_MASTER);
    ata_write(ATA_PRIMARY_IO, ATA_MASTER, START_DIR, MAX_DIR * 16 / 512, dir_buffer);

    return true;
}

bool_t isAvaDir() {
    return SB.available_direntries > 0;
}

bool_t isAvaSec(uint32_t sectors) {
    return SB.available_sectors >= sectors;
}

void updateDirAndSec(uint32_t sectors) {
    SB.available_direntries--;
    SB.available_sectors -= sectors * SB.sectors_per_cluster;

    uint8_t buffer[SECTOR_SIZE] = {0};
    memcpy(buffer, &SB, sizeof(SuperBlock));
    
    ata_identify(ATA_PRIMARY_IO, ATA_MASTER);
    ata_write(ATA_PRIMARY_IO, ATA_MASTER, START_FS, 1, buffer);
}