#ifndef _FILE_SYSTEM_
#define _FILE_SYSTEM_

#include <stdint.h>
#include <ata.h>
#include <string.h>

#define OFFSET_BEGIN 0x10000

#define START_FS (OFFSET_BEGIN / SECTOR_SIZE) // Here the file system start, it's like offset.
#define START_DIR ((OFFSET_BEGIN + 0x00200) / SECTOR_SIZE) // Here the dir entries system start, it's like offset.
#define START_FAT ((OFFSET_BEGIN + 0x01200) / 512) // Here the FAT start, it's like offset.

#define SECTOR_SIZE 512
#define MAX_SECTORS 8192
#define MAX_DIR 256
#define FAT ((MAX_SECTORS / 8) * 2) // the is 8 sector per cluster, and * 2 cause we use one byte and not 2 bytes so we * by 2.

typedef struct SuperBlock // 32 byte
{
  unsigned short total_sectors;        // Total number of sectors
  unsigned short sectors_per_cluster;  // Number of sectors in each block
  unsigned short bytes_per_sector;     // Number of bytes in each sector
  unsigned short available_sectors;    // Number of sectors available
  unsigned short total_direntries;     // Total number of directory entries
  unsigned short available_direntries; // Number of available dir entries
  unsigned char fs_type;               // File system type (FA for SFAT)
  unsigned char reserved[11];          // Reserved, all set to 0
  unsigned char label[8];              // Not guaranteed to be NUL-terminated
} SuperBlock;

typedef struct DirEntry // 16 byte
{
  unsigned char name[10];   // Not guaranteed to be NUL-terminated
  unsigned short fat_entry; // First data block
  unsigned int size;        // Size of the file in bytes
} DirEntry;

void init_fs();
void create_file(char* filename);

bool_t updateSB();
bool_t updateDir(char* filename);
bool_t updateFat();

bool_t isAvaDir();
bool_t isAvaSec(uint32_t sectors);
void updateDirAndSec(uint32_t sectors);


#endif /* _FILE_SYSTEM_ */