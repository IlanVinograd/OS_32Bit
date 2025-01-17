#include "../Includes/file_system.h"

SuperBlock SB;
bool_t dir[MAX_DIR - 1] =  {false};
int posFat;

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
    printf("Available dir's: %d\n",RED_ON_BLACK_WARNING, SB.available_direntries);
    printf("Available sectors: %d\n",RED_ON_BLACK_WARNING, SB.available_sectors);
    printf("Filesystem label: %s\n", RED_ON_BLACK_WARNING, (const char *)SB.label);

    printf("Dir Status: ", RED_ON_BLACK_WARNING);
    for(int i = 0; i < MAX_DIR; i++){
        printf("%x", RED_ON_BLACK_WARNING, dir[i]);
    }
}

void create_file(char* filename) {
    if (strlen(filename) > 10) {
        printf("Error: Filename too long. Maximum length is 10 characters.\n", RED_ON_BLACK_WARNING);
        nextLine();
        return;
    }

    if (isCreated(filename)) {
        printf("Error: File %s is already created. Choose a different name.\n", RED_ON_BLACK_WARNING, filename);
        nextLine();
        return;
    }

    if (!updateSB()) {
        printf("Error: Failed to update SuperBlock.\n", RED_ON_BLACK_WARNING);
        nextLine();
        return;
    }

    if (!updateDir(filename)) {
        printf("Error: Failed to update directory.\n", RED_ON_BLACK_WARNING);
        nextLine();
        return;
    }

    if (!updateFat()) {
        printf("Error: Failed to update FAT.\n", RED_ON_BLACK_WARNING);
        nextLine();
        return;
    }
}

void delete_file(char* filename) {
    /* We Already Check if filename is not strlen() != 0 */

    if(!isCreated(filename)) {
        printf("Error: File Not Found or Not Created.\n", RED_ON_BLACK_WARNING);
        nextLine();
        return;
    }

    if(!removeFat(filename)) {
        printf("Error: removeDir.\n", RED_ON_BLACK_WARNING);
        nextLine();
        return;
    }

    if(!removeDir(filename)) {
        printf("Error: removeDir.\n", RED_ON_BLACK_WARNING);
        nextLine();
        return;
    }
    printf("File: %s Removed.\n", RED_ON_BLACK_WARNING, filename);
    nextLine();
}

void write_to_file(char* data, char* filename) {
    if (strlen(filename) > 10) {
        printf("Error: Filename too long. Maximum length is 10 characters.\n", RED_ON_BLACK_WARNING);
        nextLine();
        return;
    }

    if (!isCreated(filename)) {
        printf("Error: File %s is not created. Choose a different File.\n", RED_ON_BLACK_WARNING, filename);
        nextLine();
        return;
    }

    if(!write_data(filename, data)) {
        printf("Error: Place data to file.\n", RED_ON_BLACK_WARNING);
        nextLine();
        return;
    }
}

void output_file(char* filename) {
    if(strlen(filename) > 10) {
        printf("Error: Filename too long. Maximum length is 10 characters.\n", RED_ON_BLACK_WARNING);
        nextLine();
        return;
    }

    if(!isCreated(filename)) {
        printf("Error: File: %s not Found.\n", RED_ON_BLACK_WARNING, filename);
        nextLine();
        return;
    }

    if(!extract_file(filename)) {
        printf("Error: Failed to update extract_file.\n", RED_ON_BLACK_WARNING);
        nextLine();
        return;
    }
}

void showAllFiles() {
    uint8_t dir_buffer[MAX_DIR * 16] = {0};
    
    ata_identify(ATA_PRIMARY_IO, ATA_MASTER);
    ata_read(ATA_PRIMARY_IO, ATA_MASTER, START_DIR, MAX_DIR * 16 / SECTOR_SIZE, dir_buffer);

    printf("  Name           Length\n", COLOR_BLACK_ON_WHITE);
    printf("  ----           ------\n", COLOR_BLACK_ON_WHITE);

    for (int i = 0; i < MAX_DIR; i++) {
        DirEntry* dir_entry = (DirEntry*)&dir_buffer[i * 16];

        if (dir_entry->name[0] != 0 && dir_entry->name[0] != '?') {
            printWithPads("  %-10s      (Size: %-4d bytes)\n",COLOR_BLACK_ON_WHITE, dir_entry->name, dir_entry->size);
        }
    }
}

bool_t isCreated(char* filename) {
    uint8_t dir_buffer[MAX_DIR * 16] = {0};

    ata_identify(ATA_PRIMARY_IO, ATA_MASTER);
    ata_read(ATA_PRIMARY_IO, ATA_MASTER, START_DIR, MAX_DIR * 16 / 512, dir_buffer);

    for (int i = 0; i < MAX_DIR; i++) {
        uint8_t* dir_entry_name = &dir_buffer[i * 16];
        if (strncmp((const uint8_t *)dir_entry_name, (const uint8_t *)filename, strlen(filename)) == 0) {
            return true;
        }
    }
    return false;
}

bool_t updateSB() {
    int32_t sectors = 1;
    if(!isAvaDir()) {
        printf("No Available Dir", YELLOW_ON_BLACK_CAUTION);
        nextLine();
        return false;
    }

    if(!isAvaSec(sectors)) {
        printf("No Available Sec", YELLOW_ON_BLACK_CAUTION);
        nextLine();
        return false;
    }
    updateDirAndSec(-sectors, -1);
    return true;
}

bool_t updateDir(char* filename) {
    DirEntry entry = {0}; // Zero-initialize the struct
    strncpy((uint8_t *)entry.name, (const uint8_t *)filename, sizeof(entry.name) - 1);
    entry.name[sizeof(entry.name) - 1] = '\0';
    entry.size = 0;

    posFat = -1;

    // Find the first available directory slot
    for (int i = 0; i < MAX_DIR; i++) {
        if (dir[i] == 0) { // Check if the directory slot is free
            dir[i] = 1;    // Mark it as used
            entry.fat_entry = i; // Set the FAT entry to the index
            posFat = i;       // Record the position
            break;
        }
    }

    if (posFat == -1) {
        printf("Error: No free directory entry found\n", YELLOW_ON_BLACK_CAUTION);
        return false;
    }

    // Read the current directory entries from the disk
    
    uint8_t dir_buffer[MAX_DIR * 16] = {0};
    
    ata_identify(ATA_PRIMARY_IO, ATA_MASTER);
    ata_read(ATA_PRIMARY_IO, ATA_MASTER, START_DIR, MAX_DIR * 16 / 512, dir_buffer);
    
    // Copy the DirEntry struct into the directory buffer at the correct position
    memcpy(&dir_buffer[posFat * 16], &entry, sizeof(DirEntry));

    // Write the updated directory entries back to the disk
    ata_identify(ATA_PRIMARY_IO, ATA_MASTER);
    ata_write(ATA_PRIMARY_IO, ATA_MASTER, START_DIR, MAX_DIR * 16 / 512, dir_buffer);

    return true;
}

bool_t updateFat() {
    uint8_t bufferFat[FAT] = {0};

    ata_identify(ATA_PRIMARY_IO, ATA_MASTER);
    ata_read(ATA_PRIMARY_IO, ATA_MASTER, START_FAT, FAT / SECTOR_SIZE, bufferFat);

    int clusters_per_file = SB.sectors_per_cluster;
    int cluster_index = -1;
    int prev_cluster = -1;

    // Find free clusters in the FAT
    for (int i = 0; i < MAX_SECTORS; i++) {
        if (bufferFat[i * 2] == 0x00 && bufferFat[i * 2 + 1] == 0x00) {
            if (cluster_index == -1) {
                cluster_index = i;
            }

            if (prev_cluster != -1) {
                bufferFat[prev_cluster * 2] = (i & 0xFF00) >> 8;
                bufferFat[prev_cluster * 2 + 1] = i & 0xFF;
            }

            prev_cluster = i;
            clusters_per_file--;

            if (clusters_per_file == 0) {
                bufferFat[i * 2] = 0xFF;
                bufferFat[i * 2 + 1] = 0xFF;
                break;
            }
        }
    }

    if (clusters_per_file > 0) {
        printf("Error: Not enough free clusters available\n", YELLOW_ON_BLACK_CAUTION);
        return false;
    }

    if (posFat != -1) {
        uint8_t dir_buffer[MAX_DIR * 16] = {0};
        ata_read(ATA_PRIMARY_IO, ATA_MASTER, START_DIR, MAX_DIR * 16 / 512, dir_buffer);

        DirEntry *entry = (DirEntry *)&dir_buffer[posFat * 16];
        entry->fat_entry = cluster_index;

        ata_write(ATA_PRIMARY_IO, ATA_MASTER, START_DIR, MAX_DIR * 16 / 512, dir_buffer);
    }

    ata_write(ATA_PRIMARY_IO, ATA_MASTER, START_FAT, FAT / SECTOR_SIZE, bufferFat);

    return true;
}

bool_t isAvaDir() {
    return SB.available_direntries > 0;
}

bool_t isAvaSec(int32_t sectors) {
    return SB.available_sectors >= sectors;
}

bool_t removeDir(char* filename) {
    uint8_t dir_buffer[MAX_DIR * 16] = {0};

    ata_identify(ATA_PRIMARY_IO, ATA_MASTER);
    ata_read(ATA_PRIMARY_IO, ATA_MASTER, START_DIR, MAX_DIR * 16 / 512, dir_buffer);

    for (int i = 0; i < MAX_DIR; i++) {
        DirEntry* entry = (DirEntry*)&dir_buffer[i * 16];
        if (strncmp((const char*)entry->name, filename, strlen(filename)) == 0) {
            memset(entry, 0, sizeof(DirEntry));
            dir[i] = false;

            ata_write(ATA_PRIMARY_IO, ATA_MASTER, START_DIR, MAX_DIR * 16 / 512, dir_buffer);

            updateDirAndSec(1, 1);

            return true;
        }
    }
    return false;
}

bool_t removeFat(char* filename) {
    uint8_t bufferFat[FAT] = {0};
    uint8_t dir_buffer[MAX_DIR * 16] = {0};

    ata_identify(ATA_PRIMARY_IO, ATA_MASTER);
    ata_read(ATA_PRIMARY_IO, ATA_MASTER, START_FAT, FAT / SECTOR_SIZE, bufferFat);
    ata_identify(ATA_PRIMARY_IO, ATA_MASTER);
    ata_read(ATA_PRIMARY_IO, ATA_MASTER, START_DIR, MAX_DIR * 16 / 512, dir_buffer);

    for (int i = 0; i < MAX_DIR; i++) {
        DirEntry* entry = (DirEntry*)&dir_buffer[i * 16];
        if (strncmp((const char*)entry->name, filename, strlen(filename)) == 0) {
            uint16_t cluster = entry->fat_entry;

            while (cluster != 0xFFFF) {
                uint16_t next_cluster = (bufferFat[cluster * 2] << 8) | bufferFat[cluster * 2 + 1];
                bufferFat[cluster * 2] = 0x00;
                bufferFat[cluster * 2 + 1] = 0x00;
                cluster = next_cluster;
            }

            ata_write(ATA_PRIMARY_IO, ATA_MASTER, START_FAT, FAT / SECTOR_SIZE, bufferFat);
            return true;
        }
    }
    return false;
}

bool_t write_data(char* filename, char* data) {
    DirEntry entry = {0};
    uint8_t dir_buffer[MAX_DIR * 16] = {0};

    ata_identify(ATA_PRIMARY_IO, ATA_MASTER);
    ata_read(ATA_PRIMARY_IO, ATA_MASTER, START_DIR, MAX_DIR * 16 / SECTOR_SIZE, dir_buffer);

    for (int i = 0; i < MAX_DIR; i++) {
        DirEntry* dir_entry = (DirEntry*)&dir_buffer[i * 16];
        if (strncmp((const uint8_t *)dir_entry->name, (const uint8_t *)filename, strlen(filename)) == 0) {
            memcpy(&entry, dir_entry, sizeof(DirEntry));
            break;
        }
    }

    // uint8_t bufferFat[FAT] = {0};
    // ata_identify(ATA_PRIMARY_IO, ATA_MASTER);                                      // NOT SURE IF NEEDED.
    // ata_read(ATA_PRIMARY_IO, ATA_MASTER, START_FAT, FAT / SECTOR_SIZE, bufferFat);

    // Get the starting cluster from the FAT entry
    uint16_t cluster = entry.fat_entry; // FAT entry already provides the starting cluster
    if (cluster == 0xFFFF) {
        printf("Error: Invalid FAT entry for the file.\n", RED_ON_BLACK_WARNING);
        return false;
    }

    uint32_t startSector = START_DATA + (cluster / 8 * SB.sectors_per_cluster);

    // Ensure the data fits into the allocated clusters
    size_t dataLength = strlen(data);
    size_t maxDataSize = SB.sectors_per_cluster * SECTOR_SIZE; // 8 sectors * 512 bytes
    if (dataLength > maxDataSize) {
        printf("Error: Data exceeds the size of the allocated cluster.\n", RED_ON_BLACK_WARNING);
        return false;
    }

    // Prepare the cluster buffer
    uint8_t clusterBuffer[maxDataSize];
    memset(clusterBuffer, 0, sizeof(clusterBuffer)); // abrogate all region to be empty.
    memcpy(clusterBuffer, data, dataLength);

    ata_write(ATA_PRIMARY_IO, ATA_MASTER, startSector, SB.sectors_per_cluster, clusterBuffer);

    // Update the file size in the directory entry
    entry.size = dataLength - 1;
    memcpy(&dir_buffer[(entry.fat_entry / 8 * 16)], &entry, sizeof(DirEntry));

    // Write the updated directory entry back to disk
    ata_identify(ATA_PRIMARY_IO, ATA_MASTER);
    ata_write(ATA_PRIMARY_IO, ATA_MASTER, START_DIR, MAX_DIR * 16 / SECTOR_SIZE, dir_buffer);

    return true;
}

bool_t extract_file(char* filename) {
    DirEntry entry = {0};
    uint8_t dir_buffer[MAX_DIR * 16] = {0};

    ata_identify(ATA_PRIMARY_IO, ATA_MASTER);
    ata_read(ATA_PRIMARY_IO, ATA_MASTER, START_DIR, MAX_DIR * 16 / SECTOR_SIZE, dir_buffer);

    for (int i = 0; i < MAX_DIR; i++) {
        DirEntry* dir_entry = (DirEntry*)&dir_buffer[i * 16];
        if (strncmp((const uint8_t *)dir_entry->name, (const uint8_t *)filename, strlen(filename)) == 0) {
            memcpy(&entry, dir_entry, sizeof(DirEntry));
            break;
        }
    }

    uint16_t cluster = entry.fat_entry; // FAT entry already provides the starting cluster
    if (cluster == 0xFFFF) {
        printf("Error: Invalid FAT entry for the file.\n", RED_ON_BLACK_WARNING);
        return false;
    }

    uint32_t startSector = START_DATA + (cluster / 8 * SB.sectors_per_cluster);

    uint8_t* data = (uint8_t*)malloc(SECTOR_SIZE * SB.sectors_per_cluster);
    if (!data) {
        printf("Error: Memory allocation failed.\n", RED_ON_BLACK_WARNING);
        return false;
    }
    memset(data, 0, SECTOR_SIZE * SB.sectors_per_cluster);
    ata_identify(ATA_PRIMARY_IO, ATA_MASTER);
    ata_read(ATA_PRIMARY_IO, ATA_MASTER, startSector, SB.sectors_per_cluster, data);

    printf("%s", COLOR_BLACK_ON_WHITE, data);
    free(data);
    nextLine();
    return true;
}

void updateDirAndSec(int32_t sectors, int32_t dir) { // Could be - or + 
    SB.available_direntries += dir;
    SB.available_sectors += sectors * SB.sectors_per_cluster;

    uint8_t buffer[SECTOR_SIZE] = {0};
    memcpy(buffer, &SB, sizeof(SuperBlock));
    
    ata_identify(ATA_PRIMARY_IO, ATA_MASTER);
    ata_write(ATA_PRIMARY_IO, ATA_MASTER, START_FS, 1, buffer);
}

void nextLine() {
    uint16_t row = keyboard_cursor_position / VGA_COLS;
    row++;
    if (row >= VGA_ROWS) {
        scroll_screen();
        row = VGA_ROWS - 1;
    }
    keyboard_cursor_position = row * VGA_COLS;
    setCursorPosition(row, 0);
}