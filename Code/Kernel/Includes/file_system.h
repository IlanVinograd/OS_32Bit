#ifndef _FILE_SYSTEM_
#define _FILE_SYSTEM_

#include <stdint.h>
#include <ata.h>
#include <string.h>

#define OFFSET_BEGIN 0x10000
#define SECTOR 512
#define START_FS (OFFSET_BEGIN / SECTOR) // Here the file system start, it's like offset.

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
void creat_file();

#endif /* _FILE_SYSTEM_ */